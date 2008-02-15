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

/* $Id: p_defopt.h,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * PDFlib shared option definitions and structures
 *
 */

#ifndef P_DEFOPT_H
#define P_DEFOPT_H

#define PDF_KEEP_TEXTLEN  (1L<<0)    /* keep text length */
#define PDF_KEEP_CONTROL  (1L<<1)    /* keep control character */
#define PDF_KEEP_UNICODE  (1L<<2)    /* keep Unicode text */
#define PDF_USE_TMPALLOC  (1L<<9)    /* use temporary memory */

#define PDF_RETURN_BOXFULL   "_boxfull"
#define PDF_RETURN_NEXTPAGE  "_nextpage"
#define PDF_RETURN_STOP      "_stop"

typedef enum
{
    is_block    = (1L<<0),
    is_image    = (1L<<1),
    is_textline = (1L<<2),
    is_textflow = (1L<<3),
    is_inline   = (1L<<4)
}
pdf_elemflags;

struct pdf_font_options_s
{
    pdc_bool autocidfont;
    pdc_bool autosubsetting;
    pdc_bool embedding;
    char *encoding;
    int flags;
    char *fontname;
    pdc_fontstyle fontstyle;
    pdc_bool fontwarning;
    pdc_bool kerning;
    int mask;
    int monospace;
    double subsetlimit;
    double subsetminsize;
    pdc_bool subsetting;
    pdc_bool unicodemap;
    pdc_bool auxiliary;
};

struct pdf_text_options_s
{
    pdc_scalar charspacing;
    pdc_scalar charspacing_pc;
    pdf_coloropt fillcolor;
    int font;
    pdc_scalar fontsize;
    pdc_bool glyphwarning;
    pdc_scalar horizscaling;
    pdc_scalar italicangle;
    pdc_bool kerning;
    int mask;
    int pcmask;
    int fontset;
    pdc_bool overline;
    pdc_bool strikeout;
    pdf_coloropt strokecolor;
    char *text;
    int textlen;
    pdc_text_format textformat;
    int textrendering;
    pdc_scalar textrise;
    pdc_scalar textrise_pc;
    pdc_scalar leading;
    pdc_scalar leading_pc;
    pdc_bool underline;
    pdc_scalar wordspacing;
    pdc_scalar wordspacing_pc;
    pdc_scalar underlinewidth;
    pdc_scalar underlineposition;
    pdc_scalar *xadvancelist;
    int nglyphs;
    char *link;
    char *linktype;
    pdc_bool charref;
};

typedef enum
{
    xo_filename,
    xo_ignoreorientation,
    xo_imagewarning,
    xo_dpi,
    xo_page,
    xo_scale
}
pdf_xobject_optflags;

typedef struct
{
    pdc_bool adjustpage;
    pdc_bool blind;
    char *filename;
    int flags;
    pdc_bool imagewarning;
    pdc_bool ignoreorientation;
    int mask;
    int im;
    int page;
    pdc_scalar dpi[2];
    pdc_scalar scale[2];
}
pdf_xobject_options;

typedef enum
{
    fit_boxsize,
    fit_fitmethod,
    fit_margin,
    fit_shrinklimit,
    fit_position,
    fit_orientate,
    fit_rotate,
    fit_refpoint
}
pdf_fit_optflags;

typedef struct
{
    pdc_scalar boxsize[2];
    pdc_fitmethod fitmethod;
    int flags;
    pdc_scalar margin[2];
    int mask;
    int pcmask;
    pdc_scalar shrinklimit;
    pdc_scalar position[2];
    int orientate;
    pdc_scalar refpoint[2];
    pdc_scalar rotate;
}
pdf_fit_options;



/* font option definitions */

#define PDF_KERNING_FLAG PDC_OPT_UNSUPP
#define PDF_SUBSETTING_FLAG PDC_OPT_UNSUPP
#define PDF_AUTOCIDFONT_FLAG PDC_OPT_UNSUPP
#define PDF_CHARREF_FLAG PDC_OPT_UNSUPP

#define PDF_FONT_OPTIONS1 \
\
    {"encoding", pdc_stringlist,  PDC_OPT_NONE, 1, 1, \
      0.0, PDF_MAX_NAMESTRING, NULL}, \
\
    {"fontname", pdc_stringlist, PDC_OPT_NONE, 1, 1, \
      1.0, PDF_MAX_FONTNAME, NULL}, \


#define PDF_FONT_OPTIONS2 \
\
    {"autocidfont", pdc_booleanlist, PDF_AUTOCIDFONT_FLAG, 1, 1, \
      0.0, 0.0, NULL}, \
\
    {"autosubsetting", pdc_booleanlist, PDF_SUBSETTING_FLAG, 1, 1, \
      0.0, 0.0, NULL}, \
\
    {"embedding", pdc_booleanlist, PDC_OPT_NONE, 1, 1, \
      0.0, 0.0, NULL}, \
\
    {"fontstyle", pdc_keywordlist, PDC_OPT_NONE, 1, 1, \
      0.0, 0.0, pdf_fontstyle_pdfkeylist}, \
\
    {"fontwarning", pdc_booleanlist, PDC_OPT_NONE, 1, 1, \
      0.0, 0.0, NULL}, \
\
    {"monospace", pdc_integerlist, PDC_OPT_NONE, 1, 1, \
      1.0, 2048.0, NULL}, \
\
    {"subsetlimit", pdc_doublelist, PDF_SUBSETTING_FLAG|PDC_OPT_PERCENT, 1, 1, \
      0.0, 100.0, NULL}, \
\
    {"subsetminsize", pdc_doublelist, PDF_SUBSETTING_FLAG, 1, 1, \
      0.0, PDC_FLOAT_MAX, NULL}, \
\
    {"subsetting", pdc_booleanlist, PDF_SUBSETTING_FLAG, 1, 1, \
      0.0, 0.0, NULL}, \
\
    {"unicodemap", pdc_booleanlist, PDF_AUTOCIDFONT_FLAG, 1, 1, \
      0.0, 0.0, NULL}, \


#define PDF_FONT_OPTIONS3 \
\
    {"kerning", pdc_booleanlist, PDF_KERNING_FLAG, 1, 1, \
      0.0, 0.0, NULL}, \


/* text option definitions */

#define PDF_TEXT_OPTIONS \
\
    {"charspacing", pdc_scalarlist, PDC_OPT_PERCENT, 1, 1, \
      PDC_FLOAT_MIN, PDC_FLOAT_MAX, NULL}, \
\
    {"glyphwarning", pdc_booleanlist, PDC_OPT_NONE, 1, 1, \
     0.0, 0.0, NULL}, \
\
    {"fillcolor", pdc_stringlist, PDC_OPT_NONE, 1, 5, \
      0.0, PDF_MAX_NAMESTRING, NULL}, \
\
    {"font", pdc_fonthandle, PDC_OPT_NONE, 1, 1, \
      0, 0, NULL}, \
\
    {"fontsize", pdc_scalarlist, PDC_OPT_NOZERO, 1, 1, \
      PDC_FLOAT_MIN, PDC_FLOAT_MAX, NULL}, \
\
    {"horizscaling", pdc_scalarlist,  PDC_OPT_PERCENT, 1, 1, \
      PDC_FLOAT_MIN, PDC_FLOAT_MAX, NULL}, \
\
    {"italicangle", pdc_scalarlist,  PDC_OPT_NONE, 1, 1, \
      -89.99, 89.99, NULL}, \
\
    {"kerning", pdc_booleanlist, PDF_KERNING_FLAG, 1, 1, \
      0.0, 0.0, NULL}, \
\
    {"overline", pdc_booleanlist, PDC_OPT_NONE, 1, 1, \
      0.0, 0.0, NULL}, \
\
    {"strikeout", pdc_booleanlist, PDC_OPT_NONE, 1, 1, \
      0.0, 0.0, NULL}, \
\
    {"strokecolor", pdc_stringlist, PDC_OPT_NONE, 1, 5, \
      0.0, PDF_MAX_NAMESTRING, NULL}, \
\
    {"textformat", pdc_keywordlist, PDC_OPT_NONE, 1, 1, \
      0.0, 0.0, pdf_textformat_keylist}, \
\
    {"textrendering", pdc_integerlist, PDC_OPT_NONE, 1, 1, \
      0, PDF_LAST_TRMODE, NULL}, \
\
    {"textrise", pdc_scalarlist, PDC_OPT_PERCENT, 1, 1, \
      PDC_FLOAT_MIN, PDC_FLOAT_MAX, NULL}, \
\
    {"underline", pdc_booleanlist, PDC_OPT_NONE, 1, 1, \
      0.0, 0.0, NULL}, \
\
    {"wordspacing", pdc_scalarlist, PDC_OPT_PERCENT, 1, 1, \
      PDC_FLOAT_MIN, PDC_FLOAT_MAX, NULL}, \
\
    {"underlinewidth", pdc_scalarlist, PDC_OPT_PERCENT, 1, 1, \
      0.0, PDC_FLOAT_MAX, pdf_underlinewidth_keylist}, \
\
    {"underlineposition", pdc_scalarlist, PDC_OPT_PERCENT, 1, 1, \
      PDC_FLOAT_MIN, PDC_FLOAT_MAX, pdf_underlineposition_keylist}, \
\
    {"weblink", pdc_stringlist, PDC_OPT_NONE, 1, 1, \
      0.0, PDF_MAXSTRINGSIZE, NULL}, \
\
    {"locallink", pdc_stringlist, PDC_OPT_NONE, 1, 1, \
      0.0, PDF_MAXSTRINGSIZE, NULL}, \
\
    {"pdflink", pdc_stringlist, PDC_OPT_NONE, 1, 1, \
      0.0, PDF_MAXSTRINGSIZE, NULL}, \
\
    {"charref", pdc_booleanlist, PDF_CHARREF_FLAG, 1, 1, \
     0.0, 0.0, NULL}, \
\


/* xobject option definitions */

#define PDF_XOBJECT_OPTIONS1 \
\
    {"adjustpage", pdc_booleanlist, PDC_OPT_PDC_1_3, 1, 1, \
      0.0, 0.0, NULL}, \
\
    {"blind", pdc_booleanlist, PDC_OPT_NONE, 1, 1, \
      0.0, 0.0, NULL}, \


#define PDF_XOBJECT_OPTIONS2 \
\
    {"ignoreorientation", pdc_booleanlist, PDC_OPT_NONE, 1, 1, \
      0.0, 0.0, NULL}, \
\
    {"imagewarning", pdc_booleanlist, PDC_OPT_NONE, 1, 1, \
      0.0, 0.0, NULL}, \
\
    {"dpi", pdc_scalarlist, PDC_OPT_NONE, 1, 2, \
      0.0, PDC_INT_MAX, pdf_dpi_keylist}, \


#define PDF_XOBJECT_OPTIONS3 \
\
    {"scale", pdc_scalarlist, PDC_OPT_NOZERO, 1, 2, \
      PDC_FLOAT_MIN, PDC_FLOAT_MAX, NULL}, \


/* general fit option definitions  */

#define PDF_FIT_OPTIONS1 \
\
    {"boxsize", pdc_scalarlist, PDC_OPT_NONE, 2, 2, \
      PDC_FLOAT_MIN, PDC_FLOAT_MAX, NULL}, \
\
    {"margin", pdc_scalarlist, PDC_OPT_NONE, 1, 2, \
      PDC_FLOAT_MIN, PDC_FLOAT_MAX, NULL}, \
\
    {"shrinklimit", pdc_scalarlist, PDC_OPT_PERCENT, 1, 1, \
      0.0, 100.0, NULL}, \
\
    {"position", pdc_scalarlist, PDC_OPT_NONE, 1, 2, \
      PDC_FLOAT_MIN, PDC_FLOAT_MAX, pdf_position_keylist}, \


#define PDF_FIT_OPTIONS2 \
\
    {"fitmethod", pdc_keywordlist, PDC_OPT_NONE, 1, 1,  \
      0.0, 0.0, pdf_fitmethod_keylist}, \
\
    {"rotate", pdc_scalarlist, PDC_OPT_NONE, 1, 1, \
      PDC_FLOAT_MIN, PDC_FLOAT_MAX, NULL}, \
\
    {"orientate", pdc_keywordlist, PDC_OPT_NONE, 1, 1, \
      0.0, 0.0, pdf_orientate_keylist}, \


#define PDF_FIT_OPTIONS3 \
\
    {"refpoint", pdc_scalarlist, PDC_OPT_NONE, 2, 2, \
      PDC_FLOAT_MIN, PDC_FLOAT_MAX, NULL}, \



/* p_font.c */
void pdf_get_font_options(PDF *p, pdf_font_options *fo, pdc_resopt *resopts);
int pdf_load_font_internal(PDF *p, pdf_font_options *fo);

/* p_image.c */
void pdf_init_xobject_options(PDF *p, pdf_xobject_options *xo);
void pdf_get_xobject_options(PDF *p, pdf_xobject_options *xo,
                             pdc_resopt *resopts);
void pdf_fit_xobject_internal(PDF *p, pdf_xobject_options *xo,
                              pdf_fit_options *fit, pdc_matrix *immatrix);

/* p_text.c */
void pdf_init_fit_options(PDF *p, pdf_fit_options *fit);
void pdf_get_fit_options(PDF *p, pdf_fit_options *fit, pdc_resopt *resopts);
void pdf_calculate_text_options(pdf_text_options *to, pdc_bool force,
        pdc_scalar fontscale);
void pdf_set_text_options(PDF *p, pdf_text_options *to);
void pdf_init_text_options(PDF *p, pdf_text_options *to);
void pdf_get_text_options(PDF *p, pdf_text_options *to, pdc_resopt *resopts);
void pdf_fit_textline_internal(PDF *p, pdf_text_options *to,
        pdf_fit_options *fit, pdc_scalar *currtx, pdc_scalar *currty);

pdc_text_format pdf_get_autotextformat(pdc_font *font, pdf_text_options *to);
pdc_bool pdf_check_textchar(PDF *p, pdc_ushort *o_uv, int flags,
        pdf_text_options *to);
pdc_byte *pdf_check_textstring(PDF *p, const char *text, int len, int flags,
                            pdf_text_options *to, int *outlen, int *outcharlen);
pdc_scalar pdf_calculate_textwidth(PDF *p, const pdc_byte *text, int len,
                                   int charlen, pdf_text_options *to);
void pdf_place_text(PDF *p, pdc_byte *text, int len, int charlen,
                    pdf_text_options *to, pdc_scalar width, pdc_bool cont);




#endif  /* P_DEFOPT_H */
