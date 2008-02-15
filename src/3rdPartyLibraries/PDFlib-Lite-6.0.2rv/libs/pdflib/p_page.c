/*---------------------------------------------------------------------------*
 |              PDFlib - A library for generating PDF on the fly             |
 +---------------------------------------------------------------------------+
 | Copyright (c) 1997-2005 Thomas Merz and PDFlib GmbH. All rights reserved. |
 +---------------------------------------------------------------------------+
 |                                                                           |
 |    This software is subject to the PDFlib license. It is NOT in the       |
 |    public domain. Extended versions and commercial licenses are           |
 |    available, please check http://www.pdflib.com.                         |
 |                                                                           |
 *---------------------------------------------------------------------------*/

/* $Id: p_page.c,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * PDFlib page related routines
 *
 */

#define P_PAGE_C

#include "p_intern.h"
#include "p_color.h"
#include "p_font.h"
#include "p_image.h"
#include "p_layer.h"
#include "p_page.h"
#include "p_tagged.h"


typedef enum
{
    trans_none,
    trans_split,
    trans_blinds,
    trans_box,
    trans_wipe,
    trans_dissolve,
    trans_glitter,
    trans_replace,

    TRANS_1_5,
    trans_fly = TRANS_1_5,
    trans_push,
    trans_cover,
    trans_uncover,
    trans_fade
}
pdf_transition;

static const pdc_keyconn pdf_transition_keylist[] =
{
    {"none",      trans_none},
    {"split",     trans_split},
    {"blinds",    trans_blinds},
    {"box",       trans_box},
    {"wipe",      trans_wipe},
    {"dissolve",  trans_dissolve},
    {"glitter",   trans_glitter},
    {"replace",   trans_replace},
    {"fly",       trans_fly},
    {"push",      trans_push},
    {"cover",     trans_cover},
    {"uncover",   trans_uncover},
    {"fade",      trans_fade},
    {NULL, 0}
};

static const pdc_keyconn pdf_transition_pdfkeylist[] =
{
    {"R",         trans_none},
    {"Split",     trans_split},
    {"Blinds",    trans_blinds},
    {"Box",       trans_box},
    {"Wipe",      trans_wipe},
    {"Dissolve",  trans_dissolve},
    {"Glitter",   trans_glitter},
    {"R",         trans_replace},
    {"Fly",       trans_fly},
    {"Push",      trans_push},
    {"Cover",     trans_cover},
    {"Uncover",   trans_uncover},
    {"Fade",      trans_fade},
    {NULL, 0}
};


typedef struct
{
    pdf_labelstyle	style;		/* page label style		*/
    char *		prefix;		/* page label prefix		*/
    int			start;		/* page label numbering start	*/
					/* 0 means "no label"		*/
} pg_label;

typedef struct
{
    char *		name;		/* group name			*/
    int			n_pages;	/* # of pages in this group	*/
    int			capacity;	/* # of pages reserved		*/
    int			start;		/* 1-based physical page number	*/
    pg_label		label;
} pg_group;

/* per page resource list
*/
struct pdf_reslist_s
{
    int *		list;		/* resource numbers		*/
    int			capacity;
    int			length;
};

/* current, or suspended page.
*/
typedef struct
{
    pdf_ppt		ppt;

    /* list of content stream IDs.
    */
    pdc_id *		contents_ids;
    int			contents_ids_capacity;
    int			next_content;

    pdf_annot *         annots;         /* annotation chain             */

    /* local values of global parameters.
    */
    pdc_scalar		ydir;		/* p->ydirection		*/

    /* resource lists.
    */
    pdf_reslist		rl_colorspaces;
    pdf_reslist		rl_extgstates;
    pdf_reslist		rl_fonts;
    pdf_reslist		rl_layers;
    pdf_reslist		rl_patterns;
    pdf_reslist		rl_shadings;
    pdf_reslist		rl_xobjects;
} pdf_page;


/* PDF page object.
*/
typedef struct
{
    pg_label		label;
    pdc_id		id;		/* object id for this page */
    pdf_page *		pg;		/* NULL if this page is not suspended */

    /* object ids (PDC_BAD_ID if not present).
    */
    pdc_id		annots_id;	/* id of page's /Annots entry */
    pdc_id		contents_id;	/* id of page's /Contents entry */
    pdc_id		res_id;		/* id of page's /Resources entry */
    pdc_id              thumb_id;       /* id of page's /Thumb entry */

    int			rotate;		/* page's /Rotate entry */
    int			transition;	/* page transition type, or -1 */
    double		duration;	/* page display duration, or -1 */
    char *		action;		/* "action" option string */

    pdc_id		act_idlist[PDF_MAX_EVENTS];	/* action object ids */
    pdc_rectangle	boxes[PDF_N_PAGE_BOXES];	/* MediaBox etc. */
} page_obj;


struct pdf_pages_s
{
    pdf_page   *curr_pg;
    pdc_bool	have_labels;
    pdc_bool	have_groups;
    pdc_bool	in_csect;		/* currently in contents section */
    int		last_suspended;		/* 1-based page number or -1 */
    pdf_ppt	default_ppt;		/* pseudo-ppt (for document scope) */

    /* as long as we support the old global parameters in addition to
    ** the new options, we have to save their values on entry to
    ** begin/resume_page(), and restore them during end/suspend_page().
    */
    pdc_scalar	old_ydir;		/* p->ydirection		*/

    /* deprecated parameters.
    */
    int         transition;             /* page transition type */
    double      duration;               /* page display duration */

    /* page descriptors in physical page order.
    */
    page_obj *	pages;                  /* page ids and suspended page descr */
    int         pages_capacity;
    int         current_page;           /* current page number (1-based) */
    int         last_page;              /* last page number allocated yet */
    int         max_page;           /* highest page number pre-allocated yet */

    /* page groups.
    */
    pg_group *	groups;
    int		groups_capacity;
    int		n_groups;

    pdc_id     *pnodes;                 /* page tree node ids */
    int         pnodes_capacity;        /* current # of entries in pnodes */
    int         current_pnode;          /* current node number (0-based) */
    int         current_pnode_kids;     /* current # of kids in current node */
};


/*********************** initialization & cleanup ***********************/

static void
pdf_init_ppt(PDF *p, pdc_bool new_ppt)
{
    if (new_ppt)
    {
	p->curr_ppt->cstate = 0;
	p->curr_ppt->tstate = 0;
    }

    p->curr_ppt->sl = 0;
    pdf_init_tstate(p);
    pdf_init_gstate(p);
    pdf_init_cstate(p);
} /* pdf_init_ppt */


static void
pdf_delete_page(PDF *p, pdf_page *pg)
{
    if (pg != 0)
    {
	pdf_cleanup_page_cstate(p, &pg->ppt);
	pdf_cleanup_page_tstate(p, &pg->ppt);

	if (pg->contents_ids)
	    pdc_free(p->pdc, pg->contents_ids);

	if (pg->annots)
	    pdf_cleanup_page_annots(p, pg->annots);

	if (pg->rl_colorspaces.list)
	    pdc_free(p->pdc, pg->rl_colorspaces.list);
	if (pg->rl_extgstates.list)
	    pdc_free(p->pdc, pg->rl_extgstates.list);
	if (pg->rl_fonts.list)
	    pdc_free(p->pdc, pg->rl_fonts.list);
	if (pg->rl_layers.list)
	    pdc_free(p->pdc, pg->rl_layers.list);
	if (pg->rl_patterns.list)
	    pdc_free(p->pdc, pg->rl_patterns.list);
	if (pg->rl_shadings.list)
	    pdc_free(p->pdc, pg->rl_shadings.list);
	if (pg->rl_xobjects.list)
	    pdc_free(p->pdc, pg->rl_xobjects.list);

	pdc_free(p->pdc, pg);
    }
} /* pdf_delete_page */


static void
pdf_init_page_obj(page_obj *po)
{
    po->id = PDC_BAD_ID;
    po->pg = (pdf_page *) 0;
    po->label.start = 0;
    po->label.prefix = (char *) 0;

    po->annots_id = PDC_BAD_ID;
    po->contents_id = PDC_BAD_ID;
    po->res_id = PDC_BAD_ID;
    po->thumb_id = PDC_BAD_ID;
    po->transition = -1;
    po->duration = -1;
    po->action = (char *) 0;

    po->rotate = 0;

    pdc_rect_init(&po->boxes[pdf_artbox], 0, 0, 0, 0);
    pdc_rect_init(&po->boxes[pdf_bleedbox], 0, 0, 0, 0);
    pdc_rect_init(&po->boxes[pdf_cropbox], 0, 0, 0, 0);
    pdc_rect_init(&po->boxes[pdf_mediabox], 0, 0, 0, 0);
    pdc_rect_init(&po->boxes[pdf_trimbox], 0, 0, 0, 0);
} /* pdf_init_page_obj */


static void
pdf_grow_pages(PDF *p)
{
    static const char fn[] = "pdf_grow_pages";

    pdf_pages *dp = p->doc_pages;
    int i;

    dp->pages = (page_obj *) pdc_realloc(p->pdc, dp->pages,
			    2 * sizeof (page_obj) * dp->pages_capacity, fn);

    for (i = dp->pages_capacity; i < dp->pages_capacity * 2; i++)
	pdf_init_page_obj(&dp->pages[i]);

    dp->pages_capacity *= 2;
} /* pdf_grow_pages */


void
pdf_init_pages(PDF *p, const char **groups, int n_groups)
{
    static const char fn[] = "pdf_init_pages";

    int		i, k;
    pdf_pages *	dp = (pdf_pages *) pdc_malloc(p->pdc, sizeof (pdf_pages), fn);

    p->doc_pages = dp;

    dp->have_labels = pdc_false;
    dp->have_groups = (n_groups != 0);
    dp->n_groups = 0;
    dp->last_suspended = 0;
    dp->in_csect = pdc_false;
    dp->transition = (int) trans_none;
    dp->duration = 0;

    dp->pages = (page_obj *) 0;
    dp->pnodes = (pdc_id *) 0;

    dp->pages_capacity = PAGES_CHUNKSIZE;
    dp->pages = (page_obj *)
	    pdc_malloc(p->pdc, sizeof (page_obj) * dp->pages_capacity, fn);

    /* mark ids to allow for pre-allocation of page ids */
    for (i = 0; i < dp->pages_capacity; i++)
	pdf_init_page_obj(&dp->pages[i]);

    dp->current_page	= 0;
    dp->last_page	= 0;
    dp->max_page	= 0;
    dp->curr_pg		= (pdf_page *) 0;

    dp->pnodes_capacity = PNODES_CHUNKSIZE;
    dp->pnodes = (pdc_id *)
		pdc_malloc(p->pdc, sizeof (pdc_id) * dp->pnodes_capacity, fn);

    dp->current_pnode	= 0;
    dp->current_pnode_kids = 0;

    /* clients may set char/word spacing and horizontal scaling outside pages
    ** for PDF_stringwidth() calculations, and they may set a color for use
    ** in PDF_makespotcolor(). that's what default_ppt is good for.
    */
    p->curr_ppt = &dp->default_ppt;
    pdf_init_ppt(p, pdc_true);

    for (i = 0; i < n_groups - 1; ++i)
	for (k = i + 1; k < n_groups; ++k)
	    if (strcmp(groups[i], groups[k]) == 0)
	    {
		pdc_error(p->pdc, PDF_E_DOC_DUPLGROUP, groups[i], 0, 0, 0);
	    }

    dp->n_groups = n_groups;
    dp->groups = (pg_group *) (n_groups ?
	pdc_malloc(p->pdc, sizeof (pg_group) * n_groups, fn) : 0);

    for (i = 0; i < n_groups; ++i)
    {
	dp->groups[i].name = pdc_strdup(p->pdc, groups[i]);
	dp->groups[i].n_pages = 0;
	dp->groups[i].capacity = 0;
	dp->groups[i].start = 1;
	dp->groups[i].label.prefix = (char *) 0;
	dp->groups[i].label.start = 0;
    }
} /* pdf_init_pages */


void pdf_check_suspended_pages(PDF *p)
{
    int		i;
    pdf_pages *	dp = p->doc_pages;

    for (i = 0; i <= dp->last_page; ++i)
    {
	if (dp->pages[i].pg != (pdf_page *) 0)
	{
	    pdc_error(p->pdc, PDF_E_PAGE_SUSPENDED,
		pdc_errprintf(p->pdc, "%d", i), 0, 0, 0);
	}
    }
} /* pdf_check_suspended_pages */


void
pdf_cleanup_pages(PDF *p)
{
    if (p->doc_pages != (pdf_pages *) 0)
    {
	int		i;
	pdf_pages *	dp = p->doc_pages;

	if (dp->groups)
	{
	    for (i = 0; i < dp->n_groups; ++i)
	    {
		if (dp->groups[i].name)
		    pdc_free(p->pdc, dp->groups[i].name);

		if (dp->groups[i].label.prefix)
		    pdc_free(p->pdc, dp->groups[i].label.prefix);
	    }

	    pdc_free(p->pdc, dp->groups);
	}

	if (dp->curr_pg)
	    pdf_delete_page(p, dp->curr_pg);

	if (dp->pages)
	{
	    for (i = 0; i <= dp->last_page; ++i)
	    {
		page_obj *po = &dp->pages[i];

		if (po->label.prefix)
		    pdc_free(p->pdc, po->label.prefix);

		if (po->action)
		    pdc_free(p->pdc, po->action);


		if (po->pg != (pdf_page *) 0)
		    pdf_delete_page(p, po->pg);
	    }

	    pdc_free(p->pdc, dp->pages);
	}

	if (dp->pnodes)
	{
	    pdc_free(p->pdc, dp->pnodes);
	}

	if (p->curr_ppt != 0)
	{
	    pdf_cleanup_page_cstate(p, &dp->default_ppt);
	    pdf_cleanup_page_tstate(p, &dp->default_ppt);
	}

	pdc_free(p->pdc, p->doc_pages);
	p->doc_pages = (pdf_pages *) 0;
    }
} /* pdf_cleanup_pages */


/******************** page group & labels management ********************/

static pg_group *
find_group(pdf_pages *dp, const char *name)
{
    int i;

    for (i = 0; i < dp->n_groups; ++i)
	if (strcmp(dp->groups[i].name, name) == 0)
	    return &dp->groups[i];

    return (pg_group *) 0;
} /* find_group */


static void
grow_group(PDF *p, pg_group *group, int pageno, int n)
{
    pdf_pages *	dp = p->doc_pages;
    int		i;

    while (dp->max_page + n >= dp->pages_capacity)
	pdf_grow_pages(p);

    if (dp->max_page >= pageno)
    {
	memmove(&dp->pages[pageno + n], &dp->pages[pageno],
		    (dp->max_page - pageno + 1) * sizeof (page_obj));

	for (i = pageno; i < pageno + n; ++i)
	    pdf_init_page_obj(&dp->pages[i]);
    }

    dp->max_page += n;

    if (dp->last_page >= pageno)
	dp->last_page += n;

    if (dp->current_page >= pageno)
	dp->current_page += n;

    group->capacity += n;

    for (i = group - dp->groups + 1; i < dp->n_groups; ++i)
	dp->groups[i].start += n;
}


/* translate the group-relative pageno to an absolute page number.
** as a side effect, the group gets enlarged as needed if pageno
** exceeds the current number of pages in the group.
*/
int
pdf_xlat_pageno(PDF *p, int pageno, const char *groupname)
{
    pdf_pages *	dp = p->doc_pages;
    pg_group *	group = (pg_group *) 0;

    if (groupname && *groupname)
    {
	if ((group = find_group(dp, groupname)) == (pg_group *) 0)
	    pdc_error(p->pdc, PDF_E_DOC_UNKNOWNGROUP, groupname, 0, 0, 0);
    }

    if (group)
    {
	if (pageno < 1)
	    pdc_error(p->pdc, PDF_E_PAGE_NOTEXIST2,
		pdc_errprintf(p->pdc, "%d", pageno), group->name, 0, 0);

	if (pageno > group->capacity)
	    grow_group(p, group, group->start + group->capacity,
					pageno - group->capacity);

	pageno = group->start + pageno - 1;
/*	dp->pages[pageno].id = pdc_alloc_id(p->out); */
    }
    else
    {
	if (dp->have_groups && pageno != 0)
	    pdc_error(p->pdc, PDF_E_PAGE_NEEDGROUP, 0, 0, 0, 0);
    }

    return pageno;
}


/* TODO (york): get rid of this function.
*/
void
pdf_init_pages2(PDF *p)
{
    pdf_pages *	dp = p->doc_pages;

    dp->pnodes[0] = pdc_alloc_id(p->out);
} /* pdf_init_pages2 */


static const pdc_defopt pdf_pagelabel_options[] =
{
    {"pagenumber", pdc_integerlist, PDC_OPT_NONE, 1, 1,
      1.0, PDC_INT_MAX, NULL},

    {"group", pdc_stringlist, PDC_OPT_NONE, 1, 1,
      1.0, PDF_MAX_NAMESTRING, NULL},

    {"style", pdc_keywordlist, PDC_OPT_NONE, 1, 1,
      0.0, 0.0, pdf_labelstyle_pdfkeylist},

    {"hypertextencoding", pdc_stringlist,  PDC_OPT_NONE, 1, 1,
      0.0, PDF_MAX_NAMESTRING, NULL},

    {"prefix", pdc_stringlist, PDC_OPT_NONE, 1, 1,
      0.0, PDF_MAX_NAMESTRING, NULL},

    {"start", pdc_integerlist, PDC_OPT_NONE, 1, 1,
      1.0, PDC_INT_MAX, NULL},

    PDC_OPT_TERMINATE
};

void
pdf_set_pagelabel(PDF *p, const char *optlist, int pageno)
{
    pdf_pages *	dp = p->doc_pages;
    pg_label *	lp;
    pdc_resopt *resopts = NULL;
    char **	strlist;
    int		inum;

    int		page = 0;
    char *	groupname = NULL;
    pdf_labelstyle style = label_none;
    pdc_encoding henc;
    char *	prefix = NULL;
    int		start = 1;

    resopts = pdc_parse_optionlist(p->pdc, optlist, pdf_pagelabel_options,
                                   NULL, pdc_true);

    switch (pageno)
    {
	case PDF_FC_BEGIN_DOCUMENT:
	    if (pdc_get_optvalues("group", resopts, NULL, &strlist))
		groupname = strlist[0];
	    else
		pdc_error(p->pdc, PDF_E_DOC_NEED_LABELOPT, "group", 0, 0, 0);

	    if (pdc_get_optvalues("pagenumber", resopts, &page, NULL))
		pdc_error(p->pdc, PDF_E_DOC_ILL_LABELOPT,
		    "pagenumber", 0, 0, 0);
	    break;

	case PDF_FC_END_DOCUMENT:
	    if (pdc_get_optvalues("group", resopts, NULL, &strlist))
		pdc_error(p->pdc, PDF_E_DOC_ILL_LABELOPT, "group", 0, 0, 0);

	    if (!pdc_get_optvalues("pagenumber", resopts, &page, NULL))
		pdc_error(p->pdc, PDF_E_DOC_NEED_LABELOPT,
		    "pagenumber", 0, 0, 0);

	    break;

	default:
	    if (pdc_get_optvalues("group", resopts, NULL, &strlist))
		pdc_error(p->pdc, PDF_E_DOC_ILL_LABELOPT, "group", 0, 0, 0);

	    if (pdc_get_optvalues("pagenumber", resopts, &page, NULL))
		pdc_error(p->pdc, PDF_E_DOC_ILL_LABELOPT,
		    "pagenumber", 0, 0, 0);

	    page = pageno;
	    break;
    }

    if (pdc_get_optvalues("style", resopts, &inum, NULL))
        style = (pdf_labelstyle) inum;

    henc = pdf_get_hypertextencoding_opt(p, resopts, NULL, pdc_true);

    pdf_get_opt_textlist(p, "prefix", resopts, henc,
                         pdc_true, NULL, &prefix, NULL);
    pdc_get_optvalues("start", resopts, &start, NULL);

    dp->have_labels = pdc_true;

    if (groupname)
    {
	pg_group *group;

	if ((group = find_group(dp, groupname)) == (pg_group *) 0)
	    pdc_error(p->pdc, PDF_E_DOC_UNKNOWNGROUP, groupname, 0, 0, 0);

	lp = &group->label;
    }
    else
    {
	if (dp->last_page < page)
	    pdc_error(p->pdc, PDF_E_PAGE_NOTEXIST,
		pdc_errprintf(p->pdc, "%d", page), 0, 0, 0);

	lp = &dp->pages[page].label;
    }

    lp->style = style;
    lp->start = start;

    if (prefix)
    {
	if (lp->prefix)
	    pdc_free(p->pdc, lp->prefix);

	lp->prefix = pdc_strdup(p->pdc, prefix);
    }
} /* pdf_set_pagelabel */


static void
write_label(PDF *p, pg_label *label, int pageno)
{
    pdc_printf(p->out, "%d", pageno);
    pdc_begin_dict(p->out);

    if (label->style != label_none)
    {
	pdc_printf(p->out, "/S/%s",
	    pdc_get_keyword(label->style, pdf_labelstyle_pdfkeylist));
    }

    if (label->prefix)
    {
	pdc_printf(p->out, "/P");
	pdf_put_hypertext(p, label->prefix);
    }

    if (label->start != 1)
	pdc_printf(p->out, "/St %d", label->start);

    pdc_end_dict(p->out);
} /* write_label */


pdc_id
pdf_write_pagelabels(PDF *p)
{
    pdf_pages *	dp = p->doc_pages;
    pdc_id	result;
    int		i, k;

    if (!dp->have_labels || dp->last_page == 0)
	return PDC_BAD_ID;

    result = pdc_begin_obj(p->out, PDC_NEW_ID);
    pdc_begin_dict(p->out);

    pdc_printf(p->out, "/Nums");
    pdc_begin_array(p->out);

    /* generate default label if page 1 doesn't have one:
    */
    if (dp->pages[1].label.start == 0 &&
	(dp->n_groups == 0 || dp->groups[0].label.start == 0))
    {
	pdc_puts(p->out, "0");
	pdc_begin_dict(p->out);
	pdc_puts(p->out, "/S/D");	/* 1-based decimal w/o prefix */
	pdc_end_dict(p->out);
    }

    if (dp->n_groups == 0)
    {
	for (i = 1; i <= dp->last_page; ++i)
	    if (dp->pages[i].label.start != 0)
		write_label(p, &dp->pages[i].label, i - 1);
    }
    else
    {
	for (i = 0; i < dp->n_groups; ++i)
	{
	    pg_group *gp = &dp->groups[i];

	    if (gp->label.start != 0 && gp->n_pages != 0)
	    {
		/* if present, the page label beats the group label.
		*/
		if (dp->pages[gp->start].label.start == 0)
		    write_label(p, &gp->label, gp->start - 1);
	    }

	    for (k = gp->start; k < gp->start + gp->n_pages; ++k)
		if (dp->pages[k].label.start != 0)
		    write_label(p, &dp->pages[k].label, k - 1);
	}
    }

    pdc_end_array_c(p->out);
    pdc_end_dict(p->out);
    pdc_end_obj(p->out);

    return result;
} /* pdf_write_pagelabels */


/************************** utility functions ***************************/

/* get the id of an existing or future page.
** pageno == 0 means current page. note that pdf_get_page_id(0) returns
** PDC_BAD_ID if no page has been opened yet, whereas pdf_current_page_id()
** returns a pre-allocated id for page 1 in this case.
*/
pdc_id
pdf_get_page_id(PDF *p, int pageno)
{
    pdf_pages *dp = p->doc_pages;

    if (pageno == 0)
    {
	return dp->pages[dp->current_page].id;
    }
    else
    {
	while (pageno >= dp->pages_capacity)
	    pdf_grow_pages(p);

	/* preallocate page object id for a later page
	*/
	if (dp->pages[pageno].id == PDC_BAD_ID)
	    dp->pages[pageno].id = pdc_alloc_id(p->out);

	return dp->pages[pageno].id;
    }
} /* pdf_get_page_id */

int
pdf_current_page(PDF *p)
{
    pdf_pages *dp = p->doc_pages;

    return dp ? dp->current_page : 0;
} /* pdf_current_page */

/* get the id of the current page. if there are no pages in the
** document yet, an id will be pre-allocated for page 1.
*/
int
pdf_current_page_id(PDF *p)
{
    pdf_pages *dp = p->doc_pages;

    if (dp->current_page != 0)
	return dp->pages[dp->current_page].id;
    else
	return pdf_get_page_id(p, 1);
} /* pdf_current_page_id */

int
pdf_last_page(PDF *p)
{
    return p->doc_pages->last_page;
} /* pdf_last_page */

int
pdf_search_page_fwd(PDF *p, int start_page, pdc_id id)
{
    pdf_pages *	dp = p->doc_pages;
    int		i;

    for (i = start_page; i <= dp->last_page; ++i)
    {
	if (dp->pages[i].id == id)
	    return i;
    }

    return -1;
} /* pdf_search_page_fwd */

int
pdf_search_page_bwd(PDF *p, int start_page, pdc_id id)
{
    pdf_pages *	dp = p->doc_pages;
    int		i;

    if (start_page == -1)
	start_page = dp->last_page;

    for (i = start_page; i > 0; --i)
    {
	if (dp->pages[i].id == id)
	    return i;
    }

    return -1;
} /* pdf_search_page_bwd */


double
pdf_get_pageheight(PDF *p)
{
    pdf_pages *	dp = p->doc_pages;
    page_obj *	po = &dp->pages[dp->current_page];

    return po->boxes[pdf_mediabox].ury - po->boxes[pdf_mediabox].lly;
} /* pdf_get_pageheight */

void
pdf_set_pagebox(
    PDF *	p,
    pdf_pagebox	box,
    double	llx,
    double	lly,
    double	urx,
    double	ury)
{
    pdf_pages *	dp = p->doc_pages;
    page_obj *	po = &dp->pages[dp->current_page];

    pdc_rect_init(&po->boxes[box], llx, lly, urx, ury);
} /* pdf_set_pagebox */

void
pdf_set_pagebox_llx(PDF *p, pdf_pagebox box, double llx)
{
    pdf_pages *	dp = p->doc_pages;
    page_obj *	po = &dp->pages[dp->current_page];

    po->boxes[box].llx = llx;
} /* pdf_set_pagebox_llx */

void
pdf_set_pagebox_lly(PDF *p, pdf_pagebox box, double lly)
{
    pdf_pages *	dp = p->doc_pages;
    page_obj *	po = &dp->pages[dp->current_page];

    po->boxes[box].lly = lly;
} /* pdf_set_pagebox_lly */

void
pdf_set_pagebox_urx(PDF *p, pdf_pagebox box, double urx)
{
    pdf_pages *	dp = p->doc_pages;
    page_obj *	po = &dp->pages[dp->current_page];

    po->boxes[box].urx = urx;
} /* pdf_set_pagebox_urx */

void
pdf_set_pagebox_ury(PDF *p, pdf_pagebox box, double ury)
{
    pdf_pages *	dp = p->doc_pages;
    page_obj *	po = &dp->pages[dp->current_page];

    po->boxes[box].ury = ury;
} /* pdf_set_pagebox_ury */

const pdc_rectangle *
pdf_get_pagebox(PDF *p, pdf_pagebox box)
{
    pdf_pages *	dp = p->doc_pages;
    page_obj *	po = &dp->pages[dp->current_page];

    return &po->boxes[box];
} /* pdf_get_pagebox */

pdf_annot *
pdf_get_annots_list(PDF *p)
{
    pdf_pages * dp = p->doc_pages;

    return dp->curr_pg->annots;
} /* pdf_get_annots_list */

void
pdf_set_annots_list(PDF *p, pdf_annot *ann)
{
    pdf_pages * dp = p->doc_pages;

    dp->curr_pg->annots = ann;
} /* pdf_set_annots_list */


pdc_id
pdf_get_thumb_id(PDF *p)
{
    pdf_pages *	dp = p->doc_pages;
    page_obj *	po = &dp->pages[dp->current_page];

    return po->thumb_id;
} /* pdf_get_thumb_id */

void
pdf_set_thumb_id(PDF *p, pdc_id id)
{
    pdf_pages *	dp = p->doc_pages;
    page_obj *	po = &dp->pages[dp->current_page];

    po->thumb_id = id;
} /* pdf_set_thumb_id */


/************************* contents sections ***************************/

void
pdf_begin_contents_section(PDF *p)
{
    pdf_page *pg = p->doc_pages->curr_pg;

    if (PDF_GET_STATE(p) != pdf_state_page || p->doc_pages->in_csect)
	return;

    p->doc_pages->in_csect = pdc_true;

    if (pg->next_content >= pg->contents_ids_capacity) {
	pg->contents_ids_capacity *= 2;
	pg->contents_ids = (pdc_id *) pdc_realloc(p->pdc, pg->contents_ids,
			    sizeof(pdc_id) * pg->contents_ids_capacity,
			    "pdf_begin_contents_section");
    }

    pg->contents_ids[pg->next_content] = pdc_begin_obj(p->out, PDC_NEW_ID);
    pdc_begin_dict(p->out);
    p->length_id = pdc_alloc_id(p->out);
    pdc_objref(p->out, "/Length", p->length_id);

    if (pdc_get_compresslevel(p->out))
	    pdc_puts(p->out, "/Filter/FlateDecode\n");

    pdc_end_dict(p->out);

    pdc_begin_pdfstream(p->out);

    pg->next_content++;
} /* pdf_begin_contents_section */


void
pdf_end_contents_section(PDF *p)
{
    if (!p->doc_pages->in_csect)
	return;

    p->doc_pages->in_csect = pdc_false;

    pdf_end_text(p);
    pdc_end_pdfstream(p->out);
    pdc_end_obj(p->out);

    pdc_put_pdfstreamlength(p->out, p->length_id);
} /* pdf_end_contents_section */


/************************* page tree generation *************************/

static void
pdf_write_pnode(PDF *p,
	pdc_id node_id,
	pdc_id parent_id,
	page_obj *kids,
	int n_kids,
	int n_pages)
{
    pdc_begin_obj(p->out, node_id);
    pdc_begin_dict(p->out);
    pdc_puts(p->out, "/Type/Pages\n");
    pdc_printf(p->out, "/Count %d\n", n_pages);

    if (parent_id != PDC_BAD_ID)
	pdc_objref(p->out, "/Parent", parent_id);

    pdc_puts(p->out, "/Kids");
    pdc_begin_array(p->out);

    do
    {
	pdc_objref_c(p->out, kids->id);
	++kids;
    } while (--n_kids > 0);

    pdc_end_array_c(p->out);
    pdc_end_dict(p->out);
    pdc_end_obj(p->out);
} /* pdf_write_pnode */

#define N_KIDS	10

static pdc_id
pdf_get_pnode_id(PDF *p)
{
    static const char fn[] = "pdf_get_pnode_id";

    pdf_pages *dp = p->doc_pages;

    if (dp->current_pnode_kids == N_KIDS)
    {
	if (++dp->current_pnode == dp->pnodes_capacity)
	{
	    dp->pnodes_capacity *= 2;
	    dp->pnodes = (pdc_id *) pdc_realloc(p->pdc, dp->pnodes,
			sizeof (pdc_id) * dp->pnodes_capacity, fn);
	}

	dp->pnodes[dp->current_pnode] = pdc_alloc_id(p->out);
	dp->current_pnode_kids = 1;
    }
    else
	++dp->current_pnode_kids;

    return dp->pnodes[dp->current_pnode];
} /* pdf_get_pnode_id */

static pdc_id
write_pages_tree(PDF *p,
    pdc_id parent_id,
    pdc_id *pnodes,
    page_obj *pages,
    int n_pages)
{
    if (n_pages <= N_KIDS)
    {
	/* this is a near-to-leaf node. use the pre-allocated id
	** from dp->pnodes.
	*/
	pdf_write_pnode(p, *pnodes, parent_id, pages, n_pages, n_pages);
	return *pnodes;
    }
    else
    {
	pdc_id node_id = pdc_alloc_id(p->out);
	page_obj kids[N_KIDS];
	int n_kids, rest;
        int tpow = N_KIDS;
	int i;

        /* tpow < n_pages <= tpow*N_KIDS
	*/
        while (tpow * N_KIDS < n_pages)
            tpow *= N_KIDS;

        n_kids = n_pages / tpow;
        rest = n_pages % tpow;

        for (i = 0; i < n_kids; ++i, pnodes += tpow / N_KIDS, pages += tpow)
	{
            kids[i].id = write_pages_tree(p, node_id, pnodes, pages, tpow);
	}

	if (rest)
	{
	    kids[i].id = write_pages_tree(p, node_id, pnodes, pages, rest);
	    ++n_kids;
	}

	pdf_write_pnode(p, node_id, parent_id, kids, n_kids, n_pages);
	return node_id;
    }
} /* write_pages_tree */

static void
pdf_write_box(PDF *p, pdc_rectangle *box, const char *name)
{
    if (pdc_rect_isnull(box))
	return;

    if (box->urx <= box->llx || box->ury <= box->lly)
    {
	pdc_error(p->pdc, PDF_E_PAGE_BADBOX, name,
	    pdc_errprintf(p->pdc, "%f %f %f %f",
	    box->llx, box->lly, box->urx, box->ury), 0, 0);
    }

    pdc_printf(p->out, "/%s[%f %f %f %f]\n",
	name, box->llx, box->lly, box->urx, box->ury);
} /* pdf_write_box */

pdc_id
pdf_write_pages_tree(PDF *p)
{
    int		i;
    pdf_pages *	dp = p->doc_pages;

    for (i = 1; i <= dp->last_page; ++i)
    {
	page_obj *po = &dp->pages[i];

	pdc_begin_obj(p->out, po->id);
	pdc_begin_dict(p->out);
	pdc_puts(p->out, "/Type/Page\n");
	pdc_objref(p->out, "/Parent", pdf_get_pnode_id(p));

	if (po->annots_id != PDC_BAD_ID)
	    pdc_objref(p->out, "/Annots", po->annots_id);

	if (po->contents_id != PDC_BAD_ID)
	    pdc_objref(p->out, "/Contents", po->contents_id);

	if (po->res_id != PDC_BAD_ID)
	    pdc_objref(p->out, "/Resources", po->res_id);

	if (po->thumb_id != PDC_BAD_ID)
	    pdc_objref(p->out, "/Thumb", po->thumb_id);

	if (po->duration > 0)
	    pdc_printf(p->out, "/Dur %f\n", po->duration);

	if (po->rotate > 0)
	    pdc_printf(p->out, "/Rotate %d\n", po->rotate);

	if (po->action)
	    pdf_write_action_entries(p, event_page, po->act_idlist);



	if (po->transition != trans_none)
	{
	    pdc_puts(p->out, "/Trans");
	    pdc_begin_dict(p->out);
	    pdc_printf(p->out, "/S/%s",
		pdc_get_keyword(po->transition, pdf_transition_pdfkeylist));

	    pdc_end_dict(p->out);
	}

	pdf_write_box(p, &po->boxes[pdf_artbox],	"ArtBox");
	pdf_write_box(p, &po->boxes[pdf_bleedbox],	"BleedBox");
	pdf_write_box(p, &po->boxes[pdf_cropbox],	"CropBox");
	pdf_write_box(p, &po->boxes[pdf_mediabox],	"MediaBox");
	pdf_write_box(p, &po->boxes[pdf_trimbox],	"TrimBox");

	pdc_end_dict(p->out);
	pdc_end_obj(p->out);
    }

    return write_pages_tree(p, PDC_BAD_ID, dp->pnodes, dp->pages + 1,
							    dp->last_page);
} /* pdf_write_pages_tree */


/**************************** resource lists ****************************/

static void
pdf_init_reslist(pdf_reslist *rl)
{
    rl->length = 0;
    rl->capacity = 0;
    rl->list = (int *) 0;
} /* pdf_init_reslist */

void
pdf_add_reslist(PDF *p, pdf_reslist *rl, int num)
{
    static const char fn[] = "pdf_add_reslist";

    if (rl->length == rl->capacity)
    {
        if (rl->capacity == 0)
        {
            rl->capacity = RESLIST_CHUNKSIZE;
            rl->list = (int *)
                pdc_malloc(p->pdc, rl->capacity * sizeof (pdf_reslist), fn);
        }
        else
        {
            rl->capacity *= 2;
            rl->list = (int *) pdc_realloc(p->pdc,
                rl->list, rl->capacity * sizeof (pdf_reslist), fn);
        }
    }

    rl->list[rl->length++] = num;
} /* pdf_add_reslist */


/****************************** begin_page ******************************/

/* begin_page_ext() only:
*/
#define PDF_ICC_FLAG  PDC_OPT_UNSUPP
#define PDF_SPOT_FLAG  PDC_OPT_UNSUPP

#define PDF_METADATA_FLAG  PDC_OPT_UNSUPP

static const pdc_defopt pdf_sepinfo_options[] =
{
    {"pages", pdc_integerlist, PDC_OPT_NONE, 1, 1,
      1.0, PDC_INT_MAX, NULL},

    {"spotname", pdc_stringlist, PDC_OPT_NONE, 1, 1,
      1.0, PDF_MAX_NAMESTRING, NULL},

    {"spotcolor", pdc_colorhandle, PDC_OPT_NONE, 1, 1,
      1.0, PDC_INT_MAX, NULL},

    PDC_OPT_TERMINATE
};

#define PDF_PAGE_OPTIONS1 \
\
    {"topdown", pdc_booleanlist, PDC_OPT_NONE, 1, 1, \
      0.0, 0.0, NULL}, \
\
    {"defaultgray", pdc_iccprofilehandle,  PDF_ICC_FLAG, 1, 1, \
      0.0, PDC_INT_MAX, NULL}, \
\
    {"defaultrgb", pdc_iccprofilehandle,  PDF_ICC_FLAG, 1, 1, \
      0.0, PDC_INT_MAX, NULL}, \
\
    {"defaultcmyk", pdc_iccprofilehandle,  PDF_ICC_FLAG, 1, 1, \
      0.0, PDC_INT_MAX, NULL}, \
\
    {"separationinfo", pdc_stringlist, PDF_SPOT_FLAG, 1, 1, \
      0.0, PDC_USHRT_MAX, NULL}, \

/* begin_page_ext() and resume_page():
*/
#define PDF_PAGE_OPTIONS2 \
\
    {"group", pdc_stringlist,  PDC_OPT_NONE, 1, 1, \
      1.0, PDF_MAX_NAMESTRING, NULL}, \
\
    {"pagenumber", pdc_integerlist,  PDC_OPT_NONE, 1, 1, \
      1.0, PDC_INT_MAX, NULL}, \

/* begin_page_ext() and end_page_ext():
*/
static const pdc_keyconn pdf_pagedim_keylist[] =
{
    { "a0.width",	(int) a0_width		},
    { "a0.height",	(int) a0_height		},
    { "a1.width",	(int) a1_width		},
    { "a1.height",	(int) a1_height		},
    { "a2.width",	(int) a2_width		},
    { "a2.height",	(int) a2_height		},
    { "a3.width",	(int) a3_width		},
    { "a3.height",	(int) a3_height		},
    { "a4.width",	(int) a4_width		},
    { "a4.height",	(int) a4_height		},
    { "a5.width",	(int) a5_width		},
    { "a5.height",	(int) a5_height		},
    { "a6.width",	(int) a6_width		},
    { "a6.height",	(int) a6_height		},
    { "b5.width",	(int) b5_width		},
    { "b5.height",	(int) b5_height		},
    { "letter.width",	(int) letter_width	},
    { "letter.height",	(int) letter_height	},
    { "legal.width",	(int) legal_width 	},
    { "legal.height",	(int) legal_height 	},
    { "ledger.width",	(int) ledger_width	},
    { "ledger.height",	(int) ledger_height	},
    { "11x17.width",	(int) p11x17_width	},
    { "11x17.height",	(int) p11x17_height	},
    { NULL, 0 }
};

#define PDF_PAGE_OPTIONS3 \
\
    {"action", pdc_stringlist,  PDC_OPT_NONE, 1, 1, \
      1.0, PDF_MAX_NAMESTRING, NULL}, \
\
    {"artbox", pdc_scalarlist, PDC_OPT_NONE, 4, 4,\
      PDC_FLOAT_MIN, PDC_FLOAT_MAX, NULL},\
\
    {"bleedbox", pdc_scalarlist, PDC_OPT_NONE, 4, 4,\
      PDC_FLOAT_MIN, PDC_FLOAT_MAX, NULL},\
\
    {"cropbox", pdc_scalarlist, PDC_OPT_NONE, 4, 4,\
      PDC_FLOAT_MIN, PDC_FLOAT_MAX, NULL},\
\
    {"duration", pdc_scalarlist, PDC_OPT_NONE, 1, 1,\
      0.0, PDC_FLOAT_MAX, NULL},\
\
    {"height", pdc_scalarlist, PDC_OPT_NONE, 1, 1,\
      0.0, PDC_FLOAT_MAX, pdf_pagedim_keylist},\
\
    {"label", pdc_stringlist,  PDC_OPT_NONE, 1, 1, \
      0.0, PDC_USHRT_MAX, NULL}, \
\
    {"mediabox", pdc_scalarlist, PDC_OPT_NONE, 4, 4,\
      PDC_FLOAT_MIN, PDC_FLOAT_MAX, NULL},\
\
    {"metadata", pdc_stringlist, PDF_METADATA_FLAG | PDC_OPT_PDC_1_4, 1, 1, \
      0.0, PDF_MAXSTRINGSIZE, NULL}, \
\
    {"rotate", pdc_integerlist,  PDC_OPT_NONE, 1, 1, \
      0.0, 270, NULL}, \
\
    {"transition", pdc_keywordlist, PDC_OPT_NONE, 1, 1, \
      0.0, 0.0, pdf_transition_keylist}, \
\
    {"trimbox", pdc_scalarlist, PDC_OPT_NONE, 4, 4,\
      PDC_FLOAT_MIN, PDC_FLOAT_MAX, NULL},\
\
    {"width", pdc_scalarlist, PDC_OPT_NONE, 1, 1,\
      0.0, PDC_FLOAT_MAX, pdf_pagedim_keylist},\

/* common helper function for pdf__begin_page_ext() and pdf__resume_page().
** returns the target group (if any) and the target page number. the
** page number is relative to the group (if available). page number -1
** means "no page number".
*/
static pg_group *
get_page_options2(PDF *p, pdc_resopt *resopts, int *pageno)
{
    pdf_pages *	dp = p->doc_pages;
    pg_group *	group = (pg_group *) 0;
    char **	strlist;

    *pageno = -1;

    if (pdc_get_optvalues("pagenumber", resopts, pageno, NULL))
    {
	if (*pageno <= 0)
	    pdc_error(p->pdc, PDF_E_PAGE_ILLNUMBER,
		pdc_errprintf(p->pdc, "%d", *pageno), 0, 0, 0);
    }

    if (pdc_get_optvalues("group", resopts, NULL, &strlist))
    {
	if ((group = find_group(dp, strlist[0])) == (pg_group *) 0)
	    pdc_error(p->pdc, PDF_E_DOC_UNKNOWNGROUP, strlist[0], 0, 0, 0);
    }

    if (group)
    {
	if (*pageno > group->n_pages)
	    pdc_error(p->pdc, PDF_E_PAGE_NOTEXIST2,
		pdc_errprintf(p->pdc, "%d", *pageno), group->name, 0, 0);
    }
    else
    {
	if (dp->have_groups)
	    pdc_error(p->pdc, PDF_E_PAGE_NEEDGROUP, 0, 0, 0, 0);

	if (*pageno > dp->last_page)
	    pdc_error(p->pdc, PDF_E_PAGE_NOTEXIST,
		pdc_errprintf(p->pdc, "%d", *pageno), 0, 0, 0);
    }

    return group;
} /* get_page_options2 */

/* common helper function for pdf__begin_page_ext() and pdf__end_page_ext().
*/
static void
get_page_options3(PDF *p, pdc_resopt *resopts, pdc_bool end_page)
{
    pdf_pages *	dp = p->doc_pages;
    int		pageno = dp->current_page;
    page_obj *	po = &dp->pages[pageno];
    pdc_scalar	width;
    pdc_scalar	height;
    pdc_bool	has_width;
    pdc_bool	has_height;
    pdc_bool	has_mediabox;

    if (pdc_get_optvalues("action", resopts, NULL, NULL))
    {
        po->action = (char *) pdc_save_lastopt(resopts, PDC_OPT_SAVE1ELEM);
	pdf_parse_and_write_actionlist(p, event_page, NULL,
                                       (char *) po->action);
    }

    pdc_get_optvalues("artbox", resopts, &po->boxes[pdf_artbox], NULL);
    pdc_get_optvalues("bleedbox", resopts, &po->boxes[pdf_bleedbox], NULL);
    pdc_get_optvalues("cropbox", resopts, &po->boxes[pdf_cropbox], NULL);
    pdc_get_optvalues("duration", resopts, &po->duration, NULL);

    if (pdc_get_optvalues("label", resopts, NULL, NULL))
    {
        char *pagelabel = pdf_get_opt_utf8name(p, "label", resopts);
	pdf_set_pagelabel(p, pagelabel, pageno);
        pdc_free(p->pdc, pagelabel);
    }

    /* the "width" and "height" options must be processed BEFORE the
    ** "mediabox" option, since the latter dominates over the formers.
    */
    has_width = pdc_get_optvalues("width", resopts, &width, NULL);
    has_height = pdc_get_optvalues("height", resopts, &height, NULL);

    if (has_width)
	po->boxes[pdf_mediabox].urx = po->boxes[pdf_mediabox].llx + width;

    if (has_height)
	po->boxes[pdf_mediabox].ury = po->boxes[pdf_mediabox].lly + height;

    has_mediabox =
	pdc_get_optvalues("mediabox", resopts, &po->boxes[pdf_mediabox], NULL);

    width = po->boxes[pdf_mediabox].urx - po->boxes[pdf_mediabox].llx;
    height = po->boxes[pdf_mediabox].ury - po->boxes[pdf_mediabox].lly;

    if (p->ydirection == -1)
    {
	if (end_page)
	{
	    if (has_mediabox || has_width || has_height)
		pdc_error(p->pdc, PDF_E_PAGE_ILLCHGSIZE, 0, 0, 0, 0);
	}
	else
	{
	    if (width == 0 || height == 0)
		pdc_error(p->pdc, PDF_E_PAGE_TOPDOWN_NODIMS, 0, 0, 0, 0);

	    if ((height < PDF_ACRO_MINPAGE || width < PDF_ACRO_MINPAGE ||
		height > PDF_ACRO_MAXPAGE || width > PDF_ACRO_MAXPAGE))
		    pdc_warning(p->pdc, PDF_E_PAGE_SIZE_ACRO, 0, 0, 0, 0);
	}
    }


    pdc_get_optvalues("rotate", resopts, &po->rotate, NULL);
    switch (po->rotate)
    {
	case 0: case 90: case 180: case 270:
	    break;

	default:
	    pdc_error(p->pdc, PDF_E_PAGE_ILLROTATE,
		pdc_errprintf(p->pdc, "%d", po->rotate), 0, 0, 0);
    }

    pdc_get_optvalues("transition", resopts, &po->transition, NULL);
    if (po->transition >= (int) TRANS_1_5 && p->compatibility < PDC_1_5)
	pdc_error(p->pdc, PDF_E_PAGE_TRANS_COMPAT,
	    pdc_get_keyword(po->transition, pdf_transition_keylist), 0, 0, 0);

    pdc_get_optvalues("trimbox", resopts, &po->boxes[pdf_trimbox], NULL);
} /* get_page_options3 */


static const pdc_defopt pdf_begin_page_ext_options[] =
{
    PDF_PAGE_OPTIONS1
    PDF_PAGE_OPTIONS2
    PDF_PAGE_OPTIONS3

    PDC_OPT_TERMINATE
};

void
pdf__begin_page_ext(
    PDF *	p,
    pdc_scalar	width,
    pdc_scalar	height,
    const char *optlist)
{
    static const char fn[] = "pdf__begin_page";

    pdf_pages *	dp = p->doc_pages;
    pdf_page *	pg;
    page_obj *	po;


    pdc_resopt *resopts = NULL;
    pg_group *	group = (pg_group *) 0;
    int		pageno = -1;

    if (width < 0)
	pdc_error(p->pdc, PDC_E_ILLARG_NONNEG, "width", 0, 0, 0);

    if (height < 0)
	pdc_error(p->pdc, PDC_E_ILLARG_NONNEG, "height", 0, 0, 0);


    if (optlist && *optlist)
    {
	pdc_clientdata cdata;

        pdf_set_clientdata(p, &cdata);
	resopts = pdc_parse_optionlist(p->pdc,
	    optlist, pdf_begin_page_ext_options, &cdata, pdc_true);

	group = get_page_options2(p, resopts, &pageno);
    }

    if (group)
    {
	if (pageno == -1)
	    pageno = group->start + group->n_pages;
	else
	    pageno = group->start + pageno - 1;

	if (++group->n_pages > group->capacity)
	{
	    grow_group(p, group, pageno, 1);
	}
	else if (pageno < group->start + group->n_pages - 1)
	{
	    memmove(&dp->pages[pageno + 1], &dp->pages[pageno],
		(group->start + group->n_pages - pageno) * sizeof (page_obj));

	    pdf_init_page_obj(&dp->pages[pageno]);
	}

	if (dp->last_page < group->start + group->n_pages - 1)
	    dp->last_page = group->start + group->n_pages - 1;
    }
    else
    {
	if (++dp->last_page >= dp->pages_capacity)
	    pdf_grow_pages(p);

	if (dp->last_page > dp->max_page)
	    ++dp->max_page;

	if (pageno == -1)
	    pageno = dp->last_page;

	if (pageno != dp->last_page)
	{
	    memmove(&dp->pages[pageno + 1], &dp->pages[pageno],
			(dp->max_page - pageno) * sizeof (page_obj));

	    pdf_init_page_obj(&dp->pages[pageno]);
	}
    }

    po = &dp->pages[pageno];
    dp->current_page = pageno;

    /* no id has been preallocated */
    if (po->id == PDC_BAD_ID)
	po->id = pdc_alloc_id(p->out);

    pg = dp->curr_pg = (pdf_page *) pdc_malloc(p->pdc, sizeof (pdf_page), fn);
    p->curr_ppt = &pg->ppt;

    pg->contents_ids = (pdc_id *) 0;
    pg->annots = (pdf_annot *) 0;

    /* save and take over global parameters.
    */
    pg->ydir =		dp->old_ydir =		p->ydirection;

    pg->rl_colorspaces.list	= (int *) 0;
    pg->rl_extgstates.list	= (int *) 0;
    pg->rl_fonts.list		= (int *) 0;
    pg->rl_layers.list		= (int *) 0;
    pg->rl_patterns.list	= (int *) 0;
    pg->rl_shadings.list	= (int *) 0;
    pg->rl_xobjects.list	= (int *) 0;

    pg->contents_ids_capacity = CONTENTS_CHUNKSIZE;
    pg->contents_ids = (pdc_id *) pdc_malloc(p->pdc,
	    sizeof(pdc_id) * pg->contents_ids_capacity, fn);

    /* might be overwritten by options */
    pdc_rect_init(&po->boxes[pdf_mediabox], 0, 0, width, height);

    if (resopts)
    {
	pdc_bool topdown = pdc_false;

        if (pdc_get_optvalues("topdown", resopts, &topdown, NULL))
	    p->ydirection = pg->ydir = topdown ? -1 : 1;


	get_page_options3(p, resopts, pdc_false);
    }

    /* initialize the current ppt descriptor. p->ydirection
    ** must be set before pdf_init_ppt()!
    */
    pdf_init_ppt(p, pdc_true);

    pg->next_content = 0;

    pdf_init_reslist(&pg->rl_colorspaces);
    pdf_init_reslist(&pg->rl_extgstates);
    pdf_init_reslist(&pg->rl_fonts);
    pdf_init_reslist(&pg->rl_layers);
    pdf_init_reslist(&pg->rl_patterns);
    pdf_init_reslist(&pg->rl_shadings);
    pdf_init_reslist(&pg->rl_xobjects);

    PDF_SET_STATE(p, pdf_state_page);

    pdf_begin_contents_section(p);

    /* top-down y coordinates */
    pdf_set_topdownsystem(p, height);
} /* pdf__begin_page_ext */


void
pdf__begin_page(
    PDF *	p,
    pdc_scalar	width,
    pdc_scalar	height)
{
    if (p->doc_pages->have_groups)
	pdc_error(p->pdc, PDF_E_PAGE_NEEDGROUP2, 0, 0, 0, 0);

    pdf__begin_page_ext(p, width, height, 0);
} /* pdf__begin_page */


/*************************** suspend & resume ***************************/

void
pdf_pg_suspend(PDF *p)
{
    pdf_pages *dp = p->doc_pages;

    if (PDF_GET_STATE(p) != pdf_state_page)
    {
	dp->last_suspended = -1;
    }
    else
    {
	pdf_page *pg = dp->curr_pg;

	pdf_end_contents_section(p);

	/* restore global parms.
	*/
	p->ydirection =		dp->old_ydir;

	pdf_get_page_colorspaces(p, &pg->rl_colorspaces);
	pdf_get_page_extgstates(p, &pg->rl_extgstates);
	pdf_get_page_fonts(p, &pg->rl_fonts);
	pdf_get_page_patterns(p, &pg->rl_patterns);
	pdf_get_page_shadings(p, &pg->rl_shadings);
	pdf_get_page_xobjects(p, &pg->rl_xobjects);

	dp->pages[dp->current_page].pg = pg;
	dp->curr_pg = (pdf_page *) 0;
	dp->last_suspended = dp->current_page;

	/* restore the default ppt for out-of-page usage.
	*/
	p->curr_ppt = &dp->default_ppt;
    }

    pdf_init_ppt(p, pdc_false);
} /* pdf_pg_suspend */


static const pdc_defopt pdf_suspend_page_options[] =
{
    PDC_OPT_TERMINATE
};

void
pdf__suspend_page(PDF *p, const char *optlist)
{
    if (optlist && *optlist)
    {
	pdc_resopt *resopts = pdc_parse_optionlist(p->pdc,
	    optlist, pdf_suspend_page_options, NULL, pdc_true);

	(void) resopts;
    }

    pdf_pg_suspend(p);
    PDF_SET_STATE(p, pdf_state_document);
} /* pdf__suspend_page */


void
pdf_pg_resume(PDF *p, int pageno)
{
    pdf_pages *dp = p->doc_pages;

    if (pageno == -1)
    {
	pageno = dp->last_suspended;
	dp->last_suspended = -1;
    }

    if (pageno == -1)
    {
	PDF_SET_STATE(p, pdf_state_document);
    }
    else
    {
	pdf_page *pg;
	int i;

	/* prevent error cleanup from killing the same page twice.
	*/
	pg = dp->curr_pg	= dp->pages[pageno].pg;
	dp->pages[pageno].pg	= (pdf_page *) 0;

	dp->current_page	= pageno;
	p->curr_ppt		= &pg->ppt;

	PDF_SET_STATE(p, pdf_state_page);

	/* save global parameters and replace them
	** with the page specific ones.
	*/
	dp->old_ydir =		p->ydirection;

	p->ydirection =		pg->ydir;

	pdf_begin_contents_section(p);

	/* mark global resources as "used on current page".
	*/
	for (i = 0; i < pg->rl_colorspaces.length; ++i)
	    pdf_mark_page_colorspace(p, pg->rl_colorspaces.list[i]);

	for (i = 0; i < pg->rl_extgstates.length; ++i)
	    pdf_mark_page_extgstate(p, pg->rl_extgstates.list[i]);

	for (i = 0; i < pg->rl_fonts.length; ++i)
	    pdf_mark_page_font(p, pg->rl_fonts.list[i]);


	for (i = 0; i < pg->rl_patterns.length; ++i)
	    pdf_mark_page_pattern(p, pg->rl_patterns.list[i]);

	for (i = 0; i < pg->rl_shadings.length; ++i)
	    pdf_mark_page_shading(p, pg->rl_shadings.list[i]);

	for (i = 0; i < pg->rl_xobjects.length; ++i)
	    pdf_mark_page_xobject(p, pg->rl_xobjects.list[i]);
    }
} /* pdf_pg_resume */


static const pdc_defopt pdf_resume_page_options[] =
{
    PDF_PAGE_OPTIONS2

    PDC_OPT_TERMINATE
};

void
pdf__resume_page(PDF *p, const char *optlist)
{
    pdf_pages *	dp = p->doc_pages;
    pg_group *	group = (pg_group *) 0;
    int		pageno = -1;		/* logical page number */
    int		physno;			/* physical page number */

    if (optlist && *optlist)
    {
	pdc_resopt *resopts = pdc_parse_optionlist(p->pdc,
	    optlist, pdf_resume_page_options, NULL, pdc_true);

	group = get_page_options2(p, resopts, &pageno);
    }

    if (group)
    {
	if (pageno == -1)
	    pageno = group->n_pages - 1;
	else
	    pageno = pageno - 1;

	physno = group->start + pageno - 1;
    }
    else
    {
	if (pageno == -1)
	    pageno = dp->last_page;

	physno = pageno;
    }

    if (dp->pages[pageno].pg == (pdf_page *) 0)
    {
	if (group)
	{
	    pdc_error(p->pdc, PDF_E_PAGE_NOSUSPEND2,
		pdc_errprintf(p->pdc, "%d", pageno), group->name, 0, 0);
	}
	else
	{
	    pdc_error(p->pdc, PDF_E_PAGE_NOSUSPEND,
		pdc_errprintf(p->pdc, "%d", pageno), 0, 0, 0);
	}
    }

    pdf_pg_resume(p, physno);
} /* pdf__resume_page */


/******************************* end_page *******************************/

static const pdc_defopt pdf_end_page_ext_options[] =
{
    PDF_PAGE_OPTIONS3

    PDC_OPT_TERMINATE
};

void
pdf__end_page_ext(PDF *p, const char *optlist)
{
    pdf_pages *	dp = p->doc_pages;
    page_obj *	po = &dp->pages[dp->current_page];

    pdc_scalar	width;
    pdc_scalar	height;
    pdf_page *	pg;


    if (optlist && *optlist)
    {
	pdc_resopt *resopts = pdc_parse_optionlist(p->pdc,
	    optlist, pdf_end_page_ext_options, NULL, pdc_true);

	get_page_options3(p, resopts, pdc_true);
    }

    width = po->boxes[pdf_mediabox].urx - po->boxes[pdf_mediabox].llx;
    height = po->boxes[pdf_mediabox].ury - po->boxes[pdf_mediabox].lly;

    if (width == 0 || height == 0)
	pdc_error(p->pdc, PDF_E_PAGE_NODIMS, 0, 0, 0, 0);

    if ((height < PDF_ACRO_MINPAGE || width < PDF_ACRO_MINPAGE ||
	height > PDF_ACRO_MAXPAGE || width > PDF_ACRO_MAXPAGE))
	    pdc_warning(p->pdc, PDF_E_PAGE_SIZE_ACRO, 0, 0, 0, 0);




    /* check whether PDF_save() and PDF_restore() calls are balanced */
    if (p->curr_ppt->sl > 0)
	pdc_error(p->pdc, PDF_E_GSTATE_UNMATCHEDSAVE, 0, 0, 0, 0);

    /* TODO (york): avoid memory leak in error case. */
    pg = dp->curr_pg;


    pdf_end_contents_section(p);


    /* if no "duration" or "transition" options have been specified
    ** for this page, fall back on the (deprecated) global parameters.
    */
    if (po->duration == -1)
	po->duration = dp->duration;

    if (po->transition == -1)
	po->transition = dp->transition;

    if (pg->next_content > 0)
    {
	if (pg->next_content == 1)
	{
	    po->contents_id = pg->contents_ids[0];
	}
	else
	{
	    int i;

	    po->contents_id = pdc_begin_obj(p->out, PDC_NEW_ID);
	    pdc_begin_array(p->out);

	    for (i = 0; i < pg->next_content; ++i)
	    {
		pdc_objref_c(p->out, pg->contents_ids[i]);
	    }

	    pdc_end_array(p->out);
	    pdc_end_obj(p->out);
	}
    }

    if (po->action)
	pdf_parse_and_write_actionlist(p, event_page, po->act_idlist,
							    po->action);

    po->annots_id = pdf_write_annots_root(p, pg->annots, NULL);

    /* resources dictionary
    */
    po->res_id = pdc_begin_obj(p->out, PDC_NEW_ID);

    pdc_begin_dict(p->out);

    pdf_write_page_fonts(p);			/* Font resources */

    pdf_write_page_colorspaces(p);		/* Color space resources */

    pdf_write_page_pattern(p);			/* Pattern resources */

    pdf_write_page_shadings(p);			/* Shading resources */

    pdf_write_xobjects(p);			/* XObject resources */

    pdf_write_page_extgstates(p);		/* ExtGState resources */

    pdc_end_dict(p->out);
    pdc_end_obj(p->out);

    pdf_write_page_annots(p, pg->annots);	/* Annotation dicts */


    /* restore global parms.
    */
    p->ydirection =	dp->old_ydir;

    /* restore the default ppt for out-of-page usage.
    */
    p->curr_ppt = &dp->default_ppt;
    pdf_init_ppt(p, pdc_false);
    PDF_SET_STATE(p, pdf_state_document);
    pdf_delete_page(p, pg);
    dp->curr_pg = (pdf_page *) 0;

    if (p->flush & (pdf_flush_page | pdf_flush_content))
	pdc_flush_stream(p->out);
} /* pdf__end_page_ext */


/*****************************************************************************/
/**                deprecated historical page functions                     **/
/*****************************************************************************/

/* set page display duration for current and future pages */

void
pdf_set_duration(PDF *p, double t)
{
    p->doc_pages->duration = t;
}

/* set transition mode for current and future pages */

void
pdf_set_transition(PDF *p, const char *transition)
{
    int i;

    if (transition == NULL || !*transition)
        transition = "none";

    i = pdc_get_keycode_ci(transition, pdf_transition_keylist);

    if (i == PDC_KEY_NOTFOUND)
        pdc_error(p->pdc, PDC_E_PAR_ILLPARAM, transition, "transition", 0, 0);

    if (i >= (int) TRANS_1_5 && p->compatibility < PDC_1_5)
	pdc_error(p->pdc, PDF_E_PAGE_TRANS_COMPAT,
	    pdc_get_keyword(i, pdf_transition_keylist), 0, 0, 0);

    p->doc_pages->transition = i;
}
