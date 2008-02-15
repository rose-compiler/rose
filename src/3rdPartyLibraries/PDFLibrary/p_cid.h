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

/* p_cid.h
 *
 * Header file for PDFlib CID font information
 *
 */

#ifndef P_CID_H
#define P_CID_H

typedef struct { const char *name; pdf_charcoll charcoll; } pdf_cmap;
typedef struct { const char *ordering; int supplement; } pdf_charcoll_names;

pdf_charcoll_names charcoll_names[] = {
    { "GB1",		0 },	/* cc_simplified_chinese */
    { "CNS1",		1 },	/* cc_traditional_chinese */
    { "Japan1",		2 },	/* cc_japanese */
    { "Korea1",		1 },	/* cc_korean */
    { "Identity",	0 }	/* cc_identity (not used) */
};

/* predefined CMaps and the corresponding character collection */

pdf_cmap cmaps[] = {
    { "GB-EUC-H",	cc_simplified_chinese },
    { "GB-EUC-V",	cc_simplified_chinese },
    { "GBpc-EUC-H",	cc_simplified_chinese },
    { "GBpc-EUC-V",	cc_simplified_chinese },
    { "GBK-EUC-H",	cc_simplified_chinese },
    { "GBK-EUC-V",	cc_simplified_chinese },
    { "UniGB-UCS2-H",	cc_simplified_chinese },
    { "UniGB-UCS2-V",	cc_simplified_chinese },

    { "B5pc-H",		cc_traditional_chinese },
    { "B5pc-V",		cc_traditional_chinese },
    { "ETen-B5-H",	cc_traditional_chinese },
    { "ETen-B5-V",	cc_traditional_chinese },
    { "ETenms-B5-H",	cc_traditional_chinese },
    { "ETenms-B5-V",	cc_traditional_chinese },
    { "CNS-EUC-H",	cc_traditional_chinese },
    { "CNS-EUC-V",	cc_traditional_chinese },
    { "UniCNS-UCS2-H",	cc_traditional_chinese },
    { "UniCNS-UCS2-V",	cc_traditional_chinese },

    { "83pv-RKSJ-H",	cc_japanese },
    { "90ms-RKSJ-H",	cc_japanese },
    { "90ms-RKSJ-V",	cc_japanese },
    { "90msp-RKSJ-H",	cc_japanese },
    { "90msp-RKSJ-V",	cc_japanese },
    { "90pv-RKSJ-H",	cc_japanese },
    { "Add-RKSJ-H",	cc_japanese },
    { "Add-RKSJ-V",	cc_japanese },
    { "EUC-H",		cc_japanese },
    { "EUC-V",		cc_japanese },
    { "Ext-RKSJ-H",	cc_japanese },
    { "Ext-RKSJ-V",	cc_japanese },
    { "H",		cc_japanese },
    { "V",		cc_japanese },
    { "UniJIS-UCS2-H",	cc_japanese },
    { "UniJIS-UCS2-V",	cc_japanese },
    { "UniJIS-UCS2-HW-H",	cc_japanese },
    { "UniJIS-UCS2-HW-V",	cc_japanese },

    { "KSC-EUC-H",	cc_korean },
    { "KSC-EUC-V",	cc_korean },
    { "KSCms-UHC-H",	cc_korean },
    { "KSCms-UHC-V",	cc_korean },
    { "KSCms-UHC-HW-H",	cc_korean },
    { "KSCms-UHC-HW-V",	cc_korean },
    { "KSCpc-EUC-H",	cc_korean },
    { "UniKS-UCS2-H",	cc_korean },
    { "UniKS-UCS2-V",	cc_korean },

    { "Identity-H",	cc_identity },
    { "Identity-V",	cc_identity }
};

/* Font descriptors for the preinstalled CID fonts */

const pdf_font pdf_cid_metrics[] = {
/* ---------------------------------------------------------- */
{	"HeiseiKakuGo-W5",		/* FontName */
	cid,				/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	4L,				/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"HeiseiKakuGo-W5",		/* Full name */
	(char *) NULL,			/* Native encoding */
	"HeiseiKakuGo-W5",		/* Family name */
	(char *) NULL,			/* Weight */
	cc_japanese,			/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	pdf_true,			/* isFixedPitch */
	-92,				/* llx */
	-250,				/* lly */
	1010,				/* urx */
	922,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	737,				/* CapHeight */
	553,				/* xHeight */
	752,				/* Ascender */
	-221,				/* Descender */
	114,				/* StdVW */
	0,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */
					/* skip character metrics array */
},

/* ---------------------------------------------------------- */
{	"HeiseiMin-W3",			/* FontName */
	cid,				/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	6L,				/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"HeiseiMin-W3",			/* Full name */
	(char *) NULL,			/* Native encoding */
	"HeiseiMin-W3",			/* Family name */
	(char *) NULL,			/* Weight */
	cc_japanese,			/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	pdf_true,			/* isFixedPitch */
	-123,				/* llx */
	-257,				/* lly */
	1001,				/* urx */
	910,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	709,				/* CapHeight */
	450,				/* xHeight */
	723,				/* Ascender */
	-241,				/* Descender */
	69,				/* StdVW */
	0,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */
					/* skip character metrics array */
},

/* ---------------------------------------------------------- */
{	"HYGoThic-Medium",		/* FontName */
	cid,				/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	6L,				/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"HYGoThic-Medium",		/* Full name */
	(char *) NULL,			/* Native encoding */
	"HYGoThic-Medium",		/* Family name */
	(char *) NULL,			/* Weight */
	cc_korean,			/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	pdf_true,			/* isFixedPitch */
	-6,				/* llx */
	-145,				/* lly */
	1003,				/* urx */
	880,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	737,				/* CapHeight */
	553,				/* xHeight */
	752,				/* Ascender */
	-271,				/* Descender */
	58,				/* StdVW */
	0,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */
					/* skip character metrics array */
},

/* ---------------------------------------------------------- */
{	"HYSMyeongJo-Medium",		/* FontName */
	cid,				/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	6L,				/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"HYSMyeongJo-Medium",		/* Full name */
	(char *) NULL,			/* Native encoding */
	"HYSMyeongJo-Medium",		/* Family name */
	(char *) NULL,			/* Weight */
	cc_korean,			/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	pdf_true,			/* isFixedPitch */
	-0,				/* llx */
	-148,				/* lly */
	1001,				/* urx */
	880,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	737,				/* CapHeight */
	553,				/* xHeight */
	752,				/* Ascender */
	-271,				/* Descender */
	58,				/* StdVW */
	0,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */
					/* skip character metrics array */
},

/* ---------------------------------------------------------- */
{	"MHei-Medium",		/* FontName */
	cid,				/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	6L,				/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"MHei-Medium",			/* Full name */
	(char *) NULL,			/* Native encoding */
	"MHei-Medium",			/* Family name */
	(char *) NULL,			/* Weight */
	cc_traditional_chinese,		/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	pdf_true,			/* isFixedPitch */
	-45,				/* llx */
	-250,				/* lly */
	1015,				/* urx */
	887,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	737,				/* CapHeight */
	553,				/* xHeight */
	752,				/* Ascender */
	-271,				/* Descender */
	58,				/* StdVW */
	0,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */
					/* skip character metrics array */
},

/* ---------------------------------------------------------- */
{	"MSung-Light",			/* FontName */
	cid,				/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	6L,				/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"MSung-Light",			/* Full name */
	(char *) NULL,			/* Native encoding */
	"MSung-Light",			/* Family name */
	(char *) NULL,			/* Weight */
	cc_traditional_chinese,		/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	pdf_true,			/* isFixedPitch */
	-160,				/* llx */
	-259,				/* lly */
	1015,				/* urx */
	888,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	737,				/* CapHeight */
	553,				/* xHeight */
	752,				/* Ascender */
	-271,				/* Descender */
	58,				/* StdVW */
	0,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */
					/* skip character metrics array */
},

/* ---------------------------------------------------------- */
{	"STSong-Light",			/* FontName */
	cid,				/* Encoding */
	0,				/* used on current page */
	0,				/* embed flag */
	(char *) NULL,			/* Name of external font file */
	0L,				/* Object id */
	4L,				/* Font flags */
	pdf_false,			/* parsed from an AFM file */
	pdf_false,			/* Multiple Master font */
	"STSong-Light",			/* Full name */
	(char *) NULL,			/* Native encoding */
	"STSong-Light",			/* Family name */
	(char *) NULL,			/* Weight */
	cc_simplified_chinese,		/* Character collection */
	(float) 0.0,			/* ItalicAngle */
	pdf_true,			/* isFixedPitch */
	-250,				/* llx */
	-143,				/* lly */
	600,				/* urx */
	857,				/* ury */
	-100,				/* UnderlinePosition */
	50,				/* UnderlineThickness */
	857,				/* CapHeight */
	599,				/* xHeight */
	857,				/* Ascender */
	-143,				/* Descender */
	91,				/* StdVW */
	91,				/* StdHW */

	0,				/* numOfChars */
	(CharMetricInfo *) NULL,	/* cmi */
	0,				/* numOfTracks */
	(TrackKernData *) NULL,		/* tkd */
	0,				/* numOfPairs */
	(PairKernData *) NULL,		/* pkd */
	0,				/* numOfComps */
	(CompCharData *) NULL,		/* ccd */
					/* skip character metrics array */
}

};

#endif /* P_CID_H */
