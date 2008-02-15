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

/* $Id: p_xgstate.c,v 1.1 2005/10/11 17:18:15 vuduc2 Exp $
 *
 * Extended graphics state handling
 *
 */

#define P_XGSTATE_C

#include "p_intern.h"

/*  external graphic state  */
struct pdf_extgstateresource_s
{
    pdc_id      obj_id;                 /* object id of this resource */
    pdc_bool    used_on_current_page;   /* this resource used on current page */

    pdc_id      font_obj;               /* font to use */
    pdc_scalar  font_size;              /* at what size */

    pdc_scalar  line_width;
    int         line_cap;
    int         line_join;
    pdc_scalar  miter_limit;
    pdc_scalar* dash_array;
    int         dash_count;
    pdc_scalar  dash_phase;

    pdf_renderingintent       ri;
    pdc_bool    stroke_adjust;
    pdc_bool    overprint_stroke;
    pdc_bool    overprint_fill;
    int         overprint_mode;

    /*
        The following entries which take functions are not implemented
        since PDFlib has no concept of a function at this time.

        BG      - black generation
        BG2     - black generation
        UCR     - undercolor-removal
        UCR2    - undercolor-removal
        TR      - transfer
        TR2     - transfer
        HT      - halftone
    */

    pdc_scalar       flatness;
    pdc_scalar       smoothness;

    /* PDF 1.4 additions */
    pdf_blendmode blendmode;            /* blend mode */
    pdc_scalar  opacity_fill;           /* fill opacity level */
    pdc_scalar  opacity_stroke;         /* stroke opacity level */
    pdc_bool    alpha_is_shape;
    pdc_bool    text_knockout;
};

pdc_id
pdf_get_gstate_id(PDF *p, int gstate)
{
    /* TODO: is this required for ExtGStates used in Shadings? */
    p->extgstates[gstate].used_on_current_page = pdc_true;

    return (p->extgstates[gstate].obj_id);
}

/* Definitions of Explicit Graphics State options */
static const pdc_defopt pdf_create_gstate_options[] =
{
    {"alphaisshape", pdc_booleanlist, PDC_OPT_PDC_1_4, 1, 1, 0.0, 0.0, NULL},

    {"blendmode", pdc_keywordlist, PDC_OPT_BUILDOR | PDC_OPT_PDC_1_4, 1, 20,
      0.0, 0.0, pdf_blendmode_pdfkeylist},

/* These features do not work in Acrobat (5.0.1)
    {"dasharray", pdc_scalarlist, PDC_OPT_PDC_1_3, 1, 8,
      PDF_SMALLREAL, PDC_FLOAT_MAX, NULL},

    {"dashphase", pdc_scalarlist, PDC_OPT_PDC_1_3, 1, 1,
      0.0, PDC_FLOAT_MAX, NULL},

    {"fontsize", pdc_scalarlist, PDC_OPT_PDC_1_3, 1, 1,
      PDF_SMALLREAL, PDC_FLOAT_MAX, NULL},

    {"font", pdc_fonthandle, PDC_OPT_PDC_1_3 | PDC_OPT_REQUIRIF1, 1, 1,
      0, 0, NULL},
*/

    {"flatness", pdc_scalarlist, PDC_OPT_PDC_1_3, 1, 1,
      PDF_SMALLREAL, PDC_FLOAT_MAX, NULL},

    {"linecap", pdc_integerlist, PDC_OPT_PDC_1_3, 1, 1, 0.0, 2.0, NULL},

    {"linejoin", pdc_integerlist, PDC_OPT_PDC_1_3, 1, 1, 0.0, 2.0, NULL},

    {"linewidth", pdc_scalarlist, PDC_OPT_PDC_1_3, 1, 1,
      PDF_SMALLREAL, PDC_FLOAT_MAX, NULL},

    {"miterlimit", pdc_scalarlist, PDC_OPT_PDC_1_3, 1, 1, 1.0, PDC_FLOAT_MAX,
      NULL},

    {"opacityfill", pdc_scalarlist, PDC_OPT_PDC_1_4 | PDC_OPT_PERCENT,
     1, 1, 0.0, 1.0, NULL},

    {"opacitystroke", pdc_scalarlist, PDC_OPT_PDC_1_4 | PDC_OPT_PERCENT,
     1, 1, 0.0, 1.0, NULL},

    {"overprintfill", pdc_booleanlist, PDC_OPT_PDC_1_3, 1, 1, 0.0, 0.0, NULL},

    {"overprintmode", pdc_integerlist, PDC_OPT_PDC_1_3, 1, 1, 0.0, 1.0, NULL},

    {"overprintstroke", pdc_booleanlist, PDC_OPT_PDC_1_3, 1, 1, 0.0, 0.0, NULL},

    {"renderingintent", pdc_keywordlist, PDC_OPT_PDC_1_3, 1, 1, 0.0, 0.0,
      pdf_renderingintent_pdfkeylist},

    {"smoothness", pdc_scalarlist, PDC_OPT_PDC_1_3 | PDC_OPT_PERCENT,
     1, 1, 0.0, 1.0, NULL},

    {"strokeadjust", pdc_booleanlist, PDC_OPT_PDC_1_3, 1, 1, 0.0, 0.0, NULL},

    {"textknockout", pdc_booleanlist, PDC_OPT_PDC_1_4, 1, 1, 0.0, 0.0, NULL},

    PDC_OPT_TERMINATE
};

static void
pdf_init_extgstateresource(pdf_extgstateresource *egsr)
{
    egsr->used_on_current_page	= pdc_false;

    /* we need to tell which parms have been set and which haven't,
    ** so we initialize to invalid values. even boolean parms are
    ** declared as integers, so we can set them to -1 here.
    */
    egsr->font_obj		= PDC_NEW_ID;
    egsr->font_size             = pdc_undef;

    egsr->line_width            = pdc_undef;
    egsr->line_cap              = pdc_undef;
    egsr->line_join             = pdc_undef;
    egsr->miter_limit           = pdc_undef;

    egsr->dash_array		= NULL;
    egsr->dash_count		= 0;
    egsr->dash_phase		= 0.0;

    egsr->ri                    = AutoIntent;
    egsr->stroke_adjust         = pdc_undef;
    egsr->overprint_stroke      = pdc_undef;
    egsr->overprint_fill        = pdc_undef;
    egsr->overprint_mode        = pdc_undef;
    egsr->flatness              = pdc_undef;
    egsr->smoothness            = pdc_undef;

    egsr->blendmode		= BM_None;
    egsr->opacity_stroke        = pdc_undef;
    egsr->opacity_fill          = pdc_undef;
    egsr->alpha_is_shape        = pdc_undef;
    egsr->text_knockout         = pdc_undef;
}

static void
pdf_grow_extgstates(PDF *p)
{
    int i;

    p->extgstates = (pdf_extgstateresource *) pdc_realloc(p->pdc, p->extgstates,
	sizeof(pdf_extgstateresource) * 2 * p->extgstates_capacity,
	"pdf_grow_extgstates");

    for (i = p->extgstates_capacity; i < 2 * p->extgstates_capacity; i++) {
	pdf_init_extgstateresource( &p->extgstates[i] );
    }

    p->extgstates_capacity *= 2;
}

void
pdf_init_extgstates(PDF *p)
{
    static const char fn[] = "pdf_init_extgstates";
    int i;

    p->extgstates_number = 0;
    p->extgstates_capacity = EXTGSTATE_CHUNKSIZE;

    p->extgstates = (pdf_extgstateresource *)
    pdc_malloc(p->pdc,
               sizeof(pdf_extgstateresource) * p->extgstates_capacity, fn);

    for (i = 0; i < p->extgstates_capacity; i++) {
	pdf_init_extgstateresource( &p->extgstates[i] );
    }
}

void
pdf_write_page_extgstates(PDF *p)
{
    int i, total = 0;

    for (i = 0; i < p->extgstates_number; i++)
	if (p->extgstates[i].used_on_current_page)
	    total++;

    if (total > 0) {
	pdc_puts(p->out, "/ExtGState");

	pdc_begin_dict(p->out);			/* ExtGState names */

	for (i = 0; i < p->extgstates_number; i++) {
	    if (p->extgstates[i].used_on_current_page) {
		p->extgstates[i].used_on_current_page = pdc_false; /* reset */
		pdc_printf(p->out, "/GS%d", i);
		pdc_objref(p->out, "", p->extgstates[i].obj_id);
	    }
	}

	pdc_end_dict(p->out);			/* ExtGState names */
    }
}

void
pdf_get_page_extgstates(PDF *p, pdf_reslist *rl)
{
    int i;

    for (i = 0; i < p->extgstates_number; i++) {
	if (p->extgstates[i].used_on_current_page) {
	    p->extgstates[i].used_on_current_page = pdc_false; /* reset */
	    pdf_add_reslist(p, rl, i);
	}
    }
}

void
pdf_mark_page_extgstate(PDF *p, int n)
{
    p->extgstates[n].used_on_current_page = pdc_true;
}

void
pdf_write_doc_extgstates(PDF *p)
{
    int i, j;

    pdf_extgstateresource *gs;

    for (i = 0; i < p->extgstates_number; i++)
    {
	gs = &p->extgstates[i];

	pdc_begin_obj(p->out, gs->obj_id);		/* ExtGState resource */
	pdc_begin_dict(p->out);

	pdc_puts(p->out, "/Type/ExtGState\n");

	if (gs->font_obj != PDC_NEW_ID)
	{
	    pdc_puts(p->out, "/Font");
	    pdc_begin_array(p->out);
	    pdc_objref(p->out, "", gs->font_obj);
	    pdc_printf(p->out, "%f", gs->font_size);
	    pdc_end_array(p->out);
	}

        if (gs->line_width != pdc_undef)
	    pdc_printf(p->out, "/LW %f\n", gs->line_width);

        if (gs->line_cap != pdc_undef)
	    pdc_printf(p->out, "/LC %d\n", gs->line_cap);

        if (gs->line_join != pdc_undef)
	    pdc_printf(p->out, "/LJ %d\n", gs->line_join);

        if (gs->miter_limit != pdc_undef)
	    pdc_printf(p->out, "/ML %f\n", gs->miter_limit);

	if (gs->dash_count > 0)
	{
	    pdc_printf(p->out, "/D");
	    pdc_begin_array(p->out);
	    pdc_begin_array(p->out);

	    for (j = 0; j < gs->dash_count; ++j)
		pdc_printf(p->out, "%f ", gs->dash_array[j]);

	    pdc_end_array_c(p->out);
            pdc_printf(p->out, "%f", gs->dash_phase);
	    pdc_end_array(p->out);
            /* but see page 157 of PDF Reference: integer */
	}

	if (gs->ri != AutoIntent)
            pdc_printf(p->out, "/RI/%s\n",
                pdc_get_keyword((long) gs->ri, pdf_renderingintent_pdfkeylist));

        if (gs->stroke_adjust != pdc_undef)
	    pdc_printf(p->out, "/SA %s\n", PDC_BOOLSTR(gs->stroke_adjust));

        if (gs->overprint_stroke != pdc_undef)
            pdc_printf(p->out, "/OP %s\n", PDC_BOOLSTR(gs->overprint_stroke));

        if (gs->overprint_fill != pdc_undef)
            pdc_printf(p->out, "/op %s\n", PDC_BOOLSTR(gs->overprint_fill));
        else if (gs->overprint_stroke == pdc_true)
            pdc_puts(p->out, "/op false\n");

        if (gs->overprint_mode != pdc_undef)
	    pdc_printf(p->out, "/OPM %d\n", gs->overprint_mode);

        if (gs->flatness != pdc_undef)
	    pdc_printf(p->out, "/FL %f\n", gs->flatness);

        if (gs->smoothness != pdc_undef)
	    pdc_printf(p->out, "/SM %f\n", gs->smoothness);

        if (gs->opacity_fill != pdc_undef)
	    pdc_printf(p->out, "/ca %f\n", gs->opacity_fill);

	if (gs->blendmode != BM_None) {
            const char *modename;

	    pdc_printf(p->out, "/BM");
	    pdc_begin_array(p->out);
	    for (j = 0; ; j++) {
                modename = pdf_blendmode_pdfkeylist[j].word;
                if (!modename) break;
                if (gs->blendmode & pdf_blendmode_pdfkeylist[j].code)
                    pdc_printf(p->out, "/%s", modename);
            }
	    pdc_end_array(p->out);
	}

        if (gs->opacity_stroke != pdc_undef)
	    pdc_printf(p->out, "/CA %f\n", gs->opacity_stroke);

        if (gs->alpha_is_shape != pdc_undef)
	    pdc_printf(p->out, "/AIS %s\n", PDC_BOOLSTR(gs->alpha_is_shape));

        if (gs->text_knockout != pdc_undef)
	    pdc_printf(p->out, "/TK %s\n", PDC_BOOLSTR(gs->text_knockout));

	pdc_end_dict(p->out);
	pdc_end_obj(p->out);			/* ExtGState resource */
    }
}

void
pdf_cleanup_extgstates(PDF *p)
{
    int i;

    if (!p->extgstates)
	return;

    for (i = 0; i < p->extgstates_number; i++) {
        if (p->extgstates[i].dash_array)
            pdc_free(p->pdc, p->extgstates[i].dash_array);
    }

    pdc_free(p->pdc, p->extgstates);
    p->extgstates = NULL;
}

int
pdf__create_gstate(PDF *p, const char *optlist)
{
    pdf_extgstateresource *gs;
    int slot = -1;
    int font = pdc_undef;
    int inum;
    pdc_clientdata data;
    pdc_resopt *results;

    if (optlist == NULL || !*optlist)
        pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "optlist", 0, 0, 0);

    slot = p->extgstates_number;
    if (slot == p->extgstates_capacity)
        pdf_grow_extgstates(p);

    p->extgstates_number++;
    gs = &p->extgstates[slot];
    gs->obj_id = pdc_alloc_id(p->out);

    /* parsing optlist */
    pdf_set_clientdata(p, &data);
    results = pdc_parse_optionlist(p->pdc, optlist, pdf_create_gstate_options,
                                   &data, pdc_true);

    pdc_get_optvalues("alphaisshape", results, &gs->alpha_is_shape, NULL);

    if (pdc_get_optvalues("blendmode", results, &inum, NULL))
        gs->blendmode = (pdf_blendmode) inum;

    gs->dash_count = pdc_get_optvalues("dasharray", results, NULL, NULL);
    gs->dash_array = (pdc_scalar *) pdc_save_lastopt(results, PDC_OPT_SAVEALL);

    pdc_get_optvalues("dashphase", results, &gs->dash_phase, NULL);

    pdc_get_optvalues("flatness", results, &gs->flatness, NULL);

    pdc_get_optvalues("font", results, &font, NULL);
    if (font != pdc_undef)
        gs->font_obj = p->fonts[font].obj_id;

    pdc_get_optvalues("fontsize", results, &gs->font_size, NULL);

    pdc_get_optvalues("linecap", results, &gs->line_cap, NULL);

    pdc_get_optvalues("linejoin", results, &gs->line_join, NULL);

    pdc_get_optvalues("linewidth", results, &gs->line_width, NULL);

    pdc_get_optvalues("miterlimit", results, &gs->miter_limit, NULL);

    pdc_get_optvalues("opacityfill", results, &gs->opacity_fill, NULL);

    pdc_get_optvalues("opacitystroke", results, &gs->opacity_stroke, NULL);

    pdc_get_optvalues("overprintfill", results, &gs->overprint_fill, NULL);

    pdc_get_optvalues("overprintmode", results, &gs->overprint_mode, NULL);

    pdc_get_optvalues("overprintstroke", results, &gs->overprint_stroke, NULL);

    if (pdc_get_optvalues("renderingintent", results, &inum, NULL))
        gs->ri = (pdf_renderingintent) inum;

    pdc_get_optvalues("smoothness", results, &gs->smoothness, NULL);

    pdc_get_optvalues("strokeadjust", results, &gs->stroke_adjust, NULL);

    pdc_get_optvalues("textknockout", results, &gs->text_knockout, NULL);

    pdc_cleanup_optionlist(p->pdc, results);


    return slot;
}

void
pdf__set_gstate(PDF *p, int gstate)
{
    pdf_check_handle(p, gstate, pdc_gstatehandle);

    pdc_printf(p->out, "/GS%d gs\n", gstate);
    p->extgstates[gstate].used_on_current_page = pdc_true;
}
