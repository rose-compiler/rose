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

/* p_pfm.c
 *
 * PDFlib routines for fast reading of PFM font metrics files
 *
 */

#include <string.h>

#include "p_intern.h"
#include "p_font.h"

/* read data types from the PFM */
#define PFM_BYTE(offset)  pfm[offset]
#define PFM_WORD(offset)  ((unsigned int) ((pfm[offset+1] << 8) + pfm[offset]))
#define PFM_SHORT(offset) ((int) ((((char*) pfm)[offset+1] << 8) + pfm[offset]))
#define PFM_DWORD(offset) ((unsigned long) ((PFM_WORD(offset+2)<<16) + PFM_WORD(offset)))

/* Offsets in the buffer containing the various PFM structures */
#define header_base		0
#define header_dfVersion	(PFM_WORD(header_base + 0))
#define header_dfSize		(PFM_DWORD(header_base + 2))
#define header_dfAscent		(PFM_WORD(header_base + 74))
#define header_dfItalic		(PFM_BYTE(header_base + 80))
#define header_dfWeight		(PFM_WORD(header_base + 83))
#define header_dfCharSet	(PFM_BYTE(header_base + 85))
#define header_dfPitchAndFamily	(PFM_BYTE(header_base + 90))
#define header_dfMaxWidth	(PFM_WORD(header_base + 93))
#define header_dfFirstChar	(PFM_BYTE(header_base + 95))
#define header_dfLastChar	(PFM_BYTE(header_base + 96))
#define header_dfDefaultChar	(PFM_BYTE(header_base + 97))

#define ext_base		117
#define ext_dfExtentTable	(PFM_DWORD(ext_base + 6))
#define ext_dfDriverInfo	(PFM_DWORD(ext_base + 22))

#define etm_base		147
#define etmCapHeight		(PFM_SHORT(etm_base + 14))
#define etmXHeight		(PFM_SHORT(etm_base + 16))
#define etmLowerCaseAscent	(PFM_SHORT(etm_base + 18))
#define etmLowerCaseDescent	(PFM_SHORT(etm_base + 20))
#define etmSlant		(PFM_SHORT(etm_base + 22))
#define etmUnderlineOffset	(PFM_SHORT(etm_base + 32))
#define etmUnderlineWidth	(PFM_SHORT(etm_base + 34))

#define dfDevice		199

/* Windows font descriptor flags */
#define PDF_FIXED_PITCHi	0x01  /* Fixed width font -- unused */

#define PDF_DONTCARE		0x00  /* Don't care or don't know. */
#define PDF_ROMAN		0x10  /* Variable stroke width, serifed */
#define PDF_SWISS		0x20  /* Variable stroke width, sans-serifed */
#define PDF_MODERN		0x30  /* fixed pitch */
#define PDF_SCRIPT		0x40  /* Cursive, etc. */
#define PDF_DECORATIVE		0x50  /* Old English, etc. */

/* Windows character set flags */
#define PFM_ANSI_CHARSET        0	/* seems to imply codepage 1250/1252 */
#define PFM_SYMBOL_CHARSET      2
#define PFM_SHIFTJIS_CHARSET    128
#define PFM_OEM_CHARSET         255

/* Windows font weights */
#define PDF_SEMIBOLD		600
#define PDF_BOLD		700

/* default missing width */
#define POSTSCRIPT_DEFAULT_WIDTH	250

#define PDF_STRING_PostScript	((const char*) "\120\157\163\164\123\143\162\151\160\164")

/*
 * Currently we do not populate the following fields correctly:
 * - familyName
 * - fullName
 * - serif flag
 */

static pdf_bool
pdf_parse_pfm(PDF *p, FILE *fp, pdf_font *font)
{
    size_t length;
    long file_length;
    unsigned char *pfm;
    int i, dfFirstChar, dfLastChar, default_width;
    unsigned long dfExtentTable;

    if (fseek(fp, 0, SEEK_END) || (file_length = ftell(fp)) == -1 ||
	fseek(fp, 0, SEEK_SET))
	return pdf_false;

    length = (size_t) file_length;

    pfm = (unsigned char *) p->malloc(p, (size_t) length, "pdf_parse_pfm");

    /* check whether this is really a valid PostScript PFM file */
    if (fread(pfm, (size_t) length, 1, fp) != 1 || 
	(header_dfVersion != 0x100 && header_dfVersion != 0x200) ||
	dfDevice > length ||
	strncmp((const char *) pfm + dfDevice, PDF_STRING_PostScript, 10) ||
	ext_dfDriverInfo > length ||
	ext_dfExtentTable + 2 * (header_dfLastChar - header_dfFirstChar) + 1 > length) {
	    p->free(p, pfm);
	    pdf_error(p, PDF_RuntimeError, "Corrupt PFM file");
    }

    if (header_dfSize != length) {
	pdf_error(p, PDF_NonfatalError, "Corrupt PFM file, proceeding anyway");
    }

    /* initialize font struct */
    pdf_init_font_struct(p, font);

    /* fetch relevant data from the PFM */

    font->afm 			= pdf_false;

#define PDF_STRING_Bold		"\102\157\154\144"
#define PDF_STRING_Semibold	"\123\145\155\151\142\157\154\144"
#define PDF_STRING_Medium	"\115\145\144\151\165\155"

    /* heuristic for estimating the stem width from the weight */
    if (header_dfWeight >= PDF_BOLD) {
	font->weight	= pdf_strdup(p, PDF_STRING_Bold);
	font->StdVW	= DEFAULT_STEMWIDTH_BOLD;
    } else if (header_dfWeight >= PDF_SEMIBOLD) {
	font->weight	= pdf_strdup(p, PDF_STRING_Semibold);
	font->StdVW	= DEFAULT_STEMWIDTH_SEMI;
    } else {
	font->weight	= pdf_strdup(p, PDF_STRING_Medium);
	font->StdVW	= DEFAULT_STEMWIDTH;
    }

    switch (header_dfPitchAndFamily & 0xF0) {
	case PDF_ROMAN:
	    font->flags |= SERIF;
	    break;
	case PDF_MODERN:
	    /* Has to be ignored, contrary to MS's specs */
	    break;
	case PDF_SCRIPT:
	    font->flags |= SCRIPT;
	    break;
	case PDF_DECORATIVE:
	    /* the dfCharSet flag lies in this case... */
	    header_dfCharSet = PFM_SYMBOL_CHARSET;
	    break;
	case PDF_SWISS:
	case PDF_DONTCARE:
	default:
	    break;
    }
    
    switch (header_dfCharSet) {
	case PFM_ANSI_CHARSET:
	    font->encoding	= winansi;
	    font->encodingScheme= pdf_strdup(p, "AdobeStandardEncoding");
	    break;
	case PFM_SYMBOL_CHARSET:
	    font->encoding	= builtin;
	    font->encodingScheme= pdf_strdup(p, "FontSpecific");
	    break;
	default:
	    return pdf_false;
    }

    font->name 		= pdf_strdup(p, (const char *)pfm + ext_dfDriverInfo);
    font->fullName	= pdf_strdup(p, font->name);	/* fake */
    font->familyName	= pdf_strdup(p, font->name);	/* fake */

    dfFirstChar = header_dfFirstChar;
    dfLastChar  = header_dfLastChar;
    dfExtentTable = ext_dfExtentTable;

    /* default values -- don't take the width of the default character */
    default_width = POSTSCRIPT_DEFAULT_WIDTH;

    for (i = 0; i < 256; i++)
	font->widths[i] = default_width;

    for (i = dfFirstChar; i <= dfLastChar; i++)
	font->widths[i] = (int) PFM_WORD(dfExtentTable + 2 * (i - dfFirstChar));

    /* check for monospaced font (the fixed pitch flag lies at us) */
    default_width = font->widths[dfFirstChar];

    for (i = dfFirstChar+1; i <= dfLastChar; i++)
	if (default_width != font->widths[i])
	    break;

    if (i == dfLastChar + 1)
	font->isFixedPitch = pdf_true;

    font->italicAngle		= (header_dfItalic ? etmSlant/((float) 10.0) : (float) 0.0);
    font->capHeight		= etmCapHeight;
    font->xHeight		= etmXHeight;
    font->descender		= -etmLowerCaseDescent;
    font->ascender		= (int) header_dfAscent;

    font->underlinePosition	= -etmUnderlineOffset;
    font->underlineThickness	= etmUnderlineWidth;

    font->llx			= (float) -200;
    font->lly			= (float) font->descender;
    font->urx			= (float) header_dfMaxWidth;
    font->ury			= (float) font->ascender;

    p->free(p, pfm);

    return pdf_true;
}

pdf_bool
pdf_get_metrics_pfm(PDF *p, pdf_font *font, const char *fontname, int enc, const char *filename)
{
    FILE	*pfmfile;

    /* open PFM file */
    if ((pfmfile = fopen(filename, READMODE)) == NULL) {
	return pdf_false;
    }

    /* Read PFM metrics */
    if (pdf_parse_pfm(p, pfmfile, font) == pdf_false) {
	fclose(pfmfile);
	pdf_error(p, PDF_RuntimeError, "Error parsing PFM file %s", filename);
    }

    fclose(pfmfile);

    if (enc != winansi && font->encoding == winansi) {
	pdf_error(p, PDF_NonfatalError,
	    "Can't reencode Windows font '%s' (using winansi)", font->name);
	enc = winansi;
    }
    if (enc != builtin && font->encoding == builtin) {
	pdf_error(p, PDF_NonfatalError,
	    "Can't reencode Symbol font '%s' (using builtin)", font->name);
    }

    if (fontname && strcmp(font->name, fontname)) {
	pdf_error(p, PDF_NonfatalError,
		"Font name mismatch in PFM file %s", filename);
    }

    pdf_make_fontflags(p, font);

    return pdf_true;
}
