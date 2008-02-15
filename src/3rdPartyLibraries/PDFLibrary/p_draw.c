/*---------------------------------------------------------------------------*
 |        PDFlib - A library for dynamically generating PDF documents        |
 +---------------------------------------------------------------------------+
 |        Copyright (c) 1997-2000 Thomas Merz. All rights reserved.          |
 +---------------------------------------------------------------------------+
 |    This software is NOT in the public domain.  It can be used under two   |
 |    substantially different licensing terms:                               |
 |                                                                           |
 |    The commercial license is available for a fee, and allows you to       |
 |    - ship a commercial product based on PDFlib                            |
 |    - implement commercial Web services with PDFlib                        |
 |    - distribute (free or commercial) software when the source code is     |
 |      not made available                                                   |
 |    Details can be found in the file PDFlib-license.pdf.                   |
 |                                                                           |
 |    The "Aladdin Free Public License" doesn't require any license fee,     |
 |    and allows you to                                                      |
 |    - develop and distribute PDFlib-based software for which the complete  |
 |      source code is made available                                        |
 |    - redistribute PDFlib non-commercially under certain conditions        |
 |    - redistribute PDFlib on digital media for a fee if the complete       |
 |      contents of the media are freely redistributable                     |
 |    Details can be found in the file aladdin-license.pdf.                  |
 |                                                                           |
 |    These conditions extend to ports to other programming languages.       |
 |    PDFlib is distributed with no warranty of any kind. Commercial users,  |
 |    however, will receive warranty and support statements in writing.      |
 *---------------------------------------------------------------------------*/

/* p_draw.c
 *
 * PDFlib drawing routines
 *
 */

#include <math.h>

#include "p_intern.h"

/* Path segment operators */

static void
pdf_begin_path(PDF *p)
{
    if (p->contents == c_path)
	return;

    pdf_end_text(p);

    p->contents = c_path;
}

void
pdf_end_path(PDF *p, pdf_bool force_endpath)
{
    if (p->contents != c_path)
	return;

    if (force_endpath)
	pdf_puts(p, "n\n");	/* force an endpath if the caller forgot it */

    p->contents = c_page;

    p->gstate[p->sl].x = (float) 0.0;
    p->gstate[p->sl].y = (float) 0.0;
}

PDFLIB_API void PDFLIB_CALL
PDF_moveto(PDF *p, float x, float y)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE];

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    /* starting a new path */
    if (p->contents != c_path) {
	p->gstate[p->sl].startx = x;
	p->gstate[p->sl].starty = y;
    }

    pdf_begin_path(p);

    pdf_printf(p, "%s %s m\n", pdf_float(buf1, x), pdf_float(buf2, y));

    p->gstate[p->sl].x = x;
    p->gstate[p->sl].y = y;
}

PDFLIB_API void PDFLIB_CALL
PDF_lineto(PDF *p, float x, float y)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE];

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    pdf_begin_path(p);

    pdf_printf(p, "%s %s l\n", pdf_float(buf1, x), pdf_float(buf2, y));

    p->gstate[p->sl].x = x;
    p->gstate[p->sl].y = y;
}

PDFLIB_API void PDFLIB_CALL
PDF_curveto(PDF *p, float x1, float y1, float x2, float y2, float x3, float y3)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE];
    char buf3[FLOATBUFSIZE], buf4[FLOATBUFSIZE];
    char buf5[FLOATBUFSIZE], buf6[FLOATBUFSIZE];

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    /* starting a new path */
    if (p->contents != c_path) {
	p->gstate[p->sl].startx = x1;
	p->gstate[p->sl].starty = y1;
    }

    pdf_begin_path(p);

    if (x2 == x3 && y2 == y3)	/* second c.p. coincides with final point */
	pdf_printf(p, "%s %s %s %s y\n",
		pdf_float(buf1, x1), pdf_float(buf2, y1),
		pdf_float(buf3, x3), pdf_float(buf4, y3));
    else			/* general case with four distinct points */
	pdf_printf(p, "%s %s %s %s %s %s c\n",
		pdf_float(buf1, x1), pdf_float(buf2, y1),
		pdf_float(buf3, x2), pdf_float(buf4, y2),
		pdf_float(buf5, x3), pdf_float(buf6, y3));

    p->gstate[p->sl].x = x3;
    p->gstate[p->sl].y = y3;
}

#define ARC_MAGIC	((float) 0.5523)
#define MIN(x, y)	((x) < (y) ? (x) : (y))

static void
pdf_arc_internal(PDF *p, float x, float y, float r, float alpha, float beta, pdf_bool moveto)
{
    float bcp;
    float cos_alpha, cos_beta, sin_alpha, sin_beta;
    float startx, starty;

    /*
     * pdf_begin_path() not required since we descend to another
     * path segment function in all cases.
     */

    /* The starting point */
    startx = (float) (x + r * cos(M_PI * alpha / 180.0));
    starty = (float) (y + r * sin(M_PI * alpha / 180.0));

    /* move to the beginning of the arc if requested, and if not already there */
    if (moveto && (p->gstate[p->sl].x != startx || p->gstate[p->sl].y != starty))
	PDF_moveto(p, startx, starty);

    if (beta - alpha > 90.0) {
	/* cut down in 90 degree segments until done */
	pdf_arc_internal(p, x, y, r, alpha, MIN(alpha + 90, beta), pdf_false);
	if (alpha + 90 < beta)
	    pdf_arc_internal(p, x, y, r, alpha + 90, beta, pdf_false);
	return;
    }

    /* speed up special case for quadrants and circles */
    if ((int) alpha == alpha && (int) beta == beta &&
    	beta - alpha == 90 && (int) alpha % 90 == 0)
    {
	/* prune angle values */
	alpha = (float) ((int) alpha % 360);

	switch ((int) alpha) {
	    case 0:
		PDF_curveto(p, x + r, y + r*ARC_MAGIC, 
			x + r*ARC_MAGIC, y + r, x, y + r);
		break;
	    case 90:
		PDF_curveto(p, x - r*ARC_MAGIC, y + r, 
			x - r, y + r*ARC_MAGIC, x - r, y);
		break;
	    case 180:
		PDF_curveto(p, x - r, y - r*ARC_MAGIC, 
			x - r*ARC_MAGIC, y - r, x, y - r);
		break;
	    case 270:
		PDF_curveto(p, x + r*ARC_MAGIC, y - r, 
			x + r, y - r*ARC_MAGIC, x + r, y);
		break;
	    default:
		break;
	}
	return;
    }

    alpha = (float) (alpha * M_PI / 180);	/* convert to radians */
    beta  = (float) (beta * M_PI / 180);	/* convert to radians */

    /* This formula yields ARC_MAGIC for alpha == 0, beta == 90 degrees */
    bcp = (float) (4.0/3 * (1 - cos((beta - alpha)/2)) / sin((beta - alpha)/2));
    
    sin_alpha = (float) sin(alpha);
    sin_beta = (float) sin(beta);
    cos_alpha = (float) cos(alpha);
    cos_beta = (float) cos(beta);

    /* new endpoint */
    p->gstate[p->sl].x = x + r * cos_beta;
    p->gstate[p->sl].y = y + r * sin_beta;

    PDF_curveto(p, 
		x + r * (cos_alpha - bcp * sin_alpha),		/* p1 */
		y + r * (sin_alpha + bcp * cos_alpha),
		x + r * (cos_beta + bcp * sin_beta),		/* p2 */
		y + r * (sin_beta - bcp * cos_beta),
		p->gstate[p->sl].x,				/* p3 */
		p->gstate[p->sl].y);
}

PDFLIB_API void PDFLIB_CALL
PDF_arc(PDF *p, float x, float y, float r, float alpha, float beta)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (r < 0)
	pdf_error(p, PDF_ValueError, "Negative arc radius %f", r);
    
    pdf_arc_internal(p, x, y, r, alpha, beta, pdf_true);
}

PDFLIB_API void PDFLIB_CALL
PDF_circle(PDF *p, float x, float y, float r)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (r < 0)
	pdf_error(p, PDF_ValueError, "Negative circle radius %f", r);

    /*
     * pdf_begin_path() not required since we descend to other
     * path segment functions.
     */

    /* draw four Bezier curves to approximate a circle */
    PDF_moveto(p, x + r, y);
    PDF_curveto(p, x + r, y + r*ARC_MAGIC, x + r*ARC_MAGIC, y + r, x, y + r);
    PDF_curveto(p, x - r*ARC_MAGIC, y + r, x - r, y + r*ARC_MAGIC, x - r, y);
    PDF_curveto(p, x - r, y - r*ARC_MAGIC, x - r*ARC_MAGIC, y - r, x, y - r);
    PDF_curveto(p, x + r*ARC_MAGIC, y - r, x + r, y - r*ARC_MAGIC, x + r, y);

    p->gstate[p->sl].x = x + r;
    p->gstate[p->sl].y = y;
}

PDFLIB_API void PDFLIB_CALL
PDF_rect(PDF *p, float x, float y, float width, float height)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE];
    char buf3[FLOATBUFSIZE], buf4[FLOATBUFSIZE];

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    /* starting a new path */
    if (p->contents != c_path) {
	p->gstate[p->sl].startx = x;
	p->gstate[p->sl].starty = y;
    }

    pdf_begin_path(p);

    pdf_printf(p, "%s %s %s %s re\n",
		    pdf_float(buf1, x), pdf_float(buf2, y),
		    pdf_float(buf3, width), pdf_float(buf4, height));

    p->gstate[p->sl].x = x;
    p->gstate[p->sl].y = y;
}

PDFLIB_API void PDFLIB_CALL
PDF_closepath(PDF *p)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    /* This may look strange but is correct */
    pdf_begin_path(p);

    pdf_puts(p, "h\n");

    p->gstate[p->sl].x = p->gstate[p->sl].startx;
    p->gstate[p->sl].y = p->gstate[p->sl].starty;
}

/* Path painting operators */

PDFLIB_API void PDFLIB_CALL
PDF_endpath(PDF *p)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->contents != c_path) {
	pdf_error(p, PDF_NonfatalError, "No path available in PDF_endpath");
	return;
    }

    pdf_end_path(p, pdf_true);
}

PDFLIB_API void PDFLIB_CALL
PDF_stroke(PDF *p)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->contents != c_path) {
	pdf_error(p, PDF_NonfatalError, "No path available in PDF_stroke");
	return;
    }

    pdf_puts(p, "S\n");

    pdf_end_path(p, pdf_false);
}

PDFLIB_API void PDFLIB_CALL
PDF_closepath_stroke(PDF *p)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->contents != c_path) {
	pdf_error(p, PDF_NonfatalError,
		"No path available in PDF_closepath_stroke");
	return;
    }

    pdf_puts(p, "s\n");

    pdf_end_path(p, pdf_false);
}

PDFLIB_API void PDFLIB_CALL
PDF_fill(PDF *p)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->contents != c_path) {
	pdf_error(p, PDF_NonfatalError, "No path available in PDF_fill");
	return;
    }

    switch (p->fillrule) {
	case pdf_fill_winding:
	    pdf_puts(p, "f\n");
	    break;

	case pdf_fill_evenodd:
	    pdf_puts(p, "f*\n");
	    break;

	default:
	    pdf_error(p, PDF_RuntimeError,
		"Inconsistent fill rule value in PDF_fill");
    }

    pdf_end_path(p, pdf_false);
}

PDFLIB_API void PDFLIB_CALL
PDF_fill_stroke(PDF *p)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->contents != c_path) {
	pdf_error(p, PDF_NonfatalError, "No path available in PDF_fill_stroke");
	return;
    }

    switch (p->fillrule) {
	case pdf_fill_winding:
	    pdf_puts(p, "B\n");
	    break;

	case pdf_fill_evenodd:
	    pdf_puts(p, "B*\n");
	    break;

	default:
	    pdf_error(p, PDF_RuntimeError,
		"Inconsistent fill rule value in PDF_fill_stroke");
    }

    pdf_end_path(p, pdf_false);
}

PDFLIB_API void PDFLIB_CALL
PDF_closepath_fill_stroke(PDF *p)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->contents != c_path) {
	pdf_error(p, PDF_NonfatalError,
		"No path available in PDF_closepath_fill_stroke");
	return;
    }

    switch (p->fillrule) {
	case pdf_fill_winding:
	pdf_puts(p, "b\n");
	break;

    case pdf_fill_evenodd:
	pdf_puts(p, "b*\n");
	break;

    default:
	pdf_error(p, PDF_RuntimeError,
	    "Inconsistent fill rule value in PDF_closepath_fill_stroke");
    }

    pdf_end_path(p, pdf_false);
}

/* Path clipping operators */

PDFLIB_API void PDFLIB_CALL
PDF_clip(PDF *p)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->contents != c_path) {
	pdf_error(p, PDF_NonfatalError, "No path available in PDF_clip");
	return;
    }

    pdf_begin_path(p);

    switch (p->fillrule) {
	case pdf_fill_winding:
	pdf_puts(p, "W n\n");

    break;
	case pdf_fill_evenodd:
	pdf_puts(p, "W* n\n");
	break;

    default:
	pdf_error(p, PDF_RuntimeError,
	    "Inconsistent fill rule value in PDF_clip");
	return;
    }

    pdf_end_path(p, pdf_false);
}
