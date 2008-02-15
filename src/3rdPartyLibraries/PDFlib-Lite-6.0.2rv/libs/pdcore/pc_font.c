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

/* $Id: pc_font.c,v 1.1 2005/10/11 17:18:13 vuduc2 Exp $
 *
 * Basic font functions
 *
 */

#include "pc_util.h"
#include "pc_font.h"

void
pdc_init_font_struct(pdc_core *pdc, pdc_font *font)
{
    /*
     * Fill in some reasonable default values in global font info in
     * case they're missing from the metrics data.
     */

    (void) pdc;

    memset(font, 0, sizeof(pdc_font));

    font->name                  = NULL;
    font->apiname               = NULL;
    font->utf8name              = NULL;
    font->ttname                = NULL;
    font->fontfilename          = NULL;
    font->cmapname              = NULL;
    font->encapiname            = NULL;
    font->glw                   = NULL;
    font->pkd                   = NULL;
    font->GID2Name              = NULL;
    font->GID2code              = NULL;
    font->code2GID              = NULL;
    font->usedGIDs              = NULL;
    font->widthsTab             = NULL;
    font->widths                = NULL;
    font->usedChars             = NULL;
    font->imgname               = NULL;
    font->img                   = NULL;
    font->t3font                = NULL;

    font->verbose               = pdc_true;
    font->verbose_open          = pdc_true;
    font->obj_id                = PDC_BAD_ID;
    font->type                  = pdc_Type1;
    font->style                 = pdc_Normal;
    font->encoding              = pdc_builtin;
    font->towinansi             = pdc_invalidenc;
    font->charcoll              = (int) cc_none;

    font->italicAngle           = 0;
    font->isFixedPitch          = pdc_false;
    font->llx                   = -50;
    font->lly                   = -200;
    font->urx                   = 1000;
    font->ury                   = 900;
    font->underlinePosition     = -100;
    font->underlineThickness    = 50;
    font->capHeight             = 700;
    font->xHeight               = 0;
    font->ascender              = 800;
    font->descender             = -200;
    font->StdVW                 = 0;
    font->StdHW                 = 0;
    font->monospace             = 0;

    font->codeSize              = 1;
    font->numOfCodes            = 256;
    font->lastCode              = -1;

#if defined(MAC) || defined(MACOSX)
    font->hostfont              = 0;
    font->macfontstyle          = 0;
    font->fondlen               = 0;
    font->fond                  = NULL;
#endif
}

void
pdc_cleanup_font_struct(pdc_core *pdc, pdc_font *font)
{
    int i;

    if (font == NULL)
        return;

    if (font->img != NULL && font->imgname == NULL)
    {
        pdc_free(pdc, font->img);
        font->img = NULL;
    }

    if (font->imgname != NULL)
    {
        pdc_free(pdc, font->imgname);
        font->imgname = NULL;
    }

    if (font->name != NULL)
    {
        pdc_free(pdc, font->name);
        font->name = NULL;
    }

    if (font->apiname != NULL)
    {
        pdc_free(pdc, font->apiname);
        font->apiname = NULL;
    }

    if (font->utf8name != NULL)
    {
        pdc_free(pdc, font->utf8name);
        font->utf8name = NULL;
    }

    if (font->ttname != NULL)
    {
        pdc_free(pdc, font->ttname);
        font->ttname = NULL;
    }

    if (font->fontfilename != NULL)
    {
        pdc_free(pdc, font->fontfilename);
        font->fontfilename = NULL;
    }

    if (font->cmapname != NULL)
    {
        pdc_free(pdc, font->cmapname);
        font->cmapname = NULL;
    }

    if (font->encapiname != NULL)
    {
        pdc_free(pdc, font->encapiname);
        font->encapiname = NULL;
    }

    if (font->glw != NULL)
    {
        pdc_free(pdc, font->glw);
        font->glw = NULL;
    }

    if (font->pkd != NULL)
    {
        pdc_free(pdc, font->pkd);
        font->pkd = NULL;
    }

    if (font->GID2Name != NULL)
    {
        if (font->names_tbf)
        {
            for (i = 0; i < font->numOfGlyphs; i++)
            {
                if (font->GID2Name[i] != NULL)
                {
                    pdc_free(pdc, font->GID2Name[i]);
                    font->GID2Name[i] = NULL;
                }
            }
        }
        pdc_free(pdc, font->GID2Name);
        font->GID2Name = NULL;
    }

    if (font->GID2code != NULL)
    {
        pdc_free(pdc, font->GID2code);
        font->GID2code = NULL;
    }

    if (font->code2GID != NULL)
    {
        pdc_free(pdc, font->code2GID);
        font->code2GID = NULL;
    }

    if (font->usedGIDs != NULL)
    {
        pdc_free(pdc, font->usedGIDs);
        font->usedGIDs = NULL;
    }

    if (font->widthsTab != NULL)
    {
        pdc_free(pdc, font->widthsTab);
        font->widthsTab = NULL;
    }

    if (font->widths != NULL)
    {
        pdc_free(pdc, font->widths);
        font->widths = NULL;
    }

    if (font->usedChars != NULL)
    {
        pdc_free(pdc, font->usedChars);
        font->usedChars = NULL;
    }

    if (font->t3font != NULL)
    {
        pdc_cleanup_t3font_struct(pdc, font->t3font);
        pdc_free(pdc, font->t3font);
        font->t3font = NULL;
    }

#if defined(MAC) || defined(MACOSX)
    if (font->fond != NULL)
    {
        pdc_free(pdc, font->fond);
        font->fond = NULL;
    }
#endif

}

void
pdc_font_trace_protocol(pdc_core *pdc, pdc_font *font)
{
    if (font != NULL &&
        pdc_trace_protocol_is_enabled(pdc, 2, trc_font))
    {
        pdc_trace(pdc,
                  "\t\tFlags: %d\n"
                  "\t\tFontBBox: %g,%g  %g,%g\n"
                  "\t\titalicAngle: %g\n"
                  "\t\tisFixedPitch: %d\n"
                  "\t\tunderlinePosition: %d\n"
                  "\t\tunderlineThickness: %d\n"
                  "\t\tcapHeight: %d\n"
                  "\t\txHeight: %d\n"
                  "\t\tascender: %d\n"
                  "\t\tdescender: %d\n"
                  "\t\tStdVW: %d\n"
                  "\t\tdefWidth: %d\n",
                  font->flags,
                  font->llx, font->lly, font->urx, font->ury,
                  font->italicAngle, font->isFixedPitch,
                  font->underlinePosition, font->underlineThickness,
                  font->capHeight, font->xHeight, font->ascender,
                  font->descender, font->StdVW, font->StdHW,
                  font->defWidth);
    }
}

void
pdc_cleanup_t3font_struct(pdc_core *pdc, pdc_t3font *t3font)
{
    int i;

    if (t3font->fontname != NULL)
        pdc_free(pdc, t3font->fontname);

    for (i = 0; i < t3font->next_glyph; i++) {
        if (t3font->glyphs[i].name)
            pdc_free(pdc, t3font->glyphs[i].name);
    }

    pdc_free(pdc, t3font->glyphs);
}
