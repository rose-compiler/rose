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

/* $Id: p_util.c,v 1.1 2005/10/11 17:18:15 vuduc2 Exp $
 *
 * PDFlib utility functions
 *
 */

#define P_UTIL_C

#include "p_intern.h"
#include "p_image.h"
#include "p_layer.h"


/* ------------------------- [hyper]textformat -----------------------------*/

void
pdf_check_textformat(PDF *p, pdc_text_format textformat)
{
    if (!p->ptfrun && p->textformat == pdc_auto2 &&
        textformat != pdc_auto2)
        pdc_error(p->pdc, PDF_E_ENC_UNSUPPENCFORMAT, "textformat", 0, 0, 0);

    pdc_trace_protocol(p->pdc, 1, trc_encoding, "\tTextformat: \"%s\"\n",
                       pdc_get_keyword(textformat, pdf_textformat_keylist));
}

void
pdf_check_hypertextformat(PDF *p, pdc_text_format hypertextformat)
{
    if (!p->ptfrun && p->textformat == pdc_auto2 &&
        hypertextformat != pdc_auto2)
        pdc_error(p->pdc, PDF_E_ENC_UNSUPPENCFORMAT, "hypertextformat",
                  0, 0, 0);

    pdc_trace_protocol(p->pdc, 1, trc_encoding, "\tHypertextformat: \"%s\"\n",
		   pdc_get_keyword(hypertextformat, pdf_textformat_keylist));
}


/* ------------------------- hypertextencoding -----------------------------*/

void
pdf_check_hypertextencoding(PDF *p, pdc_encoding hypertextencoding)
{
    if (!p->ptfrun && p->textformat == pdc_auto2 &&
        hypertextencoding != pdc_unicode)
        pdc_error(p->pdc, PDF_E_ENC_UNSUPPENCFORMAT, "hypertextencoding",
                  0, 0, 0);

    pdc_trace_protocol(p->pdc, 1, trc_encoding, "\tHypertextencoding: \"%s\"\n",
                       pdf_get_user_encoding(p, hypertextencoding));
}


/* ----------------------- string utility functions -----------------------*/

const char *
pdf__utf16_to_utf8(PDF *p, const char *utf16string, int len, int *size)
{
    pdc_text_format outtextformat = PDC_UTF8;
    pdc_byte *utf8string = NULL;
    int flags = PDC_CONV_WITHBOM | PDC_CONV_TRY7BYTES;
    int outlen;

    if (!utf16string)
        pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "utf16string", 0, 0, 0);

    pdc_convert_string(p->pdc, pdc_utf16, 0, NULL,
                       (pdc_byte *) utf16string, len,
                       &outtextformat, NULL, &utf8string, &outlen,
                       flags, pdc_true);
    pdf_insert_utilstring(p, (char *) utf8string, pdc_false);

    if (size) *size = outlen;

    return (char *) utf8string;
}

const char *
pdf__utf8_to_utf16(PDF *p, const char *utf8string, const char *format,
                   int *outlen)
{
    pdc_text_format textformat = PDC_UTF8;
    pdc_text_format outtextformat = pdc_utf16;
    pdc_byte *utf16string = NULL;
    int flags = 0;
    int len;

    if (!utf8string)
        pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "utf8string", 0, 0, 0);
    len = (int) strlen(utf8string);

    if (format && *format)
    {
        int k = pdc_get_keycode_ci(format, pdf_textformat_keylist);
        if (k == PDC_KEY_NOTFOUND ||
            ((pdc_text_format) k != pdc_utf16 &&
             (pdc_text_format) k != pdc_utf16be &&
             (pdc_text_format) k != pdc_utf16le))
            pdc_error(p->pdc, PDC_E_ILLARG_STRING, "format", format, 0, 0);
        outtextformat = (pdc_text_format) k;
    }

    if (outtextformat == pdc_utf16)
        flags |= PDC_CONV_NOBOM;
    else
        flags |= PDC_CONV_WITHBOM;
    pdc_convert_string(p->pdc, textformat, 0, NULL,
                      (pdc_byte *) utf8string, len,
                      &outtextformat, NULL, &utf16string, outlen,
                      flags, pdc_true);
    pdf_insert_utilstring(p, (char *) utf16string, pdc_false);

    return (char *) utf16string;
}

void
pdf_init_stringlists(PDF *p)
{
    p->stringlists_number = 0;
    p->stringlists_capacity = 0;
    p->stringlists = NULL;
    p->stringlistsizes = NULL;
    p->utilstrlist_index = -1;
    p->utilstring_number = 1;
}

int
pdf_insert_stringlist(PDF *p, char **stringlist, int ns)
{
    static const char fn[] = "pdf_insert_stringlist";
    int i;

    if (p->stringlists_number == p->stringlists_capacity)
    {
        int j = p->stringlists_capacity;

        if (!p->stringlists_capacity)
        {
            p->stringlists_capacity = STRINGLISTS_CHUNKSIZE;

            p->stringlists = (char ***) pdc_malloc(p->pdc,
                sizeof(char **) * p->stringlists_capacity, fn);

            p->stringlistsizes = (int *) pdc_malloc(p->pdc,
                sizeof(int) * p->stringlists_capacity, fn);
        }
        else
        {
            p->stringlists_capacity *= 2;
            p->stringlists = (char ***) pdc_realloc(p->pdc, p->stringlists,
                sizeof(char **) * p->stringlists_capacity, fn);

            p->stringlistsizes = (int *) pdc_realloc(p->pdc, p->stringlistsizes,
                sizeof(int) * p->stringlists_capacity, fn);
        }
        for (i = j; i < p->stringlists_capacity; i++)
        {
            p->stringlists[i] = NULL;
            p->stringlistsizes[i] = 0;
        }
    }

    i = p->stringlists_number;
    p->stringlists[i] = stringlist;
    p->stringlistsizes[i] = ns;
    p->stringlists_number++;

    return i;
}

void
pdf_cleanup_stringlists(PDF *p)
{
    int i;

    if (p->stringlists)
    {
        for (i = 0; i < p->stringlists_number; i++)
        {
            if (p->stringlists[i])
                pdc_cleanup_optstringlist(p->pdc,
                    p->stringlists[i], p->stringlistsizes[i]);
        }
        pdc_free(p->pdc, p->stringlists);
        pdc_free(p->pdc, p->stringlistsizes);
    }

    pdf_init_stringlists(p);
}

#define PDF_MAX_UTILSTRLISTS 11

void
pdf_insert_utilstring(PDF *p, const char *utilstring, pdc_bool pdiparam)
{
    static const char fn[] = "pdf_insert_utilstring";
    char **utilstrlist;
    int i = 0;

    if (p->utilstrlist_index == -1)
    {
        utilstrlist = (char **) pdc_calloc(p->pdc,
                                    PDF_MAX_UTILSTRLISTS * sizeof (char *), fn);
        p->utilstrlist_index =
            pdf_insert_stringlist(p, utilstrlist, PDF_MAX_UTILSTRLISTS);
    }
    utilstrlist = p->stringlists[p->utilstrlist_index];

    if (!pdiparam)
    {
        if (p->utilstring_number >= PDF_MAX_UTILSTRLISTS)
            p->utilstring_number = 1;
        i = p->utilstring_number;
    }
    if (utilstrlist[i] != NULL)
        pdc_free(p->pdc, utilstrlist[i]);
    utilstrlist[i] = (char *) utilstring;
    if (i) p->utilstring_number++;
}


/* ------------------------ name treatment -------------------------------*/

char *
pdf_convert_name(PDF *p, const char *name, int len, pdc_bool withbom)
{
    pdc_encodingvector *ev = NULL;
    pdc_text_format nameformat = pdc_utf16;
    pdc_text_format outnameformat = PDC_UTF8;
    pdc_byte *outname = NULL;
    int convflags = 0, outlen;

    if (name == NULL)
        return NULL;

    if (len == 0)
    {
        len = (int) strlen(name);

        /* already UTF-8 encoded */
        if (pdc_is_utf8_bytecode(name))
        {
            if (withbom)
                return pdc_strdup(p->pdc, name);
            else
                return pdc_strdup(p->pdc, &name[3]);
        }

        /* simple host encoded string */
        nameformat = pdc_bytes;
        ev = pdf_get_encoding_vector(p, pdf_find_encoding(p, "host"));

        /* HTML character entities */
        if (p->charref)
            convflags |= PDC_CONV_HTMLCHAR;
    }

    /* convert to UTF-8 */
    convflags |= PDC_CONV_TRY7BYTES;
    if (withbom)
        convflags |= PDC_CONV_WITHBOM;

    pdc_convert_string(p->pdc, nameformat, 0, ev, (pdc_byte *) name, len,
                       &outnameformat, NULL, &outname, &outlen, convflags,
                       pdc_true);

    return (char *) outname;
}

void
pdf_put_pdfname(PDF *p, const char *name)
{
    char *ascname = (char *) name;
    int len = (int) strlen(ascname);


    pdc_put_pdfname(p->out, ascname, len);

}


/* ---------------------- hyper text treatment -------------------------------*/

pdc_encoding
pdf_get_hypertextencoding_opt(PDF *p, pdc_resopt *resopts, int *codepage,
                                 pdc_bool verbose)
{
    pdc_encoding htenc = p->hypertextencoding;
    int cp = p->hypertextcodepage;
    char **strlist;

    if (pdc_get_optvalues("hypertextencoding", resopts, NULL, &strlist))
    {
        htenc = pdf_get_hypertextencoding(p, strlist[0], &cp, verbose);
        pdf_check_hypertextencoding(p, htenc);
    }
    else if (p->hypertextencoding == pdc_invalidenc)
    {
        /* hypertextencoding not yet set */
        p->hypertextencoding = pdf_get_hypertextencoding(p, "auto",
            &p->hypertextcodepage, pdc_true);
        if (p->hypertextencoding == pdc_invalidenc)
            pdc_error(p->pdc, -1, 0, 0, 0, 0);
        htenc = p->hypertextencoding;
    }

    if (codepage) *codepage = cp;

    return htenc;
}

/* hypertext conversion for deprecated functions */
char *
pdf_convert_hypertext_depr(PDF *p, const char *text, int len)
{
    int outlen;

    if (p->hypertextencoding == pdc_invalidenc)
    {
        p->hypertextencoding = pdf_get_hypertextencoding(p, "auto",
            &p->hypertextcodepage, pdc_true);
        if (p->hypertextencoding == pdc_invalidenc)
            pdc_error(p->pdc, -1, 0, 0, 0, 0);
    }
    return pdf_convert_hypertext(p, text, len, p->hypertextformat,
                      p->hypertextencoding, p->hypertextcodepage, &outlen,
                      PDC_UTF8_FLAG, pdc_true);
}

/*
 * Conversion to PDFDoc/EBCDIC or UTF-16/[EBCDIC-]UTF-8
 */
char *
pdf_convert_hypertext(PDF *p, const char *text, int len,
    pdc_text_format hypertextformat, pdc_encoding hypertextencoding,
    int codepage, int *outlen, pdc_bool oututf8, pdc_bool verbose)
{
    pdc_encodingvector *inev = NULL, *outev = NULL;
    pdc_byte *outtext = NULL;
    pdc_text_format textformat = pdc_utf16be;
    int convflags = PDC_CONV_WITHBOM | PDC_CONV_TRYBYTES;

    *outlen = 0;

    if (text == NULL)
        return NULL;

    if (len == 0)
        len = (int) strlen(text);

    /* incoming encoding */
    if (hypertextencoding >= 0)
        inev = pdf_get_encoding_vector(p, hypertextencoding);

    /* PDFDocEncoding */
    outev = pdf_get_encoding_vector(p, pdc_pdfdoc);



    /* conversion to UTF-16-BE or PDFDocEncoding / EBCDIC */
    pdc_convert_string(p->pdc, hypertextformat, codepage, inev,
                       (pdc_byte *)text, len,
                       &textformat, outev, &outtext, outlen,
                       convflags, verbose);

    /* conversion to UTF-8 if Unicode */
    if (oututf8 && textformat == pdc_utf16be)
    {
        pdc_text_format outtextformat = PDC_UTF8;
        pdc_byte *newtext = NULL;

        pdc_convert_string(p->pdc, textformat, 0, NULL, outtext, *outlen,
                           &outtextformat, NULL, &newtext, outlen,
                           PDC_CONV_WITHBOM, verbose);
        pdc_free(p->pdc, outtext);
        outtext = newtext;
    }

    return (char *) outtext;
}

/*
 * Conversion from [EBCDIC-]UTF-8 to UTF-16 and from EBCDIC to PDFDoc
 */
static void
pdf_put_hypertext_ext(PDF *p, const char *text, pdc_bool isfilename)
{
    pdc_byte *newtext = NULL;
    pdc_encodingvector *outev = pdf_get_encoding_vector(p, pdc_pdfdoc);
    int len = (int) pdc_strlen(text);
    int convflags = PDC_CONV_WITHBOM | PDC_CONV_TRYBYTES;

    if (pdc_is_utf8_bytecode(text))
    {
        pdc_text_format textformat = PDC_UTF8;
        pdc_text_format outtextformat = pdc_utf16be;

        pdc_convert_string(p->pdc, textformat, 0, NULL, (pdc_byte *) text, len,
                           &outtextformat, outev, &newtext, &len,
                           convflags, pdc_true);
        text = (const char *) newtext;
    }

    if (isfilename)
    {
        if (pdc_is_utf16be_unicode(text))
            pdc_error(p->pdc, PDC_E_IO_UNSUPP_UNINAME, 0, 0, 0, 0);
        pdc_put_pdffilename(p->out, text, len);
    }
    else
    {
        pdc_put_pdfstring(p->out, text, len);
    }

    if (newtext != NULL)
        pdc_free(p->pdc, newtext);
}

void
pdf_put_hypertext(PDF *p, const char *text)
{
    pdf_put_hypertext_ext(p, text, pdc_false);
}

void
pdf_put_pdffilename(PDF *p, const char *text)
{
    pdf_put_hypertext_ext(p, text, pdc_true);
}


/* ------------------------ option text list -------------------------------*/

int
pdf_get_opt_textlist(PDF *p, const char *keyword, pdc_resopt *resopts,
                     pdc_encoding enc, pdc_bool ishypertext,
                     const char *fieldname, char **text, char ***textlist)
{
    int ns;
    char **strlist;

    ns = pdc_get_optvalues(keyword, resopts, NULL, &strlist);
    if (ns)
    {
        pdc_byte *string = NULL;
        pdc_encodingvector *inev = NULL, *outev = NULL;
        pdc_text_format intextformat = pdc_bytes, outtextformat = pdc_utf16be;
        pdc_bool isutf8;
        int i, outlen;

        /* whole option list or string list is in UTF-8 */
        isutf8 = pdc_is_lastopt_utf8(resopts);

        /* Encoding */
        if (ishypertext)
        {
            int codepage = 0;

            /* Initialize */
            if (enc < 0)
                enc = pdf_get_hypertextencoding(p, "auto", &codepage, pdc_true);
            if (enc >= 0)
                inev = pdf_get_encoding_vector(p, enc);

            /* PDFDocEncoding */
            outev = pdf_get_encoding_vector(p, pdc_pdfdoc);
        }
        else
        {
            if (enc == pdc_invalidenc)
            {
                if (fieldname)
                {
                    pdc_cleanup_optionlist(p->pdc, resopts);
                    pdc_error(p->pdc, PDF_E_FF_FONTMISSING, fieldname, 0, 0, 0);
                }
                return 0;
            }
            else if (enc >= 0)
            {
                outev = pdf_get_encoding_vector(p, enc);
            }
        }


        for (i = 0; i < ns; i++)
        {
            string = (pdc_byte *) strlist[i];

            if (ishypertext || isutf8)
            {
                intextformat = isutf8 ?  PDC_UTF8 : pdc_bytes;

                pdc_convert_string(p->pdc, intextformat, 0, inev,
                            string, (int) strlen((char *) string),
                            &outtextformat, outev, &string, &outlen,
                            PDC_CONV_WITHBOM | PDC_CONV_TRYBYTES, pdc_true);
                pdc_free(p->pdc, strlist[i]);
                strlist[i] = (char *) string;
            }
        }

        if (text)
            *text = strlist[0];
        else
            *textlist = strlist;

        if (fieldname)
        {
            strlist = (char **) pdc_save_lastopt(resopts, PDC_OPT_SAVEALL);
            pdf_insert_stringlist(p, strlist, ns);
        }
    }

    return ns;
}

char *
pdf_get_opt_utf8name(PDF *p, const char *keyword, pdc_resopt *resopts)
{
    char **strlist = NULL;
    char *utf8name = NULL;

    if (pdc_get_optvalues(keyword, resopts, NULL, &strlist))
    {
        utf8name = strlist[0];
        if (pdc_is_lastopt_utf8(resopts) && !pdc_is_utf8_bytecode(utf8name))
            utf8name = pdc_strdup_withbom(p->pdc, utf8name);
        else
            pdc_save_lastopt(resopts, PDC_OPT_SAVE1ELEM);
    }

    return utf8name;
}


/* -------------------------- trace handling -------------------------------*/

void
pdf_set_trace_options(PDF *p, const char *optlist)
{
    if (pdc_set_trace_options(p->pdc, optlist))
    {
        p->debug[(int) 't'] = 1;
        pdc_set_trace(p->pdc, "PDFlib " PDFLIB_VERSIONSTRING);
    }
    else
    {
        p->debug[(int) 't'] = 0;
    }
}

#define TRACEFILE       "PDFLIBLOGFILE"
#define TRACEOPTLIST    "PDFLIBLOGGING"

void
pdf_enable_trace(PDF *p)
{
    const char *fn = "pdf_enable_trace";
    const char *filename = pdc_getenv(TRACEFILE);
    const char *optlist = pdc_getenv(TRACEOPTLIST);

    if (filename!= NULL || optlist != NULL)
    {
        if (filename != NULL)
        {
            const char *option = " filename=";
            char *newoptlist;
            size_t nb;

            if (optlist == NULL)
                optlist = "";
            nb = strlen(optlist) + strlen(option) + strlen(filename);
            newoptlist = (char *) pdc_malloc(p->pdc, nb, fn);
            strcpy(newoptlist, optlist);
            strcat(newoptlist, option);
            strcat(newoptlist, filename);

            pdf_set_trace_options(p, newoptlist);
            pdc_free(p->pdc, newoptlist);
        }
        else
        {
            pdf_set_trace_options(p, optlist);
        }
    }
}


/* -------------------------- handle check -------------------------------*/

void
pdf_check_handle(PDF *p, int handle, pdc_opttype type)
{
    int minval = 0, maxval = 0;
    pdc_bool empty = pdc_false;

    switch (type)
    {
        case pdc_actionhandle:
        maxval = p->actions_number - 1;
        break;

        case pdc_bookmarkhandle:
        maxval = p->outline_count;
        break;

        case pdc_colorhandle:
        maxval = p->colorspaces_number - 1;
        break;


        case pdc_fonthandle:
        maxval = p->fonts_number - 1;
        break;

        case pdc_gstatehandle:
        maxval = p->extgstates_number - 1;
        break;


        case pdc_imagehandle:
        maxval = p->images_capacity - 1;
        if (handle >= minval && handle <= maxval &&
            (!p->images[handle].in_use ||
              p->xobjects[p->images[handle].no].type == pdi_xobject))
            empty = pdc_true;
        break;


        case pdc_pagehandle:
        maxval = p->images_capacity - 1;
        if (handle >= minval && handle <= maxval &&
            (!p->images[handle].in_use ||
              p->xobjects[p->images[handle].no].type != pdi_xobject))
            empty = pdc_true;
        break;

        case pdc_patternhandle:
        maxval = p->pattern_number - 1;
        break;

        case pdc_shadinghandle:
        maxval = p->shadings_number - 1;
        break;

        case pdc_templatehandle:
        maxval = p->images_capacity - 1;
        if (handle >= minval && handle <= maxval &&
            (!p->images[handle].in_use ||
              p->xobjects[p->images[handle].no].type != form_xobject))
            empty = pdc_true;
        break;


        default:
        break;
    }

    if (handle < minval || handle > maxval || empty)
    {
        const char *stemp1 = pdc_errprintf(p->pdc, "%.*s", PDC_ET_MAXSTRLEN,
                                           pdc_get_handletype(type));
        const char *stemp2 = pdc_errprintf(p->pdc, "%d",
                                           p->hastobepos ? handle + 1 : handle);
        pdc_error(p->pdc, PDC_E_ILLARG_HANDLE, stemp1, stemp2, 0, 0);
    }
}

void
pdf_set_clientdata(PDF *p, pdc_clientdata *cdata)
{
    memset(cdata, 0, sizeof(pdc_clientdata));

    cdata->maxaction = p->actions_number - 1;
    cdata->maxbookmark = p->outline_count;
    cdata->maxcolor = p->colorspaces_number - 1;
    cdata->maxdocument = p->pdi_capacity - 1;
    cdata->maxfont = p->fonts_number - 1;
    cdata->maxgstate = p->extgstates_number - 1;
    cdata->maximage = p->images_capacity - 1;
    cdata->maxpage = p->images_capacity - 1;
    cdata->maxpattern = p->pattern_number - 1;
    cdata->maxshading = p->shadings_number - 1;
    cdata->maxtemplate = p->images_capacity - 1;
    cdata->maxtextflow = p->textflows_capacity - 1;

    cdata->hastobepos = p->hastobepos;
    cdata->compatibility = p->compatibility;
}
