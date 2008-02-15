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

/* $Id: pc_encoding.h,v 1.1 2005/10/11 17:18:13 vuduc2 Exp $
 *
 * Encoding data structures and routines
 *
 */

#ifndef PC_ENCODING_H
#define PC_ENCODING_H

/*
 * Symbolic names for predefined font encodings. 0 and above are used
 * as indices in the pdc_encodingvector array. The encodings starting at
 * pdc_firstvarenc have no enumeration name, because they are loaded
 * dynamically.
 * The predefined encodings must not be changed or rearranged.
 * The order of encodings here must match that of pdc_core_encodings
 * and pdc_fixed_encoding_names in pc_encoding.c.
 */
typedef enum
{
    pdc_invalidenc = -5,
    pdc_glyphid = -4,
    pdc_unicode = -3,
    pdc_builtin = -2,
    pdc_cid = -1,
    pdc_winansi = 0,
    pdc_macroman = 1,
    pdc_macroman_apple = 2,
    pdc_ebcdic = 3,
    pdc_ebcdic_37 = 4,
    pdc_pdfdoc = 5,
    pdc_stdenc = 6,
    pdc_macexpert = 7,
    pdc_firstvarenc = 8
}
pdc_encoding;

#define PDC_EBCDIC_NAME "ebcdic"
#define PDC_EBCDIC_ENC pdc_ebcdic

typedef struct pdc_encodingvector_s pdc_encodingvector;

struct pdc_encodingvector_s
{
    char *apiname;             /* PDFlib's name of the encoding at the API */
    pdc_ushort codes[256];     /* unicode values */
    char *chars[256];          /* character names */
    char given[256];           /* flags for kind of given character name */
    pdc_byte *sortedslots;     /* slots for sorted unicode values */
    int nslots;                /* number of sorted slots */
    unsigned long flags;       /* flags, see PDC_ENC_... */
};

#define PDC_ENC_INCORE      (1L<<0) /* encoding from in-core */
#define PDC_ENC_FILE        (1L<<1) /* encoding from file */
#define PDC_ENC_HOST        (1L<<2) /* encoding from host system */
#define PDC_ENC_USER        (1L<<3) /* encoding from user */
#define PDC_ENC_FONT        (1L<<4) /* encoding from font */
#define PDC_ENC_GENERATE    (1L<<5) /* encoding generated from Unicode page */
#define PDC_ENC_USED        (1L<<6) /* encoding already used */
#define PDC_ENC_SETNAMES    (1L<<7) /* character names are set */
#define PDC_ENC_ALLOCCHARS  (1L<<8) /* character names are allocated */
#define PDC_ENC_STDNAMES    (1L<<9) /* character names are all Adobe standard */
#define PDC_ENC_TEMP       (1L<<10) /* temporary generated encoding */

#define PDC_ENC_ISUNDEF     (1<<0)  /* is undefined (unknown Unicode(s) */
#define PDC_ENC_ISAGLNAME   (1<<1)  /* is AGL 1.2' name (without ambiguity) */
#define PDC_ENC_ISZADBNAME  (1<<2)  /* is ZapfDingbats name */
#define PDC_ENC_ISUNINAME   (1<<3)  /* is a "uni" name (with single Unicode) */
#define PDC_ENC_ISAMBIG     (1<<4)  /* is ambiguous name (double mapping) */
#define PDC_ENC_ISVARIANT   (1<<5)  /* is glyphic variant (contains period) */
#define PDC_ENC_ISCOMP      (1<<6)  /* is composed glyph (contains underscores
                                     * or more than one Unicode values) */
#define PDC_ENC_ISCUS       (1<<7)  /* is a glyph from Unicode's Corporate
                                     * Use Subarea (CUS) used by Adobe
                                     * (U+F600 - U+F8FF) */
#define PDC_ENC_ISLIGATURE  (1<<8)  /* is a Latin or Armenian ligature glyph */

#define PDC_ENC_OLDMAPPING  (1<<0)  /* get alternative mapping */
#define PDC_ENC_RESOLCUS    (1<<1)  /* resolve CUS value */
#define PDC_ENC_RESOLLIGAT  (1<<2)  /* resolve ligature value */

#define PDC_ENC_MODSEPAR     "_"          /* separator of modified encoding */
#define PDC_ENC_MODWINANSI   "winansi_"   /* prefix of modified winansi enc */
#define PDC_ENC_MODMACROMAN  "macroman_"  /* prefix of modified macroman enc */
#define PDC_ENC_MODEBCDIC    "ebcdic_"    /* prefix of modified ebcdic enc */
#define PDC_ENC_ISO8859      "iso8859-"   /* begin of iso8859 enc name */
#define PDC_ENC_CP125        "cp125"      /* begin of ANSI enc name */

#define PDC_ENC_TEMPNAME "__temp__enc__"  /* name of temporary encoding */

/* pc_encoding.c */
void pdc_init_encoding(pdc_core *pdc, pdc_encodingvector *ev,
                       const char *name);
pdc_encodingvector *pdc_new_encoding(pdc_core *pdc, const char *name);
void pdc_cleanup_encoding(pdc_core *pdc, pdc_encodingvector *ev);
int pdc_get_encoding_bytecode(pdc_core *pdc, pdc_encodingvector *ev,
                              pdc_ushort uv);
pdc_byte pdc_transform_bytecode(pdc_core *pdc, pdc_encodingvector *evto,
                                pdc_encodingvector *evfrom, pdc_byte code);
pdc_encodingvector *pdc_copy_core_encoding(pdc_core *pdc, const char *encoding);
const char *pdc_get_fixed_encoding_name(pdc_encoding enc);
const char *pdc_get_pdf_encoding_name(pdc_encoding enc);
pdc_encodingvector *pdc_generate_pdfencoding(pdc_core *pdc,
                                             const char *pdfname);
int pdc_glyphname2unicode(pdc_core *pdc, const char *glyphname);
const char *pdc_unicode2glyphname(pdc_core *pdc, pdc_ushort uv);
pdc_ushort pdc_insert_glyphname(pdc_core *pdc, const char *glyphname);
const char *pdc_insert_unicode(pdc_core *pdc, pdc_ushort uv);
pdc_ushort pdc_register_glyphname(pdc_core *pdc, const char *glyphname,
                                  pdc_ushort uv);
void pdc_init_pglyphtab(pdc_core *pdc);
void pdc_cleanup_pglyphtab(pdc_core *pdc);


#endif  /* PC_ENCODING_H */
