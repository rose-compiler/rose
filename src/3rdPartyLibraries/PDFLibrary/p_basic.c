/*---------------------------------------------------------------------------*
 |        PDFlib - A library for dynamically generating PDF documents        |
 +---------------------------------------------------------------------------+
 |        Copyright (c) 1997-2000 Thomas Merz. All rights reserved.          |
 +---------------------------------------------------------------------------+
 |    This software is NOT in the public domain.  It can be used under two   |
 |    substantially different licensing terms:                               |
 |                                                                           |
 |    The commercial license is available for a fee, and allows you to       |
 |    - ship a commercial product based on PDFlib                            |
 |    - implement commercial Web services with PDFlib                        |
 |    - distribute (free or commercial) software when the source code is     |
 |      not made available                                                   |
 |    Details can be found in the file PDFlib-license.pdf.                   |
 |                                                                           |
 |    The "Aladdin Free Public License" doesn't require any license fee,     |
 |    and allows you to                                                      |
 |    - develop and distribute PDFlib-based software for which the complete  |
 |      source code is made available                                        |
 |    - redistribute PDFlib non-commercially under certain conditions        |
 |    - redistribute PDFlib on digital media for a fee if the complete       |
 |      contents of the media are freely redistributable                     |
 |    Details can be found in the file aladdin-license.pdf.                  |
 |                                                                           |
 |    These conditions extend to ports to other programming languages.       |
 |    PDFlib is distributed with no warranty of any kind. Commercial users,  |
 |    however, will receive warranty and support statements in writing.      |
 *---------------------------------------------------------------------------*/

/* p_basic.c
 *
 * PDFlib general routines
 *
 */

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "p_intern.h"
#include "p_font.h"

#if !defined(WIN32) && !defined(OS2)
#include <unistd.h>
#endif

#if defined(WIN32) || defined(OS2)
#include <fcntl.h>
#include <io.h>
#endif

#ifdef MAC
#include <Files.h>
#endif

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winbase.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#ifdef HAVE_SETLOCALE
#include <locale.h>
#endif

/*
 * Boot the library core. Currently not much happens here.
 * It is safe to call PDF_boot() multiply -- not all bindings
 * support a single initialization routine.
 */
PDFLIB_API void PDFLIB_CALL
PDF_boot(void)
{
#ifdef HAVE_SETLOCALE
    /*
     * Avoid NLS messing with our decimal output separator since
     * PDF doesn't like decimal commas very much...
     * Systems without setlocale most probably don't mess with printf()
     * formats in the first place, so it doesn't hurt to leave this
     * code out on such systems.
     */

    setlocale(LC_NUMERIC, "C");
#endif /* HAVE_SETLOCALE */
}

PDFLIB_API void PDFLIB_CALL
PDF_shutdown(void)
{
    /* */
}

#if (defined(WIN32) || defined(__CYGWIN)) && defined(PDFLIB_EXPORTS)

/*
 * DLL entry function as required by Visual C++.
 * It is currently not necessary on Windows, but will eventually 
 * be used to boot thread-global resources for PDFlib
 * (mainly font-related stuff).
 */
BOOL WINAPI
DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
	    PDF_boot();
	    break;
	case DLL_THREAD_ATTACH:
	    break;
	case DLL_THREAD_DETACH:
	    break;
	case DLL_PROCESS_DETACH:
	    PDF_shutdown();
	    break;
    }

    if (hModule || lpReserved)	/* avoid compiler warning "unreferenced parameter" */
	    return TRUE;

    return TRUE;
}
#endif	/* WIN32 && PDFLIB_EXPORT */

PDFLIB_API int PDFLIB_CALL
PDF_get_majorversion()
{
    return PDFLIB_MAJORVERSION;
}

PDFLIB_API int PDFLIB_CALL
PDF_get_minorversion()
{
    return PDFLIB_MINORVERSION;
}

/* This list must be kept in sync with the corresponding #defines in pdflib.h */
static const char *pdf_error_names[] = {
    NULL,
    "memory error",
    "I/O error",
    "runtime error",
    "index error",
    "type error",
    "division by zero error",
    "overflow error",
    "syntax error",
    "value error",
    "system error",
    "warning (ignored)",
    "unknown error"
};

/* The default error handler for C and C++ clients */
static void
pdf_c_errorhandler(PDF *p, int type, const char* shortmsg)
{
    char msg[256];

    /*
     * A line like this allows custom error handlers to supply their 
     * own program name
     */
    sprintf(msg, "PDFlib %s: %s\n", pdf_error_names[type], shortmsg);

    switch (type) {
	/* Issue a warning message and continue */
	case PDF_NonfatalError:
	    (void) fprintf(stderr, msg);
	    return;

	/* give up in all other cases */
	case PDF_MemoryError:
	case PDF_IOError:
	case PDF_RuntimeError:
	case PDF_IndexError:
	case PDF_TypeError:
	case PDF_DivisionByZero:
	case PDF_OverflowError:
	case PDF_SyntaxError:
	case PDF_ValueError:
	case PDF_SystemError:
	case PDF_UnknownError:
	default:
	    (void) fprintf(stderr, msg);	/* print message */

	    if (p != NULL) {			/* first allocation? */
		if(!p->debug['u'])		/* delete incomplete PDF file */
		    (void) unlink(p->filename);

		PDF_delete(p);			/* clean up PDFlib */
	    }
	    exit(99);				/* good-bye */
    }
}

void
pdf_error(PDF *p, int type, const char *fmt, ...)
{
    char msg[256];
    va_list ap;

    va_start(ap, fmt);
    vsprintf(msg, fmt, ap);

    /*
     * We catch non-fatals here since user-supplied error handlers
     * don't know about the debug level.
     */
    if (!p->in_error && (type != PDF_NonfatalError || ((PDF *)p)->debug['w'])) {
	p->in_error = pdf_true;	/* avoid recursive errors */
	(p->errorhandler)(p, type, msg);
    }

    /* If the error handler returns the error was non-fatal */
    p->in_error = pdf_false;

    va_end(ap);
}

static void
pdf_init_document(PDF *p)
{
    id i;

    p->file_offset_capacity = ID_CHUNKSIZE;
    p->file_offset = (long *) p->malloc(p, 
	    sizeof(long) * p->file_offset_capacity, "PDF_init_document");

    p->contents_ids_capacity = CONTENTS_CHUNKSIZE;
    p->contents_ids = (id *) p->malloc(p, 
	    sizeof(id) * p->contents_ids_capacity, "PDF_init_document");

    p->pages_capacity = PAGES_CHUNKSIZE;
    p->pages = (id *) p->malloc(p,
	    sizeof(id) * p->pages_capacity, "PDF_init_document");

    /* mark ids to allow for pre-allocation of page ids */
    for (i = 0; i < p->pages_capacity; i++)
	p->pages[i] = BAD_ID;

    p->filename		= NULL;
    p->resources	= NULL;
    p->resourcefile_loaded = pdf_false;
    p->in_error		= pdf_false;
    p->fp		= NULL;
    p->currentobj	= (id) 0;
    p->current_page	= 0;
    p->pages_id		= pdf_alloc_id(p);
    p->root_id		= pdf_alloc_id(p);
    p->state		= pdf_state_open;
    p->open_action	= retain;
    p->open_mode	= open_auto;
    p->bookmark_dest	= retain;
}

/* Free all document-related resources */
static void
pdf_cleanup_document(PDF *p)
{
    if (p->file_offset) {
	p->free(p, p->file_offset);
	p->file_offset = NULL;
    }
    if (p->contents_ids) {
	p->free(p, p->contents_ids);
	p->contents_ids = NULL;
    }
    if (p->pages) {
	p->free(p, p->pages);
	p->pages = NULL;
    }
    if (p->resourcefilename) {
	p->free(p, p->resourcefilename);
	p->resourcefilename = NULL;
    }
}

/* Free all page-related resources */
static void
pdf_cleanup_page(PDF *p)
{
    pdf_cleanup_page_annots(p);
}

/* p may be NULL on the first call - we don't use it anyway */
static void *
pdf_malloc(PDF *p, size_t size, const char *caller)
{
    void *ret;

    ret = malloc(size);

#ifdef DEBUG
    if (p != NULL && p->debug['m'])
	fprintf(stderr, "%x malloced, size %d from %s, page %d\n",
		(int)ret, size, caller, p->current_page);
#endif

    /* Special error handling at first allocation (for p itself) */
    if (p != NULL) {
	if (ret == NULL) {
	    pdf_error(p, PDF_MemoryError,
		    "Couldn't allocate memory in %s!\n", caller);
	}
    }

    return ret;
}

/* We cook up our own calloc routine, using the caller-supplied 
 * malloc and memset.
 */
static void *
pdf_calloc(PDF *p, size_t size, const char *caller)
{
    void *ret;

    if ((ret = p->malloc(p, size, caller)) == NULL) {
	pdf_error(p, PDF_MemoryError,
		"Couldn't (c)allocate memory in %s!\n", caller);
    }
    memset(ret, 0, size);

#ifdef DEBUG
    if (p->debug['c'])
	fprintf(stderr, "%x calloced, size %d from %s, page %d\n",
		(int) ret, size, caller, p->current_page);
#endif

    return ret;
}

static void *
pdf_realloc(PDF *p, void *mem, size_t size, const char *caller)
{
    void *ret;

    if ((ret = realloc(mem, size)) == NULL) {
	pdf_error(p, PDF_MemoryError,
		"Couldn't reallocate memory in %s!\n", caller);
    }

#ifdef DEBUG
    if (p->debug['r'])
	fprintf(stderr, "%x realloced to %x, %d from %s, page %d\n",
		(int) mem, (int) ret, (int) size, caller, p->current_page);
#endif
    return ret;
}

static void
pdf_free(PDF *p, void *mem)
{
#ifdef DEBUG
    if (p->debug['f'])
	fprintf(stderr, "%x freed, page %d\n", (int) mem, p->current_page);
#endif

    /* We mustn't raise a fatal error here to avoid potential recursion */
    if (mem == NULL) {
	pdf_error(p, PDF_NonfatalError,
		"(Internal) Tried to free null pointer");
	return;
    }

    free(mem);
}

static size_t
pdf_writeproc_file(PDF *p, void *data, size_t size)
{
    return fwrite(data, 1, (size_t) size, p->fp);
}

#undef PDF_new

/* This is the easy version with the default handlers */
PDFLIB_API PDF * PDFLIB_CALL
PDF_new()
{
    return PDF_new2(pdf_c_errorhandler, NULL, NULL, NULL, NULL);
}

/* This is the spiced-up version with user-defined error and memory handlers */

PDFLIB_API PDF * PDFLIB_CALL
PDF_new2(
    void  (*errorhandler)(PDF *p, int type, const char *msg),
    void* (*allocproc)(PDF *p, size_t size, const char *caller),
    void* (*reallocproc)(PDF *p, void *mem, size_t size, const char *caller),
    void  (*freeproc)(PDF *p, void *mem),
    void   *opaque)
{
    PDF *p;
    int i;

    if (errorhandler == NULL)
	errorhandler = pdf_c_errorhandler;

    /* If allocproc is NULL, all entries are supplied internally by PDFlib */
    if (allocproc == NULL) {
	allocproc	= pdf_malloc;
	reallocproc	= pdf_realloc;
	freeproc	= pdf_free;
    }

    /*
     * If this goes wrong the error handler won't be able to jump in
     * automatically because there's no p; Therefore call the handler manually
     * in case of error.
     */
    p = (PDF *) (*allocproc) (NULL, sizeof(PDF), "PDF_new");

    if (p == NULL) {
	(*errorhandler)(NULL, PDF_MemoryError, "Couldn't allocate PDF object");
	return NULL;	/* Won't happen because of the error handler */
    }

    /*
     * Guard against crashes when PDF_delete is called without any
     * PDF_open_*() in between.
     */
    memset((void *)p, 0, (size_t) sizeof(PDF));

    p->magic		= PDF_MAGIC;
    p->compatibility	= PDF_1_3;
    p->errorhandler	= errorhandler;
    p->malloc		= allocproc;
    p->realloc		= reallocproc;
    p->free		= freeproc;

    p->stream.flush	= PDF_FLUSH_PAGE;
    p->writeproc 	= NULL;
    p->stream.basepos	= NULL;

    p->opaque		= opaque;
    p->calloc		= pdf_calloc;

#ifdef HAVE_LIBZ
    p->compress		= PDF_DEFAULT_COMPRESSION;
#else
    p->compress		= 0;
#endif

    p->resourcefilename	= NULL;
    p->filename		= NULL;
    p->binding		= NULL;
    p->prefix		= NULL;
    p->state		= pdf_state_null;

    /* clear all debug flags... */
    for(i = 0; i < 128; i++)
	p->debug[i] = 0;

    /* ...except warning messages for non-fatal errors -- the
     * client must explicitly disable these.
     */
    p->debug['w'] = 1;

    return p;
}

/*
 * The caller must use the contents of the returned buffer before
 * calling the next PDFlib function.
 */

PDFLIB_API const char * PDFLIB_CALL
PDF_get_buffer(PDF *p, long *size)
{
    if (PDF_SANITY_CHECK_FAILED(p)) {
	*size = (long) 0;
	return ((const char *) NULL);
    }

    if (p->writeproc)
	pdf_error(p, PDF_RuntimeError,
	    "Don't use PDF_get_buffer() when writing to file");

    if (p->state == pdf_state_page_description)
	pdf_error(p, PDF_RuntimeError,
	"Don't use PDF_get_buffer on page descriptions");

    *size = (long) (p->stream.curpos - p->stream.basepos);

    p->stream.base_offset += (size_t) (p->stream.curpos - p->stream.basepos);
    p->stream.curpos = p->stream.basepos;

    return (const char *) p->stream.basepos;
}

PDFLIB_API void * PDFLIB_CALL
PDF_get_opaque(PDF *p)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return ((void *) NULL);

    return p->opaque;
}

/* close the output file, if opened with PDF_open_file();
 * close the output stream if opened 
 */
static void
pdf_close_file(PDF *p)
{
#ifdef MAC
    FCBPBRec	fcbInfo;
    Str32	name;
    FInfo	fInfo;
    FSSpec	fSpec;
#endif

    if (p->fp == NULL)
	return;

#ifdef MAC
/*  Contributed by Leonard Rosenthol:
 *  On the MacOS, files are not associated with applications by extensions.
 *  Instead, it uses a pair of values called the type & creator.
 *  This block of code sets those values for PDF files.
 */
    memset(&fcbInfo, 0, sizeof(FCBPBRec));
    fcbInfo.ioRefNum = (short) p->fp->handle;
    fcbInfo.ioNamePtr = name;
    if (!PBGetFCBInfoSync(&fcbInfo) &&
	FSMakeFSSpec(fcbInfo.ioFCBVRefNum, fcbInfo.ioFCBParID, name, &fSpec)
		== noErr) {
	    FSpGetFInfo(&fSpec, &fInfo);
	    fInfo.fdType = 'PDF ';
	    fInfo.fdCreator = 'CARO';
	    FSpSetFInfo(&fSpec, &fInfo);
	}
#endif

    /*
     * If filename is set, we started with PDF_open_file; therefore
     * we also close the file ourselves.
     */
    if (p->filename && p->writeproc) {
	if (strcmp(p->filename, "-"))
	    fclose(p->fp);
	p->free(p, p->filename);
    }

    /* mark fp as dead in case the error handler jumps in later */
    p->fp = NULL;
}

/* 
 * PDF_delete must be called for cleanup in case of error,
 * or when the client is done producing PDF.
 * It should never be called more than once for a given PDF, although
 * we try to guard against duplicated calls.
 *
 * Note: all pdf_cleanup_*() functions may safely be called multiple times.
 */

PDFLIB_API void PDFLIB_CALL
PDF_delete(PDF *p)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    /*
     * Clean up page-related stuff if necessary. Do not raise
     * an error here since we may be called from the error handler
     */
    if (p->state != pdf_state_null) {
	if (!p->in_error)
	    pdf_error(p, PDF_NonfatalError, "Didn't close the document");
	PDF_close(p);
    }

    /* close the output stream.
     * This can't be done in PDF_close() because the caller may fetch
     * the buffer only after PDF_close()ing the document.
     */
    pdf_close_stream(p);

    if (p->binding)
	p->free(p, p->binding);

    if (p->prefix)
	p->free(p, p->prefix);

    /* free the PDF structure and try to protect against duplicated calls */

    p->magic = 0L;		/* we don't reach this with the wrong magic */
    p->free(p, (void *)p);
}

static void
pdf_init_all(PDF *p)
{
    pdf_init_document(p);
    pdf_init_info(p);
    pdf_init_images(p);
    pdf_init_xobjects(p);
    pdf_init_fonts(p);
    pdf_init_transition(p);
    pdf_init_outlines(p);
    pdf_init_annots(p);
    pdf_init_stream(p);
}

/* Must be defined octal to protect it from EBCDIC compilers */
#define PDF_MAGIC_BINARY "\045\344\343\317\322\012"

static void
pdf_write_header(PDF *p)
{
    /* Write the document header */
    /*
     * Although the %PDF-1.3 header also works with older Acrobat viewers,
     * we need a 1.2 header for special applications (controlled by the
     * client).
    */

    if (p->compatibility == PDF_1_3)
	pdf_puts(p, "%PDF-1.3\n");	      	/* PDF 1.3 header */
    else
	pdf_puts(p, "%PDF-1.2\n");	      	/* PDF 1.2 header */

    /* binary magic number */
    pdf_write(p, PDF_MAGIC_BINARY, sizeof(PDF_MAGIC_BINARY) - 1);
}

PDFLIB_API int PDFLIB_CALL
PDF_open_fp(PDF *p, FILE *fp)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return -1;

    if (fp == NULL)
	return -1;

/*
 * It is the callers responsibility to open the file in binary mode,
 * but it doesn't hurt to make sure it really is.
 * The Intel version of the Metrowerks compiler doesn't have setmode().
 */
#if !defined(__MWERKS__) && (defined(WIN32) || defined(OS2))
    setmode(fileno(fp), O_BINARY);
#endif

    pdf_init_all(p);
    p->writeproc	= pdf_writeproc_file;
    p->fp		= fp;
    p->filename		= NULL;		/* marker to remember not to close fp */

    pdf_write_header(p);

    return pdf_true;
}

PDFLIB_API int PDFLIB_CALL
PDF_open_file(PDF *p, const char *filename)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return -1;

    pdf_init_all(p);

    if (filename == NULL || *filename == '\0') {
	/* no file name supplied ==> in-core PDF generation requested */
	p->writeproc = NULL;
	p->filename = NULL;

    } else {
#ifndef MAC
	if (filename && !strcmp(filename, "-")) {
	    p->filename = NULL;
	    p->fp = stdout;
#if !defined(__MWERKS__) && (defined(WIN32) || defined(OS2))
	    setmode(fileno(stdout), O_BINARY);
#endif
	} else
#endif /* MAC */

	if ((p->fp = fopen(filename, WRITEMODE)) == NULL)
	    return -1;

	p->writeproc = pdf_writeproc_file;
	p->filename = pdf_strdup(p, filename);
    }

    pdf_write_header(p);

    return pdf_true;
}

PDFLIB_API void PDFLIB_CALL
PDF_open_mem(PDF *p, size_t (*writeproc)(PDF *p, void *data, size_t size))
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (writeproc == NULL)
	pdf_error(p, PDF_ValueError, "NULL write procedure in PDF_open_mem()");
	
    p->writeproc = writeproc;
    p->filename = NULL;

    pdf_init_all(p);
    pdf_write_header(p);
}

/* Write all pending document information up to the xref table and trailer */
static void
pdf_wrapup_document(PDF *p)
{
    long	pos;
    int		page;
    id		i;

    if (p->current_page == 0 ) {		/* avoid empty document */
	/* create 1-page document with arbitrary page size */
	PDF_begin_page(p, (float) 100, (float) 100);
	PDF_end_page(p);
	pdf_error(p, PDF_NonfatalError, "Empty document");
    }

    pdf_write_info(p);

    pdf_write_doc_fonts(p);			/* font objects */

    pdf_begin_obj(p, p->pages_id);		/* root pages object */
    pdf_begin_dict(p);
    pdf_puts(p, "/Type/Pages\n");
    pdf_printf(p, "/Count %d\n", p->current_page);

    pdf_puts(p, "/Kids[");
    for (page = 1; page <= p->current_page; page++) {
	pdf_printf(p, "%ld 0 R", p->pages[page]);
	pdf_putc(p, (char)(page % 8 ? PDF_SPACE : PDF_NEWLINE));
    }
    pdf_puts(p, "]\n");

    pdf_end_dict(p);
    pdf_end_obj(p);

    pdf_begin_obj(p, p->root_id);		/* Catalog or Root object */
    pdf_begin_dict(p);
    pdf_puts(p, "/Type/Catalog\n");

    /* 
     * specify the open action (display of the first page) 
     * default = retain: top of the first page at default zoom level
     */
    if (p->open_action == fitpage) {
	pdf_printf(p, "/OpenAction[%ld 0 R/Fit]\n", p->pages[1]);

    } else if (p->open_action == fitwidth) {
	pdf_printf(p, "/OpenAction[%ld 0 R/FitH]\n", p->pages[1]);

    } else if (p->open_action == fitheight) {
	pdf_printf(p, "/OpenAction[%ld 0 R/FitV 0]\n", p->pages[1]);

    } else if (p->open_action == fitbbox) {
	pdf_printf(p, "/OpenAction[%ld 0 R/FitB]\n", p->pages[1]);
    }

    /* 
     * specify the document's open mode
     * default = open_none: open document with neither bookmarks nor 
     * thumbnails visible
     */
    if (p->open_mode == open_bookmarks) {
	pdf_printf(p, "/PageMode/UseOutlines\n");

    } else if (p->open_mode == open_thumbnails) {
	pdf_printf(p, "/PageMode/UseThumbs\n");

    } else if (p->open_mode == open_fullscreen) {
	pdf_printf(p, "/PageMode/FullScreen\n");
    }

    						/* Pages object */
    pdf_printf(p, "/Pages %ld 0 R\n", p->pages_id);
    if (p->outline_count != 0)
	pdf_printf(p, "/Outlines %ld 0 R\n", p->outlines[0].self);

    pdf_end_dict(p);
    pdf_end_obj(p);

    pdf_write_outlines(p);

#ifdef DEBUG
    if (p->debug['s']) {
	fprintf(stderr, "PDF document statistics:\n");
	fprintf(stderr, "    %d pages\n", p->current_page);
	fprintf(stderr, "    %d fonts\n", p->fonts_number);
	fprintf(stderr, "    %d xobjects\n", p->xobjects_number);
	fprintf(stderr, "    %ld objects\n", p->currentobj + 1);
    }
#endif

    /* Don't write any object after this check! */
    for (i = 1; i <= p->currentobj; i++) {
	if (p->file_offset[i] == BAD_ID) {
	    pdf_error(p, PDF_NonfatalError, "Object %ld allocated but not used", i);
	    pdf_begin_obj(p, i);
	    pdf_end_obj(p);
	}
    }

    pos = pdf_tell(p);				/* xref table */
    pdf_puts(p, "xref\n");
    pdf_printf(p, "0 %ld\n", p->currentobj + 1);
    pdf_puts(p, "0000000000 65535 f \n");
    for (i = 1; i <= p->currentobj; i++) {
	pdf_printf(p, "%010ld 00000 n \n", p->file_offset[i]);
    }

    pdf_puts(p, "trailer\n");

    pdf_begin_dict(p);				/* trailer */
    pdf_printf(p, "/Size %ld\n", p->currentobj + 1);
    pdf_printf(p, "/Info %ld 0 R\n", p->info_id);
    pdf_printf(p, "/Root %ld 0 R\n", p->root_id);
    pdf_end_dict(p);				/* trailer */

    pdf_puts(p, "startxref\n");
    pdf_printf(p, "%ld\n", pos);
    pdf_puts(p, "%%EOF\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_close(PDF *p)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->state == pdf_state_null)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_close)");

    if (p->state == pdf_state_page_description && !p->in_error) {
	pdf_error(p, PDF_NonfatalError,
		"Didn't call PDF_end_page before PDF_close");
	PDF_end_page(p);
    }

    if (!p->in_error)
	pdf_wrapup_document(p);	/* dump the remaining PDF structures */

    pdf_flush_stream(p);

    /* close the output file if writing to file... */
    if (p->writeproc)
	pdf_close_file(p);
    
    /*
     * ...but do not close the output stream since the caller will have to
     * fetch the buffer after PDF_close().
     */

    /* clean up all document-related stuff */
    pdf_cleanup_document(p);
    pdf_cleanup_info(p);
    pdf_cleanup_images(p);
    pdf_cleanup_xobjects(p);
    pdf_cleanup_fonts(p);
    pdf_cleanup_outlines(p);
    pdf_cleanup_resources(p);

    p->state = pdf_state_null;
}

void
pdf_begin_contents_section(PDF *p)
{
    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_SystemError,
		"Tried to start contents section outside page description");

    if (p->contents != c_none)
	return;

    if (p->next_content >= p->contents_ids_capacity) {
	p->contents_ids_capacity *= 2;
	p->contents_ids = (id *) p->realloc(p, p->contents_ids,
			    sizeof(long) * p->contents_ids_capacity,
			    "pdf_begin_contents_section");
    }

    p->contents_ids[p->next_content] = pdf_begin_obj(p, NEW_ID);
    p->contents	= c_page;
    pdf_begin_dict(p);
    p->contents_length_id = pdf_alloc_id(p);
    pdf_printf(p, "/Length %ld 0 R\n", p->contents_length_id);

#ifdef HAVE_LIBZ
    if (p->compress)
	pdf_puts(p, "/Filter/FlateDecode\n");
#endif

    pdf_end_dict(p);

    pdf_begin_stream(p);

#ifdef HAVE_LIBZ
    if (p->compress)
	pdf_compress_init(p);
#endif
    							/* Contents object */
    p->start_contents_pos = pdf_tell(p);
    p->next_content++;
}

void
pdf_end_contents_section(PDF *p)
{
    long length;

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_SystemError,
		"Tried to end contents section outside page description");

    if (p->contents == c_none)
	return;

    pdf_end_text(p);
    p->contents = c_none;

#ifdef HAVE_LIBZ
    if (p->compress)
	pdf_compress_end(p);
#endif
						/* Contents object */
    length = pdf_tell(p) - p->start_contents_pos;
    pdf_end_stream(p);
    pdf_end_obj(p);

    pdf_begin_obj(p, p->contents_length_id);	/* Length object */
    pdf_printf(p, "%ld\n", length);
    pdf_end_obj(p);
}

#define ACRO3_MINPAGE	(float) 72		/* 1 inch = 2.54 cm */
#define ACRO3_MAXPAGE	(float) 3240		/* 45 inch = 114,3 cm */
#define ACRO4_MINPAGE	(float) 18		/* 0.25 inch = 0.635 cm */
#define ACRO4_MAXPAGE	(float) 14400		/* 200  inch = 508 cm */

PDFLIB_API void PDFLIB_CALL
PDF_begin_page(PDF *p, float width, float height)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->state != pdf_state_open)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_begin_page)");
    
    if (width <= 0 || height <= 0)
	pdf_error(p, PDF_ValueError, "Page size must be positive");

    if (p->compatibility == PDF_1_3 &&
	(height < ACRO4_MINPAGE || width < ACRO4_MINPAGE ||
	height > ACRO4_MAXPAGE || width > ACRO4_MAXPAGE))
	pdf_error(p, PDF_NonfatalError, "Page size incompatible with Acrobat 3/4");

    else if (p->compatibility == PDF_1_2 &&
	(height < ACRO3_MINPAGE || width < ACRO3_MINPAGE ||
	height > ACRO3_MAXPAGE || width > ACRO3_MAXPAGE))
	pdf_error(p, PDF_RuntimeError, "Page size incompatible with Acrobat 3");

    if (++(p->current_page) >= p->pages_capacity)
	pdf_grow_pages(p);

    /* no id has been preallocated */
    if (p->pages[p->current_page] == BAD_ID)
	p->pages[p->current_page] = pdf_alloc_id(p);

    p->height		= height;
    p->width		= width;
    p->thumb_id		= BAD_ID;
    p->state		= pdf_state_page_description;
    p->next_content	= 0;
    p->contents 	= c_none;
    p->procset		= 0;
    p->sl		= 0;

    pdf_init_page_annots(p);

    pdf_init_tstate(p);
    pdf_init_gstate(p);
    pdf_init_cstate(p);

    pdf_begin_contents_section(p);
}

/* This array must be kept in sync with the pdf_transition enum in p_intern.h */
static const char *pdf_transition_names[] = {
    "", "Split", "Blinds", "Box", "Wipe", "Dissolve", "Glitter", "R"
};

PDFLIB_API void PDFLIB_CALL
PDF_end_page(PDF *p)
{
    int		index;
    char	buf[FLOATBUFSIZE], buf2[FLOATBUFSIZE];
    pdf_annot	*ann;

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_end_page)");

    pdf_end_contents_section(p);

    /* Page object */
    pdf_begin_obj(p, p->pages[p->current_page]);

    pdf_begin_dict(p);
    pdf_puts(p, "/Type/Page\n");
    pdf_printf(p, "/Parent %ld 0 R\n", p->pages_id);

    p->res_id = pdf_alloc_id(p);
    pdf_printf(p, "/Resources %ld 0 R\n", p->res_id);

    pdf_printf(p, "/MediaBox[0 0 %s %s]\n",
	    pdf_float(buf, p->width), pdf_float(buf2, p->height));

    /*
     * The duration can be placed in the page dictionary (/D) or the
     * the transition dictionary (/Dur). We put it here so it can
     * be used without setting a transition effect.
     */

    if (p->duration > 0)
	pdf_printf(p, "/Dur %s\n", pdf_float(buf, p->duration));

    if (p->transition != trans_none) {
	pdf_puts(p, "/Trans");
	pdf_begin_dict(p);
	    pdf_printf(p, "/S/%s", pdf_transition_names[p->transition]);

	pdf_end_dict(p);
    }

    pdf_puts(p, "/Contents[");
    for (index = 0; index < p->next_content; index++) {
	pdf_printf(p, "%ld 0 R", p->contents_ids[index]);
	pdf_putc(p, (char) (index + 1 % 8 ? PDF_SPACE : PDF_NEWLINE));
    }
    pdf_puts(p, "]\n");

    /* Thumbnail image */
    if (p->thumb_id != BAD_ID)
	pdf_printf(p, "/Thumb %ld 0 R\n", p->thumb_id);

    /* Annotations array */
    if (p->annots) {
	pdf_puts(p, "/Annots[");

	for (ann = p->annots; ann != NULL; ann = ann->next) {
	    ann->obj_id = pdf_alloc_id(p);
	    pdf_printf(p, "%ld 0 R ", ann->obj_id);
	}

	pdf_puts(p, "]\n");
    }

    pdf_end_dict(p);		/* Page object */
    pdf_end_obj(p);

    pdf_write_page_annots(p);	/* Annotation dicts */

    /* Resource object */
    
    pdf_begin_obj(p, p->res_id);
    pdf_begin_dict(p);		/* Resource dict */

    /* ProcSet resources */

    pdf_puts(p, "/ProcSet[/PDF");
    if ( p->procset & ImageB)
	pdf_puts(p, "/ImageB");
    if ( p->procset & ImageC)
	pdf_puts(p, "/ImageC");
    if ( p->procset & ImageI)
	pdf_puts(p, "/ImageI");
    if ( p->procset & Text)
	pdf_puts(p, "/Text");
    pdf_puts(p, "]\n");

    /* Font resources */
    pdf_write_page_fonts(p);

    /* XObject resources */
    pdf_write_xobjects(p);

    pdf_end_dict(p);	/* resource dict */
    pdf_end_obj(p);	/* resource object */

    pdf_cleanup_page(p);

    p->state	= pdf_state_open;

    if (p->stream.flush & PDF_FLUSH_PAGE)
	pdf_flush_stream(p);
}

id
pdf_begin_obj(PDF *p, id obj_id)
{
    if (obj_id == NEW_ID)
	obj_id = pdf_alloc_id(p);

    p->file_offset[obj_id] = pdf_tell(p); 
    pdf_printf(p, "%ld 0 obj\n", obj_id);
    return obj_id;
}

id
pdf_alloc_id(PDF *p)
{
    p->currentobj++;

    if (p->currentobj >= p->file_offset_capacity) {
	p->file_offset_capacity *= 2;
	p->file_offset = (long *) p->realloc(p, p->file_offset,
		    sizeof(long) * p->file_offset_capacity, "pdf_alloc_id");
    }

    /* only needed for verifying obj table in PDF_close() */
    p->file_offset[p->currentobj] = BAD_ID;

    return p->currentobj;
}

void
pdf_grow_pages(PDF *p)
{
    int i;

    p->pages_capacity *= 2;
    p->pages = (id *) p->realloc(p, p->pages,
		sizeof(id) * p->pages_capacity, "pdf_grow_pages");
    for (i = p->current_page; i < p->pages_capacity; i++)
	p->pages[i] = BAD_ID;
}

/* --------------- PDFlib parameter handling ---------------------- */

/*
 * PDF_get_parameter() and PDF_set_parameter() deal with strings,
 * PDF_get_value() and PDF_set_value() deal with numerical values.
 */

/* setup */
#define PDF_PARAMETER_COMPRESS		"compress"	/* set_parameter */
#define PDF_PARAMETER_FLUSH		"flush"		/* set_parameter */
#define PDF_PARAMETER_RESOURCEFILE	"resourcefile"	/* set_parameter */
#define PDF_PARAMETER_DEBUG		"debug"		/* set_parameter */
#define PDF_PARAMETER_NODEBUG		"nodebug"	/* set_parameter */
#define PDF_PARAMETER_COMPATIBILITY	"compatibility"	/* set_parameter */
#define PDF_PARAMETER_PREFIX		"prefix"	/* set_parameter */
#define PDF_PARAMETER_WARNING		"warning"	/* set_parameter */
#define PDF_PARAMETER_BINDING		"binding"	/* internal use only */
#define PDF_PARAMETER_INERROR		"inerror"	/* internal use only */

/* page */
#define PDF_PARAMETER_PAGEWIDTH		"pagewidth"	/* set_value */
#define PDF_PARAMETER_PAGEHEIGHT	"pageheight"	/* set_value */

/* font and text */
#define PDF_PARAMETER_FONTAFM		"FontAFM"	/* set_parameter */
#define PDF_PARAMETER_FONTPFM		"FontPFM"	/* set_parameter */
#define PDF_PARAMETER_FONTOUTLINE	"FontOutline"	/* set_parameter */
#define PDF_PARAMETER_ENCODING		"Encoding"	/* set_parameter */
#define PDF_PARAMETER_TEXTX		"textx"		/* get_value */
#define PDF_PARAMETER_TEXTY		"texty"		/* get_value */
#define PDF_PARAMETER_FONT		"font"		/* get_value */
#define PDF_PARAMETER_FONTSIZE		"fontsize"	/* get_value */
#define PDF_PARAMETER_LEADING		"leading"	/* set_value */
#define PDF_PARAMETER_TEXTRISE		"textrise"	/* set_value */
#define PDF_PARAMETER_HORIZSCALING	"horizscaling"	/* set_value */
#define PDF_PARAMETER_TEXTRENDERING	"textrendering"	/* set_value */
#define PDF_PARAMETER_CHARSPACING	"charspacing"	/* set_value */
#define PDF_PARAMETER_WORDSPACING	"wordspacing"	/* set_value */
#define PDF_PARAMETER_UNDERLINE		"underline"	/* set_parameter */
#define PDF_PARAMETER_OVERLINE		"overline"	/* set_parameter */
#define PDF_PARAMETER_STRIKEOUT		"strikeout"	/* set_parameter */
#define PDF_PARAMETER_FONTNAME		"fontname"	/* get_parameter */
#define PDF_PARAMETER_FONTENCODING	"fontencoding"	/* get_parameter */
#define PDF_PARAMETER_CAPHEIGHT		"capheight"	/* get_value */
#define PDF_PARAMETER_ASCENDER		"ascender"	/* get_value */
#define PDF_PARAMETER_DESCENDER		"descender"	/* get_value */
#define PDF_PARAMETER_NATIVEUNICODE	"nativeunicode"	/* set_parameter */

/* graphics */
#define PDF_PARAMETER_CURRENTX		"currentx"	/* get_value */
#define PDF_PARAMETER_CURRENTY		"currenty"	/* get_value */
#define PDF_PARAMETER_FILLRULE		"fillrule"	/* set_parameter */

/* image */
#define PDF_PARAMETER_IMAGEWARNING	"imagewarning"	/* set_parameter */
#define PDF_PARAMETER_PASSTHROUGH	"passthrough"	/* set_parameter */
#define PDF_PARAMETER_IMAGEWIDTH	"imagewidth"	/* get_value */
#define PDF_PARAMETER_IMAGEHEIGHT	"imageheight"	/* get_value */
#define PDF_PARAMETER_RESX		"resx"		/* get_value */
#define PDF_PARAMETER_RESY		"resy"		/* get_value */

/* hypertext */
#define PDF_PARAMETER_OPENACTION	"openaction"	/* set_parameter */
#define PDF_PARAMETER_OPENMODE		"openmode"	/* set_parameter */
#define PDF_PARAMETER_BOOKMARKDEST	"bookmarkdest"	/* set_parameter */
#define PDF_PARAMETER_TRANSITION	"transition"	/* set_parameter */
#define PDF_PARAMETER_DURATION		"duration"	/* set_value */


PDFLIB_API void PDFLIB_CALL
PDF_set_parameter(PDF *p, const char *key, const char *value)
{
    const unsigned char *c;
    char *filename, *resource;

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (key == NULL || !*key || value == NULL || !*value)
	pdf_error(p, PDF_ValueError, "Tried to set NULL parameter");

    /* file name of the resource file */
    if (!strcmp(key, PDF_PARAMETER_RESOURCEFILE)) {
	/* discard old resource file name, if any */
	if (p->resourcefilename)
	    p->free(p, p->resourcefilename);
	p->resourcefilename = pdf_strdup(p, value);
	return;

    /* add a single resource line */
    } else if ( !strcmp(key, PDF_PARAMETER_FONTAFM) ||
    		!strcmp(key, PDF_PARAMETER_FONTPFM) ||
		!strcmp(key, PDF_PARAMETER_FONTOUTLINE) ||
		!strcmp(key, PDF_PARAMETER_ENCODING)) {
	    /* don't manipulate the caller's buffer */
	    resource = pdf_strdup(p, value);

	    if ((filename = strchr(resource, '=')) == NULL) {
		p->free(p, resource);
		pdf_error(p, PDF_ValueError, "Bogus resource line");
		return;
	    }
	    *filename++ = '\0';
	    pdf_add_resource(p, key, resource, filename, NULL);
	    p->free(p, resource);
	    return;

    /* set flush point */
    } else if (!strcmp(key, PDF_PARAMETER_FLUSH)) {
	if (!strcmp(value, "none"))
	    p->stream.flush |= PDF_FLUSH_NONE;
	else if (!strcmp(value, "page"))
	    p->stream.flush |= PDF_FLUSH_PAGE;
	else if (!strcmp(value, "content"))
	    p->stream.flush |= PDF_FLUSH_CONTENT;
	else if (!strcmp(value, "heavy"))
	    p->stream.flush |= PDF_FLUSH_HEAVY;
	else
	    pdf_error(p, PDF_ValueError, "Bogus flush point %s", value);

	return;

    /* activate debug flags */
    } else if (!strcmp(key, PDF_PARAMETER_DEBUG)) {
	for (c = (const unsigned char *) value; *c; c++)
	    p->debug[(int) *c] = 1;
	if (*value == 'c')
	    p->compress = 0;
	return;

    /* deactivate debug flags */
    } else if (!strcmp(key, PDF_PARAMETER_NODEBUG)) {
	for (c = (const unsigned char *) value; *c; c++)
	    p->debug[(int) *c] = 0;
#ifdef HAVE_LIBZ
	if (*value == 'c')
	    p->compress = PDF_DEFAULT_COMPRESSION;
#endif
	return;

    /* set name of the language binding in use */
    } else if (!strcmp(key, PDF_PARAMETER_BINDING)) {
	if (!p->binding)
	    p->binding = pdf_strdup(p, value);

    /* an error occured in one of the language wrappers */
    } else if (!strcmp(key, PDF_PARAMETER_INERROR)) {
	p->in_error = pdf_true;

    /* underlined text */
    } else if (!strcmp(key, PDF_PARAMETER_UNDERLINE)) {
	if (!strcmp(value, "true")) {
	    p->underline = pdf_true;
	    return;
	} else if (!strcmp(value, "false")) {
	    p->underline = pdf_false;
	    return;
	} else {
	    pdf_error(p, PDF_ValueError, "Unknown underline mode %s", value);
	}

    /* overlined text */
    } else if (!strcmp(key, PDF_PARAMETER_OVERLINE)) {
	if (!strcmp(value, "true")) {
	    p->overline = pdf_true;
	    return;
	} else if (!strcmp(value, "false")) {
	    p->overline = pdf_false;
	    return;
	} else {
	    pdf_error(p, PDF_ValueError, "Unknown overline mode %s", value);
	}

    /* striketrough text */
    } else if (!strcmp(key, PDF_PARAMETER_STRIKEOUT)) {
	if (!strcmp(value, "true")) {
	    p->strikeout = pdf_true;
	    return;
	} else if (!strcmp(value, "false")) {
	    p->strikeout = pdf_false;
	    return;
	} else {
	    pdf_error(p, PDF_ValueError, "Unknown strikeout mode %s", value);
	}

    /* compatiblity level */
    } else if (!strcmp(key, PDF_PARAMETER_COMPATIBILITY)) {
	if (p->state != pdf_state_null || p->current_page > 0)
	    pdf_error(p, PDF_RuntimeError,
	    	"Must change compatiblity level before PDF_open()");

	if (!strcmp(value, "1.2"))
	    p->compatibility = PDF_1_2;
	else if (!strcmp(value, "1.3"))
	    p->compatibility = PDF_1_3;
	else
	    pdf_error(p, PDF_ValueError, "Unknown compatibility level '%s'",
	    	value);

	return;

    /* resource file name prefix */
    } else if (!strcmp(key, PDF_PARAMETER_PREFIX)) {
	if (!value || !*value)
	    pdf_error(p, PDF_ValueError, "Empty resource prefix");

	p->prefix = pdf_strdup(p, value);

	return;

    /* enable or disable warning messages/exceptions */
    } else if (!strcmp(key, PDF_PARAMETER_WARNING)) {
	if (!strcmp(value, "true")) {
	    p->debug['w'] = pdf_true;
	} else if (!strcmp(value, "false")) {
	    p->debug['w'] = pdf_false;
	} else {
	    pdf_error(p, PDF_ValueError, "Unknown warning mode '%s'", value);
	}

	return;

    /* activate warnings about image problems */
    } else if (!strcmp(key, PDF_PARAMETER_IMAGEWARNING)) {
	if (!strcmp(value, "true")) {
	    p->debug['i'] = pdf_true;
	} else if (!strcmp(value, "false")) {
	    p->debug['i'] = pdf_false;
	} else {
	    pdf_error(p, PDF_ValueError, "Unknown image warning mode '%s'", value);
	}

	return;

    /* deactivate image pass-through mode */
    } else if (!strcmp(key, PDF_PARAMETER_PASSTHROUGH)) {
	if (!strcmp(value, "false")) {
	    p->debug['p'] = pdf_true;
	} else if (!strcmp(value, "true")) {
	    p->debug['p'] = pdf_false;
	} else {
	    pdf_error(p, PDF_ValueError, "Unknown pass-through mode '%s'", value);
	}

	return;

    /* set the open action for the document (default zoom) */
    } else if (!strcmp(key, PDF_PARAMETER_OPENACTION)) {
	if (!strcmp(value, "retain")) {
	    p->open_action = retain;
	} else if (!strcmp(value, "fitpage")) {
	    p->open_action = fitpage;
	} else if (!strcmp(value, "fitwidth")) {
	    p->open_action = fitwidth;
	} else if (!strcmp(value, "fitheight")) {
	    p->open_action = fitheight;
	} else if (!strcmp(value, "fitbbox")) {
	    p->open_action = fitbbox;
	} else {
	    pdf_error(p, PDF_ValueError, "Unknown open action '%s'", value);
	}

	return;

    /* set the document's open mode */
    } else if (!strcmp(key, PDF_PARAMETER_OPENMODE)) {
	if (!strcmp(value, "none")) {
	    p->open_mode = open_none;
	} else if (!strcmp(value, "bookmarks")) {
	    p->open_mode = open_bookmarks;
	} else if (!strcmp(value, "thumbnails")) {
	    p->open_mode = open_thumbnails;
	} else if (!strcmp(value, "fullscreen")) {
	    p->open_mode = open_fullscreen;
	} else {
	    pdf_error(p, PDF_ValueError, "Unknown open mode '%s'", value);
	}

	return;

    /* set the destination (target) for bookmarks */
    } else if (!strcmp(key, PDF_PARAMETER_BOOKMARKDEST)) {
	if (!strcmp(value, "retain")) {
	    p->bookmark_dest = retain;
	} else if (!strcmp(value, "fitpage")) {
	    p->bookmark_dest = fitpage;
	} else if (!strcmp(value, "fitwidth")) {
	    p->bookmark_dest = fitwidth;
	} else if (!strcmp(value, "fitheight")) {
	    p->bookmark_dest = fitheight;
	} else if (!strcmp(value, "fitbbox")) {
	    p->bookmark_dest = fitbbox;
	} else {
	    pdf_error(p, PDF_ValueError,
	    	"Unknown bookmark destination '%s'", value);
	}

	return;

    /* set the algorithm for area fills */
    } else if (!strcmp(key, PDF_PARAMETER_FILLRULE)) {
	if (!strcmp(value, "winding")) {
	    p->fillrule = pdf_fill_winding;
	} else if (!strcmp(value, "evenodd")) {
	    p->fillrule = pdf_fill_evenodd;
	} else {
	    pdf_error(p, PDF_ValueError, "Unknown fill rule '%s'", value);
	}

	return;

    /*
     * check native unicode mode: this will be handled in the wrapper code
     * of those language bindings which natively support Unicode. We simply
     * catch typos in the boolean value here, or complain for all non-Unicode
     * aware language bindings.
     */
    } else if (!strcmp(key, PDF_PARAMETER_NATIVEUNICODE)) {
	if (!strcmp(value, "true") || !strcmp(value, "false")) {
	    pdf_error(p, PDF_NonfatalError,
	    	"Unicode mode setting not required in this configuration");
	} else {
	    pdf_error(p, PDF_ValueError, "Unknown Unicode mode setting '%s'", value);
	}

	return;

    /* set the page transition effect */
    } else if (!strcmp(key, PDF_PARAMETER_TRANSITION)) {
	pdf_set_transition(p, value);
	return;

    /* unknown parameter */
    } else {
	pdf_error(p, PDF_ValueError,
		"Tried to set unknown parameter '%s'", key);
    }
}

PDFLIB_API void PDFLIB_CALL
PDF_set_value(PDF *p, const char *key, float value)
{
    int ivalue;

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (key == NULL || !*key)
	pdf_error(p, PDF_ValueError, "Tried to set NULL parameter");

    /* compression level */
    if (!strcmp(key, PDF_PARAMETER_COMPRESS)) {
	ivalue = (int) value;

	if (ivalue < 0 || ivalue > 9)
	    pdf_error(p, PDF_ValueError, "Bogus compression level %f", value);

	/* we must restart the compression engine if we're in the middle of a page */
	if (p->state == pdf_state_page_description)
	    pdf_end_contents_section(p);

	p->compress = ivalue;

	if (p->state == pdf_state_page_description)
	    pdf_begin_contents_section(p);

	return;

    /* page width */
    } else if (!strcmp(key, PDF_PARAMETER_PAGEWIDTH)) {
	if (p->state != pdf_state_page_description)
	    pdf_error(p, PDF_RuntimeError,
	    	"Can't change page width outside page description");

	if (value > 0)
	    p->width = value;
	else
	    pdf_error(p, PDF_ValueError, "Bogus page width %f", value);

	return;

    /* page height */
    } else if (!strcmp(key, PDF_PARAMETER_PAGEHEIGHT)) {
	if (p->state != pdf_state_page_description)
	    pdf_error(p, PDF_RuntimeError,
	    	"Can't change page height outside page description");

	if (value > 0)
	    p->height = value;
	else
	    pdf_error(p, PDF_ValueError, "Bogus page height %f", value);

	return;

    /* leading */
    } else if (!strcmp(key, PDF_PARAMETER_LEADING)) {
	pdf_set_leading(p, value);
	return;

    /* text rise */
    } else if (!strcmp(key, PDF_PARAMETER_TEXTRISE)) {
	pdf_set_text_rise(p, value);
	return;

    /* horizontal scaling */
    } else if (!strcmp(key, PDF_PARAMETER_HORIZSCALING)) {
	pdf_set_horiz_scaling(p, value);
	return;

    /* text rendering */
    } else if (!strcmp(key, PDF_PARAMETER_TEXTRENDERING)) {
	pdf_set_text_rendering(p, (int) value);
	return;

    /* character spacing */
    } else if (!strcmp(key, PDF_PARAMETER_CHARSPACING)) {
	pdf_set_char_spacing(p, value);
	return;

    /* word spacing */
    } else if (!strcmp(key, PDF_PARAMETER_WORDSPACING)) {
	pdf_set_word_spacing(p, value);
	return;

    /* duration of a page transition */
    } else if (!strcmp(key, PDF_PARAMETER_DURATION)) {
	pdf_set_duration(p, value);
	return;

    /* unknown parameter */
    } else {
	pdf_error(p, PDF_ValueError,
		"Tried to set unknown parameter '%s'", key);
    }
}

PDFLIB_API float PDFLIB_CALL
PDF_get_value(PDF *p, const char *key, float mod)
{
    int imod;

    if (PDF_SANITY_CHECK_FAILED(p))
	return (float) 0;

    if (key == NULL || !*key)
	pdf_error(p, PDF_ValueError, "Tried to get NULL parameter");

    if (!strcmp(key, PDF_PARAMETER_IMAGEWIDTH)) {
	imod = (int) mod;

	if (imod < 0 || imod >= p->images_capacity || !p->images[imod].in_use) {
	    pdf_error(p, PDF_ValueError,
		    "Bad image number %d in PDF_get_value/%s", imod, key);
	} else
	    return (float) (p->images[imod].width);

    } else if (!strcmp(key, PDF_PARAMETER_IMAGEHEIGHT)) {
	imod = (int) mod;

	if (imod < 0 || imod >= p->images_capacity || !p->images[imod].in_use) {
	    pdf_error(p, PDF_ValueError,
		    "Bad image number %d in PDF_get_value/%s", imod, key);
	} else
	    return (float) (p->images[imod].height);

    } else if (!strcmp(key, PDF_PARAMETER_RESX)) {
	imod = (int) mod;

	if (imod < 0 || imod >= p->images_capacity || !p->images[imod].in_use) {
	    pdf_error(p, PDF_ValueError,
		    "Bad image number %d in PDF_get_value/%s", imod, key);
	} else
	    return p->images[imod].dpi_x;

    } else if (!strcmp(key, PDF_PARAMETER_RESY)) {
	imod = (int) mod;

	if (imod < 0 || imod >= p->images_capacity || !p->images[imod].in_use) {
	    pdf_error(p, PDF_ValueError,
		    "Bad image number %d in PDF_get_value/%s", imod, imod);
	} else
	    return p->images[imod].dpi_y;

    } else if (!strcmp(key, PDF_PARAMETER_CURRENTX)) {
	if (mod != (float) 0 ) {
	    pdf_error(p, PDF_ValueError,
		    "Bad argument %f in PDF_get_value/%s", mod, key);
	} else
	    return p->gstate[p->sl].x;

    } else if (!strcmp(key, PDF_PARAMETER_CURRENTY)) {
	if (mod != (float) 0 ) {
	    pdf_error(p, PDF_ValueError,
		    "Bad argument %f in PDF_get_value/%s", mod, key);
	} else
	    return p->gstate[p->sl].y;

    } else if (!strcmp(key, PDF_PARAMETER_TEXTX)) {
	if (mod != (float) 0 ) {
	    pdf_error(p, PDF_ValueError,
		    "Bad argument %f in PDF_get_value/%s", mod, key);
	} else
	    return p->tstate[p->sl].m.e;

    } else if (!strcmp(key, PDF_PARAMETER_TEXTY)) {
	if (mod != (float) 0 ) {
	    pdf_error(p, PDF_ValueError,
		    "Bad argument %f in PDF_get_value/%s", mod, key);
	} else
	    return p->tstate[p->sl].m.f;

    } else if (!strcmp(key, PDF_PARAMETER_FONT)) {
	if (mod != (float) 0 ) {
	    pdf_error(p, PDF_ValueError,
		    "Bad argument %f in PDF_get_value/%s", mod, key);
	} else {
	    return (float) pdf_get_font(p);
	}

    } else if (!strcmp(key, PDF_PARAMETER_FONTSIZE)) {
	imod = (int) mod;
	if (imod != (float) 0 ) {
	    pdf_error(p, PDF_ValueError,
		    "Bad argument %f in PDF_get_value/%s", mod, key);
	} else {
	    return (float) pdf_get_fontsize(p);
	}

    } else if (!strcmp(key, PDF_PARAMETER_CAPHEIGHT)) {
	imod = (int) mod;
	if (imod < 0 || imod >= p->fonts_number)
	    pdf_error(p, PDF_ValueError,
		"Bad font descriptor %d in PDF_get_value/%s", imod, key);

	return p->fonts[imod].capHeight / (float) 1000;

    } else if (!strcmp(key, PDF_PARAMETER_ASCENDER)) {
	imod = (int) mod;
	if (imod < 0 || imod >= p->fonts_number)
	    pdf_error(p, PDF_ValueError,
		"Bad font descriptor %d in PDF_get_value/%s", imod, key);

	return p->fonts[imod].ascender / (float) 1000;

    } else if (!strcmp(key, PDF_PARAMETER_DESCENDER)) {
	imod = (int) mod;
	if (imod < 0 || imod >= p->fonts_number)
	    pdf_error(p, PDF_ValueError,
		"Bad font descriptor %d in PDF_get_value/%s", imod, key);

	return p->fonts[imod].descender / (float) 1000;

    /* unknown parameter */
    } else {
	pdf_error(p, PDF_ValueError,
		"Tried to get unknown parameter value '%s'", key);
    }

    return (float) 0;
}

PDFLIB_API const char * PDFLIB_CALL
PDF_get_parameter(PDF *p, const char *key, float mod)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return (const char *) NULL;

    if (key == NULL || !*key)
	pdf_error(p, PDF_ValueError, "Tried to get NULL parameter");

    if (!strcmp(key, PDF_PARAMETER_FONTNAME)) {
	if (mod != (float) 0) {
	    pdf_error(p, PDF_ValueError,
		    "Bad argument %f in PDF_get_parameter/%s", key, mod);
	} else {
	    return pdf_get_fontname(p);
	}

    } else if (!strcmp(key, PDF_PARAMETER_FONTENCODING)) {
	if (mod != (float) 0) {
	    pdf_error(p, PDF_ValueError,
		    "Bad argument %f in PDF_get_parameter/%s", key, mod);
	} else {
	    return pdf_get_fontencoding(p);
	}

    /* unknown parameter */
    } else {
	pdf_error(p, PDF_ValueError,
		"Tried to get unknown parameter '%s'", key);
    }

    return (const char *) NULL;
}
