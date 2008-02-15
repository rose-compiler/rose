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

/* $Id: p_pattern.c,v 1.1 2005/10/11 17:18:15 vuduc2 Exp $
 *
 * PDFlib pattern routines
 *
 */

#include "p_intern.h"
#include "p_color.h"
#include "p_image.h"
#include "p_font.h"

void
pdf_init_pattern(PDF *p)
{
    static const char fn[] = "pdf_init_pattern";
    int i;

    p->pattern_number = 0;
    p->pattern_capacity = PATTERN_CHUNKSIZE;

    p->pattern = (pdf_pattern *) pdc_malloc(p->pdc,
	sizeof(pdf_pattern) * p->pattern_capacity, fn);

    for (i = 0; i < p->pattern_capacity; i++) {
	p->pattern[i].used_on_current_page = pdc_false;
	p->pattern[i].obj_id = PDC_BAD_ID;
    }
}

void
pdf_grow_pattern(PDF *p)
{
    static const char fn[] = "pdf_grow_pattern";
    int i;

    p->pattern = (pdf_pattern *) pdc_realloc(p->pdc, p->pattern,
	sizeof(pdf_pattern) * 2 * p->pattern_capacity, fn);

    for (i = p->pattern_capacity; i < 2 * p->pattern_capacity; i++) {
	p->pattern[i].used_on_current_page = pdc_false;
	p->pattern[i].obj_id = PDC_BAD_ID;
    }

    p->pattern_capacity *= 2;
}

void
pdf_write_page_pattern(PDF *p)
{
    int i, total = 0;

    for (i = 0; i < p->pattern_number; i++)
	if (p->pattern[i].used_on_current_page)
	    total++;

    if (total > 0) {
	pdc_puts(p->out, "/Pattern");

	pdc_begin_dict(p->out);			/* pattern */

	for (i = 0; i < p->pattern_number; i++) {
	    if (p->pattern[i].used_on_current_page) {
		p->pattern[i].used_on_current_page = pdc_false; /* reset */
		pdc_printf(p->out, "/P%d", i);
		pdc_objref(p->out, "", p->pattern[i].obj_id);
	    }
	}

	pdc_end_dict(p->out);			/* pattern */
    }
}

void
pdf_get_page_patterns(PDF *p, pdf_reslist *rl)
{
    int i;

    for (i = 0; i < p->pattern_number; i++) {
	if (p->pattern[i].used_on_current_page) {
	    p->pattern[i].used_on_current_page = pdc_false; /* reset */
	    pdf_add_reslist(p, rl, i);
	}
    }
}

void
pdf_mark_page_pattern(PDF *p, int n)
{
    p->pattern[n].used_on_current_page = pdc_true;
}

void
pdf_cleanup_pattern(PDF *p)
{
    if (p->pattern) {
	pdc_free(p->pdc, p->pattern);
	p->pattern = NULL;
    }
}

/* Start a new pattern definition. */
int
pdf__begin_pattern(
    PDF *p,
    pdc_scalar width,
    pdc_scalar height,
    pdc_scalar xstep,
    pdc_scalar ystep,
    int painttype)
{
    int slot = -1;

    if (width <= 0)
	pdc_error(p->pdc, PDC_E_ILLARG_POSITIVE, "width", 0, 0, 0);

    if (height <= 0)
	pdc_error(p->pdc, PDC_E_ILLARG_POSITIVE, "height", 0, 0, 0);

    if (painttype != 1 && painttype != 2)
	pdc_error(p->pdc, PDC_E_ILLARG_INT,
	    "painttype", pdc_errprintf(p->pdc, "%d", painttype), 0, 0);

    if (p->pattern_number == p->pattern_capacity)
	pdf_grow_pattern(p);

    pdf_pg_suspend(p);
    PDF_SET_STATE(p, pdf_state_pattern);

    p->pattern[p->pattern_number].obj_id = pdc_begin_obj(p->out, PDC_NEW_ID);
    p->pattern[p->pattern_number].painttype = painttype;

    pdc_begin_dict(p->out);				/* pattern dict*/

    p->res_id = pdc_alloc_id(p->out);

    pdc_puts(p->out, "/PatternType 1\n");		/* tiling pattern */

    /* colored or uncolored pattern */
    pdc_printf(p->out, "/PaintType %d\n", painttype);
    pdc_puts(p->out, "/TilingType 1\n");		/* constant spacing */

    pdc_printf(p->out, "/BBox[0 0 %f %f]\n", width, height);

    pdc_printf(p->out, "/XStep %f\n", xstep);
    pdc_printf(p->out, "/YStep %f\n", ystep);

    pdc_objref(p->out, "/Resources", p->res_id);

    p->length_id = pdc_alloc_id(p->out);
    pdc_objref(p->out, "/Length", p->length_id);

    if (pdc_get_compresslevel(p->out))
	pdc_puts(p->out, "/Filter/FlateDecode\n");

    pdc_end_dict(p->out);				/* pattern dict*/
    pdc_begin_pdfstream(p->out);

    slot = p->pattern_number;
    p->pattern_number++;

    /* top-down y-coordinates */
    pdf_set_topdownsystem(p, height);

    return slot;
}

/* Finish the pattern definition. */
void
pdf__end_pattern(PDF *p)
{
    /* check whether pdf__save() and pdf__restore() calls are balanced */
    if (p->curr_ppt->sl > 0)
	pdc_error(p->pdc, PDF_E_GSTATE_UNMATCHEDSAVE, 0, 0, 0, 0);

    pdf_end_text(p);
    pdc_end_pdfstream(p->out);
    pdc_end_obj(p->out);			/* pattern */

    pdc_put_pdfstreamlength(p->out, p->length_id);

    pdc_begin_obj(p->out, p->res_id);		/* Resource object */
    pdc_begin_dict(p->out);			/* Resource dict */

    pdf_write_page_fonts(p);			/* Font resources */

    pdf_write_page_colorspaces(p);		/* Color space resources */

    pdf_write_page_pattern(p);			/* Pattern resources */

    pdf_write_xobjects(p);			/* XObject resources */

    pdf_write_page_extgstates(p);		/* ExtGState resources */

    pdc_end_dict(p->out);			/* resource dict */
    pdc_end_obj(p->out);			/* resource object */

    pdf_pg_resume(p, -1);

    if (p->flush & pdf_flush_content)
	pdc_flush_stream(p->out);
}
