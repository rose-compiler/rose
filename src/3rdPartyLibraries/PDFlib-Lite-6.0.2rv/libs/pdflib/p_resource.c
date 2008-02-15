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

/* $Id: p_resource.c,v 1.1 2005/10/11 17:18:15 vuduc2 Exp $
 *
 * PDFlib resource routines
 *
 */

#include <errno.h>

#include "p_intern.h"

#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


#define RESOURCEFILE            "PDFLIBRESOURCE"        /* name of env var. */

#ifndef MVS
#define DEFAULTRESOURCEFILE     "pdflib.upr"
#else
#define DEFAULTRESOURCEFILE     "upr"
#endif

struct pdf_res_s {
    char                *name;
    char                *value;
    pdf_res             *prev;
    pdf_res             *next;
};

struct pdf_category_s {
    char                *category;
    pdf_res             *kids;
    pdf_category        *next;
};

struct pdf_virtfile_s {
    char                *name;
    const void          *data;
    size_t              size;
    pdc_bool            iscopy;
    int                 lockcount;
    pdf_virtfile        *next;
};

typedef enum {
    pdf_FontOutline,
    pdf_FontAFM,
    pdf_FontPFM,
    pdf_HostFont,
    pdf_Encoding,
    pdf_ICCProfile,
    pdf_StandardOutputIntent,
    pdf_SearchPath
} pdf_rescategory;

static const pdc_keyconn pdf_rescategories[] =
{
    {"FontOutline",          pdf_FontOutline},
    {"FontAFM",              pdf_FontAFM},
    {"FontPFM",              pdf_FontPFM},
    {"HostFont",             pdf_HostFont},
    {"Encoding",             pdf_Encoding},
    {"ICCProfile",           pdf_ICCProfile},
    {"StandardOutputIntent", pdf_StandardOutputIntent},
    {"SearchPath",           pdf_SearchPath},
    {NULL, 0}
};

static void
pdf_read_resourcefile(PDF *p, const char *filename)
{
    pdc_file   *fp = NULL;
    char      **linelist;
    char       *line;
    char       *category = NULL;
    char       *uprfilename = NULL;
#if defined(AS400) || defined(WIN32)
#define BUFSIZE 2048
    char        buffer[BUFSIZE];
#ifdef WIN32
    char        regkey[128];
    HKEY        hKey = NULL;
    DWORD       size, lType;
#endif
#endif
    int         il, nlines = 0, nextcat, begin;

#ifdef WIN32

/* don't add patchlevel's to registry searchpath */
#define stringiz1(x)	#x
#define stringiz(x)	stringiz1(x)

#define PDFLIBKEY  "Software\\PDFlib\\PDFlib\\"

    strcpy(regkey, PDFLIBKEY);
    strcat(regkey, PDFLIB_VERSIONSTRING);

    /* process registry entries */
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, regkey, 0L,
        (REGSAM) KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
    {
        pdc_trace_protocol(p->pdc, 1, trc_resource,
                           "\n\tRead registry key \"%s\":\n", PDFLIBKEY);

        size = BUFSIZE - 2;
        if (RegQueryValueExA(hKey, "SearchPath", (LPDWORD) NULL,
                             &lType, (LPBYTE) buffer, &size)
            == ERROR_SUCCESS && *buffer)
        {
            char **pathlist;
            int ip, np;

            np = pdc_split_stringlist(p->pdc, buffer,
                                      ";", &pathlist);
            for (ip = 0; ip < np; ip++)
                pdf_add_resource(p, "SearchPath", pathlist[ip]);
            pdc_cleanup_stringlist(p->pdc, pathlist);
        }

        size = BUFSIZE - 2;
        if (RegQueryValueExA(hKey, "prefix", (LPDWORD) NULL,
                             &lType, (LPBYTE) buffer, &size)
            == ERROR_SUCCESS && *buffer)
        {
            /* '/' because of downward compatibility */
            if (p->prefix)
            {
                pdc_free(p->pdc, p->prefix);
                p->prefix = NULL;
            }
            p->prefix = pdc_strdup(p->pdc,
                            &buffer[buffer[0] == '/' ? 1 : 0]);
        }

        RegCloseKey(hKey);
    }
#endif  /* WIN32 */

#ifdef AS400
    pdc_trace_protocol(p->pdc, 1, trc_resource, "\n\tSet default resources\n");
    strcpy (buffer, "/pdflib/");
    strcat (buffer, PDFLIB_VERSIONSTRING);
    il = (int) strlen(buffer);
    strcat (buffer, "/fonts");
    pdf_add_resource(p, "SearchPath", buffer);
    strcpy(&buffer[il], "/bind/data");
    pdf_add_resource(p, "SearchPath", buffer);
#endif  /* AS400 */

    /* searching for name of upr file */
    uprfilename = (char *)filename;
    if (!uprfilename || *uprfilename == '\0')
    {
        /* user-supplied upr file */
        uprfilename = pdc_getenv(RESOURCEFILE);
        if (!uprfilename || *uprfilename == '\0')
        {
            uprfilename = DEFAULTRESOURCEFILE;

            /* user-supplied upr file */
            fp = pdf_fopen(p, uprfilename, NULL, 0);
            if (fp == NULL)
            {
                uprfilename = NULL;
#ifdef WIN32
                /* process registry entries */
                if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, regkey, 0L,
                    (REGSAM) KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
                {
                    size = BUFSIZE - 2;
                    if (RegQueryValueExA(hKey, "resourcefile", (LPDWORD) NULL,
                                         &lType, (LPBYTE) buffer, &size)
                        == ERROR_SUCCESS && *buffer)
                    {
                        uprfilename = buffer;
                    }

                    RegCloseKey(hKey);
                }
#endif  /* WIN32 */
            }
        }

        if (!uprfilename || *uprfilename == '\0')
            return;

        if (p->resourcefilename)
        {
            pdc_free(p->pdc, p->resourcefilename);
            p->resourcefilename = NULL;
        }
        p->resourcefilename = pdc_strdup(p->pdc, uprfilename);
    }

    pdc_trace_protocol(p->pdc, 1, trc_resource,
                       "\n\tRead resource file \"%s\":\n", uprfilename);

    /* read upr file */
    if ((fp == NULL) && ((fp = pdf_fopen(p, uprfilename, "UPR ", 0)) == NULL))
	pdc_error(p->pdc, -1, 0, 0, 0, 0);

    nlines = pdc_read_textfile(p->pdc, fp, &linelist);
    pdc_fclose(fp);

    if (!nlines) return;

    /* Lines loop */
    begin = 1;
    nextcat = 0;
    for (il = 0; il < nlines; il++)
    {
        line = linelist[il];

        /* Next category */
        if (line[0] == '.' && strlen(line) == 1)
        {
            begin = 0;
            nextcat = 1;
            continue;
        }

        /* Skip category list */
        if (begin) continue;

        /* Prefiex or category expected */
        if (nextcat)
        {
            /* Directory prefix */
            if (line[0] == '/')
            {
                if (p->prefix)
                {
                    pdc_free(p->pdc, p->prefix);
                    p->prefix = NULL;
                }
                p->prefix = pdc_strdup(p->pdc, &line[1]);
                continue;
            }

            /* Ressource Category */
            category = line;
            nextcat = 0;
            continue;
        }

        /* Add resource */
        pdf_add_resource(p, category, line);
    }

    pdc_cleanup_stringlist(p->pdc, linelist);
}

void
pdf_add_resource(PDF *p, const char *category, const char *resource)
{
    static const char fn[] = "pdf_add_resource";
    pdf_rescategory rescat;
    pdf_category *cat, *lastcat = NULL;
    pdf_res *res, *lastres = NULL;
    char          *resutf8 = NULL;
    char          *name;
    char          *value;
    char          *prefix = NULL;
    size_t        len;
    int           k, absolut;

    /* We no longer raise an error but silently ignore unknown categories */
    k = pdc_get_keycode_ci(category, pdf_rescategories);
    if (k == PDC_KEY_NOTFOUND)
        return;
    rescat = (pdf_rescategory) k;

    /* Read resource configuration file if it is pending */
    if (p->resfilepending)
    {
        p->resfilepending = pdc_false;
        pdf_read_resourcefile(p, p->resourcefilename);
    }

    /* Find start of this category's resource list, if the category exists */
    for (cat = p->resources; cat != (pdf_category *) NULL; cat = cat->next)
    {
        lastcat = cat;
        if (!strcmp(cat->category, category))
            break;
    }
    if (cat == NULL)
    {
        cat = (pdf_category *) pdc_malloc(p->pdc, sizeof(pdf_category), fn);
        cat->category = pdc_strdup(p->pdc, category);
        cat->kids = NULL;
        cat->next = NULL;

        if (lastcat)
            lastcat->next = cat;
        else
            p->resources = cat;
    }

    /* Convert resource string to UTF-8 */
    resutf8 = pdf_convert_name(p, resource, 0, pdc_false);

    /* Determine name and value of resource */
    absolut = 0;
    len = strlen(resutf8);
    value = strchr(resutf8, '=');
    if (value)
    {
        len = (size_t) (value - resutf8);
        value++;
        if (*value == '=')
        {
            absolut = 1;
            value++;
        }

        /* file name is assumed */
        if (value[0] != '\0' && value[0] == '.' && value[1] == '/')
        {
            value += 2;
        }
    }

    /* Copy resource name */
    name = resutf8;
    name[len] = 0;
    pdc_strtrim(name);

    /* Find resource name in resource list */
    for (res = cat->kids; res != (pdf_res *) NULL; res = res->next)
    {
        if (!strcmp(res->name, name))
            break;
        lastres = res;
    }

    /* New resource */
    if (res == NULL)
    {
        res = (pdf_res *) pdc_calloc(p->pdc, sizeof(pdf_res), fn);
        if (lastres)
            lastres->next = res;
        else
            cat->kids = res;
        res->prev = lastres;
        res->name = pdc_strdup(p->pdc, name);
    }

    /* New value */
    if (res->value)
        pdc_free(p->pdc, res->value);
    res->value = NULL;
    if (!value)
    {
        value = "";
    }
    else if (!absolut && p->prefix)
    {
        /* Directory prefix */
        prefix = p->prefix;
        if (prefix[0] != '\0' && prefix[0] == '.' && prefix[1] == '/')
            prefix += 2;
        if (prefix)
            res->value = pdc_file_fullname_mem(p->pdc, prefix, value);
    }
    if (!res->value)
    {
        res->value = pdc_strdup(p->pdc, value);
        pdc_str2trim(res->value);
    }

    pdc_free(p->pdc, resutf8);

    pdc_trace_protocol(p->pdc, 1, trc_resource,
            "\tNew category.resource: \"%s.%s%s%s\"\n", category, res->name,
            strlen(res->value) ? " = " : "", res->value);

    switch (rescat)
    {
        case pdf_FontOutline:
        case pdf_FontAFM:
        case pdf_FontPFM:
        case pdf_HostFont:
        case pdf_Encoding:
        case pdf_ICCProfile:
        if (!strlen(res->name) || !strlen(res->value))
            pdc_error(p->pdc, PDF_E_RES_BADRES, resource, category, 0, 0);
        break;

        default:
        break;
    }
}

char *
pdf_find_resource(PDF *p, const char *category, const char *name)
{
    pdf_category *cat;
    pdf_res *res;

    /* Read resource configuration file if it is pending */
    if (p->resfilepending)
    {
        p->resfilepending = pdc_false;
        pdf_read_resourcefile(p, p->resourcefilename);
    }

    for (cat = p->resources; cat != (pdf_category *) NULL; cat = cat->next)
    {
        if (!strcmp(cat->category, category))
        {
            for (res = cat->kids; res != (pdf_res *)NULL; res = res->next)
            {
                if (!strcmp(res->name, name))
                {
                    pdc_trace_protocol(p->pdc, 1, trc_resource,
                            "\tFound category.resource: \"%s.%s%s%s\"\n",
                            category, res->name,
                            strlen(res->value) ? " = " : "", res->value);
                    return res->value;
                }
            }
        }
    }

    return NULL;
}

void
pdf_cleanup_resources(PDF *p)
{
    pdf_category *cat, *lastcat;
    pdf_res *res, *lastres;

    for (cat = p->resources; cat != (pdf_category *) NULL; /* */)
    {
        for (res = cat->kids; res != (pdf_res *) NULL; /* */)
        {
            lastres = res;
            res = lastres->next;
            pdc_free(p->pdc, lastres->name);
            if (lastres->value)
                pdc_free(p->pdc, lastres->value);
            pdc_free(p->pdc, lastres);
        }
        lastcat = cat;
        cat = lastcat->next;
        pdc_free(p->pdc, lastcat->category);
        pdc_free(p->pdc, lastcat);
    }

    p->resources = NULL;
}

static pdf_virtfile *
pdf_find_pvf(PDF *p, const char *filename, pdf_virtfile **lastvfile)
{
    pdf_virtfile  *vfile;

    if (lastvfile != NULL)
        *lastvfile = NULL;
    for (vfile = p->filesystem; vfile != NULL; vfile = vfile->next)
    {
        if (!strcmp(vfile->name, filename))
        {
            pdc_trace_protocol(p->pdc, 1, trc_filesearch,
                "\n\tVirtual file \"%s\" found\n", filename);
            return vfile;
        }
        if (lastvfile != NULL)
            *lastvfile = vfile;
    }
    return NULL;
}

/* definitions of pvf options */
static const pdc_defopt pdf_create_pvf_options[] =
{
    {"copy", pdc_booleanlist, 0, 1, 1, 0.0, 0.0, NULL},

    PDC_OPT_TERMINATE
};

void
pdf__create_pvf(PDF *p, const char *filename,
                const void *data, size_t size, const char *optlist)
{
    static const char fn[] = "pdf__create_pvf";
    pdc_bool iscopy = pdc_false;
    pdf_virtfile  *vfile, *lastvfile = NULL;
    pdc_resopt *results;

    if (!data)
        pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "data", 0, 0, 0);

    if (!size)
        pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "size", 0, 0, 0);

    /* Parse optlist */
    results = pdc_parse_optionlist(p->pdc, optlist, pdf_create_pvf_options,
                                   NULL, pdc_true);
    pdc_get_optvalues("copy", results, &iscopy, NULL);
    pdc_cleanup_optionlist(p->pdc, results);

    /* Find virtual file in file system */
    vfile = pdf_find_pvf(p, filename, &lastvfile);

    /* Name already exists */
    if (vfile != NULL)
        pdc_error(p->pdc, PDC_E_PVF_NAMEEXISTS, filename, 0, 0, 0);

    /* New virtual file */
    vfile = (pdf_virtfile *) pdc_calloc(p->pdc, sizeof(pdf_virtfile), fn);
    if (lastvfile)
        lastvfile->next = vfile;
    else
        p->filesystem = vfile;

    /* Fill up file struct */
    vfile->name = pdc_strdup(p->pdc, filename);
    if (iscopy == pdc_true)
    {
        vfile->data = (const void *) pdc_malloc(p->pdc, size, fn);
        memcpy((void *) vfile->data, data, size);
    }
    else
    {
        vfile->data = data;
    }
    vfile->size = size;
    vfile->iscopy = iscopy;
    vfile->lockcount = 0;
    vfile->next = NULL;

    pdc_trace_protocol(p->pdc, 1, trc_filesearch,
        "\n\tVirtual file \"%s\" created\n", filename);
}

int
pdf__delete_pvf(PDF *p, const char *filename)
{
    pdf_virtfile  *vfile, *lastvfile = NULL;

    /* Find virtual file in file system */
    vfile = pdf_find_pvf(p, filename, &lastvfile);
    if (vfile)
    {
        /* File exists but locked */
        if (vfile->lockcount > 0)
        {
            return pdc_undef;
        }

        /* Delete */
        if (vfile->iscopy == pdc_true)
        {
            pdc_free(p->pdc, (void *) vfile->data);
            vfile->data = NULL;
        }
        pdc_free(p->pdc, vfile->name);
        if (lastvfile)
            lastvfile->next = vfile->next;
        else
            p->filesystem = vfile->next;
        pdc_free(p->pdc, vfile);

        pdc_trace_protocol(p->pdc, 1, trc_filesearch,
            "\tVirtual file \"%s\" deleted\n", filename);
    }

    return pdc_true;
}

void
pdf_lock_pvf(PDF *p, const char *filename)
{
    pdf_virtfile *vfile = pdf_find_pvf(p, filename, NULL);
    if (vfile)
    {
        (vfile->lockcount)++;

        pdc_trace_protocol(p->pdc, 1, trc_filesearch,
            "\tVirtual file \"%s\" locked\n", filename);
    }
}

void
pdf_unlock_pvf(PDF *p, const char *filename)
{
    pdf_virtfile *vfile = pdf_find_pvf(p, filename, NULL);
    if (vfile)
    {
        (vfile->lockcount)--;

        pdc_trace_protocol(p->pdc, 1, trc_filesearch,
            "\tVirtual file \"%s\" unlocked\n", filename);
    }
}

void
pdf_cleanup_filesystem(PDF *p)
{
    pdf_virtfile *vfile, *nextvfile;

    for (vfile = p->filesystem; vfile != NULL; /* */)
    {
        nextvfile = vfile->next;
        if (vfile->iscopy == pdc_true && vfile->data)
            pdc_free(p->pdc, (void *) vfile->data);
        if (vfile->name)
            pdc_free(p->pdc, vfile->name);
        pdc_free(p->pdc, vfile);
        vfile = nextvfile;
    }
    p->filesystem = NULL;
}

const char *
pdf_convert_filename(PDF *p, const char *filename, int len,
                     const char *paramname, pdc_bool withbom)
{
    char *fname = NULL;
    const char *outfilename = NULL;
    int i = 0;

    if (filename == NULL)
        pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, paramname, 0, 0, 0);

    fname = pdf_convert_name(p, filename, len, withbom);

    if (fname == NULL || *fname == '\0')
        pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, paramname, 0, 0, 0);

    if (pdc_is_utf8_bytecode(fname))
    {
#if defined(PDC_UNICODE_FILENAME)
        i = 3;
#else
        pdc_encodingvector *ev =
            pdf_get_encoding_vector(p, pdf_find_encoding(p, "host"));
        pdc_text_format informat = PDC_UTF8;
        pdc_text_format outformat = pdc_utf16;
        pdc_byte *ffname = NULL;

        len = (int) strlen(fname);
        pdc_convert_string(p->pdc, informat, 0, ev, (pdc_byte *) fname, len,
                           &outformat, ev, &ffname, &len, PDC_CONV_TRYBYTES,
                           pdc_true);
        pdc_free(p->pdc, fname);
        if (outformat == pdc_utf16)
        {
            pdc_free(p->pdc, ffname);
            pdc_error(p->pdc, PDC_E_IO_UNSUPP_UNINAME, 0, 0, 0, 0);
        }
        fname = (char *) ffname;
#endif
    }

    outfilename = pdc_errprintf(p->pdc, "%s", &fname[i]);
    pdc_free(p->pdc, fname);

    return outfilename;
}

#if defined(_MSC_VER) && defined(_MANAGED)
#pragma unmanaged
#endif
pdc_file *
pdf_fopen(PDF *p, const char *filename, const char *qualifier, int flags)
{
    char fullname[PDC_FILENAMELEN];

    return pdf_fopen_name(p, filename, fullname, qualifier, flags);
}

pdc_file *
pdf_fopen_name(PDF *p, const char *filename, char *fullname,
               const char *qualifier, int flags)
{
    const pdc_byte *data = NULL;
    pdc_file *sfp = NULL;
    size_t size = 0;
    pdf_virtfile *vfile;

    strcpy(fullname, filename);

    vfile = pdf_find_pvf(p, filename, NULL);
    if (vfile)
    {
        size = vfile->size;
        data = (const pdc_byte *) vfile->data;
        sfp = pdc_fopen(p->pdc, filename, qualifier, data, size, flags);
    }
    else
    {
        pdf_category *cat;

        /* Bad filename */
        if (!*filename || !strcmp(filename, ".") || !strcmp(filename, ".."))
        {
            pdc_set_errmsg(p->pdc, PDC_E_IO_ILLFILENAME, filename, 0, 0, 0);
            return NULL;
        }


        /* Read resource configuration file if it is pending */
        if (p->resfilepending)
        {
            p->resfilepending = pdc_false;
            pdf_read_resourcefile(p, p->resourcefilename);
        }

        pdc_trace_protocol(p->pdc, 1, trc_filesearch,
            "\n\tSearching for file \"%s\":\n", filename);

        /* Searching resource category */
        for (cat = p->resources; cat != (pdf_category *) NULL; cat = cat->next)
            if (!strcmp(cat->category, "SearchPath")) break;

        if (!cat)
        {
            /* No resource category */
            sfp = pdc_fopen(p->pdc, filename, qualifier, NULL, 0, flags);
        }
        else
        {
            pdf_res *res = cat->kids;
            pdf_res *lastres = cat->kids;
            char *pathname = NULL;
            FILE *fp = NULL;
            int errnum = PDC_E_IO_RDOPEN_NF;

            /* Find last SearchPath entry */
            while (res != (pdf_res *) NULL)
            {
                lastres = res;
                res = res->next;
            }

            /* First local search and then search with concatenated
             * filename with search paths one after another backwards
             */
            while (1)
            {
                /* Test opening */
                pdc_file_fullname(pathname, filename, fullname);

                if (pathname != NULL)
                    pdc_trace_protocol(p->pdc, 1, trc_filesearch,
                        "\tin directory \"%s\": \"%s\"\n", pathname, fullname);

                fp = pdc_wfopen(p->pdc, fullname, READBMODE);
                if (fp)
                {
                    /* File found */
                    fclose(fp);
                    sfp = pdc_fopen(p->pdc, fullname, qualifier, NULL, 0,flags);
                    break;
                }
                errnum = pdc_get_fopen_errnum(p->pdc, PDC_E_IO_RDOPEN);
                if (errno != 0 && errnum != PDC_E_IO_RDOPEN_NF) break;

                if (lastres == (pdf_res *) NULL)
                    break;

                pathname = lastres->name;
                lastres = lastres->prev;
            }

            if (sfp == NULL)
                pdc_set_fopen_errmsg(p->pdc, PDC_E_IO_RDOPEN,
                                     qualifier, filename);
            else
                filename = fullname;
        }
    }

    pdc_trace_protocol(p->pdc, 1, trc_filesearch,
        "\tFile \"%s\" %sfound\n", fullname, sfp == NULL ? "not " : "");
    return sfp;
}
#if defined(_MSC_VER) && defined(_MANAGED)
#pragma managed
#endif
