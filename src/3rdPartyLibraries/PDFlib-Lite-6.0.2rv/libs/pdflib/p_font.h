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

/* $Id: p_font.h,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * Header file for the PDFlib font subsystem
 *
 */

#ifndef P_FONT_H
#define P_FONT_H

#define PDF_DEFAULT_WIDTH       250     /* some reasonable default */
#define PDF_DEFAULT_CIDWIDTH    1000    /* for CID fonts */
#define PDF_DEFAULT_GLYPH       0x0020  /* space */

/* internal maximal length of fontnames */
#define PDF_MAX_FONTNAME        128

/* last text rendering mode number */
#define  PDF_LAST_TRMODE  7

struct pdf_encoding_s
{
    pdc_encodingvector *ev;        /* encoding vector */
    pdc_id      id;                /* encoding object id */
    pdc_id      tounicode_id;      /* tounicode object ids */
    pdc_bool    used_in_formfield; /* this encoding is in use in form field */
};

/* p_truetype.c */
pdc_bool        pdf_get_metrics_tt(PDF *p, pdc_font *font,
                    const char *fontname, pdc_encoding enc,
                    const char *filename);
int             pdf_check_tt_font(PDF *p, const char *filename,
                    const char *fontname, pdc_font *font);
int             pdf_check_tt_hostfont(PDF *p, const char *hostname);

/* p_afm.c */
pdc_bool        pdf_process_metrics_data(PDF *p, pdc_font *font,
                    const char *fontname);
pdc_bool        pdf_get_metrics_afm(PDF *p, pdc_font *font,
                    const char *fontname, pdc_encoding enc,
                    const char *filename);
pdc_bool        pdf_get_core_metrics_afm(PDF *p, pdc_font *font,
                    pdc_core_metric *metric, const char *fontname,
                    const char *filename);

/* p_pfm.c */
pdc_bool        pdf_check_pfm_encoding(PDF *p, pdc_font *font,
                       const char *fontname, pdc_encoding enc);
pdc_bool        pdf_get_metrics_pfm(PDF *p, pdc_font *font,
                    const char *fontname, pdc_encoding enc,
                    const char *filename);

/* p_cid.c */
pdc_bool        pdf_get_metrics_cid(PDF *p, pdc_font *font,
                                    const char *fontname,
                                    const char *encoding);
pdc_encoding    pdf_handle_cidfont(PDF *p, const char *fontname,
                                   const char *encoding, int *o_slot);
const char*     pdf_get_ordering_cid(PDF *p, pdc_font *font);
int             pdf_get_charcoll(const char *ordering);
int             pdf_get_supplement_cid(PDF *p, pdc_font *font);
void            pdf_put_cidglyph_widths(PDF *p, pdc_font *font);
const char*     pdf_get_abb_cjk_fontname(const char *fontname);



/* p_font.c */
void pdf_get_page_fonts(PDF *p, pdf_reslist *rl);
void pdf_init_font_options(PDF *p, pdf_font_options *fo);
void pdf_cleanup_font(PDF *p, pdc_font *font);
void pdf_cleanup_font_options(PDF *p);
void pdf_parse_font_options(PDF *p, const char *optlist);
double pdf_get_font_float_option(PDF *p, pdf_font_optflags fflags);
pdc_bool pdf_make_fontflag(PDF *p, pdc_font *font);
void pdf_font_issemantic(PDF *p, pdc_font *font);
const char *pdf_get_encoding_name(PDF *p, pdc_encoding enc, pdc_font *font);
const char *pdf_get_font_char_option(PDF *p, pdf_font_optflags fflags);
const char *pdf_get_pdf_fontname(pdc_font *font);
void pdf_transform_fontwidths(PDF *p, pdc_font *font,
        pdc_encodingvector *evto, pdc_encodingvector *evfrom);


/* p_type1.c */

pdc_bool pdf_t1open_fontfile(PDF *p, pdc_font *font, const char *fontname,
                             PDF_data_source *t1src);
pdc_bool pdf_make_t1src(PDF *p, pdc_font *font, PDF_data_source *t1src);
void     pdf_put_length_objs(PDF *p, PDF_data_source *t1src,
		     pdc_id length1_id, pdc_id length2_id, pdc_id length3_id);

/* p_type3.c */
void    pdf_init_type3(PDF *p);
int     pdf_handle_t3font(PDF *p, const char *fontname, pdc_encoding enc,
                          int oldslot);

#endif  /* P_FONT_H */
