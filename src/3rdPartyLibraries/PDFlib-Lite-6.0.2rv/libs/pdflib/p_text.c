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

/* $Id: p_text.c,v 1.1 2005/10/11 17:18:15 vuduc2 Exp $
 *
 * PDFlib text routines
 *
 */

#define P_TEXT_C

#include "p_intern.h"
#include "p_color.h"
#include "p_defopt.h"
#include "p_font.h"
#include "p_layer.h"
#include "p_tagged.h"


/* --------------------- Text state and options functions ------------------- */

struct pdf_tstate_s
{
    pdc_bool    glyphinit;      /* glyph description initialized */
    pdc_bool    hsinit;         /* horizontal scaling initialized */
    int         mask;           /* bit mask for text options */
    int         font;           /* slot number of the current font */
    int         trm;            /* text rendering mode */
    pdc_scalar  fs;             /* font size */
    pdc_scalar  ld;             /* leading */
    pdc_scalar  cs;             /* character spacing */
    pdc_scalar  ws;             /* word spacing */
    pdc_scalar  hs;             /* horizontal scaling */
    pdc_scalar  ia;             /* italic angle */
    pdc_scalar  rise;           /* text rise */
    pdc_scalar  ulw;            /* underline width */
    pdc_scalar  ulp;            /* underline position */

    pdc_bool    newpos;         /* new text position */
    pdc_scalar  currtx;         /* x coordinate of current text position */
    pdc_scalar  currty;         /* y coordinate of current text position */
    pdc_scalar  prevtx;         /* x coordinate of previous text position */
    pdc_scalar  prevty;         /* y coordinate of previous text position */
    pdc_scalar  linetx;         /* x coordinate of text line start position */
    pdc_scalar  refptx;         /* x and y coordinate of reference position */
    pdc_scalar  refpty;         /* for moving to next text line start position*/
};

/* Initialize the text state at the beginning of each page */
void
pdf_init_tstate(PDF *p)
{
    static const char fn[] = "pdf_init_tstate";

    /* text state */
    pdf_ppt *ppt = p->curr_ppt;
    pdf_tstate *ts;

    if (!p->curr_ppt->tstate)
    {
	p->curr_ppt->tstate = (pdf_tstate *) pdc_malloc(p->pdc,
                                   PDF_MAX_SAVE_LEVEL * sizeof(pdf_tstate), fn);
	ppt->currto = (pdf_text_options *) pdc_malloc(p->pdc,
                          sizeof(pdf_text_options), fn);
    }

    ts = &ppt->tstate[ppt->sl];

    ts->glyphinit = pdc_undef;
    ts->hsinit = (p->ydirection == -1) ? pdc_false : pdc_true;

    ts->mask = 0;
    ts->font = -1;
    ts->trm = 0;
    ts->fs = 0;
    ts->ld = 0;
    ts->cs = 0;
    ts->ws = 0;
    ts->hs = 1;
    ts->ia = 0;
    ts->rise = 0;
    ts->ulw = PDF_UNDERLINEWIDTH_AUTO;
    ts->ulp = PDF_UNDERLINEPOSITION_AUTO;

    ts->newpos = pdc_false;
    ts->currtx = 0;
    ts->currty = 0;
    ts->prevtx = 0;
    ts->prevty = 0;
    ts->linetx = 0;
    ts->refptx = 0;
    ts->refpty = 0;

    /* current text options */
    pdf_init_text_options(p, ppt->currto);
}

void
pdf_cleanup_page_tstate(PDF *p, pdf_ppt *ppt)
{
    if (ppt->tstate != NULL)
    {
        pdc_free(p->pdc, ppt->tstate);
        pdc_free(p->pdc, ppt->currto);
	ppt->tstate = NULL;
	ppt->currto = NULL;
    }
}

void
pdf_save_tstate(PDF *p)
{
    pdf_ppt *ppt = p->curr_ppt;
    int sl = ppt->sl;

    memcpy(&ppt->tstate[sl + 1], &ppt->tstate[sl], sizeof(pdf_tstate));
}

void
pdf_restore_currto(PDF *p)
{
    pdf_ppt *ppt = p->curr_ppt;
    pdf_text_options *currto = ppt->currto;
    pdf_tstate *ts = &ppt->tstate[ppt->sl];

    currto->mask = ts->mask;
    currto->font = ts->font;
    currto->textrendering = ts->trm;
    currto->fontsize = ts->fs;
    currto->leading = ts->ld;
    currto->charspacing = ts->cs;
    currto->wordspacing = ts->ws;
    currto->horizscaling = ts->hs;
    currto->italicangle = ts->ia;
    currto->textrise = ts->rise;
    currto->underlinewidth = ts->ulw;
    currto->underlineposition = ts->ulp;
}

void
pdf_set_tstate(PDF *p, pdc_scalar value, pdf_text_optflags tflag)
{
    pdf_ppt *ppt = p->curr_ppt;
    pdf_tstate *ts = &ppt->tstate[ppt->sl];
    pdf_text_options *currto = ppt->currto;
    int ivalue = (int) value;
    pdc_scalar prevvalue;

    /* text state parameter values can never be percentages */

    switch (tflag)
    {
        case to_font:
        pdf_check_handle(p, ivalue, pdc_fonthandle);
        prevvalue = ts->font;
        ts->font = currto->font = ivalue;
        break;

        case to_textrendering:
        if (ivalue < 0 || ivalue > PDF_LAST_TRMODE)
            pdc_error(p->pdc, PDC_E_ILLARG_INT,
                "textrendering", pdc_errprintf(p->pdc, "%d", ivalue),
                0, 0);
        prevvalue = ts->trm;
        ts->trm = currto->textrendering = ivalue;
        break;

        case to_fontsize:
        if (PDC_FLOAT_ISNULL(value))
            pdc_error(p->pdc, PDC_E_ILLARG_FLOAT,
                "fontsize", pdc_errprintf(p->pdc, "%f", value), 0, 0);
        prevvalue = ts->fs;
        ts->ld = currto->leading = value;
        if (!PDC_FLOAT_ISNULL(value - prevvalue))
            currto->mask |= (1 << to_leading);
        prevvalue = ts->fs;
        ts->fs = currto->fontsize = value;
        break;

        case to_leading:
        prevvalue = ts->ld;
        ts->ld = currto->leading = value;
        break;

        case to_charspacing:
        prevvalue = ts->cs;
        ts->cs = currto->charspacing = value;
        break;

        case to_wordspacing:
        prevvalue = ts->ws;
        ts->ws = currto->wordspacing = value;
        break;

        case to_underlinewidth:
        prevvalue = ts->ulw;
        ts->ulw = currto->underlinewidth = value;
        break;

        case to_underlineposition:
        prevvalue = ts->ulp;
        ts->ulp = currto->underlineposition = value;
        break;

        case to_horizscaling:
        if (PDC_FLOAT_ISNULL(value))
            pdc_error(p->pdc, PDC_E_ILLARG_FLOAT,
                "horizscaling", pdc_errprintf(p->pdc, "%f", value), 0, 0);
        prevvalue = ts->hs;
        ts->hs = currto->horizscaling = value;
        break;

        case to_italicangle:
        if (value <= -90 || value >= 90)
            pdc_error(p->pdc, PDC_E_ILLARG_FLOAT,
                "italicangle", pdc_errprintf(p->pdc, "%f", value), 0, 0);
        prevvalue = ts->ia;
        ts->ia = currto->italicangle = value;
        break;

        case to_textrise:
        prevvalue = ts->rise;
        ts->rise = currto->textrise = value;
        break;


        case to_overline:
        currto->overline = (pdc_bool) ivalue;
        return;

        case to_strikeout:
        currto->strikeout = (pdc_bool) ivalue;
        return;

        case to_underline:
        currto->underline = (pdc_bool) ivalue;
        return;

        case to_textformat:
        currto->textformat = (pdc_text_format) ivalue;
        return;

        case to_charref:
        currto->charref = (pdc_bool) ivalue;
        return;

        case to_glyphwarning:
        currto->glyphwarning = (pdc_bool) ivalue;
        return;

        default:
        return;
    }

    if (!PDC_FLOAT_ISNULL(value - prevvalue))
        currto->mask |= (1 << tflag);
    ts->mask = currto->mask;
}

void
pdf__setfont(PDF *p, int font, pdc_scalar fontsize)
{
    pdf_set_tstate(p, (pdc_scalar) font, to_font);
    pdf_set_tstate(p, fontsize, to_fontsize);
}

void
pdf__set_text_pos(PDF *p, pdc_scalar x, pdc_scalar y)
{
    pdf_ppt *ppt = p->curr_ppt;
    pdf_tstate *ts = &ppt->tstate[ppt->sl];

    ts->newpos = pdc_true;
    ts->currtx = x;
    ts->currty = y;
    ts->prevtx = ts->refptx;
    ts->prevty = ts->refpty;
    ts->linetx = x;
}

double
pdf_get_tstate(PDF *p, pdf_text_optflags tflag)
{
    pdf_ppt *ppt = p->curr_ppt;
    pdf_text_options *currto = ppt->currto;

    switch (tflag)
    {
        case to_font:
        return (double) currto->font;

        case to_textrendering:
        return (double) currto->textrendering;

        case to_fontsize:
        return (double) currto->fontsize;

        case to_leading:
        return (double) currto->leading;

        case to_charspacing:
        return (double) currto->charspacing;

        case to_wordspacing:
        return (double) currto->wordspacing;

        case to_horizscaling:
        return (double) currto->horizscaling;

        case to_italicangle:
        return (double) currto->italicangle;

        case to_textrise:
        return (double) currto->textrise;

        case to_underlinewidth:
        return (double) currto->underlinewidth;

        case to_underlineposition:
        return (double) currto->underlineposition;


        case to_overline:
        return (double) currto->overline;

        case to_strikeout:
        return (double) currto->strikeout;

        case to_underline:
        return (double) currto->underline;

        case to_textx:
        return (double) ppt->tstate[ppt->sl].currtx;

        case to_texty:
        return (double) ppt->tstate[ppt->sl].currty;

        default:
        break;
    }

    return 0;
}

int
pdf_get_font(PDF *p)
{
    if (p->curr_ppt)
        return (int) pdf_get_tstate(p, to_font);
    return -1;
}

void
pdf_init_text_options(PDF *p, pdf_text_options *to)
{
    to->mask = 0;
    to->pcmask = 0;
    to->font = -1;
    to->fontsize = 0;
    to->fontset = 0;
    to->leading = 0;
    to->leading_pc = 0;
    to->textrendering = 0;
    to->charspacing = 0;
    to->charspacing_pc = 0;
    to->horizscaling = 1;
    to->italicangle = 0;
    to->textrise = 0;
    to->textrise_pc = 0;
    to->wordspacing = 0;
    to->wordspacing_pc = 0;
    to->underlinewidth = PDF_UNDERLINEWIDTH_AUTO;
    to->underlineposition = PDF_UNDERLINEPOSITION_AUTO;
    to->overline = pdc_false;
    to->strikeout = pdc_false;
    to->underline = pdc_false;
    to->text = NULL;
    to->textlen = 0;
    to->textformat = p->textformat;
    to->charref = p->charref;
    to->glyphwarning = p->debug[(int) 'g'];
    pdf_init_coloropt(&to->fillcolor);
    pdf_init_coloropt(&to->strokecolor);
    to->xadvancelist = NULL;
    to->nglyphs = 0;
    to->link = NULL;
}

static pdf_text_optflags pdf_toptflags[] =
{
    to_font, to_fontsize, to_textrendering, to_charspacing,
    to_horizscaling, to_italicangle, to_wordspacing, to_textrise
};

void
pdf_calculate_text_options(pdf_text_options *to, pdc_bool force,
                           pdc_scalar fontscale)
{
    if (to->mask & (1 << to_fontsize) || force)
    {
        to->fontsize *= fontscale;
    }

    if ((to->mask & (1 << to_charspacing) || force) &&
        (to->pcmask & (1 << to_charspacing)))
    {
        to->charspacing = to->charspacing_pc * to->fontsize;
    }

    if ((to->mask & (1 << to_wordspacing) || force) &&
        (to->pcmask & (1 << to_wordspacing)))
    {
        to->wordspacing = to->wordspacing_pc * to->fontsize;
    }

    if ((to->mask & (1 << to_textrise) || force) &&
        (to->pcmask & (1 << to_textrise)))
    {
        to->textrise = to->textrise_pc * to->fontsize;
    }

    /* maybe used in a future version */
    if ((to->mask & (1 << to_leading) || force) &&
        (to->pcmask & (1 << to_leading)))
    {
        to->leading = to->leading_pc * to->fontsize;
    }
}

void
pdf_set_text_options(PDF *p, pdf_text_options *to)
{
    pdf_ppt *ppt = p->curr_ppt;
    pdf_text_options *currto = p->curr_ppt->currto;
    pdf_tstate *ts = &ppt->tstate[ppt->sl];
    pdf_text_optflags tflag;
    int i, n;

    /* we synchronize both text state and text options */

    n = sizeof(pdf_toptflags) / sizeof(pdf_text_optflags);
    for (i = 0; i < n; i++)
    {
        tflag = pdf_toptflags[i];
        if (to->mask & (1 << tflag))
        {
            switch (tflag)
            {
                case to_font:
                if (!(currto->mask & (1 << tflag)) &&
                    to->font == currto->font)
                    break;
                ts->font = currto->font = to->font;
                continue;

                case to_fontsize:
                if (!(currto->mask & (1 << tflag)) &&
                    PDC_FLOAT_ISNULL(to->fontsize - currto->fontsize))
                    break;
                ts->fs = currto->fontsize = to->fontsize;
                continue;

                case to_textrendering:
                if (!(currto->mask & (1 << tflag)) &&
                    to->textrendering == currto->textrendering)
                    break;
                ts->trm = currto->textrendering = to->textrendering;
                continue;

                /* to->leading is never used */

                case to_charspacing:
                if (!(currto->mask & (1 << tflag)) &&
                    PDC_FLOAT_ISNULL(to->charspacing - currto->charspacing))
                    break;
                ts->cs = currto->charspacing = to->charspacing;
                continue;

                case to_horizscaling:
                if (!(currto->mask & (1 << tflag)) &&
                    PDC_FLOAT_ISNULL(to->horizscaling - currto->horizscaling))
                    break;
                ts->hs = currto->horizscaling = to->horizscaling;
                continue;

                case to_italicangle:
                if (!(currto->mask & (1 << tflag)) &&
                    PDC_FLOAT_ISNULL(to->italicangle - currto->italicangle))
                    break;
                ts->ia = currto->italicangle = to->italicangle;
                continue;

                case to_wordspacing:
                if (!(currto->mask & (1 << tflag)) &&
                    PDC_FLOAT_ISNULL(to->wordspacing - currto->wordspacing))
                    break;
                ts->ws = currto->wordspacing = to->wordspacing;
                continue;

                case to_textrise:
                if (!(currto->mask & (1 << tflag)) &&
                    PDC_FLOAT_ISNULL(to->textrise - currto->textrise))
                    break;
                ts->rise = currto->textrise = to->textrise;
                continue;

                case to_underlinewidth:
                if (!(currto->mask & (1 << tflag)) &&
                    PDC_FLOAT_ISNULL(to->underlinewidth -
                                     currto->underlinewidth))
                    break;
                ts->ulw = currto->underlinewidth = to->underlinewidth;
                continue;

                case to_underlineposition:
                if (!(currto->mask & (1 << tflag)) &&
                    PDC_FLOAT_ISNULL(to->underlineposition -
                                     currto->underlineposition))
                    break;
                ts->ulp = currto->underlineposition = to->underlineposition;
                continue;

                default:
                continue;
            }

            to->mask &= ~(1 << tflag);
        }
    }

    ts->mask = currto->mask = to->mask;
}

void
pdf_get_text_options(PDF *p, pdf_text_options *to, pdc_resopt *resopts)
{
    char **strlist;
    int inum;

    if (pdc_get_optvalues("font", resopts, &to->font, NULL))
    {
        to->mask |= (1L << to_font);
        to->fontset |= (1L << to_font);
    }

    if (pdc_get_optvalues("fontsize", resopts, &to->fontsize, NULL))
    {
        to->mask |= (1L << to_fontsize);
        to->fontset |= (1L << to_fontsize);
    }

    if (pdc_get_optvalues("charref", resopts, &to->charref, NULL))
        to->mask |= (1L << to_charref);

    if (pdc_get_optvalues("glyphwarning", resopts, &to->glyphwarning, NULL))
        to->mask |= (1L << to_glyphwarning);

    if (pdc_get_optvalues("charspacing", resopts, &to->charspacing, NULL))
    {
        if (pdc_is_lastopt_percent(resopts, 0))
        {
            to->pcmask |= (1 << to_charspacing);
            to->charspacing_pc = to->charspacing;
        }
        else
            to->pcmask &= ~(1 << to_charspacing);
        to->mask |= (1L << to_charspacing);
    }

    if (pdc_get_optvalues("horizscaling", resopts, &to->horizscaling, NULL))
    {
        if (!pdc_is_lastopt_percent(resopts, 0))
            to->horizscaling /= 100.0;
        to->mask |= (1L << to_horizscaling);
    }

    if (pdc_get_optvalues("italicangle", resopts, &to->italicangle, NULL))
        to->mask |= (1L << to_italicangle);


    if (pdc_get_optvalues("overline", resopts, &to->overline, NULL))
        to->mask |= (1L << to_overline);

    if (pdc_get_optvalues("strikeout", resopts, &to->strikeout, NULL))
        to->mask |= (1L << to_strikeout);

    if (pdc_get_optvalues("textformat", resopts, &inum, NULL))
    {
        to->textformat = (pdc_text_format) inum;
        to->mask |= (1L << to_textformat);
        pdf_check_textformat(p, to->textformat);
    }

    if (pdc_get_optvalues("textrendering", resopts, &to->textrendering, NULL))
        to->mask |= (1L << to_textrendering);

    if (pdc_get_optvalues("textrise", resopts, &to->textrise, NULL))
    {
        if (pdc_is_lastopt_percent(resopts, 0))
        {
            to->pcmask |= (1 << to_textrise);
            to->textrise_pc = to->textrise;
        }
        else
            to->pcmask &= ~(1 << to_textrise);
        to->mask |= (1L << to_textrise);
    }

    if (pdc_get_optvalues("underline", resopts, &to->underline, NULL))
        to->mask |= (1L << to_underline);

    if (pdc_get_optvalues("wordspacing", resopts, &to->wordspacing, NULL))
    {
        if (pdc_is_lastopt_percent(resopts, 0))
        {
            to->pcmask |= (1 << to_wordspacing);
            to->wordspacing_pc = to->wordspacing;
        }
        else
            to->pcmask &= ~(1 << to_wordspacing);
        to->mask |= (1L << to_wordspacing);
    }

    if (pdc_get_optvalues("underlinewidth", resopts, &to->underlinewidth, NULL))
    {
        if (pdc_is_lastopt_percent(resopts, 0))
        {
            to->pcmask |= (1 << to_underlinewidth);
        }
        else
            to->pcmask &= ~(1 << to_underlinewidth);
        to->mask |= (1L << to_underlinewidth);
    }

    if (pdc_get_optvalues("underlineposition", resopts,
                          &to->underlineposition, NULL))
    {
        if (pdc_is_lastopt_percent(resopts, 0))
        {
            to->pcmask |= (1 << to_underlineposition);
        }
        else
            to->pcmask &= ~(1 << to_underlineposition);
        to->mask |= (1L << to_underlineposition);
    }

    inum = pdc_get_optvalues("fillcolor", resopts, NULL, &strlist);
    if (inum)
    {
        pdf_parse_coloropt(p, "fillcolor", strlist, inum, (int) color_lab,
                           &to->fillcolor);
        to->mask |= (1L << to_fillcolor);
    }

    inum = pdc_get_optvalues("strokecolor", resopts, NULL, &strlist);
    if (inum)
    {
        pdf_parse_coloropt(p, "strokecolor", strlist, inum, (int) color_lab,
                           &to->strokecolor);
        to->mask |= (1L << to_strokecolor);
    }

    inum = pdc_get_optvalues("xadvancelist", resopts, NULL, &strlist);
    if (inum)
    {
        to->xadvancelist = (pdc_scalar *) strlist;
        to->nglyphs = inum;
    }

    if (pdc_get_optvalues("weblink", resopts, NULL, &strlist))
    {
        to->link = strlist[0];
        to->linktype = "URI";
    }
    else if (pdc_get_optvalues("locallink", resopts, NULL, &strlist))
    {
        to->link = strlist[0];
        to->linktype = "GoTo";
    }
    else if (pdc_get_optvalues("pdflink", resopts, NULL, &strlist))
    {
        to->link = strlist[0];
        to->linktype = "GoToR";
    }
}

/* ------------------------ Text object functions -------------------------- */

static void
pdf_begin_text(PDF *p)
{
    pdf_ppt *ppt = p->curr_ppt;
    pdf_tstate *ts = &ppt->tstate[ppt->sl];
    pdf_text_options *currto = ppt->currto;

    /* end text object if italicangle changed */
    if (currto->mask & (1L << to_italicangle))
        pdf_end_text(p);

    /* begin text object */
    if (!p->in_text)
    {
        p->in_text = pdc_true;
        pdc_puts(p->out, "BT\n");
    }

    if (PDF_FORCE_OUTPUT() && ts->glyphinit == pdc_undef)
        ts->glyphinit = pdc_false;

    if ((currto->mask & (1L << to_font)) ||
        (currto->mask & (1L << to_fontsize)) || !ts->glyphinit)
    {
        pdc_printf(p->out, "/F%d %f Tf\n",
            currto->font, p->ydirection * currto->fontsize);

        p->fonts[currto->font].used_in_current_doc = pdc_true;
        p->fonts[currto->font].used_on_current_page = pdc_true;
    }

    if (currto->mask & (1L << to_textrendering) || !ts->glyphinit)
        pdc_printf(p->out, "%d Tr\n", currto->textrendering);

    if (currto->mask & (1L << to_leading) || !ts->glyphinit)
        pdc_printf(p->out, "%f TL\n", p->ydirection * currto->leading);

    if (currto->mask & (1L << to_charspacing) || !ts->glyphinit)
        pdc_printf(p->out, "%f Tc\n", p->ydirection * currto->charspacing);

    if (!ts->hsinit || currto->mask & (1L << to_horizscaling) || !ts->glyphinit)
        pdc_printf(p->out, "%f Tz\n",
                   100 * p->ydirection * currto->horizscaling);

    if (currto->mask & (1L << to_textrise) || !ts->glyphinit)
       pdc_printf(p->out, "%f Ts\n", p->ydirection * currto->textrise);

    /* initialize */
    if (!ts->glyphinit)
        ts->glyphinit = pdc_true;
    ts->hsinit = pdc_true;
    ts->mask = currto->mask = 0;
}

void
pdf_end_text(PDF *p)
{
    pdf_ppt *ppt = p->curr_ppt;
    pdf_tstate *ts = &ppt->tstate[ppt->sl];

    if (p->in_text)
    {
        p->in_text = pdc_false;
        pdc_puts(p->out, "ET\n");

        ts->newpos = pdc_false;
        ts->prevtx = 0;
        ts->prevty = 0;
        ts->refptx = 0;
        ts->refpty = 0;
    }
}

void
pdf_reset_tstate(PDF *p)
{
    pdf_ppt *ppt = p->curr_ppt;
    pdf_tstate *ts = &ppt->tstate[ppt->sl];

    pdf_set_tstate(p, 0, to_textrendering);
    pdf_set_tstate(p, 0, to_leading);
    pdf_set_tstate(p, 0, to_charspacing);
    pdf_set_tstate(p, 0, to_wordspacing);
    pdf_set_tstate(p, 1, to_horizscaling);
    pdf_set_tstate(p, 0, to_italicangle);
    pdf_set_tstate(p, 0, to_textrise);
    pdf_set_tstate(p, PDF_UNDERLINEWIDTH_AUTO, to_underlinewidth);
    pdf_set_tstate(p, PDF_UNDERLINEPOSITION_AUTO, to_underlineposition);

    ts->hsinit = (p->ydirection == -1) ? pdc_false : pdc_true;
    if (ts->mask || !ts->hsinit)
    {
        pdc_scalar ydirection = p->ydirection;
        p->ydirection = 1;
        pdf_begin_text(p);
        pdf_end_text(p);
        p->ydirection = ydirection;
    }
}


/* ------------------- Text string checking function ---------------------- */

pdc_bool
pdf_check_textchar(PDF *p, pdc_ushort *o_uv, int flags, pdf_text_options *to)
{
    pdc_font *currfont = &p->fonts[to->font];
    pdc_ushort uv = *o_uv;
    pdc_bool isnew = pdc_false;

    switch(currfont->encoding)
    {
        case pdc_builtin:
        {
            if (uv > 0x00FF)
            {
                if (to->glyphwarning == pdc_true && !(flags & PDF_KEEP_UNICODE))
                    pdc_warning(p->pdc, PDF_E_TEXT_BUILTINNOTSHOW,
                                pdc_errprintf(p->pdc, "0x%04X", uv), 0, 0, 0);
                uv = 0;
                isnew = pdc_true;
            }
        }
        break;

        case pdc_unicode:
        {
            pdc_ushort gid;

            /* check Glyph-ID */
            gid = currfont->code2GID[uv];
            if (!gid)
            {
                pdc_ushort uvm =
                    pdc_get_alter_glyphname(uv, currfont->missingglyphs, NULL);
                if (uvm != uv)
                {
                    gid = currfont->code2GID[uvm];
                    if (!(flags & PDF_KEEP_UNICODE))
                        uv = uvm;
                }
                if (!gid)
                {
                    if (to->glyphwarning == pdc_true &&
                        !(flags & PDF_KEEP_UNICODE))
                        pdc_warning(p->pdc, PDF_E_TEXT_UNICODENOTSHOW,
                            pdc_errprintf(p->pdc, "%04X", uv), 0, 0, 0);
                    uv = pdc_get_equi_unicode(uv);
                    if (!uv) uv = PDC_UNICODE_NBSP;
                    gid = currfont->code2GID[(int)uv];
                }
                isnew = (flags & PDF_KEEP_UNICODE) ? pdc_false : pdc_true;
            }

            /* generate 8-bit code */
            if (currfont->lastCode > -1 && !currfont->GID2code[gid])
            {
                if (currfont->lastCode == 255)
                {
                    if (to->glyphwarning == pdc_true)
                        pdc_warning(p->pdc, PDF_E_TEXT_TOOMANYCODES,
                                    0, 0, 0, 0);
                    uv = PDC_UNICODE_SPACE;
                    isnew = pdc_true;
                }
                else
                {
                    currfont->lastCode++;
                    currfont->GID2code[gid] = (pdc_ushort) currfont->lastCode;
                }
            }
        }
        break;

        case pdc_glyphid:
        {
            if (uv >= (pdc_ushort) currfont->numOfCodes)
            {
                if (to->glyphwarning == pdc_true)
                    pdc_warning(p->pdc, PDF_E_TEXT_GLYPHIDNOTSHOW,
                            pdc_errprintf(p->pdc, "%d", uv), 0, 0, 0);
                uv = 0;
                isnew = pdc_true;
            }
        }
        break;

        default:
        {
            if (currfont->encoding >= 0)
            {
                pdc_encodingvector *ev =
                    pdf_get_encoding_vector(p, currfont->encoding);

                int i = pdc_get_encoding_bytecode(p->pdc, ev, uv);
                if (i < 0 ||
                    (currfont->code2GID != NULL && !currfont->code2GID[i]))
                {
                    if (to->glyphwarning == pdc_true)
                        pdc_warning(p->pdc, PDF_E_TEXT_UNICODENOTSHOW,
                            pdc_errprintf(p->pdc, "%04X", uv), 0, 0, 0);
                    i = pdc_get_encoding_bytecode(p->pdc, ev, PDC_UNICODE_NBSP);
                    if (i < 0)
                        i = currfont->spacechar;
                }
                uv = (pdc_ushort) i;
                isnew = pdc_true;
            }
        }
        break;
    }

    *o_uv = uv;
    return isnew;
}

pdc_text_format
pdf_get_autotextformat(pdc_font *font, pdf_text_options *to)
{
    pdc_bool sizebyte = (font->codeSize <= 1 && !font->unibyte);

    if (to->textformat == pdc_auto && sizebyte)
        return pdc_bytes;
    else if (to->textformat == pdc_auto2 && sizebyte)
        return pdc_bytes2;
    return to->textformat;
}

/* Converts and checks input text string.
 *
 * The function returns a pointer to an allocated temporary memory
 * (pdc_malloc_tmp, pdc_free_tmp) containing the converted string
 * if flag PDF_USE_TMPALLOC is set.
 *
 * If return value is NULL error was occurred.
 */

pdc_byte *
pdf_check_textstring(PDF *p, const char *text, int len, int flags,
                     pdf_text_options *to, int *outlen, int *outcharlen)
{

    pdc_font *currfont = &p->fonts[to->font];
    pdc_encoding enc = currfont->encoding;
    pdc_encodingvector *ev = pdf_get_encoding_vector(p, enc);
    pdc_encodingvector *inev = NULL;
    pdc_encodingvector *outev = NULL;

    pdc_byte *outtext;
    pdc_text_format textformat, targettextformat;
    int convflags = PDC_CONV_NOBOM;
    int maxlen, charlen = 1;

    textformat = pdf_get_autotextformat(currfont, to);
    targettextformat = pdc_utf16;

    if (flags & PDF_KEEP_UNICODE)
        convflags |= PDC_CONV_NEWALLOC;

    if (flags & PDF_KEEP_UNICODE)
        inev = ev;
    else
        convflags |= PDC_CONV_KEEPBYTES;

    if (flags & PDF_USE_TMPALLOC)
        convflags |= PDC_CONV_TMPALLOC;


    /* convert to 8-bit or UTF-16 text string */
    pdc_convert_string(p->pdc, textformat, currfont->codepage, inev,
                       (pdc_byte *)text, len,
                       &targettextformat, outev, &outtext, outlen,
                       convflags, to->glyphwarning);

    /* check text string */
    if (outtext && *outlen)
    {

        /* 2 byte storage length of a character */
        if (targettextformat == pdc_utf16)
            charlen = 2;

        /* Maximal text string length - found out emprirically! */
        maxlen = (currfont->codeSize == 1) ? PDF_MAXARRAYSIZE : PDF_MAXDICTSIZE;
        if (!(flags & PDF_KEEP_TEXTLEN) && *outlen > maxlen)
        {
            if (to->glyphwarning)
                pdc_warning(p->pdc, PDF_E_TEXT_TOOLONG,
                    pdc_errprintf(p->pdc, "%d", maxlen), 0, 0, 0);
            *outlen = maxlen;
        }

    }
    *outcharlen = charlen;

    return outtext;
}


/* ------------------------ Text widths functions ------------------------ */

/* Calculates the geometrical width of input text string depending on
 *
 *      to->font, to->fontsize, to->kerning,
 *      to->charspacing, to->horizscaling, to->wordspacing,
 *      to->xadvancelist
 */

pdc_scalar
pdf_calculate_textwidth(PDF *p, const pdc_byte *text, int len, int charlen,
                        pdf_text_options *to)
{
    pdc_font *currfont = &p->fonts[to->font];
    pdc_ushort uv;
    pdc_ushort *ustext = (pdc_ushort *) text;
    pdc_scalar glwidth = 0, width = 0;
    pdc_scalar font2user = to->fontsize / 1000.0;
    pdc_scalar charspacing = to->charspacing / font2user;
    pdc_scalar wordspacing = to->wordspacing / font2user;
    pdc_bool haswidths = (currfont->widths != NULL);
    int i;

    /* We cannot handle empty strings and fonts with unknown
     * code size - especially CID fonts with no UCS-2 CMap */
    if (!len || !currfont->codeSize)
        return width;

    for (i = 0; i < len/charlen; i++)
    {
        if (charlen == 1)
            uv = (pdc_ushort) text[i];
        else
            uv = ustext[i];

        /* start by adding in the width of the character */
        if (currfont->monospace)
        {
            glwidth = (pdc_scalar) currfont->monospace;
        }
        else if (haswidths)
        {
            glwidth = (pdc_scalar) currfont->widths[uv];
        }
        width += glwidth;


        /* supplied glyph widths */
        if (i < to->nglyphs)
        {
            pdc_scalar shift = to->xadvancelist[i] / font2user - glwidth;
            width += shift;
            if (p->ptfrun)
                shift = PDC_ROUND(1e10 * shift) / 1e10;
            shift = PDC_ROUND(1e1 * shift) / 1e1;
            to->xadvancelist[i] = shift;
        }

        /* now add the character spacing parameter */
        width += charspacing;

        /* take word spacing parameter into account at each blank */
        if (wordspacing != 0 && uv == currfont->spacechar)
            width += wordspacing;
    }

    /* take horizontal scaling factor and font scaling factor into account */
    width *= font2user * to->horizscaling;

    return width;
}


pdc_scalar
pdf__stringwidth(PDF *p, const char *text, int len, int font,
                 pdc_scalar fontsize)
{
    pdc_byte *utext;
    int charlen;
    pdc_scalar width = 0;
    pdf_text_options to = *p->curr_ppt->currto;

    if (text && len == 0)
        len = (int) strlen(text);
    if (text == NULL || len <= 0)
        return width;

    pdf_check_handle(p, font, pdc_fonthandle);

    if (PDC_FLOAT_ISNULL(fontsize))
        pdc_error(p->pdc, PDC_E_ILLARG_FLOAT, "fontsize", "0", 0, 0);

    /* convert text string */
    to.font = font;
    to.fontsize = fontsize;
    utext = pdf_check_textstring(p, text, len,
                                 PDF_KEEP_TEXTLEN | PDF_USE_TMPALLOC,
                                 &to, &len, &charlen);
    if (utext)
        width = pdf_calculate_textwidth(p, utext, len, charlen, &to);

    return width;
}

/* ------------------------ Text output functions ------------------------ */

static void
pdf_convert_text_towinansi(PDF *p, const pdc_byte *fromtext, int len,
                           pdc_byte *totext, pdc_font *currfont)
{
    pdc_encodingvector *evfrom = pdf_get_encoding_vector(p, currfont->encoding);
    pdc_encodingvector *evto = pdf_get_encoding_vector(p, currfont->towinansi);
    int i;

    for (i = 0; i < len; i++)
        totext[i] = pdc_transform_bytecode(p->pdc, evto, evfrom, fromtext[i]);
}

void
pdf_put_fieldtext(PDF *p, const char *text, int font)
{
    if (pdc_is_utf8_bytecode(text))
    {
        pdf_put_hypertext(p, text);
    }
    else
    {
        static const char fn[] = "pdf_put_fieldtext";
        pdc_font *currfont = &p->fonts[font];
        char *tmpstring = (char *) text;
        int len = (int) pdc_strlen(text);

        if (len && currfont->towinansi != pdc_invalidenc &&
            !pdc_is_utf16be_unicode(text))
        {
            /* Convert 8-bit code string to winansi */
            tmpstring = (char *) pdc_malloc_tmp(p->pdc,
                                                (size_t) len, fn, NULL, NULL);
            pdf_convert_text_towinansi(p, (pdc_byte *) text, len,
                                       (pdc_byte *) tmpstring, currfont);
        }

        pdc_put_pdfstring(p->out, tmpstring, len);
        if (tmpstring != text)
            pdc_free_tmp(p->pdc, tmpstring);
    }
}

static void
pdf_put_textstring(PDF *p, const pdc_byte *text, int len, int charlen,
                   pdc_font *currfont)
{
    static const char fn[] = "pdf_put_textstring";
    pdc_byte *tmpstring = (pdc_byte *) text;

    (void) charlen;

    if (len)
    {
        if (currfont->towinansi != pdc_invalidenc)
        {
            /* Convert 8-bit code string to winansi */
            tmpstring = (pdc_byte *) pdc_malloc_tmp(p->pdc,
                                         (size_t) len, fn, NULL, NULL);
            pdf_convert_text_towinansi(p, text, len, tmpstring, currfont);
        }
    }

    pdc_put_pdfstring(p->out, (char *) tmpstring, len);
    if (tmpstring != text)
        pdc_free_tmp(p->pdc, tmpstring);
}

static void
pdf_put_textstring_shift(PDF *p, pdc_byte *text, int len, int charlen,
                         pdf_text_options *to, pdc_scalar spaceshift)
{
    pdc_font *currfont = &p->fonts[to->font];
    pdc_ushort *ustext = (pdc_ushort *) text;
    pdc_ushort left, right;
    pdc_byte *currtext;
    pdc_scalar shift;
    int currlen, nchars;
    int i;

    currtext = text;
    currlen = charlen;
    nchars = len/charlen;
    for (i = 1; i < nchars; i++)
    {
        if (charlen == 1)
        {
            left =  (pdc_ushort) text[i-1];
            right = (pdc_ushort) text[i];
        }
        else
        {
            left =  ustext[i-1];
            right = ustext[i];
        }

        /* PDF wants the inverse shift amount
         * (positive numbers move left, negative move right!) */

        if (spaceshift != 0 && left == currfont->spacechar)
            shift = -spaceshift;
        else
            shift = 0;


        if (i <= to->nglyphs)
            shift -= to->xadvancelist[i-1];

        if (shift)
        {
            pdf_put_textstring(p, currtext, currlen, charlen, currfont);
            pdc_printf(p->out, "%f", shift);
            currtext = &text[charlen * i];
            currlen = 0;
        }
        currlen += charlen;
    }

    pdf_put_textstring(p, currtext, currlen, charlen, currfont);

    if (to->nglyphs && to->nglyphs >= nchars)
        pdc_printf(p->out, "%f", -to->xadvancelist[nchars - 1]);

}


/* --------------------- General text placing function --------------------- */


#define PDF_RENDERMODE_FILLCLIP 4

void
pdf_place_text(PDF *p, pdc_byte *text, int len, int charlen,
               pdf_text_options *to, pdc_scalar width, pdc_bool cont)
{
    pdf_tstate *ts = &p->curr_ppt->tstate[p->curr_ppt->sl];
    pdc_font *currfont = &p->fonts[to->font];
    pdc_scalar dx, dy, tx, ty, spaceshift = 0, leading = 0;
    pdc_scalar font2user = to->fontsize / 1000.0;
    pdc_bool takeTJ = pdc_false;

    /* text position */
    if (!cont)
    {
        tx = ts->currtx;
        ty = ts->currty;
    }
    else
    {
        leading = p->ydirection * to->leading;
        tx = ts->linetx;
        ty = ts->currty - leading;
    }

    /* fill and stroke color for text */
    if (to->mask & (1 << to_fillcolor))
        pdf_set_coloropt(p, (int) pdf_fill, &to->fillcolor);
    if (to->mask & (1 << to_strokecolor))
        pdf_set_coloropt(p, (int) pdf_stroke, &to->strokecolor);

    /* text decoration */
    if (width && (to->underline || to->overline || to->strikeout))
    {
        pdc_scalar scale = fabs(to->horizscaling);
        pdc_scalar delta_y, fs, trise, lineheight;
        pdc_scalar txe = (tx + width);
        pdc_scalar linewidth = 0;
        pdc_scalar deflinewidth = 0;
        pdc_scalar lineposition = 0;

        fs = p->ydirection * font2user;
        trise = p->ydirection * to->textrise;

        lineheight = fs * currfont->ascender;
        delta_y = scale * (fs * currfont->underlinePosition + trise);
        if (currfont->underlineThickness == 0)
            currfont->underlineThickness = 50;
        deflinewidth = fabs(scale * fs * currfont->underlineThickness);

        pdf__save(p);

        if (to->underlinewidth == PDF_UNDERLINEWIDTH_AUTO)
        {
            linewidth = deflinewidth;
        }
        else
        {
            linewidth = to->underlinewidth;
            if ((to->pcmask & (1 << to_underlinewidth)))
                linewidth *= fabs(to->fontsize);
        }

        if (to->underlineposition == PDF_UNDERLINEPOSITION_AUTO)
        {
            lineposition = delta_y;
        }
        else
        {
            lineposition = p->ydirection * to->underlineposition;
            if ((to->pcmask & (1 << to_underlineposition)))
                lineposition *= to->fontsize;
        }

        pdf__setlinecap(p, 0);
        pdf__setdash(p, 0, 0);

        if (to->underline)
        {
            pdf__setlinewidth(p, linewidth);
            pdf__moveto(p, tx,  ty + lineposition);
            pdf__lineto(p, txe, ty + lineposition);
            pdf__stroke(p);
        }

        if (to->strikeout)
        {
            pdf__setlinewidth(p, deflinewidth);
            pdf__moveto(p, tx,  ty + lineheight/2 + delta_y);
            pdf__lineto(p, txe, ty + lineheight/2 + delta_y);
            pdf__stroke(p);
        }

        if (to->overline)
        {
            delta_y = scale * (fs * currfont->underlinePosition - trise);

            pdf__setlinewidth(p, deflinewidth);
            pdf__moveto(p, tx,  ty + lineheight - delta_y);
            pdf__lineto(p, txe, ty + lineheight - delta_y);
            pdf__stroke(p);
        }

        pdf__restore(p);
    }



    /* wordspacing */
    if (!PDC_FLOAT_ISNULL(to->wordspacing))
    {
        spaceshift = to->wordspacing / font2user;
        if (p->ptfrun)
            spaceshift = PDC_ROUND(1e10 * spaceshift) / 1e10;
        spaceshift = PDC_ROUND(1e1 * spaceshift) / 1e1;
        takeTJ = PDC_FLOAT_ISNULL(spaceshift) ? pdc_false : pdc_true;
    }


    /* supplied glyph widths */
    if (!takeTJ)
        takeTJ = to->nglyphs;

    /* begin text object */
    pdf_begin_text(p);

    /* italic angle - realized by Tm operator */
    if (!PDC_FLOAT_ISNULL(to->italicangle))
    {
        pdc_printf(p->out, "1 0 %f 1 %f %f Tm\n",
               tan(-p->ydirection * to->italicangle * PDC_DEG2RAD), tx, ty);

        cont = pdc_false;
        ts->newpos = pdc_false;
        ts->refptx = tx;
        ts->refpty = ty;
    }
    else
    {
        /* components of text displacement vector */
        if (!cont)
        {
            dx = tx - ts->prevtx;
            dy = ty - ts->prevty;
        }
        else
        {
            dx = tx - ts->refptx;
            dy = ty - ts->refpty + leading;
        }

        /* condition for text displacement operator Td */
        if (!PDC_FLOAT_ISNULL(dx) || !PDC_FLOAT_ISNULL(dy) ||
            ts->newpos || (cont && takeTJ))
        {
            if (cont)
            {
                dy -= leading;
                cont = pdc_false;
            }
            pdc_printf(p->out, "%f %f Td\n", dx, dy);

            /* new reference position for next line*/
            ts->newpos = pdc_false;
            ts->refptx = tx;
            ts->refpty = ty;
        }
        else
        {
            ts->refpty -= leading;
        }
    }

    /* show text */
    if (!takeTJ)
    {
        pdf_put_textstring(p, text, len, charlen, currfont);
        if (!cont)
            pdc_puts(p->out, "Tj\n");
        else
            pdc_puts(p->out, "'\n");
    }
    else
    {
        pdc_puts(p->out, "[");
        pdf_put_textstring_shift(p, text, len, charlen, to, spaceshift);
        pdc_puts(p->out, "]TJ\n");
    }

    /* new text position */
    ts->currtx = tx + width;
    ts->currty = ty;
    ts->prevtx = ts->currtx;
    ts->prevty = ts->currty;

    if (to->textrendering >= PDF_RENDERMODE_FILLCLIP)
        pdf_end_text(p);
}

/* --------------------- Simple text showing functions --------------------- */

void
pdf__show_text(
    PDF *p,
    const char *text,
    int len,
    pdc_bool cont)
{
    static const char *fn = "pdf__show_text";
    pdf_text_options *currto = p->curr_ppt->currto;
    pdc_byte *utext = NULL;
    int charlen = 1;
    pdc_scalar width;

    if (text && len == 0)
        len = (int) strlen(text);
    if (text == NULL || len <= 0)
    {
        if (cont)
            len = 0;
        else
            return;
    }

    /* no font set */
    if (currto->font == -1)
        pdc_error(p->pdc, PDF_E_TEXT_NOFONT, 0, 0, 0, 0);

    width = 0;
    if (len)
    {
        /* convert text string */
        utext = pdf_check_textstring(p, text, len, PDF_USE_TMPALLOC,
                                     currto, &len, &charlen);
	if (!utext)
            return;

        /* length of text */
        width = pdf_calculate_textwidth(p, utext, len, charlen, currto);
    }
    else
    {
        utext = (pdc_byte *) pdc_calloc_tmp(p->pdc, 2, fn, NULL, NULL);
    }


    /* place text */
    pdf_place_text(p, utext, len, charlen, currto, width, cont);
}


/* ---------- Text showing function with explicit glyph widths  ---------- */

void
pdf__xshow(PDF *p, const char *text, int len, const pdc_scalar *xadvancelist)
{
    static const char *fn = "pdf__xshow";
    pdf_text_options *currto = p->curr_ppt->currto;
    pdc_byte *utext = NULL;
    int charlen = 1;
    size_t nbytes = 0;
    pdc_scalar width;

    if (text && len == 0)
        len = (int) strlen(text);
    if (text == NULL || !len)
        return;

    /* no font set */
    if (currto->font == -1)
        pdc_error(p->pdc, PDF_E_TEXT_NOFONT, 0, 0, 0, 0);

    /* convert text string */
    utext = pdf_check_textstring(p, text, len, PDF_USE_TMPALLOC,
                                 currto, &len, &charlen);
    if (!utext)
        return;

    /* allocating glyph widths arrays */
    nbytes = (size_t) (len / charlen) * sizeof(pdc_scalar);
    currto->xadvancelist = (pdc_scalar *) pdc_malloc_tmp(p->pdc,
                                             nbytes, fn, NULL, NULL);
    memcpy(currto->xadvancelist, xadvancelist, nbytes);
    currto->nglyphs = len / charlen;

    /* length of text */
    width = pdf_calculate_textwidth(p, utext, len, charlen, currto);


    /* place text */
    pdf_place_text(p, utext, len, charlen, currto, width, pdc_false);

    currto->xadvancelist = NULL;
    currto->nglyphs = 0;
}


/* ----------------------- Text fitting function ------------------------ */

void
pdf_init_fit_options(PDF *p, pdf_fit_options *fit)
{
    (void) p;

    fit->boxsize[0] = 0;
    fit->boxsize[1] = 0;
    fit->flags = 0;
    fit->fitmethod = pdc_nofit;
    fit->margin[0] = 0;
    fit->margin[1] = 0;
    fit->mask = 0;
    fit->pcmask = 0;
    fit->shrinklimit = 0.75;
    fit->position[0] = 0;
    fit->position[1] = 0;
    fit->orientate = 0;
    fit->rotate = 0;
    fit->refpoint[0] = 0;
    fit->refpoint[1] = 0;
}

void
pdf_get_fit_options(PDF *p, pdf_fit_options *fit, pdc_resopt *resopts)
{
    int inum;

    (void) p;

    if (pdc_get_optvalues("fitmethod", resopts, &inum, NULL))
    {
        fit->fitmethod = (pdc_fitmethod) inum;
        fit->mask |= (1L << fit_fitmethod);
    }

    if (pdc_get_optvalues("rotate", resopts, &fit->rotate, NULL))
        fit->mask |= (1L << fit_rotate);

    if (pdc_get_optvalues("orientate", resopts, &fit->orientate, NULL))
        fit->mask |= (1L << fit_orientate);

    if (fit->flags & is_textline)
    {
        inum = pdc_get_optvalues("margin", resopts, fit->margin, NULL);
        if (inum)
        {
            if (inum == 1)
                fit->margin[1] = fit->margin[0];
            fit->mask |= (1L << fit_margin);
        }
    }

    if (fit->flags & is_block)
    {
        if (pdc_get_optvalues("refpoint", resopts, fit->refpoint, NULL))
            fit->mask |= (1L << fit_refpoint);
    }


    if (fit->flags & is_block || !(fit->flags & is_textflow))
    {
        if (pdc_get_optvalues("boxsize", resopts, fit->boxsize, NULL))
            fit->mask |= (1L << fit_boxsize);

        if (pdc_get_optvalues("shrinklimit", resopts, &fit->shrinklimit, NULL))
            fit->mask |= (1L << fit_shrinklimit);

        inum = pdc_get_optvalues("position", resopts, fit->position, NULL);
        if (inum)
        {
            if (inum == 1)
                fit->position[1] = fit->position[0];
            fit->mask |= (1L << fit_position);
        }
    }
}

/* definitions of fit text options */
static const pdc_defopt pdf_fit_textline_options[] =
{
    PDF_TEXT_OPTIONS

    {"xadvancelist", pdc_scalarlist, PDC_OPT_NOZERO, 0, PDC_USHRT_MAX,
      PDC_FLOAT_MIN, PDC_FLOAT_MAX, NULL},

    PDF_FIT_OPTIONS1
    PDF_FIT_OPTIONS2
    PDC_OPT_TERMINATE
};

void
pdf__fit_textline(PDF *p, const char *text, int len, pdc_scalar x, pdc_scalar y,
                  const char *optlist)
{
    pdf_ppt *ppt = p->curr_ppt;
    pdf_text_options to;
    pdf_fit_options fit;
    pdc_matrix invm;
    pdc_scalar currtx, currty;

    if (text && len == 0)
        len = (int) strlen(text);
    if (text == NULL || len <= 0)
        return;

    /* initialize text options */
    to = *ppt->currto;
    to.text = (char *) text;
    to.textlen = len;

    /* initialize fit options */
    pdf_init_fit_options(p, &fit);
    fit.flags |= is_textline;
    fit.refpoint[0] = x;
    fit.refpoint[1] = y;

    /* parsing option list */
    if (optlist && strlen(optlist))
    {
        pdc_resopt *resopts;
        pdc_clientdata data;

        pdf_set_clientdata(p, &data);
        resopts = pdc_parse_optionlist(p->pdc, optlist,
                      pdf_fit_textline_options, &data, pdc_true);
        if (!resopts)
            return;

        pdf_get_text_options(p, &to, resopts);
        pdf_get_fit_options(p, &fit, resopts);
    }

    /* no font set */
    if (to.font == -1)
        pdc_error(p->pdc, PDF_E_TEXT_NOFONT, 0, 0, 0, 0);

    pdf__save(p);

    /* put out text line */
    pdf_fit_textline_internal(p, &to, &fit, &currtx, &currty);

    pdf__restore(p);

    /* calculate current text position*/
    pdc_invert_matrix(p->pdc, &invm, &ppt->gstate[ppt->sl].ctm);
    pdc_transform_point(&invm, currtx, currty, &currtx, &currty);
    pdf__set_text_pos(p, currtx, currty);
}

void
pdf_fit_textline_internal(PDF *p, pdf_text_options *to, pdf_fit_options *fit,
                          pdc_scalar *currtx, pdc_scalar *currty)
{
    pdc_byte *utext = (pdc_byte *) "";
    int len, charlen;

    pdc_matrix m, mm;
    pdc_vector elemsize, elemscale, elemmargin, relpos, polyline[5];
    pdc_box fitbox, elembox;
    pdc_scalar ss, width, height;
    pdc_scalar x, y, tx = 0, ty = 0;
    pdc_scalar font2user = to->fontsize / 1000.0;
    int indangle;

    /* calculate and set text options */
    pdf_calculate_text_options(to, pdc_false, 1.0);
    pdf_set_text_options(p, to);

    /* convert text string */
    utext = pdf_check_textstring(p, to->text, to->textlen, PDF_USE_TMPALLOC,
                                 to, &len, &charlen);
    if (utext == NULL || len == 0)
        return;

    if (to->glyphwarning && to->nglyphs && len/charlen != to->nglyphs)
        pdc_warning(p->pdc, PDF_E_TEXT_SIZENOMATCH,
                    pdc_errprintf(p->pdc, "%d", to->nglyphs),
                    pdc_errprintf(p->pdc, "%d", len/charlen), 0, 0);

    /* width of text */
    width = pdf_calculate_textwidth(p, utext, len, charlen, to);
    if (width > PDF_SMALLREAL)
    {
        elemmargin.x = fit->margin[0];
        elemsize.x = width + 2 * elemmargin.x;

        /* text height */
        height = fabs(font2user * p->fonts[to->font].capHeight);
        elemmargin.y = fit->margin[1];
        elemsize.y = height + 2 * elemmargin.y;

        /* orientation */
        indangle = fit->orientate / 90;
        if (indangle % 2)
        {
            ss = elemsize.x;
            elemsize.x = elemsize.y;
            elemsize.y = ss;
        }

        /* box for fitting */
        fitbox.ll.x = 0;
        fitbox.ll.y = 0;
        fitbox.ur.x = fit->boxsize[0];
        fitbox.ur.y = fit->boxsize[1];

        /* relative position */
        relpos.x = fit->position[0] / 100.0;
        relpos.y = fit->position[1] / 100.0;

        /* calculate image box */
        pdc_place_element(fit->fitmethod, fit->shrinklimit, &fitbox, &relpos,
                          &elemsize, &elembox, &elemscale);

        /* reference point */
        pdc_translation_matrix(fit->refpoint[0], fit->refpoint[1], &m);

        /* clipping */
        if (fit->fitmethod == pdc_clip || fit->fitmethod == pdc_slice)
        {
            pdf_concat_raw(p, &m);
            pdf__rect(p, 0, 0, fit->boxsize[0], fit->boxsize[1]);
            pdf__clip(p);
            pdc_identity_matrix(&m);
        }

        /* optional rotation */
        if (fabs(fit->rotate) > PDC_FLOAT_PREC)
        {
            pdc_rotation_matrix(p->ydirection * fit->rotate, &mm);
            pdc_multiply_matrix(&mm, &m);
        }

        /* translation of element box */
        elembox.ll.y *= p->ydirection;
        elembox.ur.y *= p->ydirection;
        pdc_box2polyline(&elembox, polyline);
        tx = polyline[indangle].x;
        ty = polyline[indangle].y;
        pdc_translation_matrix(tx, ty, &mm);
        pdc_multiply_matrix(&mm, &m);

        /* orientation of text */
        if (fit->orientate != 0)
        {
            pdc_rotation_matrix(p->ydirection * fit->orientate, &mm);
            pdc_multiply_matrix(&mm, &m);
            if (indangle % 2)
            {
                ss = elemscale.x;
                elemscale.x = elemscale.y;
                elemscale.y = ss;
            }
        }

        if (elemscale.x != 1 || elemscale.y != 1)
        {
            pdc_scale_matrix(elemscale.x, elemscale.y, &mm);
            pdc_multiply_matrix(&mm, &m);
        }

        /* ctm */
        pdf_concat_raw(p, &m);

        /* text position */
        x = elemmargin.x;
        y = elemmargin.y;
        pdf__set_text_pos(p, x, y);


        /* place text */
        pdf_place_text(p, utext, len, charlen, to, width, pdc_false);

        /* create a link */
        if (to->link)
        {
            pdc_scalar desc = fabs(font2user * p->fonts[to->font].descender);

            utext = pdf_check_textstring(p, to->text, to->textlen,
                                         PDF_USE_TMPALLOC | PDF_KEEP_UNICODE,
                                         to, &len, &charlen);
            pdf_create_link(p, to->linktype, x, y - p->ydirection * desc,
                            x + width, y + p->ydirection * to->fontsize,
                            to->link, (char *) utext, len);
            pdc_free_tmp(p->pdc, utext);
        }
    }

    /* save text position */
    if (currtx && currty)
    {
        pdf_ppt *ppt = p->curr_ppt;
        pdf_tstate *ts = &ppt->tstate[ppt->sl];

        pdc_transform_point(&ppt->gstate[ppt->sl].ctm,
                            ts->currtx, ts->currty, currtx, currty);
    }
}

/*****************************************************************************/
/**         deprecated historical text formatting function                  **/
/*****************************************************************************/

/* this helper function returns the width of the null-terminated string
** 'text' for the current font and size EXCLUDING the last character's
** additional charspacing.
*/
static pdc_scalar
pdf_swidth(PDF *p, const char *text)
{
    pdf_text_options *currto = p->curr_ppt->currto;

    pdc_scalar width = pdf_calculate_textwidth(p,
                          (pdc_byte *) text, (int)strlen(text), 1, currto);
    return (width - currto->horizscaling * currto->charspacing);
}

static void
pdf_show_aligned(PDF *p, const char *text, pdc_scalar x, pdc_scalar y,
                 pdc_scalar wordspacing, pdf_alignment mode)
{
    if (!text)
	return;

    switch (mode) {
        default:
        case text_left:
        case text_justify:
        case text_fulljustify:
	    /* nothing extra here... */
	    break;

        case text_right:
	    x -= pdf_swidth(p, text);
	    break;

        case text_center:
	    x -= pdf_swidth(p, text) / 2;
	    break;
    }

    pdf__set_text_pos(p, x, y);
    pdf_set_tstate(p, wordspacing, to_wordspacing);
    pdf__show_text(p, text, (int) strlen(text), pdc_false);
}

int
pdf__show_boxed(
    PDF *p,
    const char *text, int len,
    pdc_scalar left,
    pdc_scalar bottom,
    pdc_scalar width,
    pdc_scalar height,
    const char *hmode,
    const char *feature)
{
    pdc_scalar  old_wordspacing, wordspacing, textwidth, curx, cury;
    pdc_bool	prematureexit;	/* return because box is too small */
    int		curTextPos;	/* character currently processed */
    int		lastdone;	/* last input character processed */
    int         toconv = len;
    pdf_text_options *currto = p->curr_ppt->currto;
    pdc_font *currfont;
    pdc_byte *utext = NULL;
    pdc_text_format old_textformat;
    pdf_alignment mode = text_left;
    pdc_bool blind = pdc_false;

    /* text length */
    if (text == NULL)
        return 0;
    if (!len)
        len = (int) strlen(text);
    if (!len)
        return 0;

    if (hmode == NULL || *hmode == '\0')
	pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "hmode", 0, 0, 0);

    if (!strcmp(hmode, "left"))
        mode = text_left;
    else if (!strcmp(hmode, "right"))
        mode = text_right;
    else if (!strcmp(hmode, "center"))
        mode = text_center;
    else if (!strcmp(hmode, "justify"))
        mode = text_justify;
    else if (!strcmp(hmode, "fulljustify"))
        mode = text_fulljustify;
    else
	pdc_error(p->pdc, PDC_E_ILLARG_STRING, "hmode", hmode, 0, 0);

    if (feature != NULL && *feature != '\0')
    {
	if (!strcmp(feature, "blind"))
	    blind = pdc_true;
	else
	    pdc_error(p->pdc, PDC_E_ILLARG_STRING, "feature", feature, 0, 0);
    }

    /* no font set */
    if (currto->font == -1)
        pdc_error(p->pdc, PDF_E_TEXT_NOFONT, 0, 0, 0, 0);
    currfont = &p->fonts[currto->font];

    if (width == 0 && height != 0)
        pdc_error(p->pdc, PDC_E_ILLARG_FLOAT,
            "width", pdc_errprintf(p->pdc, "%f", width), 0, 0);

    if (width != 0 && height == 0)
        pdc_error(p->pdc, PDC_E_ILLARG_FLOAT,
            "height", pdc_errprintf(p->pdc, "%f", height), 0, 0);

    /* we cannot handle several encodings */
    if (currfont->encoding == pdc_unicode)
    {
        pdc_error(p->pdc, PDF_E_DOC_FUNCUNSUPP, "Unicode", 0, 0, 0);
    }

    if (currfont->encoding == pdc_glyphid)
    {
        pdc_error(p->pdc, PDF_E_DOC_FUNCUNSUPP, "glyphid", 0, 0, 0);
    }

    if (currfont->encoding == pdc_cid)
    {
	pdc_error(p->pdc, PDF_E_DOC_FUNCUNSUPP, "CID", 0, 0, 0);
    }

    if (currfont->encoding == pdc_ebcdic || currfont->encoding == pdc_ebcdic_37)
    {
	pdc_error(p->pdc, PDF_E_DOC_FUNCUNSUPP, "EBCDIC", 0, 0, 0);
    }

    /* old wordspacing */
    old_textformat = currto->textformat;

    /* convert text string */
    if (toconv)
    {
        int charlen;

        /* convert text string */
        utext = pdf_check_textstring(p, text, len,
                    PDF_KEEP_CONTROL | PDF_KEEP_TEXTLEN | PDF_USE_TMPALLOC,
                    currto, &len, &charlen);
        if (!utext)
            return 0;

        utext[len] = 0;
        text = (const char *) utext;
        currto->textformat = pdc_bytes;
    }

    /* old wordspacing */
    old_wordspacing = currto->wordspacing;

    /* special case for a single aligned line */
    if (width == 0 && height == 0)
    {
        if (!blind)
            pdf_show_aligned(p, text, left, bottom, old_wordspacing, mode);

        if (toconv)
            currto->textformat = old_textformat;
        return 0;
    }

    curx = left;
    cury = bottom + p->ydirection * height;
    prematureexit = pdc_false;
    curTextPos = 0;
    lastdone = 0;

    /* switch curx for right and center justification */
    if (mode == text_right)
	curx += width;
    else if (mode == text_center)
	curx += (width / 2);

#define	MAX_CHARS_IN_LINE	2048

    /* loop until all characters processed, or box full */

    while ((curTextPos < len) && !prematureexit)
    {
	/* buffer for constructing the line */
	char	linebuf[MAX_CHARS_IN_LINE];
	int	curCharsInLine = 0;	/* # of chars in constructed line */
	int	lastWordBreak = 0;	/* the last seen space char */
	int	wordBreakCount = 0;	/* # of blanks in this line */

	/* loop over the input string */
        while (curTextPos < len)
        {
	    if (curCharsInLine >= MAX_CHARS_IN_LINE)
		pdc_error(p->pdc, PDC_E_ILLARG_TOOLONG, "(text line)",
		    pdc_errprintf(p->pdc, "%d", MAX_CHARS_IN_LINE-1), 0, 0);

	    /* abandon DOS line-ends */
	    if (text[curTextPos] == PDF_RETURN &&
		text[curTextPos+1] == PDF_NEWLINE)
		    curTextPos++;

	    /* if it's a forced line break draw the line */
	    if (text[curTextPos] == PDF_NEWLINE ||
		text[curTextPos] == PDF_RETURN)
            {
                cury -= p->ydirection * currto->leading;

                if (p->ydirection * (cury - bottom) < 0) {
		    prematureexit = pdc_true;	/* box full */
		    break;
		}

                linebuf[curCharsInLine] = 0;    /* terminate the line */

		/* check whether the line is too long */
                wordspacing = 0;
                pdf_set_tstate(p, wordspacing, to_wordspacing);
                textwidth = pdf_swidth(p, linebuf);

		/* the forced break occurs too late for this line */
		if (textwidth > width)
                {
		    if (wordBreakCount == 0) {	/* no blank found */
			prematureexit = pdc_true;
			break;
		    }
                    linebuf[lastWordBreak] = 0;   /* terminate at last blank */
		    if (curTextPos > 0 && text[curTextPos-1] == PDF_RETURN)
			--curTextPos;
		    curTextPos -= (curCharsInLine - lastWordBreak);

		    if (!blind)
                    {
                        textwidth = pdf_swidth(p, linebuf);
			if (wordBreakCount != 1 &&
                                (mode == text_justify ||
                                 mode == text_fulljustify))
                        {
                            wordspacing = (width - textwidth) /
                                ((wordBreakCount - 1) * currto->horizscaling);
			}
                        pdf_show_aligned(p, linebuf, curx, cury, wordspacing,
                                         mode);
		    }
		}
                else if (!blind)
                {
                    if (mode == text_fulljustify && wordBreakCount > 0)
                    {
                        wordspacing = (width - textwidth) /
                                  (wordBreakCount * currto->horizscaling);
		    }
                    pdf_show_aligned(p, linebuf, curx, cury, wordspacing, mode);
		}

		lastdone = curTextPos;
		curCharsInLine = lastWordBreak = wordBreakCount = 0;
		curTextPos++;

	    }
            else if (text[curTextPos] == PDF_SPACE)
            {
                linebuf[curCharsInLine] = 0;    /* terminate the line */

		/* line too long ==> break at last blank */
                wordspacing = 0;
                pdf_set_tstate(p, wordspacing, to_wordspacing);
                if (pdf_swidth(p, linebuf) > width)
                {
                    cury -= p->ydirection * currto->leading;

                    if (p->ydirection * (cury - bottom) < 0)
                    {
			prematureexit = pdc_true; 	/* box full */
			break;
		    }

                    linebuf[lastWordBreak] = 0; /* terminate at last blank */
		    curTextPos -= (curCharsInLine - lastWordBreak - 1);

		    if (lastWordBreak == 0)
			curTextPos--;

		    /* LATER: * force break if wordBreakCount == 1,
		     * i.e., no blank
		     */
		    if (wordBreakCount == 0)
                    {
			prematureexit = pdc_true;
			break;
		    }

		    /* adjust word spacing for full justify */
                    if (wordBreakCount != 1 && (mode == text_justify ||
                                                mode == text_fulljustify))
                    {
                        textwidth = pdf_swidth(p, linebuf);
                        wordspacing = (width - textwidth) /
                            ((wordBreakCount - 1) * currto->horizscaling);
		    }

		    lastdone = curTextPos;
		    if (!blind)
                    {
                        pdf_show_aligned(p, linebuf, curx, cury, wordspacing,
                                         mode);
                    }
		    curCharsInLine = lastWordBreak = wordBreakCount = 0;
		}
                else
                {
		    /* blank found, and line still fits */
		    wordBreakCount++;
		    lastWordBreak = curCharsInLine;
		    linebuf[curCharsInLine++] = text[curTextPos++];
		}
	    }
            else
            {
		/* regular character ==> store in buffer */
		linebuf[curCharsInLine++] = text[curTextPos++];
	    }
	}

	if (prematureexit) {
	    break;		/* box full */
	}

	/* if there is anything left in the buffer, draw it */
        if (curTextPos >= len && curCharsInLine != 0)
        {
            cury -= p->ydirection * currto->leading;

            if (p->ydirection * (cury - bottom) < 0)
            {
		prematureexit = pdc_true; 	/* box full */
		break;
	    }

            linebuf[curCharsInLine] = 0;        /* terminate the line */

	    /* check if the last line is too long */
            wordspacing = 0;
            pdf_set_tstate(p, wordspacing, to_wordspacing);
            textwidth = pdf_swidth(p, linebuf);

	    if (textwidth > width)
            {
		if (wordBreakCount == 0)
		{
		    prematureexit = pdc_true;
		    break;
		}

                linebuf[lastWordBreak] = 0;     /* terminate at last blank */
		curTextPos -= (curCharsInLine - lastWordBreak - 1);

		/* recalculate the width */
                textwidth = pdf_swidth(p, linebuf);

		/* adjust word spacing for full justify */
                if (wordBreakCount != 1 && (mode == text_justify ||
                                            mode == text_fulljustify))
                {
                    wordspacing = (width - textwidth) /
                        ((wordBreakCount - 1) * currto->horizscaling);
		}
	    }
            else if (!blind)
            {
                if (mode == text_fulljustify && wordBreakCount)
                {
                    wordspacing = (width - textwidth) /
                              (wordBreakCount * currto->horizscaling);
                }
            }

	    lastdone = curTextPos;
	    if (!blind)
            {
                pdf_show_aligned(p, linebuf, curx, cury, wordspacing, mode);
            }
	    curCharsInLine = lastWordBreak = wordBreakCount = 0;
	}
    }

    pdf_set_tstate(p, old_wordspacing, to_wordspacing);

    /* return number of remaining characters  */

    while (text[lastdone] == PDF_SPACE)
	++lastdone;

    if ((text[lastdone] == PDF_RETURN ||
	text[lastdone] == PDF_NEWLINE) && text[lastdone+1] == 0)
	    ++lastdone;

    if (toconv)
        currto->textformat = old_textformat;

    return (int) (len - lastdone);
}
