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

/* p_text.c
 *
 * PDFlib text routines
 *
 */

#include <string.h>
#include <ctype.h>
#include <math.h>

#include "p_intern.h"
#include "p_font.h"

/* ------------------------ Auxiliary routines ------------------------ */

static void
pdf_quote_string2(PDF *p, const char *text, int len)
{
    const unsigned char *goal, *s;

    pdf_putc(p, PDF_PARENLEFT);
    goal = (const unsigned char *) text + len;

    for (s = (const unsigned char *) text; s < goal; s++) {
	switch (*s) {
	    case PDF_RETURN:
		pdf_putc(p, PDF_BACKSLASH);
		pdf_putc(p, PDF_r);
		break;

	    case PDF_NEWLINE:
		pdf_putc(p, PDF_BACKSLASH);
		pdf_putc(p, PDF_n);
		break;

	    default:
		if (*s == PDF_PARENLEFT || *s == PDF_PARENRIGHT || *s == PDF_BACKSLASH)
		    pdf_putc(p, PDF_BACKSLASH);
		pdf_putc(p, (char) *s);
	}
    }
    pdf_putc(p, PDF_PARENRIGHT);
}

void
pdf_quote_string(PDF *p, const char *text)
{
    int len;

    len = (int) pdf_strlen(text) - 1;	/* subtract a null byte... */

    /* ...and possibly another one */
    if (pdf_is_unicode(text))
	len--;

    pdf_quote_string2(p, text, len);
}

static pdf_bool
pdf_is_identity_matrix(pdf_matrix *m)
{
    return (m->a == (float) 1 &&
	    m->b == (float) 0 &&
	    m->c == (float) 0 &&
	    m->d == (float) 1 &&
	    m->e == (float) 0 &&
	    m->f == (float) 0);
}

/* ------------------------ Text object operators ------------------------ */

void
pdf_begin_text(PDF *p, pdf_bool setpos)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE];
    char buf3[FLOATBUFSIZE], buf4[FLOATBUFSIZE];
    char buf5[FLOATBUFSIZE], buf6[FLOATBUFSIZE];
    pdf_matrix *m;

    if (p->contents != c_text) {
	pdf_end_path(p, pdf_true);

	p->procset	|= Text;
	p->contents	= c_text;
	p->leading_done = pdf_false;

	pdf_puts(p, "BT\n");

	/* BT resets the current point, text matrix, and line matrix */
	p->gstate[p->sl].x = (float) 0.0;
	p->gstate[p->sl].y = (float) 0.0;
    }

    /* output the matrix if values have been set before */
    m = &p->tstate[p->sl].m;
    if (setpos && !pdf_is_identity_matrix(m)) {
	pdf_printf(p, "%s %s %s %s %s %s Tm\n",
	      pdf_float(buf1, m->a), pdf_float(buf2, m->b),
	      pdf_float(buf3, m->c), pdf_float(buf4, m->d),
	      pdf_float(buf5, m->e), pdf_float(buf6, m->f));
    }
}

void
pdf_end_text(PDF *p)
{
    if (p->contents != c_text)
	return;

    p->contents	= c_page;

    pdf_puts(p, "ET\n");
}

/* ------------------------ Text state operators ------------------------ */

/* Initialize the text state at the beginning of each page */
void
pdf_init_tstate(PDF *p)
{
    pdf_tstate *ts;

    ts = &p->tstate[p->sl];

    ts->c	= (float) 0;
    ts->w	= (float) 0;
    ts->h	= (float) 1;
    ts->l	= (float) 0;
    ts->f	= -1;
    ts->fs	= (float) 0;

    ts->m.a	= (float) 1;
    ts->m.b	= (float) 0;
    ts->m.c	= (float) 0;
    ts->m.d	= (float) 1;
    ts->m.e	= (float) 0;
    ts->m.f	= (float) 0;

    ts->mode	= 0;
    ts->rise	= (float) 0;

    ts->lm.a	= (float) 1;
    ts->lm.b	= (float) 0;
    ts->lm.c	= (float) 0;
    ts->lm.d	= (float) 1;
    ts->lm.e	= (float) 0;
    ts->lm.f	= (float) 0;

    p->underline	= pdf_false;
    p->overline		= pdf_false;
    p->strikeout	= pdf_false;
    p->leading_done	= pdf_false;
}

/* character spacing for justified lines */
void
pdf_set_char_spacing(PDF *p, float spacing)
{
    char buf[FLOATBUFSIZE];

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    /*
     * We take care of spacing values != 0 in the text output functions,
     * but must explicitly reset here.
     */
    if (spacing == (float) 0) {
	pdf_begin_text(p, pdf_false);
	pdf_printf(p, "%s Tc\n", pdf_float(buf, spacing));
    }

    p->tstate[p->sl].c = spacing;
    p->leading_done = pdf_false;
}

/* word spacing for justified lines */
void
pdf_set_word_spacing(PDF *p, float spacing)
{
    char buf[FLOATBUFSIZE];

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    /*
     * We take care of spacing values != 0 in the text output functions,
     * but must explicitly reset here.
     */
    if (spacing == (float) 0) {
	pdf_begin_text(p, pdf_false);
	pdf_printf(p, "%s Tw\n", pdf_float(buf, spacing));
    }

    p->tstate[p->sl].w = spacing;
    p->leading_done = pdf_false;
}

void
pdf_set_horiz_scaling(PDF *p, float scale)
{
    char buf[FLOATBUFSIZE];

    if (scale == (float) 0.0)
	pdf_error(p, PDF_ValueError, "Bad horizontal text scaling 0");
	
    if (scale == 100 * p->tstate[p->sl].h)
	return;

    pdf_begin_text(p, pdf_false);
    pdf_printf(p, "%s Tz\n", pdf_float(buf, scale));

    p->tstate[p->sl].h = scale / (float) 100.0;
}

void
pdf_set_leading(PDF *p, float l)
{
    if (l == p->tstate[p->sl].l)
	return;

    p->tstate[p->sl].l = l;
    p->leading_done = pdf_false;
}

#define LAST_MODE	7

void
pdf_set_text_rendering(PDF *p, int mode)
{
    if (mode < 0 || mode > LAST_MODE) {
	pdf_error(p, PDF_NonfatalError, "Bogus text rendering mode %d", mode);
	return;
    }
		
    if (mode == p->tstate[p->sl].mode)
	return;

    pdf_begin_text(p, pdf_false);
    pdf_printf(p, "%d Tr\n", mode);

    p->tstate[p->sl].mode = mode;
}
#undef LAST_MODE

void
pdf_set_text_rise(PDF *p, float rise)
{
    char buf[FLOATBUFSIZE];

    if (rise == p->tstate[p->sl].rise)
	return;

    pdf_begin_text(p, pdf_false);
    pdf_printf(p, "%s Ts\n", pdf_float(buf, rise));

    p->tstate[p->sl].rise = rise;
}

/* Text positioning operators */

PDFLIB_API void PDFLIB_CALL
PDF_set_text_matrix(PDF *p, float a, float b, float c, float d, float e, float f)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    p->tstate[p->sl].m.a = a;
    p->tstate[p->sl].m.b = b;
    p->tstate[p->sl].m.c = c;
    p->tstate[p->sl].m.d = d;
    p->tstate[p->sl].m.e = e;
    p->tstate[p->sl].m.f = f;

    pdf_begin_text(p, pdf_true);
}

PDFLIB_API void PDFLIB_CALL
PDF_set_text_pos(PDF *p, float x, float y)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    p->tstate[p->sl].m.e = x;
    p->tstate[p->sl].m.f = y;

    p->tstate[p->sl].lm.e = x;
    p->tstate[p->sl].lm.f = y;

    pdf_begin_text(p, pdf_true);
}

/* String width calculations */

PDFLIB_API float PDFLIB_CALL
PDF_stringwidth(PDF *p, const char *text, int font, float size)
{
    return PDF_stringwidth2(p, text, 0, font, size);
}

PDFLIB_API float PDFLIB_CALL
PDF_stringwidth2(PDF *p, const char *text, int len, int font, float size)
{
    const char *cp;
    int i;
    float width = (float) 0.0;

    if (PDF_SANITY_CHECK_FAILED(p))
	return width;

    /* Check parameters */
    if (text == NULL || *text == '\0')
	return width;

    if (len == 0)
	len = (int) strlen(text);

    if (font < 0 || font >= p->fonts_number)
	pdf_error(p, PDF_ValueError, "Bad font descriptor %d", font);
	
    if (size == (float) 0.0)
	pdf_error(p, PDF_ValueError, "Bad font size 0 in PDF_stringwidth");

    /* We cannot handle CID fonts */
    if (p->fonts[font].encoding == cid)
	return width;

    for (i = 0, cp = text; i < len; cp++, i++) {
	/* take word spacing parameter into account at each blank */
	if (*cp == PDF_SPACE)
	    width += p->tstate[p->sl].w;

	/* individual character width plus character spacing parameter */
	width += size * p->fonts[font].widths[(unsigned char) *cp]/((float)1000)
		 + p->tstate[p->sl].c;
    }

    /* don't take into account the last character's additional spacing */
    width -= p->tstate[p->sl].c;

    /* take current text matrix and horizontal scaling factor into account */
    width = width * p->tstate[p->sl].m.a * p->tstate[p->sl].h +
	    p->tstate[p->sl].rise * p->tstate[p->sl].m.c;

    return width;
}

/* ------------------------ Text control functions ------------------------ */

static void
pdf_underline(PDF *p, const char *text, int len, float x, float y)
{
    float length, delta_y, linewidth, xscale, yscale;
    pdf_tstate *ts;

    ts = &p->tstate[p->sl];

    /* We can't do underline for CID fonts */
    if (p->fonts[ts->f].encoding == cid)
	return;

    xscale = (float) fabs((double) ts->m.a);
    yscale = (float) fabs((double) ts->m.d);

    linewidth = ts->fs * p->fonts[ts->f].underlineThickness / 1000 *
		ts->h * (xscale > yscale ?  xscale : yscale);

    /* the font size may be negative, resulting in bad line width */
    linewidth = (float) fabs((double) linewidth);

    length = PDF_stringwidth2(p, text, len, ts->f, ts->fs);

    delta_y = (ts->fs * p->fonts[ts->f].underlinePosition / 1000 +
	ts->rise) * (float) fabs((double) ts->h) * (xscale > yscale ?  xscale : yscale);

    PDF_save(p);

    PDF_setlinewidth(p, linewidth);
    PDF_setlinecap(p, 0);
    PDF_setdash(p, 0, 0);
    PDF_moveto(p, x,          y + delta_y);
    PDF_lineto(p, x + length, y + delta_y);
    PDF_stroke(p);

    PDF_restore(p);
}

static void
pdf_overline(PDF *p, const char *text, int len, float x, float y)
{
    float length, delta_y, linewidth, xscale, yscale, lineheight;
    pdf_tstate *ts;

    ts = &p->tstate[p->sl];

    /* We can't do overline for CID fonts */
    if (p->fonts[ts->f].encoding == cid)
	return;

    xscale = (float) fabs((double) ts->m.a);
    yscale = (float) fabs((double) ts->m.d);

    linewidth = ts->fs * p->fonts[ts->f].underlineThickness / 1000 *
		ts->h * (xscale > yscale ?  xscale : yscale);

    /* the font size may be negative, resulting in bad line width */
    linewidth = (float) fabs((double) linewidth);

    lineheight = ((float) p->fonts[ts->f].ascender/1000) * ts->fs;

    length = PDF_stringwidth2(p, text, len, ts->f, ts->fs);

    delta_y = (ts->fs * p->fonts[ts->f].underlinePosition / 1000 +
	ts->rise) * (float) fabs((double) ts->h) * (xscale > yscale ?  xscale : yscale);

    PDF_save(p);

    PDF_setlinewidth(p, linewidth);
    PDF_setlinecap(p, 0);
    PDF_setdash(p, 0, 0);
    PDF_moveto(p, x,          y + lineheight - delta_y);
    PDF_lineto(p, x + length, y + lineheight - delta_y);
    PDF_stroke(p);

    PDF_restore(p);
}

static void
pdf_strikeout(PDF *p, const char *text, int len, float x, float y)
{
    float length, delta_y, linewidth, xscale, yscale, lineheight;
    pdf_tstate *ts;

    ts = &p->tstate[p->sl];

    /* We can't do strikeout for CID fonts */
    if (p->fonts[ts->f].encoding == cid)
	return;

    xscale = (float) fabs((double) ts->m.a);
    yscale = (float) fabs((double) ts->m.d);

    linewidth = ts->fs * p->fonts[ts->f].underlineThickness / 1000 *
		ts->h * (xscale > yscale ?  xscale : yscale);

    /* the font size may be negative, resulting in bad line width */
    linewidth = (float) fabs((double) linewidth);

    lineheight = ((float) p->fonts[ts->f].ascender/1000) * ts->fs;

    length = PDF_stringwidth2(p, text, len, ts->f, ts->fs);

    delta_y = (ts->fs * p->fonts[ts->f].underlinePosition / 1000 +
	ts->rise) * (float) fabs((double) ts->h) * (xscale > yscale ?  xscale : yscale);

    PDF_save(p);

    PDF_setlinewidth(p, linewidth);
    PDF_setlinecap(p, 0);
    PDF_setdash(p, 0, 0);
    PDF_moveto(p, x,          y + lineheight/2 + delta_y);
    PDF_lineto(p, x + length, y + lineheight/2 + delta_y);
    PDF_stroke(p);

    PDF_restore(p);
}

/* ------------------------ Text rendering operators ------------------------ */

#define PDF_RENDERMODE_FILLCLIP 4

PDFLIB_API void PDFLIB_CALL
PDF_show(PDF *p, const char *text)
{
    PDF_show2(p, text, 0);
}

PDFLIB_API void PDFLIB_CALL
PDF_show2(PDF *p, const char *text, int len)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE];

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_show)");

    if (text == NULL)
	return;

    if (len == 0) {
	len = (int) strlen(text);
	if (len == 0)
	    return;
    }

    if (p->underline)
	pdf_underline(p, text, len, p->tstate[p->sl].m.e, p->tstate[p->sl].m.f);
    if (p->overline)
	pdf_overline(p, text, len, p->tstate[p->sl].m.e, p->tstate[p->sl].m.f);
    if (p->strikeout)
	pdf_strikeout(p, text, len, p->tstate[p->sl].m.e, p->tstate[p->sl].m.f);


    if (p->tstate[p->sl].w != (float) 0 || p->tstate[p->sl].c != (float) 0) {

	p->tstate[p->sl].m.f += p->tstate[p->sl].l;
	pdf_begin_text(p, (p->contents != c_text));

	if (!p->leading_done) {
	    pdf_printf(p, "%s TL\n", pdf_float(buf1, p->tstate[p->sl].l));

	    /* word and character spacing will be set via the " operator */
	    p->leading_done = pdf_true;
	}

	pdf_printf(p, "%s %s",	pdf_float(buf1, p->tstate[p->sl].w),
				pdf_float(buf2, p->tstate[p->sl].c));
	pdf_quote_string2(p, text, len);
	pdf_puts(p, "\"\n");

    } else {

	pdf_begin_text(p, (p->contents != c_text));
	pdf_quote_string2(p, text, len);
	pdf_puts(p, "Tj\n");
    }

    if (p->tstate[p->sl].mode >= PDF_RENDERMODE_FILLCLIP)
	pdf_end_text(p);

    p->tstate[p->sl].m.e += 
    	PDF_stringwidth2(p, text, len, p->tstate[p->sl].f, p->tstate[p->sl].fs);
}

PDFLIB_API void PDFLIB_CALL
PDF_show_xy(PDF *p, const char *text, float x, float y)
{
    PDF_show_xy2(p, text, 0, x, y);
}

PDFLIB_API void PDFLIB_CALL
PDF_show_xy2(PDF *p, const char *text, int len, float x, float y)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE];

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_show_xy)");

    if (text == NULL) {
	p->tstate[p->sl].lm.e = x;
	p->tstate[p->sl].lm.f = y;

	p->tstate[p->sl].m.e = x;
	p->tstate[p->sl].m.f = y;
	return;
    }

    if (len == 0)
	len = (int) strlen(text);

    if (p->underline)
	pdf_underline(p, text, len, x, y);
    if (p->overline)
	pdf_overline(p, text, len, x, y);
    if (p->strikeout)
	pdf_strikeout(p, text, len, x, y);

    p->tstate[p->sl].m.e = x;
    p->tstate[p->sl].m.f = y;

    p->tstate[p->sl].lm.e = x;
    p->tstate[p->sl].lm.f = y;

    if (p->tstate[p->sl].w != (float) 0 || p->tstate[p->sl].c != (float) 0) {
	p->tstate[p->sl].m.f += p->tstate[p->sl].l;
	pdf_begin_text(p, pdf_true);

	if (!p->leading_done) {
	    pdf_printf(p, "%s TL\n", pdf_float(buf1, p->tstate[p->sl].l));

	    /* word and character spacing will be set via the " operator */
	    p->leading_done = pdf_true;
	}

	pdf_printf(p, "%s %s",	pdf_float(buf1, p->tstate[p->sl].w),
				pdf_float(buf2, p->tstate[p->sl].c));
	pdf_quote_string2(p, text, len);
	pdf_puts(p, "\"\n");

    } else {

	pdf_begin_text(p, pdf_true);
	pdf_quote_string2(p, text, len);
	pdf_puts(p, "Tj\n");
    }

    if (p->tstate[p->sl].mode >= PDF_RENDERMODE_FILLCLIP)
	pdf_end_text(p);

    p->tstate[p->sl].m.e +=
    	PDF_stringwidth2(p, text, len, p->tstate[p->sl].f, p->tstate[p->sl].fs);
}

PDFLIB_API void PDFLIB_CALL
PDF_continue_text(PDF *p, const char *text)
{
    PDF_continue_text2(p, text, 0);
}

PDFLIB_API void PDFLIB_CALL
PDF_continue_text2(PDF *p, const char *text, int len)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE];

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_continue_text)");

    p->tstate[p->sl].m.e  = p->tstate[p->sl].lm.e;
    p->tstate[p->sl].m.f -= p->tstate[p->sl].l;

    if (text == NULL) {
	pdf_puts(p, "()'\n");
	return;
    }

    if (len == 0)
	len = (int) strlen(text);

    if (p->underline)
	pdf_underline(p, text, len, p->tstate[p->sl].m.e, p->tstate[p->sl].m.f);
    if (p->overline)
	pdf_overline(p, text, len, p->tstate[p->sl].m.e, p->tstate[p->sl].m.f);
    if (p->strikeout)
	pdf_strikeout(p, text, len, p->tstate[p->sl].m.e, p->tstate[p->sl].m.f);

    if (p->contents != c_text) {
	if (p->tstate[p->sl].w != (float) 0 || p->tstate[p->sl].c != (float)0) {
	    p->tstate[p->sl].m.f += p->tstate[p->sl].l;
	    pdf_begin_text(p, pdf_true);

	    if (!p->leading_done) {
		pdf_printf(p, "%s TL\n", pdf_float(buf1, p->tstate[p->sl].l));

		if (p->tstate[p->sl].w != (float) 0)
		    pdf_printf(p,"%s Tw\n",pdf_float(buf1, p->tstate[p->sl].w));

		if (p->tstate[p->sl].c != (float) 0)
		    pdf_printf(p,"%s Tc\n",pdf_float(buf1, p->tstate[p->sl].c));

		p->leading_done = pdf_true;
	    }

	    pdf_printf(p, "%s %s", pdf_float(buf1, p->tstate[p->sl].w),
				    pdf_float(buf2, p->tstate[p->sl].c));
	    pdf_quote_string2(p, text, len);
	    pdf_puts(p, "\"\n");

	} else {

	    pdf_begin_text(p, pdf_true);
	    pdf_quote_string2(p, text, len);
	    pdf_puts(p, "Tj\n");
	}

    } else {
	if (!p->leading_done) {
	    pdf_printf(p, "%s TL\n", pdf_float(buf1, p->tstate[p->sl].l));

	    if (p->tstate[p->sl].w != (float) 0)
		pdf_printf(p, "%s Tw\n", pdf_float(buf1, p->tstate[p->sl].w));

	    if (p->tstate[p->sl].c != (float) 0)
		pdf_printf(p, "%s Tc\n", pdf_float(buf1, p->tstate[p->sl].c));

	    p->leading_done = pdf_true;
	}

	pdf_quote_string2(p, text, len);
	pdf_puts(p, "'\n");
    }

    if (p->tstate[p->sl].mode >= PDF_RENDERMODE_FILLCLIP)
	pdf_end_text(p);

    p->tstate[p->sl].m.e +=
    	PDF_stringwidth2(p, text, len, p->tstate[p->sl].f, p->tstate[p->sl].fs);
}

/* ----------------------- Text formatting routines ------------------------ */

/* text alignment modes */
typedef enum { pdf_align_left, pdf_align_right, pdf_align_center,
	pdf_align_justify, pdf_align_fulljustify
} pdf_alignment;

static void
pdf_show_aligned(PDF *p, const char *text, float x, float y, pdf_alignment mode)
{
    pdf_tstate	*ts;

    ts = &p->tstate[p->sl];

    switch (mode) {
	case pdf_align_left:
	case pdf_align_justify:
	case pdf_align_fulljustify:
	    /* nothing extra here... */
	    break;

	case pdf_align_right:
	    x -=
	    	PDF_stringwidth(p, text, ts->f, ts->fs);
	    break;

	case pdf_align_center:
	    x -= PDF_stringwidth(p, text, ts->f, ts->fs) / 2;
	    break;
    }

    PDF_show_xy(p, text, x, y);
}

PDFLIB_API int PDFLIB_CALL
PDF_show_boxed(PDF *p, const char *text, float left, float bottom, float width, float height, const char *hmode, const char *feature)
{
    float	textwidth, old_word_spacing, curx, cury;
    pdf_bool	prematureexit;	/* return because box is too small */
    int		curTextPos;	/* character currently processed */
    int		totallen;	/* total length of input string */
    int		lastdone;	/* last input character processed */
    pdf_alignment mode = pdf_align_left;
    pdf_bool	blind = pdf_false;
    pdf_tstate	*ts;

    if (PDF_SANITY_CHECK_FAILED(p))
	return 0;

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_show_boxed)");

    if (text == NULL || *text == '\0')
	return 0;

    if (hmode == NULL || *hmode == '\0')
	pdf_error(p, PDF_ValueError, "Missing alignment mode in PDF_show_boxed");

    if (!strcmp(hmode, "left"))
	mode = pdf_align_left;
    else if (!strcmp(hmode, "right"))
	mode = pdf_align_right;
    else if (!strcmp(hmode, "center"))
	mode = pdf_align_center;
    else if (!strcmp(hmode, "justify"))
	mode = pdf_align_justify;
    else if (!strcmp(hmode, "fulljustify"))
	mode = pdf_align_fulljustify;
    else
	pdf_error(p, PDF_ValueError,
	    "Bad alignment mode '%s' in PDF_show_boxed", hmode);

    if (feature != NULL && *feature != '\0') {
	if (!strcmp(feature, "blind"))
	    blind = pdf_true;
	else
	    pdf_error(p, PDF_ValueError,
		    "Unknown feature parameter '%s' in PDF_show_boxed", feature);
    }

    ts = &p->tstate[p->sl];

    if (p->fonts_number == 0 || ts->f == -1) {		/* no font set */
        pdf_error(p, PDF_RuntimeError,
                "Must set font before calling PDF_show_boxed");
        return 0;
    }

    /* we cannot handle CID fonts */
    if (p->fonts[ts->f].encoding == cid)
	return 0;

    /* special case for a single aligned line */
    if (width == 0 && height == 0) {
	if (!blind)
	    pdf_show_aligned(p, text, left, bottom, mode);
	return 0;
    }

    if (width == 0 || height == 0)
	pdf_error(p, PDF_ValueError, "Bad size of text box in PDF_show_boxed");

    old_word_spacing = ts->w;
    totallen = (int) strlen(text);

    curx = left;
    cury = bottom + height;
    prematureexit = pdf_false;	
    curTextPos = 0;
    lastdone = 0;

    /* switch curx for right and center justification */
    if (mode == pdf_align_right)
	curx += width;
    else if (mode == pdf_align_center)
	curx += (width / 2);

#define	MAX_CHARS_IN_LINE	1024

    /* loop until all characters processed, or box full */

    while ((curTextPos < totallen) && !prematureexit) {
	/* buffer for constructing the line */
	char	linebuf[MAX_CHARS_IN_LINE];
	int	curCharsInLine = 0;	/* # of chars in constructed line */
	int	lastWordBreak = 0;	/* the last seen space char */
	int	wordBreakCount = 0;	/* # of blanks in this line */
	
	/* loop over the input string */
	while (curTextPos < totallen) {
	    if (curCharsInLine >= MAX_CHARS_IN_LINE) {
		pdf_error(p, PDF_SystemError, "Line too long in PDF_show_boxed()");
	    }

	    /* abandon DOS line-ends */
	    if (text[curTextPos] == PDF_RETURN && text[curTextPos+1] == PDF_NEWLINE)
		curTextPos++;

	    /* if it's a forced line break draw the line */
	    if (text[curTextPos] == PDF_NEWLINE || text[curTextPos] == PDF_RETURN) {

		cury -= ts->l;			/* adjust cury by leading */

		if (cury < bottom) {
		    prematureexit = pdf_true;	/* box full */
		    break;
		}

		linebuf[curCharsInLine] = 0;	/* terminate the line */
		
		/* check whether the line is too long */
		ts->w = (float) 0.0;
		textwidth = PDF_stringwidth(p, linebuf, ts->f, ts->fs);

		/* the forced break occurs too late for this line */
		if (textwidth > width) {
		    if (wordBreakCount == 1) {	/* no blank found */
			prematureexit = pdf_true;
			break;
		    }
		    linebuf[lastWordBreak] = 0;	/* terminate at last blank */
		    curTextPos -= (curCharsInLine - lastWordBreak);

		    if (!blind) {
			pdf_set_word_spacing(p, (float) 0.0);
			pdf_show_aligned(p, linebuf, curx, cury, mode);
		    }

		} else if (!blind) {

		    if (mode == pdf_align_fulljustify && wordBreakCount > 1) {
			pdf_set_word_spacing(p,
			    (width - textwidth) /
			    (wordBreakCount * ts->h * ts->m.a));
		    } else {
			pdf_set_word_spacing(p, (float) 0.0);
		    }

		    pdf_show_aligned(p, linebuf, curx, cury, mode);
		}

		lastdone = curTextPos;
		curCharsInLine = lastWordBreak = wordBreakCount = 0;
		curTextPos++;

	    } else if (text[curTextPos] == PDF_SPACE) {
		linebuf[curCharsInLine] = 0;	/* terminate the line */
		ts->w = (float) 0.0;

		/* line too long ==> break at last blank */
		if (PDF_stringwidth(p, linebuf, ts->f, ts->fs) > width) {
		    cury -= ts->l;		/* adjust cury by leading */

		    if (cury < bottom) {
			prematureexit = pdf_true; 	/* box full */
			break;
		    }

		    linebuf[lastWordBreak] = 0;	/* terminate at last blank */
		    curTextPos -= (curCharsInLine - lastWordBreak - 1);

		    if (lastWordBreak == 0)
			curTextPos--;
		    
		    /* TODO: * force break if wordBreakCount == 1, i.e., no blank */
		    if (wordBreakCount == 1) {
			prematureexit = pdf_true;
			break;
		    }

		    /* adjust word spacing for full justify */
		    if (mode == pdf_align_justify || mode == pdf_align_fulljustify) {
			ts->w = (float) 0.0;
			textwidth = PDF_stringwidth(p, linebuf, ts->f, ts->fs);
			if (!blind) {
			    pdf_set_word_spacing(p,
				(width - textwidth) /
				((wordBreakCount - 1) * ts->h * ts->m.a));
			}
		    }

		    lastdone = curTextPos;
		    if (!blind)
			pdf_show_aligned(p, linebuf, curx, cury, mode);
		    curCharsInLine = lastWordBreak = wordBreakCount = 0;

		} else {
		    /* blank found, and line still fits */
		    wordBreakCount++;
		    lastWordBreak = curCharsInLine;
		    linebuf[curCharsInLine++] = text[curTextPos++];
		}

	    } else {
		/* regular character ==> store in buffer */
		linebuf[curCharsInLine++] = text[curTextPos++];
	    }
	}

	if (prematureexit) {
	    break;		/* box full */
	}
	
	/* if there is anything left in the buffer, draw it */
	if (curTextPos >= totallen && curCharsInLine != 0) {
	    cury -= ts->l;		/* adjust cury for line height */

	    if (cury < bottom) {
		prematureexit = pdf_true; 	/* box full */
		break;
	    }

	    linebuf[curCharsInLine] = 0;	/* terminate the line */
	    
	    /* check if the last line is too long */
	    ts->w = (float) 0.0;
	    textwidth = PDF_stringwidth(p, linebuf, ts->f, ts->fs);

	    if (textwidth > width) {
		linebuf[lastWordBreak] = 0;	/* terminate at last blank */
		curTextPos -= (curCharsInLine - lastWordBreak - 1);

		/* recalculate the width */
		textwidth = PDF_stringwidth(p, linebuf, ts->f, ts->fs);

		/* adjust word spacing for full justify */
		if (mode == pdf_align_justify ||
		    mode == pdf_align_fulljustify) {
		    if (!blind) {
			pdf_set_word_spacing(p,
			(width - textwidth) / ((wordBreakCount - 1) * ts->h * ts->m.a));
		    }
		}

	    } else if (!blind) {

		if (mode == pdf_align_fulljustify && wordBreakCount) {
		    pdf_set_word_spacing(p,
			    (width - textwidth) /
			    (wordBreakCount * ts->h * ts->m.a));
		} else {
		    pdf_set_word_spacing(p, (float) 0.0);
		}
	    }

	    lastdone = curTextPos;
	    if (!blind)
		pdf_show_aligned(p, linebuf, curx, cury, mode);
	    curCharsInLine = lastWordBreak = wordBreakCount = 0;
	}
    }

    if (!blind)
	pdf_set_word_spacing(p, old_word_spacing);

    /* return number of remaining characters */
    return (int) (totallen - lastdone);
}
