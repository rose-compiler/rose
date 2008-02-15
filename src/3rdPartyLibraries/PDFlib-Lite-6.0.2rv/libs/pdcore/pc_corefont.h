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

/* $Id: pc_corefont.h,v 1.1 2005/10/11 17:18:13 vuduc2 Exp $
 *
 * Core font data structures and routines
 *
 */

#include "pc_font.h"

#ifndef PC_COREFONT_H
#define PC_COREFONT_H

/* Code interval width data structure */
struct pdc_interwidth_s
{
    pdc_ushort startcode;   /* start code of interval */
    pdc_ushort width;       /* width of glyphs in the code interval */
};

/* The in-core PDFlib font metric structure */
struct pdc_core_metric_s {
    char *name;                         /* font name */
    unsigned long flags;                /* font flags for font descriptor */
    pdc_fonttype type;                  /* type of font */
    int charcoll;                       /* CID character collection supported */
                                        /* < 0: Halfwidth Latin-1 character */

    pdc_scalar  italicAngle;            /* AFM key: ItalicAngle */
    int         isFixedPitch;           /* AFM key: IsFixedPitch */
    pdc_scalar  llx;                    /* AFM key: FontBBox */
    pdc_scalar  lly;                    /* AFM key: FontBBox */
    pdc_scalar  urx;                    /* AFM key: FontBBox */
    pdc_scalar  ury;                    /* AFM key: FontBBox */
    int         underlinePosition;      /* AFM key: UnderlinePosition */
    int         underlineThickness;     /* AFM key: UnderlineThickness */
    int         capHeight;              /* AFM key: CapHeight */
    int         xHeight;                /* AFM key: XHeight */
    int         ascender;               /* AFM key: Ascender */
    int         descender;              /* AFM key: Descender */
    int         StdVW;                  /* AFM key: StdVW */
    int         StdHW;                  /* AFM key: StdHW */
    int                 numOfInter;     /* # of entries in code intervals */
    pdc_interwidth      *ciw;           /* ptr to code intervals array */
    int                 numOfGlyphs;    /* # of entries in glyph widths */
    pdc_glyphwidth      *glw;           /* ptr to glyph widths array */

};

#endif /* PC_COREFONT_H */
