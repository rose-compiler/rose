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

/* pdflib.h
 *
 * PDFlib public function and constant declarations
 *
 */

#ifndef PDFLIB_H
#define PDFLIB_H

/* 
 * ----------------------------------------------------------------------
 * Setup, mostly Windows calling conventions and DLL stuff
 * ----------------------------------------------------------------------
 */

#ifndef SWIG
#include <stdlib.h>
#include <stdio.h>
#endif

#ifdef WIN32

#define PDFLIB_CALL	__cdecl

#ifdef PDFLIB_EXPORTS
#define PDFLIB_API __declspec(dllexport) /* prepare a DLL (PDFlib-internal use only) */

#elif defined(PDFLIB_DLL)
#define PDFLIB_API __declspec(dllimport) /* PDFlib clients - import PDFlib DLL fcts. */

#else	/* !PDFLIB_DLL */	
#define PDFLIB_API /* */		 /* default: generate or use static library */

#endif	/* !PDFLIB_DLL */

#else	/* !WIN32 */

#if (((defined __IBMC__) || (defined __IBMCPP__)) && (defined __DLL__))
    #define PDFLIB_CALL _Export
    #define PDFLIB_API
#endif	/* IBM VisualAge C++ DLL */

#ifndef PDFLIB_CALL
#define PDFLIB_CALL
#endif
#ifndef PDFLIB_API
#define PDFLIB_API
#endif

#endif	/* !WIN32 */

/* Make our declarations C++ compatible */
#ifdef __cplusplus
extern "C" {
#endif

/* Define the basic PDF type. This is used opaquely at the API level. */
typedef struct PDF_s PDF;

/* 
 * ----------------------------------------------------------------------
 * p_basic.c
 * ----------------------------------------------------------------------
 */

/* General Functions */

#ifndef SWIG
/*
 * The version defines below may be used to check the version of the
 * include file against the library. This is not reasonable at the
 * scripting API level since both version constants and version functions
 * are supplied from the library in this case.
 */

/* PDFlib version number, major part */
#define PDFLIB_MAJORVERSION	3

/* PDFlib version number, minor part (must use two decimal places if != 0) */
#define PDFLIB_MINORVERSION	02
#define PDFLIB_VERSIONSTRING	"3.02"

/*
 * ActiveX uses the Class ID;
 * Tcl and Perl have intrinsic versioning which we make use of;
 */
#if !defined(PDFLIB_ACTIVEX) && !defined(PDFLIB_TCL) && !defined(PDFLIB_PERL)

/* Returns the PDFlib major version number. */
PDFLIB_API int PDFLIB_CALL
PDF_get_majorversion(void);

/* Returns the PDFlib minor version number. */
PDFLIB_API int PDFLIB_CALL
PDF_get_minorversion(void);

#endif /* !defined(PDFLIB_ACTIVEX) && !defined(PDFLIB_TCL) && !defined(PDFLIB_PERL) */

/* Boot PDFlib. Recommended although currently not required.
 Booting is done automatically for Java, Tcl, Perl, and Python. */
PDFLIB_API void PDFLIB_CALL
PDF_boot(void);

/* Shut down PDFlib. Recommended although currently not required. */
PDFLIB_API void PDFLIB_CALL
PDF_shutdown(void);

/* This typedef is required to pacify the Watcom compiler in C++ mode. */
typedef void (*errorproc)(PDF *p, int type, const char *msg);

/* Create a new PDF object.  Returns a pointer to the opaque PDF datatype 
which you will need as the "p" argument for all other functions. All 
function pointers may be NULL if default handlers are to be used. */
PDFLIB_API PDF * PDFLIB_CALL
PDF_new2(errorproc errorhandler,
    void* (*allocproc)(PDF *p, size_t size, const char *caller),
    void* (*reallocproc)(PDF *p, void *mem, size_t size, const char *caller),
    void  (*freeproc)(PDF *p, void *mem),
    void   *opaque);

/* Fetch opaque application pointer stored in PDFlib (useful for
 multi-threading) */
PDFLIB_API void * PDFLIB_CALL
PDF_get_opaque(PDF *p);

#endif	/* !SWIG */

/* Create a new PDF object. */
PDFLIB_API PDF * PDFLIB_CALL
PDF_new(void);

/* Delete the PDF object. */
PDFLIB_API void PDFLIB_CALL
PDF_delete(PDF *p);

/* Open a new PDF file associated with p, using the supplied file name.
 Returns -1 on error. */
PDFLIB_API int PDFLIB_CALL
PDF_open_file(PDF *p, const char *filename);

/* Close the generated PDF file. */
PDFLIB_API void PDFLIB_CALL
PDF_close(PDF *p);

#if !defined(SWIG)
/* Open a new PDF file associated with p, using the supplied file handle.
 Returns -1 on error. */
PDFLIB_API int PDFLIB_CALL
PDF_open_fp(PDF *p, FILE *fp);

/* Open a new PDF in memory, and install a callback for fetching the data */
PDFLIB_API void PDFLIB_CALL
PDF_open_mem(PDF *p, size_t (*writeproc)(PDF *p, void *data, size_t size));
#endif	/* !defined(SWIG) */

/* Get the contents of the PDF output buffer. The result must be used
 by the client before calling any other PDFlib function.  Must not be
 called within page descriptions. */
PDFLIB_API const char * PDFLIB_CALL
PDF_get_buffer(PDF *p, long *size);

/* Start a new page. */
PDFLIB_API void PDFLIB_CALL
PDF_begin_page(PDF *p, float width, float height);

/* Finish the page. */
PDFLIB_API void PDFLIB_CALL
PDF_end_page(PDF *p);

#ifndef SWIG
/* PDFlib exceptions which may be handled by a user-supplied error handler */
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

#endif	/* !SWIG */

/* Set some PDFlib parameter with string type */
PDFLIB_API void PDFLIB_CALL
PDF_set_parameter(PDF *p, const char *key, const char *value);

/* Set the contents of some PDFlib parameter with string type */
PDFLIB_API const char * PDFLIB_CALL
PDF_get_parameter(PDF *p, const char *key, float modifier);

/* Set some PDFlib parameter with float type */
PDFLIB_API void PDFLIB_CALL
PDF_set_value(PDF *p, const char *key, float value);

/* Get the value of some PDFlib parameter with float type */
PDFLIB_API float PDFLIB_CALL
PDF_get_value(PDF *p, const char *key, float modifier);

/* 
 * ----------------------------------------------------------------------
 * p_font.c
 * ----------------------------------------------------------------------
 */

/* Text Functions, Font Handling Functions */

/* Prepare a font for later use with PDF_setfont(). The metrics will be
 loaded, and if embed is nonzero, the font file will be checked (but not
 yet used. Encoding is one of "builtin", "macroman", "winansi", "host", or
 a user-defined name, or the name of a CMap. */
PDFLIB_API int PDFLIB_CALL
PDF_findfont(PDF *p, const char *fontname, const char *encoding, int embed);

/* Set the current font in the given size. The font descriptor must have
 been retrieved via PDF_findfont(). */
PDFLIB_API void PDFLIB_CALL
PDF_setfont(PDF *p, int font, float fontsize);

/* 
 * ----------------------------------------------------------------------
 * p_text.c
 * ----------------------------------------------------------------------
 */

/* Text Output Functions */

/* Print text in the current font and size at the current position. */
PDFLIB_API void PDFLIB_CALL
PDF_show(PDF *p, const char *text);

/* Print text in the current font at (x, y). */
PDFLIB_API void PDFLIB_CALL
PDF_show_xy(PDF *p, const char *text, float x, float y);

/* Print text at the next line. */
PDFLIB_API void PDFLIB_CALL
PDF_continue_text(PDF *p, const char *text);

/* Format text in the current font and size into the supplied text box
 according to the requested formatting mode. If width and height
 are 0, only a single line is placed at the point (left, top) in the
 requested mode. Returns the number of characters which didn't fit in
 the box. */
PDFLIB_API int PDFLIB_CALL
PDF_show_boxed(PDF *p, const char *text, float left, float top, float width, float height, const char *hmode, const char *feature);

/* Set a transformation matrix to be applied to the current font. */
PDFLIB_API void PDFLIB_CALL
PDF_set_text_matrix(PDF *p, float a, float b, float c, float d, float e, float f);

/* Set the text output position to (x, y). */
PDFLIB_API void PDFLIB_CALL
PDF_set_text_pos(PDF *p, float x, float y);

/* Return the width of text in an arbitrary font which has been selected
 with PDF_findfont(). */
PDFLIB_API float PDFLIB_CALL
PDF_stringwidth(PDF *p, const char *text, int font, float size);

#ifndef SWIG

/* Function duplicates with explicit string length for use with 
strings containing null characters. These are for C and C++ clients only,
but are used internally for the other language bindings. */

/* Same as PDF_show() but with explicit string length. */
PDFLIB_API void PDFLIB_CALL
PDF_show2(PDF *p, const char *text, int len);

/* Same as PDF_show_xy() but with explicit string length. */
PDFLIB_API void PDFLIB_CALL
PDF_show_xy2(PDF *p, const char *text, int len, float x, float y);

/* Same as PDF_continue_text but with explicit string length. */
PDFLIB_API void PDFLIB_CALL
PDF_continue_text2(PDF *p, const char *text, int len);

/* Same as PDF_stringwidth but with explicit string length. */
PDFLIB_API float PDFLIB_CALL
PDF_stringwidth2(PDF *p, const char *text, int len, int font, float size);

#endif /* !SWIG */

/* 
 * ----------------------------------------------------------------------
 * p_gstate.c
 * ----------------------------------------------------------------------
 */

/* Graphics Functions, General Graphics State Functions */

/* Set the current dash pattern to b black and w white units. */
PDFLIB_API void PDFLIB_CALL
PDF_setdash(PDF *p, float b, float w);

/* Set a more complicated dash pattern defined by an array. */
PDFLIB_API void PDFLIB_CALL
PDF_setpolydash(PDF *p, float *dasharray, int length);

#ifndef SWIG
/* Maximum length of dash arrays */
#define MAX_DASH_LENGTH	8
#endif

/* Set the flatness to a value between 0 and 100 inclusive. */
PDFLIB_API void PDFLIB_CALL
PDF_setflat(PDF *p, float flatness);

/* Set the line join parameter to a value between 0 and 2 inclusive. */
PDFLIB_API void PDFLIB_CALL
PDF_setlinejoin(PDF *p, int linejoin);

/* Set the linecap parameter to a value between 0 and 2 inclusive. */
PDFLIB_API void PDFLIB_CALL
PDF_setlinecap(PDF *p, int linecap);

/* Set the miter limit to a value greater than or equal to 1. */
PDFLIB_API void PDFLIB_CALL
PDF_setmiterlimit(PDF *p, float miter);

/* Set the current linewidth to width. */
PDFLIB_API void PDFLIB_CALL
PDF_setlinewidth(PDF *p, float width);


/* Special Graphics State Functions */

/* Save the current graphics state. */
PDFLIB_API void PDFLIB_CALL
PDF_save(PDF *p);

/* Restore the most recently saved graphics state. */
PDFLIB_API void PDFLIB_CALL
PDF_restore(PDF *p);

/* Translate the origin of the coordinate system to (tx, ty). */
PDFLIB_API void PDFLIB_CALL
PDF_translate(PDF *p, float tx, float ty);

/* Scale the coordinate system by (sx, sy). */
PDFLIB_API void PDFLIB_CALL
PDF_scale(PDF *p, float sx, float sy);

/* Rotate the coordinate system by phi degrees. */
PDFLIB_API void PDFLIB_CALL
PDF_rotate(PDF *p, float phi);

/* Skew the coordinate system in x and y direction by alpha and beta degrees. */
PDFLIB_API void PDFLIB_CALL
PDF_skew(PDF *p, float alpha, float beta);

/* Concatenate a matrix to the CTM. a*d must not be equal to b*c. */
PDFLIB_API void PDFLIB_CALL
PDF_concat(PDF *p, float a, float b, float c, float d, float e, float f);

/* 
 * ----------------------------------------------------------------------
 * p_draw.c
 * ----------------------------------------------------------------------
 */

/* Path Segment Functions */

/* Set the current point to (x, y). */
PDFLIB_API void PDFLIB_CALL
PDF_moveto(PDF *p, float x, float y);

/* Draw a line from the current point to (x, y). */
PDFLIB_API void PDFLIB_CALL
PDF_lineto(PDF *p, float x, float y);

/* Draw a Bezier curve from the current point, using 3 more control points. */
PDFLIB_API void PDFLIB_CALL
PDF_curveto(PDF *p, float x1, float y1, float x2, float y2, float x3, float y3);

/* Draw a circle with center (x, y) and radius r. */
PDFLIB_API void PDFLIB_CALL
PDF_circle(PDF *p, float x, float y, float r);

/* Draw a circular arc with center (x, y) and radius r from alpha1 to alpha2. */
PDFLIB_API void PDFLIB_CALL
PDF_arc(PDF *p, float x, float y, float r, float alpha1, float alpha2);

/* Draw a rectangle at lower left (x, y) with width and height. */
PDFLIB_API void PDFLIB_CALL
PDF_rect(PDF *p, float x, float y, float width, float height);

/* Close the current path. */
PDFLIB_API void PDFLIB_CALL
PDF_closepath(PDF *p);

/* Path Painting and Clipping Functions */

/* Stroke the path with the current color and line width,and clear it. */
PDFLIB_API void PDFLIB_CALL
PDF_stroke(PDF *p);

/* Close the path, and stroke it. */
PDFLIB_API void PDFLIB_CALL
PDF_closepath_stroke(PDF *p);

/* Fill the interior of the path with the current fill color. */
PDFLIB_API void PDFLIB_CALL
PDF_fill(PDF *p);

/* Fill and stroke the path with the current fill and stroke color. */
PDFLIB_API void PDFLIB_CALL
PDF_fill_stroke(PDF *p);

/* Close the path, fill, and stroke it. */
PDFLIB_API void PDFLIB_CALL
PDF_closepath_fill_stroke(PDF *p);

/* End the current path. Deprecated, use one of the stroke, fill, or clip
 functions instead.  */
PDFLIB_API void PDFLIB_CALL
PDF_endpath(PDF *p);

/* Use the current path as clipping path. */
PDFLIB_API void PDFLIB_CALL
PDF_clip(PDF *p);

/* 
 * ----------------------------------------------------------------------
 * p_color.c
 * ----------------------------------------------------------------------
 */

/* Color Functions */

/* Set the current fill color to a gray value between 0 and 1 inclusive. */
PDFLIB_API void PDFLIB_CALL
PDF_setgray_fill(PDF *p, float g);

/* Set the current stroke color to a gray value between 0 and 1 inclusive. */
PDFLIB_API void PDFLIB_CALL
PDF_setgray_stroke(PDF *p, float g);

/* Set the current fill and stroke color. */
PDFLIB_API void PDFLIB_CALL
PDF_setgray(PDF *p, float g);

/* Set the current fill color to the supplied RGB values. */
PDFLIB_API void PDFLIB_CALL
PDF_setrgbcolor_fill(PDF *p, float red, float green, float blue);

/* Set the current stroke color to the supplied RGB values. */
PDFLIB_API void PDFLIB_CALL
PDF_setrgbcolor_stroke(PDF *p, float red, float green, float blue);

/* Set the current fill and stroke color to the supplied RGB values. */
PDFLIB_API void PDFLIB_CALL
PDF_setrgbcolor(PDF *p, float red, float green, float blue);

#ifdef PDF_CMYK_SUPPORTED

/* Set the current fill color to the supplied CMYK values. */
PDFLIB_API void PDFLIB_CALL
PDF_setcmykcolor_fill(PDF *p, float cyan, float magenta, float yellow, float black);

/* Set the current stroke color to the supplied CMYK values. */
PDFLIB_API void PDFLIB_CALL
PDF_setcmykcolor_stroke(PDF *p, float cyan, float magenta, float yellow, float black);

/* Set the current fill and stroke color to the supplied CMYK values. */
PDFLIB_API void PDFLIB_CALL
PDF_setcmykcolor(PDF *p, float cyan, float magenta, float yellow, float black);

#endif /* PDF_CMYK_SUPPORTED */

/* 
 * ----------------------------------------------------------------------
 * p_image.c
 * ----------------------------------------------------------------------
 */

/* Image Functions */

/* Place an image at the lower left corner (x, y), and scale it. */
PDFLIB_API void PDFLIB_CALL
PDF_place_image(PDF *p, int image, float x, float y, float scale);

/* Use image data from a variety of data sources. Returns an image descriptor
 or -1. Supported types are "jpeg", "ccitt", "raw". Supported sources are
 "memory", "fileref", "url". len is only used for type="raw", params is only
 used for type="ccitt". */
PDFLIB_API int PDFLIB_CALL
PDF_open_image(PDF *p, const char *type, const char *source, const char *data, long length, int width, int height, int components, int bpc, const char *params);

/* Open an image for later use. Returns an image descriptor or -1. Supported
 types are "jpeg", "tiff", "gif", and "png" (depending on configuration,
 however). stringparam is either "", "mask", "masked", or "page". intparam
 is either 0, the image number of the applied mask, or the page. */
PDFLIB_API int PDFLIB_CALL
PDF_open_image_file(PDF *p, const char *type, const char *filename, const char *stringparam, int intparam);

/* Close an image retrieved with one of the PDF_open_image*() functions. */
PDFLIB_API void PDFLIB_CALL
PDF_close_image(PDF *p, int image);

#ifdef PDF_THUMBNAILS_SUPPORTED
/* Add an existing image as thumbnail for the current page. PDFlib doesn't
help with preparing the thumbnail, but simply places it in the output. */
PDFLIB_API void PDFLIB_CALL
PDF_add_thumbnail(PDF *p, int im);
#endif	/* PDF_THUMBNAILS_SUPPORTED */

/* 
 * ----------------------------------------------------------------------
 * p_ccitt.c
 * ----------------------------------------------------------------------
 */

/* Open a raw CCITT image for later use. Returns an image descriptor or -1. */
PDFLIB_API int PDFLIB_CALL
PDF_open_CCITT(PDF *p, const char *filename, int width, int height, int BitReverse, int K, int BlackIs1);

/* 
 * ----------------------------------------------------------------------
 * p_hyper.c
 * ----------------------------------------------------------------------
 */

/* Hypertext Functions, Bookmarks */

/* Add a nested bookmark under parent, or a new top-level bookmark if 
 parent = 0. text may be Unicode. Returns a bookmark descriptor which may be
 used as parent for subsequent nested bookmarks. If open = 1, child
 bookmarks will be folded out, and invisible if open = 0. */
PDFLIB_API int PDFLIB_CALL
PDF_add_bookmark(PDF *p, const char *text, int parent, int open);

/* Document Information Fields */

/* Fill document information field key with value. value may be Unicode. */
PDFLIB_API void PDFLIB_CALL
PDF_set_info(PDF *p, const char *key, const char *value);

/* 
 * ----------------------------------------------------------------------
 * p_annots.c
 * ----------------------------------------------------------------------
 */

/* File Attachments */

/* Add a file attachment annotation. description and author may be Unicode. 
 icon is one of "graph, "paperclip", "pushpin", or "tag". */
PDFLIB_API void PDFLIB_CALL
PDF_attach_file(PDF *p, float llx, float lly, float urx, float ury, const char *filename, const char *description, const char *author, const char *mimetype, const char *icon);

/* Note Annotations */

/* Add a note annotation. contents and title may be Unicode. icon is one
 of "comment, "insert", "note", "paragraph", "newparagraph", "key", or "help".
 */
PDFLIB_API void PDFLIB_CALL
PDF_add_note(PDF *p, float llx, float lly, float urx, float ury, const char *contents, const char *title, const char *icon, int open);

/* Links */

/* Add a file link annotation (to a PDF file). */
PDFLIB_API void PDFLIB_CALL
PDF_add_pdflink(PDF *p, float llx, float lly, float urx, float ury, const char *filename, int page, const char *dest);

/* Add a launch annotation (arbitrary file type). */
PDFLIB_API void PDFLIB_CALL
PDF_add_launchlink(PDF *p, float llx, float lly, float urx, float ury, const char *filename);

/* Add a link annotation with a target within the current file. dest can be
 "fullpage" or "fitwidth". */
PDFLIB_API void PDFLIB_CALL
PDF_add_locallink(PDF *p, float llx, float lly, float urx, float ury, int page, const char *dest);

/* Add a weblink annotation. */
PDFLIB_API void PDFLIB_CALL
PDF_add_weblink(PDF *p, float llx, float lly, float urx, float ury, const char *url);

/* Set the border style for all kinds of annotations.
 These settings are used for all annotations until a new style is set. 
 Supported border style names are "solid" and "dashed". */
PDFLIB_API void PDFLIB_CALL
PDF_set_border_style(PDF *p, const char *style, float width);

/* Set the border color for all kinds of annotations. */
PDFLIB_API void PDFLIB_CALL
PDF_set_border_color(PDF *p, float red, float green, float blue);

/* Set the border dash style for all kinds of annotations. See PDF_setdash(). */
PDFLIB_API void PDFLIB_CALL
PDF_set_border_dash(PDF *p, float b, float w);

/* 
 * ----------------------------------------------------------------------
 * Convenience stuff
 * ----------------------------------------------------------------------
 */

/* Page Size Formats */

/*
Although PDF doesn´t impose any restrictions on the usable page size,
Acrobat implementations suffer from architectural limits concerning
the page size.
Although PDFlib will generate PDF documents with page sizes outside
these limits, the default error handler will issue a warning message.

Acrobat 3 minimum page size: 1" = 72 pt = 2.54 cm
Acrobat 3 maximum page size: 45" = 3240 pt = 114.3 cm
Acrobat 4 minimum page size: 0.25" = 18 pt = 0.635 cm
Acrobat 4 maximum page size: 200" = 14400 pt = 508 cm
*/

/* The page sizes are only available to the C and C++ bindings */
#ifndef SWIG
#define a0_width	 (float) 2380.0
#define a0_height	 (float) 3368.0
#define a1_width	 (float) 1684.0
#define a1_height	 (float) 2380.0
#define a2_width	 (float) 1190.0
#define a2_height	 (float) 1684.0
#define a3_width	 (float) 842.0
#define a3_height	 (float) 1190.0
#define a4_width	 (float) 595.0
#define a4_height	 (float) 842.0
#define a5_width	 (float) 421.0
#define a5_height	 (float) 595.0
#define a6_width	 (float) 297.0
#define a6_height	 (float) 421.0
#define b5_width	 (float) 501.0
#define b5_height	 (float) 709.0
#define letter_width	 (float) 612.0
#define letter_height	 (float) 792.0
#define legal_width 	 (float) 612.0
#define legal_height 	 (float) 1008.0
#define ledger_width	 (float) 1224.0
#define ledger_height	 (float) 792.0
#define p11x17_width	 (float) 792.0
#define p11x17_height	 (float) 1224.0
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif	/* PDFLIB_H */
