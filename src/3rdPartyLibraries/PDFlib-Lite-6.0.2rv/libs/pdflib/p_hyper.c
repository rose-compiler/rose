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

/* $Id: p_hyper.c,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * PDFlib routines for hypertext stuff:
 * named destination, bookmarks, document info
 *
 */

#define P_HYPER_C

#include "p_intern.h"
#include "p_color.h"



/* --------------------------  named destinations  -------------------------- */

typedef enum
{
    fixed,
    fitwindow,
    fitwidth,
    fitheight,
    fitrect,
    fitvisible,
    fitvisiblewidth,
    fitvisibleheight,
    nameddest,
    filedest
}
pdf_desttype;

static const pdc_keyconn pdf_type_keylist[] =
{
    {"fixed",           fixed},
    {"fitwindow",       fitwindow},
    {"fitwidth",        fitwidth},
    {"fitheight",       fitheight},
    {"fitrect",         fitrect},
    {"fitvisible",      fitvisible},
    {"fitvisiblewidth", fitvisiblewidth},
    {"fitvisibleheight",fitvisibleheight},
    {"nameddest",       nameddest},
    {"file",            filedest},
    {NULL, 0}
};

/* Destination structure */
struct pdf_dest_s
{
    pdf_desttype type;
    char        *filename;      /* name of a file to be launched - deprecated */
    int         remote_page;    /* remote target page number */
    int		pgnum;
    pdc_id      page;           /* local target page object id */
    char        *name;          /* destination name, only for type=nameddest */
    int         len;            /* length of the name string */
    pdc_scalar  zoom;           /* magnification */
    pdc_scalar  left;
    pdc_scalar  right;
    pdc_scalar  bottom;
    pdc_scalar  top;
    pdc_scalar  color[3];       /* rgb color of bookmark text - deprecated */
    pdc_fontstyle fontstyle;    /* font style of bookmark text - deprecated */
};

static const pdc_defopt pdf_destination_options[] =
{
    {"hypertextencoding", pdc_stringlist, PDC_OPT_NONE, 1, 1,
      0.0, PDF_MAX_NAMESTRING, NULL},

    {"hypertextformat", pdc_keywordlist, PDC_OPT_NONE, 1, 1,
      0.0, 0.0, pdf_textformat_keylist},

    {"fitbbox", pdc_booleanlist, PDC_OPT_NONE,
      1, 1, 0.0, 0.0, NULL},

    {"fitheight", pdc_booleanlist, PDC_OPT_NONE,
      1, 1, 0.0, 0.0, NULL},

    {"fitpage", pdc_booleanlist, PDC_OPT_NONE,
      1, 1, 0.0, 0.0, NULL},

    {"fitwidth", pdc_booleanlist, PDC_OPT_NONE,
      1, 1, 0.0, 0.0, NULL},

    {"retain", pdc_booleanlist, PDC_OPT_NONE,
      1, 1, 0.0, 0.0, NULL},

    {"type", pdc_keywordlist, PDC_OPT_NONE,
      1, 1, 0.0, 0.0, pdf_type_keylist},

    {"name", pdc_stringlist, PDC_OPT_NONE,
      1, 1, 1.0, PDF_MAXSTRINGSIZE, NULL},

    {"page", pdc_integerlist, PDC_OPT_NONE,
      1, 1, 0, PDC_INT_MAX, NULL},

    {"group", pdc_stringlist,  PDC_OPT_NONE, 1, 1,
      1.0, PDF_MAX_NAMESTRING, NULL},

    /* Acrobat 5 supports a maximum zoom of 1600%, but we allow some more */
    {"zoom", pdc_scalarlist, PDC_OPT_PERCENT,
      1, 1, 0.0, 10000, NULL},

    {"left", pdc_scalarlist, PDC_OPT_NONE,
      1, 1, 0.0, PDF_ACRO_MAXPAGE, NULL},

    {"right", pdc_scalarlist, PDC_OPT_NONE,
      1, 1, 0.0, PDF_ACRO_MAXPAGE, NULL},

    {"bottom", pdc_scalarlist, PDC_OPT_REQUIRIF1,
      1, 1, 0.0, PDF_ACRO_MAXPAGE, NULL},

    {"top", pdc_scalarlist, PDC_OPT_NONE,
      1, 1, 0.0, PDF_ACRO_MAXPAGE, NULL},

    {"color", pdc_scalarlist, PDC_OPT_NONE,
      1, 3, 0.0, 1.0, NULL},

    {"fontstyle", pdc_keywordlist, PDC_OPT_NONE,
      1, 1, 0.0, 0.0, pdf_fontstyle_pdfkeylist},

    {"filename", pdc_stringlist, PDC_OPT_NONE,
      1, 1, 0.0, PDF_FILENAMELEN, NULL},

    PDC_OPT_TERMINATE
};

pdf_dest *
pdf_init_destination(PDF *p)
{
    static const char fn[] = "pdf_init_destination";
    pdf_dest *dest = (pdf_dest *) pdc_malloc(p->pdc, sizeof(pdf_dest), fn);

    dest->type = fitwindow;
    dest->remote_page = 0;
    dest->pgnum = 0;
    dest->page = PDC_BAD_ID;
    dest->left = -1;
    dest->right = -1;
    dest->bottom = -1;
    dest->top = -1;
    dest->zoom = -1;
    dest->name = NULL;
    dest->color[0] = 0.0;
    dest->color[1] = 0.0;
    dest->color[2] = 0.0;
    dest->fontstyle = pdc_Normal;
    dest->filename = NULL;

    return dest;
}

void
pdf_cleanup_destination(PDF *p, pdf_dest *dest)
{
    if (dest)
    {
        if (dest->name)
        {
            pdc_free(p->pdc, dest->name);
            dest->name = NULL;
        }
        if (dest->filename)
        {
            pdc_free(p->pdc, dest->filename);
            dest->filename = NULL;
        }

        pdc_free(p->pdc, dest);
    }
}

pdf_dest *
pdf_parse_destination_optlist(
    PDF *p,
    const char *optlist,
    int page,
    pdf_destuse destuse,
    pdc_bool verbose)
{
    int minpage;
    pdc_resopt *resopts;
    pdc_encoding hypertextencoding;
    const char *keyword;
    const char *type_name;
    char **strlist = NULL;
    int inum;
    pdc_bool boolval;

    /* Defaults */
    pdf_dest *dest = pdf_init_destination(p);

    /* parse option list */
    resopts = pdc_parse_optionlist(p->pdc, optlist, pdf_destination_options,
                                   NULL, pdc_true);

    if (pdc_get_optvalues("fitbbox", resopts, &boolval, NULL) &&
        boolval == pdc_true)
        dest->type = fitvisible;

    if (pdc_get_optvalues("fitheight", resopts, &boolval, NULL) &&
        boolval == pdc_true)
        dest->type = fitheight;

    if (pdc_get_optvalues("fitpage", resopts, &boolval, NULL) &&
        boolval == pdc_true)
        dest->type = fitwindow;

    if (pdc_get_optvalues("fitwidth", resopts, &boolval, NULL) &&
        boolval == pdc_true)
        dest->type = fitwidth;

    if (pdc_get_optvalues("retain", resopts, &boolval, NULL) &&
        boolval == pdc_true)
        dest->type = fixed;

    if (pdc_get_optvalues("type", resopts, &inum, NULL))
        dest->type = (pdf_desttype) inum;
    type_name = pdc_get_keyword(dest->type, pdf_type_keylist);

    hypertextencoding =
        pdf_get_hypertextencoding_opt(p, resopts, NULL, verbose);

    keyword = "name";
    if (pdf_get_opt_textlist(p, keyword, resopts, hypertextencoding, pdc_true,
                             NULL, &dest->name, NULL))
    {
        if (dest->type != nameddest)
        {
            dest->name = NULL;
            if (verbose)
                pdc_warning(p->pdc, PDF_E_HYP_OPTIGNORE_FORTYPE, keyword,
                            type_name, 0, 0);
        }
        else
            pdc_save_lastopt(resopts, PDC_OPT_SAVE1ELEM);
    }

    keyword = "page";
    if (pdc_get_optvalues(keyword, resopts, &page, NULL) &&
        dest->type == filedest && verbose)
        pdc_warning(p->pdc, PDF_E_HYP_OPTIGNORE_FORTYPE, keyword, type_name,
                    0, 0);

    keyword = "group";
    if (pdc_get_optvalues(keyword, resopts, NULL, &strlist))
    {
	page = pdf_xlat_pageno(p, page, strlist[0]);
    }

    keyword = "zoom";
    if (pdc_get_optvalues(keyword, resopts, &dest->zoom, NULL) &&
        dest->type != fixed && verbose)
        pdc_warning(p->pdc, PDF_E_HYP_OPTIGNORE_FORTYPE, keyword, type_name,
                    0, 0);

    keyword = "left";
    if (pdc_get_optvalues(keyword, resopts, &dest->left, NULL) &&
        (dest->type == fitwindow  || dest->type == fitwidth ||
         dest->type == fitvisible || dest->type == fitvisiblewidth ||
         dest->type == nameddest  || dest->type == filedest) && verbose)
        pdc_warning(p->pdc, PDF_E_HYP_OPTIGNORE_FORTYPE, keyword, type_name,
                    0, 0);

    keyword = "right";
    if (pdc_get_optvalues(keyword, resopts, &dest->right, NULL) &&
        dest->type != fitrect && verbose)
        pdc_warning(p->pdc, PDF_E_HYP_OPTIGNORE_FORTYPE, keyword, type_name,
                    0, 0);

    keyword = "bottom";
    if (pdc_get_optvalues(keyword, resopts, &dest->bottom, NULL) &&
        dest->type != fitrect && verbose)
        pdc_warning(p->pdc, PDF_E_HYP_OPTIGNORE_FORTYPE, keyword, type_name,
                    0, 0);

    keyword = "top";
    if (pdc_get_optvalues(keyword, resopts, &dest->top, NULL) &&
        (dest->type == fitwindow  || dest->type == fitheight ||
         dest->type == fitvisible || dest->type == fitvisibleheight ||
         dest->type == nameddest  || dest->type == filedest) && verbose)
        pdc_warning(p->pdc, PDF_E_HYP_OPTIGNORE_FORTYPE, keyword, type_name,
                    0, 0);

    keyword = "color";
    if (pdc_get_optvalues(keyword, resopts, &dest->color, NULL) &&
        destuse != pdf_bookmark && verbose)
        pdc_warning(p->pdc, PDF_E_HYP_OPTIGNORE_FORELEM, keyword, 0, 0, 0);

    keyword = "fontstyle";
    if (pdc_get_optvalues(keyword, resopts, &inum, NULL))
    {
        dest->fontstyle = (pdc_fontstyle) inum;
        if (destuse != pdf_bookmark && verbose)
            pdc_warning(p->pdc, PDF_E_HYP_OPTIGNORE_FORELEM, keyword, 0, 0, 0);
    }

    keyword = "filename";
    if (pdc_get_optvalues(keyword, resopts, NULL, NULL))
    {
        if (dest->type != filedest)
        {
            if (verbose)
                pdc_warning(p->pdc, PDF_E_HYP_OPTIGNORE_FORTYPE, keyword,
                            type_name, 0, 0);
        }
        else
            dest->filename =
                (char *) pdc_save_lastopt(resopts, PDC_OPT_SAVE1ELEM);
    }

    pdc_cleanup_optionlist(p->pdc, resopts);

    switch (dest->type)
    {
        case fitwidth:
        /* Trick: we don't know the height of a future page yet,
         * so we use a "large" value for top which will do for
         * most pages. If it doesn't work, not much harm is done.
         */
        if (dest->top == -1)
            dest->top = 10000;
        break;

        case fitrect:
        case fitheight:
        case fitvisiblewidth:
        case fitvisibleheight:
        if (dest->left == -1)
            dest->left = 0;
        if (dest->bottom == -1)
            dest->bottom = 0;
        if (dest->right == -1)
            dest->right = 1000;
        if (dest->top == -1)
            dest->top = 1000;
        break;

        case nameddest:
        if (destuse == pdf_nameddest)
        {
            pdf_cleanup_destination(p, dest);
            pdc_error(p->pdc, PDC_E_OPT_ILLKEYWORD, "type", type_name, 0, 0);
        }
        if (dest->name == NULL)
        {
            pdf_cleanup_destination(p, dest);
            pdc_error(p->pdc, PDC_E_OPT_NOTFOUND, "name", 0, 0, 0);
        }
        break;

        case filedest:
        if (destuse != pdf_bookmark)
        {
            pdf_cleanup_destination(p, dest);
            pdc_error(p->pdc, PDC_E_OPT_ILLKEYWORD, "type", type_name, 0, 0);
        }
        if (dest->filename == NULL)
        {
            pdf_cleanup_destination(p, dest);
            pdc_error(p->pdc, PDC_E_OPT_NOTFOUND, "filename", 0, 0, 0);
        }
        break;

        default:
        break;
    }

    /* check for minpage */
    minpage = (destuse == pdf_bookmark) ? 0 : 1;
    switch (destuse)
    {
        case pdf_nameddest:
        case pdf_locallink:
	    if (page == 0)
	    {
		page = pdf_current_page(p);
	    }

        case pdf_openaction:
        case pdf_bookmark:
        case pdf_remotelink:
	    if (page < minpage)
	    {
		const char *stemp = pdc_errprintf(p->pdc, "%d", page);
		pdf_cleanup_destination(p, dest);
		pdc_error(p->pdc, PDC_E_ILLARG_HANDLE, "page", stemp, 0, 0);
	    }
	    break;
    }

    dest->pgnum = page;

    if (destuse != pdf_remotelink && destuse != pdf_openaction && page != 0)
    {
	dest->page = pdf_get_page_id(p, page);
    }

    /* remote page number */
    if (destuse == pdf_remotelink)
        dest->remote_page = page;

    return dest;
}

#if defined(_MSC_VER) && defined(_MANAGED)
#pragma unmanaged
#endif
pdf_dest *
pdf_get_option_destname(PDF *p, pdc_resopt *resopts,
                        pdc_encoding hypertextencoding)
{
    pdc_text_format hypertextformat = pdc_bytes;
    pdf_dest *dest = NULL;
    char **strlist;
    int outlen;

    if (pdc_get_optvalues("destname", resopts, NULL, &strlist))
    {
        dest = pdf_init_destination(p);
        dest->type = nameddest;

        if (pdc_is_lastopt_utf8(resopts))
            hypertextformat = PDC_UTF8;
        dest->name = pdf_convert_hypertext(p, strlist[0], 0, hypertextformat,
                                      hypertextencoding, p->hypertextcodepage,
                                      &outlen, PDC_UTF8_FLAG, pdc_true);
    }
    return dest;
}
#if defined(_MSC_VER) && defined(_MANAGED)
#pragma managed
#endif


void
pdf_write_destination(PDF *p, pdf_dest *dest)
{
    if (dest->type == nameddest)
    {
        pdf_put_hypertext(p, dest->name);
        pdc_puts(p->out, "\n");
        return;
    }

    pdc_begin_array(p->out);

    if (dest->remote_page)
    {
        pdc_printf(p->out, "%d", dest->remote_page - 1);   /* zero-based */
    }
    else
    {
        if (dest->page == PDC_BAD_ID)
	    dest->page = pdf_get_page_id(p, dest->pgnum);

        pdc_objref_c(p->out, dest->page);
    }

    switch (dest->type) {

        case fixed:
        pdc_puts(p->out, "/XYZ ");

        if (dest->left != -1)
            pdc_printf(p->out, "%f ", dest->left);
        else
            pdc_puts(p->out, "null ");

        if (dest->top != -1)
            pdc_printf(p->out, "%f ", dest->top);
        else
            pdc_puts(p->out, "null ");

        if (dest->zoom != -1)
            pdc_printf(p->out, "%f", dest->zoom);
        else
            pdc_puts(p->out, "null");

        break;

        case fitwindow:
        pdc_puts(p->out, "/Fit");
        break;

        case fitwidth:
        pdc_printf(p->out, "/FitH %f", dest->top);
        break;

        case fitheight:
        pdc_printf(p->out, "/FitV %f", dest->left);
        break;

        case fitrect:
        pdc_printf(p->out, "/FitR %f %f %f %f",
            dest->left, dest->bottom, dest->right, dest->top);
        break;

        case fitvisible:
        pdc_puts(p->out, "/FitB");
        break;

        case fitvisiblewidth:
        pdc_printf(p->out, "/FitBH %f", dest->top);
        break;

        case fitvisibleheight:
        pdc_printf(p->out, "/FitBV %f", dest->left);
        break;

        default:
        break;
    }

    pdc_end_array(p->out);
}

void
pdf__add_nameddest(
    PDF *p,
    const char *name,
    int len,
    const char *optlist)
{
    pdc_resopt *resopts = NULL;
    pdc_text_format hypertextformat = p->hypertextformat;
    pdc_encoding hypertextencoding;
    pdc_id obj_id = PDC_BAD_ID;
    pdc_bool verbose = pdc_true;
    char *name2 = NULL;
    pdf_dest *dest;
    int codepage;
    int inum;

    if (!name)
        pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "name", 0, 0, 0);

    resopts = pdc_parse_optionlist(p->pdc, optlist,
                                   pdf_destination_options, NULL, verbose);

    hypertextencoding =
        pdf_get_hypertextencoding_opt(p, resopts, &codepage, verbose);

    if (pdc_get_optvalues("hypertextformat", resopts, &inum, NULL))
    {
        hypertextformat = (pdc_text_format) inum;
        pdf_check_hypertextformat(p, hypertextformat);
    }

    pdc_cleanup_optionlist(p->pdc, resopts);

    /* create hypertext string */
    name2 = pdf_convert_hypertext(p, name, len, hypertextformat,
                                  hypertextencoding, codepage, &len,
                                  pdc_true, verbose);
    if (name2 == NULL)
        pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "name", 0, 0, 0);

    /* parsing option list */
    dest = pdf_parse_destination_optlist(p, optlist, 0, pdf_nameddest, verbose);

    /* interrupt the content stream if we are on a page */
    if (PDF_GET_STATE(p) == pdf_state_page)
        pdf_end_contents_section(p);

    obj_id = pdc_begin_obj(p->out, PDC_NEW_ID); /* Dest object */
    pdc_begin_dict(p->out);                     /* Destination dict */

    pdc_puts(p->out, "/D");
    pdf_write_destination(p, dest);

    pdc_end_dict(p->out);                       /* Destination dict */
    pdc_end_obj(p->out);                        /* Dest object */

    /* continue the contents stream */
    if (PDF_GET_STATE(p) == pdf_state_page)
        pdf_begin_contents_section(p);

    pdf_cleanup_destination(p, dest);

    /* insert name in tree */
    pdf_insert_name(p, name2, names_dests, obj_id);
}


/* --------------------------  bookmarks  -------------------------- */

static const pdc_defopt pdf_create_bookmark_options[] =
{
    {"hypertextencoding", pdc_stringlist,  PDC_OPT_NONE, 1, 1,
      0.0, PDF_MAX_NAMESTRING, NULL},

    {"hypertextformat", pdc_keywordlist, PDC_OPT_NONE, 1, 1,
      0.0, 0.0, pdf_textformat_keylist},

    {"textcolor", pdc_stringlist, PDC_OPT_NONE, 2, 5,
      0.0, PDF_MAX_NAMESTRING, NULL},

    {"fontstyle", pdc_keywordlist, PDC_OPT_NONE, 1, 1,
      0.0, 0.0, pdf_fontstyle_pdfkeylist},

    {"parent", pdc_bookmarkhandle, PDC_OPT_NONE, 1, 1,
      0.0, 0.0, NULL},

    {"index", pdc_integerlist, PDC_OPT_NONE, 1, 1,
      -1, PDC_INT_MAX, NULL},

    {"open", pdc_booleanlist, PDC_OPT_NONE, 1, 1,
      0.0, 0.0, NULL},

    {"destination", pdc_stringlist, PDC_OPT_NONE, 1, 1,
      0.0, PDF_MAXSTRINGSIZE, NULL},

    {"destname", pdc_stringlist, PDC_OPT_IGNOREIF1, 1, 1,
      0.0, PDF_MAXSTRINGSIZE, NULL},

    {"action", pdc_stringlist, PDC_OPT_NONE, 1, 1,
      0.0, PDF_MAXSTRINGSIZE, NULL},

    PDC_OPT_TERMINATE
};

struct pdf_outline_s {
    pdc_id              obj_id;         /* id of this outline object */
    char                *text;          /* bookmark text */
    int                 count;          /* number of open sub-entries */
    pdc_bool            open;           /* whether or not to display children */
    pdc_scalar          textcolor[3];   /* rgb color of bookmark text */
    pdc_fontstyle       fontstyle;      /* font style of bookmark text */
    char                *action;        /* action optlist */
    pdf_dest            *dest;          /* outline destination */

    /* these members control automatic ordering of bookmarks.
    */
    pdc_bool		in_order;	/* this book mark is "in order" */
    pdc_id		page_id;	/* id of page where this bookmark */
					/*   was defined */

    /* the members below are indices into the p->outlines[] array.
    */
    int                 prev;           /* previous entry at this level */
    int                 next;           /* next entry at this level */
    int                 parent;         /* ancestor's index */
    int                 first;          /* first sub-entry */
    int                 last;           /* last sub-entry */
};

static void
pdf_init_outline(PDF *p, pdf_outline *outline)
{
    (void) p;

    outline->obj_id = PDC_BAD_ID;
    outline->text = NULL;
    outline->count = 0;
    outline->open = pdc_false;
    outline->textcolor[0] = 0.0;
    outline->textcolor[1] = 0.0;
    outline->textcolor[2] = 0.0;
    outline->fontstyle = pdc_Normal;
    outline->action = NULL;
    outline->dest = NULL;
    outline->in_order = pdc_false;
    outline->page_id = PDC_BAD_ID;
    outline->prev = 0;
    outline->next = 0;
    outline->parent = 0;
    outline->first = 0;
    outline->last = 0;
}

/* We can't work with pointers in the outline objects because
 * the complete outline block may be reallocated. Therefore we use
 * this simple mechanism for achieving indirection.
 */
#define COUNT(jndex)    (p->outlines[jndex].count)
#define OPEN(jndex)     (p->outlines[jndex].open)
#define IN_ORDER(jndex) (p->outlines[jndex].in_order)
#define PAGE_ID(jndex)  (p->outlines[jndex].page_id)
#define LAST(jndex)     (p->outlines[jndex].last)
#define PARENT(jndex)   (p->outlines[jndex].parent)
#define FIRST(jndex)    (p->outlines[jndex].first)
#define OBJ_ID(jndex)   (p->outlines[jndex].obj_id)
#define PREV(jndex)     (p->outlines[jndex].prev)
#define NEXT(jndex)     (p->outlines[jndex].next)

static int
search_forward(PDF *p, int start_page, int start_index)
{
    int idx;

    for (idx = start_index; idx != 0; idx = NEXT(idx))
    {
	if (IN_ORDER(idx))
	    return pdf_search_page_fwd(p, start_page, PAGE_ID(idx));
    }

    return PDC_INT_MAX;
}

static int
search_backward(PDF *p, int start_page, int start_index)
{
    int idx;

    for (idx = start_index; idx != 0; idx = PREV(idx))
    {
	if (IN_ORDER(idx))
	{
	    int pg = pdf_search_page_bwd(p, start_page, PAGE_ID(idx));

            return (pg == -1) ? PDC_INT_MAX : pg;
	}
    }

    return -1;
}

static int
pdf_insert_bookmark(
    PDF *p,
    const char *hypertext,
    pdf_outline *outline,
    int jndex)
{
    static const char fn[] = "pdf_insert_bookmark";
    pdf_outline *root, *self;
    int parent;
    int self_idx;
    int pageno = pdf_current_page(p);

    /* allocation */
    if (p->outline_count == 0)
    {
        p->outline_capacity = OUTLINE_CHUNKSIZE;
        p->outlines = (pdf_outline *) pdc_calloc(p->pdc,
                          sizeof(pdf_outline) * p->outline_capacity, fn);

        /* populate the root outline object */
        root = &p->outlines[0];
        pdf_init_outline(p, root);
        root->obj_id = pdc_alloc_id(p->out);
        root->open = pdc_true;

        /* set the open mode show bookmarks if we have at least one,
         * and the client didn't already set his own open mode.
         */
        pdf_fix_openmode(p);
    }
    else if (p->outline_count + 1 >= p->outline_capacity)
    {
        p->outline_capacity *= 2;
        p->outlines = (pdf_outline *) pdc_realloc(p->pdc, p->outlines,
                          sizeof(pdf_outline) * p->outline_capacity, fn);
    }

    /* copy */
    self_idx = ++p->outline_count;
    self = &p->outlines[self_idx];
    memcpy(self, outline, sizeof(pdf_outline));

    self->obj_id = pdc_alloc_id(p->out);
    self->text = (char *) hypertext;
    self->page_id = pdf_get_page_id(p, 0);
    parent = self->parent;

    /* default destination */
    if (self->action == NULL && self->dest == NULL)
        self->dest = pdf_init_destination(p);

    /* no destination */
    if (self->dest != NULL &&
        self->dest->name != NULL && !strlen(self->dest->name))
    {
        pdf_cleanup_destination(p, self->dest);
        self->dest = NULL;
    }

    /* current page */
    if (self->dest)
    {
	/* this ugly code is for compatibility with the
	** obsolete "bookmarkdest" parameter.
	*/
        if (self->dest->pgnum == 0)
            self->dest->pgnum = pdf_current_page(p);

        if (self->dest->pgnum == 0)
	{
            self->dest->pgnum = 1;
	}
	else if (self->dest->page == PDC_BAD_ID)
	{
            self->dest->page = pdf_get_page_id(p, self->dest->pgnum);
	}
    }

    /* special case: empty list.
    */
    if (FIRST(parent) == 0)
    {
        if (jndex > 0)
	    pdc_error(p->pdc, PDC_E_OPT_ILLINTEGER, "index",
                pdc_errprintf(p->pdc, "%d", jndex), 0, 0);

        FIRST(parent) = LAST(parent) = self_idx;
	self->in_order = pdc_true;
    }
    else switch (jndex)
    {
	case -2:	/* insert "in order" */
	{
	    /* the "natural" case: append to the end if appropriate.
	    */
	    if (pageno >= search_backward(p, -1, LAST(parent)))
	    {
		self->prev = LAST(parent);
		NEXT(LAST(parent)) = self_idx;
		LAST(parent) = self_idx;
	    }
	    else
	    {
		int idx;
		int curr_pg = 1;
		int next_pg;

		for (idx = FIRST(parent); idx != 0; idx = NEXT(idx))
		{
		    if (!IN_ORDER(idx))
			continue;

		    next_pg = pdf_search_page_fwd(p, curr_pg, PAGE_ID(idx));

		    /* TODO: understand why this can happen.
		    */
		    if (next_pg < 1)
		    {
			idx = 0;
			break;
		    }

		    if (next_pg > pageno)
		    {
			self->next = idx;
			self->prev = PREV(idx);
			PREV(idx) = self_idx;

			if (self->prev == 0)
			    FIRST(parent) = self_idx;
			else
			    NEXT(self->prev) = self_idx;

			break;
		    }

		    curr_pg = next_pg;
		}

		/* if there are no "in order" bookmarks yet,
		** we simply append this one to the end.
		*/
		if (idx == 0)
		{
		    self->prev = LAST(parent);
		    NEXT(LAST(parent)) = self_idx;
		    LAST(parent) = self_idx;
		}
	    }

	    self->in_order = pdc_true;
	    break;
	}

	case -1:	/* append to the end */
	{
	    self->prev = LAST(parent);
	    NEXT(LAST(parent)) = self_idx;
	    LAST(parent) = self_idx;

	    self->in_order =
		(pageno >= search_backward(p, pageno, self->prev));
	    break;
	}

	case 0:		/* insert at the beginning */
	{
	    self->next = FIRST(parent);
	    PREV(FIRST(parent)) = self_idx;
	    FIRST(parent) = self_idx;

	    self->in_order =
		(pageno <= search_forward(p, pageno, self->next));
	    break;
	}

	default:	/* insert before [1..LAST] */
	{
	    int i;
	    int target = FIRST(parent);

            for (i = 0; i < jndex; ++i)
	    {
		if (target == LAST(parent))
		    pdc_error(p->pdc, PDC_E_OPT_ILLINTEGER, "index",
                        pdc_errprintf(p->pdc, "%d", jndex), 0, 0);

		target = NEXT(target);
	    }

	    self->next = target;
	    self->prev = PREV(target);
	    NEXT(self->prev) = PREV(self->next) = self_idx;

	    self->in_order =
		((pageno >= search_backward(p, pageno, self->prev)) &&
		(pageno <= search_forward(p, pageno, self->next)));
	    break;
	}
    } /* else switch */

    /* increase the number of open sub-entries for all relevant ancestors */
    do {
        COUNT(parent)++;
    } while (OPEN(parent) && (parent = PARENT(parent)) != 0);

    return (self_idx);          /* caller may use this as handle */
}

int
pdf__create_bookmark(PDF *p, const char *text, int len, const char *optlist)
{
    pdc_resopt *resopts = NULL;
    pdc_clientdata data;
    pdf_outline self;
    pdf_dest *dest = NULL;
    pdc_text_format hypertextformat;
    pdc_encoding hypertextencoding;
    pdf_coloropt textcolor;
    char *hypertext = NULL;
    const char *keyword = NULL;
    pdc_bool verbose;
    char **strlist = NULL;
    int codepage = p->hypertextcodepage;
    int ns, inum, outlen, retval = 0;
    int jndex = -2;

    /* Initialize */
    pdf_init_outline(p, &self);
    hypertextformat = p->hypertextformat;
    hypertextencoding = p->hypertextencoding;
    verbose = pdc_true;

    /* Parsing option list */
    if (optlist && strlen(optlist))
    {
        pdf_set_clientdata(p, &data);
        resopts = pdc_parse_optionlist(p->pdc, optlist,
                      pdf_create_bookmark_options, &data, verbose);

        hypertextencoding =
            pdf_get_hypertextencoding_opt(p, resopts, &codepage, verbose);

        if (pdc_get_optvalues("hypertextformat", resopts, &inum, NULL))
        {
            hypertextformat = (pdc_text_format) inum;
            pdf_check_hypertextformat(p, hypertextformat);
        }

        ns = pdc_get_optvalues("textcolor", resopts, NULL, &strlist);
        if (ns)
        {
            pdf_parse_coloropt(p, "textcolor", strlist, ns, (int) color_rgb,
                               &textcolor);
            self.textcolor[0] = textcolor.value[0];
            self.textcolor[1] = textcolor.value[1];
            self.textcolor[2] = textcolor.value[2];
        }

        if (pdc_get_optvalues("fontstyle", resopts, &inum, NULL))
            self.fontstyle = (pdc_fontstyle) inum;

        pdc_get_optvalues("parent", resopts, &self.parent, NULL);

        pdc_get_optvalues("index", resopts, &jndex, NULL);

        pdc_get_optvalues("open", resopts, &self.open, NULL);

        if (pdc_get_optvalues("destination", resopts, NULL, &strlist))
        {
            self.dest = pdf_parse_destination_optlist(p, strlist[0], 0,
                            pdf_bookmark, verbose);
            keyword = "destination";
        }

        dest = pdf_get_option_destname(p, resopts, hypertextencoding);
        if (dest)
        {
            self.dest = dest;
            keyword = "destname";
        }

        if (pdc_get_optvalues("action", resopts, NULL, &strlist))
        {
            if (self.dest)
            {
                pdf_cleanup_destination(p, self.dest);
                self.dest = NULL;
                if (verbose)
                    pdc_warning(p->pdc, PDC_E_OPT_IGNORE, keyword, "action",
                                0, 0);
            }

            /* parsing of action list */
            pdf_parse_and_write_actionlist(p, event_bookmark, NULL,
                                           (const char *) strlist[0]);
            self.action =
                (char *) pdc_save_lastopt(resopts, PDC_OPT_SAVE1ELEM);
        }

        pdc_cleanup_optionlist(p->pdc, resopts);
    }

    /* create hypertext string */
    hypertext = pdf_convert_hypertext(p, text, len, hypertextformat,
                                      hypertextencoding, codepage, &outlen,
                                      PDC_UTF8_FLAG, verbose);
    if (hypertext)
        retval = pdf_insert_bookmark(p, hypertext, &self, jndex);

    return retval;
}

static void
pdf_write_outline_dict(PDF *p, int entry)
{
    pdf_outline *outline = &p->outlines[entry];
    pdc_id act_idlist[PDF_MAX_EVENTS];

    /* write action objects */
    if (outline->action)
        pdf_parse_and_write_actionlist(p, event_bookmark, act_idlist,
                                       (const char *) outline->action);

    pdc_begin_obj(p->out, OBJ_ID(entry));   /* outline object */
    pdc_begin_dict(p->out);

    pdc_objref(p->out, "/Parent", OBJ_ID(PARENT(entry)));

    /* outline destination */
    if (outline->dest)
    {
        pdc_puts(p->out, "/Dest");
        pdf_write_destination(p, outline->dest);
    }

    /* write Action entries */
    else if (outline->action)
        pdf_write_action_entries(p, event_bookmark, act_idlist);

    pdc_puts(p->out, "/Title"); /* outline text */
    pdf_put_hypertext(p, outline->text);
    pdc_puts(p->out, "\n");

    if (PREV(entry))
        pdc_objref(p->out, "/Prev", OBJ_ID(PREV(entry)));
    if (NEXT(entry))
        pdc_objref(p->out, "/Next", OBJ_ID(NEXT(entry)));

    if (FIRST(entry)) {
        pdc_objref(p->out, "/First", OBJ_ID(FIRST(entry)));
        pdc_objref(p->out, "/Last", OBJ_ID(LAST(entry)));
    }
    if (COUNT(entry)) {
        if (OPEN(entry))
            pdc_printf(p->out, "/Count %d\n", COUNT(entry));    /* open */
        else
            pdc_printf(p->out, "/Count %d\n", -COUNT(entry));/* closed */
    }

    /* Color */
    if (outline->textcolor[0] != 0.0 ||
        outline->textcolor[1] != 0.0 ||
        outline->textcolor[2] != 0.0)
        pdc_printf(p->out, "/C[%f %f %f]\n", outline->textcolor[0],
                                              outline->textcolor[1],
                                              outline->textcolor[2]);

    /* FontStyle */
    if (outline->fontstyle != pdc_Normal)
    {
        int fontstyle = 0;
        if (outline->fontstyle == pdc_Bold)
            fontstyle = 2;
        if (outline->fontstyle == pdc_Italic)
            fontstyle = 1;
        if (outline->fontstyle == pdc_BoldItalic)
            fontstyle = 3;
        pdc_printf(p->out, "/F %d\n", fontstyle);
    }

    pdc_end_dict(p->out);
    pdc_end_obj(p->out);                        /* outline object */
}

void
pdf_write_outlines(PDF *p)
{
    int i;

    if (p->outline_count == 0)          /* no outlines: return */
        return;

    pdc_begin_obj(p->out, p->outlines[0].obj_id); /* root outline object */
    pdc_begin_dict(p->out);

    if (p->outlines[0].count != 0)
        pdc_printf(p->out, "/Count %d\n", COUNT(0));
    pdc_objref(p->out, "/First", OBJ_ID(FIRST(0)));
    pdc_objref(p->out, "/Last", OBJ_ID(LAST(0)));

    pdc_end_dict(p->out);
    pdc_end_obj(p->out);                        /* root outline object */

#define PDF_FLUSH_AFTER_MANY_OUTLINES   1000    /* ca. 50-100 KB */
    for (i = 1; i <= p->outline_count; i++) {
        /* reduce memory usage for many outline entries */
        if (i % PDF_FLUSH_AFTER_MANY_OUTLINES == 0)
            pdc_flush_stream(p->out);

        pdf_write_outline_dict(p, i);
    }
}

void
pdf_write_outline_root(PDF *p)
{
    if (p->outline_count != 0)
        pdc_objref(p->out, "/Outlines", p->outlines[0].obj_id);
}

void
pdf_init_outlines(PDF *p)
{
    p->outline_count = 0;
}

/* Free outline entries */
void
pdf_cleanup_outlines(PDF *p)
{
    int i;

    if (!p->outlines || p->outline_count == 0)
        return;

    /* outlines[0] is the outline root object */
    for (i = 0; i <= p->outline_count; i++)
    {
        if (p->outlines[i].text)
        {
            pdc_free(p->pdc, p->outlines[i].text);
            p->outlines[i].text = NULL;
        }
        if (p->outlines[i].action)
        {
            pdc_free(p->pdc, p->outlines[i].action);
            p->outlines[i].action = NULL;
        }
        pdf_cleanup_destination(p, p->outlines[i].dest);
        p->outlines[i].dest = NULL;
    }

    pdc_free(p->pdc, (void*) p->outlines);

    p->outlines = NULL;
}


/*****************************************************************************/
/**              deprecated historical bookmark function                    **/
/*****************************************************************************/

int
pdf__add_bookmark(PDF *p, const char *text, int len, int parent, int open)
{
    pdf_outline self;
    pdf_dest *dest = (pdf_dest *) p->bookmark_dest;
    char *hypertext = NULL;
    int acthdl;
    int retval = 0;

    pdf_init_outline(p, &self);

    if (parent != 0)
        pdf_check_handle(p, parent, pdc_bookmarkhandle);
    self.parent = parent;
    self.open = open;

    /* creating a Launch action - defined via bookmarkdest */
    if (dest->filename)
    {
        char actoptlist[2048];

        sprintf(actoptlist, "filename {%s} ", dest->filename);
        acthdl = pdf__create_action(p, "Launch", actoptlist);
        if (acthdl != -1)
        {
            if (p->hastobepos) acthdl++;
            sprintf(actoptlist, "activate %d", acthdl);
            self.action = pdc_strdup(p->pdc, actoptlist);
        }
    }
    else
    {
        self.dest = pdf_init_destination(p);
        *self.dest = *dest;
        if (dest->name)
            self.dest->name = pdc_strdup(p->pdc, dest->name);
    }

    memcpy(self.textcolor, dest->color, 3 * sizeof(pdc_scalar));
    self.fontstyle = dest->fontstyle;

    hypertext = pdf_convert_hypertext_depr(p, text, len);
    if (hypertext)
        retval = pdf_insert_bookmark(p, hypertext, &self, -1);

    return retval;
}

/* --------------------------  document info  ------------------------------- */

struct pdf_info_s
{
    char                *key;           /* ASCII string */
    char                *value;         /* Unicode string */
    pdf_info            *next;          /* next info entry */
};

void
pdf_cleanup_info(PDF *p)
{
    pdf_info *info, *last;

    if (p->userinfo)
    {
        for (info = p->userinfo; info != NULL; /* */)
        {
            last = info;
            info = info->next;

            pdc_free(p->pdc, last->key);
            pdc_free(p->pdc, last->value);
            pdc_free(p->pdc, last);
        }

        p->userinfo = NULL;
    }
}

static pdf_info *
pdf_have_infokey(PDF *p, const char *key)
{
    pdf_info *info;

    for (info = p->userinfo; info != NULL; info = info->next)
    {
        if (strlen(info->key) == strlen(key) && !strcmp(info->key, key))
            return info;
    }

    return NULL;
}

void
pdf_feed_digest_info(PDF *p)
{
    pdf_info *info;

    if (p->userinfo)
    {
        for (info = p->userinfo; info != NULL; info = info->next)
        {
            pdc_update_digest(p->out,
                (unsigned char *) info->key, strlen(info->key));
        }
    }
}

#define PDF_TRAPPED_TRUE      "True"
#define PDF_TRAPPED_FALSE     "False"
#define PDF_TRAPPED_UNKNOWN   "Unknown"

/* Set Info dictionary entries */
void
pdf__set_info(PDF *p, const char *key, const char *value, int len)
{
    static const char fn[] = "pdf__set_info";
    char *key_buf, *val_buf;
    pdf_info *oldentry, *newentry;

    if (key == NULL || !*key)
        pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "key", 0, 0, 0);

    if (!strcmp(key, "Producer") || !strcmp(key, "CreationDate") ||
        !strcmp(key, "ModDate"))
        pdc_error(p->pdc, PDC_E_ILLARG_STRING, "key", key, 0, 0);

    /* converting key */
    key_buf = pdf_convert_name(p, key, 0, pdc_false);

    /* convert text string */
    val_buf = pdf_convert_hypertext_depr(p, value, len);
    if (!val_buf)
        pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "value", 0, 0, 0);

    /* special handling required for "Trapped" */
    if (!strcmp(key_buf, "Trapped"))
    {
        if (strcmp(val_buf, PDF_TRAPPED_TRUE) &&
            strcmp(val_buf, PDF_TRAPPED_FALSE) &&
            strcmp(val_buf, PDF_TRAPPED_UNKNOWN))
        {
            pdc_free(p->pdc, val_buf);
            pdc_free(p->pdc, key_buf);
            pdc_error(p->pdc, PDC_E_PAR_ILLPARAM, value, key, 0, 0);
        }
    }

    oldentry = pdf_have_infokey(p, key_buf);
    if (oldentry != NULL)
    {
        pdc_free(p->pdc, key_buf);
        pdc_free(p->pdc, oldentry->value);
        oldentry->value = val_buf;
    }
    else
    {
        newentry = (pdf_info *)
            pdc_malloc(p->pdc, sizeof(pdf_info), fn);
        newentry->key  = key_buf;
        newentry->value = val_buf;
        newentry->next = p->userinfo;

        /* ordering doesn't matter so we insert at the beginning */
        p->userinfo = newentry;
    }
}


pdc_id
pdf_write_info(PDF *p)
{
    char time_str[PDC_TIME_SBUF_SIZE];
    char producer[256];
    pdf_info    *info;
    pdc_id      info_id;
    char *      product = "PDFlib Lite";

    info_id = pdc_begin_obj(p->out, PDC_NEW_ID);        /* Info object */

    pdc_begin_dict(p->out);

    /*
     * Although it would be syntactically correct, we must not remove
     * the space characters after the dictionary keys since this
     * would break the PDF properties feature in Windows Explorer.
     */

    if (p->userinfo)
    {
        for (info = p->userinfo; info != NULL; info = info->next)
        {
            pdf_put_pdfname(p, info->key);
            pdc_puts(p->out, " ");

            if (strcmp(info->key, "Trapped"))
                pdf_put_hypertext(p, info->value);
            else
                pdf_put_pdfname(p, info->value);

            pdc_puts(p->out, "\n");
        }
    }

    pdc_get_timestr(time_str);

    pdc_puts(p->out, "/CreationDate ");
    pdf_put_hypertext(p, time_str);
    pdc_puts(p->out, "\n");


    /*
     * If you change the /Producer entry your license to use
     * PDFlib will be void!
     */

    if (p->binding)
        sprintf(producer, "%s %s (%s/%s)", product,
            PDFLIB_VERSIONSTRING, p->binding, PDF_PLATFORM);
    else
        sprintf(producer, "%s %s (%s)", product,
            PDFLIB_VERSIONSTRING, PDF_PLATFORM);

    pdc_puts(p->out, "/Producer ");
    pdf_put_hypertext(p, producer);
    pdc_puts(p->out, "\n");

    pdc_end_dict(p->out);
    pdc_end_obj(p->out);                        /* Info object */

    return info_id;
}

