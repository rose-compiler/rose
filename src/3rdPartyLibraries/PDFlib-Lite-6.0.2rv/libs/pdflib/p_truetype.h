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

/* $Id: p_truetype.h,v 1.1 2005/10/11 17:18:15 vuduc2 Exp $
 *
 * PDFlib TrueType typedefs, structures, and enums
 *
 */

#ifndef P_TRUETYPE_H
#define P_TRUETYPE_H

/***********************/
#define PDF_KEEP_SUBRS
/***********************/

typedef unsigned char	tt_byte;
typedef char	tt_char;
typedef unsigned short	tt_ushort;
typedef short	tt_short;
typedef unsigned int	tt_ulong;
typedef int	tt_long;

typedef long	tt_fixed;
typedef short	tt_fword;
typedef unsigned short	tt_ufword;

#define tt_get_fixed	tt_get_long
#define tt_get_fword	tt_get_short
#define tt_get_ufword	tt_get_ushort

#define TT_XKERNPAIR_CHUNKSIZE  256
#define TT_ACCGIDTAB_CHUNKSIZE  256

#define TT_OPERAND_STACKSIZE 48

#define TT_NUMSTDSTRINGS  391
#define TT_NUMISOSTRINGS  229
#define TT_NUMEXPSTRINGS  379
#define TT_NUMEXPERTGIDS  166

#define TT_OFFSETTAB_SIZE  12

#define TT_MINUSEDCHARS_SIZE  32

typedef enum
{
    cff_version =             0,
    cff_Notice =              1,
    cff_FullName =            2,
    cff_FamilyName =          3,
    cff_Weight =              4,
    cff_FontBBox =            5,
    cff_BlueValues =          6,
    cff_OtherBlues =          7,
    cff_FamilyBlues =         8,
    cff_FamilyOtherBlues =    9,
    cff_StdHW =               10,
    cff_StdVW =               11,
    cff_Copyright =           0x100,   /* 12  0 */
    cff_isFixedPitch =        0x101,   /* 12  1 */
    cff_ItalicAngle =         0x102,   /* 12  2 */
    cff_UnderlinePosition =   0x103,   /* 12  3 */
    cff_UnderlineThickness =  0x104,   /* 12  4 */
    cff_PaintType =           0x105,   /* 12  5 */
    cff_CharstringType =      0x106,   /* 12  6 */
    cff_FontMatrix =          0x107,   /* 12  7 */
    cff_StrokeWidth =         0x108,   /* 12  8 */
    cff_BlueScale =           0x109,   /* 12  9 */
    cff_BlueShift =           0x10A,   /* 12 10 */
    cff_BlueFuzz =            0x10B,   /* 12 11 */
    cff_StemSnapH =           0x10C,   /* 12 12 */
    cff_StemSnapV =           0x10D,   /* 12 13 */
    cff_ForceBold =           0x10E,   /* 12 14 */
    /*                        0x10F,      12 15 */
    /*                        0x110,      12 16 */
    cff_LanguageGroup =       0x111,   /* 12 17 */
    cff_ExpansionFactor =     0x112,   /* 12 18 */
    cff_initialRandomSeed =   0x113,   /* 12 19 */
    cff_SyntheticBase =       0x114,   /* 12 20 */
    cff_PostScript =          0x115,   /* 12 21 */
    cff_BaseFontName =        0x116,   /* 12 22 */
    cff_BaseFontBlend =       0x117,   /* 12 23 */
    /*                        0x118,      12 24 */
    /*                        0x119,      12 25 */
    /*                        0x11A,      12 26 */
    /*                        0x11B,      12 27 */
    /*                        0x11C,      12 28 */
    /*                        0x11D,      12 29 */
    cff_ROS =                 0x11E,   /* 12 30 */
    cff_CIDFontVersion =      0x11F,   /* 12 31 */
    cff_CIDFontRevision =     0x120,   /* 12 32 */
    cff_CIDFontType =         0x121,   /* 12 33 */
    cff_CIDCount =            0x122,   /* 12 34 */
    cff_UIDBase =             0x123,   /* 12 35 */
    cff_FDArray =             0x124,   /* 12 36 */
    cff_FDSelect =            0x125,   /* 12 37 */
    cff_FontName =            0x126,   /* 12 38 */
    /*                        0x127,      12 39 */
    cff_UniqueID =            13,
    cff_XUID =                14,
    cff_charset =             15,
    cff_Encoding =            16,
    cff_CharStrings =         17,
    cff_Private =             18,
    cff_Subrs =               19,
    cff_defaultWidthX =       20,
    cff_nominalWidthX =       21
}
tt_cff_oper;

typedef struct
{
    char	tag[5];
    tt_ulong	checksum;
    tt_ulong	offset;
    tt_ulong	length;
} tt_dirent;

typedef enum
{
    tt_pfid_apple,
    tt_pfid_mac,
    tt_pfid_iso,
    tt_pfid_win
} tt_platform_id;

typedef enum
{
    tt_wenc_symbol,
    tt_wenc_text
} tt_win_encoding_id;

/* format 4 encoding table:
*/
typedef struct
{
    tt_ushort   encodingID;
    tt_ushort	format;
    tt_ushort	length;
    tt_ushort	version;
    tt_ushort	segCountX2;	/* segCount * 2		*/
    tt_ushort	searchRange;
    tt_ushort	entrySelector;
    tt_ushort	rangeShift;
    tt_ushort *	endCount;	/* [segCount]		*/
    tt_ushort *	startCount;	/* [segCount]		*/
    tt_short *	idDelta;	/* [segCount]		*/
    tt_ushort *	idRangeOffs;	/* [segCount]		*/
    int		numGlyphIds;
    tt_ushort *	glyphIdArray;
} tt_cmap4;

/* combined data structure for format 0 and format 6 encoding tables:
*/
typedef struct
{
    tt_ushort	format;
    tt_ushort	length;
    tt_ushort	language;
    tt_ushort	firstCode;
    tt_ushort	entryCount;
    tt_ushort	*glyphIdArray;
} tt_cmap0_6;

typedef struct
{
    tt_ushort	version;
    tt_ushort	numEncTabs;

    tt_cmap4	*win;
    tt_cmap0_6	*mac;
} tt_tab_cmap;

typedef struct
{
    tt_fixed	version;
    tt_fixed	fontRevision;
    tt_ulong	checkSumAdjustment;
    tt_ulong	magicNumber;
    tt_ushort	flags;
    tt_ushort	unitsPerEm;
    tt_ulong	created[2];
    tt_ulong	modified[2];
    tt_fword	xMin, yMin;
    tt_fword	xMax, yMax;
    tt_ushort	macStyle;
    tt_ushort	lowestRecPPEM;
    tt_short	fontDirectionHint;
    tt_short	indexToLocFormat;
    tt_short	glyphDataFormat;
} tt_tab_head;

typedef struct
{
    tt_fixed	version;
    tt_fword	ascender;
    tt_fword	descender;
    tt_fword	lineGap;
    tt_fword	advanceWidthMax;        /* specification: tt_ufword */
    tt_fword	minLeftSideBearing;
    tt_fword	minRightSideBearing;
    tt_fword	xMaxExtent;
    tt_short	caretSlopeRise;
    tt_short	caretSlopeRun;
    tt_short	res1;
    tt_short	res2;
    tt_short	res3;
    tt_short	res4;
    tt_short	res5;
    tt_short	metricDataFormat;
    tt_ushort	numberOfHMetrics;
} tt_tab_hhea;

typedef struct
{
    tt_fword    advanceWidth;           /* specification: tt_ufword */
    tt_fword	lsb;
} tt_metric;

typedef struct
{
    tt_metric *	metrics;
    tt_fword *	lsbs;
} tt_tab_hmtx;

typedef struct
{
    tt_fixed	version;
    tt_ushort	numGlyphs;
    tt_ushort	maxPoints;
    tt_ushort	maxContours;
    tt_ushort	maxCompositePoints;
    tt_ushort	maxCompositeContours;
    tt_ushort	maxZones;
    tt_ushort	maxTwilightPoints;
    tt_ushort	maxStorage;
    tt_ushort	maxFunctionDefs;
    tt_ushort	maxInstructionDefs;
    tt_ushort	maxStackElements;
    tt_ushort	maxSizeOfInstructions;
    tt_ushort	maxComponentElements;
    tt_ushort	maxComponentDepth;
} tt_tab_maxp;

typedef struct
{
    tt_ushort   platform;
    tt_ushort   language;
    tt_ushort   namid;
    tt_ushort   length;
    tt_ushort   offset;
} tt_nameref;

typedef struct
{
    tt_ushort	format;
    tt_ushort	numNameRecords;
    tt_ushort	offsetStrings;
    tt_nameref *namerecords;
    char       *englishname4;
    char       *englishname6;
} tt_tab_name;

typedef struct
{
    tt_ushort	version;
    tt_short	xAvgCharWidth;
    tt_ushort	usWeightClass;
    tt_ushort	usWidthClass;
    tt_ushort	fsType;			/* tt_short? */
    tt_short	ySubscriptXSize;
    tt_short	ySubscriptYSize;
    tt_short	ySubscriptXOffset;
    tt_short	ySubscriptYOffset;
    tt_short	ySuperscriptXSize;
    tt_short	ySuperscriptYSize;
    tt_short	ySuperscriptXOffset;
    tt_short	ySuperscriptYOffset;
    tt_short	yStrikeoutSize;
    tt_short	yStrikeoutPosition;
    tt_ushort	sFamilyClass;		/* tt_short? */
    tt_byte	panose[10];
    tt_ulong	ulUnicodeRange1;
    tt_ulong	ulUnicodeRange2;
    tt_ulong	ulUnicodeRange3;
    tt_ulong	ulUnicodeRange4;
    tt_char	achVendID[4];
    tt_ushort	fsSelection;
    tt_ushort	usFirstCharIndex;
    tt_ushort	usLastCharIndex;

    /* tt_ushort according to spec; obviously a spec bug.
    */
    tt_short	sTypoAscender;
    tt_short	sTypoDescender;
    tt_short	sTypoLineGap;

    tt_ushort	usWinAscent;
    tt_ushort	usWinDescent;

    /* if version >= 1
    */
    tt_ulong	ulCodePageRange[2];

    /* if version >= 2 (according to OpenType spec)
    */
    tt_short	sxHeight;
    tt_short	sCapHeight;
    tt_ushort	usDefaultChar;
    tt_ushort	usBreakChar;
    tt_ushort	usMaxContext;
} tt_tab_OS_2;


typedef struct
{
    tt_ulong       offset;
    tt_ulong       length;
}
tt_tab_CFF_;

typedef struct
{
    tt_fixed	formatType;
    double	italicAngle;
    tt_fword	underlinePosition;
    tt_fword	underlineThickness;
    tt_ulong	isFixedPitch;
    tt_ulong	minMemType42;
    tt_ulong	maxMemType42;
    tt_ulong	minMemType1;
    tt_ulong	maxMemType1;

} tt_tab_post;

typedef struct
{
    tt_ushort   start;
    tt_ushort   end;
    tt_ushort   cclass;
} tt_record_classrange;

typedef struct
{
    tt_ushort	left;
    tt_ushort	right;
    tt_short	value;
    tt_short    dominant;
} tt_xkern_pair;

typedef struct
{
    tt_xkern_pair *pairs;
    int            capacity;
    int            number;
} tt_xkern_pair_list;

typedef struct
{
    pdc_bool    check;          /* check for fontname */
    pdc_bool    incore;         /* whole font in-core */
    pdc_bool    savecff;        /* save results of CFF analyzing */
    tt_byte *	img;		/* in-core TT font file image	*/
    tt_byte *	end;		/* first byte above image buf	*/
    tt_byte *	pos;		/* current "file" position	*/
    pdc_file *	fp;

    int		n_tables;
    tt_ulong    offset;
    tt_dirent *	dir;

    /* "required" tables:
    */
    tt_tab_cmap *tab_cmap;
    tt_tab_head *tab_head;
    tt_tab_hhea *tab_hhea;
    tt_tab_hmtx *tab_hmtx;
    tt_tab_maxp *tab_maxp;
    tt_tab_name *tab_name;
    tt_tab_post *tab_post;
    tt_tab_OS_2 *tab_OS_2;
    tt_tab_CFF_ *tab_CFF_;

    tt_ushort   *GID2Code;      /* [ttf->tab_maxp->numGlyphs] */
                                /* glyphid: Code = Unicode */
    int         numGlyphs;      /* number of glyphs */
    int         hasGlyphNames;  /* CFF: has glyph names */
    int         minunival;      /* minimal Unicode value and */
    int         maxunival;      /* minimal Unicode value for cmap supplement */
    int         builtinenc;     /* builtin encoding */
    int         charcoll;       /* character collection */


    const char *filename;       /* font file name */
    const char *fontname;       /* font API name */
    char *utf16fontname;        /* UTF-16-BE font name for TTC fonts */
    int fnamelen;               /* font name length */
    pdc_font   *font;
} tt_file;

/* TrueType table names, defined as octal codes */
#define pdf_str_ttcf    "\164\164\143\146"
#define pdf_str_bhed    "\142\150\145\144"
#define pdf_str_CFF_    "\103\106\106\040"
#define pdf_str_cvt_	"\143\166\164\040"
#define pdf_str_cmap	"\143\155\141\160"
#define pdf_str_fpgm	"\146\160\147\155"
#define pdf_str_glyf	"\147\154\171\146"
#define pdf_str_GPOS    "\107\120\117\123"
#define pdf_str_head	"\150\145\141\144"
#define pdf_str_hhea	"\150\150\145\141"
#define pdf_str_hmtx	"\150\155\164\170"
#define pdf_str_kern    "\153\145\162\156"
#define pdf_str_loca	"\154\157\143\141"
#define pdf_str_maxp	"\155\141\170\160"
#define pdf_str_name	"\156\141\155\145"
#define pdf_str_OS_2	"\117\123\057\062"	/* OS/2 */
#define pdf_str_post	"\160\157\163\164"
#define pdf_str_prep	"\160\162\145\160"

/* Windows code page numbers */
#define TT_CP1252     0 /* Latin 1 */
#define TT_CP1250     1 /* Latin 2: Eastern Europe */
#define TT_CP1251     2 /* Cyrillic */
#define TT_CP1253     3 /* Greek */
#define TT_CP1254     4 /* Turkish */
#define TT_CP1255     5 /* Hebrew */
#define TT_CP1256     6 /* Arabic */
#define TT_CP1257     7 /* Windows Baltic */
#define TT_CP1258     8 /* Vietnamese */
#define TT_CP874     16 /* Thai */
#define TT_CP932     17 /* JIS/Japan */
#define TT_CP936     18 /* Chinese: Simplified chars--PRC and Singapore */
#define TT_CP949     19 /* Korean Wansung */
#define TT_CP950     20 /* Chinese: Traditional chars--Taiwan and Hong Kong */
#define TT_CP1361    21 /* Korean Johab */
#define TT_CP869     48 /* IBM Greek */
#define TT_CP866     49 /* MS-DOS Russian */
#define TT_CP865     50 /* MS-DOS Nordic */
#define TT_CP864     51 /* Arabic */
#define TT_CP863     52 /* MS-DOS Canadian French */
#define TT_CP862     53 /* Hebrew */
#define TT_CP861     54 /* MS-DOS Icelandic */
#define TT_CP860     55 /* MS-DOS Portuguese */
#define TT_CP857     56 /* IBM Turkish */
#define TT_CP855     57 /* IBM Cyrillic; primarily Russian */
#define TT_CP852     58 /* Latin 2 */
#define TT_CP775     59 /* MS-DOS Baltic */
#define TT_CP737     60 /* Greek; former 437 G */
#define TT_CP708     61 /* Arabic; ASMO 708 */
#define TT_CP850     62 /* WE/Latin 1 */
#define TT_CP437     63 /* US */

/* Composite font flags. */
/* See Reference of OpenType: glyf - Glyf Data */
#define ARGS_ARE_WORDS       0x001
#define ARGS_ARE_XY_VALUES   0x002
#define ROUND_XY_TO_GRID     0x004
#define WE_HAVE_A_SCALE      0x008
/* reserved                  0x010 */
#define MORE_COMPONENTS      0x020
#define WE_HAVE_AN_XY_SCALE  0x040
#define WE_HAVE_A_2X2        0x080
#define WE_HAVE_INSTR        0x100
#define USE_MY_METRICS       0x200


/* Functions */

#define PDF_TT2PDF(v) (int) PDC_ROUND(v * 1000.0 / ttf->tab_head->unitsPerEm)

#define TT_ASSERT(p, ttf, cond)         \
        ((cond) ? (void) 0 : tt_assert(p, ttf))

#define TT_IOCHECK(p, ttf, cond)        \
        ((cond) ? (void) 0 : tt_error(p, ttf))

tt_file  *pdf_init_tt(PDF *p);
pdc_bool  pdf_read_offset_tab(PDF *p, tt_file *ttf);
pdc_bool  pdf_test_tt_font(PDF *p, tt_byte *img, tt_ulong *n_fonts);
void      pdf_cleanup_tt(PDF *p, tt_file *ttf);

int       tt_tag2idx(tt_file *ttf, char *tag);
void      tt_get_tab_maxp(PDF *p, tt_file *ttf);
void      tt_get_tab_head(PDF *p, tt_file *ttf);
void      tt_get_tab_cmap(PDF *p, tt_file *ttf);
pdc_bool  tt_get_tab_CFF_(PDF *p, tt_file *ttf);
void      tt_get_tab_OS_2(PDF *p, tt_file *ttf);



void	  tt_assert(PDF *p, tt_file *ttf);
void	  tt_error(PDF *p, tt_file *ttf);
void      tt_seek(PDF *p, tt_file *ttf, long offset);
void      tt_read(PDF *p, tt_file *ttf, void *buf, unsigned int nbytes);
long      tt_tell(PDF *p, tt_file *ttf);
tt_ushort tt_get_ushort(PDF *p, tt_file *ttf);
tt_short  tt_get_short(PDF *p, tt_file *ttf);
tt_ulong  tt_get_ulong3(PDF *p, tt_file *ttf);
tt_ulong  tt_get_ulong(PDF *p, tt_file *ttf);
tt_long   tt_get_long(PDF *p, tt_file *ttf);
tt_ulong  tt_get_offset(PDF *p, tt_file *ttf, tt_byte offsize);


#endif /* P_TRUETYPE_H */

