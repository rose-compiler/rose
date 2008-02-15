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

/* $Id: p_gstate.c,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * PDFlib routines dealing with the graphics states
 *
 */

#include "p_intern.h"

/* ---------------------- matrix functions ----------------------------- */

void
pdf_concat_raw(PDF *p, pdc_matrix *m)
{
    if (pdc_is_identity_matrix(m))
	return;

    pdf_end_text(p);

    pdc_printf(p->out, "%f %f %f %f %f %f cm\n",
		    m->a, m->b, m->c, m->d, m->e, m->f);

    pdc_multiply_matrix(m, &p->curr_ppt->gstate[p->curr_ppt->sl].ctm);
}

void
pdf_set_topdownsystem(PDF *p, pdc_scalar height)
{
    if (p->ydirection < 0)
    {
        pdc_matrix m, sm;
        pdc_translation_matrix(0, height, &m);
        pdc_scale_matrix(1, -1, &sm);
        pdc_multiply_matrix(&sm, &m);
        pdf_concat_raw(p, &m);
    }
}

/* -------------------- Special graphics state ---------------------------- */

void
pdf_init_gstate(PDF *p)
{
    pdf_gstate *gs = &p->curr_ppt->gstate[p->curr_ppt->sl];

    pdc_identity_matrix(&gs->ctm);

    gs->x = 0;
    gs->y = 0;

    p->curr_ppt->fillrule = pdf_fill_winding;

    gs->lwidth = 1;
    gs->lcap = 0;
    gs->ljoin = 0;
    gs->miter = 10;
    gs->flatness = -1;	/* -1 means "has not been set" */
    gs->dashed = pdc_false;
}

void
pdf__save(PDF *p)
{
    pdf_ppt *	ppt = p->curr_ppt;
    int		sl = ppt->sl;

    if (sl == PDF_MAX_SAVE_LEVEL - 1)
	pdc_error(p->pdc, PDF_E_GSTATE_SAVELEVEL,
	    pdc_errprintf(p->pdc, "%d", PDF_MAX_SAVE_LEVEL - 1), 0, 0, 0);

    pdf_end_text(p);
    pdc_puts(p->out, "q\n");

    /* propagate states to next level */
    memcpy(&ppt->gstate[sl + 1], &ppt->gstate[sl], sizeof(pdf_gstate));
    pdf_save_cstate(p);
    pdf_save_tstate(p);
    ++ppt->sl;
}

void
pdf__restore(PDF *p)
{
    if (p->curr_ppt->sl == 0)
	pdc_error(p->pdc, PDF_E_GSTATE_RESTORE, 0, 0, 0, 0);

    pdf_end_text(p);

    pdc_puts(p->out, "Q\n");

    p->curr_ppt->sl--;

    pdf_restore_currto(p);
}

void
pdf__translate(PDF *p, pdc_scalar tx, pdc_scalar ty)
{
    pdc_matrix m;

    if (tx == 0 && ty == 0)
	return;

    pdc_translation_matrix(tx, ty, &m);

    pdf_concat_raw(p, &m);
}

void
pdf__scale(PDF *p, pdc_scalar sx, pdc_scalar sy)
{
    pdc_matrix m;

    if (sx == 0)
	pdc_error(p->pdc, PDC_E_ILLARG_FLOAT, "sx", "0", 0, 0);

    if (sy == 0)
	pdc_error(p->pdc, PDC_E_ILLARG_FLOAT, "sy", "0", 0, 0);

    if (sx == 1 && sy == 1)
	return;

    pdc_scale_matrix(sx, sy, &m);

    pdf_concat_raw(p, &m);
}

void
pdf__rotate(PDF *p, pdc_scalar phi)
{
    pdc_matrix m;

    if (phi == 0)
	return;

    pdc_rotation_matrix(p->ydirection * phi, &m);

    pdf_concat_raw(p, &m);
}

void
pdf__skew(PDF *p, pdc_scalar alpha, pdc_scalar beta)
{
    pdc_matrix m;

    if (alpha == 0 && beta == 0)
	return;

    if (alpha > 360 || alpha < -360 ||
	alpha == -90 || alpha == -270 ||
	alpha == 90 || alpha == 270) {
	pdc_error(p->pdc, PDC_E_ILLARG_FLOAT,
	    "alpha", pdc_errprintf(p->pdc, "%f", alpha), 0, 0);
    }

    if (beta > 360 || beta < -360 ||
	beta == -90 || beta == -270 ||
	beta == 90 || beta == 270) {
	pdc_error(p->pdc, PDC_E_ILLARG_FLOAT,
	    "beta", pdc_errprintf(p->pdc, "%f", beta), 0, 0);
    }

    pdc_skew_matrix(p->ydirection * alpha, p->ydirection * beta, &m);

    pdf_concat_raw(p, &m);
}

void
pdf__concat(PDF *p, pdc_scalar a, pdc_scalar b, pdc_scalar c, pdc_scalar d,
            pdc_scalar e, pdc_scalar f)
{
    pdc_matrix m;
    pdc_scalar det = a * d - b * c;

    if (fabs(det) < PDF_SMALLREAL)
	pdc_error(p->pdc, PDC_E_ILLARG_MATRIX,
	    pdc_errprintf(p->pdc, "%f %f %f %f %f %f", a, b, c, d, e, f),
	    0, 0, 0);

    m.a = a;
    m.b = b;
    m.c = c;
    m.d = d;
    m.e = e;
    m.f = f;

    pdf_concat_raw(p, &m);
}

void
pdf_setmatrix_e(PDF *p, pdc_matrix *n)
{
    pdc_matrix m;
    pdc_scalar det = n->a * n->d - n->b * n->c;

    if (fabs(det) < PDF_SMALLREAL)
	pdc_error(p->pdc, PDC_E_ILLARG_MATRIX,
	    pdc_errprintf(p->pdc, "%f %f %f %f %f %f",
		n->a, n->b, n->c, n->d, n->e, n->f),
	    0, 0, 0);

    pdc_invert_matrix(p->pdc, &m, &p->curr_ppt->gstate[p->curr_ppt->sl].ctm);
    pdc_multiply_matrix(n, &m);
    pdf_concat_raw(p, &m);
}


void
pdf__setmatrix(PDF *p, pdc_scalar a, pdc_scalar b, pdc_scalar c, pdc_scalar d,
    pdc_scalar e, pdc_scalar f)
{
    pdc_matrix n;

    n.a = a;
    n.b = b;
    n.c = c;
    n.d = d;
    n.e = e;
    n.f = f;
    pdf_setmatrix_e(p, &n);
}

/* -------------------- General graphics state ---------------------------- */

#define MAX_DASH_LENGTH	8

/* definitions of dash options */
static const pdc_defopt pdf_dashoptions[] =
{
    {"dasharray", pdc_scalarlist, 0, 2, MAX_DASH_LENGTH,
      PDC_FLOAT_PREC, PDC_FLOAT_MAX, NULL},

    {"dashphase", pdc_scalarlist, 0, 1, 1, 0.0, PDC_FLOAT_MAX, NULL},

    PDC_OPT_TERMINATE
};

static void
pdf_setdashpattern(PDF *p, pdc_scalar *darray, int length, pdc_scalar phase)
{
    pdf_gstate *gs = &p->curr_ppt->gstate[p->curr_ppt->sl];

    /* length == 0 or 1 means solid line */
    if (length < 2)
    {
        if (gs->dashed || PDF_FORCE_OUTPUT())
        {
            pdc_puts(p->out, "[] 0 d\n");
            gs->dashed = pdc_false;
        }
    }
    else
    {
        int i;

	pdc_begin_array(p->out);
        for (i = 0; i < length; i++)
        {
            pdc_printf(p->out, "%f ", darray[i]);
        }
	pdc_end_array_c(p->out);
        pdc_printf(p->out, "%f d\n", phase);
        gs->dashed = pdc_true;
    }
}

void
pdf__setdash(PDF *p, pdc_scalar b, pdc_scalar w)
{
    pdc_scalar darray[2];
    int length = 2;

    if (b < 0)
	pdc_error(p->pdc, PDC_E_ILLARG_FLOAT,
	    "b", pdc_errprintf(p->pdc, "%f", b), 0, 0);

    if (w < 0)
	pdc_error(p->pdc, PDC_E_ILLARG_FLOAT,
	    "w", pdc_errprintf(p->pdc, "%f", w), 0, 0);

    /* both zero means solid line */
    if (b == 0.0 && w == 0.0)
    {
        length = 0;
    }
    else
    {
        darray[0] = b;
        darray[1] = w;
    }
    pdf_setdashpattern(p, darray, length, 0);
}

void
pdf__setdashpattern(PDF *p, const char *optlist)
{
    pdc_resopt *results;
    char **strlist;
    pdc_scalar *darray = NULL, phase = 0;
    int length;

    /* parsing optlist */
    results = pdc_parse_optionlist(p->pdc, optlist, pdf_dashoptions, NULL,
                                   pdc_true);

    length = pdc_get_optvalues("dasharray", results, NULL, &strlist);
    darray = (pdc_scalar *) strlist;

    pdc_get_optvalues("dashphase", results, &phase, NULL);

    pdf_setdashpattern(p, darray, length, phase);

    pdc_cleanup_optionlist(p->pdc, results);
}

void
pdf__setflat(PDF *p, pdc_scalar flat)
{
    pdf_gstate *gs = &p->curr_ppt->gstate[p->curr_ppt->sl];

    if (flat < 0.0 || flat > 100.0)
	pdc_error(p->pdc, PDC_E_ILLARG_FLOAT,
	    "flat", pdc_errprintf(p->pdc, "%f", flat), 0, 0);

    if (flat != gs->flatness || PDF_FORCE_OUTPUT())
    {
        gs->flatness = flat;
        pdc_printf(p->out, "%f i\n", flat);
    }
}

void
pdf__setlinejoin(PDF *p, int join)
{
    pdf_gstate *gs = &p->curr_ppt->gstate[p->curr_ppt->sl];
    const int LAST_JOIN = 2;

    if (join < 0 || join > LAST_JOIN)
	pdc_error(p->pdc, PDC_E_ILLARG_INT,
	    "join", pdc_errprintf(p->pdc, "%d", join), 0, 0);

    if (join != gs->ljoin || PDF_FORCE_OUTPUT())
    {
        gs->ljoin = join;
        pdc_printf(p->out, "%d j\n", join);
    }
}

void
pdf__setlinecap(PDF *p, int cap)
{
    pdf_gstate *gs = &p->curr_ppt->gstate[p->curr_ppt->sl];
    const int LAST_CAP = 2;

    if (cap < 0 || cap > LAST_CAP)
	pdc_error(p->pdc, PDC_E_ILLARG_INT,
	    "cap", pdc_errprintf(p->pdc, "%d", cap), 0, 0);

    if (cap != gs->lcap || PDF_FORCE_OUTPUT())
    {
        gs->lcap = cap;
        pdc_printf(p->out, "%d J\n", cap);
    }
}

void
pdf__setmiterlimit(PDF *p, pdc_scalar miter)
{
    pdf_gstate *gs = &p->curr_ppt->gstate[p->curr_ppt->sl];

    if (miter < 1)
	pdc_error(p->pdc, PDC_E_ILLARG_FLOAT,
	    "miter", pdc_errprintf(p->pdc, "%f", miter), 0, 0);

    if (miter != gs->miter || PDF_FORCE_OUTPUT())
    {
        gs->miter = miter;
        pdc_printf(p->out, "%f M\n", miter);
    }
}

void
pdf__setlinewidth(PDF *p, pdc_scalar width)
{
    pdf_gstate *gs = &p->curr_ppt->gstate[p->curr_ppt->sl];

    if (width <= 0)
	pdc_error(p->pdc, PDC_E_ILLARG_FLOAT,
	    "width", pdc_errprintf(p->pdc, "%f", width), 0, 0);

    if (width != gs->lwidth || PDF_FORCE_OUTPUT())
    {
        gs->lwidth = width;
        pdc_printf(p->out, "%f w\n", width);
    }
}

/* reset all gstate parameters except CTM
*/
void
pdf_reset_gstate(PDF *p)
{
    pdf_gstate *gs = &p->curr_ppt->gstate[p->curr_ppt->sl];


    pdf__setcolor(p, "fillstroke", "gray", 0, 0, 0, 0);


    pdf__setlinewidth(p, 1);
    pdf__setlinecap(p, 0);
    pdf__setlinejoin(p, 0);
    pdf__setmiterlimit(p, 10);
    pdf__setdash(p, 0, 0);

    if (gs->flatness != -1)
	pdf__setflat(p, 1);
}

void
pdf__initgraphics(PDF *p)
{
    pdc_matrix inv_ctm;

    pdf_reset_gstate(p);

    pdc_invert_matrix(p->pdc, &inv_ctm,
	&p->curr_ppt->gstate[p->curr_ppt->sl].ctm);
    pdf_concat_raw(p, &inv_ctm);

    /* This also resets the CTM which guards against rounding artifacts. */
    pdf_init_gstate(p);
}
