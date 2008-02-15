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

/* $Id: p_font.c,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * PDFlib font handling routines
 *
 */

#define P_FONT_C

#include "p_intern.h"
#include "p_color.h"
#include "p_defopt.h"
#include "p_font.h"
#include "p_truetype.h"


typedef enum
{
    font_afm  = 1,
    font_pfm  = 2,
    font_ttot = 3,
    font_pfab = 4
}
pdf_fontfile_type;

static const pdc_keyconn pdf_extension_names[] =
{
    {".tte", font_ttot},
    {".ttf", font_ttot},
    {".otf", font_ttot},
    {".afm", font_afm},
    {".pfm", font_pfm},
    {".ttc", font_ttot},
    {".TTE", font_ttot},
    {".TTF", font_ttot},
    {".OTF", font_ttot},
    {".AFM", font_afm},
    {".PFM", font_pfm},
    {".TTC", font_ttot},
    {".pfa", font_pfab},
    {".pfb", font_pfab},
    {".PFA", font_pfab},
    {".PFB", font_pfab},
    {NULL, 0}
};

static const pdc_keyconn pdf_fontoption_keylist[] =
{
    {"fontname",       fo_fontname},
    {"encoding",       fo_encoding},
    {"fontstyle",      fo_fontstyle},
    {"monospace",      fo_monospace},
    {NULL, 0}
};

const char *
pdf_get_font_char_option(PDF *p, pdf_font_optflags fflags)
{
    pdf_text_options *to = p->curr_ppt->currto;
    pdc_font *currfont;

    if (p->fonts_number == 0 || to->font == -1)
        pdc_error(p->pdc, PDF_E_TEXT_NOFONT_PAR,
                  pdc_get_keyword(fflags, pdf_fontoption_keylist), 0, 0, 0);
    currfont = &p->fonts[to->font];

    switch (fflags)
    {
        case fo_fontname:
        return (const char *) currfont->name;

        case fo_encoding:
        return pdf_get_encoding_name(p, currfont->encoding, currfont);

        case fo_fontstyle:
        return pdc_get_keyword(currfont->style, pdf_fontstyle_pdfkeylist);

        default:
        return NULL;
    }
}

double
pdf_get_font_float_option(PDF *p, pdf_font_optflags fflags)
{
    pdf_text_options *to = p->curr_ppt->currto;
    pdc_font *currfont;

    if (p->fonts_number == 0 || to->font == -1)
        pdc_error(p->pdc, PDF_E_TEXT_NOFONT_PAR,
                  pdc_get_keyword(fflags, pdf_fontoption_keylist), 0, 0, 0);
    currfont = &p->fonts[to->font];

    switch (fflags)
    {
        case fo_monospace:
        return (double) currfont->monospace;

        default:
        return 0;
    }
}

void
pdf_cleanup_font(PDF *p, pdc_font *font)
{
    if (font->imgname)
        pdf_unlock_pvf(p, font->imgname);
    pdc_cleanup_font_struct(p->pdc, font);
}

void
pdf_cleanup_fonts(PDF *p)
{
    int slot;

    if (p->fonts)
    {
        for (slot = 0; slot < p->fonts_number; slot++)
            pdf_cleanup_font(p, &p->fonts[slot]);

        if (p->fonts)
            pdc_free(p->pdc, p->fonts);
        p->fonts = NULL;
    }
}

void
pdf_init_fonts(PDF *p)
{
    static const char fn[] = "pdf_init_fonts";

    p->fonts_number     = 0;
    p->fonts_capacity   = FONTS_CHUNKSIZE;

    p->fonts = (pdc_font *) pdc_calloc(p->pdc,
                sizeof(pdc_font) * p->fonts_capacity, fn);

    p->t3font = (pdc_t3font *) NULL;

    pdf_init_encoding_ids(p);
}

void
pdf_grow_fonts(PDF *p)
{
    static const char fn[] = "pdf_grow_fonts";

    p->fonts = (pdc_font *) pdc_realloc(p->pdc, p->fonts,
                sizeof(pdc_font) * 2 * p->fonts_capacity, fn);

    p->fonts_capacity *= 2;
}

int
pdf_init_newfont(PDF *p)
{
    pdc_font *font;
    int slot;

    /* slot for font struct */
    slot = p->fonts_number;
    if (slot >= p->fonts_capacity)
        pdf_grow_fonts(p);

    /* initialize font struct */
    font = &p->fonts[slot];
    pdc_init_font_struct(p->pdc, font);

    return slot;
}

static pdc_bool
pdf_get_metrics_core(PDF *p, pdc_font *font, const char *fontname,
                     pdc_encoding enc)
{
    const pdc_core_metric *metric;

    metric = pdc_get_core_metric(fontname);
    if (metric != NULL)
    {
        pdc_trace_protocol(p->pdc, 1, trc_font,
            "\tLoading metric data for core font \"%s\":\n", fontname);

        /* Fill up the font struct */
        pdc_fill_font_metric(p->pdc, font, metric);
        font->encoding = enc;

        /* Process metric data */
        if (pdf_process_metrics_data(p, font, fontname))
        {
            if (!pdf_make_fontflag(p, font))
                return pdc_false;

            if (font->monospace)
            {
                pdc_set_errmsg(p->pdc, PDC_E_OPT_IGNORED, "monospace", 0, 0, 0);
                if (font->verbose == pdc_true)
                {
                    pdf_cleanup_font(p, font);
                    pdc_error(p->pdc, -1, 0, 0, 0, 0);
                }
                return pdc_false;
            }
            return pdc_true;
        }
    }
    return pdc_undef;
}

pdc_bool
pdf_make_fontflag(PDF *p, pdc_font *font)
{
    (void) p;

    if (font->type != pdc_Type3)
    {
        if (font->isFixedPitch)
            font->flags |= FIXEDWIDTH;

        if (font->isstdlatin == pdc_true ||
            font->encoding == pdc_winansi ||
            font->encoding == pdc_macroman ||
            font->encoding == pdc_ebcdic ||
            font->encoding == pdc_ebcdic_37)
            font->flags |= ADOBESTANDARD;
        else
            font->flags |= SYMBOL;

        if (font->italicAngle < 0 ||
            font->style == pdc_Italic || font->style == pdc_BoldItalic)
            font->flags |= ITALIC;
        if (font->italicAngle == 0 && font->flags & ITALIC)
            font->italicAngle = PDC_DEF_ITALICANGLE;

        /* heuristic to identify (small) caps fonts */
        if (font->name &&
            (strstr(font->name, "Caps") ||
            !strcmp(font->name + strlen(font->name) - 2, "SC")))
            font->flags |= SMALLCAPS;

        if (font->style == pdc_Bold || font->style == pdc_BoldItalic)
            font->StdVW = PDF_STEMV_BOLD;

        if (strstr(font->name, "Bold") || font->StdVW > PDF_STEMV_SEMIBOLD)
            font->flags |= FORCEBOLD;
    }

    if (font->style != pdc_Normal &&
        (font->embedding || font->type == pdc_Type1 ||
         font->type == pdc_MMType1 || font->type == pdc_Type3))
    {
        pdc_set_errmsg(p->pdc, PDF_E_FONT_EMBEDSTYLE, 0, 0, 0, 0);
        if (font->verbose == pdc_true)
        {
            pdf_cleanup_font(p, font);
            pdc_error(p->pdc, -1, 0, 0, 0, 0);
        }
        return pdc_false;
    }

    pdc_font_trace_protocol(p->pdc, font);

    return pdc_true;
}

void
pdf_font_issemantic(PDF *p, pdc_font *font)
{
    pdc_encoding enc = font->encoding;
    pdc_ushort spacechar = 0;

    /* Flag: encoding is Unicode interpretable */
    if (enc >= 0 || enc == pdc_unicode ||
        (enc == pdc_cid && font->codeSize == 2))
        font->issemantic = pdc_true ;

    /* Flag: encoding with ASCII space for wordspacing */
    if (enc >= 0)
    {
        int i = pdc_get_encoding_bytecode(p->pdc,
                                          pdf_get_encoding_vector(p, enc),
                                          PDC_UNICODE_SPACE);
        if (i > -1)
        {
            spacechar = (pdc_ushort) i;
            if (spacechar == PDC_UNICODE_SPACE)
                font->asciispace = pdc_true;
        }
    }

    /* determine code of space character */
    font->spacechar = 0;
    switch(enc)
    {
        case pdc_cid:
        if (font->codeSize == 2)
            font->spacechar = PDC_UNICODE_SPACE;
        break;

        case pdc_unicode:
        font->spacechar = PDC_UNICODE_SPACE;
        break;

        case pdc_glyphid:
        font->spacechar = font->code2GID[PDC_UNICODE_SPACE];
        break;

        default:
        font->spacechar = spacechar;
        break;
    }
}

/* definitions of font options */
static const pdc_defopt pdf_load_font_options[] =
{
    PDF_FONT_OPTIONS2
    PDF_FONT_OPTIONS3
    PDC_OPT_TERMINATE
};

void
pdf_init_font_options(PDF *p, pdf_font_options *fo)
{
    static const char fn[] = "pdf_init_font_options";

    if (fo == NULL)
    {
        p->currfo = (pdf_font_options *) pdc_malloc(p->pdc,
                        sizeof(pdf_font_options), fn);


        fo = p->currfo;
    }
    else
    {
    }

    fo->embedding = pdc_false;
    fo->encoding = NULL;
    fo->flags = 0;
    fo->fontname = NULL;
    fo->fontstyle = pdc_Normal;
    fo->fontwarning = p->debug[(int) 'F'];
    fo->kerning = pdc_false;
    fo->mask = 0;
    fo->monospace = 0;
    fo->auxiliary = pdc_false;
}

void
pdf_cleanup_font_options(PDF *p)
{
    if (p->currfo)
    {
        pdc_free(p->pdc, p->currfo);
        p->currfo = NULL;
    }
}

void
pdf_parse_font_options(PDF *p, const char *optlist)
{
    pdc_resopt *resopts = pdc_parse_optionlist(p->pdc, optlist,
			   pdf_load_font_options, NULL, p->debug[(int) 'F']);
    if (resopts)
    {
        pdf_get_font_options(p, p->currfo, resopts);
        pdc_cleanup_optionlist(p->pdc, resopts);
    }
}

void
pdf_get_font_options(PDF *p, pdf_font_options *fo, pdc_resopt *resopts)
{
    int inum;

    (void) p;

    if (fo->flags & is_block || fo->flags & is_textflow)
    {
        fo->fontname = pdf_get_opt_utf8name(p, "fontname", resopts);
        if (fo->fontname)
        {
            fo->mask |= (1L << fo_fontname);
        }

        if (pdc_get_optvalues("encoding", resopts, NULL, NULL))
        {
            fo->encoding = (char *)pdc_save_lastopt(resopts, PDC_OPT_SAVE1ELEM);
            fo->mask |= (1L << fo_encoding);
        }
    }

    if (pdc_get_optvalues("fontwarning", resopts, &fo->fontwarning, NULL))
        fo->mask |= (1L << fo_fontwarning);

    if (pdc_get_optvalues("embedding", resopts, &fo->embedding, NULL))
        fo->mask |= (1L << fo_embedding);


    if (pdc_get_optvalues("fontstyle", resopts, &inum, NULL))
    {
        fo->fontstyle = (pdc_fontstyle) inum;
        fo->mask |= (1L << fo_fontstyle);
    }

    if (pdc_get_optvalues("monospace", resopts, &fo->monospace, NULL))
        fo->mask |= (1L << fo_monospace);
}

int
pdf__load_font(PDF *p, const char *fontname, int inlen,
               const char *encoding, const char *optlist)
{
    int slot;
    char *fname;
    pdf_font_options fo;

    if (encoding == NULL || *encoding == '\0')
        pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "encoding", 0, 0, 0);

    if (fontname == NULL)
        pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "fontname", 0, 0, 0);

    /* Converting fontname */
    fname = pdf_convert_name(p, fontname, inlen, pdc_true);
    if (fname == NULL || *fname == '\0')
        pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "fontname", 0, 0, 0);

    /* initialize */
    pdf_init_font_options(p, &fo);
    fo.fontname = (char *) fname;
    fo.encoding = (char *) encoding;

    /* parsing option list */
    if (optlist && strlen(optlist))
    {
        pdc_resopt *resopts = pdc_parse_optionlist(p->pdc, optlist,
			   pdf_load_font_options, NULL, p->debug[(int) 'F']);
        if (!resopts)
            return -1;
        pdf_get_font_options(p, &fo, resopts);
        pdc_cleanup_optionlist(p->pdc, resopts);
    }

    slot = pdf_load_font_internal(p, &fo);
    pdc_free(p->pdc, fname);
    return slot;
}

int
pdf_load_font_internal(PDF *p, pdf_font_options *fo)
{
    static const char fn[] = "pdf_load_font_internal";
    const char *fontname;
    const char *encoding;
    pdc_encoding enc = pdc_invalidenc, enc_old;
    pdc_bool ksearch;
    pdc_bool verbose;
    pdc_bool kret;
    int slot = -1;
    size_t len;
    int i;
    int retval;
    pdc_font *font;
    const char *extension = NULL;
    const char *outfilename = NULL;
    char *fontname_p = NULL;
    char *filename = NULL, testfilename[PDF_MAX_FONTNAME + 5];
    char *mmparam, mastername[PDF_MAX_FONTNAME + 1];

    /* UTF-8 font name without BOM */
    fontname = pdc_utf8strprint(p->pdc, fo->fontname);

    /* font encoding */
    encoding = fo->encoding;

    /* slot for new font struct */
    slot = pdf_init_newfont(p);
    font = &p->fonts[slot];

    /* copy options */
    font->embedding = fo->embedding;
    font->style = fo->fontstyle;
    font->verbose = fo->fontwarning;
    font->verbose_open = fo->fontwarning;
    font->kerning = fo->kerning;
    font->monospace = fo->monospace;
    verbose = font->verbose;

    slot = -1;
    kret = pdc_false;

    /* search for a registered encoding */
    enc = pdf_find_encoding(p, encoding);
    if (enc == pdc_invalidenc)
    {
        /* search for a predefined CMap and registered fonts */
        enc = pdf_handle_cidfont(p, fontname, encoding, &slot);
        if (enc == pdc_invalidenc)
        {
            /* search for a new encoding */
            enc = pdf_insert_encoding(p, encoding, &font->codepage,
                                      font->verbose);
            if (enc == pdc_invalidenc)
                kret = pdc_true;
        }
        else if (enc == pdc_cid && slot != -1)
        {
            kret = pdc_true;
            if (slot < 0)
            {
                slot = -1;
                pdf_cleanup_font(p, font);
                if (verbose)
                    pdc_error(p->pdc, -1, 0, 0, 0, 0);
            }
        }
    }
    pdc_trace_protocol(p->pdc, 1, trc_encoding, "\tFont encoding: \"%s\"\n",
                       pdf_get_user_encoding(p, enc));
    enc_old = enc;
    if (kret)
        return slot;

    if (enc == pdc_unicode || enc == pdc_glyphid)
    {
        pdf_cleanup_font(p, font);
        pdc_error(p->pdc, PDF_E_UNSUPP_UNICODE, 0, 0, 0, 0);
    }

    /*
     * Look whether font is already in the cache.
     * Look first for the auxiliary font (obj_id == -1).
     * If a font with same encoding and same relevant options is found,
     * return its descriptor.
     * If a Type 3 font with the same name but different encoding
     * is found, make a copy in a new slot and attach the requested encoding.
     */

    for (slot = 0; slot < p->fonts_number; slot++)
    {
        if (p->fonts[slot].obj_id == PDC_BAD_ID)
        {
            if (fo->auxiliary)
                return slot;
        }
        else if (!fo->auxiliary &&
                 !strcmp(p->fonts[slot].apiname, fontname) &&
                 p->fonts[slot].style == font->style)
        {
            if (p->fonts[slot].type == pdc_Type3)
            {
                if (enc < 0 )
                {
		    pdc_set_errmsg(p->pdc, PDF_E_FONT_BADENC,
			p->fonts[slot].name,
			pdf_get_encoding_name(p, enc, font), 0, 0);

                    pdf_cleanup_font(p, font);
                    if (verbose == pdc_true)
                        pdc_error(p->pdc, -1, 0, 0, 0, 0);

                    return -1;
                }
                if (p->fonts[slot].encoding != enc)
                    slot = pdf_handle_t3font(p, fontname, enc, slot);

                return slot;
            }
            else if (p->fonts[slot].monospace == font->monospace)
            {
                if (p->fonts[slot].encoding == enc &&
                    p->fonts[slot].codepage == font->codepage)
                {
                    return slot;
                }
                else if (p->fonts[slot].encoding >= 0)
                {
                    char *encname, *adaptname;
                    int kc;

                    /* Comparing apiname of encoding */
                    if (!strcmp(encoding, p->fonts[slot].encapiname))
                        return slot;

                    /* Name of adapted to font encoding */
                    encname = (char *) pdf_get_encoding_name(p, enc, font);
                    len = strlen(encname) + 1 + strlen(fontname) + 1;
                    adaptname = (char *) pdc_malloc_tmp(p->pdc, len, fn, 0, 0);
                    strcpy(adaptname, encname);
                    strcat(adaptname, PDC_ENC_MODSEPAR);
                    strcat(adaptname, fontname);
                    kc = strcmp(adaptname, pdf_get_encoding_name(p,
                                p->fonts[slot].encoding, &p->fonts[slot]));
                    pdc_free_tmp(p->pdc, adaptname);
                    if (!kc)
                        return slot;
                }
            }
        }
    }


    /* Multiple Master handling:
     * - strip MM parameters to build the master name
     * - the master name is used to find the metrics
     * - the instance name (client-supplied font name) is used in all places
     * - although the master name is used for finding the metrics, the
     *   instance name is stored in the font struct.
     */

    len = strlen(fontname);
    if (len > PDF_MAX_FONTNAME)
    {
	pdc_set_errmsg(p->pdc, PDC_E_ILLARG_TOOLONG, "fontname",
	    pdc_errprintf(p->pdc, "%d", PDF_MAX_FONTNAME), 0, 0);

        pdf_cleanup_font(p, font);
        if (verbose)
            pdc_error(p->pdc, -1, 0, 0, 0, 0);

        return -1;
    }
    strcpy(mastername, fontname);

    /* A Multiple Master font was requested */
    if ((mmparam = strstr(mastername, "MM_")) != NULL)
    {
        if (font->embedding)
        {
	    pdc_set_errmsg(p->pdc, PDF_E_FONT_EMBEDMM, fontname, 0, 0, 0);

            pdf_cleanup_font(p, font);
            if (verbose)
                pdc_error(p->pdc, -1, 0, 0, 0, 0);

            return -1;
        }
        mmparam[2] = '\0';      /* strip the parameter from the master name */
    }

    /* Font with vertical writing mode */
    fontname_p = mastername;
    if (mastername[0] == '@')
    {
        font->vertical = pdc_true;
        fontname_p = &mastername[1];
    }

    /* protocol */
    pdc_trace_protocol(p->pdc, 1, trc_font,
        "\n\tPDFlib fontname: \"%s\"\n", fontname_p);

    /* API font name */
    font->apiname = pdc_strdup(p->pdc, fontname);

    /* UTF-8 font name with BOM */
    font->utf8name = pdc_strdup(p->pdc, fo->fontname);

    /* specified encoding name */
    font->encapiname = pdc_strdup(p->pdc, encoding);

    /* Font file search hierarchy
     * - Check "FontOutline" resource entry and check TrueType font
     * - Check "FontAFM" resource entry
     * - Check "FontPFM" resource entry
     * - Check "HostFont" resource entry
     * - Check available in-core metrics
     * - Check host font
     */
    retval = pdc_false;
    ksearch = pdc_true;
    while (ksearch)
    {
#ifdef PDF_TRUETYPE_SUPPORTED
        /* Check specified TrueType file */
        filename = pdf_find_resource(p, "FontOutline", fontname_p);
        if (!filename)
        {
            /* check for TTC font names with index */
            char ittc = ':';
            char *sf = strrchr(fontname_p, ittc);

            if (sf != NULL)
            {
                *sf = 0;
                filename = pdf_find_resource(p, "FontOutline", fontname_p);
                *sf = ittc;
            }
        }
        if (filename)
        {
            outfilename = filename;
            retval = pdf_check_tt_font(p, filename, fontname_p, font);
            if (retval == pdc_undef)
            {
                retval = pdc_false;
                break;
            }
            if (retval == pdc_true)
            {
                retval = pdf_get_metrics_tt(p, font, fontname_p, enc, filename);
                break;
            }
        }
#endif /* PDF_TRUETYPE_SUPPORTED */

        /* Check specified AFM file */
        filename = pdf_find_resource(p, "FontAFM", fontname_p);
        if (filename)
        {
            retval = pdf_get_metrics_afm(p, font, fontname_p, enc, filename);
            if (retval == pdc_undef)
                retval = pdc_false;
            break;
        }

        /* Check specified PFM file */
        filename = pdf_find_resource(p, "FontPFM", fontname_p);
        if (filename)
        {
            retval = pdf_get_metrics_pfm(p, font, fontname_p, enc, filename);
            if (retval == pdc_undef)
                retval = pdc_false;
            break;
        }



        /* Check available in-core metrics */
        retval = pdf_get_metrics_core(p, font, fontname_p, enc);
        if (retval != pdc_undef)
            break;
        retval = pdc_false;


        /* tSearching for a metric file */
        pdc_trace_protocol(p->pdc, 1, trc_font,
            "\tSearching for font metric data file:\n");

        font->verbose_open = pdc_false;
        filename = testfilename;
        for (i = 0; i < 100; i++)
        {
            extension = pdf_extension_names[i].word;
            if (!extension) break;
            strcpy(testfilename, fontname_p);
            strcat(testfilename, extension);

            switch (pdf_extension_names[i].code)
            {
                case font_ttot:
                retval = pdf_check_tt_font(p, filename, fontname_p, font);
                if (retval == pdc_true)
                    retval = pdf_get_metrics_tt(p, font, fontname_p, enc,
                                                filename);
                break;

                case font_afm:
                retval = pdf_get_metrics_afm(p, font, fontname_p, enc,
                                             filename);
                break;

                case font_pfm:
                retval = pdf_get_metrics_pfm(p, font, fontname_p, enc,
                                             filename);
                break;

                default:
                break;
            }
            if (retval != pdc_undef)
            {
                if (pdf_extension_names[i].code == font_ttot)
                    outfilename = filename;
                break;
            }
        }

        if (retval == pdc_undef)
	{
            pdc_set_errmsg(p->pdc, PDF_E_FONT_NOMETRICS, fontname, 0, 0, 0);

            pdc_trace_protocol(p->pdc, 1, trc_font,
                "\tMetric data file for font \"%s\" not found\n", fontname);

	    if (verbose)
            {
                pdf_cleanup_font(p, font);
		pdc_error(p->pdc, -1, 0, 0, 0, 0);
            }
	}
        ksearch = pdc_false;
    }

    if (retval == pdc_false)
    {
        pdf_cleanup_font(p, font);
        return -1;
    }

    /* store instance name instead of master name in the font structure */
    if (mmparam)
    {
        pdc_free(p->pdc, font->name);
        font->name = pdc_strdup(p->pdc, fontname);
    }

    /* If embedding was requested, check font file (or raise an exception) */
    font->verbose_open = font->verbose;
    if (font->embedding)
    {
        if (font->img == NULL)
        {
            char fullname[PDC_FILENAMELEN];
            pdc_file *fp = NULL;
            retval = pdc_false;

            if (outfilename)
            {
                /* Font outline file specified */
                if (font->type == pdc_Type1 || font->type == pdc_MMType1)
                {
                    retval = pdf_t1open_fontfile(p, font, outfilename, NULL);
                }
                else
                {
                    fp = pdf_fopen_name(p, outfilename, fullname, "font ", 0);
                    if (fp != NULL)
                        retval = pdc_true;
                }
            }
            else
            {
                /* Searching font outline file */
                pdc_trace_protocol(p->pdc, 1, trc_font,
                    "\tSearching for font outline data file:\n");

                outfilename = testfilename;
                for (i = 0; i < 100; i++)
                {
                    extension = pdf_extension_names[i].word;
                    if (!extension) break;

                    strcpy(testfilename, fontname_p);
                    strcat(testfilename, extension);

                    if (font->type == pdc_Type1 || font->type == pdc_MMType1)
                    {
                        if (pdf_extension_names[i].code == font_pfab)
                        {
                            retval = pdf_t1open_fontfile(p, font, outfilename,
                                                         NULL);
                        }
                    }
                    else if (pdf_extension_names[i].code == font_ttot)
                    {
                        fp = pdf_fopen_name(p, outfilename, fullname, NULL, 0);
                        if (fp != NULL)
                            retval = pdc_true;
                    }
                    if (retval == pdc_true)
                        break;
                }
                if (retval == pdc_false)
		{
		    pdc_set_errmsg(p->pdc, PDF_E_FONT_NOOUTLINE, fontname,
			           0, 0, 0);
                }
            }

            if (retval == pdc_true && fp != NULL)
            {
                if (pdc_file_isvirtual(fp) == pdc_true)
                {
                    font->imgname = pdc_strdup(p->pdc, outfilename);
                    pdf_lock_pvf(p, font->imgname);
                }
                else
                    font->fontfilename = pdc_strdup(p->pdc, fullname);
                pdc_fclose(fp);
            }
            if (retval == pdc_false)
            {
                pdc_trace_protocol(p->pdc, 1, trc_font,
                    "\tOutline data file for font \"%s\" not found\n",
                    fontname);

                if (verbose)
                {
                    pdf_cleanup_font(p, font);
                    pdc_error(p->pdc, -1, 0, 0, 0, 0);
                }
            }
            else
                pdc_trace_protocol(p->pdc, 1, trc_font,
                    "\tFont outline data file \"%s\" available\n",
                    font->fontfilename ? font->fontfilename : font->imgname);
        }
    }
    else if (font->img)
    {
        if (!font->imgname)
            pdc_free(p->pdc, font->img);
        else
        {
            pdf_unlock_pvf(p, font->imgname);
            pdc_free(p->pdc, font->imgname);
            font->imgname = NULL;
        }
        font->img = NULL;
        font->filelen = 0;
    }

    if (retval && font->monospace && font->embedding)
    {
        pdc_set_errmsg(p->pdc, PDC_E_OPT_IGNORED, "monospace", 0, 0, 0);
        if (font->verbose == pdc_true)
        {
            pdf_cleanup_font(p, font);
            pdc_error(p->pdc, -1, 0, 0, 0, 0);
        }
        retval = pdc_false;
    }

    if (retval == pdc_false)
    {
        pdf_cleanup_font(p, font);
        return -1;
    }

    if (font->encoding != enc_old)
        pdc_trace_protocol(p->pdc, 1, trc_encoding,
                           "\tDetermined font encoding: \"%s\"\n",
                           pdf_get_user_encoding(p, font->encoding));

    /* Now everything is fine; fill the remaining font cache entries */

    p->fonts_number++;

    font->verbose_open = pdc_true;

    if (enc >= 0)
        p->encodings[enc].ev->flags |= PDC_ENC_USED;

    pdf_font_issemantic(p, font);

    if (!fo->auxiliary)
        font->obj_id = pdc_alloc_id(p->out);

    return slot;
}

int
pdf__get_glyphid(PDF *p, int font, int code)
{
    int gid = 0;

    if (font < 0 || font >= p->fonts_number)
        pdc_error(p->pdc, PDC_E_ILLARG_INT,
            "font", pdc_errprintf(p->pdc, "%d", font), 0, 0);

    if (code < 0 || code >= p->fonts[font].numOfCodes)
        pdc_error(p->pdc, PDC_E_ILLARG_INT,
            "code", pdc_errprintf(p->pdc, "%d", code), 0, 0);

    if (p->fonts[font].code2GID)
        gid = p->fonts[font].code2GID[code];
    else
        pdc_error(p->pdc, PDF_E_FONT_NOGLYPHID, p->fonts[font].apiname, 0, 0,0);

    return gid;
}

const char *
pdf_get_pdf_fontname(pdc_font *font)
{
    const char *name;

    name = pdc_get_abb_std_fontname(font->name);
    if (!name)
    {
        name = pdf_get_abb_cjk_fontname(font->name);
    }
    if (!name)
    {
        switch (font->type)
        {
            case pdc_TrueType:
            case pdc_Type1C:
            case pdc_CIDFontType2:
            case pdc_CIDFontType0:
            name = font->ttname;
            break;

            default:
            name = font->name;
            break;
        }
    }
    return (const char *) name;
}

const char *
pdf_get_encoding_name(PDF *p, pdc_encoding enc, pdc_font *font)
{
    const char *apiname = pdc_get_fixed_encoding_name(enc);
    if (!apiname[0] && enc >= 0)
        apiname = (const char *) p->encodings[enc].ev->apiname;
    else if (enc == pdc_cid && font != NULL && font->cmapname != NULL)
        apiname = pdc_errprintf(p->pdc, "%.*s",
                                PDC_ET_MAXSTRLEN, font->cmapname);
    return apiname;
}

void
pdf_transform_fontwidths(PDF *p, pdc_font *font, pdc_encodingvector *evto,
                         pdc_encodingvector *evfrom)
{
    int widths[256];
    int i, j;

    for (i = 0; i < 256; i++)
        widths[i] = font->defWidth;
    for (i = 0; i < 256; i++)
    {
        j = (int) pdc_transform_bytecode(p->pdc, evto, evfrom, (pdc_byte)i);
        widths[j] = font->widths[i];
    }
    widths[0] = font->defWidth;
    memcpy(font->widths, widths, 256 * sizeof(int));
}



static void
pdf_write_fontdescriptor(
    PDF *p,
    pdc_font *font,
    int missingwidth,
    pdc_id fontdescriptor_id,
    pdc_id fontfile_id)
{
    /*
     * Font descriptor object
     */
    pdc_begin_obj(p->out, fontdescriptor_id);   /* font descriptor obj */
    pdc_begin_dict(p->out);                     /* font descriptor dict */

    pdc_puts(p->out, "/Type/FontDescriptor\n");
    pdc_printf(p->out, "/Ascent %d\n", font->ascender);
    pdc_printf(p->out, "/CapHeight %d\n", font->capHeight);
    pdc_printf(p->out, "/Descent %d\n", font->descender);
    pdc_printf(p->out, "/Flags %ld\n", font->flags);
    pdc_printf(p->out, "/FontBBox[%d %d %d %d]\n",
        (int) font->llx, (int) font->lly, (int) font->urx, (int) font->ury);

    pdc_printf(p->out, "/FontName");
    pdf_put_pdfname(p, font->name);
    pdc_puts(p->out, "\n");

    pdc_printf(p->out, "/ItalicAngle %d\n", (int) (font->italicAngle));
    pdc_printf(p->out, "/StemV %d\n", font->StdVW);

    if (font->StdHW > 0)
        pdc_printf(p->out, "/StemH %d\n", font->StdHW);

    if (font->xHeight > 0)
        pdc_printf(p->out, "/XHeight %d\n", font->xHeight);

    if (missingwidth > 0)
        pdc_printf(p->out, "/MissingWidth %d\n", missingwidth);

    if (fontfile_id != PDC_BAD_ID)
    {
        switch(font->type)
        {
            case pdc_Type1:
            case pdc_MMType1:
	    pdc_objref(p->out, "/FontFile", fontfile_id);
            break;

#ifdef PDF_TRUETYPE_SUPPORTED
            case pdc_TrueType:
            case pdc_CIDFontType2:
	    pdc_objref(p->out, "/FontFile2", fontfile_id);
            break;

            case pdc_Type1C:
            case pdc_CIDFontType0:
	    pdc_objref(p->out, "/FontFile3", fontfile_id);
            break;
#endif /* PDF_TRUETYPE_SUPPORTED */

            default:
            break;
        }
    }

    pdc_end_dict(p->out);                       /* font descriptor dict */
    pdc_end_obj(p->out);                        /* font descriptor obj */
}

static void
pdf_put_font(PDF *p, pdc_font *font)
{
    const char        *fontname = font->name;
    pdc_id             fontdescriptor_id = PDC_BAD_ID;
    pdc_id             fontfile_id = PDC_BAD_ID;
    pdc_id             encoding_id = PDC_BAD_ID;
    pdc_id             descendant_id = PDC_BAD_ID;
    pdc_encoding       enc = font->encoding;
    pdc_bool           standard_font = pdc_false;
    pdc_bool           comp_font = pdc_false;
    pdc_scalar         a = 1.0;
    PDF_data_source    src;

    /* save font struct members */
    pdc_encoding       font_encoding = font->encoding;
    int                font_numOfCodes = font->numOfCodes;
    int                font_codeSize = font->codeSize;

    int                missingwidth = 0;
    int                i;


    /*
     * This font has been defined, but never used in the document. Ignore it.
     * However, the font's object id has already been allocated,
     * so we mark the object as free in order to avoid a complaint
     * of the object machinery.
     */
    if (enc == pdc_invalidenc || font->used_in_current_doc == pdc_false)
    {
        pdc_mark_free(p->out, font->obj_id);
        return;
    }


    /* ID for embedded font */
    if (font->embedding)
    {
        switch(font->type)
        {
            case pdc_Type1:
            case pdc_MMType1:
            case pdc_TrueType:
            case pdc_CIDFontType2:
            case pdc_Type1C:
            case pdc_CIDFontType0:
            fontfile_id = pdc_alloc_id(p->out);
            break;

            default:
            break;
        }
    }
    else if (font->type == pdc_Type1)
    {
        /* check whether we have one of the 14 standard fonts */
        standard_font = pdc_is_standard_font(fontname);
    }

    /*
     * Font dictionary
     */
    pdc_begin_obj(p->out, font->obj_id);                /* font obj */
    pdc_begin_dict(p->out);                             /* font dict */
    pdc_puts(p->out, "/Type/Font\n");

    /* /Subtype */
    switch (font->type)
    {
        case pdc_Type1:
        pdc_puts(p->out, "/Subtype/Type1\n");
        break;

        case pdc_MMType1:
        pdc_puts(p->out, "/Subtype/MMType1\n");
        break;

        case pdc_TrueType:
        pdc_puts(p->out, "/Subtype/TrueType\n");
        fontname = font->ttname;
        break;

        case pdc_Type1C:
        fontname = font->ttname;
        pdc_puts(p->out, "/Subtype/Type1\n");
        break;

        case pdc_CIDFontType2:
        case pdc_CIDFontType0:
        fontname = font->ttname;
        pdc_puts(p->out, "/Subtype/Type0\n");
        comp_font = pdc_true;
        break;

        case pdc_Type3:
        pdc_puts(p->out, "/Subtype/Type3\n");
        break;

        default:
        break;
    }

    /* /Name */
    if (font->type == pdc_Type3 || font->used_in_formfield)
    {
        /*
         * The name is optional, but if we include it it will show up
         * in Acrobat's font info box. However, if the same font name
         * is used with different encodings Acrobat 4 will not be
         * able to distinguish both. For this reason we add the
         * encoding name to make the font name unique.
         */

        const char *name = fontname;

        if (font->used_in_formfield)
            name = pdf_get_pdf_fontname(font);

        pdc_puts(p->out, "/Name");
        pdf_put_pdfname(p, name);
        pdc_puts(p->out, "\n");
    }

    /* /BaseFont */
    switch (font->type)
    {
        case pdc_Type1:
        case pdc_MMType1:
        case pdc_TrueType:
        case pdc_Type1C:
        case pdc_CIDFontType2:
        case pdc_CIDFontType0:
        {
            pdc_puts(p->out, "/BaseFont");
            pdf_put_pdfname(p, fontname);
            if (font->cmapname)
                pdc_printf(p->out, "-%s", font->cmapname);
            if (font->style != pdc_Normal && !comp_font)
                pdc_printf(p->out, ",%s",
                    pdc_get_keyword(font->style, pdf_fontstyle_pdfkeylist));
            pdc_puts(p->out, "\n");
        }
        break;

        /* /FontBBox, /FontMatrix, /CharProcs /Resources */
        case pdc_Type3:
        pdc_printf(p->out, "/FontBBox[%f %f %f %f]\n",
            font->t3font->bbox.llx, font->t3font->bbox.lly,
            font->t3font->bbox.urx, font->t3font->bbox.ury);

        pdc_printf(p->out, "/FontMatrix[%f %f %f %f %f %f]\n",
            font->t3font->matrix.a, font->t3font->matrix.b,
            font->t3font->matrix.c, font->t3font->matrix.d,
            font->t3font->matrix.e, font->t3font->matrix.f);
        pdc_objref(p->out, "/CharProcs", font->t3font->charprocs_id);
        pdc_objref(p->out, "/Resources", font->t3font->res_id);

        /* We must apply a correctional factor since Type 3 fonts not
         * necessarily use 1000 units per em. We apply the correction
         * here, and store the 1000-based width values in the font in
         * order to speed up PDF_stringwidth().
         */
        a = 1000 * font->t3font->matrix.a;
        break;

        default:
        break;
    }

    /* changing 8-bit font encoding to winansi */
    if (font->towinansi != pdc_invalidenc)
    {
        pdf_transform_fontwidths(p, font,
            pdf_get_encoding_vector(p, font->towinansi),
            pdf_get_encoding_vector(p, enc));

        enc = font->towinansi;
        font->encoding = enc;
    }

    /* /FontDescriptor, /FirstChar, /LastChar, /Widths */
    switch (font->type)
    {
        case pdc_Type1:
        if (standard_font == pdc_true) break;
        case pdc_MMType1:
        case pdc_TrueType:
        case pdc_Type1C:
        {
            fontdescriptor_id = pdc_alloc_id(p->out);
            pdc_objref(p->out, "/FontDescriptor", fontdescriptor_id);
        }
        case pdc_Type3:
        {
            int firstchar = 0;
            int lastchar = 255;
            int defwidth = 0;


            /* determine missing width.
             * Only for embedded fonts because of a bug in Acrobat,
             * which arises if the font is not installed at host.
             */
            if (font->embedding)
            {
                if (font->type != pdc_Type3)
                    defwidth = font->widths[0];
                for (i = 1; i < 255; i++)
                {
                    if (font->widths[i] != defwidth)
                        break;
                }
                if (i > 1)
                    firstchar = i;
                for (i = 255; i > 0; i--)
                {
                    if (i == firstchar || font->widths[i] != defwidth)
                        break;
                }
                lastchar = i;
                if (firstchar > 0 || lastchar < 255)
                    missingwidth = (int) (defwidth / a + 0.5);
            }

            pdc_printf(p->out, "/FirstChar %d\n", firstchar);
            pdc_printf(p->out, "/LastChar %d\n", lastchar);

            pdc_puts(p->out, "/Widths");
	    pdc_begin_array(p->out);
            for (i = firstchar; i <= lastchar; i++)
            {
                pdc_printf(p->out, "%d", (int) (font->widths[i] / a + 0.5));
                if (i < 255)
                    pdc_printf(p->out, "%s", ((i + 1) % 16) ? " " : "\n");
            }
	    pdc_end_array(p->out);
        }
        break;

        default:
        break;
    }

    /* /Encoding */
    switch (font->type)
    {
        case pdc_Type1:
        case pdc_MMType1:
        case pdc_TrueType:
        case pdc_Type1C:
        if (enc == pdc_winansi && !font->used_in_formfield)
        {
            pdc_printf(p->out, "/Encoding/WinAnsiEncoding\n");
            break;
        }
        if (enc == pdc_macroman && font->hasnomac == pdc_false &&
            !font->used_in_formfield)
        {
            pdc_printf(p->out, "/Encoding/MacRomanEncoding\n");
            break;
        }
        case pdc_Type3:
        if (enc >= 0)
        {
            if (p->encodings[enc].id == PDC_BAD_ID)
                p->encodings[enc].id = pdc_alloc_id(p->out);
            encoding_id = p->encodings[enc].id;
        }
        if (encoding_id != PDC_BAD_ID)
            pdc_objref(p->out, "/Encoding", encoding_id);
        break;

        case pdc_CIDFontType2:
        case pdc_CIDFontType0:
        if (font->cmapname)
            pdc_printf(p->out, "/Encoding/%s\n", font->cmapname);
        break;

        default:
        break;
    }


    /* /DescendantFonts */
    if (comp_font == pdc_true)
    {
        descendant_id = pdc_alloc_id(p->out);
        pdc_puts(p->out, "/DescendantFonts");
	pdc_begin_array(p->out);
	pdc_objref(p->out, "", descendant_id);
	pdc_end_array(p->out);
    }

    pdc_end_dict(p->out);                               /* font dict */
    pdc_end_obj(p->out);                                /* font obj */

    /*
     * Encoding dictionary
     */
    if (encoding_id != PDC_BAD_ID)
    {
        char *charname;

        pdc_begin_obj(p->out, encoding_id);             /* encoding obj */
        pdc_begin_dict(p->out);                         /* encoding dict */

        pdc_puts(p->out, "/Type/Encoding\n");

        {
            pdc_encodingvector *ev = pdf_get_encoding_vector(p, enc);
            pdc_encodingvector *evb = NULL;

            pdf_set_encoding_glyphnames(p, enc);

            /* See Implementation Note 46. The restrictions described there
             * are also valid for Acrobat versions up to now.
             */
            if (font->type != pdc_Type3 && !font->used_in_formfield)
            {
                if (!strncmp(ev->apiname, PDC_ENC_MODWINANSI,
                             strlen(PDC_ENC_MODWINANSI)) ||
                    !strncmp(ev->apiname, PDC_ENC_ISO8859,
                             strlen(PDC_ENC_ISO8859)) ||
                    !strncmp(ev->apiname, PDC_ENC_CP125,
                             strlen(PDC_ENC_CP125)))
                {
                    pdc_puts(p->out, "/BaseEncoding/WinAnsiEncoding\n");
                    evb = pdf_get_encoding_vector(p, pdc_winansi);
                }
                else if (!strncmp(ev->apiname, PDC_ENC_MODMACROMAN,
                             strlen(PDC_ENC_MODMACROMAN)))
                {
                    pdc_puts(p->out, "/BaseEncoding/MacRomanEncoding\n");
                    evb = pdf_get_encoding_vector(p, pdc_macroman);
                }
                else
                {
                    /* /BaseEncoding/StandardEncoding */
                    evb = pdf_get_encoding_vector(p, pdc_stdenc);
                }
            }

            if (evb != NULL)
            {
                int iv = -1;
                for (i = 0; i < font->numOfCodes; i++)
                {
                    charname = ev->chars[i];
                    pdc_get_alter_glyphname(ev->codes[i], font->missingglyphs,
                                            &charname);

                    /* enforce first three names because of bug in Acrobat 6 */
                    if (i < 3 ||
                        (charname && !evb->chars[i]) ||
                        (!charname && evb->chars[i]) ||
                        (charname && evb->chars[i] &&
                         strcmp(charname, evb->chars[i])))
                    {
                        if (iv == -1)
                            pdc_puts(p->out, "/Differences[0");
                        if (i > iv + 1)
                            pdc_printf(p->out, "%d", i);
                        pdf_put_pdfname(p, charname);
                        pdc_puts(p->out, "\n");
                        iv = i;
                    }
                }
                if (iv > -1)
		    pdc_end_array(p->out);
            }
            else
            {
                pdc_puts(p->out, "/Differences[0");
                for (i = 0; i < font->numOfCodes; i++)
                {
                    charname = ev->chars[i];
                    pdc_get_alter_glyphname(ev->codes[i], font->missingglyphs,
                                            &charname);
                    pdf_put_pdfname(p, charname);
                    pdc_puts(p->out, "\n");
                }
		pdc_end_array(p->out);
            }
        }

        pdc_end_dict(p->out);                           /* encoding dict */
        pdc_end_obj(p->out);                            /* encoding obj */

        if (p->flush & pdf_flush_content)
            pdc_flush_stream(p->out);
    }


    /*
     * CID fonts dictionary
     */
    if (descendant_id != PDC_BAD_ID)
    {
        pdc_begin_obj(p->out, descendant_id);           /* CID font obj */
        pdc_begin_dict(p->out);                         /* CID font dict */
        pdc_puts(p->out, "/Type/Font\n");

        /* /Subtype */
        if (font->type == pdc_CIDFontType0)
            pdc_puts(p->out, "/Subtype/CIDFontType0\n");
        if (font->type == pdc_CIDFontType2)
            pdc_puts(p->out, "/Subtype/CIDFontType2\n");

        /* /BaseFont */
        pdc_puts(p->out, "/BaseFont");
        pdf_put_pdfname(p, fontname);
        if (font->style != pdc_Normal)
            pdc_printf(p->out, ",%s",
                pdc_get_keyword(font->style, pdf_fontstyle_pdfkeylist));
        pdc_puts(p->out, "\n");

        /* /CIDSystemInfo */
        pdc_puts(p->out, "/CIDSystemInfo<</Registry");
        pdf_put_hypertext(p, "Adobe");
        pdc_puts(p->out, "/Ordering");
        pdf_put_hypertext(p, pdf_get_ordering_cid(p, font));
        pdc_printf(p->out, "/Supplement %d>>\n",
                   pdf_get_supplement_cid(p, font));

        /* /FontDescriptor */
        fontdescriptor_id = pdc_alloc_id(p->out);
        pdc_objref(p->out, "/FontDescriptor", fontdescriptor_id);

        /* /DW /W */
#ifdef PDF_CJKFONTWIDTHS_SUPPORTED
        if (font->standardcjk)
            pdf_put_cidglyph_widths(p, font);
#endif /* PDF_CJKFONTWIDTHS_SUPPORTED */


        pdc_end_dict(p->out);                           /* CID font dict */
        pdc_end_obj(p->out);                            /* CID font obj */
    }

    /*
     * FontDescriptor dictionary
     */
    if (fontdescriptor_id != PDC_BAD_ID)
        pdf_write_fontdescriptor(p, font, missingwidth, fontdescriptor_id,
                                 fontfile_id);

    /*
     * Font embedding
     */
    if (fontfile_id != PDC_BAD_ID)
    {
        pdc_id    length_id = PDC_BAD_ID;
        pdc_id    length1_id = PDC_BAD_ID;
        pdc_id    length2_id = PDC_BAD_ID;
        pdc_id    length3_id = PDC_BAD_ID;
        pdc_bool  compress = pdc_false;

        /* Prepare embedding */
        switch(font->type)
        {
            case pdc_Type1:
            case pdc_MMType1:
            {
                pdf_make_t1src(p, font, &src);
                length1_id = pdc_alloc_id(p->out);
                length2_id = pdc_alloc_id(p->out);
                length3_id = pdc_alloc_id(p->out);
            }
            break;

#ifdef PDF_TRUETYPE_SUPPORTED
            case pdc_TrueType:
            case pdc_CIDFontType2:
            {
                length1_id = pdc_alloc_id(p->out);
            }
            case pdc_Type1C:
            case pdc_CIDFontType0:
            {
                src.private_data = (void *) font->fontfilename;
                if (font->fontfilename)
                {
                    /* Read the font from file */
                    src.init = pdf_data_source_file_init;
                    src.fill = pdf_data_source_file_fill;
                    src.terminate = pdf_data_source_file_terminate;
                    switch(font->type)
                    {
                        case pdc_TrueType:
                        case pdc_CIDFontType2:
                        src.offset = (long) 0;
                        src.length = (long) 0;
                        break;

                        case pdc_Type1C:
                        case pdc_CIDFontType0:
                        src.offset = font->cff_offset;
                        src.length = (long) font->cff_length;
                        break;

                        default:
                        break;
                    }
                }
                else
                {
                    /* Read the font from memory */
                    src.init = NULL;
                    src.fill = pdf_data_source_buf_fill;
                    src.terminate = NULL;
                    switch(font->type)
                    {
                        case pdc_TrueType:
                        case pdc_CIDFontType2:
                        src.buffer_start = font->img;
                        src.buffer_length = font->filelen;
                        break;

                        case pdc_Type1C:
                        case pdc_CIDFontType0:
                        src.buffer_start = font->img + font->cff_offset;
                        src.buffer_length = font->cff_length;
                        break;

                        default:
                        break;
                    }
                    src.bytes_available = 0;
                    src.next_byte = NULL;
                }
            }
            break;
#endif /* PDF_TRUETYPE_SUPPORTED */

            default:
            break;
        }

        /* Embedded font stream dictionary */
        pdc_begin_obj(p->out, fontfile_id);     /* Embedded font stream obj */
        pdc_begin_dict(p->out);                 /* Embedded font stream dict */

        /* /Length, /Filter */
        length_id = pdc_alloc_id(p->out);
	pdc_objref(p->out, "/Length", length_id);
        switch(font->type)
        {
            case pdc_Type1:
            case pdc_MMType1:
            break;

#ifdef PDF_TRUETYPE_SUPPORTED
            case pdc_TrueType:
            case pdc_CIDFontType2:
            case pdc_Type1C:
            case pdc_CIDFontType0:
            if (font->filelen != 0L)
            {
                compress = pdc_true;
                if (pdc_get_compresslevel(p->out))
                    pdc_puts(p->out, "/Filter/FlateDecode\n");
            }
            break;
#endif /* PDF_TRUETYPE_SUPPORTED */

            default:
            break;
        }

        /* /Length1, /Length2, Length3 */
        if (length1_id != PDC_BAD_ID)
	    pdc_objref(p->out, "/Length1", length1_id);
        if (length2_id != PDC_BAD_ID)
	    pdc_objref(p->out, "/Length2", length2_id);
        if (length3_id != PDC_BAD_ID)
	    pdc_objref(p->out, "/Length3", length3_id);

#ifdef PDF_TRUETYPE_SUPPORTED
        /* /Subtype */
        if(font->type == pdc_Type1C)
            pdc_puts(p->out, "/Subtype/Type1C\n");
        if (font->type == pdc_CIDFontType0)
            pdc_puts(p->out, "/Subtype/CIDFontType0C\n");
#endif /* PDF_TRUETYPE_SUPPORTED */

        pdc_end_dict(p->out);                   /* Embedded font stream dict */

        /* Stream */
	pdf_copy_stream(p, &src, compress);

        pdc_end_obj(p->out);                    /* Embedded font stream obj */

        pdc_put_pdfstreamlength(p->out, length_id);

        /* Length objects */
        switch(font->type)
        {
            case pdc_Type1:
            case pdc_MMType1:
            pdf_put_length_objs(p, &src, length1_id, length2_id, length3_id);
            break;

#ifdef PDF_TRUETYPE_SUPPORTED
            case pdc_TrueType:
            case pdc_CIDFontType2:
            if (compress)
            {
                pdc_begin_obj(p->out, length1_id);      /* Length1 obj */
                pdc_printf(p->out, "%ld\n", (long) font->filelen);
                pdc_end_obj(p->out);                    /* Length1 obj */
            }
            else
            {
                /* same as /Length */
                pdc_put_pdfstreamlength(p->out, length1_id);
            }
            break;
#endif /* PDF_TRUETYPE_SUPPORTED */

            default:
            break;
        }
    }

    if (p->flush & pdf_flush_content)
        pdc_flush_stream(p->out);

    /* restore font struct members */
    font->encoding = font_encoding;
    font->numOfCodes = font_numOfCodes;
    font->codeSize = font_codeSize;
}

void
pdf_write_doc_fonts(PDF *p)
{
    int slot;

    /* output pending font objects */
    for (slot = 0; slot < p->fonts_number; slot++)
    {
        pdc_font *font = &p->fonts[slot];

        switch(p->fonts[slot].type)
        {
            case pdc_Type1:
            case pdc_MMType1:
#ifdef PDF_TRUETYPE_SUPPORTED
            case pdc_TrueType:
            case pdc_CIDFontType2:
            case pdc_Type1C:
#endif /* PDF_TRUETYPE_SUPPORTED */
            case pdc_CIDFontType0:
            case pdc_Type3:
            if (font->obj_id != PDC_BAD_ID)
                pdf_put_font(p, font);
            break;

            default:
            break;
        }
    }
}

void
pdf_write_page_fonts(PDF *p)
{
    int i, total = 0;

    /* This doesn't really belong here, but all modules which write
     * font resources also need this, so we include it here.
     * Note that keeping track of ProcSets is considered obsolete
     * starting with PDF 1.4, so we always include the full set which
     * is written as a constant object at the beginning of the file.
     */

    pdc_objref(p->out, "/ProcSet",  p->procset_id);

    for (i = 0; i < p->fonts_number; i++)
        if (p->fonts[i].used_on_current_page == pdc_true)
            total++;

    if (total > 0)
    {
        pdc_puts(p->out, "/Font");

        pdc_begin_dict(p->out);         /* font resource dict */

        for (i = 0; i < p->fonts_number; i++)
            if (p->fonts[i].used_on_current_page == pdc_true) {
                p->fonts[i].used_on_current_page = pdc_false;   /* reset */
                pdc_printf(p->out, "/F%d", i);
		pdc_objref(p->out, "", p->fonts[i].obj_id);
            }

        pdc_end_dict(p->out);           /* font resource dict */
    }
}

void
pdf_get_page_fonts(PDF *p, pdf_reslist *rl)
{
    int i;

    for (i = 0; i < p->fonts_number; i++) {
	if (p->fonts[i].used_on_current_page) {
	    p->fonts[i].used_on_current_page = pdc_false; /* reset */
	    pdf_add_reslist(p, rl, i);
	}
    }
}

void
pdf_mark_page_font(PDF *p, int n)
{
    p->fonts[n].used_on_current_page = pdc_true;
}
