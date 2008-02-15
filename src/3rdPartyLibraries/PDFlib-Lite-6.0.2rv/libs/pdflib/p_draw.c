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

/* $Id: p_draw.c,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * PDFlib drawing routines
 *
 */

#include "p_intern.h"
#include "p_layer.h"
#include "p_tagged.h"

/* Path segment operators */

static void
pdf_begin_path(PDF *p)
{
    if (PDF_GET_STATE(p) == pdf_state_path)
	return;




    pdf_end_text(p);
    PDF_PUSH_STATE(p, "pdf_begin_path", pdf_state_path);
}

static void
pdf_end_path(PDF *p)
{
    pdf_ppt *ppt = p->curr_ppt;

    PDF_POP_STATE(p, "pdf_end_path");

    ppt->gstate[ppt->sl].x = 0;
    ppt->gstate[ppt->sl].y = 0;
}

/* ----------------- Basic functions for API functions --------------*/

void
pdf__moveto(PDF *p, pdc_scalar x, pdc_scalar y)
{
    pdf_ppt *ppt = p->curr_ppt;

    ppt->gstate[ppt->sl].startx = ppt->gstate[ppt->sl].x = x;
    ppt->gstate[ppt->sl].starty = ppt->gstate[ppt->sl].y = y;

    pdf_begin_path(p);
    pdc_printf(p->out, "%f %f m\n", x, y);
}

void
pdf__rmoveto(PDF *p, pdc_scalar x, pdc_scalar y)
{
    pdf_ppt *	ppt = p->curr_ppt;
    pdc_scalar  x_0 = ppt->gstate[ppt->sl].x;
    pdc_scalar  y_0 = ppt->gstate[ppt->sl].y;

    pdf__moveto(p, x_0 + x, y_0 + y);
}

void
pdf__lineto(PDF *p, pdc_scalar x, pdc_scalar y)
{
    pdf_ppt *ppt = p->curr_ppt;

    pdc_printf(p->out, "%f %f l\n", x, y);

    ppt->gstate[ppt->sl].x = x;
    ppt->gstate[ppt->sl].y = y;
}

void
pdf__rlineto(PDF *p, pdc_scalar x, pdc_scalar y)
{
    pdf_ppt *	ppt = p->curr_ppt;
    pdc_scalar  x_0 = ppt->gstate[ppt->sl].x;
    pdc_scalar  y_0 = ppt->gstate[ppt->sl].y;

    pdf__lineto(p, x_0 + x, y_0 + y);
}

void
pdf__curveto(PDF *p,
    pdc_scalar x_1, pdc_scalar y_1,
    pdc_scalar x_2, pdc_scalar y_2,
    pdc_scalar x_3, pdc_scalar y_3)
{
    pdf_ppt *ppt = p->curr_ppt;

    if (x_2 == x_3 && y_2 == y_3)  /* second c.p. coincides with final point */
        pdc_printf(p->out, "%f %f %f %f y\n", x_1, y_1, x_3, y_3);

    else                        /* general case with four distinct points */
        pdc_printf(p->out, "%f %f %f %f %f %f c\n",
                    x_1, y_1, x_2, y_2, x_3, y_3);

    ppt->gstate[ppt->sl].x = x_3;
    ppt->gstate[ppt->sl].y = y_3;
}

void
pdf__rcurveto(PDF *p,
    pdc_scalar x_1, pdc_scalar y_1,
    pdc_scalar x_2, pdc_scalar y_2,
    pdc_scalar x_3, pdc_scalar y_3)
{
    pdf_ppt *	ppt = p->curr_ppt;
    pdc_scalar  x_0 = ppt->gstate[ppt->sl].x;
    pdc_scalar  y_0 = ppt->gstate[ppt->sl].y;

    pdf__curveto(p, x_0 + x_1, y_0 + y_1,
                    x_0 + x_2, y_0 + y_2,
                    x_0 + x_3, y_0 + y_3);
}

void
pdf_rrcurveto(PDF *p,
    pdc_scalar x_1, pdc_scalar y_1,
    pdc_scalar x_2, pdc_scalar y_2,
    pdc_scalar x_3, pdc_scalar y_3)
{
    pdf__rcurveto(p, x_1, y_1,
                     x_1 + x_2, y_1 + y_2,
                     x_1 + x_2 + x_3, y_1 + y_2 + y_3);
}

void
pdf_hvcurveto(PDF *p, pdc_scalar x_1, pdc_scalar x_2,
                       pdc_scalar y_2, pdc_scalar y_3)
{
    pdf_rrcurveto(p, x_1, 0, x_2, y_2, 0, y_3);
}

void
pdf_vhcurveto(PDF *p, pdc_scalar y_1, pdc_scalar x_2,
                       pdc_scalar y_2, pdc_scalar x_3)
{
    pdf_rrcurveto(p, 0, y_1, x_2, y_2, x_3, 0);
}

void
pdf__rect(PDF *p, pdc_scalar x, pdc_scalar y,
                  pdc_scalar width, pdc_scalar height)
{
    pdf_ppt *ppt = p->curr_ppt;

    ppt->gstate[ppt->sl].startx = ppt->gstate[ppt->sl].x = x;
    ppt->gstate[ppt->sl].starty = ppt->gstate[ppt->sl].y = y;

    pdf_begin_path(p);
    pdc_printf(p->out, "%f %f %f %f re\n", x, y, width, p->ydirection * height);
}

/* 4/3 * (1-cos 45°)/sin 45° = 4/3 * sqrt(2) - 1 */
#define ARC_MAGIC       (0.552284749)

static void
pdf_short_arc(PDF *p, pdc_scalar x, pdc_scalar y, pdc_scalar r,
                      pdc_scalar alpha, pdc_scalar beta)
{
    pdc_scalar bcp;
    pdc_scalar cos_alpha, cos_beta, sin_alpha, sin_beta;

    alpha = alpha * PDC_DEG2RAD;
    beta  = beta * PDC_DEG2RAD;

    /* This formula yields ARC_MAGIC for alpha == 0, beta == 90 degrees */
    bcp = (4.0/3 * (1 - cos((beta - alpha)/2)) / sin((beta - alpha)/2));

    sin_alpha = sin(alpha);
    sin_beta = sin(beta);
    cos_alpha = cos(alpha);
    cos_beta = cos(beta);

    pdf__curveto(p,
                x + r * (cos_alpha - bcp * sin_alpha),          /* p1 */
                y + r * (sin_alpha + bcp * cos_alpha),
                x + r * (cos_beta + bcp * sin_beta),            /* p2 */
                y + r * (sin_beta - bcp * cos_beta),
                x + r * cos_beta,                               /* p3 */
                y + r * sin_beta);
}

static void
pdf_orient_arc(PDF *p, pdc_scalar x, pdc_scalar y, pdc_scalar r,
                       pdc_scalar alpha, pdc_scalar beta, pdc_scalar orient)
{
    pdf_ppt *ppt = p->curr_ppt;
    pdc_scalar rad_a = alpha * PDC_DEG2RAD;
    pdc_scalar startx = (x + r * cos(rad_a));
    pdc_scalar starty = (y + r * sin(rad_a));

    if (r < 0)
        pdc_error(p->pdc, PDC_E_ILLARG_FLOAT,
            "r", pdc_errprintf(p->pdc, "%f", r), 0, 0);

    if (PDF_GET_STATE(p) != pdf_state_path)
    {
        pdf__moveto(p, startx, starty);		/* this enters pdf_state_path */
    }
    else if ((ppt->gstate[ppt->sl].x != startx
	     || ppt->gstate[ppt->sl].y != starty))
    {
        pdf__lineto(p, startx, starty);
    }

    if (orient > 0)
    {
        while (beta < alpha)
            beta += 360;

        if (alpha == beta)
            return;

        while (beta - alpha > 90)
        {
            pdf_short_arc(p, x, y, r, alpha, alpha + 90);
            alpha += 90;
        }
    }
    else
    {
        while (alpha < beta)
            alpha += 360;

        if (alpha == beta)
            return;

        while (alpha - beta > 90)
        {
            pdf_short_arc(p, x, y, r, alpha, alpha - 90);
            alpha -= 90;
        }
    }

    if (alpha != beta)
        pdf_short_arc(p, x, y, r, alpha, beta);
}

void
pdf__arc(PDF *p, pdc_scalar x, pdc_scalar y, pdc_scalar r,
                 pdc_scalar alpha, pdc_scalar beta)
{
    pdf_orient_arc(p, x, y, r,
                   p->ydirection * alpha, p->ydirection * beta, p->ydirection);
}

void
pdf__arcn(PDF *p, pdc_scalar x, pdc_scalar y, pdc_scalar r,
                  pdc_scalar alpha, pdc_scalar beta)
{
    pdf_orient_arc(p, x, y, r,
                   p->ydirection * alpha, p->ydirection * beta, -p->ydirection);
}

void
pdf__circle(PDF *p, pdc_scalar x, pdc_scalar y, pdc_scalar r)
{
    if (r < 0)
        pdc_error(p->pdc, PDC_E_ILLARG_FLOAT,
            "r", pdc_errprintf(p->pdc, "%f", r), 0, 0);

    /*
     * pdf_begin_path() not required since we descend to other
     * path segment functions.
     */

    /* draw four Bezier curves to approximate a circle */
    pdf__moveto(p, x + r, y);
    pdf__curveto(p, x + r, y + r*ARC_MAGIC, x + r*ARC_MAGIC, y + r, x, y + r);
    pdf__curveto(p, x - r*ARC_MAGIC, y + r, x - r, y + r*ARC_MAGIC, x - r, y);
    pdf__curveto(p, x - r, y - r*ARC_MAGIC, x - r*ARC_MAGIC, y - r, x, y - r);
    pdf__curveto(p, x + r*ARC_MAGIC, y - r, x + r, y - r*ARC_MAGIC, x + r, y);

    pdf__closepath(p);
}

void
pdf__closepath(PDF *p)
{
    pdf_ppt *ppt = p->curr_ppt;

    pdc_puts(p->out, "h\n");

    ppt->gstate[ppt->sl].x = ppt->gstate[ppt->sl].startx;
    ppt->gstate[ppt->sl].y = ppt->gstate[ppt->sl].starty;
}

void
pdf__endpath(PDF *p)
{
    pdc_puts(p->out, "n\n");
    pdf_end_path(p);
}

void
pdf__stroke(PDF *p)
{
    pdc_puts(p->out, "S\n");
    pdf_end_path(p);
}

void
pdf__closepath_stroke(PDF *p)
{
    pdc_puts(p->out, "s\n");
    pdf_end_path(p);
}

void
pdf__fill(PDF *p)
{
    if (p->curr_ppt->fillrule == pdf_fill_winding)
        pdc_puts(p->out, "f\n");
    else if (p->curr_ppt->fillrule == pdf_fill_evenodd)
        pdc_puts(p->out, "f*\n");

    pdf_end_path(p);
}

void
pdf__fill_stroke(PDF *p)
{
    if (p->curr_ppt->fillrule == pdf_fill_winding)
        pdc_puts(p->out, "B\n");
    else if (p->curr_ppt->fillrule == pdf_fill_evenodd)
        pdc_puts(p->out, "B*\n");

    pdf_end_path(p);
}

void
pdf__closepath_fill_stroke(PDF *p)
{
    if (p->curr_ppt->fillrule == pdf_fill_winding)
        pdc_puts(p->out, "b\n");
    else if (p->curr_ppt->fillrule == pdf_fill_evenodd)
        pdc_puts(p->out, "b*\n");

    pdf_end_path(p);
}

void
pdf__clip(PDF *p)
{
    if (p->curr_ppt->fillrule == pdf_fill_winding)
        pdc_puts(p->out, "W n\n");
    else if (p->curr_ppt->fillrule == pdf_fill_evenodd)
        pdc_puts(p->out, "W* n\n");

    pdf_end_path(p);
}

