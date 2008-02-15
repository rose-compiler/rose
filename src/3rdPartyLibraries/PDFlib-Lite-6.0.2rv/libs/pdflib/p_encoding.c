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

/* $Id: p_encoding.c,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * PDFlib encoding handling routines
 *
 */

#include "p_intern.h"
#include "p_font.h"

#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif /* WIN32 */


void
pdf__encoding_set_char(PDF *p, const char *encoding, int slot,
                       const char *glyphname, int uv)
{
    int enc;
    pdc_encodingvector *ev;
    char given;

    if (!encoding || !*encoding)
        pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "encoding", 0, 0, 0);

    if (slot < 0 || slot > 255)
        pdc_error(p->pdc, PDC_E_ILLARG_INT,
            "slot", pdc_errprintf(p->pdc, "%d", slot), 0, 0);

    if (uv < 0 || uv > PDC_MAX_UNICODE)
        pdc_error(p->pdc, PDC_E_ILLARG_INT,
            "uv", pdc_errprintf(p->pdc, "%d", uv), 0, 0);

    if (!glyphname || !*glyphname)
    {
        if (uv == 0)
            pdc_error(p->pdc, PDF_E_ENC_GLYPHORCODE, 0, 0, 0, 0);
    }

    for (enc = (int) pdc_invalidenc + 1; enc < (int) pdc_firstvarenc; enc++)
    {
        if (!strcmp(encoding, pdc_get_fixed_encoding_name((pdc_encoding) enc)))
            pdc_error(p->pdc, PDF_E_ENC_CANTCHANGE, encoding, 0, 0, 0);
    }

    if (uv == 0)
    {
        given = 1;
        uv = (int) pdc_insert_glyphname(p->pdc, glyphname);
    }
    else if (!glyphname || !*glyphname)
    {
        given = 0;
        glyphname = pdc_insert_unicode(p->pdc, (pdc_ushort) uv);
    }
    else
    {
        const char *reg_glyphname;
        pdc_ushort reg_uv;
        int retval;

        given = 1;
        reg_glyphname = pdc_unicode2adobe((pdc_ushort) uv);
        if (reg_glyphname)
        {
            if (strcmp(reg_glyphname, glyphname) &&
                p->debug[(int) 'F'] == pdc_true)
            {
                pdc_warning(p->pdc, PDF_E_ENC_BADGLYPH,
                    glyphname,
                    pdc_errprintf(p->pdc, "%04X", uv),
                    reg_glyphname, 0);
            }

            /* We take the registered name */
        }
        else
        {
            retval = pdc_adobe2unicode(glyphname);
            if (retval > -1)
            {
                reg_uv = (pdc_ushort) retval;
                if (reg_uv && reg_uv != (pdc_ushort) uv &&
                    p->debug[(int) 'F'] == pdc_true)
                {
                    pdc_error(p->pdc, PDF_E_ENC_BADUNICODE,
                        pdc_errprintf(p->pdc, "%04X", uv), glyphname,
                        pdc_errprintf(p->pdc, "%04X", reg_uv), 0);
                }
            }

            /* We register the new unicode value */
            pdc_register_glyphname(p->pdc, glyphname, (pdc_ushort) uv);
        }
    }

    /* search for a registered encoding */
    enc = pdf_find_encoding(p, encoding);
    if (enc >= p->encodings_capacity)
        pdf_grow_encodings(p);

    if (enc == pdc_invalidenc)
    {
        enc = p->encodings_number;
        p->encodings[enc].ev = pdc_new_encoding(p->pdc, encoding);
        p->encodings[enc].ev->flags |= PDC_ENC_USER;
        p->encodings[enc].ev->flags |= PDC_ENC_SETNAMES;
        p->encodings[enc].ev->flags |= PDC_ENC_ALLOCCHARS;
        p->encodings_number++;
    }
    else if (!(p->encodings[enc].ev->flags & PDC_ENC_USER))
    {
	pdc_error(p->pdc, PDF_E_ENC_CANTCHANGE, encoding, 0, 0, 0);
    }
    else if (p->encodings[enc].ev->flags & PDC_ENC_USED)
    {
	pdc_error(p->pdc, PDF_E_ENC_INUSE, encoding, 0, 0, 0);
    }

    /* Free character name */
    ev = p->encodings[enc].ev;
    if (ev->chars[slot] != NULL)
        pdc_free(p->pdc, ev->chars[slot]);

    /* Saving */
    ev->codes[slot] = (pdc_ushort) uv;
    if (glyphname != NULL)
        ev->chars[slot] = pdc_strdup(p->pdc, glyphname);
    ev->given[slot] = given;
}

const char *
pdf__encoding_get_glyphname(PDF *p, const char *encoding, int slot)
{
    const char *name = NULL;
    pdc_encoding enc;

    /* search for a registered encoding */
    enc = pdf_find_encoding(p, encoding);

    /* Unicode, glyphid or 8-bit encoding */
    if (enc == pdc_unicode)
    {
        if (slot < 0 || slot > PDC_MAX_UNICODE)
            pdc_error(p->pdc, PDC_E_ILLARG_INT,
                "slot", pdc_errprintf(p->pdc, "%d", slot), 0, 0);

        name = pdc_unicode2glyphname(p->pdc, (pdc_ushort) slot);
    }
    else if (enc == pdc_glyphid)
    {
        int fno = pdf_get_font(p);
        if (fno > -1)
        {
            pdc_font *font = &p->fonts[fno];

            if (slot < 0 || slot >= font->numOfCodes)
                pdc_error(p->pdc, PDC_E_ILLARG_INT,
                    "slot", pdc_errprintf(p->pdc, "%d", slot), 0, 0);

            if (font->encoding != pdc_glyphid)
                pdc_error(p->pdc, PDF_E_ENC_BADFONT,
                    pdc_errprintf(p->pdc, "%d", font),
                    pdf_get_encoding_name(p, enc, font), 0, 0);

            if (font->GID2Name)
                name = (const char *) font->GID2Name[slot];
            else if (font->GID2code)
                name = pdc_unicode2glyphname(p->pdc,
                           (pdc_ushort) font->GID2code[slot]);
        }
    }
    else
    {
        if (slot < 0 || slot >= 256)
	    pdc_error(p->pdc, PDC_E_ILLARG_INT,
		"slot", pdc_errprintf(p->pdc, "%d", slot), 0, 0);

        /* cid or builtin */
        if (enc < 0)
	    pdc_error(p->pdc, PDF_E_ENC_CANTQUERY, encoding, 0, 0, 0);

        if (enc >= p->encodings_number)
	    pdc_error(p->pdc, PDF_E_ENC_NOTFOUND, encoding, 0, 0, 0);

        pdf_set_encoding_glyphnames(p, enc);

        if (p->encodings[enc].ev->chars[slot])
            name = p->encodings[enc].ev->chars[slot];
    }

    if (!name)
        name = pdc_get_notdef_glyphname();
    return name;
}

int
pdf__encoding_get_unicode(PDF *p, const char *encoding, int slot)
{
    pdc_encoding enc;
    int code = 0;

    /* search for a registered encoding */
    enc = pdf_find_encoding(p, encoding);

    /* Unicode, glyphid or 8-bit encoding */
    if (enc == pdc_unicode)
    {
        if (slot < 0 || slot > PDC_MAX_UNICODE)
            pdc_error(p->pdc, PDC_E_ILLARG_INT,
                "slot", pdc_errprintf(p->pdc, "%d", slot), 0, 0);

        code = slot;
    }
    else if (enc == pdc_glyphid)
    {
        int fno = pdf_get_font(p);
        if (fno > -1 && p->fonts[fno].GID2code)
        {
            pdc_font *font = &p->fonts[fno];

            if (slot < 0 || slot >= font->numOfCodes)
                pdc_error(p->pdc, PDC_E_ILLARG_INT,
                    "slot", pdc_errprintf(p->pdc, "%d", slot), 0, 0);

            if (font->encoding != pdc_glyphid)
                pdc_error(p->pdc, PDF_E_ENC_BADFONT,
                    pdc_errprintf(p->pdc, "%d", font),
                    pdf_get_encoding_name(p, enc, font), 0, 0);

            code = font->GID2code[slot];
        }
    }
    else
    {
        if (slot < 0 || slot > 255)
            pdc_error(p->pdc, PDC_E_ILLARG_INT,
                "slot", pdc_errprintf(p->pdc, "%d", slot), 0, 0);

        /* cid or builtin */
        if (enc < 0)
            pdc_error(p->pdc, PDF_E_ENC_CANTQUERY, encoding, 0, 0, 0);

        if (enc >= p->encodings_number)
            pdc_error(p->pdc, PDF_E_ENC_NOTFOUND, encoding, 0, 0, 0);

        code = (int) p->encodings[enc].ev->codes[slot];
    }

    return code;
}


/*
 * Try to read an encoding from file.
 *
 * Return value: allocated encoding struct
 *               NULL: error
 */

static pdc_encodingvector *
pdf_read_encoding(PDF *p, const char *encoding, const char *filename,
                  pdc_bool verbose)
{
    pdc_encodingvector  *ev = NULL;
    pdc_file            *fp;
    char               **linelist = NULL, **itemlist = NULL;
    char                *line, *item;
    const char          *stemp;
    int                 nlines = 0, l, nitems;
    pdc_bool            isenc = pdc_undef;
    pdc_byte            code;
    pdc_ushort          uv;

    if ((fp = pdf_fopen(p, filename, "encoding ", 0)) == NULL)
    {
	if (verbose)
	    pdc_error(p->pdc, -1, 0, 0, 0, 0);
    }
    else
    {
        nlines = pdc_read_textfile(p->pdc, fp, &linelist);
        pdc_fclose(fp);
    }
    if (!nlines)
        return NULL;

    ev = pdc_new_encoding(p->pdc, encoding);

    for (l = 0; l < nlines; l++)
    {
        line = linelist[l];
        nitems = pdc_split_stringlist(p->pdc, line, NULL, &itemlist);
        if (!nitems) continue;

        /* Glyphname or Unicode value */
        item = itemlist[0];
        if (isenc == pdc_undef)
        {
            if (!strncmp(item, "0x", 2) || !strncmp(item, "0X", 2))
                isenc = pdc_false;
            else
                isenc = pdc_true;
        }
        if (isenc)
        {
            uv = pdc_insert_glyphname(p->pdc, item);
            if (nitems == 3 && !pdc_str2integer(itemlist[2],
                                   PDC_INT_UNICODE, (pdc_ushort *) &uv))
                goto PDF_ENC_ERROR;
        }
        else if (!pdc_str2integer(item, PDC_INT_UNICODE, (pdc_ushort *) &uv))
        {
            goto PDF_ENC_ERROR;
        }
        if (nitems < 2)
            goto PDF_ENC_ERROR;

        /* Code value */
        if (!pdc_str2integer(itemlist[1], PDC_INT_UNSIGNED | PDC_INT_CHAR,
                             (pdc_byte *) &code))
        {
            if (!pdc_str2integer(itemlist[1], PDC_INT_CODE, (pdc_byte *) &code))
                goto PDF_ENC_ERROR;
        }

        /* Saving */
        ev->codes[code] = uv;
        if (isenc)
        {
            ev->chars[code] = pdc_strdup(p->pdc, item);
            ev->given[code] = 1;
        }
        else
        {
            ev->chars[code] = (char *) pdc_insert_unicode(p->pdc, uv);
        }

        pdc_cleanup_stringlist(p->pdc, itemlist);


    }

    ev->flags |= PDC_ENC_FILE;
    ev->flags |= PDC_ENC_SETNAMES;
    if (isenc)
        ev->flags |= PDC_ENC_ALLOCCHARS;

    pdc_cleanup_stringlist(p->pdc, linelist);

    return ev;

    PDF_ENC_ERROR:
    stemp = pdc_errprintf(p->pdc, "%.*s", PDC_ET_MAXSTRLEN, line);
    pdc_cleanup_stringlist(p->pdc, itemlist);
    pdc_cleanup_stringlist(p->pdc, linelist);
    pdc_cleanup_encoding(p->pdc, ev);
    if (verbose)
        pdc_error(p->pdc, PDF_E_ENC_BADLINE,
                  filename, stemp, 0, 0);
    return NULL;
}


/*
 * Try to generate an encoding from a Unicode code page.
 *
 * Return value: allocated encoding struct
 *               NULL: error
 */

pdc_encodingvector *
pdf_generate_encoding(PDF *p, const char *encoding)
{
    pdc_encodingvector  *ev = NULL;
    char               **itemlist = NULL, *item;
    pdc_ushort           uv, uv1 = 0xFFFF, uv2 = 0xFFFF;
    int                  nitems, slot;

    nitems = pdc_split_stringlist(p->pdc, encoding, " U", &itemlist);
    if (nitems && nitems <= 2 && !strncmp(encoding, "U+", 2))
    {
        /* first unicode offset */
        item = itemlist[0];
        if (pdc_str2integer(item, PDC_INT_UNICODE, (pdc_ushort *) &uv1))
        {
            if (nitems == 2)
            {
                /* second unicode offset */
                item = itemlist[1];
                if (!pdc_str2integer(item, PDC_INT_UNICODE,
                                     (pdc_ushort *) &uv2))
                    uv2 = 0xFFFF;
            }
            if ((nitems == 1 && uv1 <= 0xFF00) ||
                (nitems == 2 && uv1 <= 0xFF80 && uv2 <= 0xFF80))
            {
                uv = uv1;
                ev = pdc_new_encoding(p->pdc, encoding);
                for (slot = 0; slot < 256; slot++)
                {
                    if (slot == 128 && nitems == 2) uv = uv2;
                    ev->codes[slot] = uv;
                    ev->chars[slot] = (char *) pdc_insert_unicode(p->pdc, uv);
                    uv++;
                }
                ev->flags |= PDC_ENC_GENERATE;
                ev->flags |= PDC_ENC_SETNAMES;
            }
        }
    }

    pdc_cleanup_stringlist(p->pdc, itemlist);
    return ev;
}

static const char *
pdf_subst_encoding_name(PDF *p, const char *encoding, char *buffer)
{
    (void) p;
    (void) buffer;

    /* special case for the platform-specific host encoding */
    if (!strcmp(encoding, "host") || !strcmp(encoding, "auto"))
    {

#if defined(PDFLIB_EBCDIC)
        return PDC_EBCDIC_NAME;

#elif defined(MAC)
        return "macroman";

#elif defined(WIN32)
        UINT cpident = GetACP();
        if (!strcmp(encoding, "auto"))
        {
            if (cpident == 10000)
                strcpy(buffer, "macroman");
            else if (cpident == 20924)
                strcpy(buffer, "ebcdic");
            else if (cpident >= 28590 && cpident <= 28599)
                sprintf(buffer, "iso8859-%d", cpident-28590);
            else if (cpident >= 28600 && cpident <= 28609)
                sprintf(buffer, "iso8859-%d", cpident-28600+10);
            else if (cpident == 1200 || cpident == 1201)
                strcpy(buffer, "unicode");
            else
                sprintf(buffer, "cp%d", cpident);
            encoding = buffer;
        }
        else
        {
            return "winansi";
        }
#else
        return "iso8859-1";
#endif
    }

    /* These encodings will be mapped to winansi */
    if (!strcmp(encoding, "host") ||
        !strcmp(encoding, "auto")  ||
        !strcmp(encoding, "cp1252"))
        return "winansi";

    return encoding;
}

pdc_encoding
pdf_insert_encoding(PDF *p, const char *encoding, int *codepage,
                    pdc_bool verbose)
{
    char *filename;
    char buffer[32];
    pdc_encodingvector *ev = NULL;
    pdc_encoding enc = (pdc_encoding) p->encodings_number;

    *codepage = 0;

    /* substituting encoding name */
    encoding = pdf_subst_encoding_name(p, encoding, buffer);

    /* check for an user-defined encoding */
    filename = pdf_find_resource(p, "Encoding", encoding);
    if (filename)
        ev = pdf_read_encoding(p, encoding, filename, verbose);
    if (ev == NULL)
    {
        /* check for a generate encoding */
        ev = pdf_generate_encoding(p, encoding);
        if (ev == NULL)
        {
            {
                if (!strcmp(encoding, PDC_ENC_TEMPNAME))
                {
                    ev = pdc_new_encoding(p->pdc, encoding);
                    ev->flags |= PDC_ENC_TEMP;
                }
                else
                {
                    pdc_set_errmsg(p->pdc, PDF_E_ENC_NOTFOUND,
                                   encoding, 0, 0, 0);

                    if (verbose)
                        pdc_error(p->pdc, -1, 0, 0, 0, 0);

                    return pdc_invalidenc;
                }
            }
        }
    }

    if (*codepage)
    {
        enc = pdc_unicode;
    }
    else
    {
        p->encodings[enc].ev = ev;
        p->encodings_number++;
    }

    return enc;
}

pdc_encoding
pdf_find_encoding(PDF *p, const char *encoding)
{
    int slot;
    char buffer[32];

    /* substituting encoding name */
    encoding = pdf_subst_encoding_name(p, encoding, buffer);

    /* search for a fixed encoding */
    for (slot = (pdc_encoding) pdc_invalidenc + 1;
         slot < (pdc_encoding) pdc_firstvarenc; slot++)
    {
        if (!strcmp(encoding, pdc_get_fixed_encoding_name((pdc_encoding) slot)))
        {
            /* copy in-core encoding at fixed slots */
            if (slot >= 0 && p->encodings[slot].ev == NULL)
               p->encodings[slot].ev = pdc_copy_core_encoding(p->pdc, encoding);
            return((pdc_encoding) slot);
        }
    }

    /* search for a user defined encoding */
    for (slot = (pdc_encoding) pdc_firstvarenc;
         slot < p->encodings_number; slot++)
    {
        if (p->encodings[slot].ev->apiname != NULL &&
            !strcmp(encoding, p->encodings[slot].ev->apiname))
            return((pdc_encoding) slot);
    }

    if (slot >= p->encodings_capacity)
        pdf_grow_encodings(p);

    /* search for an in-core encoding */
    p->encodings[slot].ev = pdc_copy_core_encoding(p->pdc, encoding);
    if (p->encodings[slot].ev != NULL)
    {
        p->encodings_number++;
        return((pdc_encoding) slot);
    }

    return (pdc_invalidenc);
}

pdc_encoding
pdf_get_hypertextencoding(PDF *p, const char *encoding, int *codepage,
                          pdc_bool verbose)
{
    pdc_encoding enc = pdc_invalidenc;

    *codepage = 0;

    if (!*encoding)
    {
        enc = pdc_unicode;
    }
    else
    {
        enc = pdf_find_encoding(p, encoding);
        if (enc == pdc_invalidenc)
            enc = pdf_insert_encoding(p, encoding, codepage, verbose);
        if (enc < 0 && enc != pdc_invalidenc && enc != pdc_unicode)
        {
            pdc_set_errmsg(p->pdc, PDF_E_ENC_BADHYPTEXTENC, encoding, 0, 0, 0);
            enc = pdc_invalidenc;
        }
    }

    if (enc == pdc_invalidenc && verbose)
        pdc_error(p->pdc, -1, 0, 0, 0, 0);

    return enc;
}

pdc_encodingvector *
pdf_get_encoding_vector(PDF *p, pdc_encoding enc)
{
    pdc_encodingvector *ev = NULL;

    if (enc >= 0)
    {
        ev = p->encodings[enc].ev;
        if (ev == NULL)
        {
            const char *encoding = pdc_get_fixed_encoding_name(enc);
            if (encoding[0])
            {
                pdf_find_encoding(p, encoding);
                ev = p->encodings[enc].ev;
            }
        }
    }

    return ev;
}

const char *
pdf_get_user_encoding(PDF *p, pdc_encoding enc)
{
    const char *encoding = pdc_get_fixed_encoding_name(enc);

    if (!encoding[0] && enc >= 0)
        encoding = p->encodings[enc].ev->apiname;

    return encoding;
}

void
pdf_set_encoding_glyphnames(PDF *p, pdc_encoding enc)
{
    pdc_encodingvector *ev = p->encodings[enc].ev;
    int slot;
    pdc_ushort uv;

    if (!(ev->flags & PDC_ENC_SETNAMES))
    {
        ev->flags |= PDC_ENC_SETNAMES;
        for (slot = 0; slot < 256; slot++)
        {
            uv = ev->codes[slot];
            ev->chars[slot] = (char *)pdc_unicode2glyphname(p->pdc, uv);
        }
    }
}

pdc_bool
pdf_get_encoding_isstdflag(PDF *p, pdc_encoding enc)
{
    pdc_encodingvector *ev = p->encodings[enc].ev;
    int slot;
    pdc_bool isstd = pdc_true;

    if (!(ev->flags & PDC_ENC_INCORE) && !(ev->flags & PDC_ENC_STDNAMES))
    {
        for (slot = 0; slot < 256; slot++)
        {
            if (!(ev->flags & PDC_ENC_SETNAMES))
                ev->chars[slot] =
                    (char *) pdc_unicode2glyphname(p->pdc, ev->codes[slot]);
            if (isstd == pdc_true && ev->chars[slot])
            {
                isstd = pdc_is_std_charname((char *) ev->chars[slot]);
                if (isstd == pdc_false && (ev->flags & PDC_ENC_SETNAMES))
                    break;
            }
        }
        ev->flags |= PDC_ENC_SETNAMES;
        if (isstd == pdc_true)
            ev->flags |= PDC_ENC_STDNAMES;
    }

    return (ev->flags & PDC_ENC_STDNAMES) ? pdc_true : pdc_false;
}

pdc_bool
pdf_is_encoding_subset(PDF *p, pdc_encodingvector *testev,
                       pdc_encodingvector *refev)
{
    pdc_ushort uv;
    int code;

    for (code = 0; code < 256; code++)
    {
        uv = testev->codes[code];
        if (pdc_get_encoding_bytecode(p->pdc, refev, uv) == -1)
        {
            const char *glyphname = pdc_unicode2adobe(uv);
            if (glyphname && strcmp(glyphname, pdc_get_notdef_glyphname()))
            {
               break;
            }
        }
    }

    return (code == 256) ? pdc_true : pdc_false;
}

void
pdf_init_encoding_ids(PDF *p)
{
    int slot;

    for (slot = 0; slot < p->encodings_capacity; slot++)
    {
        p->encodings[slot].id = PDC_BAD_ID;
        p->encodings[slot].tounicode_id = PDC_BAD_ID;
        p->encodings[slot].used_in_formfield = pdc_false;
    }
}

void
pdf_init_encodings(PDF *p)
{
    static const char fn[] = "pdf_init_encodings";
    int slot;

    p->encodings_capacity = ENCODINGS_CHUNKSIZE;

    p->encodings = (pdf_encoding *) pdc_malloc(p->pdc,
                        sizeof(pdf_encoding) * p->encodings_capacity, fn);

    for (slot = 0; slot < p->encodings_capacity; slot++)
        p->encodings[slot].ev = NULL;

    /* we must reserve the first pdc_firstvarenc slots for standard encodings,
     * because the program identify their by index of p->encodings array!
     */
    p->encodings_number = (int) pdc_firstvarenc;
}

void
pdf_grow_encodings(PDF *p)
{
    static const char fn[] = "pdf_grow_encodings";
    int slot;

    p->encodings = (pdf_encoding *) pdc_realloc(p->pdc, p->encodings,
                       sizeof(pdf_encoding) * 2 * p->encodings_capacity, fn);

    p->encodings_capacity *= 2;
    for (slot = p->encodings_number; slot < p->encodings_capacity; slot++)
    {
        p->encodings[slot].ev = NULL;
        p->encodings[slot].id = PDC_BAD_ID;
        p->encodings[slot].tounicode_id = PDC_BAD_ID;
        p->encodings[slot].used_in_formfield = pdc_false;
    }
}

void
pdf_cleanup_encodings(PDF *p)
{
    int slot;

    if (!p->encodings)
        return;

    for (slot = 0; slot < p->encodings_number; slot++)
    {
        if (p->encodings[slot].ev)
            pdc_cleanup_encoding(p->pdc, p->encodings[slot].ev);
    }

    if (p->encodings)
        pdc_free(p->pdc, p->encodings);
    p->encodings = NULL;
}

