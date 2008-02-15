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

/* $Id: p_afm.c,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * PDFlib AFM parsing routines
 *
 */

#include "p_intern.h"
#include "p_font.h"

#define AFM_LINEBUF       4096

#define AFM_SEPARATORS    "\f\n\r\t\v ,:;"

#define AFM_MINNUMGLYPHS  5

/* The values of each of these enumerated items correspond to an entry in the
 * table of strings defined below. Therefore, if you add a new string as
 * new keyword into the keyStrings table, you must also add a corresponding
 * pdf_afmkey AND it MUST be in the same position!
 *
 * IMPORTANT: since the sorting algorithm is a binary search, the strings of
 * keywords must be placed in lexicographical order, below. [Therefore, the
 * enumerated items are not necessarily in lexicographical order, depending
 * on the name chosen. BUT, they must be placed in the same position as the
 * corresponding key string.] The NOPE shall remain in the last position,
 * since it does not correspond to any key string.
 */

#ifndef PDFLIB_EBCDIC
typedef enum
{
    ASCENDER,
    CHARBBOX,
    CODE,
    COMPCHAR,
    CODEHEX,
    CAPHEIGHT,
    CHARWIDTH,
    CHARACTERSET,
    CHARACTERS,
    COMMENT,
    DESCENDER,
    ENCODINGSCHEME,
    ENDCHARMETRICS,
    ENDCOMPOSITES,
    ENDDIRECTION,
    ENDFONTMETRICS,
    ENDKERNDATA,
    ENDKERNPAIRS,
    ENDKERNPAIRS0,
    ENDKERNPAIRS1,
    ENDMASTERFONTMETRICS,
    ENDTRACKKERN,
    ESCCHAR,
    FAMILYNAME,
    FONTBBOX,
    FONTNAME,
    FULLNAME,
    ISBASEFONT,
    ISCIDFONT,
    ISFIXEDPITCH,
    ISFIXEDV,
    ITALICANGLE,
    KERNPAIR,
    KERNPAIRHAMT,
    KERNPAIRXAMT,
    KERNPAIRYAMT,
    LIGATURE,
    MAPPINGSCHEME,
    METRICSSETS,
    CHARNAME,
    NOTICE,
    COMPCHARPIECE,
    STARTCHARMETRICS,
    STARTCOMPFONTMETRICS,
    STARTCOMPOSITES,
    STARTDIRECTION,
    STARTFONTMETRICS,
    STARTKERNDATA,
    STARTKERNPAIRS,
    STARTKERNPAIRS0,
    STARTKERNPAIRS1,
    STARTMASTERFONTMETRICS,
    STARTTRACKKERN,
    STDHW,
    STDVW,
    TRACKKERN,
    UNDERLINEPOSITION,
    UNDERLINETHICKNESS,
    VVECTOR,
    VERSION,
    XYWIDTH,
    XY0WIDTH,
    X0WIDTH,
    Y0WIDTH,
    XY1WIDTH,
    X1WIDTH,
    Y1WIDTH,
    XWIDTH,
    YWIDTH,
    WEIGHT,
    XHEIGHT,
    NOPE
}
pdf_afmkey;

/* keywords for the system:
 * This a table of all of the current strings that are vaild AFM keys.
 * Each entry can be referenced by the appropriate pdf_afmkey value (an
 * enumerated data type defined above). If you add a new keyword here,
 * a corresponding pdf_afmkey MUST be added to the enumerated data type
 * defined above, AND it MUST be added in the same position as the
 * string is in this table.
 *
 * IMPORTANT: since the sorting algorithm is a binary search, the keywords
 * must be placed in lexicographical order. And, NULL should remain at the
 * end.
 */

static const char *keyStrings[] =
{
    "Ascender",
    "B",
    "C",
    "CC",
    "CH",
    "CapHeight",
    "CharWidth",
    "CharacterSet",
    "Characters",
    "Comment",
    "Descender",
    "EncodingScheme",
    "EndCharMetrics",
    "EndComposites",
    "EndDirection",
    "EndFontMetrics",
    "EndKernData",
    "EndKernPairs",
    "EndKernPairs0",
    "EndKernPairs1",
    "EndMasterFontMetrics",
    "EndTrackKern",
    "EscChar",
    "FamilyName",
    "FontBBox",
    "FontName",
    "FullName",
    "IsBaseFont",
    "IsCIDFont",
    "IsFixedPitch",
    "IsFixedV",
    "ItalicAngle",
    "KP",
    "KPH",
    "KPX",
    "KPY",
    "L",
    "MappingScheme",
    "MetricsSets",
    "N",
    "Notice",
    "PCC",
    "StartCharMetrics",
    "StartCompFontMetrics",
    "StartComposites",
    "StartDirection",
    "StartFontMetrics",
    "StartKernData",
    "StartKernPairs",
    "StartKernPairs0",
    "StartKernPairs1",
    "StartMasterFontMetrics",
    "StartTrackKern",
    "StdHW",
    "StdVW",
    "TrackKern",
    "UnderlinePosition",
    "UnderlineThickness",
    "VVector",
    "Version",
    "W",
    "W0",
    "W0X",
    "W0Y",
    "W1",
    "W1X",
    "W1Y",
    "WX",
    "WY",
    "Weight",
    "XHeight"
};

#else   /* !PDFLIB_EBCDIC */
#endif  /* PDFLIB_EBCDIC */

static pdc_bool
pdf_parse_afm(
    PDF *p,
    pdc_file *fp,
    pdc_font *font,
    const char *fontname,
    const char *filename)
{
    static const char fn[] = "pdf_parse_afm";
    const char *afmtype = NULL;
    char **wordlist, *keyword, *arg1;
    char line[AFM_LINEBUF];
    int i, cmp, lo, hi, nwords, nglyphs = 0, nline = 0;
    int tablen = ((sizeof keyStrings) / (sizeof (char *)));
    pdc_sint32 iz;
    double dz;
    pdc_scalar charwidth = -1;
    pdf_afmkey keynumber;
    pdc_glyphwidth *glw;
    pdc_bool toskip = pdc_false;
    pdc_bool is_zadbfont = !strcmp(fontname, "ZapfDingbats");


    /* all new glyph names of AGL 2.0 are missing */
    font->missingglyphs = 0xFFFFFFFF;

    /* read loop. because of Mac files we use pdc_fgetline */
    while (pdc_fgetline(line, AFM_LINEBUF, fp) != NULL)
    {
        /* split line */
        nline++;
        nwords = pdc_split_stringlist(p->pdc, line, AFM_SEPARATORS, &wordlist);
        if (!nwords) continue;
        keyword = wordlist[0];

        /* find keynumber */
        lo = 0;
        hi = tablen;
        keynumber = NOPE;
        while (lo < hi)
        {
            i = (lo + hi) / 2;
            cmp = strcmp(keyword, keyStrings[i]);

            if (cmp == 0)
            {
                keynumber = (pdf_afmkey) i;
                break;
            }

            if (cmp < 0)
                hi = i;
            else
                lo = i + 1;
        }

        /* unkown key */
        if (keynumber == NOPE)
        {
            if (font->verbose == pdc_true)
                pdc_warning(p->pdc, PDF_E_T1_AFMBADKEY, keyword, filename, 0,0);

            goto PDF_PARSECONTD;
        }
        if (keynumber == ENDDIRECTION)
            toskip = pdc_false;

        if (nwords == 1 || toskip == pdc_true)
            goto PDF_PARSECONTD;

        /* key switch */
        arg1 = wordlist[1];
        switch (keynumber)
        {
            case STARTDIRECTION:
            if (pdc_str2integer(arg1, 0, &iz) != pdc_true)
                goto PDF_SYNTAXERROR;
            if (iz)
                toskip = pdc_true;
            break;

            case STARTCOMPFONTMETRICS:
            afmtype = "ACFM";
            goto PDF_SYNTAXERROR;

            case STARTMASTERFONTMETRICS:
            afmtype = "AMFM";
            goto PDF_SYNTAXERROR;

            case ISCIDFONT:
            afmtype = "CID font";
            if (!strcmp(arg1, "true"))
                goto PDF_SYNTAXERROR;
            break;

            case FONTNAME:
            font->name = pdc_strdup(p->pdc, arg1);
            pdc_trace_protocol(p->pdc, 1, trc_font,
                "\tPostScript fontname: \"%s\"\n", font->name);
            break;

            /* Recognize Multiple Master fonts by last part of name */
            case FAMILYNAME:
            if (!strcmp(wordlist[nwords-1], "MM"))
                font->type = pdc_MMType1;
            break;

            /* Default: FontSpecific */
            case ENCODINGSCHEME:
            if (!pdc_stricmp(arg1, "StandardEncoding") ||
                !pdc_stricmp(arg1, "AdobeStandardEncoding"))
                font->isstdlatin = pdc_true ;
            break;

            case STDHW:
            if (pdc_str2double(arg1, &dz) != pdc_true)
                goto PDF_SYNTAXERROR;
            font->StdHW = (int) dz;
            break;

            case STDVW:
            if (pdc_str2double(arg1, &dz) != pdc_true)
                goto PDF_SYNTAXERROR;
            font->StdVW = (int) dz;
            break;

            /* If we don't know StdVW we guess it from the font weight */
            case WEIGHT:
            if (font->StdVW == 0)
            {
                if (!pdc_stricmp(arg1, "Light"))
                    font->StdVW = PDF_STEMV_LIGHT;
                else if (!pdc_stricmp(arg1, "Medium"))
                    font->StdVW = PDF_STEMV_MEDIUM;
                else if (!pdc_stricmp(arg1, "Semi") ||
                         !pdc_stricmp(arg1, "SemiBold"))
                    font->StdVW = PDF_STEMV_SEMIBOLD;
                else if (!pdc_stricmp(arg1, "Bold"))
                    font->StdVW = PDF_STEMV_BOLD;
                else if (!pdc_stricmp(arg1, "Extra") ||
                         !pdc_stricmp(arg1, "ExtraBold"))
                    font->StdVW = PDF_STEMV_EXTRABOLD;
                else if (!pdc_stricmp(arg1, "Black"))
                    font->StdVW = PDF_STEMV_BLACK;
                else
                    font->StdVW = PDF_STEMV_NORMAL;
            }
            break;

            case ISFIXEDPITCH:
            if (!pdc_stricmp(arg1, "false"))
                font->isFixedPitch = pdc_false;
            else
                font->isFixedPitch = pdc_true;
            break;

            /* New AFM 4.1 keyword "CharWidth" implies fixed pitch */
            case CHARWIDTH:
            if (pdc_str2double(arg1, &dz) != pdc_true)
                goto PDF_SYNTAXERROR;
            charwidth = dz;
            font->isFixedPitch = pdc_true;
            break;

            case ITALICANGLE:
            {
                if (pdc_str2double(arg1, &dz) != pdc_true)
                    goto PDF_SYNTAXERROR;
                font->italicAngle = dz;
            }
            break;

            case UNDERLINEPOSITION:
            if (pdc_str2double(arg1, &dz) != pdc_true)
                goto PDF_SYNTAXERROR;
            font->underlinePosition = (int) dz;
            break;

            case UNDERLINETHICKNESS:
            if (pdc_str2double(arg1, &dz) != pdc_true)
                goto PDF_SYNTAXERROR;
            font->underlineThickness = (int) dz;
            break;

            case FONTBBOX:
            {
                if (nwords != 5)
                    goto PDF_SYNTAXERROR;
                for (i = 1; i < nwords; i++)
                {
                    if (pdc_str2double(wordlist[i], &dz) != pdc_true)
                        goto PDF_SYNTAXERROR;
                    if (i == 1)
                        font->llx = dz;
                    else if (i == 2)
                        font->lly = dz;
                    else if (i == 3)
                        font->urx = dz;
                    else if (i == 4)
                        font->ury = dz;
                }
            }
            break;

            case CAPHEIGHT:
            if (pdc_str2double(arg1, &dz) != pdc_true)
                goto PDF_SYNTAXERROR;
            font->capHeight = (int) dz;
            break;

            case XHEIGHT:
            if (pdc_str2double(arg1, &dz) != pdc_true)
                goto PDF_SYNTAXERROR;
            font->xHeight = (int) dz;
            break;

            case DESCENDER:
            if (pdc_str2double(arg1, &dz) != pdc_true)
                goto PDF_SYNTAXERROR;
            font->descender = (int) dz;
            break;

            case ASCENDER:
            if (pdc_str2double(arg1, &dz) != pdc_true)
                goto PDF_SYNTAXERROR;
            font->ascender = (int) dz;
            break;

            /* Character widths */

            case STARTCHARMETRICS:
            if (pdc_str2integer(arg1, PDC_INT_UNSIGNED, (pdc_sint32 *) &nglyphs)
                != pdc_true || nglyphs <= 0)
                goto PDF_SYNTAXERROR;
            font->glw = (pdc_glyphwidth *) pdc_calloc(p->pdc,
                            (size_t) nglyphs * sizeof(pdc_glyphwidth), fn);
            break;

            /* Character code */
            case CODE:
            case CODEHEX:
            if (!nglyphs || !font->glw)
                goto PDF_SYNTAXERROR;
            if (font->numOfGlyphs >= nglyphs)
            {
                nglyphs++;
                font->glw = (pdc_glyphwidth *) pdc_realloc(p->pdc, font->glw,
                                (size_t) nglyphs * sizeof(pdc_glyphwidth), fn);
            }
            glw = &font->glw[font->numOfGlyphs];
            if (keynumber == CODE)
            {
                if (pdc_str2integer(arg1, 0, &iz) != pdc_true)
                    goto PDF_SYNTAXERROR;
            }
            else
            {
                if (pdc_str2integer(arg1, PDC_INT_HEXADEC, &iz) != pdc_true)
                    goto PDF_SYNTAXERROR;
            }
            glw->code = (pdc_short) iz;
            glw->unicode = 0;
            glw->width = (pdc_ushort)
                (font->monospace ? font->monospace : charwidth);
            font->numOfGlyphs++;

            /* Character width and name */
            for (i = 2; i < nwords; i++)
            {
                if (!strcmp(wordlist[i], "WX") ||
                    !strcmp(wordlist[i], "W0X") ||
                    !strcmp(wordlist[i], "W"))
                {
                    i++;
                    if (i == nwords)
                        goto PDF_SYNTAXERROR;
                    if (pdc_str2double(wordlist[i], &dz) != pdc_true)
                        goto PDF_SYNTAXERROR;
                    glw->width = (pdc_ushort)
                         (font->monospace ? font->monospace : dz);
                }

                if (!strcmp(wordlist[i], "N"))
                {
                    i++;
                    if (i == nwords)
                        goto PDF_SYNTAXERROR;
                    glw->unicode = is_zadbfont ?
                                     (pdc_ushort) pdc_zadb2unicode(wordlist[i]):
                                      pdc_insert_glyphname(p->pdc, wordlist[i]);
                    pdc_delete_missingglyph_bit(glw->unicode,
                                                &font->missingglyphs);

                    pdc_trace_protocol(p->pdc, 2, trc_font,
                        "\t\tGlyph%4d: Code=%3d  U+%04X  Width=%4d  \"%s\"\n",
                        font->numOfGlyphs, glw->code, glw->unicode,
                        glw->width, wordlist[i]);
                }
            }
            break;


            default:
            break;
        }

        PDF_PARSECONTD:
        pdc_cleanup_stringlist(p->pdc, wordlist);

        if (keynumber == ENDFONTMETRICS)
            break;
    }
    pdc_fclose(fp);
    return pdc_true;

    PDF_SYNTAXERROR:
    pdc_fclose(fp);
    pdc_cleanup_stringlist(p->pdc, wordlist);

    if (afmtype)
        pdc_set_errmsg(p->pdc, PDF_E_T1_UNSUPP_FORMAT, afmtype, 0, 0, 0);
    else
        pdc_set_errmsg(p->pdc, PDC_E_IO_ILLSYNTAX, "AFM ", filename,
                       pdc_errprintf(p->pdc, "%d", nline), 0);

    if (font->verbose == pdc_true)
        pdc_error(p->pdc, -1, 0, 0, 0, 0);

    return pdc_false;
}

pdc_bool
pdf_process_metrics_data(
    PDF *p,
    pdc_font *font,
    const char *fontname)
{
    static const char fn[] = "pdf_process_metrics_data";
    int *widths;
    pdc_ushort uv;
    pdc_encoding enc = font->encoding;
    pdc_encodingvector *ev = NULL;
    int foundChars = 0, tofree = 0, i, k;

    /* Unallowed encoding */
    if (enc == pdc_cid || enc < pdc_builtin) {

	pdc_set_errmsg(p->pdc, PDF_E_FONT_BADENC,
	    fontname, pdf_get_encoding_name(p, enc, font), 0, 0);

        if (font->verbose == pdc_true)
	    pdc_error(p->pdc, -1, 0, 0, 0, 0);

        return pdc_false;
    }

    /* Determine the default character width (width of space character) */
    if (font->monospace)
    {
        font->defWidth = font->monospace;
    }
    else
    {
        font->defWidth = PDF_DEFAULT_WIDTH;
        for (i = 0; i < font->numOfGlyphs; i++)
        {
            if (font->glw[i].unicode == PDF_DEFAULT_GLYPH)
            {
                font->defWidth = (int) font->glw[i].width;
                break;
            }
        }
    }

    /* builtin font */
    if (font->isstdlatin == pdc_false && enc != pdc_builtin &&
        !strcmp(font->encapiname, "auto"))
    {
        enc = pdc_builtin;
        font->encoding = enc;
    }

    /* optimizing PDF output */
    if (enc == pdc_ebcdic || enc == pdc_ebcdic_37)
        font->towinansi = pdc_winansi;

    /*
     * Generate character width according to the chosen encoding
     */

    {
        font->code2GID = (pdc_ushort *) pdc_calloc(p->pdc,
                                   font->numOfCodes  * sizeof (pdc_ushort), fn);
        font->widths = (int *) pdc_calloc(p->pdc,
                                   font->numOfCodes * sizeof(int), fn);
        widths = font->widths;

        /* Given encoding */
        if (enc >= 0 && enc < p->encodings_number)
        {
            ev = pdf_get_encoding_vector(p, enc);
            for (k = 0; k < font->numOfCodes; k++)
            {
                uv = ev->codes[k];
                widths[k] = font->defWidth;
                if (uv == 0x0000)
                    continue;

                uv = pdc_get_alter_glyphname(uv, font->missingglyphs, NULL);
                if (uv)
                {
                    for (i = 0; i < font->numOfGlyphs; i++)
                    {
                        if (font->glw[i].unicode == uv)
                        {
                            widths[k] = font->glw[i].width;
                            font->code2GID[k] = 1;
                            foundChars++;
                            break;
                        }
                    }
                }
            }

            /* No characters found and lex Symbolii */
            if (foundChars < AFM_MINNUMGLYPHS ||
                (!strcmp(fontname, "Symbol") && enc == pdc_winansi))
            {
                if (font->isstdlatin == pdc_true)
                {
                    pdc_set_errmsg(p->pdc, PDF_E_FONT_BADENC,
                        fontname, pdf_get_encoding_name(p, enc, font),
                        0, 0);

                    if (font->verbose)
                        pdc_error(p->pdc, -1, 0, 0, 0, 0);

                    return pdc_false;
                }
                else
                {
                    /* We enforce builtin encoding */
                    if (font->verbose && strcmp(font->encapiname, "auto"))
                        pdc_warning(p->pdc, PDF_E_FONT_FORCEENC,
                            pdf_get_encoding_name(p, pdc_builtin, font),
                            pdf_get_encoding_name(p, enc, font), fontname,
                            0);
                    enc = pdc_builtin;
                    font->encoding = enc;
                    font->towinansi = pdc_invalidenc;
                }
            }
        }

        /* built-in encoding */
        if (enc == pdc_builtin)
        {
            if (font->glw == NULL)
            {
                pdc_set_errmsg(p->pdc, PDF_E_FONT_BADENC,
                    fontname, pdf_get_encoding_name(p, enc, font), 0, 0);

                if (font->verbose)
                    pdc_error(p->pdc, -1, 0, 0, 0, 0);

                return pdc_false;
            }

            /* temporary encoding */
            ev = (pdc_encodingvector *)
                     pdc_malloc(p->pdc, sizeof(pdc_encodingvector), fn);
            pdc_init_encoding(p->pdc, ev, "");
            tofree = 1;
            for (i = 0; i < font->numOfCodes; i++)
            {
                widths[i] = font->defWidth;
                ev->codes[i] = PDF_DEFAULT_GLYPH;
            }
            for (i = 0; i < font->numOfGlyphs; i++)
            {
                pdc_short code = font->glw[i].code;

                if (code >= 0 && code < font->numOfCodes)
                {
                    widths[code] = font->glw[i].width;
                    ev->codes[code] = font->glw[i].unicode;
                }
            }
        }
    }


    if (tofree) pdc_cleanup_encoding(p->pdc, ev);

    if (font->glw != NULL)
    {
        pdc_free(p->pdc, font->glw);
        font->glw = NULL;
    }

    return pdc_true;
}

pdc_bool
pdf_get_metrics_afm(
    PDF *p,
    pdc_font *font,
    const char *fontname,
    pdc_encoding enc,
    const char *filename)
{
    char fullname[PDC_FILENAMELEN];
    pdc_file *afmfile;

    /* open AFM file */
    if ((afmfile = pdf_fopen_name(p, filename, fullname, "AFM ", 0)) == NULL)
    {
	if (font->verbose_open)
	    pdc_error(p->pdc, -1, 0, 0, 0, 0);

        return pdc_undef;
    }

    pdc_trace_protocol(p->pdc, 1, trc_font,
        "\tLoading AFM metric fontfile \"%s\":\n", fullname);

    /* parse AFM file */
    if (pdf_parse_afm(p, afmfile, font, fontname, fullname) == pdc_false)
        return pdc_false;

    /* process metric data */
    font->encoding = enc;
    if (pdf_process_metrics_data(p, font, fontname) == pdc_false)
        return pdc_false;

    if (!pdf_make_fontflag(p, font))
        return pdc_false;

    return pdc_true;
}

pdc_bool
pdf_get_core_metrics_afm(
    PDF *p,
    pdc_font *font,
    pdc_core_metric *metric,
    const char *fontname,
    const char *filename)
{
    pdc_file *afmfile;

    /* open AFM file */
    if ((afmfile = pdf_fopen(p, filename, "AFM ", 0)) == NULL)
	pdc_error(p->pdc, -1, 0, 0, 0, 0);

    /* parse AFM file */
    if (pdf_parse_afm(p, afmfile, font, fontname, filename) == pdc_false)
        return pdc_false;

    /* process metric data */
    font->encoding = pdc_invalidenc;
    pdc_fill_core_metric(p->pdc, font, metric);

    return pdc_true;
}

