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

/* p_metrics.h
 *
 * Built-in font info for the 14 base fonts of PDF.
 * These "in-core" metrics may be extended beyond the base-14 fonts
 * with a supplied utility which generates this header from an
 * arbitrary number of AFMs or PFMs.
 *
 * This file contains the metrics for the 14 base fonts for ebcdic,
 * winansi and macroman encodings. According to the target platform,
 * only one set of metrics will be used:
 *
 * PDFLIB_EBCDIC: ebcdic encoding
 * MAC: macroman encoding
 * all other platforms: winansi encoding
 *
 */

#ifndef P_METRICS_H
#define P_METRICS_H

#if defined(PDFLIB_EBCDIC)

/*
 * Font metrics generated with compile_metrics utility
 *
 * Encoding used: ebcdic
 */

/* -------- Generated from metrics file Courier-Bold.afm -------- */
{	"Courier-Bold",			/* FontName */
	ebcdic,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	262177L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Courier Bold",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Courier",			/* Family name */
	"Bold",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	1,				/* isFixedPitch */
	-113,				/* llx */
	-250,				/* lly */
	749,				/* urx */
	801,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	562,				/* CapHeight */
	439,				/* xHeight */
	629,				/* Ascender */
	-157,				/* Descender */
	106,				/* StdVW */
	84,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x27 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x2f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x37 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x3f */
	600,	250,	250,	250,	250,	250,	250,	250, /* 0x47 */
	250,	250,	600,	600,	600,	600,	600,	600, /* 0x4f */
	600,	250,	250,	250,	250,	250,	250,	250, /* 0x57 */
	250,	250,	600,	600,	600,	600,	600,	600, /* 0x5f */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0x67 */
	250,	250,	600,	600,	600,	600,	600,	600, /* 0x6f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x77 */
	250,	600,	600,	600,	600,	600,	600,	600, /* 0x7f */
	250,	600,	600,	600,	600,	600,	600,	600, /* 0x87 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0x8f */
	250,	600,	600,	600,	600,	600,	600,	600, /* 0x97 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0x9f */
	250,	600,	600,	600,	600,	600,	600,	600, /* 0xa7 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0xaf */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xb7 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xbf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xc7 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0xcf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xd7 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0xdf */
	600,	250,	600,	600,	600,	600,	600,	600, /* 0xe7 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0xef */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xf7 */
	600,	600,	250,	250,	250,	250,	250,	250  /* 0xff */
}
},

/* -------- Generated from metrics file Courier-BoldOblique.afm -------- */
{	"Courier-BoldOblique",			/* FontName */
	ebcdic,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	262241L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Courier Bold Oblique",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Courier",			/* Family name */
	"Bold",				/* Weight */
	cc_none,				/* Character collection */
	(float) -12.0,			/* ItalicAngle */
	1,				/* isFixedPitch */
	-57,				/* llx */
	-250,				/* lly */
	869,				/* urx */
	801,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	562,				/* CapHeight */
	439,				/* xHeight */
	629,				/* Ascender */
	-157,				/* Descender */
	106,				/* StdVW */
	84,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x27 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x2f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x37 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x3f */
	600,	250,	250,	250,	250,	250,	250,	250, /* 0x47 */
	250,	250,	600,	600,	600,	600,	600,	600, /* 0x4f */
	600,	250,	250,	250,	250,	250,	250,	250, /* 0x57 */
	250,	250,	600,	600,	600,	600,	600,	600, /* 0x5f */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0x67 */
	250,	250,	600,	600,	600,	600,	600,	600, /* 0x6f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x77 */
	250,	600,	600,	600,	600,	600,	600,	600, /* 0x7f */
	250,	600,	600,	600,	600,	600,	600,	600, /* 0x87 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0x8f */
	250,	600,	600,	600,	600,	600,	600,	600, /* 0x97 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0x9f */
	250,	600,	600,	600,	600,	600,	600,	600, /* 0xa7 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0xaf */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xb7 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xbf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xc7 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0xcf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xd7 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0xdf */
	600,	250,	600,	600,	600,	600,	600,	600, /* 0xe7 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0xef */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xf7 */
	600,	600,	250,	250,	250,	250,	250,	250  /* 0xff */
}
},

/* -------- Generated from metrics file Courier-Oblique.afm -------- */
{	"Courier-Oblique",			/* FontName */
	ebcdic,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	97L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Courier Oblique",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Courier",			/* Family name */
	"Medium",				/* Weight */
	cc_none,				/* Character collection */
	(float) -12.0,			/* ItalicAngle */
	1,				/* isFixedPitch */
	-27,				/* llx */
	-250,				/* lly */
	742,				/* urx */
	805,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	562,				/* CapHeight */
	426,				/* xHeight */
	629,				/* Ascender */
	-157,				/* Descender */
	51,				/* StdVW */
	51,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x27 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x2f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x37 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x3f */
	600,	250,	250,	250,	250,	250,	250,	250, /* 0x47 */
	250,	250,	600,	600,	600,	600,	600,	600, /* 0x4f */
	600,	250,	250,	250,	250,	250,	250,	250, /* 0x57 */
	250,	250,	600,	600,	600,	600,	600,	600, /* 0x5f */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0x67 */
	250,	250,	600,	600,	600,	600,	600,	600, /* 0x6f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x77 */
	250,	600,	600,	600,	600,	600,	600,	600, /* 0x7f */
	250,	600,	600,	600,	600,	600,	600,	600, /* 0x87 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0x8f */
	250,	600,	600,	600,	600,	600,	600,	600, /* 0x97 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0x9f */
	250,	600,	600,	600,	600,	600,	600,	600, /* 0xa7 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0xaf */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xb7 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xbf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xc7 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0xcf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xd7 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0xdf */
	600,	250,	600,	600,	600,	600,	600,	600, /* 0xe7 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0xef */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xf7 */
	600,	600,	250,	250,	250,	250,	250,	250  /* 0xff */
}
},

/* -------- Generated from metrics file Courier.afm -------- */
{	"Courier",			/* FontName */
	ebcdic,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	33L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Courier",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Courier",			/* Family name */
	"Medium",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	1,				/* isFixedPitch */
	-23,				/* llx */
	-250,				/* lly */
	623,				/* urx */
	805,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	562,				/* CapHeight */
	426,				/* xHeight */
	629,				/* Ascender */
	-157,				/* Descender */
	51,				/* StdVW */
	51,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x27 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x2f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x37 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x3f */
	600,	250,	250,	250,	250,	250,	250,	250, /* 0x47 */
	250,	250,	600,	600,	600,	600,	600,	600, /* 0x4f */
	600,	250,	250,	250,	250,	250,	250,	250, /* 0x57 */
	250,	250,	600,	600,	600,	600,	600,	600, /* 0x5f */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0x67 */
	250,	250,	600,	600,	600,	600,	600,	600, /* 0x6f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x77 */
	250,	600,	600,	600,	600,	600,	600,	600, /* 0x7f */
	250,	600,	600,	600,	600,	600,	600,	600, /* 0x87 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0x8f */
	250,	600,	600,	600,	600,	600,	600,	600, /* 0x97 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0x9f */
	250,	600,	600,	600,	600,	600,	600,	600, /* 0xa7 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0xaf */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xb7 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xbf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xc7 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0xcf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xd7 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0xdf */
	600,	250,	600,	600,	600,	600,	600,	600, /* 0xe7 */
	600,	600,	250,	250,	250,	250,	250,	250, /* 0xef */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xf7 */
	600,	600,	250,	250,	250,	250,	250,	250  /* 0xff */
}
},

/* -------- Generated from metrics file Helvetica-Bold.afm -------- */
{	"Helvetica-Bold",			/* FontName */
	ebcdic,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	262176L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Helvetica Bold",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Helvetica",			/* Family name */
	"Bold",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-170,				/* llx */
	-228,				/* lly */
	1003,				/* urx */
	962,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	718,				/* CapHeight */
	532,				/* xHeight */
	718,				/* Ascender */
	-207,				/* Descender */
	140,				/* StdVW */
	118,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x27 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x2f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x37 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x3f */
	278,	250,	250,	250,	250,	250,	250,	250, /* 0x47 */
	250,	250,	333,	278,	584,	333,	584,	333, /* 0x4f */
	722,	250,	250,	250,	250,	250,	250,	250, /* 0x57 */
	250,	250,	333,	556,	389,	333,	333,	584, /* 0x5f */
	333,	278,	250,	250,	250,	250,	250,	250, /* 0x67 */
	250,	250,	280,	278,	889,	556,	584,	611, /* 0x6f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x77 */
	250,	333,	333,	556,	975,	238,	584,	474, /* 0x7f */
	250,	556,	611,	556,	611,	556,	333,	611, /* 0x87 */
	611,	278,	250,	250,	250,	250,	250,	250, /* 0x8f */
	250,	278,	556,	278,	889,	611,	611,	611, /* 0x97 */
	611,	389,	250,	250,	250,	250,	250,	250, /* 0x9f */
	250,	584,	556,	333,	611,	556,	778,	556, /* 0xa7 */
	556,	500,	250,	250,	250,	250,	250,	250, /* 0xaf */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xb7 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xbf */
	389,	722,	722,	722,	722,	667,	611,	778, /* 0xc7 */
	722,	278,	250,	250,	250,	250,	250,	250, /* 0xcf */
	389,	556,	722,	611,	833,	722,	778,	667, /* 0xd7 */
	778,	722,	250,	250,	250,	250,	250,	250, /* 0xdf */
	278,	250,	667,	611,	722,	667,	944,	667, /* 0xe7 */
	667,	611,	250,	250,	250,	250,	250,	250, /* 0xef */
	556,	556,	556,	556,	556,	556,	556,	556, /* 0xf7 */
	556,	556,	250,	250,	250,	250,	250,	250  /* 0xff */
}
},

/* -------- Generated from metrics file Helvetica-BoldOblique.afm -------- */
{	"Helvetica-BoldOblique",			/* FontName */
	ebcdic,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	262240L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Helvetica Bold Oblique",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Helvetica",			/* Family name */
	"Bold",				/* Weight */
	cc_none,				/* Character collection */
	(float) -12.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-174,				/* llx */
	-228,				/* lly */
	1114,				/* urx */
	962,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	718,				/* CapHeight */
	532,				/* xHeight */
	718,				/* Ascender */
	-207,				/* Descender */
	140,				/* StdVW */
	118,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x27 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x2f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x37 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x3f */
	278,	250,	250,	250,	250,	250,	250,	250, /* 0x47 */
	250,	250,	333,	278,	584,	333,	584,	333, /* 0x4f */
	722,	250,	250,	250,	250,	250,	250,	250, /* 0x57 */
	250,	250,	333,	556,	389,	333,	333,	584, /* 0x5f */
	333,	278,	250,	250,	250,	250,	250,	250, /* 0x67 */
	250,	250,	280,	278,	889,	556,	584,	611, /* 0x6f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x77 */
	250,	333,	333,	556,	975,	238,	584,	474, /* 0x7f */
	250,	556,	611,	556,	611,	556,	333,	611, /* 0x87 */
	611,	278,	250,	250,	250,	250,	250,	250, /* 0x8f */
	250,	278,	556,	278,	889,	611,	611,	611, /* 0x97 */
	611,	389,	250,	250,	250,	250,	250,	250, /* 0x9f */
	250,	584,	556,	333,	611,	556,	778,	556, /* 0xa7 */
	556,	500,	250,	250,	250,	250,	250,	250, /* 0xaf */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xb7 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xbf */
	389,	722,	722,	722,	722,	667,	611,	778, /* 0xc7 */
	722,	278,	250,	250,	250,	250,	250,	250, /* 0xcf */
	389,	556,	722,	611,	833,	722,	778,	667, /* 0xd7 */
	778,	722,	250,	250,	250,	250,	250,	250, /* 0xdf */
	278,	250,	667,	611,	722,	667,	944,	667, /* 0xe7 */
	667,	611,	250,	250,	250,	250,	250,	250, /* 0xef */
	556,	556,	556,	556,	556,	556,	556,	556, /* 0xf7 */
	556,	556,	250,	250,	250,	250,	250,	250  /* 0xff */
}
},

/* -------- Generated from metrics file Helvetica-Oblique.afm -------- */
{	"Helvetica-Oblique",			/* FontName */
	ebcdic,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	96L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Helvetica Oblique",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Helvetica",			/* Family name */
	"Medium",				/* Weight */
	cc_none,				/* Character collection */
	(float) -12.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-170,				/* llx */
	-225,				/* lly */
	1116,				/* urx */
	931,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	718,				/* CapHeight */
	523,				/* xHeight */
	718,				/* Ascender */
	-207,				/* Descender */
	88,				/* StdVW */
	76,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x27 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x2f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x37 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x3f */
	278,	250,	250,	250,	250,	250,	250,	250, /* 0x47 */
	250,	250,	278,	278,	584,	333,	584,	278, /* 0x4f */
	667,	250,	250,	250,	250,	250,	250,	250, /* 0x57 */
	250,	250,	278,	556,	389,	333,	278,	469, /* 0x5f */
	333,	278,	250,	250,	250,	250,	250,	250, /* 0x67 */
	250,	250,	260,	278,	889,	556,	584,	556, /* 0x6f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x77 */
	250,	333,	278,	556,	1015,	191,	584,	355, /* 0x7f */
	250,	556,	556,	500,	556,	556,	278,	556, /* 0x87 */
	556,	222,	250,	250,	250,	250,	250,	250, /* 0x8f */
	250,	222,	500,	222,	833,	556,	556,	556, /* 0x97 */
	556,	333,	250,	250,	250,	250,	250,	250, /* 0x9f */
	250,	584,	500,	278,	556,	500,	722,	500, /* 0xa7 */
	500,	500,	250,	250,	250,	250,	250,	250, /* 0xaf */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xb7 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xbf */
	334,	667,	667,	722,	722,	667,	611,	778, /* 0xc7 */
	722,	278,	250,	250,	250,	250,	250,	250, /* 0xcf */
	334,	500,	667,	556,	833,	722,	778,	667, /* 0xd7 */
	778,	722,	250,	250,	250,	250,	250,	250, /* 0xdf */
	278,	250,	667,	611,	722,	667,	944,	667, /* 0xe7 */
	667,	611,	250,	250,	250,	250,	250,	250, /* 0xef */
	556,	556,	556,	556,	556,	556,	556,	556, /* 0xf7 */
	556,	556,	250,	250,	250,	250,	250,	250  /* 0xff */
}
},

/* -------- Generated from metrics file Helvetica.afm -------- */
{	"Helvetica",			/* FontName */
	ebcdic,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	32L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Helvetica",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Helvetica",			/* Family name */
	"Medium",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-166,				/* llx */
	-225,				/* lly */
	1000,				/* urx */
	931,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	718,				/* CapHeight */
	523,				/* xHeight */
	718,				/* Ascender */
	-207,				/* Descender */
	88,				/* StdVW */
	76,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x27 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x2f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x37 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x3f */
	278,	250,	250,	250,	250,	250,	250,	250, /* 0x47 */
	250,	250,	278,	278,	584,	333,	584,	278, /* 0x4f */
	667,	250,	250,	250,	250,	250,	250,	250, /* 0x57 */
	250,	250,	278,	556,	389,	333,	278,	469, /* 0x5f */
	333,	278,	250,	250,	250,	250,	250,	250, /* 0x67 */
	250,	250,	260,	278,	889,	556,	584,	556, /* 0x6f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x77 */
	250,	333,	278,	556,	1015,	191,	584,	355, /* 0x7f */
	250,	556,	556,	500,	556,	556,	278,	556, /* 0x87 */
	556,	222,	250,	250,	250,	250,	250,	250, /* 0x8f */
	250,	222,	500,	222,	833,	556,	556,	556, /* 0x97 */
	556,	333,	250,	250,	250,	250,	250,	250, /* 0x9f */
	250,	584,	500,	278,	556,	500,	722,	500, /* 0xa7 */
	500,	500,	250,	250,	250,	250,	250,	250, /* 0xaf */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xb7 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xbf */
	334,	667,	667,	722,	722,	667,	611,	778, /* 0xc7 */
	722,	278,	250,	250,	250,	250,	250,	250, /* 0xcf */
	334,	500,	667,	556,	833,	722,	778,	667, /* 0xd7 */
	778,	722,	250,	250,	250,	250,	250,	250, /* 0xdf */
	278,	250,	667,	611,	722,	667,	944,	667, /* 0xe7 */
	667,	611,	250,	250,	250,	250,	250,	250, /* 0xef */
	556,	556,	556,	556,	556,	556,	556,	556, /* 0xf7 */
	556,	556,	250,	250,	250,	250,	250,	250  /* 0xff */
}
},

/* -------- Generated from metrics file Symbol.afm -------- */
{	"Symbol",			/* FontName */
	builtin,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	4L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Symbol",			/* Full name */
	"FontSpecific",		/* Native encoding */
	"Symbol",			/* Family name */
	"Medium",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-180,				/* llx */
	-293,				/* lly */
	1090,				/* urx */
	1010,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	700,				/* CapHeight */
	0,				/* xHeight */
	800,				/* Ascender */
	-200,				/* Descender */
	85,				/* StdVW */
	92,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	333,	713,	500,	549,	833,	778,	439, /* 0x27 */
	333,	333,	500,	549,	250,	549,	250,	278, /* 0x2f */
	500,	500,	500,	500,	500,	500,	500,	500, /* 0x37 */
	500,	500,	278,	278,	549,	549,	549,	444, /* 0x3f */
	549,	722,	667,	722,	612,	611,	763,	603, /* 0x47 */
	722,	333,	631,	722,	686,	889,	722,	722, /* 0x4f */
	768,	741,	556,	592,	611,	690,	439,	768, /* 0x57 */
	645,	795,	611,	333,	863,	333,	658,	500, /* 0x5f */
	500,	631,	549,	549,	494,	439,	521,	411, /* 0x67 */
	603,	329,	603,	549,	549,	576,	521,	549, /* 0x6f */
	549,	521,	549,	603,	439,	576,	713,	686, /* 0x77 */
	493,	686,	494,	480,	200,	480,	549,	250, /* 0x7f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x87 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x8f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x97 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x9f */
	750,	620,	247,	549,	167,	713,	500,	753, /* 0xa7 */
	753,	753,	753,	1042,	987,	603,	987,	603, /* 0xaf */
	400,	549,	411,	549,	549,	713,	494,	460, /* 0xb7 */
	549,	549,	549,	549,	1000,	603,	1000,	658, /* 0xbf */
	823,	686,	795,	987,	768,	768,	823,	768, /* 0xc7 */
	768,	713,	713,	713,	713,	713,	713,	713, /* 0xcf */
	768,	713,	790,	790,	890,	823,	549,	250, /* 0xd7 */
	713,	603,	603,	1042,	987,	603,	987,	603, /* 0xdf */
	494,	329,	790,	790,	786,	713,	384,	384, /* 0xe7 */
	384,	384,	384,	384,	494,	494,	494,	494, /* 0xef */
	250,	329,	274,	686,	686,	686,	384,	384, /* 0xf7 */
	384,	384,	384,	384,	494,	494,	494,	250  /* 0xff */
}
},

/* -------- Generated from metrics file Times-Bold.afm -------- */
{	"Times-Bold",			/* FontName */
	ebcdic,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	262176L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Times Bold",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Times",			/* Family name */
	"Bold",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-168,				/* llx */
	-218,				/* lly */
	1000,				/* urx */
	935,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	676,				/* CapHeight */
	461,				/* xHeight */
	683,				/* Ascender */
	-217,				/* Descender */
	139,				/* StdVW */
	44,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x27 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x2f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x37 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x3f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x47 */
	250,	250,	333,	250,	570,	333,	570,	333, /* 0x4f */
	833,	250,	250,	250,	250,	250,	250,	250, /* 0x57 */
	250,	250,	333,	500,	500,	333,	333,	581, /* 0x5f */
	333,	278,	250,	250,	250,	250,	250,	250, /* 0x67 */
	250,	250,	220,	250,	1000,	500,	570,	500, /* 0x6f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x77 */
	250,	333,	333,	500,	930,	278,	570,	555, /* 0x7f */
	250,	500,	556,	444,	556,	444,	333,	500, /* 0x87 */
	556,	278,	250,	250,	250,	250,	250,	250, /* 0x8f */
	250,	333,	556,	278,	833,	556,	500,	556, /* 0x97 */
	556,	444,	250,	250,	250,	250,	250,	250, /* 0x9f */
	250,	520,	389,	333,	556,	500,	722,	500, /* 0xa7 */
	500,	444,	250,	250,	250,	250,	250,	250, /* 0xaf */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xb7 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xbf */
	394,	722,	667,	722,	722,	667,	611,	778, /* 0xc7 */
	778,	389,	250,	250,	250,	250,	250,	250, /* 0xcf */
	394,	500,	778,	667,	944,	722,	778,	611, /* 0xd7 */
	778,	722,	250,	250,	250,	250,	250,	250, /* 0xdf */
	278,	250,	556,	667,	722,	722,	1000,	722, /* 0xe7 */
	722,	667,	250,	250,	250,	250,	250,	250, /* 0xef */
	500,	500,	500,	500,	500,	500,	500,	500, /* 0xf7 */
	500,	500,	250,	250,	250,	250,	250,	250  /* 0xff */
}
},

/* -------- Generated from metrics file Times-BoldItalic.afm -------- */
{	"Times-BoldItalic",			/* FontName */
	ebcdic,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	262240L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Times Bold Italic",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Times",			/* Family name */
	"Bold",				/* Weight */
	cc_none,				/* Character collection */
	(float) -15.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-200,				/* llx */
	-218,				/* lly */
	996,				/* urx */
	921,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	669,				/* CapHeight */
	462,				/* xHeight */
	683,				/* Ascender */
	-217,				/* Descender */
	121,				/* StdVW */
	42,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x27 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x2f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x37 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x3f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x47 */
	250,	250,	333,	250,	570,	333,	570,	389, /* 0x4f */
	778,	250,	250,	250,	250,	250,	250,	250, /* 0x57 */
	250,	250,	333,	500,	500,	333,	333,	570, /* 0x5f */
	333,	278,	250,	250,	250,	250,	250,	250, /* 0x67 */
	250,	250,	220,	250,	833,	500,	570,	500, /* 0x6f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x77 */
	250,	333,	333,	500,	832,	278,	570,	555, /* 0x7f */
	250,	500,	500,	444,	500,	444,	333,	500, /* 0x87 */
	556,	278,	250,	250,	250,	250,	250,	250, /* 0x8f */
	250,	278,	500,	278,	778,	556,	500,	500, /* 0x97 */
	500,	389,	250,	250,	250,	250,	250,	250, /* 0x9f */
	250,	570,	389,	278,	556,	444,	667,	500, /* 0xa7 */
	444,	389,	250,	250,	250,	250,	250,	250, /* 0xaf */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xb7 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xbf */
	348,	667,	667,	667,	722,	667,	667,	722, /* 0xc7 */
	778,	389,	250,	250,	250,	250,	250,	250, /* 0xcf */
	348,	500,	667,	611,	889,	722,	722,	611, /* 0xd7 */
	722,	667,	250,	250,	250,	250,	250,	250, /* 0xdf */
	278,	250,	556,	611,	722,	667,	889,	667, /* 0xe7 */
	611,	611,	250,	250,	250,	250,	250,	250, /* 0xef */
	500,	500,	500,	500,	500,	500,	500,	500, /* 0xf7 */
	500,	500,	250,	250,	250,	250,	250,	250  /* 0xff */
}
},

/* -------- Generated from metrics file Times-Italic.afm -------- */
{	"Times-Italic",			/* FontName */
	ebcdic,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	96L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Times Italic",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Times",			/* Family name */
	"Medium",				/* Weight */
	cc_none,				/* Character collection */
	(float) -15.5,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-169,				/* llx */
	-217,				/* lly */
	1010,				/* urx */
	883,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	653,				/* CapHeight */
	441,				/* xHeight */
	683,				/* Ascender */
	-217,				/* Descender */
	76,				/* StdVW */
	32,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x27 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x2f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x37 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x3f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x47 */
	250,	250,	389,	250,	675,	333,	675,	333, /* 0x4f */
	778,	250,	250,	250,	250,	250,	250,	250, /* 0x57 */
	250,	250,	389,	500,	500,	333,	333,	422, /* 0x5f */
	333,	278,	250,	250,	250,	250,	250,	250, /* 0x67 */
	250,	250,	275,	250,	833,	500,	675,	500, /* 0x6f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x77 */
	250,	333,	333,	500,	920,	214,	675,	420, /* 0x7f */
	250,	500,	500,	444,	500,	444,	278,	500, /* 0x87 */
	500,	278,	250,	250,	250,	250,	250,	250, /* 0x8f */
	250,	278,	444,	278,	722,	500,	500,	500, /* 0x97 */
	500,	389,	250,	250,	250,	250,	250,	250, /* 0x9f */
	250,	541,	389,	278,	500,	444,	667,	444, /* 0xa7 */
	444,	389,	250,	250,	250,	250,	250,	250, /* 0xaf */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xb7 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xbf */
	400,	611,	611,	667,	722,	611,	611,	722, /* 0xc7 */
	722,	333,	250,	250,	250,	250,	250,	250, /* 0xcf */
	400,	444,	667,	556,	833,	667,	722,	611, /* 0xd7 */
	722,	611,	250,	250,	250,	250,	250,	250, /* 0xdf */
	278,	250,	500,	556,	722,	611,	833,	611, /* 0xe7 */
	556,	556,	250,	250,	250,	250,	250,	250, /* 0xef */
	500,	500,	500,	500,	500,	500,	500,	500, /* 0xf7 */
	500,	500,	250,	250,	250,	250,	250,	250  /* 0xff */
}
},

/* -------- Generated from metrics file Times-Roman.afm -------- */
{	"Times-Roman",			/* FontName */
	ebcdic,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	32L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Times Roman",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Times",			/* Family name */
	"Roman",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-168,				/* llx */
	-218,				/* lly */
	1000,				/* urx */
	898,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	662,				/* CapHeight */
	450,				/* xHeight */
	683,				/* Ascender */
	-217,				/* Descender */
	84,				/* StdVW */
	28,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x27 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x2f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x37 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x3f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x47 */
	250,	250,	333,	250,	564,	333,	564,	333, /* 0x4f */
	778,	250,	250,	250,	250,	250,	250,	250, /* 0x57 */
	250,	250,	333,	500,	500,	333,	278,	469, /* 0x5f */
	333,	278,	250,	250,	250,	250,	250,	250, /* 0x67 */
	250,	250,	200,	250,	833,	500,	564,	444, /* 0x6f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x77 */
	250,	333,	278,	500,	921,	180,	564,	408, /* 0x7f */
	250,	444,	500,	444,	500,	444,	333,	500, /* 0x87 */
	500,	278,	250,	250,	250,	250,	250,	250, /* 0x8f */
	250,	278,	500,	278,	778,	500,	500,	500, /* 0x97 */
	500,	333,	250,	250,	250,	250,	250,	250, /* 0x9f */
	250,	541,	389,	278,	500,	500,	722,	500, /* 0xa7 */
	500,	444,	250,	250,	250,	250,	250,	250, /* 0xaf */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xb7 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0xbf */
	480,	722,	667,	667,	722,	611,	556,	722, /* 0xc7 */
	722,	333,	250,	250,	250,	250,	250,	250, /* 0xcf */
	480,	389,	722,	611,	889,	722,	722,	556, /* 0xd7 */
	722,	667,	250,	250,	250,	250,	250,	250, /* 0xdf */
	278,	250,	556,	611,	722,	722,	944,	722, /* 0xe7 */
	722,	611,	250,	250,	250,	250,	250,	250, /* 0xef */
	500,	500,	500,	500,	500,	500,	500,	500, /* 0xf7 */
	500,	500,	250,	250,	250,	250,	250,	250  /* 0xff */
}
},

/* -------- Generated from metrics file ZapfDingbats.afm -------- */
{	"ZapfDingbats",			/* FontName */
	builtin,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	4L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"ITC Zapf Dingbats",			/* Full name */
	"FontSpecific",		/* Native encoding */
	"ITC Zapf Dingbats",			/* Family name */
	"Medium",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-1,				/* llx */
	-143,				/* lly */
	981,				/* urx */
	820,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	700,				/* CapHeight */
	0,				/* xHeight */
	800,				/* Ascender */
	-200,				/* Descender */
	90,				/* StdVW */
	28,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	278,	974,	961,	974,	980,	719,	789,	790, /* 0x27 */
	791,	690,	960,	939,	549,	855,	911,	933, /* 0x2f */
	911,	945,	974,	755,	846,	762,	761,	571, /* 0x37 */
	677,	763,	760,	759,	754,	494,	552,	537, /* 0x3f */
	577,	692,	786,	788,	788,	790,	793,	794, /* 0x47 */
	816,	823,	789,	841,	823,	833,	816,	831, /* 0x4f */
	923,	744,	723,	749,	790,	792,	695,	776, /* 0x57 */
	768,	792,	759,	707,	708,	682,	701,	826, /* 0x5f */
	815,	789,	789,	707,	687,	696,	689,	786, /* 0x67 */
	787,	713,	791,	785,	791,	873,	761,	762, /* 0x6f */
	762,	759,	759,	892,	892,	788,	784,	438, /* 0x77 */
	138,	277,	415,	392,	392,	668,	668,	250, /* 0x7f */
	390,	390,	317,	317,	276,	276,	509,	509, /* 0x87 */
	410,	410,	234,	234,	334,	334,	250,	250, /* 0x8f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x97 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x9f */
	250,	732,	544,	544,	910,	667,	760,	760, /* 0xa7 */
	776,	595,	694,	626,	788,	788,	788,	788, /* 0xaf */
	788,	788,	788,	788,	788,	788,	788,	788, /* 0xb7 */
	788,	788,	788,	788,	788,	788,	788,	788, /* 0xbf */
	788,	788,	788,	788,	788,	788,	788,	788, /* 0xc7 */
	788,	788,	788,	788,	788,	788,	788,	788, /* 0xcf */
	788,	788,	788,	788,	894,	838,	1016,	458, /* 0xd7 */
	748,	924,	748,	918,	927,	928,	928,	834, /* 0xdf */
	873,	828,	924,	924,	917,	930,	931,	463, /* 0xe7 */
	883,	836,	836,	867,	867,	696,	696,	874, /* 0xef */
	250,	874,	760,	946,	771,	865,	771,	888, /* 0xf7 */
	967,	888,	831,	873,	927,	970,	918,	250  /* 0xff */
}
},

#elif defined(MAC)

/*
 * Font metrics generated with compile_metrics utility
 *
 * Encoding used: macroman
 */

/* -------- Generated from metrics file Courier-Bold.afm -------- */
{	"Courier-Bold",			/* FontName */
	macroman,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	262177L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Courier Bold",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Courier",			/* Family name */
	"Bold",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	1,				/* isFixedPitch */
	-113,				/* llx */
	-250,				/* lly */
	749,				/* urx */
	801,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	562,				/* CapHeight */
	439,				/* xHeight */
	629,				/* Ascender */
	-157,				/* Descender */
	106,				/* StdVW */
	84,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x27 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x2f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x37 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x3f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x47 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x4f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x57 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x5f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x67 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x6f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x77 */
	600,	600,	600,	600,	600,	600,	600,	250, /* 0x7f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x87 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x8f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x97 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x9f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xa7 */
	600,	600,	600,	600,	600,	250,	600,	600, /* 0xaf */
	250,	600,	250,	250,	600,	600,	250,	250, /* 0xb7 */
	250,	250,	250,	600,	600,	250,	600,	600, /* 0xbf */
	600,	600,	600,	250,	600,	250,	250,	600, /* 0xc7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xcf */
	600,	600,	600,	600,	600,	600,	600,	250, /* 0xd7 */
	600,	600,	600,	600,	250,	250,	600,	600, /* 0xdf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xe7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xef */
	250,	600,	600,	600,	600,	600,	600,	600, /* 0xf7 */
	600,	600,	600,	600,	600,	600,	600,	600  /* 0xff */
}
},

/* -------- Generated from metrics file Courier-BoldOblique.afm -------- */
{	"Courier-BoldOblique",			/* FontName */
	macroman,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	262241L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Courier Bold Oblique",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Courier",			/* Family name */
	"Bold",				/* Weight */
	cc_none,				/* Character collection */
	(float) -12.0,			/* ItalicAngle */
	1,				/* isFixedPitch */
	-57,				/* llx */
	-250,				/* lly */
	869,				/* urx */
	801,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	562,				/* CapHeight */
	439,				/* xHeight */
	629,				/* Ascender */
	-157,				/* Descender */
	106,				/* StdVW */
	84,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x27 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x2f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x37 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x3f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x47 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x4f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x57 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x5f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x67 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x6f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x77 */
	600,	600,	600,	600,	600,	600,	600,	250, /* 0x7f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x87 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x8f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x97 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x9f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xa7 */
	600,	600,	600,	600,	600,	250,	600,	600, /* 0xaf */
	250,	600,	250,	250,	600,	600,	250,	250, /* 0xb7 */
	250,	250,	250,	600,	600,	250,	600,	600, /* 0xbf */
	600,	600,	600,	250,	600,	250,	250,	600, /* 0xc7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xcf */
	600,	600,	600,	600,	600,	600,	600,	250, /* 0xd7 */
	600,	600,	600,	600,	250,	250,	600,	600, /* 0xdf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xe7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xef */
	250,	600,	600,	600,	600,	600,	600,	600, /* 0xf7 */
	600,	600,	600,	600,	600,	600,	600,	600  /* 0xff */
}
},

/* -------- Generated from metrics file Courier-Oblique.afm -------- */
{	"Courier-Oblique",			/* FontName */
	macroman,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	97L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Courier Oblique",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Courier",			/* Family name */
	"Medium",				/* Weight */
	cc_none,				/* Character collection */
	(float) -12.0,			/* ItalicAngle */
	1,				/* isFixedPitch */
	-27,				/* llx */
	-250,				/* lly */
	742,				/* urx */
	805,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	562,				/* CapHeight */
	426,				/* xHeight */
	629,				/* Ascender */
	-157,				/* Descender */
	51,				/* StdVW */
	51,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x27 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x2f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x37 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x3f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x47 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x4f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x57 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x5f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x67 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x6f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x77 */
	600,	600,	600,	600,	600,	600,	600,	250, /* 0x7f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x87 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x8f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x97 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x9f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xa7 */
	600,	600,	600,	600,	600,	250,	600,	600, /* 0xaf */
	250,	600,	250,	250,	600,	600,	250,	250, /* 0xb7 */
	250,	250,	250,	600,	600,	250,	600,	600, /* 0xbf */
	600,	600,	600,	250,	600,	250,	250,	600, /* 0xc7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xcf */
	600,	600,	600,	600,	600,	600,	600,	250, /* 0xd7 */
	600,	600,	600,	600,	250,	250,	600,	600, /* 0xdf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xe7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xef */
	250,	600,	600,	600,	600,	600,	600,	600, /* 0xf7 */
	600,	600,	600,	600,	600,	600,	600,	600  /* 0xff */
}
},

/* -------- Generated from metrics file Courier.afm -------- */
{	"Courier",			/* FontName */
	macroman,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	33L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Courier",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Courier",			/* Family name */
	"Medium",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	1,				/* isFixedPitch */
	-23,				/* llx */
	-250,				/* lly */
	623,				/* urx */
	805,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	562,				/* CapHeight */
	426,				/* xHeight */
	629,				/* Ascender */
	-157,				/* Descender */
	51,				/* StdVW */
	51,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x27 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x2f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x37 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x3f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x47 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x4f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x57 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x5f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x67 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x6f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x77 */
	600,	600,	600,	600,	600,	600,	600,	250, /* 0x7f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x87 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x8f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x97 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x9f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xa7 */
	600,	600,	600,	600,	600,	250,	600,	600, /* 0xaf */
	250,	600,	250,	250,	600,	600,	250,	250, /* 0xb7 */
	250,	250,	250,	600,	600,	250,	600,	600, /* 0xbf */
	600,	600,	600,	250,	600,	250,	250,	600, /* 0xc7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xcf */
	600,	600,	600,	600,	600,	600,	600,	250, /* 0xd7 */
	600,	600,	600,	600,	250,	250,	600,	600, /* 0xdf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xe7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xef */
	250,	600,	600,	600,	600,	600,	600,	600, /* 0xf7 */
	600,	600,	600,	600,	600,	600,	600,	600  /* 0xff */
}
},

/* -------- Generated from metrics file Helvetica-Bold.afm -------- */
{	"Helvetica-Bold",			/* FontName */
	macroman,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	262176L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Helvetica Bold",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Helvetica",			/* Family name */
	"Bold",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-170,				/* llx */
	-228,				/* lly */
	1003,				/* urx */
	962,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	718,				/* CapHeight */
	532,				/* xHeight */
	718,				/* Ascender */
	-207,				/* Descender */
	140,				/* StdVW */
	118,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	278,	333,	474,	556,	556,	889,	722,	238, /* 0x27 */
	333,	333,	389,	584,	278,	333,	278,	278, /* 0x2f */
	556,	556,	556,	556,	556,	556,	556,	556, /* 0x37 */
	556,	556,	333,	333,	584,	584,	584,	611, /* 0x3f */
	975,	722,	722,	722,	722,	667,	611,	778, /* 0x47 */
	722,	278,	556,	722,	611,	833,	722,	778, /* 0x4f */
	667,	778,	722,	667,	611,	722,	667,	944, /* 0x57 */
	667,	667,	611,	333,	278,	333,	584,	556, /* 0x5f */
	333,	556,	611,	556,	611,	556,	333,	611, /* 0x67 */
	611,	278,	278,	556,	278,	889,	611,	611, /* 0x6f */
	611,	611,	389,	556,	333,	611,	556,	778, /* 0x77 */
	556,	556,	500,	389,	280,	389,	584,	250, /* 0x7f */
	722,	722,	722,	667,	722,	778,	722,	556, /* 0x87 */
	556,	556,	556,	556,	556,	556,	556,	556, /* 0x8f */
	556,	556,	278,	278,	278,	278,	611,	611, /* 0x97 */
	611,	611,	611,	611,	611,	611,	611,	611, /* 0x9f */
	556,	400,	556,	556,	556,	350,	556,	611, /* 0xa7 */
	737,	737,	1000,	333,	333,	250,	1000,	778, /* 0xaf */
	250,	584,	250,	250,	556,	611,	250,	250, /* 0xb7 */
	250,	250,	250,	370,	365,	250,	889,	611, /* 0xbf */
	611,	333,	584,	250,	556,	250,	250,	556, /* 0xc7 */
	556,	1000,	278,	722,	722,	778,	1000,	944, /* 0xcf */
	556,	1000,	500,	500,	278,	278,	584,	250, /* 0xd7 */
	556,	667,	167,	556,	250,	250,	611,	611, /* 0xdf */
	556,	278,	278,	500,	1000,	722,	667,	722, /* 0xe7 */
	667,	667,	278,	278,	278,	278,	778,	778, /* 0xef */
	250,	778,	722,	722,	722,	278,	333,	333, /* 0xf7 */
	333,	333,	333,	333,	333,	333,	333,	333  /* 0xff */
}
},

/* -------- Generated from metrics file Helvetica-BoldOblique.afm -------- */
{	"Helvetica-BoldOblique",			/* FontName */
	macroman,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	262240L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Helvetica Bold Oblique",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Helvetica",			/* Family name */
	"Bold",				/* Weight */
	cc_none,				/* Character collection */
	(float) -12.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-174,				/* llx */
	-228,				/* lly */
	1114,				/* urx */
	962,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	718,				/* CapHeight */
	532,				/* xHeight */
	718,				/* Ascender */
	-207,				/* Descender */
	140,				/* StdVW */
	118,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	278,	333,	474,	556,	556,	889,	722,	238, /* 0x27 */
	333,	333,	389,	584,	278,	333,	278,	278, /* 0x2f */
	556,	556,	556,	556,	556,	556,	556,	556, /* 0x37 */
	556,	556,	333,	333,	584,	584,	584,	611, /* 0x3f */
	975,	722,	722,	722,	722,	667,	611,	778, /* 0x47 */
	722,	278,	556,	722,	611,	833,	722,	778, /* 0x4f */
	667,	778,	722,	667,	611,	722,	667,	944, /* 0x57 */
	667,	667,	611,	333,	278,	333,	584,	556, /* 0x5f */
	333,	556,	611,	556,	611,	556,	333,	611, /* 0x67 */
	611,	278,	278,	556,	278,	889,	611,	611, /* 0x6f */
	611,	611,	389,	556,	333,	611,	556,	778, /* 0x77 */
	556,	556,	500,	389,	280,	389,	584,	250, /* 0x7f */
	722,	722,	722,	667,	722,	778,	722,	556, /* 0x87 */
	556,	556,	556,	556,	556,	556,	556,	556, /* 0x8f */
	556,	556,	278,	278,	278,	278,	611,	611, /* 0x97 */
	611,	611,	611,	611,	611,	611,	611,	611, /* 0x9f */
	556,	400,	556,	556,	556,	350,	556,	611, /* 0xa7 */
	737,	737,	1000,	333,	333,	250,	1000,	778, /* 0xaf */
	250,	584,	250,	250,	556,	611,	250,	250, /* 0xb7 */
	250,	250,	250,	370,	365,	250,	889,	611, /* 0xbf */
	611,	333,	584,	250,	556,	250,	250,	556, /* 0xc7 */
	556,	1000,	278,	722,	722,	778,	1000,	944, /* 0xcf */
	556,	1000,	500,	500,	278,	278,	584,	250, /* 0xd7 */
	556,	667,	167,	556,	250,	250,	611,	611, /* 0xdf */
	556,	278,	278,	500,	1000,	722,	667,	722, /* 0xe7 */
	667,	667,	278,	278,	278,	278,	778,	778, /* 0xef */
	250,	778,	722,	722,	722,	278,	333,	333, /* 0xf7 */
	333,	333,	333,	333,	333,	333,	333,	333  /* 0xff */
}
},

/* -------- Generated from metrics file Helvetica-Oblique.afm -------- */
{	"Helvetica-Oblique",			/* FontName */
	macroman,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	96L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Helvetica Oblique",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Helvetica",			/* Family name */
	"Medium",				/* Weight */
	cc_none,				/* Character collection */
	(float) -12.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-170,				/* llx */
	-225,				/* lly */
	1116,				/* urx */
	931,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	718,				/* CapHeight */
	523,				/* xHeight */
	718,				/* Ascender */
	-207,				/* Descender */
	88,				/* StdVW */
	76,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	278,	278,	355,	556,	556,	889,	667,	191, /* 0x27 */
	333,	333,	389,	584,	278,	333,	278,	278, /* 0x2f */
	556,	556,	556,	556,	556,	556,	556,	556, /* 0x37 */
	556,	556,	278,	278,	584,	584,	584,	556, /* 0x3f */
	1015,	667,	667,	722,	722,	667,	611,	778, /* 0x47 */
	722,	278,	500,	667,	556,	833,	722,	778, /* 0x4f */
	667,	778,	722,	667,	611,	722,	667,	944, /* 0x57 */
	667,	667,	611,	278,	278,	278,	469,	556, /* 0x5f */
	333,	556,	556,	500,	556,	556,	278,	556, /* 0x67 */
	556,	222,	222,	500,	222,	833,	556,	556, /* 0x6f */
	556,	556,	333,	500,	278,	556,	500,	722, /* 0x77 */
	500,	500,	500,	334,	260,	334,	584,	250, /* 0x7f */
	667,	667,	722,	667,	722,	778,	722,	556, /* 0x87 */
	556,	556,	556,	556,	556,	500,	556,	556, /* 0x8f */
	556,	556,	278,	278,	278,	278,	556,	556, /* 0x97 */
	556,	556,	556,	556,	556,	556,	556,	556, /* 0x9f */
	556,	400,	556,	556,	556,	350,	537,	611, /* 0xa7 */
	737,	737,	1000,	333,	333,	250,	1000,	778, /* 0xaf */
	250,	584,	250,	250,	556,	556,	250,	250, /* 0xb7 */
	250,	250,	250,	370,	365,	250,	889,	611, /* 0xbf */
	611,	333,	584,	250,	556,	250,	250,	556, /* 0xc7 */
	556,	1000,	278,	667,	667,	778,	1000,	944, /* 0xcf */
	556,	1000,	333,	333,	222,	222,	584,	250, /* 0xd7 */
	500,	667,	167,	556,	250,	250,	500,	500, /* 0xdf */
	556,	278,	222,	333,	1000,	667,	667,	667, /* 0xe7 */
	667,	667,	278,	278,	278,	278,	778,	778, /* 0xef */
	250,	778,	722,	722,	722,	278,	333,	333, /* 0xf7 */
	333,	333,	333,	333,	333,	333,	333,	333  /* 0xff */
}
},

/* -------- Generated from metrics file Helvetica.afm -------- */
{	"Helvetica",			/* FontName */
	macroman,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	32L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Helvetica",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Helvetica",			/* Family name */
	"Medium",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-166,				/* llx */
	-225,				/* lly */
	1000,				/* urx */
	931,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	718,				/* CapHeight */
	523,				/* xHeight */
	718,				/* Ascender */
	-207,				/* Descender */
	88,				/* StdVW */
	76,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	278,	278,	355,	556,	556,	889,	667,	191, /* 0x27 */
	333,	333,	389,	584,	278,	333,	278,	278, /* 0x2f */
	556,	556,	556,	556,	556,	556,	556,	556, /* 0x37 */
	556,	556,	278,	278,	584,	584,	584,	556, /* 0x3f */
	1015,	667,	667,	722,	722,	667,	611,	778, /* 0x47 */
	722,	278,	500,	667,	556,	833,	722,	778, /* 0x4f */
	667,	778,	722,	667,	611,	722,	667,	944, /* 0x57 */
	667,	667,	611,	278,	278,	278,	469,	556, /* 0x5f */
	333,	556,	556,	500,	556,	556,	278,	556, /* 0x67 */
	556,	222,	222,	500,	222,	833,	556,	556, /* 0x6f */
	556,	556,	333,	500,	278,	556,	500,	722, /* 0x77 */
	500,	500,	500,	334,	260,	334,	584,	250, /* 0x7f */
	667,	667,	722,	667,	722,	778,	722,	556, /* 0x87 */
	556,	556,	556,	556,	556,	500,	556,	556, /* 0x8f */
	556,	556,	278,	278,	278,	278,	556,	556, /* 0x97 */
	556,	556,	556,	556,	556,	556,	556,	556, /* 0x9f */
	556,	400,	556,	556,	556,	350,	537,	611, /* 0xa7 */
	737,	737,	1000,	333,	333,	250,	1000,	778, /* 0xaf */
	250,	584,	250,	250,	556,	556,	250,	250, /* 0xb7 */
	250,	250,	250,	370,	365,	250,	889,	611, /* 0xbf */
	611,	333,	584,	250,	556,	250,	250,	556, /* 0xc7 */
	556,	1000,	278,	667,	667,	778,	1000,	944, /* 0xcf */
	556,	1000,	333,	333,	222,	222,	584,	250, /* 0xd7 */
	500,	667,	167,	556,	250,	250,	500,	500, /* 0xdf */
	556,	278,	222,	333,	1000,	667,	667,	667, /* 0xe7 */
	667,	667,	278,	278,	278,	278,	778,	778, /* 0xef */
	250,	778,	722,	722,	722,	278,	333,	333, /* 0xf7 */
	333,	333,	333,	333,	333,	333,	333,	333  /* 0xff */
}
},

/* -------- Generated from metrics file Symbol.afm -------- */
{	"Symbol",			/* FontName */
	builtin,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	4L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Symbol",			/* Full name */
	"FontSpecific",		/* Native encoding */
	"Symbol",			/* Family name */
	"Medium",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-180,				/* llx */
	-293,				/* lly */
	1090,				/* urx */
	1010,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	700,				/* CapHeight */
	0,				/* xHeight */
	800,				/* Ascender */
	-200,				/* Descender */
	85,				/* StdVW */
	92,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	333,	713,	500,	549,	833,	778,	439, /* 0x27 */
	333,	333,	500,	549,	250,	549,	250,	278, /* 0x2f */
	500,	500,	500,	500,	500,	500,	500,	500, /* 0x37 */
	500,	500,	278,	278,	549,	549,	549,	444, /* 0x3f */
	549,	722,	667,	722,	612,	611,	763,	603, /* 0x47 */
	722,	333,	631,	722,	686,	889,	722,	722, /* 0x4f */
	768,	741,	556,	592,	611,	690,	439,	768, /* 0x57 */
	645,	795,	611,	333,	863,	333,	658,	500, /* 0x5f */
	500,	631,	549,	549,	494,	439,	521,	411, /* 0x67 */
	603,	329,	603,	549,	549,	576,	521,	549, /* 0x6f */
	549,	521,	549,	603,	439,	576,	713,	686, /* 0x77 */
	493,	686,	494,	480,	200,	480,	549,	250, /* 0x7f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x87 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x8f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x97 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x9f */
	750,	620,	247,	549,	167,	713,	500,	753, /* 0xa7 */
	753,	753,	753,	1042,	987,	603,	987,	603, /* 0xaf */
	400,	549,	411,	549,	549,	713,	494,	460, /* 0xb7 */
	549,	549,	549,	549,	1000,	603,	1000,	658, /* 0xbf */
	823,	686,	795,	987,	768,	768,	823,	768, /* 0xc7 */
	768,	713,	713,	713,	713,	713,	713,	713, /* 0xcf */
	768,	713,	790,	790,	890,	823,	549,	250, /* 0xd7 */
	713,	603,	603,	1042,	987,	603,	987,	603, /* 0xdf */
	494,	329,	790,	790,	786,	713,	384,	384, /* 0xe7 */
	384,	384,	384,	384,	494,	494,	494,	494, /* 0xef */
	250,	329,	274,	686,	686,	686,	384,	384, /* 0xf7 */
	384,	384,	384,	384,	494,	494,	494,	250  /* 0xff */
}
},

/* -------- Generated from metrics file Times-Bold.afm -------- */
{	"Times-Bold",			/* FontName */
	macroman,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	262176L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Times Bold",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Times",			/* Family name */
	"Bold",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-168,				/* llx */
	-218,				/* lly */
	1000,				/* urx */
	935,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	676,				/* CapHeight */
	461,				/* xHeight */
	683,				/* Ascender */
	-217,				/* Descender */
	139,				/* StdVW */
	44,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	333,	555,	500,	500,	1000,	833,	278, /* 0x27 */
	333,	333,	500,	570,	250,	333,	250,	278, /* 0x2f */
	500,	500,	500,	500,	500,	500,	500,	500, /* 0x37 */
	500,	500,	333,	333,	570,	570,	570,	500, /* 0x3f */
	930,	722,	667,	722,	722,	667,	611,	778, /* 0x47 */
	778,	389,	500,	778,	667,	944,	722,	778, /* 0x4f */
	611,	778,	722,	556,	667,	722,	722,	1000, /* 0x57 */
	722,	722,	667,	333,	278,	333,	581,	500, /* 0x5f */
	333,	500,	556,	444,	556,	444,	333,	500, /* 0x67 */
	556,	278,	333,	556,	278,	833,	556,	500, /* 0x6f */
	556,	556,	444,	389,	333,	556,	500,	722, /* 0x77 */
	500,	500,	444,	394,	220,	394,	520,	250, /* 0x7f */
	722,	722,	722,	667,	722,	778,	722,	500, /* 0x87 */
	500,	500,	500,	500,	500,	444,	444,	444, /* 0x8f */
	444,	444,	278,	278,	278,	278,	556,	500, /* 0x97 */
	500,	500,	500,	500,	556,	556,	556,	556, /* 0x9f */
	500,	400,	500,	500,	500,	350,	540,	556, /* 0xa7 */
	747,	747,	1000,	333,	333,	250,	1000,	778, /* 0xaf */
	250,	570,	250,	250,	500,	556,	250,	250, /* 0xb7 */
	250,	250,	250,	300,	330,	250,	722,	500, /* 0xbf */
	500,	333,	570,	250,	500,	250,	250,	500, /* 0xc7 */
	500,	1000,	250,	722,	722,	778,	1000,	722, /* 0xcf */
	500,	1000,	500,	500,	333,	333,	570,	250, /* 0xd7 */
	500,	722,	167,	500,	250,	250,	556,	556, /* 0xdf */
	500,	250,	333,	500,	1000,	722,	667,	722, /* 0xe7 */
	667,	667,	389,	389,	389,	389,	778,	778, /* 0xef */
	250,	778,	722,	722,	722,	278,	333,	333, /* 0xf7 */
	333,	333,	333,	333,	333,	333,	333,	333  /* 0xff */
}
},

/* -------- Generated from metrics file Times-BoldItalic.afm -------- */
{	"Times-BoldItalic",			/* FontName */
	macroman,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	262240L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Times Bold Italic",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Times",			/* Family name */
	"Bold",				/* Weight */
	cc_none,				/* Character collection */
	(float) -15.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-200,				/* llx */
	-218,				/* lly */
	996,				/* urx */
	921,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	669,				/* CapHeight */
	462,				/* xHeight */
	683,				/* Ascender */
	-217,				/* Descender */
	121,				/* StdVW */
	42,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	389,	555,	500,	500,	833,	778,	278, /* 0x27 */
	333,	333,	500,	570,	250,	333,	250,	278, /* 0x2f */
	500,	500,	500,	500,	500,	500,	500,	500, /* 0x37 */
	500,	500,	333,	333,	570,	570,	570,	500, /* 0x3f */
	832,	667,	667,	667,	722,	667,	667,	722, /* 0x47 */
	778,	389,	500,	667,	611,	889,	722,	722, /* 0x4f */
	611,	722,	667,	556,	611,	722,	667,	889, /* 0x57 */
	667,	611,	611,	333,	278,	333,	570,	500, /* 0x5f */
	333,	500,	500,	444,	500,	444,	333,	500, /* 0x67 */
	556,	278,	278,	500,	278,	778,	556,	500, /* 0x6f */
	500,	500,	389,	389,	278,	556,	444,	667, /* 0x77 */
	500,	444,	389,	348,	220,	348,	570,	250, /* 0x7f */
	667,	667,	667,	667,	722,	722,	722,	500, /* 0x87 */
	500,	500,	500,	500,	500,	444,	444,	444, /* 0x8f */
	444,	444,	278,	278,	278,	278,	556,	500, /* 0x97 */
	500,	500,	500,	500,	556,	556,	556,	556, /* 0x9f */
	500,	400,	500,	500,	500,	350,	500,	500, /* 0xa7 */
	747,	747,	1000,	333,	333,	250,	944,	722, /* 0xaf */
	250,	570,	250,	250,	500,	576,	250,	250, /* 0xb7 */
	250,	250,	250,	266,	300,	250,	722,	500, /* 0xbf */
	500,	389,	606,	250,	500,	250,	250,	500, /* 0xc7 */
	500,	1000,	250,	667,	667,	722,	944,	722, /* 0xcf */
	500,	1000,	500,	500,	333,	333,	570,	250, /* 0xd7 */
	444,	611,	167,	500,	250,	250,	556,	556, /* 0xdf */
	500,	250,	333,	500,	1000,	667,	667,	667, /* 0xe7 */
	667,	667,	389,	389,	389,	389,	722,	722, /* 0xef */
	250,	722,	722,	722,	722,	278,	333,	333, /* 0xf7 */
	333,	333,	333,	333,	333,	333,	333,	333  /* 0xff */
}
},

/* -------- Generated from metrics file Times-Italic.afm -------- */
{	"Times-Italic",			/* FontName */
	macroman,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	96L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Times Italic",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Times",			/* Family name */
	"Medium",				/* Weight */
	cc_none,				/* Character collection */
	(float) -15.5,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-169,				/* llx */
	-217,				/* lly */
	1010,				/* urx */
	883,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	653,				/* CapHeight */
	441,				/* xHeight */
	683,				/* Ascender */
	-217,				/* Descender */
	76,				/* StdVW */
	32,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	333,	420,	500,	500,	833,	778,	214, /* 0x27 */
	333,	333,	500,	675,	250,	333,	250,	278, /* 0x2f */
	500,	500,	500,	500,	500,	500,	500,	500, /* 0x37 */
	500,	500,	333,	333,	675,	675,	675,	500, /* 0x3f */
	920,	611,	611,	667,	722,	611,	611,	722, /* 0x47 */
	722,	333,	444,	667,	556,	833,	667,	722, /* 0x4f */
	611,	722,	611,	500,	556,	722,	611,	833, /* 0x57 */
	611,	556,	556,	389,	278,	389,	422,	500, /* 0x5f */
	333,	500,	500,	444,	500,	444,	278,	500, /* 0x67 */
	500,	278,	278,	444,	278,	722,	500,	500, /* 0x6f */
	500,	500,	389,	389,	278,	500,	444,	667, /* 0x77 */
	444,	444,	389,	400,	275,	400,	541,	250, /* 0x7f */
	611,	611,	667,	611,	667,	722,	722,	500, /* 0x87 */
	500,	500,	500,	500,	500,	444,	444,	444, /* 0x8f */
	444,	444,	278,	278,	278,	278,	500,	500, /* 0x97 */
	500,	500,	500,	500,	500,	500,	500,	500, /* 0x9f */
	500,	400,	500,	500,	500,	350,	523,	500, /* 0xa7 */
	760,	760,	980,	333,	333,	250,	889,	722, /* 0xaf */
	250,	675,	250,	250,	500,	500,	250,	250, /* 0xb7 */
	250,	250,	250,	276,	310,	250,	667,	500, /* 0xbf */
	500,	389,	675,	250,	500,	250,	250,	500, /* 0xc7 */
	500,	889,	250,	611,	611,	722,	944,	667, /* 0xcf */
	500,	889,	556,	556,	333,	333,	675,	250, /* 0xd7 */
	444,	556,	167,	500,	250,	250,	500,	500, /* 0xdf */
	500,	250,	333,	556,	1000,	611,	611,	611, /* 0xe7 */
	611,	611,	333,	333,	333,	333,	722,	722, /* 0xef */
	250,	722,	722,	722,	722,	278,	333,	333, /* 0xf7 */
	333,	333,	333,	333,	333,	333,	333,	333  /* 0xff */
}
},

/* -------- Generated from metrics file Times-Roman.afm -------- */
{	"Times-Roman",			/* FontName */
	macroman,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	32L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Times Roman",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Times",			/* Family name */
	"Roman",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-168,				/* llx */
	-218,				/* lly */
	1000,				/* urx */
	898,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	662,				/* CapHeight */
	450,				/* xHeight */
	683,				/* Ascender */
	-217,				/* Descender */
	84,				/* StdVW */
	28,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	333,	408,	500,	500,	833,	778,	180, /* 0x27 */
	333,	333,	500,	564,	250,	333,	250,	278, /* 0x2f */
	500,	500,	500,	500,	500,	500,	500,	500, /* 0x37 */
	500,	500,	278,	278,	564,	564,	564,	444, /* 0x3f */
	921,	722,	667,	667,	722,	611,	556,	722, /* 0x47 */
	722,	333,	389,	722,	611,	889,	722,	722, /* 0x4f */
	556,	722,	667,	556,	611,	722,	722,	944, /* 0x57 */
	722,	722,	611,	333,	278,	333,	469,	500, /* 0x5f */
	333,	444,	500,	444,	500,	444,	333,	500, /* 0x67 */
	500,	278,	278,	500,	278,	778,	500,	500, /* 0x6f */
	500,	500,	333,	389,	278,	500,	500,	722, /* 0x77 */
	500,	500,	444,	480,	200,	480,	541,	250, /* 0x7f */
	722,	722,	667,	611,	722,	722,	722,	444, /* 0x87 */
	444,	444,	444,	444,	444,	444,	444,	444, /* 0x8f */
	444,	444,	278,	278,	278,	278,	500,	500, /* 0x97 */
	500,	500,	500,	500,	500,	500,	500,	500, /* 0x9f */
	500,	400,	500,	500,	500,	350,	453,	500, /* 0xa7 */
	760,	760,	980,	333,	333,	250,	889,	722, /* 0xaf */
	250,	564,	250,	250,	500,	500,	250,	250, /* 0xb7 */
	250,	250,	250,	276,	310,	250,	667,	500, /* 0xbf */
	444,	333,	564,	250,	500,	250,	250,	500, /* 0xc7 */
	500,	1000,	250,	722,	722,	722,	889,	722, /* 0xcf */
	500,	1000,	444,	444,	333,	333,	564,	250, /* 0xd7 */
	500,	722,	167,	500,	250,	250,	556,	556, /* 0xdf */
	500,	250,	333,	444,	1000,	722,	611,	722, /* 0xe7 */
	611,	611,	333,	333,	333,	333,	722,	722, /* 0xef */
	250,	722,	722,	722,	722,	278,	333,	333, /* 0xf7 */
	333,	333,	333,	333,	333,	333,	333,	333  /* 0xff */
}
},

/* -------- Generated from metrics file ZapfDingbats.afm -------- */
{	"ZapfDingbats",			/* FontName */
	builtin,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	4L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"ITC Zapf Dingbats",			/* Full name */
	"FontSpecific",		/* Native encoding */
	"ITC Zapf Dingbats",			/* Family name */
	"Medium",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-1,				/* llx */
	-143,				/* lly */
	981,				/* urx */
	820,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	700,				/* CapHeight */
	0,				/* xHeight */
	800,				/* Ascender */
	-200,				/* Descender */
	90,				/* StdVW */
	28,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	278,	974,	961,	974,	980,	719,	789,	790, /* 0x27 */
	791,	690,	960,	939,	549,	855,	911,	933, /* 0x2f */
	911,	945,	974,	755,	846,	762,	761,	571, /* 0x37 */
	677,	763,	760,	759,	754,	494,	552,	537, /* 0x3f */
	577,	692,	786,	788,	788,	790,	793,	794, /* 0x47 */
	816,	823,	789,	841,	823,	833,	816,	831, /* 0x4f */
	923,	744,	723,	749,	790,	792,	695,	776, /* 0x57 */
	768,	792,	759,	707,	708,	682,	701,	826, /* 0x5f */
	815,	789,	789,	707,	687,	696,	689,	786, /* 0x67 */
	787,	713,	791,	785,	791,	873,	761,	762, /* 0x6f */
	762,	759,	759,	892,	892,	788,	784,	438, /* 0x77 */
	138,	277,	415,	392,	392,	668,	668,	250, /* 0x7f */
	390,	390,	317,	317,	276,	276,	509,	509, /* 0x87 */
	410,	410,	234,	234,	334,	334,	250,	250, /* 0x8f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x97 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x9f */
	250,	732,	544,	544,	910,	667,	760,	760, /* 0xa7 */
	776,	595,	694,	626,	788,	788,	788,	788, /* 0xaf */
	788,	788,	788,	788,	788,	788,	788,	788, /* 0xb7 */
	788,	788,	788,	788,	788,	788,	788,	788, /* 0xbf */
	788,	788,	788,	788,	788,	788,	788,	788, /* 0xc7 */
	788,	788,	788,	788,	788,	788,	788,	788, /* 0xcf */
	788,	788,	788,	788,	894,	838,	1016,	458, /* 0xd7 */
	748,	924,	748,	918,	927,	928,	928,	834, /* 0xdf */
	873,	828,	924,	924,	917,	930,	931,	463, /* 0xe7 */
	883,	836,	836,	867,	867,	696,	696,	874, /* 0xef */
	250,	874,	760,	946,	771,	865,	771,	888, /* 0xf7 */
	967,	888,	831,	873,	927,	970,	918,	250  /* 0xff */
}
},

#else	/* all other platforms */


/*
 * Font metrics generated with compile_metrics utility
 *
 * Encoding used: winansi
 */

/* -------- Generated from metrics file Courier-Bold.afm -------- */
{	"Courier-Bold",			/* FontName */
	winansi,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	262177L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Courier Bold",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Courier",			/* Family name */
	"Bold",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	1,				/* isFixedPitch */
	-113,				/* llx */
	-250,				/* lly */
	749,				/* urx */
	801,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	562,				/* CapHeight */
	439,				/* xHeight */
	629,				/* Ascender */
	-157,				/* Descender */
	106,				/* StdVW */
	84,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x27 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x2f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x37 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x3f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x47 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x4f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x57 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x5f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x67 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x6f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x77 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x7f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x87 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x8f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x97 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x9f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xa7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xaf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xb7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xbf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xc7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xcf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xd7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xdf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xe7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xef */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xf7 */
	600,	600,	600,	600,	600,	600,	600,	600  /* 0xff */
}
},

/* -------- Generated from metrics file Courier-BoldOblique.afm -------- */
{	"Courier-BoldOblique",			/* FontName */
	winansi,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	262241L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Courier Bold Oblique",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Courier",			/* Family name */
	"Bold",				/* Weight */
	cc_none,				/* Character collection */
	(float) -12.0,			/* ItalicAngle */
	1,				/* isFixedPitch */
	-57,				/* llx */
	-250,				/* lly */
	869,				/* urx */
	801,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	562,				/* CapHeight */
	439,				/* xHeight */
	629,				/* Ascender */
	-157,				/* Descender */
	106,				/* StdVW */
	84,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x27 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x2f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x37 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x3f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x47 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x4f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x57 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x5f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x67 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x6f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x77 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x7f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x87 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x8f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x97 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x9f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xa7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xaf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xb7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xbf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xc7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xcf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xd7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xdf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xe7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xef */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xf7 */
	600,	600,	600,	600,	600,	600,	600,	600  /* 0xff */
}
},

/* -------- Generated from metrics file Courier-Oblique.afm -------- */
{	"Courier-Oblique",			/* FontName */
	winansi,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	97L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Courier Oblique",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Courier",			/* Family name */
	"Medium",				/* Weight */
	cc_none,				/* Character collection */
	(float) -12.0,			/* ItalicAngle */
	1,				/* isFixedPitch */
	-27,				/* llx */
	-250,				/* lly */
	742,				/* urx */
	805,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	562,				/* CapHeight */
	426,				/* xHeight */
	629,				/* Ascender */
	-157,				/* Descender */
	51,				/* StdVW */
	51,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x27 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x2f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x37 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x3f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x47 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x4f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x57 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x5f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x67 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x6f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x77 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x7f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x87 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x8f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x97 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x9f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xa7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xaf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xb7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xbf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xc7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xcf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xd7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xdf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xe7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xef */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xf7 */
	600,	600,	600,	600,	600,	600,	600,	600  /* 0xff */
}
},

/* -------- Generated from metrics file Courier.afm -------- */
{	"Courier",			/* FontName */
	winansi,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	33L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Courier",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Courier",			/* Family name */
	"Medium",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	1,				/* isFixedPitch */
	-23,				/* llx */
	-250,				/* lly */
	623,				/* urx */
	805,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	562,				/* CapHeight */
	426,				/* xHeight */
	629,				/* Ascender */
	-157,				/* Descender */
	51,				/* StdVW */
	51,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x27 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x2f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x37 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x3f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x47 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x4f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x57 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x5f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x67 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x6f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x77 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x7f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x87 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x8f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x97 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0x9f */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xa7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xaf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xb7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xbf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xc7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xcf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xd7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xdf */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xe7 */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xef */
	600,	600,	600,	600,	600,	600,	600,	600, /* 0xf7 */
	600,	600,	600,	600,	600,	600,	600,	600  /* 0xff */
}
},

/* -------- Generated from metrics file Helvetica-Bold.afm -------- */
{	"Helvetica-Bold",			/* FontName */
	winansi,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	262176L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Helvetica Bold",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Helvetica",			/* Family name */
	"Bold",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-170,				/* llx */
	-228,				/* lly */
	1003,				/* urx */
	962,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	718,				/* CapHeight */
	532,				/* xHeight */
	718,				/* Ascender */
	-207,				/* Descender */
	140,				/* StdVW */
	118,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	278,	333,	474,	556,	556,	889,	722,	238, /* 0x27 */
	333,	333,	389,	584,	278,	333,	278,	278, /* 0x2f */
	556,	556,	556,	556,	556,	556,	556,	556, /* 0x37 */
	556,	556,	333,	333,	584,	584,	584,	611, /* 0x3f */
	975,	722,	722,	722,	722,	667,	611,	778, /* 0x47 */
	722,	278,	556,	722,	611,	833,	722,	778, /* 0x4f */
	667,	778,	722,	667,	611,	722,	667,	944, /* 0x57 */
	667,	667,	611,	333,	278,	333,	584,	556, /* 0x5f */
	333,	556,	611,	556,	611,	556,	333,	611, /* 0x67 */
	611,	278,	278,	556,	278,	889,	611,	611, /* 0x6f */
	611,	611,	389,	556,	333,	611,	556,	778, /* 0x77 */
	556,	556,	500,	389,	280,	389,	584,	350, /* 0x7f */
	556,	350,	278,	556,	500,	1000,	556,	556, /* 0x87 */
	333,	1000,	667,	333,	1000,	350,	611,	350, /* 0x8f */
	350,	278,	278,	500,	500,	350,	556,	1000, /* 0x97 */
	333,	1000,	556,	333,	944,	350,	500,	667, /* 0x9f */
	278,	333,	556,	556,	556,	556,	280,	556, /* 0xa7 */
	333,	737,	370,	556,	584,	333,	737,	333, /* 0xaf */
	400,	584,	333,	333,	333,	611,	556,	278, /* 0xb7 */
	333,	333,	365,	556,	834,	834,	834,	611, /* 0xbf */
	722,	722,	722,	722,	722,	722,	1000,	722, /* 0xc7 */
	667,	667,	667,	667,	278,	278,	278,	278, /* 0xcf */
	722,	722,	778,	778,	778,	778,	778,	584, /* 0xd7 */
	778,	722,	722,	722,	722,	667,	667,	611, /* 0xdf */
	556,	556,	556,	556,	556,	556,	889,	556, /* 0xe7 */
	556,	556,	556,	556,	278,	278,	278,	278, /* 0xef */
	611,	611,	611,	611,	611,	611,	611,	584, /* 0xf7 */
	611,	611,	611,	611,	611,	556,	611,	556  /* 0xff */
}
},

/* -------- Generated from metrics file Helvetica-BoldOblique.afm -------- */
{	"Helvetica-BoldOblique",			/* FontName */
	winansi,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	262240L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Helvetica Bold Oblique",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Helvetica",			/* Family name */
	"Bold",				/* Weight */
	cc_none,				/* Character collection */
	(float) -12.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-174,				/* llx */
	-228,				/* lly */
	1114,				/* urx */
	962,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	718,				/* CapHeight */
	532,				/* xHeight */
	718,				/* Ascender */
	-207,				/* Descender */
	140,				/* StdVW */
	118,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	278,	333,	474,	556,	556,	889,	722,	238, /* 0x27 */
	333,	333,	389,	584,	278,	333,	278,	278, /* 0x2f */
	556,	556,	556,	556,	556,	556,	556,	556, /* 0x37 */
	556,	556,	333,	333,	584,	584,	584,	611, /* 0x3f */
	975,	722,	722,	722,	722,	667,	611,	778, /* 0x47 */
	722,	278,	556,	722,	611,	833,	722,	778, /* 0x4f */
	667,	778,	722,	667,	611,	722,	667,	944, /* 0x57 */
	667,	667,	611,	333,	278,	333,	584,	556, /* 0x5f */
	333,	556,	611,	556,	611,	556,	333,	611, /* 0x67 */
	611,	278,	278,	556,	278,	889,	611,	611, /* 0x6f */
	611,	611,	389,	556,	333,	611,	556,	778, /* 0x77 */
	556,	556,	500,	389,	280,	389,	584,	350, /* 0x7f */
	556,	350,	278,	556,	500,	1000,	556,	556, /* 0x87 */
	333,	1000,	667,	333,	1000,	350,	611,	350, /* 0x8f */
	350,	278,	278,	500,	500,	350,	556,	1000, /* 0x97 */
	333,	1000,	556,	333,	944,	350,	500,	667, /* 0x9f */
	278,	333,	556,	556,	556,	556,	280,	556, /* 0xa7 */
	333,	737,	370,	556,	584,	333,	737,	333, /* 0xaf */
	400,	584,	333,	333,	333,	611,	556,	278, /* 0xb7 */
	333,	333,	365,	556,	834,	834,	834,	611, /* 0xbf */
	722,	722,	722,	722,	722,	722,	1000,	722, /* 0xc7 */
	667,	667,	667,	667,	278,	278,	278,	278, /* 0xcf */
	722,	722,	778,	778,	778,	778,	778,	584, /* 0xd7 */
	778,	722,	722,	722,	722,	667,	667,	611, /* 0xdf */
	556,	556,	556,	556,	556,	556,	889,	556, /* 0xe7 */
	556,	556,	556,	556,	278,	278,	278,	278, /* 0xef */
	611,	611,	611,	611,	611,	611,	611,	584, /* 0xf7 */
	611,	611,	611,	611,	611,	556,	611,	556  /* 0xff */
}
},

/* -------- Generated from metrics file Helvetica-Oblique.afm -------- */
{	"Helvetica-Oblique",			/* FontName */
	winansi,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	96L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Helvetica Oblique",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Helvetica",			/* Family name */
	"Medium",				/* Weight */
	cc_none,				/* Character collection */
	(float) -12.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-170,				/* llx */
	-225,				/* lly */
	1116,				/* urx */
	931,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	718,				/* CapHeight */
	523,				/* xHeight */
	718,				/* Ascender */
	-207,				/* Descender */
	88,				/* StdVW */
	76,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	278,	278,	355,	556,	556,	889,	667,	191, /* 0x27 */
	333,	333,	389,	584,	278,	333,	278,	278, /* 0x2f */
	556,	556,	556,	556,	556,	556,	556,	556, /* 0x37 */
	556,	556,	278,	278,	584,	584,	584,	556, /* 0x3f */
	1015,	667,	667,	722,	722,	667,	611,	778, /* 0x47 */
	722,	278,	500,	667,	556,	833,	722,	778, /* 0x4f */
	667,	778,	722,	667,	611,	722,	667,	944, /* 0x57 */
	667,	667,	611,	278,	278,	278,	469,	556, /* 0x5f */
	333,	556,	556,	500,	556,	556,	278,	556, /* 0x67 */
	556,	222,	222,	500,	222,	833,	556,	556, /* 0x6f */
	556,	556,	333,	500,	278,	556,	500,	722, /* 0x77 */
	500,	500,	500,	334,	260,	334,	584,	350, /* 0x7f */
	556,	350,	222,	556,	333,	1000,	556,	556, /* 0x87 */
	333,	1000,	667,	333,	1000,	350,	611,	350, /* 0x8f */
	350,	222,	222,	333,	333,	350,	556,	1000, /* 0x97 */
	333,	1000,	500,	333,	944,	350,	500,	667, /* 0x9f */
	278,	333,	556,	556,	556,	556,	260,	556, /* 0xa7 */
	333,	737,	370,	556,	584,	333,	737,	333, /* 0xaf */
	400,	584,	333,	333,	333,	556,	537,	278, /* 0xb7 */
	333,	333,	365,	556,	834,	834,	834,	611, /* 0xbf */
	667,	667,	667,	667,	667,	667,	1000,	722, /* 0xc7 */
	667,	667,	667,	667,	278,	278,	278,	278, /* 0xcf */
	722,	722,	778,	778,	778,	778,	778,	584, /* 0xd7 */
	778,	722,	722,	722,	722,	667,	667,	611, /* 0xdf */
	556,	556,	556,	556,	556,	556,	889,	500, /* 0xe7 */
	556,	556,	556,	556,	278,	278,	278,	278, /* 0xef */
	556,	556,	556,	556,	556,	556,	556,	584, /* 0xf7 */
	611,	556,	556,	556,	556,	500,	556,	500  /* 0xff */
}
},

/* -------- Generated from metrics file Helvetica.afm -------- */
{	"Helvetica",			/* FontName */
	winansi,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	32L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Helvetica",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Helvetica",			/* Family name */
	"Medium",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-166,				/* llx */
	-225,				/* lly */
	1000,				/* urx */
	931,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	718,				/* CapHeight */
	523,				/* xHeight */
	718,				/* Ascender */
	-207,				/* Descender */
	88,				/* StdVW */
	76,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	278,	278,	355,	556,	556,	889,	667,	191, /* 0x27 */
	333,	333,	389,	584,	278,	333,	278,	278, /* 0x2f */
	556,	556,	556,	556,	556,	556,	556,	556, /* 0x37 */
	556,	556,	278,	278,	584,	584,	584,	556, /* 0x3f */
	1015,	667,	667,	722,	722,	667,	611,	778, /* 0x47 */
	722,	278,	500,	667,	556,	833,	722,	778, /* 0x4f */
	667,	778,	722,	667,	611,	722,	667,	944, /* 0x57 */
	667,	667,	611,	278,	278,	278,	469,	556, /* 0x5f */
	333,	556,	556,	500,	556,	556,	278,	556, /* 0x67 */
	556,	222,	222,	500,	222,	833,	556,	556, /* 0x6f */
	556,	556,	333,	500,	278,	556,	500,	722, /* 0x77 */
	500,	500,	500,	334,	260,	334,	584,	350, /* 0x7f */
	556,	350,	222,	556,	333,	1000,	556,	556, /* 0x87 */
	333,	1000,	667,	333,	1000,	350,	611,	350, /* 0x8f */
	350,	222,	222,	333,	333,	350,	556,	1000, /* 0x97 */
	333,	1000,	500,	333,	944,	350,	500,	667, /* 0x9f */
	278,	333,	556,	556,	556,	556,	260,	556, /* 0xa7 */
	333,	737,	370,	556,	584,	333,	737,	333, /* 0xaf */
	400,	584,	333,	333,	333,	556,	537,	278, /* 0xb7 */
	333,	333,	365,	556,	834,	834,	834,	611, /* 0xbf */
	667,	667,	667,	667,	667,	667,	1000,	722, /* 0xc7 */
	667,	667,	667,	667,	278,	278,	278,	278, /* 0xcf */
	722,	722,	778,	778,	778,	778,	778,	584, /* 0xd7 */
	778,	722,	722,	722,	722,	667,	667,	611, /* 0xdf */
	556,	556,	556,	556,	556,	556,	889,	500, /* 0xe7 */
	556,	556,	556,	556,	278,	278,	278,	278, /* 0xef */
	556,	556,	556,	556,	556,	556,	556,	584, /* 0xf7 */
	611,	556,	556,	556,	556,	500,	556,	500  /* 0xff */
}
},

/* -------- Generated from metrics file Symbol.afm -------- */
{	"Symbol",			/* FontName */
	builtin,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	4L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Symbol",			/* Full name */
	"FontSpecific",		/* Native encoding */
	"Symbol",			/* Family name */
	"Medium",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-180,				/* llx */
	-293,				/* lly */
	1090,				/* urx */
	1010,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	700,				/* CapHeight */
	0,				/* xHeight */
	800,				/* Ascender */
	-200,				/* Descender */
	85,				/* StdVW */
	92,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	333,	713,	500,	549,	833,	778,	439, /* 0x27 */
	333,	333,	500,	549,	250,	549,	250,	278, /* 0x2f */
	500,	500,	500,	500,	500,	500,	500,	500, /* 0x37 */
	500,	500,	278,	278,	549,	549,	549,	444, /* 0x3f */
	549,	722,	667,	722,	612,	611,	763,	603, /* 0x47 */
	722,	333,	631,	722,	686,	889,	722,	722, /* 0x4f */
	768,	741,	556,	592,	611,	690,	439,	768, /* 0x57 */
	645,	795,	611,	333,	863,	333,	658,	500, /* 0x5f */
	500,	631,	549,	549,	494,	439,	521,	411, /* 0x67 */
	603,	329,	603,	549,	549,	576,	521,	549, /* 0x6f */
	549,	521,	549,	603,	439,	576,	713,	686, /* 0x77 */
	493,	686,	494,	480,	200,	480,	549,	250, /* 0x7f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x87 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x8f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x97 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x9f */
	750,	620,	247,	549,	167,	713,	500,	753, /* 0xa7 */
	753,	753,	753,	1042,	987,	603,	987,	603, /* 0xaf */
	400,	549,	411,	549,	549,	713,	494,	460, /* 0xb7 */
	549,	549,	549,	549,	1000,	603,	1000,	658, /* 0xbf */
	823,	686,	795,	987,	768,	768,	823,	768, /* 0xc7 */
	768,	713,	713,	713,	713,	713,	713,	713, /* 0xcf */
	768,	713,	790,	790,	890,	823,	549,	250, /* 0xd7 */
	713,	603,	603,	1042,	987,	603,	987,	603, /* 0xdf */
	494,	329,	790,	790,	786,	713,	384,	384, /* 0xe7 */
	384,	384,	384,	384,	494,	494,	494,	494, /* 0xef */
	250,	329,	274,	686,	686,	686,	384,	384, /* 0xf7 */
	384,	384,	384,	384,	494,	494,	494,	250  /* 0xff */
}
},

/* -------- Generated from metrics file Times-Bold.afm -------- */
{	"Times-Bold",			/* FontName */
	winansi,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	262176L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Times Bold",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Times",			/* Family name */
	"Bold",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-168,				/* llx */
	-218,				/* lly */
	1000,				/* urx */
	935,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	676,				/* CapHeight */
	461,				/* xHeight */
	683,				/* Ascender */
	-217,				/* Descender */
	139,				/* StdVW */
	44,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	333,	555,	500,	500,	1000,	833,	278, /* 0x27 */
	333,	333,	500,	570,	250,	333,	250,	278, /* 0x2f */
	500,	500,	500,	500,	500,	500,	500,	500, /* 0x37 */
	500,	500,	333,	333,	570,	570,	570,	500, /* 0x3f */
	930,	722,	667,	722,	722,	667,	611,	778, /* 0x47 */
	778,	389,	500,	778,	667,	944,	722,	778, /* 0x4f */
	611,	778,	722,	556,	667,	722,	722,	1000, /* 0x57 */
	722,	722,	667,	333,	278,	333,	581,	500, /* 0x5f */
	333,	500,	556,	444,	556,	444,	333,	500, /* 0x67 */
	556,	278,	333,	556,	278,	833,	556,	500, /* 0x6f */
	556,	556,	444,	389,	333,	556,	500,	722, /* 0x77 */
	500,	500,	444,	394,	220,	394,	520,	350, /* 0x7f */
	500,	350,	333,	500,	500,	1000,	500,	500, /* 0x87 */
	333,	1000,	556,	333,	1000,	350,	667,	350, /* 0x8f */
	350,	333,	333,	500,	500,	350,	500,	1000, /* 0x97 */
	333,	1000,	389,	333,	722,	350,	444,	722, /* 0x9f */
	250,	333,	500,	500,	500,	500,	220,	500, /* 0xa7 */
	333,	747,	300,	500,	570,	333,	747,	333, /* 0xaf */
	400,	570,	300,	300,	333,	556,	540,	250, /* 0xb7 */
	333,	300,	330,	500,	750,	750,	750,	500, /* 0xbf */
	722,	722,	722,	722,	722,	722,	1000,	722, /* 0xc7 */
	667,	667,	667,	667,	389,	389,	389,	389, /* 0xcf */
	722,	722,	778,	778,	778,	778,	778,	570, /* 0xd7 */
	778,	722,	722,	722,	722,	722,	611,	556, /* 0xdf */
	500,	500,	500,	500,	500,	500,	722,	444, /* 0xe7 */
	444,	444,	444,	444,	278,	278,	278,	278, /* 0xef */
	500,	556,	500,	500,	500,	500,	500,	570, /* 0xf7 */
	500,	556,	556,	556,	556,	500,	556,	500  /* 0xff */
}
},

/* -------- Generated from metrics file Times-BoldItalic.afm -------- */
{	"Times-BoldItalic",			/* FontName */
	winansi,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	262240L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Times Bold Italic",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Times",			/* Family name */
	"Bold",				/* Weight */
	cc_none,				/* Character collection */
	(float) -15.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-200,				/* llx */
	-218,				/* lly */
	996,				/* urx */
	921,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	669,				/* CapHeight */
	462,				/* xHeight */
	683,				/* Ascender */
	-217,				/* Descender */
	121,				/* StdVW */
	42,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	389,	555,	500,	500,	833,	778,	278, /* 0x27 */
	333,	333,	500,	570,	250,	333,	250,	278, /* 0x2f */
	500,	500,	500,	500,	500,	500,	500,	500, /* 0x37 */
	500,	500,	333,	333,	570,	570,	570,	500, /* 0x3f */
	832,	667,	667,	667,	722,	667,	667,	722, /* 0x47 */
	778,	389,	500,	667,	611,	889,	722,	722, /* 0x4f */
	611,	722,	667,	556,	611,	722,	667,	889, /* 0x57 */
	667,	611,	611,	333,	278,	333,	570,	500, /* 0x5f */
	333,	500,	500,	444,	500,	444,	333,	500, /* 0x67 */
	556,	278,	278,	500,	278,	778,	556,	500, /* 0x6f */
	500,	500,	389,	389,	278,	556,	444,	667, /* 0x77 */
	500,	444,	389,	348,	220,	348,	570,	350, /* 0x7f */
	500,	350,	333,	500,	500,	1000,	500,	500, /* 0x87 */
	333,	1000,	556,	333,	944,	350,	611,	350, /* 0x8f */
	350,	333,	333,	500,	500,	350,	500,	1000, /* 0x97 */
	333,	1000,	389,	333,	722,	350,	389,	611, /* 0x9f */
	250,	389,	500,	500,	500,	500,	220,	500, /* 0xa7 */
	333,	747,	266,	500,	606,	333,	747,	333, /* 0xaf */
	400,	570,	300,	300,	333,	576,	500,	250, /* 0xb7 */
	333,	300,	300,	500,	750,	750,	750,	500, /* 0xbf */
	667,	667,	667,	667,	667,	667,	944,	667, /* 0xc7 */
	667,	667,	667,	667,	389,	389,	389,	389, /* 0xcf */
	722,	722,	722,	722,	722,	722,	722,	570, /* 0xd7 */
	722,	722,	722,	722,	722,	611,	611,	500, /* 0xdf */
	500,	500,	500,	500,	500,	500,	722,	444, /* 0xe7 */
	444,	444,	444,	444,	278,	278,	278,	278, /* 0xef */
	500,	556,	500,	500,	500,	500,	500,	570, /* 0xf7 */
	500,	556,	556,	556,	556,	444,	500,	444  /* 0xff */
}
},

/* -------- Generated from metrics file Times-Italic.afm -------- */
{	"Times-Italic",			/* FontName */
	winansi,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	96L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Times Italic",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Times",			/* Family name */
	"Medium",				/* Weight */
	cc_none,				/* Character collection */
	(float) -15.5,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-169,				/* llx */
	-217,				/* lly */
	1010,				/* urx */
	883,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	653,				/* CapHeight */
	441,				/* xHeight */
	683,				/* Ascender */
	-217,				/* Descender */
	76,				/* StdVW */
	32,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	333,	420,	500,	500,	833,	778,	214, /* 0x27 */
	333,	333,	500,	675,	250,	333,	250,	278, /* 0x2f */
	500,	500,	500,	500,	500,	500,	500,	500, /* 0x37 */
	500,	500,	333,	333,	675,	675,	675,	500, /* 0x3f */
	920,	611,	611,	667,	722,	611,	611,	722, /* 0x47 */
	722,	333,	444,	667,	556,	833,	667,	722, /* 0x4f */
	611,	722,	611,	500,	556,	722,	611,	833, /* 0x57 */
	611,	556,	556,	389,	278,	389,	422,	500, /* 0x5f */
	333,	500,	500,	444,	500,	444,	278,	500, /* 0x67 */
	500,	278,	278,	444,	278,	722,	500,	500, /* 0x6f */
	500,	500,	389,	389,	278,	500,	444,	667, /* 0x77 */
	444,	444,	389,	400,	275,	400,	541,	350, /* 0x7f */
	500,	350,	333,	500,	556,	889,	500,	500, /* 0x87 */
	333,	1000,	500,	333,	944,	350,	556,	350, /* 0x8f */
	350,	333,	333,	556,	556,	350,	500,	889, /* 0x97 */
	333,	980,	389,	333,	667,	350,	389,	556, /* 0x9f */
	250,	389,	500,	500,	500,	500,	275,	500, /* 0xa7 */
	333,	760,	276,	500,	675,	333,	760,	333, /* 0xaf */
	400,	675,	300,	300,	333,	500,	523,	250, /* 0xb7 */
	333,	300,	310,	500,	750,	750,	750,	500, /* 0xbf */
	611,	611,	611,	611,	611,	611,	889,	667, /* 0xc7 */
	611,	611,	611,	611,	333,	333,	333,	333, /* 0xcf */
	722,	667,	722,	722,	722,	722,	722,	675, /* 0xd7 */
	722,	722,	722,	722,	722,	556,	611,	500, /* 0xdf */
	500,	500,	500,	500,	500,	500,	667,	444, /* 0xe7 */
	444,	444,	444,	444,	278,	278,	278,	278, /* 0xef */
	500,	500,	500,	500,	500,	500,	500,	675, /* 0xf7 */
	500,	500,	500,	500,	500,	444,	500,	444  /* 0xff */
}
},

/* -------- Generated from metrics file Times-Roman.afm -------- */
{	"Times-Roman",			/* FontName */
	winansi,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	32L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"Times Roman",			/* Full name */
	"AdobeStandardEncoding",		/* Native encoding */
	"Times",			/* Family name */
	"Roman",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-168,				/* llx */
	-218,				/* lly */
	1000,				/* urx */
	898,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	662,				/* CapHeight */
	450,				/* xHeight */
	683,				/* Ascender */
	-217,				/* Descender */
	84,				/* StdVW */
	28,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	250,	333,	408,	500,	500,	833,	778,	180, /* 0x27 */
	333,	333,	500,	564,	250,	333,	250,	278, /* 0x2f */
	500,	500,	500,	500,	500,	500,	500,	500, /* 0x37 */
	500,	500,	278,	278,	564,	564,	564,	444, /* 0x3f */
	921,	722,	667,	667,	722,	611,	556,	722, /* 0x47 */
	722,	333,	389,	722,	611,	889,	722,	722, /* 0x4f */
	556,	722,	667,	556,	611,	722,	722,	944, /* 0x57 */
	722,	722,	611,	333,	278,	333,	469,	500, /* 0x5f */
	333,	444,	500,	444,	500,	444,	333,	500, /* 0x67 */
	500,	278,	278,	500,	278,	778,	500,	500, /* 0x6f */
	500,	500,	333,	389,	278,	500,	500,	722, /* 0x77 */
	500,	500,	444,	480,	200,	480,	541,	350, /* 0x7f */
	500,	350,	333,	500,	444,	1000,	500,	500, /* 0x87 */
	333,	1000,	556,	333,	889,	350,	611,	350, /* 0x8f */
	350,	333,	333,	444,	444,	350,	500,	1000, /* 0x97 */
	333,	980,	389,	333,	722,	350,	444,	722, /* 0x9f */
	250,	333,	500,	500,	500,	500,	200,	500, /* 0xa7 */
	333,	760,	276,	500,	564,	333,	760,	333, /* 0xaf */
	400,	564,	300,	300,	333,	500,	453,	250, /* 0xb7 */
	333,	300,	310,	500,	750,	750,	750,	444, /* 0xbf */
	722,	722,	722,	722,	722,	722,	889,	667, /* 0xc7 */
	611,	611,	611,	611,	333,	333,	333,	333, /* 0xcf */
	722,	722,	722,	722,	722,	722,	722,	564, /* 0xd7 */
	722,	722,	722,	722,	722,	722,	556,	500, /* 0xdf */
	444,	444,	444,	444,	444,	444,	667,	444, /* 0xe7 */
	444,	444,	444,	444,	278,	278,	278,	278, /* 0xef */
	500,	500,	500,	500,	500,	500,	500,	564, /* 0xf7 */
	500,	500,	500,	500,	500,	500,	500,	500  /* 0xff */
}
},

/* -------- Generated from metrics file ZapfDingbats.afm -------- */
{	"ZapfDingbats",			/* FontName */
	builtin,			/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	4L,			/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"ITC Zapf Dingbats",			/* Full name */
	"FontSpecific",		/* Native encoding */
	"ITC Zapf Dingbats",			/* Family name */
	"Medium",				/* Weight */
	cc_none,				/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	0,				/* isFixedPitch */
	-1,				/* llx */
	-143,				/* lly */
	981,				/* urx */
	820,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	700,				/* CapHeight */
	0,				/* xHeight */
	800,				/* Ascender */
	-200,				/* Descender */
	90,				/* StdVW */
	28,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */

/* Character metrics */
{	250,	250,	250,	250,	250,	250,	250,	250, /* 0x07 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x0f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x17 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x1f */
	278,	974,	961,	974,	980,	719,	789,	790, /* 0x27 */
	791,	690,	960,	939,	549,	855,	911,	933, /* 0x2f */
	911,	945,	974,	755,	846,	762,	761,	571, /* 0x37 */
	677,	763,	760,	759,	754,	494,	552,	537, /* 0x3f */
	577,	692,	786,	788,	788,	790,	793,	794, /* 0x47 */
	816,	823,	789,	841,	823,	833,	816,	831, /* 0x4f */
	923,	744,	723,	749,	790,	792,	695,	776, /* 0x57 */
	768,	792,	759,	707,	708,	682,	701,	826, /* 0x5f */
	815,	789,	789,	707,	687,	696,	689,	786, /* 0x67 */
	787,	713,	791,	785,	791,	873,	761,	762, /* 0x6f */
	762,	759,	759,	892,	892,	788,	784,	438, /* 0x77 */
	138,	277,	415,	392,	392,	668,	668,	250, /* 0x7f */
	390,	390,	317,	317,	276,	276,	509,	509, /* 0x87 */
	410,	410,	234,	234,	334,	334,	250,	250, /* 0x8f */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x97 */
	250,	250,	250,	250,	250,	250,	250,	250, /* 0x9f */
	250,	732,	544,	544,	910,	667,	760,	760, /* 0xa7 */
	776,	595,	694,	626,	788,	788,	788,	788, /* 0xaf */
	788,	788,	788,	788,	788,	788,	788,	788, /* 0xb7 */
	788,	788,	788,	788,	788,	788,	788,	788, /* 0xbf */
	788,	788,	788,	788,	788,	788,	788,	788, /* 0xc7 */
	788,	788,	788,	788,	788,	788,	788,	788, /* 0xcf */
	788,	788,	788,	788,	894,	838,	1016,	458, /* 0xd7 */
	748,	924,	748,	918,	927,	928,	928,	834, /* 0xdf */
	873,	828,	924,	924,	917,	930,	931,	463, /* 0xe7 */
	883,	836,	836,	867,	867,	696,	696,	874, /* 0xef */
	250,	874,	760,	946,	771,	865,	771,	888, /* 0xf7 */
	967,	888,	831,	873,	927,	970,	918,	250  /* 0xff */
}
},

#endif /* !PDFLIB_EBCDIC && !MAC, i.e. WIN and others */

#endif /* P_METRICS_H */
