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

/* p_color.c
 *
 * PDFlib color routines
 *
 */

#include <stdio.h>
#include <math.h>

#include "p_intern.h"

void
pdf_init_cstate(PDF *p)
{
    pdf_cstate *cs;

    cs = &p->cstate[0];

    cs->fill.cs		= DeviceGray;
    cs->fill.val.gray	= (float) 0.0;

    cs->stroke.cs	= DeviceGray;
    cs->stroke.val.gray	= (float) 0.0;
}

/* Color operators */

/* Avoid wrong error messages due to rounding artifacts.
 * This doesn't do any harm since we truncate to 5 decimal places anyway
 * when producing PDF output.
 */
#define EPSILON	1.00001

PDFLIB_API void PDFLIB_CALL
PDF_setgray_fill(PDF *p, float g)
{
    char buf[FLOATBUFSIZE];

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (g < 0.0 || g > EPSILON ) {
	pdf_error(p, PDF_NonfatalError, 
	    "Bogus gray value (%f) in PDF_setgray_fill", g);
	return;
    }

    pdf_printf(p, "%s g\n", pdf_float(buf, g));

    p->cstate[p->sl].fill.cs		= DeviceGray;
    p->cstate[p->sl].fill.val.gray	= g;
}

PDFLIB_API void PDFLIB_CALL
PDF_setgray_stroke(PDF *p, float g)
{
    char buf[FLOATBUFSIZE];

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (g < 0.0 || g > EPSILON ) {
	pdf_error(p, PDF_NonfatalError, 
	    "Bogus gray value (%f) in PDF_setgray_stroke", g);
	return;
    }

    pdf_printf(p, "%s G\n", pdf_float(buf, g));

    p->cstate[p->sl].stroke.cs		= DeviceGray;
    p->cstate[p->sl].stroke.val.gray	= g;
}

PDFLIB_API void PDFLIB_CALL
PDF_setgray(PDF *p, float g)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (g < 0.0 || g > EPSILON ) {
	pdf_error(p, PDF_NonfatalError, 
	    "Bogus gray value (%f) in PDF_setgray", g);
	return;
    }

    PDF_setgray_stroke(p, g);
    PDF_setgray_fill(p, g);
}

PDFLIB_API void PDFLIB_CALL
PDF_setrgbcolor_fill(PDF *p, float red, float green, float blue)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE], buf3[FLOATBUFSIZE];

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (red < 0.0 || red > EPSILON || green < 0.0 || green > EPSILON ||
	blue < 0.0 || blue > EPSILON) {
	pdf_error(p, PDF_NonfatalError, 
	    "Bogus color value (%f/%f/%f) in PDF_setrgbcolor_fill",
	    red, green, blue);
	return;
    }

    if (red == green && green == blue)
	PDF_setgray_fill(p, red);
    else {
	pdf_printf(p, "%s %s %s rg\n",
	   pdf_float(buf1, red), pdf_float(buf2, green), pdf_float(buf3, blue));

	p->cstate[p->sl].fill.cs	= DeviceRGB;
	p->cstate[p->sl].fill.val.rgb.r= red;
	p->cstate[p->sl].fill.val.rgb.g= green;
	p->cstate[p->sl].fill.val.rgb.b= blue;
    }
}

PDFLIB_API void PDFLIB_CALL
PDF_setrgbcolor_stroke(PDF *p, float red, float green, float blue)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE], buf3[FLOATBUFSIZE];

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (red < 0.0 || red > EPSILON || green < 0.0 || green > EPSILON ||
	blue < 0.0 || blue > EPSILON) {
	pdf_error(p, PDF_NonfatalError, 
	    "Bogus color value (%f/%f/%f) in PDF_setrgbcolor_stroke",
	    red, green, blue);
	return;
    }

    if (red == green && green == blue)
	PDF_setgray_stroke(p, red);
    else {
	pdf_printf(p, "%s %s %s RG\n",
	   pdf_float(buf1, red), pdf_float(buf2, green), pdf_float(buf3, blue));
	p->cstate[p->sl].stroke.cs		= DeviceRGB;
	p->cstate[p->sl].stroke.val.rgb.r	= red;
	p->cstate[p->sl].stroke.val.rgb.g	= green;
	p->cstate[p->sl].stroke.val.rgb.b	= blue;
    }
}

PDFLIB_API void PDFLIB_CALL
PDF_setrgbcolor(PDF *p, float red, float green, float blue)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (red < 0.0 || red > EPSILON || green < 0.0 || green > EPSILON ||
	blue < 0.0 || blue > EPSILON) {
	pdf_error(p, PDF_NonfatalError, 
	    "Bogus color value (%f/%f/%f) in PDF_setrgbcolor",
	    red, green, blue);
	return;
    }

    if (red == green && green == blue)
	PDF_setgray(p, red);
    else {
	PDF_setrgbcolor_fill(p, red, green, blue);
	PDF_setrgbcolor_stroke(p, red, green, blue);
    }
}

#ifdef PDF_CMYK_SUPPORTED

PDFLIB_API void PDFLIB_CALL
PDF_setcmykcolor_fill(PDF *p, float cyan, float magenta, float yellow, float black)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE], buf3[FLOATBUFSIZE];
    char buf4[FLOATBUFSIZE];

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (cyan < 0.0 || cyan > EPSILON || magenta < 0.0 || magenta > EPSILON ||
	yellow < 0.0 || yellow > EPSILON || black < 0.0 || black > EPSILON) {
	pdf_error(p, PDF_NonfatalError, 
	    "Bogus color value (%f/%f/%f/%f) in PDF_setcmykcolor_fill",
	    cyan, magenta, yellow, black);
	return;
    }

    pdf_printf(p, "%s %s %s %s k\n",
       pdf_float(buf1, cyan), pdf_float(buf2, magenta),
       pdf_float(buf3, yellow), pdf_float(buf4, black));

    p->cstate[p->sl].fill.cs		= DeviceCMYK;
    p->cstate[p->sl].fill.val.cmyk.c	= cyan;
    p->cstate[p->sl].fill.val.cmyk.m	= magenta;
    p->cstate[p->sl].fill.val.cmyk.y	= yellow;
    p->cstate[p->sl].fill.val.cmyk.k	= black;
}

PDFLIB_API void PDFLIB_CALL
PDF_setcmykcolor_stroke(PDF *p, float cyan, float magenta, float yellow, float black)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE], buf3[FLOATBUFSIZE];
    char buf4[FLOATBUFSIZE];

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (cyan < 0.0 || cyan > EPSILON || magenta < 0.0 || magenta > EPSILON ||
	yellow < 0.0 || yellow > EPSILON || black < 0.0 || black > EPSILON) {
	pdf_error(p, PDF_NonfatalError, 
	    "Bogus color value (%f/%f/%f/%f) in PDF_setcmykcolor_stroke",
	    cyan, magenta, yellow, black);
	return;
    }

    pdf_printf(p, "%s %s %s %s K\n",
       pdf_float(buf1, cyan), pdf_float(buf2, magenta),
       pdf_float(buf3, yellow), pdf_float(buf4, black));

    p->cstate[p->sl].stroke.cs		= DeviceCMYK;
    p->cstate[p->sl].fill.val.cmyk.c	= cyan;
    p->cstate[p->sl].fill.val.cmyk.m	= magenta;
    p->cstate[p->sl].fill.val.cmyk.y	= yellow;
    p->cstate[p->sl].fill.val.cmyk.k	= black;
}

PDFLIB_API void PDFLIB_CALL
PDF_setcmykcolor(PDF *p, float cyan, float magenta, float yellow, float black)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (cyan < 0.0 || cyan > EPSILON || magenta < 0.0 || magenta > EPSILON ||
	yellow < 0.0 || yellow > EPSILON || black < 0.0 || black > EPSILON) {
	pdf_error(p, PDF_NonfatalError, 
	    "Bogus color value (%f/%f/%f/%f) in PDF_setcmykcolor",
	    cyan, magenta, yellow, black);
	return;
    }

    PDF_setcmykcolor_fill(p, cyan, magenta, yellow, black);
    PDF_setcmykcolor_stroke(p, cyan, magenta, yellow, black);
}

#endif /* PDF_CMYK_SUPPORTED */
