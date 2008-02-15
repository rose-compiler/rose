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

/* $Id: pc_unicode.h,v 1.1 2005/10/11 17:18:13 vuduc2 Exp $
 *
 * Unicode glyph name conversion routines
 *
 */

#ifndef PC_UNICODE_H
#define PC_UNICODE_H

#define PDC_MAX_UNICODE          (int) 0x0000FFFF

#define PDC_MAX_UVLIST           16

#define PDC_REPL_CHAR            (int) 0x0000FFFD

#define PDC_UNICODE_HT           0x0009
#define PDC_UNICODE_LF           0x000A
#define PDC_UNICODE_VT           0x000B
#define PDC_UNICODE_FF           0x000C
#define PDC_UNICODE_CR           0x000D
#define PDC_UNICODE_SPACE        0x0020
#define PDC_UNICODE_AMPERSAND    0x0026
#define PDC_UNICODE_HYPHEN       0x002D
#define PDC_UNICODE_PERIOD       0x002E
#define PDC_UNICODE_SEMICOLON    0x003B
#define PDC_UNICODE_LESS_THAN    0x003C
#define PDC_UNICODE_GREATER_THAN 0x003E
#define PDC_UNICODE_LEFT_CURLY   0x007B
#define PDC_UNICODE_RIGHT_CURLY  0x007D
#define PDC_UNICODE_DELETE       0x007F
#define PDC_UNICODE_NEL          0x0085
#define PDC_UNICODE_NBSP         0x00A0
#define PDC_UNICODE_SHY          0x00AD
#define PDC_UNICODE_MACRON       0x00AF
#define PDC_UNICODE_MICRO        0x00B5
#define PDC_UNICODE_MIDDLEDOT    0x00B7
#define PDC_UNICODE_MODMACRON    0x02C9
#define PDC_UNICODE_CAPDELTA     0x0394
#define PDC_UNICODE_CAPOMEGA     0x03A9
#define PDC_UNICODE_SMALLMU      0x03BC
#define PDC_UNICODE_LS           0x2028
#define PDC_UNICODE_PS           0x2029
#define PDC_UNICODE_FRACSLASH    0x2044
#define PDC_UNICODE_EURO         0x20AC
#define PDC_UNICODE_OHMSIGN      0x2126
#define PDC_UNICODE_INCREMENT    0x2206
#define PDC_UNICODE_DIVSLASH     0x2215
#define PDC_UNICODE_BULLETOP     0x2219

#define PDF_UNICODE_CRLF         0xFFFE
#define PDC_UNICODE_NOTCHAR      0xFFFF

/* Unicode borders of Unicode Corporate Use Subarea as used by Adobe Systems */
#define PDC_UNICODE_MINCUS       0xF600
#define PDC_UNICODE_MAXCUS       0xF8FF

/* Latin and Armenian ligatures */
#define PDC_UNICODE_CAPLIGATIJ   0x0132
#define PDC_UNICODE_SMALLLIGATIJ 0x0133
#define PDC_UNICODE_MINLIGAT     0xFB00
#define PDC_UNICODE_MAXLIGAT     0xFB17


/* The Unicode byte order mark (BOM) byte parts */
#define PDF_BOM0		 0xFE
#define PDF_BOM1                 0xFF
#define PDF_BOM2                 0xEF
#define PDF_BOM3                 0xBB
#define PDF_BOM4                 0xBF

/*
 * check whether the string is plain C or UTF16 unicode
 * by looking for the BOM in big-endian or little-endian format resp.
 * s must not be NULL.
 */
#define pdc_is_utf16be_unicode(s) \
        (((pdc_byte *)(s))[0] == PDF_BOM0 && \
         ((pdc_byte *)(s))[1] == PDF_BOM1)

#define pdc_is_utf16le_unicode(s) \
        (((pdc_byte *)(s))[0] == PDF_BOM1 && \
         ((pdc_byte *)(s))[1] == PDF_BOM0)

/*
 * check whether the string is plain C or UTF8 unicode
 * by looking for the BOM
 * s must not be NULL.
 */
#define pdc_is_utf8_unicode(s) \
        (((pdc_byte *)(s))[0] == PDF_BOM2 && \
         ((pdc_byte *)(s))[1] == PDF_BOM3 && \
         ((pdc_byte *)(s))[2] == PDF_BOM4)


#define pdc_is_utf8_bytecode(s) \
        (((pdc_byte *)(s))[0] == PDF_BOM2 && \
         ((pdc_byte *)(s))[1] == PDF_BOM3 && \
         ((pdc_byte *)(s))[2] == PDF_BOM4)
#define pdc_copy_utf8_bom(s) \
         ((pdc_byte *)(s))[0] = PDF_BOM2, \
         ((pdc_byte *)(s))[1] = PDF_BOM3, \
         ((pdc_byte *)(s))[2] = PDF_BOM4;
#define PDC_UTF8 pdc_utf8
#define PDC_UTF8_FLAG pdc_false


#define PDC_HTML_CTRLCHAR     '&'
#define PDC_HTML_DELIMITCHAR  ';'

typedef struct
{
    pdc_ushort code;
    const char *glyphname;
}
pdc_glyph_tab;

typedef struct
{
    pdc_ushort uvo;     /* original Unicode value */
    pdc_ushort uvr;     /* replacing Unicode value */
}
pdc_decomp_glyph_tab;

typedef enum
{
    conversionOK,       /* conversion successful */
    sourceExhausted,    /* partial character in source, but hit end */
    targetExhausted,    /* insuff. room in target for conversion */
    sourceIllegal       /* source sequence is illegal/malformed */
}
pdc_convers_result;

typedef enum
{
    strictConversion = 0,
    lenientConversion
}
pdc_convers_flags;

#define PDC_CONV_KEEPBYTES  (1<<0)
#define PDC_CONV_TRY7BYTES  (1<<1)
#define PDC_CONV_TRYBYTES   (1<<2)
#define PDC_CONV_WITHBOM    (1<<3)
#define PDC_CONV_NOBOM      (1<<4)
#define PDC_CONV_ANALYZE    (1<<5)
#define PDC_CONV_TMPALLOC   (1<<6)
#define PDC_CONV_HTMLCHAR   (1<<7)
#define PDC_CONV_NEWALLOC   (1<<8)

/* DON'T change the order */
typedef enum
{
    pdc_auto       = 1,
    pdc_auto2      = 2,
    pdc_bytes      = 3,
    pdc_bytes2     = 4,
    pdc_utf8       = 5,    /* UTF-8 */

    pdc_utf16      = 7,    /* UTF-16 */
    pdc_utf16be    = 8,    /* UTF-16 big endian */
    pdc_utf16le    = 9     /* UTF-16 little endian */
}
pdc_text_format;

int pdc_adobe2unicode(const char *name);
const char *pdc_unicode2adobe(pdc_ushort uv);
const char *pdc_get_notdef_glyphname(void);
int pdc_zadb2unicode(const char *name);
const char *pdc_unicode2zadb(pdc_ushort uv);
pdc_ushort pdc_string2unicode(pdc_core *pdc, const char *text, int i_flags,
                              const pdc_keyconn *keyconn, pdc_bool verbose);


pdc_bool pdc_is_std_charname(const char *name);
pdc_ushort pdc_get_equi_unicode(pdc_ushort uv);
void pdc_delete_missingglyph_bit(pdc_ushort uv, pdc_ulong *bmask);
pdc_ushort pdc_get_alter_glyphname(pdc_ushort uv, pdc_ulong bmask,
    char **glyphname);

int pdc_convert_string(pdc_core *pdc,
    pdc_text_format inutf, int codepage, pdc_encodingvector *inev,
    pdc_byte *instring, int inlen, pdc_text_format *oututf_p,
    pdc_encodingvector *outev, pdc_byte **outstring, int *outlen, int flags,
    pdc_bool verbose);

#endif /* PC_UNICODE_H */
