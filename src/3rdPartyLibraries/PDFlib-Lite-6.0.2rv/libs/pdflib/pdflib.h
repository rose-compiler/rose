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

/* $Id: pdflib.h,v 1.1 2005/10/11 17:18:15 vuduc2 Exp $
 *
 * PDFlib public function declarations
 *
 */

#ifndef PDFLIB_H
#define PDFLIB_H

/* Make our declarations C++ compatible */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <setjmp.h>

/*
 * The version defines below can be used to check the version of the
 * include file against the library.
 */

#define PDFLIB_MAJORVERSION	6	/* PDFlib major version number */
#define PDFLIB_MINORVERSION	0       /* PDFlib minor version number */
#define PDFLIB_REVISION		2       /* PDFlib revision number */
#define PDFLIB_VERSIONSTRING	"6.0.2"       /* The whole bunch */


/*
 * ----------------------------------------------------------------------
 * Setup, mostly Windows calling conventions and DLL stuff
 * ----------------------------------------------------------------------
 */

#if defined(WIN32) && !defined(PDFLIB_CALL)
    #define PDFLIB_CALL	__cdecl
#endif

#if defined(WIN32)

    #ifdef PDFLIB_EXPORTS
    #define PDFLIB_API __declspec(dllexport) /* prepare a DLL (internal use) */

    #elif defined(PDFLIB_DLL)

    #define PDFLIB_API __declspec(dllimport) /* PDFlib clients: import  DLL */
    #endif	/* PDFLIB_DLL */

#endif /* WIN32 */

#if !defined(WIN32) && \
    ((defined __IBMC__ || defined __IBMCPP__) && defined __DLL__ && defined OS2)
    #define PDFLIB_CALL _Export
    #define PDFLIB_API
#endif	/* IBM VisualAge C++ DLL */

#ifndef PDFLIB_CALL
    #define PDFLIB_CALL	/* */	/* default: no special calling conventions */
#endif

#ifndef PDFLIB_API
    #define PDFLIB_API /* */	/* default: generate or use static library */
#endif

/* Define the basic PDF type. This is used opaquely at the API level. */
#if !defined(PDF) || defined(ACTIVEX)
	typedef struct PDF_s PDF;
#endif /* !PDF */

/* Export the API functions when creating a shared library on the Mac with CW */
#if defined(__MWERKS__) && defined(PDFLIB_EXPORTS)
#pragma export on
#endif

/* The API structure with function pointers. */
typedef struct PDFlib_api_s PDFlib_api;


/*
 * ----------------------------------------------------------------------
 * Function prototypes for all supported API functions
 * ----------------------------------------------------------------------
 */

/* Activate a previously created structure element or other content item. */
PDFLIB_API void PDFLIB_CALL
PDF_activate_item(PDF *p, int id);

/* Deprecated, use  PDF_create_bookmark(). */
PDFLIB_API int PDFLIB_CALL
PDF_add_bookmark(PDF *p, const char *text, int parent, int open);

/* Deprecated, use  PDF_create_bookmark(). */
PDFLIB_API int PDFLIB_CALL
PDF_add_bookmark2(PDF *p, const char *text, int len, int parent, int open);

/* Deprecated, use PDF_create_action() and PDF_create_annotation(). */
PDFLIB_API void PDFLIB_CALL
PDF_add_launchlink(PDF *p, double llx, double lly, double urx, double ury,
    const char *filename);

/* Deprecated, use PDF_create_action() and PDF_create_annotation(). */
PDFLIB_API void PDFLIB_CALL
PDF_add_locallink(PDF *p, double llx, double lly, double urx, double ury,
    int page, const char *optlist);

/* Create a named destination on an arbitrary page in the current document. */
PDFLIB_API void PDFLIB_CALL
PDF_add_nameddest(PDF *p, const char *name, int len, const char *optlist);

/* Deprecated, use PDF_create_annotation() instead. */
PDFLIB_API void PDFLIB_CALL
PDF_add_note(PDF *p, double llx, double lly, double urx, double ury,
    const char *contents, const char *title, const char *icon, int open);

/* Deprecated, use PDF_create_annotation() instead. */
PDFLIB_API void PDFLIB_CALL
PDF_add_note2(PDF *p, double llx, double lly, double urx, double ury,
    const char *contents, int len_cont, const char *title, int len_title,
    const char *icon, int open);

/* Deprecated, use PDF_create_action() and PDF_create_annotation(). */
PDFLIB_API void PDFLIB_CALL
PDF_add_pdflink(PDF *p, double llx, double lly, double urx, double ury,
    const char *filename, int page, const char *optlist);

/* Add an existing image as thumbnail for the current page. */
PDFLIB_API void PDFLIB_CALL
PDF_add_thumbnail(PDF *p, int image);

/* Deprecated, use PDF_create_action() and PDF_create_annotation(). */
PDFLIB_API void PDFLIB_CALL
PDF_add_weblink(PDF *p, double llx, double lly, double urx, double ury,
    const char *url);

/* Draw a counterclockwise circular arc segment. */
PDFLIB_API void PDFLIB_CALL
PDF_arc(PDF *p, double x, double y, double r, double alpha, double beta);

/* Draw a clockwise circular arc segment. */
PDFLIB_API void PDFLIB_CALL
PDF_arcn(PDF *p, double x, double y, double r, double alpha, double beta);

/* Deprecated, use  PDF_create_annotation() instead. */
PDFLIB_API void PDFLIB_CALL
PDF_attach_file(PDF *p, double llx, double lly, double urx, double ury,
    const char *filename, const char *description, const char *author,
    const char *mimetype, const char *icon);

/* Deprecated, use  PDF_create_annotation() instead. */
PDFLIB_API void PDFLIB_CALL
PDF_attach_file2(PDF *p, double llx, double lly, double urx, double ury,
    const char *filename, int len_filename, const char *description,
    int len_descr, const char *author, int len_auth, const char *mimetype,
    const char *icon);

/* Create a new PDF file subject to various options. */
PDFLIB_API int PDFLIB_CALL
PDF_begin_document(PDF *p, const char *filename, int len, const char *optlist);

/* Create a new PDF file subject to various options. */
typedef size_t (*writeproc_t)(PDF *p1, void *data, size_t size);
PDFLIB_API void PDFLIB_CALL
PDF_begin_document_callback(PDF *p, writeproc_t writeproc, const char *optlist);

/* Start a Type 3 font definition. */
PDFLIB_API void PDFLIB_CALL
PDF_begin_font(PDF *p, const char *fontname, int len,
    double a, double b, double c, double d, double e, double f,
    const char *optlist);

/* Start a glyph definition for a Type 3 font. */
PDFLIB_API void PDFLIB_CALL
PDF_begin_glyph(PDF *p, const char *glyphname, double wx,
    double llx, double lly, double urx, double ury);

/* Open a structure element or other content item with attributes supplied
 as options. */
PDFLIB_API int PDFLIB_CALL
PDF_begin_item(PDF *p, const char *tag, const char *optlist);

/* Start a layer for subsequent output on the page (requires PDF 1.5). */
PDFLIB_API void PDFLIB_CALL
PDF_begin_layer(PDF *p, int layer);

/* Begin a marked content sequence with or without properties (unsupported). */
PDFLIB_API void PDFLIB_CALL
PDF_begin_mc(PDF *p, const char *tag, const char *optlist);

/* Deprecated, use PDF_begin_page_ext(). */
PDFLIB_API void PDFLIB_CALL
PDF_begin_page(PDF *p, double width, double height);

/* Add a new page to the document, and specify various options. */
PDFLIB_API void PDFLIB_CALL
PDF_begin_page_ext(PDF *p, double width, double height, const char *optlist);

/* Start a pattern definition. */
PDFLIB_API int PDFLIB_CALL
PDF_begin_pattern(PDF *p,
    double width, double height, double xstep, double ystep, int painttype);

/* Start a template definition. */
PDFLIB_API int PDFLIB_CALL
PDF_begin_template(PDF *p, double width, double height);

/* Start a template definition with option list (unsupported). */
PDFLIB_API int PDFLIB_CALL
PDF_begin_template2(PDF *p, double width, double height, const char *optlist);

/* Boot PDFlib (recommended although currently not required). */
PDFLIB_API void PDFLIB_CALL
PDF_boot(void);

/* Draw a circle. */
PDFLIB_API void PDFLIB_CALL
PDF_circle(PDF *p, double x, double y, double r);

/* Use the current path as clipping path, and terminate the path. */
PDFLIB_API void PDFLIB_CALL
PDF_clip(PDF *p);

/* Deprecated, use PDF_end_document(). */
PDFLIB_API void PDFLIB_CALL
PDF_close(PDF *p);

/* Close an image. */
PDFLIB_API void PDFLIB_CALL
PDF_close_image(PDF *p, int image);

/* Close all open page handles, and close the input PDF document. */
PDFLIB_API void PDFLIB_CALL
PDF_close_pdi(PDF *p, int doc);

/* Close the page handle, and free all page-related resources. */
PDFLIB_API void PDFLIB_CALL
PDF_close_pdi_page(PDF *p, int page);

/* Close the current path. */
PDFLIB_API void PDFLIB_CALL
PDF_closepath(PDF *p);

/* Close the path, fill, and stroke it. */
PDFLIB_API void PDFLIB_CALL
PDF_closepath_fill_stroke(PDF *p);

/* Close the path, and stroke it. */
PDFLIB_API void PDFLIB_CALL
PDF_closepath_stroke(PDF *p);

/* Concatenate a matrix to the current transformation matrix. */
PDFLIB_API void PDFLIB_CALL
PDF_concat(PDF *p, double a, double b, double c, double d, double e, double f);

/* Print text at the next line. */
PDFLIB_API void PDFLIB_CALL
PDF_continue_text(PDF *p, const char *text);

/* Same as PDF_continue_text but with explicit string length. */
PDFLIB_API void PDFLIB_CALL
PDF_continue_text2(PDF *p, const char *text, int len);

/* Create an action which can be applied to various objects and events. */
PDFLIB_API int PDFLIB_CALL
PDF_create_action(PDF *p, const char *type, const char *optlist);

/* Create a rectangular annotation on the current page. */
PDFLIB_API void PDFLIB_CALL
PDF_create_annotation(PDF *p, double llx, double lly, double urx, double ury,
    const char *type, const char *optlist);

/* Create a bookmark subject to various options. */
PDFLIB_API int PDFLIB_CALL
PDF_create_bookmark(PDF *p, const char *text, int len, const char *optlist);

/* Create a form field on the current page subject to various options. */
PDFLIB_API void PDFLIB_CALL
PDF_create_field(PDF *p, double llx, double lly, double urx, double ury,
    const char *name, int len, const char *type, const char *optlist);

/* Create a form field group subject to various options. */
PDFLIB_API void PDFLIB_CALL
PDF_create_fieldgroup(PDF *p, const char *name, int len, const char *optlist);

/* Create a graphics state object subject to various options. */
PDFLIB_API int PDFLIB_CALL
PDF_create_gstate(PDF *p, const char *optlist);

/* Create a named virtual read-only file from data provided in memory. */
PDFLIB_API void PDFLIB_CALL
PDF_create_pvf(PDF *p, const char *filename, int len,
               const void *data, size_t size, const char *optlist);

/* Preprocess text for later formatting and create a textflow object. */
PDFLIB_API int PDFLIB_CALL
PDF_create_textflow(PDF *p, const char *text, int len, const char *optlist);

/* Draw a Bezier curve from the current point, using 3 more control points. */
PDFLIB_API void PDFLIB_CALL
PDF_curveto(PDF *p,
    double x_1, double y_1, double x_2, double y_2, double x_3, double y_3);

/* Create a new layer definition (requires PDF 1.5). */
PDFLIB_API int PDFLIB_CALL
PDF_define_layer(PDF *p, const char *name, int len, const char *optlist);

/* Delete a PDFlib object and free all internal resources. */
PDFLIB_API void PDFLIB_CALL
PDF_delete(PDF *p);

/* Delete a named virtual file and free its data structures (but not the
 contents). */
PDFLIB_API int PDFLIB_CALL
PDF_delete_pvf(PDF *p, const char *filename, int len);

/* Delete a textflow and the associated data structures. */
PDFLIB_API void PDFLIB_CALL
PDF_delete_textflow(PDF *p, int textflow);

/* Retrieve a glyph name from an encoding (unsupported). */
PDFLIB_API const char * PDFLIB_CALL
PDF_encoding_get_glyphname(PDF *p, const char *encoding, int slot);

/* Retrieve a slot's unicode value from an encoding (unsupported). */
PDFLIB_API int PDFLIB_CALL
PDF_encoding_get_unicode(PDF *p, const char *encoding, int slot);

/* Add a glyph name and/or Unicode value to a custom encoding. */
PDFLIB_API void PDFLIB_CALL
PDF_encoding_set_char(PDF *p, const char *encoding, int slot,
    const char *glyphname, int uv);

/* Close the generated PDF file and apply various options. */
PDFLIB_API void PDFLIB_CALL
PDF_end_document(PDF *p, const char *optlist);

/* Terminate a Type 3 font definition. */
PDFLIB_API void PDFLIB_CALL
PDF_end_font(PDF *p);

/* Terminate a glyph definition for a Type 3 font. */
PDFLIB_API void PDFLIB_CALL
PDF_end_glyph(PDF *p);

/* Close a structure element or other content item. */
PDFLIB_API void PDFLIB_CALL
PDF_end_item(PDF *p, int id);

/* Deactivate all active layers (requires PDF 1.5). */
PDFLIB_API void PDFLIB_CALL
PDF_end_layer(PDF *p);

/* End the least recently opened marked content sequence (unsupported). */
PDFLIB_API void PDFLIB_CALL
PDF_end_mc(PDF *p);

/* Deprecated, use PDF_end_page_ext(). */
PDFLIB_API void PDFLIB_CALL
PDF_end_page(PDF *p);

/* Finish a page, and apply various options. */
PDFLIB_API void PDFLIB_CALL
PDF_end_page_ext(PDF *p, const char *optlist);

/* Finish a pattern definition. */
PDFLIB_API void PDFLIB_CALL
PDF_end_pattern(PDF *p);

/* Finish a template definition. */
PDFLIB_API void PDFLIB_CALL
PDF_end_template(PDF *p);

/* End the current path without filling or stroking it. */
PDFLIB_API void PDFLIB_CALL
PDF_endpath(PDF *p);

/* Fill the interior of the path with the current fill color. */
PDFLIB_API void PDFLIB_CALL
PDF_fill(PDF *p);

/* Fill an image block with variable data according to its properties. */
PDFLIB_API int PDFLIB_CALL
PDF_fill_imageblock(PDF *p, int page, const char *blockname,
    int image, const char *optlist);

/* Fill a PDF block with variable data according to its properties. */
PDFLIB_API int PDFLIB_CALL
PDF_fill_pdfblock(PDF *p, int page, const char *blockname,
    int contents, const char *optlist);

/* Fill and stroke the path with the current fill and stroke color. */
PDFLIB_API void PDFLIB_CALL
PDF_fill_stroke(PDF *p);

/* Fill a text block with variable data according to its properties. */
PDFLIB_API int PDFLIB_CALL
PDF_fill_textblock(PDF *p, int page, const char *blockname,
    const char *text, int len, const char *optlist);

/* Deprecated, use  PDF_load_font(). */
PDFLIB_API int PDFLIB_CALL
PDF_findfont(PDF *p, const char *fontname, const char *encoding, int embed);

/* Place an image or template at on the page, subject to various options. */
PDFLIB_API void PDFLIB_CALL
PDF_fit_image(PDF *p, int image, double x, double y, const char *optlist);

/* Place an imported PDF page on the page subject to various options. */
PDFLIB_API void PDFLIB_CALL
PDF_fit_pdi_page(PDF *p, int page, double x, double y, const char *optlist);

/* Format the next portion of a textflow into a rectangular area. */
PDFLIB_API const char * PDFLIB_CALL
PDF_fit_textflow(PDF *p, int textflow, double llx, double lly,
    double urx, double ury, const char *optlist);

/* Place a single line of text at position (x, y) subject to various options. */
PDFLIB_API void PDFLIB_CALL
PDF_fit_textline(PDF *p, const char *text, int len, double x, double y,
    const char *optlist);

/*
 * Retrieve a structure with PDFlib API function pointers (mainly for DLLs).
 * Although this function is published here, it is not supposed to be used
 * directly by clients. Use PDF_new_dl() (in pdflibdl.c) instead.
 */
PDFLIB_API const PDFlib_api * PDFLIB_CALL
PDF_get_api(void);

/* Get the name of the API function which threw the last exception or failed. */
PDFLIB_API const char * PDFLIB_CALL
PDF_get_apiname(PDF *p);

/* Get the contents of the PDF output buffer. */
PDFLIB_API const char * PDFLIB_CALL
PDF_get_buffer(PDF *p, long *size);

/* Get the descriptive text of the last thrown exception or the reason of
 a failed function call. */
PDFLIB_API const char * PDFLIB_CALL
PDF_get_errmsg(PDF *p);

/* Get the number of the last thrown exception or the reason of a failed
 function call. */
PDFLIB_API int PDFLIB_CALL
PDF_get_errnum(PDF *p);

/* Request a glyph ID value from a font (unsupported). */
PDFLIB_API int PDFLIB_CALL
PDF_get_glyphid(PDF *p, int font, int code);

/* Request the amount of kerning between two characters (unsupported). */
PDFLIB_API double PDFLIB_CALL
PDF_get_kern_amount(PDF *p, int font, int firstchar, int secondchar);

/* Depreceated, use PDF_get_value(). */
PDFLIB_API int PDFLIB_CALL
PDF_get_majorversion(void);

/* Depreceated, use PDF_get_value(). */
PDFLIB_API int PDFLIB_CALL
PDF_get_minorversion(void);

/* Fetch the opaque application pointer stored in PDFlib. */
PDFLIB_API void * PDFLIB_CALL
PDF_get_opaque(PDF *p);

/* Get the contents of some PDFlib parameter with string type. */
PDFLIB_API const char * PDFLIB_CALL
PDF_get_parameter(PDF *p, const char *key, double modifier);

/* Get some PDI document parameter with string type. */
PDFLIB_API const char *PDFLIB_CALL
PDF_get_pdi_parameter(PDF *p, const char *key, int doc, int page,
    int reserved, int *len);

/* Get some PDI document parameter with numerical type. */
PDFLIB_API double PDFLIB_CALL
PDF_get_pdi_value(PDF *p, const char *key, int doc, int page, int reserved);

/* Get the value of some PDFlib parameter with numerical type. */
PDFLIB_API double PDFLIB_CALL
PDF_get_value(PDF *p, const char *key, double modifier);

/* Query the current state of a textflow. */
PDFLIB_API double PDFLIB_CALL
PDF_info_textflow(PDF *p, int textflow, const char *keyword);

/* Reset all color and graphics state parameters to their defaults. */
PDFLIB_API void PDFLIB_CALL
PDF_initgraphics(PDF *p);

/* Draw a line from the current point to another point. */
PDFLIB_API void PDFLIB_CALL
PDF_lineto(PDF *p, double x, double y);

/* Search for a font and prepare it for later use. */
PDFLIB_API int PDFLIB_CALL
PDF_load_font(PDF *p, const char *fontname, int len,
    const char *encoding, const char *optlist);

/* Search for an ICC profile, and prepare it for later use. */
PDFLIB_API int PDFLIB_CALL
PDF_load_iccprofile(PDF *p, const char *profilename, int len,
    const char *optlist);

/* Open a disk-based or virtual image file subject to various options. */
PDFLIB_API int PDFLIB_CALL
PDF_load_image(PDF *p, const char *imagetype, const char *filename,
    int len, const char *optlist);

/* Find a built-in spot color name, or make a named spot color from the
 current fill color. */
PDFLIB_API int PDFLIB_CALL
PDF_makespotcolor(PDF *p, const char *spotname, int reserved);

/* Add a marked content point with or without properties (unsupported). */
PDFLIB_API void PDFLIB_CALL
PDF_mc_point(PDF *p, const char *tag, const char *optlist);

/* Set the current point. */
PDFLIB_API void PDFLIB_CALL
PDF_moveto(PDF *p, double x, double y);

/* Create a new PDFlib object with default settings. */
PDFLIB_API PDF * PDFLIB_CALL
PDF_new(void);

/* Create a new PDFlib object with client-supplied error handling and memory
 allocation routines. */
typedef void  (*errorproc_t)(PDF *p1, int errortype, const char *msg);
typedef void* (*allocproc_t)(PDF *p2, size_t size, const char *caller);
typedef void* (*reallocproc_t)(PDF *p3,
                void *mem, size_t size, const char *caller);
typedef void  (*freeproc_t)(PDF *p4, void *mem);

PDFLIB_API PDF * PDFLIB_CALL
PDF_new2(errorproc_t errorhandler, allocproc_t allocproc,
        reallocproc_t reallocproc, freeproc_t freeproc, void *opaque);

/* Deprecated, use PDF_load_image(). */
PDFLIB_API int PDFLIB_CALL
PDF_open_CCITT(PDF *p, const char *filename, int width, int height,
    int BitReverse, int K, int BlackIs1);

/* Deprecated, use PDF_begin_document(). */
PDFLIB_API int PDFLIB_CALL
PDF_open_file(PDF *p, const char *filename);

/* Deprecated, use PDF_load_image() with virtual files. */
PDFLIB_API int PDFLIB_CALL
PDF_open_image(PDF *p, const char *imagetype, const char *source,
    const char *data, long length, int width, int height, int components,
    int bpc, const char *params);

/* Deprecated, use PDF_load_image(). */
PDFLIB_API int PDFLIB_CALL
PDF_open_image_file(PDF *p, const char *imagetype, const char *filename,
    const char *stringparam, int intparam);

/* Deprecated, use PDF_begin_document_callback(). */
PDFLIB_API void PDFLIB_CALL
PDF_open_mem(PDF *p, writeproc_t writeproc);

/* Open a disk-based or virtual PDF document and prepare it for later use. */
PDFLIB_API int PDFLIB_CALL
PDF_open_pdi(PDF *p, const char *filename, const char *optlist, int len);

/* Open an existing PDF document from a custom data source and prepare it for
 later use. */
PDFLIB_API int PDFLIB_CALL
PDF_open_pdi_callback(PDF *p, void *opaque, size_t filesize,
    size_t (*readproc)(void *opaque, void *buffer, size_t size),
    int (*seekproc)(void *opaque, long offset),
    const char *optlist);

/* Prepare a page for later use with PDF_fit_pdi_page(). */
PDFLIB_API int PDFLIB_CALL
PDF_open_pdi_page(PDF *p, int doc, int pagenumber, const char *optlist);

/* Deprecated, use PDF_fit_image(). */
PDFLIB_API void PDFLIB_CALL
PDF_place_image(PDF *p, int image, double x, double y, double scale);

/* Deprecated, use PDF_fit_pdi_page(). */
PDFLIB_API void PDFLIB_CALL
PDF_place_pdi_page(PDF *p, int page, double x, double y, double sx, double sy);

/* Process certain elements of an imported PDF document. */
PDFLIB_API int PDFLIB_CALL
PDF_process_pdi(PDF *p, int doc, int page, const char *optlist);

/* Draw a Bezier curve using relative coordinates (unsupported). */
PDFLIB_API void PDFLIB_CALL
PDF_rcurveto(PDF *p,
    double x_1, double y_1, double x_2, double y_2, double x_3, double y_3);

/* Draw a rectangle. */
PDFLIB_API void PDFLIB_CALL
PDF_rect(PDF *p, double x, double y, double width, double height);

/* Restore the most recently saved graphics state from the stack. */
PDFLIB_API void PDFLIB_CALL
PDF_restore(PDF *p);

/* Resume a page to add more content to it. */
PDFLIB_API void PDFLIB_CALL
PDF_resume_page(PDF *p, const char *optlist);

/* Draw a line from the current point to (cp + (x, y)) (unsupported). */
PDFLIB_API void PDFLIB_CALL
PDF_rlineto(PDF *p, double x, double y);

/* Set the new current point relative the old current point (unsupported). */
PDFLIB_API void PDFLIB_CALL
PDF_rmoveto(PDF *p, double x, double y);

/* Rotate the coordinate system. */
PDFLIB_API void PDFLIB_CALL
PDF_rotate(PDF *p, double phi);

/* Save the current graphics state to a stack. */
PDFLIB_API void PDFLIB_CALL
PDF_save(PDF *p);

/* Scale the coordinate system. */
PDFLIB_API void PDFLIB_CALL
PDF_scale(PDF *p, double sx, double sy);

/* Deprecated, use PDF_create_annotation(). */
PDFLIB_API void PDFLIB_CALL
PDF_set_border_color(PDF *p, double red, double green, double blue);

/* Deprecated, use PDF_create_annotation(). */
PDFLIB_API void PDFLIB_CALL
PDF_set_border_dash(PDF *p, double b, double w);

/* Deprecated, use PDF_create_annotation(). */
PDFLIB_API void PDFLIB_CALL
PDF_set_border_style(PDF *p, const char *style, double width);

/* Activate a gstate object. */
PDFLIB_API void PDFLIB_CALL
PDF_set_gstate(PDF *p, int gstate);

/* Fill document information field key with value. */
PDFLIB_API void PDFLIB_CALL
PDF_set_info(PDF *p, const char *key, const char *value);

/* Like PDF_set_info(), but with explicit string length. */
PDFLIB_API void PDFLIB_CALL
PDF_set_info2(PDF *p, const char *key, const char *value, int len);

/* Define hierarchical and group relationships among layers (requires
 PDF 1.5). */
PDFLIB_API void PDFLIB_CALL
PDF_set_layer_dependency(PDF *p, const char *type, const char *optlist);

/* Set some PDFlib parameter with string type. */
PDFLIB_API void PDFLIB_CALL
PDF_set_parameter(PDF *p, const char *key, const char *value);

/* Set the position for text output on the page. */
PDFLIB_API void PDFLIB_CALL
PDF_set_text_pos(PDF *p, double x, double y);

/* Set the value of some PDFlib parameter with numerical type. */
PDFLIB_API void PDFLIB_CALL
PDF_set_value(PDF *p, const char *key, double value);

/* Set the current color space and color. */
PDFLIB_API void PDFLIB_CALL
PDF_setcolor(PDF *p, const char *fstype, const char *colorspace,
    double c1, double c2, double c3, double c4);

/* Set the current dash pattern. */
PDFLIB_API void PDFLIB_CALL
PDF_setdash(PDF *p, double b, double w);

/* Set a dash pattern defined by an option list. */
PDFLIB_API void PDFLIB_CALL
PDF_setdashpattern(PDF *p, const char *optlist);

/* Set the flatness parameter. */
PDFLIB_API void PDFLIB_CALL
PDF_setflat(PDF *p, double flatness);

/* Set the current font in the specified size. */
PDFLIB_API void PDFLIB_CALL
PDF_setfont(PDF *p, int font, double fontsize);

/* Deprecated, use PDF_setcolor(). */
PDFLIB_API void PDFLIB_CALL
PDF_setgray(PDF *p, double gray);

/* Deprecated, use PDF_setcolor(). */
PDFLIB_API void PDFLIB_CALL
PDF_setgray_fill(PDF *p, double gray);

/* Deprecated, use PDF_setcolor(). */
PDFLIB_API void PDFLIB_CALL
PDF_setgray_stroke(PDF *p, double gray);

/* Set the linecap parameter. */
PDFLIB_API void PDFLIB_CALL
PDF_setlinecap(PDF *p, int linecap);

/* Set the linejoin parameter. */
PDFLIB_API void PDFLIB_CALL
PDF_setlinejoin(PDF *p, int linejoin);

/* Set the current linewidth. */
PDFLIB_API void PDFLIB_CALL
PDF_setlinewidth(PDF *p, double width);

/* Explicitly set the current transformation matrix. */
PDFLIB_API void PDFLIB_CALL
PDF_setmatrix(PDF *p, double a, double b, double c, double d,
    double e, double f);

/* Set the miter limit. */
PDFLIB_API void PDFLIB_CALL
PDF_setmiterlimit(PDF *p, double miter);

/* Deprecated, use PDF_setdashpattern(). */
PDFLIB_API void PDFLIB_CALL
PDF_setpolydash(PDF *p, float *dasharray, int length);

/* Deprecated, use PDF_setcolor(). */
PDFLIB_API void PDFLIB_CALL
PDF_setrgbcolor(PDF *p, double red, double green, double blue);

/* Deprecated, use PDF_setcolor(). */
PDFLIB_API void PDFLIB_CALL
PDF_setrgbcolor_fill(PDF *p, double red, double green, double blue);

/* Deprecated, use PDF_setcolor(). */
PDFLIB_API void PDFLIB_CALL
PDF_setrgbcolor_stroke(PDF *p, double red, double green, double blue);

/* Define a blend from the current fill color to another color (requires
 PDF 1.4 or above). */
PDFLIB_API int PDFLIB_CALL
PDF_shading(PDF *p, const char *shtype, double x_0, double y_0, double x_1,
    double y_1, double c_1, double c_2, double c_3, double c_4,
    const char *optlist);

/* Define a shading pattern using a shading object (requires PDF 1.4 or
 above). */
PDFLIB_API int PDFLIB_CALL
PDF_shading_pattern(PDF *p, int shading, const char *optlist);

/* Fill an area with a shading, based on a shading object (requires PDF 1.4
 or above). */
PDFLIB_API void PDFLIB_CALL
PDF_shfill(PDF *p, int shading);

/* Print text in the current font and size at the current position. */
PDFLIB_API void PDFLIB_CALL
PDF_show(PDF *p, const char *text);

/* Same as PDF_show() but with explicit string length. */
PDFLIB_API void PDFLIB_CALL
PDF_show2(PDF *p, const char *text, int len);

/* Deprecated, use PDF_fit_textline() or PDF_fit_textflow(). */
PDFLIB_API int PDFLIB_CALL
PDF_show_boxed(PDF *p, const char *text, double left, double top,
    double width, double height, const char *hmode, const char *feature);

/* Deprecated, use PDF_fit_textline() or PDF_fit_textflow(). */
PDFLIB_API int PDFLIB_CALL
PDF_show_boxed2(PDF *p, const char *text, int len, double left, double top,
    double width, double height, const char *hmode, const char *feature);

/* Print text in the current font. */
PDFLIB_API void PDFLIB_CALL
PDF_show_xy(PDF *p, const char *text, double x, double y);

/* Same as PDF_show_xy() but with explicit string length. */
PDFLIB_API void PDFLIB_CALL
PDF_show_xy2(PDF *p, const char *text, int len, double x, double y);

/* Shut down PDFlib (recommended although currently not required). */
PDFLIB_API void PDFLIB_CALL
PDF_shutdown(void);

/* Skew the coordinate system. */
PDFLIB_API void PDFLIB_CALL
PDF_skew(PDF *p, double alpha, double beta);

/* Return the width of text in an arbitrary font. */
PDFLIB_API double PDFLIB_CALL
PDF_stringwidth(PDF *p, const char *text, int font, double fontsize);

/* Same as PDF_stringwidth() but with explicit string length. */
PDFLIB_API double PDFLIB_CALL
PDF_stringwidth2(PDF *p, const char *text, int len, int font, double fontsize);

/* Stroke the path with the current color and line width, and clear it. */
PDFLIB_API void PDFLIB_CALL
PDF_stroke(PDF *p);

/* Suspend the current page so that it can later be resumed. */
PDFLIB_API void PDFLIB_CALL
PDF_suspend_page(PDF *p, const char *optlist);

/* Translate the origin of the coordinate system. */
PDFLIB_API void PDFLIB_CALL
PDF_translate(PDF *p, double tx, double ty);

/* Convert a string from UTF-16 format to UTF-8. */
PDFLIB_API const char * PDFLIB_CALL
PDF_utf16_to_utf8(PDF *p, const char *utf16string, int len, int *size);

/* Convert a string from UTF-8 format to UTF-16. */
PDFLIB_API const char * PDFLIB_CALL
PDF_utf8_to_utf16(PDF *p, const char *utf8string, const char *ordering,
    int *size);

/* Print text in the current font and size, using individual horizontal
 positions (unsupported). */
PDFLIB_API void PDFLIB_CALL
PDF_xshow(PDF *p, const char *text, int len, const double *xadvancelist);


/*
 * ----------------------------------------------------------------------
 * PDFlib API structure with function pointers to all API functions
 * ----------------------------------------------------------------------
 */

/* Auxiliary structure for try/catch */
typedef struct
{
    jmp_buf	jbuf;
} pdf_jmpbuf;


/* The API structure with pointers to all PDFlib API functions */
struct PDFlib_api_s {
    /* version numbers for checking the DLL against client code */
    size_t sizeof_PDFlib_api;	/* size of this structure */

    int major;			/* PDFlib major version number */
    int minor;			/* PDFlib minor version number */
    int revision;		/* PDFlib revision number */

    int reserved;		/* reserved */

    void (PDFLIB_CALL * PDF_activate_item)(PDF *p, int id);
    int (PDFLIB_CALL * PDF_add_bookmark)(PDF *p, const char *text,
	    int parent, int open);
    int (PDFLIB_CALL * PDF_add_bookmark2) (PDF *p, const char *text, int len,
	    int parent, int open);
    void (PDFLIB_CALL * PDF_add_launchlink)(PDF *p,
                double llx, double lly, double urx,
                double ury, const char *filename);
    void (PDFLIB_CALL * PDF_add_locallink)(PDF *p,
                double llx, double lly, double urx,
                double ury, int page, const char *optlist);
    void (PDFLIB_CALL * PDF_add_nameddest) (PDF *p, const char *name,
	    int len, const char *optlist);
    void (PDFLIB_CALL * PDF_add_note)(PDF *p, double llx, double lly,
                double urx, double ury, const char *contents, const char *title,
		const char *icon, int open);
    void (PDFLIB_CALL * PDF_add_note2) (PDF *p, double llx, double lly,
                double urx, double ury, const char *contents, int len_cont,
		const char *title, int len_title, const char *icon, int open);
    void (PDFLIB_CALL * PDF_add_pdflink)(PDF *p,
                double llx, double lly, double urx, double ury,
		const char *filename, int page, const char *optlist);
    void (PDFLIB_CALL * PDF_add_thumbnail)(PDF *p, int image);
    void (PDFLIB_CALL * PDF_add_weblink)(PDF *p, double llx,
		double lly, double urx, double ury, const char *url);
    void (PDFLIB_CALL * PDF_arc)(PDF *p, double x, double y,
                        double r, double alpha, double beta);
    void (PDFLIB_CALL * PDF_arcn)(PDF *p, double x, double y,
                        double r, double alpha, double beta);
    void (PDFLIB_CALL * PDF_attach_file)(PDF *p, double llx, double lly,
                double urx, double ury, const char *filename,
		const char *description,
		const char *author, const char *mimetype, const char *icon);
    void (PDFLIB_CALL * PDF_attach_file2) (PDF *p, double llx, double lly,
                double urx, double ury, const char *filename, int len_filename,
		const char *description, int len_descr, const char *author,
		int len_auth, const char *mimetype, const char *icon);
    int  (PDFLIB_CALL * PDF_begin_document)(PDF *p, const char *filename,
                        int len, const char *optlist);
    void (PDFLIB_CALL * PDF_begin_document_callback)(PDF *p,
                        writeproc_t writeproc, const char *optlist);
    void (PDFLIB_CALL * PDF_begin_font)(PDF *p, const char *fontname,
        int len, double a, double b, double c, double d, double e, double f,
        const char *optlist);
    void (PDFLIB_CALL * PDF_begin_glyph)(PDF *p, const char *glyphname,
        double wx, double llx, double lly, double urx, double ury);
    int  (PDFLIB_CALL * PDF_begin_item)(PDF *p, const char *tag,
						    const char *optlist);
    void (PDFLIB_CALL * PDF_begin_layer)(PDF *p, int layer);
    void (PDFLIB_CALL * PDF_begin_mc)(PDF *p,
				const char *tag, const char *optlist);
    void (PDFLIB_CALL * PDF_begin_page)(PDF *p, double width,
                                double height);
    void (PDFLIB_CALL * PDF_begin_page_ext)(PDF *p, double width,
                                double height, const char *optlist);
    int  (PDFLIB_CALL * PDF_begin_pattern)(PDF *p,
                        double width, double height,
                        double xstep, double ystep, int painttype);
    int  (PDFLIB_CALL * PDF_begin_template)(PDF *p,
                        double width, double height);
    void (PDFLIB_CALL * PDF_boot)(void);
    void (PDFLIB_CALL * PDF_circle)(PDF *p, double x, double y, double r);
    void (PDFLIB_CALL * PDF_clip)(PDF *p);
    void (PDFLIB_CALL * PDF_close)(PDF *p);
    void (PDFLIB_CALL * PDF_close_image)(PDF *p, int image);
    void (PDFLIB_CALL * PDF_close_pdi)(PDF *p, int doc);
    void (PDFLIB_CALL * PDF_close_pdi_page)(PDF *p, int page);
    void (PDFLIB_CALL * PDF_closepath)(PDF *p);
    void (PDFLIB_CALL * PDF_closepath_fill_stroke)(PDF *p);
    void (PDFLIB_CALL * PDF_closepath_stroke)(PDF *p);
    void (PDFLIB_CALL * PDF_concat)(PDF *p, double a, double b,
                                    double c, double d, double e, double f);
    void (PDFLIB_CALL * PDF_continue_text)(PDF *p, const char *text);
    void (PDFLIB_CALL * PDF_continue_text2)(PDF *p, const char *text,
					    int len);
    int (PDFLIB_CALL * PDF_create_action)(PDF *p, const char *type,
               const char *optlist);
    void (PDFLIB_CALL * PDF_create_annotation)(PDF *p,
                double llx, double lly, double urx, double ury,
                const char *type, const char *optlist);
    int (PDFLIB_CALL * PDF_create_bookmark)(PDF *p, const char *text, int len,
            const char *optlist);
    void (PDFLIB_CALL * PDF_create_field) (PDF *p, double llx, double lly,
                double urx, double ury, const char *name, int len,
                const char *type, const char *optlist);
    void (PDFLIB_CALL * PDF_create_fieldgroup) (PDF *p, const char *name,
                int len, const char *optlist);
    int (PDFLIB_CALL * PDF_create_gstate) (PDF *p, const char *optlist);
    void (PDFLIB_CALL * PDF_create_pvf)(PDF *p, const char *filename,
                            int len, const void *data, size_t size,
                            const char *optlist);
    int (PDFLIB_CALL * PDF_create_textflow)(PDF *p, const char *text, int len,
                                            const char *optlist);
    void (PDFLIB_CALL * PDF_curveto)(PDF *p, double x_1, double y_1,
                        double x_2, double y_2, double x_3, double y_3);
    int (PDFLIB_CALL * PDF_define_layer)(PDF *p, const char *name, int len,
						    const char *optlist);

    void (PDFLIB_CALL * PDF_delete)(PDF *);
    int (PDFLIB_CALL * PDF_delete_pvf)(PDF *p, const char *filename, int len);
    void (PDFLIB_CALL * PDF_delete_textflow)(PDF *p, int textflow);
    void (PDFLIB_CALL * PDF_encoding_set_char) (PDF *p, const char *encoding,
                            int slot, const char *glyphname, int uv);
    void (PDFLIB_CALL * PDF_end_document)(PDF *p, const char *optlist);
    void (PDFLIB_CALL * PDF_end_font)(PDF *p);
    void (PDFLIB_CALL * PDF_end_glyph)(PDF *p);
    void (PDFLIB_CALL * PDF_end_item)(PDF *p, int id);
    void (PDFLIB_CALL * PDF_end_layer)(PDF *p);
    void (PDFLIB_CALL * PDF_end_mc)(PDF *p);
    void (PDFLIB_CALL * PDF_end_page)(PDF *p);
    void (PDFLIB_CALL * PDF_end_page_ext)(PDF *p, const char *optlist);
    void (PDFLIB_CALL * PDF_end_pattern)(PDF *p);
    void (PDFLIB_CALL * PDF_end_template)(PDF *p);
    void (PDFLIB_CALL * PDF_endpath)(PDF *p);
    void (PDFLIB_CALL * PDF_fill)(PDF *p);
    int (PDFLIB_CALL * PDF_fill_imageblock) (PDF *p, int page,
		const char *blockname, int image, const char *optlist);
    int (PDFLIB_CALL * PDF_fill_pdfblock) (PDF *p, int page,
	    const char *blockname, int contents, const char *optlist);
    void (PDFLIB_CALL * PDF_fill_stroke)(PDF *p);
    int (PDFLIB_CALL * PDF_fill_textblock) (PDF *p, int page,
	    const char *blockname, const char *text, int len,
	    const char *optlist);
    int  (PDFLIB_CALL * PDF_findfont)(PDF *p, const char *fontname,
			    const char *encoding, int embed);
    void (PDFLIB_CALL * PDF_fit_image) (PDF *p, int image, double x, double y,
	    const char *optlist);
    void (PDFLIB_CALL * PDF_fit_pdi_page) (PDF *p, int page, double x,
            double y, const char *optlist);
    const char * (PDFLIB_CALL * PDF_fit_textflow)(PDF *p, int textflow,
                        double llx, double lly, double urx, double ury,
                        const char *optlist);
    void (PDFLIB_CALL * PDF_fit_textline)(PDF *p, const char *text,
                         int len, double x, double y, const char *optlist);
    const PDFlib_api * (PDFLIB_CALL * PDF_get_api)(void);
    const char * (PDFLIB_CALL * PDF_get_apiname) (PDF *p);
    const char * (PDFLIB_CALL * PDF_get_buffer)(PDF *p, long *size);
    const char * (PDFLIB_CALL * PDF_get_errmsg) (PDF *p);
    int (PDFLIB_CALL * PDF_get_errnum) (PDF *p);
    int  (PDFLIB_CALL * PDF_get_minorversion)(void);
    int  (PDFLIB_CALL * PDF_get_majorversion)(void);
    void * (PDFLIB_CALL * PDF_get_opaque)(PDF *p);
    const char * (PDFLIB_CALL * PDF_get_parameter)(PDF *p,
                                const char *key, double modifier);
    const char * (PDFLIB_CALL * PDF_get_pdi_parameter)(PDF *p,
		    const char *key, int doc, int page, int reserved, int *len);
    double (PDFLIB_CALL * PDF_get_pdi_value)(PDF *p, const char *key,
					    int doc, int page, int reserved);
    double (PDFLIB_CALL * PDF_get_value)(PDF *p, const char *key,
                                double modifier);
    double (PDFLIB_CALL * PDF_info_textflow)(PDF *p, int textflow,
                        const char *keyword);
    void (PDFLIB_CALL * PDF_initgraphics)(PDF *p);
    void (PDFLIB_CALL * PDF_lineto)(PDF *p, double x, double y);
    int (PDFLIB_CALL * PDF_load_font)(PDF *p, const char *fontname,
		int len, const char *encoding, const char *optlist);
    int  (PDFLIB_CALL * PDF_load_iccprofile)(PDF *p, const char *profilename,
                        int len, const char *optlist);
    int (PDFLIB_CALL * PDF_load_image) (PDF *p, const char *imagetype,
	    const char *filename, int len, const char *optlist);
    int  (PDFLIB_CALL * PDF_makespotcolor)(PDF *p, const char *spotname,
                        int len);
    void (PDFLIB_CALL * PDF_mc_point)(PDF *p,
    				const char *tag, const char *optlist);
    void (PDFLIB_CALL * PDF_moveto)(PDF *p, double x, double y);
    PDF* (PDFLIB_CALL * PDF_new)(void);
    PDF* (PDFLIB_CALL * PDF_new2)(errorproc_t errorhandler,
                                allocproc_t allocproc,
                                reallocproc_t reallocproc,
                                freeproc_t freeproc, void *opaque);
    int (PDFLIB_CALL * PDF_open_CCITT)(PDF *p, const char *filename,
    			int width, int height,
			int BitReverse, int K, int BlackIs1);
    int  (PDFLIB_CALL * PDF_open_file)(PDF *p, const char *filename);
    int (PDFLIB_CALL * PDF_open_image)(PDF *p, const char *imagetype,
		const char *source, const char *data, long length, int width,
		int height, int components, int bpc, const char *params);
    int (PDFLIB_CALL * PDF_open_image_file)(PDF *p, const char *imagetype,
		const char *filename, const char *stringparam, int intparam);
    void (PDFLIB_CALL * PDF_open_mem)(PDF *p, writeproc_t writeproc);
    int  (PDFLIB_CALL * PDF_open_pdi)(PDF *p, const char *filename,
                                const char *optlist, int len);
    int (PDFLIB_CALL * PDF_open_pdi_callback) (PDF *p, void *opaque,
	    size_t filesize, size_t (*readproc)(void *opaque, void *buffer,
	    size_t size), int (*seekproc)(void *opaque, long offset),
	    const char *optlist);
    int  (PDFLIB_CALL * PDF_open_pdi_page)(PDF *p,
				int doc, int pagenumber, const char *optlist);
    void (PDFLIB_CALL * PDF_place_image)(PDF *p, int image,
                                        double x, double y, double scale);
    void (PDFLIB_CALL * PDF_place_pdi_page)(PDF *p, int page,
                                double x, double y, double sx, double sy);
    int (PDFLIB_CALL * PDF_process_pdi)(PDF *p,
				int doc, int page, const char *optlist);
    void (PDFLIB_CALL * PDF_rect)(PDF *p, double x, double y,
                        double width, double height);
    void (PDFLIB_CALL * PDF_restore)(PDF *p);
    void (PDFLIB_CALL * PDF_resume_page)(PDF *p, const char *optlist);
    void (PDFLIB_CALL * PDF_rotate)(PDF *p, double phi);
    void (PDFLIB_CALL * PDF_save)(PDF *p);
    void (PDFLIB_CALL * PDF_scale)(PDF *p, double sx, double sy);
    void (PDFLIB_CALL * PDF_set_border_color)(PDF *p,
                                    double red, double green, double blue);
    void (PDFLIB_CALL * PDF_set_border_dash)(PDF *p, double b, double w);
    void (PDFLIB_CALL * PDF_set_border_style)(PDF *p,
                                    const char *style, double width);
    void (PDFLIB_CALL * PDF_set_gstate) (PDF *p, int gstate);
    void (PDFLIB_CALL * PDF_set_info)(PDF *p, const char *key,
	    const char *value);
    void (PDFLIB_CALL * PDF_set_info2) (PDF *p, const char *key,
	    const char *value, int len);
    void (PDFLIB_CALL * PDF_set_layer_dependency)(PDF *p, const char *type,
						    const char *optlist);
    void (PDFLIB_CALL * PDF_set_parameter)(PDF *p,
				const char *key, const char *value);
    void (PDFLIB_CALL * PDF_set_text_pos)(PDF *p, double x, double y);
    void (PDFLIB_CALL * PDF_set_value)(PDF *p, const char *key,
                                double value);
    void (PDFLIB_CALL * PDF_setcolor)(PDF *p,
			const char *fstype, const char *colorspace,
                        double c1, double c2, double c3, double c4);
    void (PDFLIB_CALL * PDF_setdash)(PDF *p, double b, double w);
    void (PDFLIB_CALL * PDF_setdashpattern) (PDF *p, const char *optlist);
    void (PDFLIB_CALL * PDF_setflat)(PDF *p, double flatness);
    void (PDFLIB_CALL * PDF_setfont)(PDF *p, int font, double fontsize);
    void (PDFLIB_CALL * PDF_setgray)(PDF *p, double gray);
    void (PDFLIB_CALL * PDF_setgray_stroke)(PDF *p, double gray);
    void (PDFLIB_CALL * PDF_setgray_fill)(PDF *p, double gray);
    void (PDFLIB_CALL * PDF_setlinecap)(PDF *p, int linecap);
    void (PDFLIB_CALL * PDF_setlinejoin)(PDF *p, int linejoin);
    void (PDFLIB_CALL * PDF_setlinewidth)(PDF *p, double width);
    void (PDFLIB_CALL * PDF_setmatrix)(PDF *p, double a, double b,
                                    double c, double d, double e, double f);
    void (PDFLIB_CALL * PDF_setmiterlimit)(PDF *p, double miter);
    void (PDFLIB_CALL * PDF_setpolydash)(PDF *p, float *dasharray,
				    int length);
    void (PDFLIB_CALL * PDF_setrgbcolor)(PDF *p, double red, double green,
                        double blue);
    void (PDFLIB_CALL * PDF_setrgbcolor_fill)(PDF *p,
                        double red, double green, double blue);
    void (PDFLIB_CALL * PDF_setrgbcolor_stroke)(PDF *p,
                        double red, double green, double blue);
    int (PDFLIB_CALL * PDF_shading) (PDF *p, const char *shtype, double x_0,
            double y_0, double x_1, double y_1, double c_1, double c_2,
            double c_3, double c_4, const char *optlist);
    int (PDFLIB_CALL * PDF_shading_pattern) (PDF *p, int shading,
	    const char *optlist);
    void (PDFLIB_CALL * PDF_shfill) (PDF *p, int shading);
    void (PDFLIB_CALL * PDF_show)(PDF *p, const char *text);
    void (PDFLIB_CALL * PDF_show2)(PDF *p, const char *text, int len);
    int  (PDFLIB_CALL * PDF_show_boxed)(PDF *p, const char *text,
                        double left, double top, double width, double height,
                        const char *hmode, const char *feature);
    int  (PDFLIB_CALL * PDF_show_boxed2)(PDF *p, const char *text, int len,
                        double left, double top, double width, double height,
                        const char *hmode, const char *feature);
    void (PDFLIB_CALL * PDF_show_xy)(PDF *p, const char *text, double x,
                        double y);
    void (PDFLIB_CALL * PDF_show_xy2)(PDF *p, const char *text,
                                            int len, double x, double y);
    void (PDFLIB_CALL * PDF_shutdown)(void);
    void (PDFLIB_CALL * PDF_skew)(PDF *p, double alpha, double beta);
    double (PDFLIB_CALL * PDF_stringwidth)(PDF *p,
                                const char *text, int font, double fontsize);
    double (PDFLIB_CALL * PDF_stringwidth2)(PDF *p, const char *text,
                        int len, int font, double fontsize);
    void (PDFLIB_CALL * PDF_stroke)(PDF *p);
    void (PDFLIB_CALL * PDF_suspend_page)(PDF *p, const char *optlist);
    void (PDFLIB_CALL * PDF_translate)(PDF *p, double tx, double ty);
    const char * (PDFLIB_CALL * PDF_utf16_to_utf8) (PDF *p,
        const char *utf16string, int len, int *size);
    const char * (PDFLIB_CALL * PDF_utf8_to_utf16) (PDF *p,
        const char *utf8string, const char *format, int *size);
    void (PDFLIB_CALL * PDF_xshow)(PDF *p, const char *text, int len,
    	const double *xadvancelist);

    int  (PDFLIB_CALL * pdf_catch)(PDF *p);
    void (PDFLIB_CALL * pdf_exit_try)(PDF *p);
    pdf_jmpbuf * (PDFLIB_CALL * pdf_jbuf)(PDF *p);
    void (PDFLIB_CALL * pdf_rethrow)(PDF *p);
};


/*
 * ----------------------------------------------------------------------
 * Exception handling with try/catch implementation
 * ----------------------------------------------------------------------
 */

/* Set up an exception handling frame; must always be paired with PDF_CATCH().*/

#define PDF_TRY(p)		if (p) { if (setjmp(pdf_jbuf(p)->jbuf) == 0)

/* Inform the exception machinery that a PDF_TRY() will be left without
   entering the corresponding PDF_CATCH( ) clause. */
#define PDF_EXIT_TRY(p)		pdf_exit_try(p)

/* Catch an exception; must always be paired with PDF_TRY(). */
#define PDF_CATCH(p)		} if (pdf_catch(p))

/* Re-throw an exception to another handler. */
#define PDF_RETHROW(p)		pdf_rethrow(p)


/*
 * ----------------------------------------------------------------------
 * End of supported public declarations
 * ----------------------------------------------------------------------
 */

/*
 * ------------------------------------------------------------------------
 * Deprecated: macros for page size formats
 * ------------------------------------------------------------------------
 */

/*
 * The page sizes are only available to the C and C++ bindings.
 * These are deprecated; corresponding options are supported in
 * PDF_begin_page_ext().
 */

#define a0_width	 2380.0
#define a0_height	 3368.0
#define a1_width	 1684.0
#define a1_height	 2380.0
#define a2_width	 1190.0
#define a2_height	 1684.0
#define a3_width	 842.0
#define a3_height	 1190.0
#define a4_width	 595.0
#define a4_height	 842.0
#define a5_width	 421.0
#define a5_height	 595.0
#define a6_width	 297.0
#define a6_height	 421.0
#define b5_width	 501.0
#define b5_height	 709.0
#define letter_width	 612.0
#define letter_height	 792.0
#define legal_width 	 612.0
#define legal_height 	 1008.0
#define ledger_width	 1224.0
#define ledger_height	 792.0
#define p11x17_width	 792.0
#define p11x17_height	 1224.0


/*
 * ----------------------------------------------------------------------
 * Deprecated: Error classes
 * ----------------------------------------------------------------------
 */

/*
 * Error classes are deprecated; use PDF_TRY/PDF_CATCH instead.
 * Note that old-style error handlers are still supported, but
 * they will always receive type PDF_NonfatalError (for warnings)
 * or PDF_UnknownError (for other exceptions).
 */

#define PDF_MemoryError    1
#define PDF_IOError        2
#define PDF_RuntimeError   3
#define PDF_IndexError     4
#define PDF_TypeError      5
#define PDF_DivisionByZero 6
#define PDF_OverflowError  7
#define PDF_SyntaxError    8
#define PDF_ValueError     9
#define PDF_SystemError   10

#define PDF_NonfatalError 11
#define PDF_UnknownError  12


/*
 * ----------------------------------------------------------------------
 * Private stuff, do not use explicitly but only via the macros above!
 * ----------------------------------------------------------------------
 */

PDFLIB_API pdf_jmpbuf * PDFLIB_CALL
pdf_jbuf(PDF *p);

PDFLIB_API void PDFLIB_CALL
pdf_exit_try(PDF *p);

PDFLIB_API int PDFLIB_CALL
pdf_catch(PDF *p);

PDFLIB_API void PDFLIB_CALL
pdf_rethrow(PDF *p);

PDFLIB_API void PDFLIB_CALL
pdf_throw(PDF *p, const char *binding, const char *apiname, const char *errmsg);


/*
 * ----------------------------------------------------------------------
 * End of useful stuff
 * ----------------------------------------------------------------------
 */

#if defined(__MWERKS__) && defined(PDFLIB_EXPORTS)
#pragma export off
#endif

#ifdef __cplusplus
}	/* extern "C" */
#endif

#endif	/* PDFLIB_H */

/*
 * vim600: sw=4 fdm=marker
 */
