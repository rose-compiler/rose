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

/* $Id: pc_font.h,v 1.1 2005/10/11 17:18:13 vuduc2 Exp $
 *
 * Header file for font handling
 *
 */

#ifndef PC_FONT_H
#define PC_FONT_H

#include "pc_core.h"
#include "pc_geom.h"
#include "pc_encoding.h"

/* Predefined character collections */
typedef enum {
    cc_simplified_chinese,
    cc_traditional_chinese,
    cc_japanese,
    cc_korean,
    cc_identity,
    cc_none
} pdc_charcoll;

/* Font types */
typedef enum {
    pdc_Type1,          /* Type1 fonts */
    pdc_MMType1,        /* Multiple master fonts */
    pdc_TrueType,       /* TrueType fonts for 1-byte encoding */
    pdc_CIDFontType2,   /* TrueType fonts for 2-byte encoding */
    pdc_Type1C,         /* CFF PostScript fonts for 1-byte encoding */
    pdc_CIDFontType0,   /* OpenType fonts with CFF_ table for 2-byte encoding */
    pdc_CIDFontType0C,  /* CFF PostScript fonts for 2-byte encoding */
    pdc_Type3,          /* Type3 fonts */
    pdc_unknownType     /* for initialization only */
} pdc_fonttype;

/* Font styles */
typedef enum {
    pdc_Normal,
    pdc_Bold,
    pdc_Italic,
    pdc_BoldItalic
} pdc_fontstyle;

typedef struct pdc_interwidth_s pdc_interwidth;
typedef struct pdc_core_metric_s pdc_core_metric;
typedef struct pdc_glyphwidth_s pdc_glyphwidth;
typedef struct pdc_kerningpair_s pdc_kerningpair;
typedef struct pdc_widthdata_s pdc_widthdata;
typedef struct pdc_t3glyph_s pdc_t3glyph;
typedef struct pdc_t3font_s pdc_t3font;
typedef struct pdc_font_s pdc_font;

/* Glyph width data structure */
struct pdc_glyphwidth_s
{
    pdc_ushort unicode;     /* unicode of glyph */
    short code;             /* builtin 8-bit code */
    pdc_short width;        /* glyph width */
};

/* Kerning pair data */
struct pdc_kerningpair_s
{
    pdc_ushort glyph1;      /* either 8-bit code or unicode of glyph 1 */
    pdc_ushort glyph2;      /* either 8-bit code or unicode of glyph 2 */
    short xamt;             /* x amount of kerning */
    short dominant;         /* = 1: kerning pair domimant */
};

struct pdc_widthdata_s
{
    pdc_ushort  startcode;  /* start unicode value of interval */
    int         width;      /* width of characters in the code */
                            /* interval  */
};

struct pdc_t3glyph_s
{
    char        *name;
    pdc_id      charproc_id;
    pdc_scalar  width;
};

struct pdc_t3font_s
{
    pdc_t3glyph *glyphs;        /* dynamically growing glyph table */
    int         capacity;       /* current number of slots */
    int         next_glyph;     /* next available slot */

    char        *fontname;      /* fontname */
    pdc_id      charprocs_id;   /* id of /CharProcs dict */
    pdc_id      res_id;         /* id of /Resources dict */
    pdc_bool    colorized;      /* glyphs colorized */
    pdc_matrix  matrix;         /* font matrix */
    pdc_rectangle bbox;		/* font bounding box */
};

/* The core PDFlib font structure */
struct pdc_font_s {
    pdc_bool    verbose;                /* put out warning/error messages */
    pdc_bool    verbose_open;           /* after opening font file */
    char        *name;                  /* fontname */
    char        *apiname;               /* fontname specified in API call */
    char        *utf8name;              /* fontname in UTF-8 with BOM */
    char        *ttname;                /* names[6] in the TT name table */
    char        *fontfilename;          /* name of external font file */

    pdc_bool    vertical;               /* vertical writing mode */
    pdc_bool    embedding;              /* font embedding */
    pdc_bool    kerning;                /* font kerning */
    pdc_bool    autocidfont;            /* automatic convert to CID font */
    pdc_bool    unicodemap;             /* automatic creation of Unicode CMap */
    pdc_bool    subsetting;             /* font subsetting */
    pdc_bool    autosubsetting;         /* automatic font subsetting */
    double      subsetminsize;          /* minimal size for font subsetting */
    double      subsetlimit;            /* maximal percent for font subsetting*/

    pdc_bool    used_in_formfield;      /* this font is in use in form field */
    pdc_bool    used_in_current_doc;    /* this font is in use in current d. */
    pdc_bool    used_on_current_page;   /* this font is in use on current p. */
    pdc_id      obj_id;                 /* object id of this font */

    unsigned long flags;                /* font flags for font descriptor */
    pdc_fonttype  type;                 /* type of font */
    pdc_fontstyle style;                /* TT: style of font */
    pdc_bool      isstdlatin;           /* is standard latin font */
    pdc_bool      hasnomac;             /* TT: has no macroman cmap (0,1) */

    pdc_encoding  encoding;             /* PDFlib font encoding shortcut */
    int           codepage;             /* OEM multi byte code-page number */
    int           charcoll;             /* CID character collection supported */
    char          *cmapname;            /* CID CMap name */
    char          *encapiname;          /* Encoding name specified in API call*/
    pdc_encoding  towinansi;            /* convert to winansi enc. for output */
    pdc_bool      standardcjk;          /* is standard CJK font */
    pdc_bool      hwcmap;               /* HW japanese Unicode CMap */
    pdc_bool      unibyte;              /* Unicode encoding as byte encoding */
    pdc_bool      asciispace;           /* encoding has space at x20 */
    pdc_bool      issemantic;           /* encoding is Unicode interpretable */
    pdc_ushort    spacechar;            /* code of space character */
    pdc_ulong     missingglyphs;        /* bit mask for missing new AGL glyphs*/

    double      italicAngle;            /* AFM key: ItalicAngle */
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
    int         monospace;              /* monospace amount */

    int                 numOfGlyphs;    /* # of Glyph ID's */
    pdc_glyphwidth      *glw;           /* ptr to glyph metrics array */
    int                 numOfPairs;     /* # of entries in pair kerning array */
    pdc_kerningpair     *pkd;           /* ptr to pair kerning array */

    int                 codeSize;       /* code size */
                                        /* = 0: unknown, no Unicode CMap */
                                        /* = 1: 1 byte encoding */
                                        /* = 2: 2 byte encoding */
    int                 numOfCodes;     /* # of codes defined by encoding */
    int                 lastCode;       /* last byte code for generating */
                                        /* runtime byte encoding. */
                                        /* = -1: ignore */
    pdc_bool            expectGlyphs;   /* glyph id text strings are expected */
    pdc_bool            names_tbf;      /* glyph names to be freed */
    char                **GID2Name;     /* mapping Glyph ID -> Glyph name */
    pdc_ushort          *GID2code;      /* mapping Glyph ID -> code */
                                        /* glyphid: code = unicode! */
    pdc_ushort          *code2GID;      /* mapping code -> Glyph ID */
                                        /* glyphid: code = glyphid! */
    pdc_ushort          *usedGIDs;      /* used Glyph IDs */
    int                 defWidth;       /* default width */
    int                 numOfWidths;    /* # of character width intervals */
    pdc_widthdata       *widthsTab;     /* ptr to character width intervals */
                                        /* or NULL - then consecutive */
    int                 *widths;        /* characters widths [numOfCodes] */
    char                *usedChars;     /* bit field for used characters  */
                                        /* in a document */

    char                *imgname;       /* name of virtual file contains *img */
    size_t              filelen;        /* length of uncompressed font data */
    pdc_byte            *img;           /* font (or CFF table) data in memory */
    long                cff_offset;     /* start of CFF table in font */
    size_t              cff_length;     /* length of CFF table in font */
    pdc_t3font          *t3font;        /* type 3 font data */

#if defined(MAC) || defined(MACOSX)
    int                 hostfont;       /* ID of MAC host font */
    int                 macfontstyle;   /* QickDraw font style */
    size_t              fondlen;        /* length of FOND resource */
    pdc_byte            *fond;          /* FOND resource in memory */
#endif
};

/* these defaults are used when the stem value must be derived from the name */
#define PDF_STEMV_MIN           50      /* minimum StemV value */
#define PDF_STEMV_LIGHT         71      /* light StemV value */
#define PDF_STEMV_NORMAL        109     /* normal StemV value */
#define PDF_STEMV_MEDIUM        125     /* mediumbold StemV value */
#define PDF_STEMV_SEMIBOLD      135     /* semibold StemV value */
#define PDF_STEMV_BOLD          165     /* bold StemV value */
#define PDF_STEMV_EXTRABOLD     201     /* extrabold StemV value */
#define PDF_STEMV_BLACK         241     /* black StemV value */

/* Bit positions for the font descriptor flag */
#define FIXEDWIDTH      (long) (1L<<0)
#define SERIF           (long) (1L<<1)
#define SYMBOL          (long) (1L<<2)
#define SCRIPT          (long) (1L<<3)
#define ADOBESTANDARD   (long) (1L<<5)
#define ITALIC          (long) (1L<<6)
#define SMALLCAPS       (long) (1L<<17)
#define FORCEBOLD       (long) (1L<<18)

#define PDC_DEF_ITALICANGLE    -12     /* default italic angle */

/* pc_font.c */
void pdc_init_font_struct(pdc_core *pdc, pdc_font *font);
void pdc_cleanup_font_struct(pdc_core *pdc, pdc_font *font);
void pdc_font_trace_protocol(pdc_core *pdc, pdc_font *font);
void pdc_cleanup_t3font_struct(pdc_core *pdc, pdc_t3font *t3font);

/* pc_corefont.c */
pdc_bool pdc_is_standard_font(const char *fontname);
const char *pdc_get_abb_std_fontname(const char *fontname);
const pdc_core_metric *pdc_get_core_metric(const char *fontname);
void pdc_init_core_metric(pdc_core *pdc, pdc_core_metric *metric);
void pdc_cleanup_core_metric(pdc_core *pdc, pdc_core_metric *metric);
void pdc_fill_core_metric(pdc_core *pdc,
                          pdc_font *font, pdc_core_metric *metric);
void pdc_fill_font_metric(pdc_core *pdc,
                          pdc_font *font, const pdc_core_metric *metric);

#endif  /* PC_FONT_H */
