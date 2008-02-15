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

/* $Id: p_cid.c,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * PDFlib CID font handling routines
 *
 */

#include "p_intern.h"
#include "p_font.h"
#include "p_cid.h"

/* returns encoding:
** pdc_invalidenc: predefined CMap not found
** pdc_cide: predefined CMap found
**
** *o_slot:
** >=  0:  standard font found
** == -1:  predefined CMap found but no standard font
** <= -2:  error occured
*/
pdc_encoding
pdf_handle_cidfont(PDF *p, const char *fontname,
                   const char *encoding, int *o_slot)
{
    pdc_font *font = &p->fonts[p->fonts_number];
    int slot, cmap, metric, len;

    *o_slot = -1;

    /*
     * Look whether font is already in the cache.
     * If font with same name and encoding is found,
     * return its slot number.
     */

    for (slot = 0; slot < p->fonts_number; slot++)
    {
        if (p->fonts[slot].encoding == pdc_cid &&
            p->fonts[slot].style == font->style &&
            !strcmp(p->fonts[slot].name, fontname) &&
            !strcmp(p->fonts[slot].cmapname, encoding))
        {
            *o_slot = slot;
            return pdc_cid;
        }
    }

    /* Check the requested CMap */
    for (cmap = 0; cmap < NUMBER_OF_CMAPS; cmap++)
        if (!strcmp(cmaps[cmap].name, encoding))
            break;

    /* Unknown predefined CMap */
    if (cmap == NUMBER_OF_CMAPS)
        return pdc_invalidenc;

    /* Check whether this CMap is supported in the desired PDF version */
    if (cmaps[cmap].compatibility > p->compatibility)
    {
	pdc_set_errmsg(p->pdc, PDF_E_DOC_PDFVERSION,
            encoding, pdc_get_pdfversion(p->pdc, p->compatibility), 0, 0);
        *o_slot = -PDF_E_DOC_PDFVERSION;
        return pdc_cid;
    }

    /* For Unicode capable language wrappers only UCS2 CMaps allowed */
    if (cmaps[cmap].codesize == 0 && p->textformat == pdc_auto2)
    {
        pdc_set_errmsg(p->pdc, PDF_E_FONT_NEEDUCS2, encoding, fontname, 0, 0);
        *o_slot = -PDF_E_FONT_NEEDUCS2;
        return pdc_cid;
    }

    /* Check whether the font name is among the known CID fonts */
    for (metric = 0; metric < SIZEOF_CID_METRICS; metric++)
        if (!strcmp(pdf_cid_metrics[metric].name, fontname))
            break;

    /* Selected CMap and known standard font don't match */
    if (metric < SIZEOF_CID_METRICS &&
        cmaps[cmap].charcoll != (int) cc_identity &&
        (cmaps[cmap].charcoll != abs(pdf_cid_metrics[metric].charcoll) ||
         (pdf_cid_metrics[metric].charcoll == (int) cc_japanese &&
          cmaps[cmap].codesize == -2)))
    {
        pdc_set_errmsg(p->pdc, PDF_E_CJK_UNSUPP_CHARCOLL, fontname, encoding,
                       0, 0);
        *o_slot = -PDF_E_CJK_UNSUPP_CHARCOLL;
        return pdc_cid;
    }

    /* Embedding not possible */
    if (metric < SIZEOF_CID_METRICS && font->embedding == pdc_true)
    {
        pdc_set_errmsg(p->pdc, PDF_E_FONT_EMBEDCMAP, encoding, 0, 0, 0);
        *o_slot = -PDF_E_FONT_EMBEDCMAP;
        return pdc_cid;
    }

    /* Code size of CMap */
    font->codeSize = 0;
    font->numOfCodes = 0;

    /* CMap and default settings */
    len = (int) strlen(cmaps[cmap].name);
    if (cmaps[cmap].name[len-1] == 'V')
        font->vertical = pdc_true;
    font->cmapname = pdc_strdup(p->pdc, encoding);
    font->encoding = pdc_cid;
    font->unicodemap = pdc_false;
    font->autocidfont = pdc_false;
    if (metric < SIZEOF_CID_METRICS || font->codeSize == 0)
    {
        font->subsetting = pdc_false;
        font->autosubsetting = pdc_false;
        font->kerning = pdc_false;
    }

    /* found standard font */
    if (metric < SIZEOF_CID_METRICS)
    {
        *o_slot = p->fonts_number;
        p->fonts_number++;

        /* Fill up the font struct */
        pdc_fill_font_metric(p->pdc, font, &pdf_cid_metrics[metric]);

        /* Now everything is fine; fill the remaining entries */
        font->standardcjk = pdc_true;
        font->ttname = pdc_strdup(p->pdc, fontname);
        font->apiname = pdc_strdup(p->pdc, fontname);
        font->obj_id = pdc_alloc_id(p->out);

        pdf_font_issemantic(p, font);
    }

    font->charcoll = (int) cmaps[cmap].charcoll;

    pdc_trace_protocol(p->pdc, 1, trc_font,
        "\n\t%s CJK font: \"%s\"\n\tPredefined CMap: \"%s\"\n"
        "\tOrdering: \"%s\"\n\tSupplement: %d\n",
        font->standardcjk ? "Adobe Standard" : "Custom", fontname, encoding,
        pdf_get_ordering_cid(p, font), pdf_get_supplement_cid(p, font));

    return pdc_cid;
}

const char *
pdf_get_ordering_cid(PDF *p, pdc_font *font)
{
    (void) p;
    return pdc_get_keyword((int) font->charcoll, charcoll_names);
}

int
pdf_get_charcoll(const char *ordering)
{
    int charcoll;

    charcoll = (int) pdc_get_keycode(ordering, charcoll_names);

    if (charcoll == PDC_KEY_NOTFOUND)
        return cc_none;
    else
        return charcoll;
}

int
pdf_get_supplement_cid(PDF *p, pdc_font *font)
{
    int cmap, supplement;

    for (cmap = 0; cmap < NUMBER_OF_CMAPS; cmap++)
        if (!strcmp(cmaps[cmap].name, font->cmapname))
            break;

    switch (p->compatibility)
    {
        case PDC_1_3:
        supplement = cmaps[cmap].supplement13;
        break;

        default:
        supplement = cmaps[cmap].supplement14;
        break;
    }

    return supplement;
}

const char *
pdf_get_abb_cjk_fontname(const char *fontname)
{
    int slot;

    for (slot = 0;
         slot < (int)(sizeof(pdf_abb_cjk_names) / sizeof(pdf_abb_cjk_names[0]));
         slot++)
    {
        if (!strcmp(pdf_cid_metrics[slot].name, fontname))
            return pdf_abb_cjk_names[slot];
    }

    return NULL;
}


#ifdef PDF_CJKFONTWIDTHS_SUPPORTED
void
pdf_put_cidglyph_widths(PDF *p, pdc_font *font)
{
    int slot;

    /* Check whether the font name is among the known CID fonts */
    for (slot = 0; slot < SIZEOF_CID_METRICS; slot++) {
        if (!strcmp(pdf_cid_metrics[slot].name, font->apiname))
            break;
    }
    pdc_printf(p->out, "/DW %d\n",
        font->monospace ? font->monospace : PDF_DEFAULT_CIDWIDTH);
    if (!font->monospace)
    {
        int slot4 = 4 * slot;
        if (slot < SIZEOF_CID_METRICS && pdf_cid_width_arrays[slot4])
        {
            pdc_puts(p->out, pdf_cid_width_arrays[slot4 + 0]);
            pdc_puts(p->out, pdf_cid_width_arrays[slot4 + 1]);
            pdc_puts(p->out, pdf_cid_width_arrays[slot4 + 2]);
            pdc_puts(p->out, pdf_cid_width_arrays[slot4 + 3]);
        }
    }
}
#endif /* PDF_CJKFONTWIDTHS_SUPPORTED */
