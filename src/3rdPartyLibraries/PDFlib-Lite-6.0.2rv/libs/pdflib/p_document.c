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

/* $Id: p_document.c,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * PDFlib document related routines
 *
 */


#undef MVS_TEST

#define P_DOCUMENT_C

#include "p_intern.h"
#include "p_font.h"
#include "p_image.h"
#include "p_layer.h"
#include "p_page.h"
#include "p_tagged.h"




#if (defined(WIN32) || defined(OS2)) && !defined(WINCE)
#include <fcntl.h>
#include <io.h>
#endif


#define PDF_MAX_LANGCODE  8

/* Document open modes */

typedef enum
{
    open_auto,
    open_none,
    open_bookmarks,
    open_thumbnails,
    open_fullscreen

}
pdf_openmode;

static const pdc_keyconn pdf_openmode_keylist[] =
{
    {"none",        open_none},
    {"bookmarks",   open_bookmarks},
    {"thumbnails",  open_thumbnails},
    {"fullscreen",  open_fullscreen},

    {NULL, 0}
};

static const pdc_keyconn pdf_openmode_pdfkeylist[] =
{
    {"UseNone",     open_auto},
    {"UseNone",     open_none},
    {"UseOutlines", open_bookmarks},
    {"UseThumbs",   open_thumbnails},
    {"FullScreen",  open_fullscreen},

    {NULL, 0}
};


/* Document page layout */

typedef enum
{
    layout_singlepage,
    layout_onecolumn,
    layout_twocolumnleft,
    layout_twocolumnright
}
pdf_pagelayout;

static const pdc_keyconn pdf_pagelayout_pdfkeylist[] =
{
    {"SinglePage",     layout_singlepage},
    {"OneColumn",      layout_onecolumn},
    {"TwoColumnLeft",  layout_twocolumnleft},
    {"TwoColumnRight", layout_twocolumnright},
    {NULL, 0}
};


/* NonFullScreenPageMode */

static const pdc_keyconn pdf_nonfullscreen_keylist[] =
{
    {"none",        open_none},
    {"bookmarks",   open_bookmarks},
    {"thumbnails",  open_thumbnails},

    {NULL, 0}
};

/* Direction */

typedef enum
{
    doc_l2r,
    doc_r2l
}
pdf_textdirection;

static const pdc_keyconn pdf_textdirection_pdfkeylist[] =
{
    {"L2R",   doc_l2r},
    {"R2L",   doc_r2l},
    {NULL, 0}
};


/* compatibility */

static const pdc_keyconn pdf_compatibility_keylist[] =
{
    {"1.3", PDC_1_3},
    {"1.4", PDC_1_4},
    {"1.5", PDC_1_5},
    {"1.6", PDC_1_6},
    {NULL, 0}
};


static const pdc_keyconn pdf_pdfx_keylist[] =
{
    {NULL, 0}
};


/* configurable flush points */

static const pdc_keyconn pdf_flush_keylist[] =
{
    {"none",    pdf_flush_none},
    {"page",    pdf_flush_page},
    {"content", pdf_flush_content},
    {"heavy",   pdf_flush_heavy},
    {NULL, 0}
};

static const pdc_keyconn pl_pwencoding_keylist[] =
{
    {"ebcdic",          pdc_ebcdic},
    {"pdfdoc",          pdc_pdfdoc},
    {"winansi",         pdc_winansi},
    {"macroman",        pdc_macroman_apple},
    {NULL, 0}
};

#define PDF_MAXPW 0
static const pdc_keyconn pdc_permissions_keylist[] =
{
    {NULL, 0}
};

#define PDF_SECURITY_FLAG  PDC_OPT_UNSUPP

#define PDF_LINEARIZE_FLAG  PDC_OPT_UNSUPP

#define PDF_ICC_FLAG  PDC_OPT_UNSUPP

#define PDF_TAGGED_FLAG  PDC_OPT_UNSUPP

#define PDF_METADATA_FLAG  PDC_OPT_UNSUPP

#define PDF_DOCUMENT_OPTIONS1 \
\
    {"pdfx", pdc_keywordlist, PDF_ICC_FLAG, 1, 1, \
      0.0, 0.0, pdf_pdfx_keylist}, \
\
    {"compatibility", pdc_keywordlist, PDC_OPT_IGNOREIF1, 1, 1, \
      0.0, 0.0, pdf_compatibility_keylist}, \
\
    {"flush", pdc_keywordlist, PDC_OPT_NONE, 1, 1, \
      0.0, 0.0, pdf_flush_keylist}, \
\
    {"passwordencoding", pdc_keywordlist, PDF_SECURITY_FLAG, 1, 1, \
      0.0, 0.0, pl_pwencoding_keylist}, \
\
    {"masterpassword", pdc_stringlist,  PDF_SECURITY_FLAG, 1, 1, \
      0.0, PDF_MAXPW, NULL}, \
\
    {"userpassword", pdc_stringlist,  PDF_SECURITY_FLAG, 1, 1, \
      0.0, PDF_MAXPW, NULL}, \
\
    {"permissions", pdc_keywordlist, \
      PDF_SECURITY_FLAG | PDC_OPT_BUILDOR | PDC_OPT_DUPORIGVAL, 1, 9,\
      0.0, 0.0, pdc_permissions_keylist}, \
\
    {"tagged", pdc_booleanlist, PDF_TAGGED_FLAG, 1, 1, \
      0.0, 0.0, NULL}, \
\
    {"lang", pdc_stringlist,  PDF_TAGGED_FLAG, 1, 1, \
      0.0, PDF_MAX_LANGCODE, NULL}, \
\
    {"groups", pdc_stringlist,  PDC_OPT_NONE, 1, PDC_USHRT_MAX, \
      0.0, PDF_MAX_NAMESTRING, NULL}, \
\
    {"linearize", pdc_booleanlist, PDF_LINEARIZE_FLAG, 1, 1, \
      0.0, 0.0, NULL}, \
\
    {"inmemory", pdc_booleanlist, PDF_LINEARIZE_FLAG, 1, 1,\
      0.0, 0.0, NULL}, \
\
    {"tempdirname", pdc_stringlist,  PDF_LINEARIZE_FLAG, 1, 1, \
      4.0, 400.0, NULL}, \


#if defined(MVS) || defined(MVS_TEST)
#define PDF_DOCUMENT_OPTIONS10 \
\
    {"recordsize", pdc_integerlist, PDF_LINEARIZE_FLAG, 1, 1, \
      0.0, 32768.0, NULL}, \
\
    {"tempfilenames", pdc_stringlist,  PDF_LINEARIZE_FLAG, 2, 2, \
      4.0, 400.0, NULL}, \

#endif


#define PDF_DOCUMENT_OPTIONS2 \
\
    {"destination", pdc_stringlist, PDC_OPT_NONE, 1, 1, \
      0.0, PDF_MAXSTRINGSIZE, NULL}, \
\
    {"action", pdc_stringlist, PDC_OPT_NONE, 1, 1, \
      0.0, PDF_MAXSTRINGSIZE, NULL}, \
\
    {"labels", pdc_stringlist,  PDC_OPT_NONE, 1, PDC_USHRT_MAX, \
      0.0, PDC_USHRT_MAX, NULL}, \
\
    {"openmode", pdc_keywordlist, PDC_OPT_NONE, 1, 1, \
      0.0, 0.0, pdf_openmode_keylist}, \
\
    {"pagelayout", pdc_keywordlist, PDC_OPT_NONE, 1, 1, \
      0.0, 0.0, pdf_pagelayout_pdfkeylist}, \
\
    {"uri", pdc_stringlist, PDC_OPT_NONE, 1, 1, \
      0.0, PDF_MAXSTRINGSIZE, NULL}, \
\
    {"viewerpreferences", pdc_stringlist, PDC_OPT_NONE, 1, 1, \
      0.0, PDC_USHRT_MAX, NULL}, \
\
    {"metadata", pdc_stringlist, PDF_METADATA_FLAG, 1, 1, \
      0.0, PDF_MAXSTRINGSIZE, NULL}, \


/* document struct */

struct pdf_document_s
{
    int compatibility;               /* PDF version number * 10 */
    pdf_flush_state flush;           /* flush points for callback output */





    char lang[PDF_MAX_LANGCODE + 1]; /* default natural language */
    char *action;                    /* document actions */
    pdf_dest *dest;                  /* destination as open action */
    char *uri;                       /* document's base url */
    char *viewerpreferences;         /* option list with viewer preferences */
    pdc_bool writevpdict;            /* viewer preferences dictionary
                                      * must be written */
    pdf_openmode openmode;           /* document open mode */
    pdf_pagelayout pagelayout;       /* page layout within document */



    char *filename;                  /* file name of document */
    size_t (*writeproc)(PDF *p, void *data, size_t size);
                                     /* output procedure */
    FILE *fp;                        /* file id - deprecated */
    int len;                         /* length of custom */
};

static pdf_document *
pdf_init_get_document(PDF *p)
{
    static const char fn[] = "pdf_init_get_document";

    if (p->document == NULL)
    {
        pdf_document *doc = (pdf_document *)
                                pdc_malloc(p->pdc, sizeof(pdf_document), fn);

        doc->compatibility = PDC_1_5;
        doc->flush = pdf_flush_page;





        doc->lang[0] = 0;
        doc->action = NULL;
        doc->dest = NULL;
        doc->uri = NULL;
        doc->viewerpreferences = NULL;
        doc->writevpdict = pdc_false;
        doc->openmode = open_auto;
        doc->pagelayout = layout_singlepage;



        doc->fp = NULL;
        doc->filename = NULL;
        doc->writeproc = NULL;
        doc->len = 0;

        p->document = doc;
    }

    return p->document;
}

static void
pdf_cleanup_document_internal(PDF *p)
{
    pdf_document *doc = (pdf_document *) p->document;

    if (doc)
    {
        pdf_cleanup_destination(p, doc->dest);
        doc->dest = NULL;

        if (doc->action)
        {
            pdc_free(p->pdc, doc->action);
            doc->action = NULL;
        }

        if (doc->uri)
        {
            pdc_free(p->pdc, doc->uri);
            doc->uri = NULL;
        }

        if (doc->viewerpreferences)
        {
            pdc_free(p->pdc, doc->viewerpreferences);
            doc->viewerpreferences = NULL;
        }




        if (doc->filename)
        {
            pdc_free(p->pdc, doc->filename);
            doc->filename = NULL;
        }

        pdc_free(p->pdc, doc);
        p->document = NULL;
    }
}


void
pdf_fix_openmode(PDF *p)
{
    pdf_document *doc = pdf_init_get_document(p);

    if (doc->openmode == open_auto)
        doc->openmode = open_bookmarks;
}


static const pdc_defopt pdf_viewerpreferences_options[] =
{
    {"centerwindow", pdc_booleanlist, PDC_OPT_NONE, 1, 1,
      0.0, 0, NULL},

    {"direction", pdc_keywordlist, PDC_OPT_NONE, 1, 1,
      0.0, 0.0, pdf_textdirection_pdfkeylist},

    {"displaydoctitle", pdc_booleanlist, PDC_OPT_NONE, 1, 1,
      0.0, 0, NULL},

    {"fitwindow", pdc_booleanlist, PDC_OPT_NONE, 1, 1,
      0.0, 0, NULL},

    {"hidemenubar", pdc_booleanlist, PDC_OPT_NONE, 1, 1,
      0.0, 0, NULL},

    {"hidetoolbar", pdc_booleanlist, PDC_OPT_NONE, 1, 1,
      0.0, 0, NULL},

    {"hidewindowui", pdc_booleanlist, PDC_OPT_NONE, 1, 1,
      0.0, 0, NULL},

    {"nonfullscreenpagemode", pdc_keywordlist, PDC_OPT_NONE, 1, 1,
      0.0, 0.0, pdf_nonfullscreen_keylist},

    {"viewarea", pdc_keywordlist, PDC_OPT_NONE, 1, 1,
      0.0, 0.0, pdf_usebox_keylist},

    {"viewclip", pdc_keywordlist, PDC_OPT_NONE, 1, 1,
      0.0, 0.0, pdf_usebox_keylist},

    {"printarea", pdc_keywordlist, PDC_OPT_NONE, 1, 1,
      0.0, 0.0, pdf_usebox_keylist},

    {"printclip", pdc_keywordlist, PDC_OPT_NONE, 1, 1,
      0.0, 0.0, pdf_usebox_keylist},

    PDC_OPT_TERMINATE
};

static int
pdf_parse_and_write_viewerpreferences(PDF *p, const char *optlist,
                                      pdc_bool output)
{
    pdc_resopt *resopts = NULL;
    pdc_bool writevpdict = pdc_false;
    pdc_bool flag;
    int inum;

    /* parsing option list */
    resopts = pdc_parse_optionlist(p->pdc, optlist,
                  pdf_viewerpreferences_options, NULL, pdc_true);


    if (pdc_get_optvalues("hidetoolbar", resopts, &flag, NULL) && flag)
    {
        writevpdict = pdc_true;
        if (output) pdc_printf(p->out, "/HideToolbar true\n");
    }

    if (pdc_get_optvalues("hidemenubar", resopts, &flag, NULL) && flag)
    {
        writevpdict = pdc_true;
        if (output) pdc_printf(p->out, "/HideMenubar true\n");
    }

    if (pdc_get_optvalues("hidewindowui", resopts, &flag, NULL) && flag)
    {
        writevpdict = pdc_true;
        if (output) pdc_printf(p->out, "/HideWindowUI true\n");
    }

    if (pdc_get_optvalues("fitwindow", resopts, &flag, NULL) && flag)
    {
        writevpdict = pdc_true;
        if (output) pdc_printf(p->out, "/FitWindow true\n");
    }

    if (pdc_get_optvalues("centerwindow", resopts, &flag, NULL) && flag)
    {
        writevpdict = pdc_true;
        if (output) pdc_printf(p->out, "/CenterWindow true\n");
    }

    if (pdc_get_optvalues("displaydoctitle", resopts, &flag, NULL) && flag)
    {
        writevpdict = pdc_true;
        if (output) pdc_printf(p->out, "/DisplayDocTitle true\n");
    }

    if (pdc_get_optvalues("nonfullscreenpagemode", resopts, &inum, NULL) &&
        inum != (int) open_none)
    {
        writevpdict = pdc_true;
        if (output) pdc_printf(p->out, "/NonFullScreenPageMode/%s\n",
                   pdc_get_keyword(inum, pdf_openmode_pdfkeylist));
    }


    if (pdc_get_optvalues("direction", resopts, &inum, NULL) &&
        inum != (int) doc_l2r)
    {
        writevpdict = pdc_true;
        if (output) pdc_printf(p->out, "/Direction/%s\n",
                   pdc_get_keyword(inum, pdf_textdirection_pdfkeylist));
    }

    if (pdc_get_optvalues("viewarea", resopts, &inum, NULL) &&
        inum != (int) use_crop)
    {
        writevpdict = pdc_true;
        if (output) pdc_printf(p->out, "/ViewArea%s\n",
                   pdc_get_keyword(inum, pdf_usebox_pdfkeylist));
    }

    if (pdc_get_optvalues("viewclip", resopts, &inum, NULL) &&
        inum != (int) use_crop)
    {
        writevpdict = pdc_true;
        if (output) pdc_printf(p->out, "/ViewClip%s\n",
                   pdc_get_keyword(inum, pdf_usebox_pdfkeylist));
    }

    if (pdc_get_optvalues("printarea", resopts, &inum, NULL) &&
        inum != (int) use_crop)
    {
        writevpdict = pdc_true;
        if (output) pdc_printf(p->out, "/PrintArea%s\n",
                   pdc_get_keyword(inum, pdf_usebox_pdfkeylist));
    }

    if (pdc_get_optvalues("printclip", resopts, &inum, NULL) &&
        inum != (int) use_crop)
    {
        writevpdict = pdc_true;
        if (output) pdc_printf(p->out, "/PrintClip%s\n",
                   pdc_get_keyword(inum, pdf_usebox_pdfkeylist));
    }

    pdc_cleanup_optionlist(p->pdc, resopts);

    return writevpdict;
}



static void
pdf_get_document_common_options(PDF *p, pdc_resopt *resopts, int fcode)
{
    pdf_document *doc = p->document;
    char **strlist;
    int i, inum;

    if (pdc_get_optvalues("destination", resopts, NULL, &strlist))
    {
        if (doc->dest)
            pdc_free(p->pdc, doc->dest);
        doc->dest = pdf_parse_destination_optlist(p, strlist[0], 1,
                                                  pdf_openaction, pdc_true);
    }

    if (pdc_get_optvalues("action", resopts, NULL, NULL))
    {
        if (doc->action)
            pdc_free(p->pdc, doc->action);
        doc->action = (char *) pdc_save_lastopt(resopts, PDC_OPT_SAVE1ELEM);
        pdf_parse_and_write_actionlist(p, event_document, NULL, doc->action);
    }

    inum = pdc_get_optvalues("labels", resopts, NULL, &strlist);
    for (i = 0; i < inum; i++)
	pdf_set_pagelabel(p, strlist[i], fcode);

    if (pdc_get_optvalues("openmode", resopts, &inum, NULL))
        doc->openmode = (pdf_openmode) inum;


    if (pdc_get_optvalues("pagelayout", resopts, &inum, NULL))
        doc->pagelayout = (pdf_pagelayout) inum;

    if (pdc_get_optvalues("uri", resopts, NULL, NULL))
    {
        if (doc->uri)
            pdc_free(p->pdc, doc->uri);
        doc->uri = (char *) pdc_save_lastopt(resopts, PDC_OPT_SAVE1ELEM);
    }

    if (pdc_get_optvalues("viewerpreferences", resopts, NULL, NULL))
    {
        if (doc->viewerpreferences)
            pdc_free(p->pdc, doc->viewerpreferences);
        doc->viewerpreferences =
            (char *) pdc_save_lastopt(resopts, PDC_OPT_SAVE1ELEM);
        doc->writevpdict |=
            pdf_parse_and_write_viewerpreferences(p, doc->viewerpreferences,
                                                  pdc_false);
    }

}

static const pdc_defopt pdf_begin_document_options[] =
{
    PDF_DOCUMENT_OPTIONS1
#if defined(MVS) || defined(MVS_TEST)
    PDF_DOCUMENT_OPTIONS10
#endif
    PDF_DOCUMENT_OPTIONS2
    PDC_OPT_TERMINATE
};


/*
 * The external callback interface requires a PDF* as the first argument,
 * while the internal interface uses pdc_output* and doesn't know about PDF*.
 * We use a wrapper to bridge the gap, and store the PDF* within the
 * pdc_output structure opaquely.
 */

static size_t
writeproc_wrapper(pdc_output *out, void *data, size_t size)
{
    size_t ret;

    PDF *p = (PDF *) pdc_get_opaque(out);

    ret = (p->writeproc)(p, data, size);
    pdc_trace_protocol(p->pdc, 1, trc_api,
                       "/* writeproc(data[%p], %d)[%d] */\n", data, size, ret);
    return ret;
}


/* ---------------------------- begin document -------------------------- */

static int
pdf_begin_document_internal(PDF *p, const char *optlist)
{
    pdf_document *doc = p->document;
    pdc_resopt *resopts = NULL;
    char **groups = NULL;
    int n_groups = 0;
    pdc_outctl oc;

    /* parsing option list */
    if (optlist && *optlist)
    {
        int inum;
        resopts = pdc_parse_optionlist(p->pdc, optlist,
                                   pdf_begin_document_options, NULL, pdc_true);

        pdc_get_optvalues("compatibility", resopts, &doc->compatibility, NULL);

        if (pdc_get_optvalues("flush", resopts, &inum, NULL))
            doc->flush = (pdf_flush_state) inum;

        pdc_get_optvalues("lang", resopts, doc->lang, NULL);





	n_groups = pdc_get_optvalues("groups", resopts, NULL, &groups);
    }

    /* copy for easy access */
    p->compatibility = doc->compatibility;
    p->flush = doc->flush;




    /*
     * None of these functions must call pdc_alloc_id() or generate
     * any output since the output machinery is not yet initialized!
     */

    pdf_init_pages(p, (const char **) groups, n_groups);

    /* common options */
    pdf_get_document_common_options(p, resopts, PDF_FC_BEGIN_DOCUMENT);

    /* deprecated */
    p->bookmark_dest = pdf_init_destination(p);

    pdf_init_images(p);
    pdf_init_xobjects(p);
    pdf_init_fonts(p);
    pdf_init_outlines(p);
    pdf_init_annot_params(p);
    pdf_init_colorspaces(p);
    pdf_init_pattern(p);
    pdf_init_shadings(p);
    pdf_init_extgstates(p);






    /* create document digest */
    pdc_init_digest(p->out);

    if (doc->fp)
        pdc_update_digest(p->out, (pdc_byte *) doc->fp, doc->len);
    else if (doc->writeproc)
        pdc_update_digest(p->out, (pdc_byte *) &doc->writeproc, doc->len);
    else if (doc->filename && !p->ptfrun)
        pdc_update_digest(p->out, (pdc_byte *) doc->filename, doc->len);
    pdf_feed_digest_info(p);

    if (!p->ptfrun)
    {
        pdc_update_digest(p->out, (pdc_byte *) &p, sizeof(PDF*));
        pdc_update_digest(p->out, (pdc_byte *) p, sizeof(PDF));
    }


    pdc_finish_digest(p->out, !p->ptfrun);

    /* preparing output struct */
    pdc_init_outctl(&oc);
    if (doc->fp)
        oc.fp = doc->fp;
    else if (doc->writeproc)
    {
        oc.writeproc = writeproc_wrapper;
        p->writeproc = doc->writeproc;
    }
    else if (doc->filename)
        oc.filename = doc->filename;
    else
        oc.filename = "";



    if (!pdc_init_output((void *) p, p->out, doc->compatibility, &oc))
    {
        if (oc.filename && *oc.filename)
        {
            pdc_set_fopen_errmsg(p->pdc,
                pdc_get_fopen_errnum(p->pdc, PDC_E_IO_WROPEN), "PDF ",
                pdc_errprintf(p->pdc, "%.*s", PDC_ET_MAXSTRLEN, oc.filename));

            if (p->debug[(int) 'o'])
                pdc_warning(p->pdc, -1, 0, 0, 0, 0);
        }
        return -1;
    }

    /* Write the constant /ProcSet array once at the beginning */
    p->procset_id = pdc_begin_obj(p->out, PDC_NEW_ID);
    pdc_puts(p->out, "[/PDF/ImageB/ImageC/ImageI/Text]\n");
    pdc_end_obj(p->out);

    pdf_init_pages2(p);

    return 1;
}

#if defined(_MSC_VER) && defined(_MANAGED)
#pragma unmanaged
#endif
int
pdf__begin_document(PDF *p, const char *filename, int len, const char *optlist)
{
    pdf_document *doc;
    int retval;


    doc = pdf_init_get_document(p);

    /* file ID or filename */
    if (len == -1)
    {
        FILE *fp = (FILE *) filename;

        /*
         * It is the callers responsibility to open the file in binary mode,
         * but it doesn't hurt to make sure it really is.
         * The Intel version of the Metrowerks compiler doesn't have setmode().
         */
#if !defined(__MWERKS__) && (defined(WIN32) || defined(OS2))
#if defined WINCE
        _setmode(fileno(fp), _O_BINARY);
#else
        setmode(fileno(fp), O_BINARY);
#endif
#endif

        doc->fp = fp;
        doc->len = sizeof(FILE);
    }
    else if (filename && (*filename || len > 0))
    {
        filename = pdf_convert_filename(p, filename, len, "filename", pdc_true);
        doc->filename = pdc_strdup(p->pdc, filename);
        doc->len = (int) strlen(doc->filename);
    }

    retval = pdf_begin_document_internal(p, optlist);

    if (retval > -1)
        PDF_SET_STATE(p, pdf_state_document);

    return retval;
}
#if defined(_MSC_VER) && defined(_MANAGED)
#pragma managed
#endif

void
pdf__begin_document_callback(PDF *p,
    size_t (*i_writeproc)(PDF *p, void *data, size_t size), const char *optlist)
{
    size_t (*writeproc)(PDF *, void *, size_t) = i_writeproc;
    pdf_document *doc;

    if (writeproc == NULL)
        pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "writeproc", 0, 0, 0);

    doc = pdf_init_get_document(p);

    /* initializing and opening the document */
    doc->writeproc = writeproc;
    doc->len = sizeof(writeproc);

    (void) pdf_begin_document_internal(p, optlist);

    PDF_SET_STATE(p, pdf_state_document);
}


/* -----------------------------  name tree  ----------------------------- */

struct pdf_name_s
{
    pdc_id              obj_id;         /* id of this name object */
    char *              name;           /* name string */
    pdf_nametree_type   type;           /* name tree type */
};

static void
pdf_cleanup_names(PDF *p)
{
    int i;

    if (p->names == NULL)
        return;

    for (i = 0; i < p->names_number; i++)
    {
        pdc_free(p->pdc, p->names[i].name);
    }

    pdc_free(p->pdc, p->names);
    p->names_number = 0;
    p->names = NULL;
}

void
pdf_insert_name(PDF *p, const char *name, pdf_nametree_type type, pdc_id obj_id)
{
    static const char fn[] = "pdf_insert_name";
    int i;

    if (p->names == NULL || p->names_number == p->names_capacity)
    {
        if (p->names == NULL)
        {
            p->names_number = 0;
            p->names_capacity = NAMES_CHUNKSIZE;
            p->names = (pdf_name *) pdc_malloc(p->pdc,
                sizeof(pdf_name) * p->names_capacity, fn);
        }
        else
        {
            p->names_capacity *= 2;
            p->names = (pdf_name *) pdc_realloc(p->pdc, p->names,
                sizeof(pdf_name) * p->names_capacity, fn);
        }
        for (i = p->names_number; i < p->names_capacity; i++)
        {
            p->names[i].obj_id = PDC_BAD_ID;
            p->names[i].name = NULL;
            p->names[i].type = names_undef;
        }
    }

    /* check identity */
    for (i = 0; i < p->names_number; i++)
    {
        if (p->names[i].type == type && !strcmp(p->names[i].name, name))
        {
            pdc_free(p->pdc, p->names[i].name);
            p->names[i].name = (char *) name;
            return;
        }
    }

    p->names[i].obj_id = obj_id;
    p->names[i].name = (char *) name;
    p->names[i].type = type;
    p->names_number++;
}

static pdc_id
pdf_write_names(PDF *p, pdf_nametree_type type)
{
    pdc_id ret = PDC_BAD_ID;
    int i, ibeg = -1, iend = 0;

    for (i = 0; i < p->names_number; i++)
    {
        if (p->names[i].type == type)
        {
            if (ibeg == -1)
                ibeg = i;
            iend = i;
        }
    }

    if (ibeg > -1)
    {
        ret = pdc_begin_obj(p->out, PDC_NEW_ID);    /* Names object */

        pdc_begin_dict(p->out);                     /* Node dict */

        pdc_puts(p->out, "/Limits");
        pdc_begin_array(p->out);
        pdf_put_hypertext(p, p->names[ibeg].name);
        pdf_put_hypertext(p, p->names[iend].name);
        pdc_end_array(p->out);

        pdc_puts(p->out, "/Names");
        pdc_begin_array(p->out);

        for (i = ibeg; i <= iend; i++)
        {
            if (p->names[i].type == type)
            {
                pdf_put_hypertext(p, p->names[i].name);
                pdc_objref(p->out, "", p->names[i].obj_id);
            }
        }

        pdc_end_array(p->out);

        pdc_end_dict(p->out);                       /* Node dict */

        pdc_end_obj(p->out);                        /* Names object */

    }
    return ret;
}

static int
name_compare( const void*  a, const void*  b)
{
    pdf_name *p1 = (pdf_name *) a;
    pdf_name *p2 = (pdf_name *) b;

    return strcmp(p1->name, p2->name);
}

/* ---------------------------- write document -------------------------- */

static pdc_id
pdf_write_pages_and_catalog(PDF *p)
{
    pdf_document *doc = p->document;
    pdc_bool openact = pdc_false;
    pdc_id act_idlist[PDF_MAX_EVENTS];
    pdc_id root_id = PDC_BAD_ID;
    pdc_id names_dests_id = PDC_BAD_ID;
    pdc_id names_javascript_id = PDC_BAD_ID;
    pdc_id names_ap_id = PDC_BAD_ID;

    pdc_id pages_id = pdf_write_pages_tree(p);
    pdc_id labels_id = pdf_write_pagelabels(p);



    /* name tree dictionaries */
    if (p->names_number)
    {

        qsort(p->names, (size_t) p->names_number, sizeof(pdf_name),
              name_compare);


        names_dests_id = pdf_write_names(p, names_dests);
        names_javascript_id = pdf_write_names(p, names_javascript);
        names_ap_id = pdf_write_names(p, names_ap);
    }




    /* write action objects */
    if (doc->action)
        pdf_parse_and_write_actionlist(p, event_document, act_idlist,
                                       (const char *) doc->action);

    root_id = pdc_begin_obj(p->out, PDC_NEW_ID);        /* Catalog object */

    pdc_begin_dict(p->out);
    pdc_puts(p->out, "/Type/Catalog\n");

    pdc_objref(p->out, "/Pages", pages_id);		/* Pages object */

    if (labels_id != PDC_BAD_ID)
    {
        pdc_objref(p->out, "/PageLabels", labels_id);
    }

    if (p->names_number)
    {
        pdc_printf(p->out, "/Names");
        pdc_begin_dict(p->out);                         /* Names */

        if (names_dests_id != PDC_BAD_ID)
            pdc_objref(p->out, "/Dests", names_dests_id);
        if (names_javascript_id != PDC_BAD_ID)
            pdc_objref(p->out, "/JavaScript", names_javascript_id);
        if (names_ap_id != PDC_BAD_ID)
            pdc_objref(p->out, "/AP", names_ap_id);

        pdc_end_dict(p->out);                           /* Names */
    }

    if (doc->writevpdict)
    {
        pdc_printf(p->out, "/ViewerPreferences\n");
        pdc_begin_dict(p->out);                         /* ViewerPreferences */
        pdf_parse_and_write_viewerpreferences(p,
                       doc->viewerpreferences, pdc_true);
        pdc_end_dict(p->out);                           /* ViewerPreferences */
    }

    if (doc->pagelayout != layout_singlepage)
        pdc_printf(p->out, "/PageLayout/%s\n",
                pdc_get_keyword(doc->pagelayout, pdf_pagelayout_pdfkeylist));

    if (doc->openmode != open_auto && doc->openmode != open_none)
        pdc_printf(p->out, "/PageMode/%s\n",
                pdc_get_keyword(doc->openmode, pdf_openmode_pdfkeylist));

    pdf_write_outline_root(p);  /* /Outlines */

    if (doc->action)  /* /AA */
        openact = pdf_write_action_entries(p, event_document, act_idlist);

    if (doc->dest && !openact)
    {
        pdc_puts(p->out, "/OpenAction");
        pdf_write_destination(p, doc->dest);
    }

    if (doc->uri)
    {
        pdc_puts(p->out, "/URI");
        pdc_begin_dict(p->out);
        pdc_printf(p->out, "/Base");
        pdf_put_hypertext(p, doc->uri);
        pdc_end_dict(p->out);
    }


    if (doc->lang[0])
    {
        pdc_puts(p->out, "/Lang");
        pdf_put_hypertext(p, doc->lang);
        pdc_puts(p->out, "\n");
    }

    /* /StructTreeRoot /MarkInfo */

    /* /OCProperties */



    /* not supported: /Threads /PieceInfo /Perms /Legal */

    pdc_end_dict(p->out);                               /* Catalog */
    pdc_end_obj(p->out);

    return root_id;
}

static void
pdf_write_document(PDF *p)
{
    if (PDF_GET_STATE(p) != pdf_state_error)
    {
        pdc_id info_id;
        pdc_id root_id;

        if (pdf_last_page(p) == 0)
            pdc_error(p->pdc, PDF_E_DOC_EMPTY, 0, 0, 0, 0);


        /* Write all pending document information up to xref table + trailer */
        info_id = pdf_write_info(p);
        pdf_write_doc_fonts(p);                 /* font objects */
        pdf_write_doc_colorspaces(p);           /* color space resources */
        pdf_write_doc_extgstates(p);            /* ExtGState resources */
        root_id = pdf_write_pages_and_catalog(p);
        pdf_write_outlines(p);
        pdc_write_xref(p->out);
        pdc_write_trailer(p->out, info_id, root_id, 0, -1, -1, -1);
    }

    pdc_close_output(p->out);
}

/* ------------------------------ end document ---------------------------- */

void
pdf_cleanup_document(PDF *p)
{
    if (PDF_GET_STATE(p) != pdf_state_object)
    {
        /* Don't call pdc_cleanup_output() here because we may still need
         * the buffer contents for pdf__get_buffer() after pdf__end_document().
         */

        pdf_cleanup_actions(p);
        pdf_cleanup_destination(p, p->bookmark_dest); /* deprecated */
        pdf_cleanup_pages(p);
        pdf_cleanup_document_internal(p);
        pdf_cleanup_info(p);
        pdf_cleanup_fonts(p);
        pdf_cleanup_outlines(p);
        pdf_cleanup_annot_params(p);
        pdf_cleanup_names(p);
        pdf_cleanup_colorspaces(p);
        pdf_cleanup_pattern(p);
        pdf_cleanup_shadings(p);
        pdf_cleanup_images(p);
        pdf_cleanup_xobjects(p);
        pdf_cleanup_extgstates(p);







        pdf_cleanup_stringlists(p);

        PDF_SET_STATE(p, pdf_state_object);
    }
}

static const pdc_defopt pdf_end_document_options[] =
{
    PDF_DOCUMENT_OPTIONS2
    PDC_OPT_TERMINATE
};

void
pdf__end_document(PDF *p, const char *optlist)
{
    pdf_document *doc;

    /* check if there are any suspended pages left.
    */
    pdf_check_suspended_pages(p);

    /* get document pointer */
    doc = pdf_init_get_document(p);

    if (optlist && *optlist)
    {
        pdc_resopt *resopts = NULL;

        /* parsing option list */
        resopts = pdc_parse_optionlist(p->pdc, optlist,
                                  pdf_end_document_options, NULL, pdc_true);

        /* get options */
        pdf_get_document_common_options(p, resopts, PDF_FC_END_DOCUMENT);
    }

    pdf_write_document(p);


    pdf_cleanup_document(p);
}

const char *
pdf__get_buffer(PDF *p, long *size)
{
    const char *ret;
    pdc_off_t llsize;


    ret = pdc_get_stream_contents(p->out, &llsize);

    if (llsize > LONG_MAX)
	pdc_error(p->pdc, PDF_E_DOC_GETBUF_2GB, 0, 0, 0, 0);

    *size = (long) llsize;
    return ret;
}


/*****************************************************************************/
/**               deprecated historical document functions                  **/
/*****************************************************************************/

void
pdf_set_flush(PDF *p, const char *flush)
{
    if (p->binding != NULL && strcmp(p->binding, "C++"))
        return;

    if (flush != NULL && *flush)
    {
        int i = pdc_get_keycode_ci(flush, pdf_flush_keylist);
        if (i != PDC_KEY_NOTFOUND)
        {
            pdf_document *doc = pdf_init_get_document(p);

            doc->flush = (pdf_flush_state) i;
            p->flush = doc->flush;
            return;
        }
        pdc_error(p->pdc, PDC_E_PAR_ILLPARAM, flush, "flush",
                  0, 0);
    }
}

void
pdf_set_uri(PDF *p, const char *uri)
{
    pdf_document *doc = pdf_init_get_document(p);

    if (doc->uri)
        pdc_free(p->pdc, doc->uri);
    doc->uri = pdc_strdup(p->pdc, uri);
}


void
pdf_set_compatibility(PDF *p, const char *compatibility)
{

    if (compatibility != NULL && *compatibility)
    {
        int i = pdc_get_keycode_ci(compatibility, pdf_compatibility_keylist);
        if (i != PDC_KEY_NOTFOUND)
        {
            pdf_document *doc = pdf_init_get_document(p);

            doc->compatibility = p->compatibility = i;
            return;
        }
        pdc_error(p->pdc, PDC_E_PAR_ILLPARAM, compatibility, "compatibility",
                  0, 0);
    }
}

void
pdf_set_openaction(PDF *p, const char *openaction)
{
    pdf_document *doc = pdf_init_get_document(p);

    if (openaction != NULL && *openaction)
    {
        pdf_cleanup_destination(p, doc->dest);
        doc->dest = pdf_parse_destination_optlist(p, openaction, 1,
                                                  pdf_openaction, pdc_true);
    }
}

void
pdf_set_openmode(PDF *p, const char *openmode)
{
    int i;

    if (openmode == NULL || !*openmode)
        openmode = "none";

    i = pdc_get_keycode_ci(openmode, pdf_openmode_keylist);
    if (i != PDC_KEY_NOTFOUND)
    {
        pdf_document *doc = pdf_init_get_document(p);

        doc->openmode = (pdf_openmode) i;
    }
    else
        pdc_error(p->pdc, PDC_E_PAR_ILLPARAM, openmode, "openmode", 0, 0);
}

void
pdf_set_viewerpreference(PDF *p, const char *viewerpreference)
{
    static const char fn[] = "pdf_set_viewerpreference";
    pdf_document *doc = pdf_init_get_document(p);
    char *optlist;
    size_t nb1 = 0, nb2 = 0;

    if (doc->viewerpreferences)
        nb1 = strlen(doc->viewerpreferences) * sizeof(char *);
    nb2 = strlen(viewerpreference) * sizeof(char *);

    optlist = (char *) pdc_malloc(p->pdc, nb1 + nb2 + 2, fn);
    optlist[0] = 0;
    if (doc->viewerpreferences)
    {
        strcat(optlist, doc->viewerpreferences);
        strcat(optlist, " ");
    }
    strcat(optlist, viewerpreference);

    if (doc->viewerpreferences)
        pdc_free(p->pdc, doc->viewerpreferences);
    doc->viewerpreferences = optlist;
    doc->writevpdict |=
        pdf_parse_and_write_viewerpreferences(p, optlist, pdc_false);
}




