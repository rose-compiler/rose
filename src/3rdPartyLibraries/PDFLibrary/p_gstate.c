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

/* p_gstate.c
 *
 * PDFlib routines dealing with the graphics states
 *
 */

#include <math.h>
#include <string.h>

#include "p_intern.h"

void
pdf_init_gstate(PDF *p)
{
#ifdef CTM_NYI
    p->gstate[p->sl].ctm.a = (float) 1;
    p->gstate[p->sl].ctm.b = (float) 0;
    p->gstate[p->sl].ctm.c = (float) 0;
    p->gstate[p->sl].ctm.d = (float) 1;
    p->gstate[p->sl].ctm.e = (float) 0.0;
    p->gstate[p->sl].ctm.f = (float) 0.0;
#endif

    p->gstate[p->sl].x = (float) 0.0;
    p->gstate[p->sl].y = (float) 0.0;

    p->fillrule = pdf_fill_winding;
}

/* ---------------------- Utility functions ----------------------------- */

#ifdef CTM_NYI
/* left-multiply M to N and store the result in N */
void
pdf_matrix_mul(pdf_matrix *M, pdf_matrix *N)
{
    pdf_matrix result;

    result.a = N->a * M->a + N->b * M->c;
    result.b = N->a * M->b + N->b * M->d;
    result.c = N->c * M->a + N->d * M->c;
    result.d = N->c * M->b + N->d * M->d;

    result.e = N->e * M->a + N->f * M->c + M->e;
    result.f = N->e * M->b + N->f * M->d + M->f;

    N->a = result.a;
    N->b = result.b;
    N->c = result.c;
    N->d = result.d;
    N->e = result.e;
    N->f = result.f;
}
#endif

/* -------------------- Special graphics state ---------------------------- */

PDFLIB_API void PDFLIB_CALL
PDF_save(PDF *p)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->sl == PDF_MAX_SAVE_LEVEL - 1)
	pdf_error(p, PDF_ValueError, "Too many save levels");

    pdf_end_path(p, pdf_true);
    pdf_end_text(p);

    pdf_puts(p, "q\n");

    /* propagate states to next level */
    p->sl++;
    memcpy(&p->gstate[p->sl], &p->gstate[p->sl - 1], sizeof(pdf_gstate));
    memcpy(&p->tstate[p->sl], &p->tstate[p->sl - 1], sizeof(pdf_tstate));
    memcpy(&p->cstate[p->sl], &p->cstate[p->sl - 1], sizeof(pdf_cstate));
}

PDFLIB_API void PDFLIB_CALL
PDF_restore(PDF *p)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->sl == 0)
	pdf_error(p, PDF_ValueError, "Invalid restore");

    pdf_end_path(p, pdf_true);
    pdf_end_text(p);

    pdf_puts(p, "Q\n");

    p->sl--;
}

void
pdf_concat_raw(PDF *p, pdf_matrix m)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE], buf3[FLOATBUFSIZE];
    char buf4[FLOATBUFSIZE], buf5[FLOATBUFSIZE], buf6[FLOATBUFSIZE];

    pdf_end_path(p, pdf_true);
    pdf_end_text(p);

    pdf_printf(p, "%s %s %s %s %s %s cm\n",
	      pdf_float(buf1, m.a), pdf_float(buf2, m.b), pdf_float(buf3, m.c),
	      pdf_float(buf4, m.d), pdf_float(buf5, m.e), pdf_float(buf6, m.f));
}

/* Convenience routines */

PDFLIB_API void PDFLIB_CALL
PDF_translate(PDF *p, float tx, float ty)
{
    pdf_matrix m;

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (tx == (float) 0 && ty == (float) 0)
	return;

    m.a = (float) 1;
    m.b = (float) 0;
    m.c = (float) 0;
    m.d = (float) 1;
    m.e = tx;
    m.f = ty;

    pdf_concat_raw(p, m);
#ifdef CTM_NYI
    pdf_matrix_mul(m, p->gstate[p->sl].ctm);
#endif
}

PDFLIB_API void PDFLIB_CALL
PDF_scale(PDF *p, float sx, float sy)
{
    pdf_matrix m;

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (sx == (float) 0 || sy == (float) 0)
	pdf_error(p, PDF_ValueError, "Zero scaling factor");

    if (sx == (float) 1 && sy == (float) 1)
	return;

    m.a = sx;
    m.b = (float) 0;
    m.c = (float) 0;
    m.d = sy;
    m.e = (float) 0;
    m.f = (float) 0;

    pdf_concat_raw(p, m);
#ifdef CTM_NYI
    pdf_matrix_mul(m, p->gstate[p->sl].ctm);
#endif
}

PDFLIB_API void PDFLIB_CALL
PDF_rotate(PDF *p, float phi)
{
    pdf_matrix m;
    float c, s;

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (phi == (float) 0)
	return;

    phi = (float) (phi * M_PI / 180);	/* convert to radians */

    c = (float) cos(phi);
    s = (float) sin(phi);

    m.a = (float) c;
    m.b = (float) s;
    m.c = (float) -s;
    m.d = (float) c;
    m.e = (float) 0;
    m.f = (float) 0;

    pdf_concat_raw(p, m);
#ifdef CTM_NYI
    pdf_matrix_mul(m, p->gstate[p->sl].ctm);
#endif
}

PDFLIB_API void PDFLIB_CALL
PDF_skew(PDF *p, float alpha, float beta)
{
    pdf_matrix m;

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (alpha > (float) 360 || alpha < (float) 0 ||
	beta > (float) 360 || beta < (float) 0 ||
	alpha == (float) 90 || alpha == (float) 270 ||
	beta == (float) 90 || beta == (float) 270) {
	pdf_error(p, PDF_ValueError, "Bad skewing values %f, %f", alpha, beta);
    }

    if (alpha == (float) 0 && beta == (float) 0)
	return;

    m.a = (float) 1;
    m.b = (float) tan((float) (alpha * M_PI / 180));
    m.c = (float) tan((float) (beta * M_PI / 180));
    m.d = (float) 1;
    m.e = (float) 0;
    m.f = (float) 0;

    pdf_concat_raw(p, m);
#ifdef CTM_NYI
    pdf_matrix_mul(m, p->gstate[p->sl].ctm);
#endif
}


PDFLIB_API void PDFLIB_CALL
PDF_concat(PDF *p, float a, float b, float c, float d, float e, float f)
{
    pdf_matrix m;

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (a * d == b * c) {
	pdf_error(p, PDF_ValueError, "Degenerate matrix in PDF_concat");
    }

    m.a = (float) a;
    m.b = (float) b;
    m.c = (float) c;
    m.d = (float) d;
    m.e = (float) e;
    m.f = (float) f;

    pdf_concat_raw(p, m);
#ifdef CTM_NYI
    pdf_matrix_mul(m, p->gstate[p->sl].ctm);
#endif
}

/* -------------------- General graphics state ---------------------------- */

PDFLIB_API void PDFLIB_CALL
PDF_setdash(PDF *p, float b, float w)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE];

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (b < (float) 0.0 || w < (float) 0.0) {
	pdf_error(p, PDF_NonfatalError, "Negative dash value %f, %f", b, w);
	return;
    }

    if (p->contents == c_path) {
	pdf_error(p, PDF_NonfatalError,
		"Didn't end the path before PDF_setdash");
	pdf_end_path(p, pdf_true);
    }

    if (b == 0.0 && w == 0.0)		/* both zero means solid line */
	pdf_puts(p, "[] 0 d\n");
    else
	pdf_printf(p, "[%s %s] 0 d\n", pdf_float(buf1, b), pdf_float(buf2, w));
}

PDFLIB_API void PDFLIB_CALL
PDF_setpolydash(PDF *p, float *darray, int length)
{
    char buf[FLOATBUFSIZE];
    int i;

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (length == 0 || length == 1) {	/* length == 0 or 1 means solid line */
	pdf_puts(p, "[] 0 d\n");
	return;
    }

    /* sanity checks */
    if (!darray || length < 0) {
	pdf_error(p, PDF_RuntimeError, "Bad array in PDF_setpolydash");
	return;
    }

    if (length > MAX_DASH_LENGTH) {
	pdf_error(p, PDF_NonfatalError, "Dash array too long (%d) - truncated",
		length);
	length = MAX_DASH_LENGTH;
    }

    if (p->contents == c_path) {
	pdf_error(p, PDF_NonfatalError,
		"Didn't end the path before PDF_setpolydash");
	pdf_end_path(p, pdf_true);
    }

    for (i = 0; i < length; i++) {
	if (darray[i] < (float) 0.0) {
	    pdf_error(p, PDF_NonfatalError, "Negative dash value %f",
	    	darray[i]);
	    return;
	}
    }

    pdf_puts(p, "[");

    for (i = 0; i < length; i++) {
	pdf_printf(p, "%s ", pdf_float(buf, darray[i]));
    }
    pdf_puts(p, "] 0 d\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_setflat(PDF *p, float flat)
{
    char buf[FLOATBUFSIZE];

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (flat < 0.0 || flat > 100.0) {
	pdf_error(p, PDF_NonfatalError, "Illegal flat value %f", flat);
	return;
    }

    if (p->contents == c_path) {
	pdf_error(p, PDF_NonfatalError,
		"Didn't end the path before PDF_setflat");
	pdf_end_path(p, pdf_true);
    }

    pdf_printf(p, "%s i\n", pdf_float(buf, flat));
}

#define LAST_JOIN	2

PDFLIB_API void PDFLIB_CALL
PDF_setlinejoin(PDF *p, int join)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->contents == c_path) {
	pdf_error(p, PDF_NonfatalError,
		"Didn't end the path before PDF_setlinejoin");
	pdf_end_path(p, pdf_true);
    }

    if (join > LAST_JOIN) {
	pdf_error(p, PDF_NonfatalError, "Bogus line join value %d", join);
	return;
    }

    pdf_printf(p, "%d j\n", join);
}
#undef LAST_JOIN

#define LAST_CAP	2

PDFLIB_API void PDFLIB_CALL
PDF_setlinecap(PDF *p, int cap)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->contents == c_path) {
	pdf_error(p, PDF_NonfatalError,
		"Didn't end the path before PDF_setlinecap");
	pdf_end_path(p, pdf_true);
    }

    if (cap > LAST_CAP) {
	pdf_error(p, PDF_NonfatalError, "Bogus line cap value %d", cap);
	return;
    }

    pdf_printf(p, "%d J\n", cap);
}
#undef LAST_CAP

PDFLIB_API void PDFLIB_CALL
PDF_setmiterlimit(PDF *p, float miter)
{
    char buf[FLOATBUFSIZE];

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->contents == c_path) {
	pdf_error(p, PDF_NonfatalError,
		"Didn't end the path before PDF_setmiterlimit");
	pdf_end_path(p, pdf_true);
    }

    if (miter < (float) 1.0) {
	pdf_error(p, PDF_NonfatalError, "Bogus miter limit %f", miter);
	return;
    }

    pdf_printf(p, "%s M\n", pdf_float(buf, miter));
}

PDFLIB_API void PDFLIB_CALL
PDF_setlinewidth(PDF *p, float width)
{
    char buf[FLOATBUFSIZE];

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->contents == c_path) {
	pdf_error(p, PDF_NonfatalError,
		"Didn't end the path before PDF_setlinewidth");
	pdf_end_path(p, pdf_true);
    }

    if (width <= (float) 0.0) {
	pdf_error(p, PDF_NonfatalError, "Bogus line width %f", width);
	return;
    }

    pdf_printf(p, "%s w\n", pdf_float(buf, width));
}
