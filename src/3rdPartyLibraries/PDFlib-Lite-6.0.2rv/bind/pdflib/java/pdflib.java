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

/* $Id: pdflib.java,v 1.1 2005/10/11 17:18:08 vuduc2 Exp $
 *
 * PDFlib Java class
 */

package com.pdflib;

import java.io.*;

/** PDFlib -- a library for generating PDF on the fly.

    Note that this is only a syntax summary. It covers several products:
    PDFlib Lite, PDFlib, PDFlib+PDI, and PDFlib Personalization Server (PPS).
    Not all features are available in all products, although dummies for
    all missing API functions are provided. A comparison which
    details function availability in different products is available at
    http://www.pdflib.com.

    For complete information please refer to the PDFlib API reference
    manual which is available in the PDF file PDFlib-manual.pdf in the
    PDFlib distribution.

    @author Thomas Merz
    @version 6.0.2
*/

public final class pdflib {

    // The initialization code for loading the PDFlib shared library.
    // The library name will be transformed into something platform-
    // specific by the VM, e.g. libpdf_java.so or pdf_java.dll.

    static {
	try {
	    System.loadLibrary("pdf_java");
	} catch (UnsatisfiedLinkError e) {
	    System.err.println(
	"Cannot load the PDFlib shared library/DLL for Java.\n" +
	"Make sure to properly install the native PDFlib library.\n\n" +
	"For your information, the current value of java.library.path is:\n" +
	 System.getProperty("java.library.path") + "\n");

	    throw e;
	}
	PDF_boot();
    }

    // ------------------------------------------------------------------------
    // public functions

    /** Activate a previously created structure element or other content item.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final void activate_item(int id)
    throws PDFlibException
    {
	PDF_activate_item(p, id);
    }

    /** @deprecated Use  PDF_create_bookmark().
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final int add_bookmark(String text, int parent, int open)
    throws PDFlibException
    {
        return PDF_add_bookmark(p, text, parent, open);
    }

    /** @deprecated Use PDF_create_action() and PDF_create_annotation().
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void add_launchlink(
    double llx, double lly, double urx, double ury, String filename)
    throws PDFlibException
    {
	PDF_add_launchlink(p, llx, lly, urx, ury, filename);
    }

    /** @deprecated Use PDF_create_action() and PDF_create_annotation().
	@exception com.pdflib.PDFlibException
	PDF output cannot be finished after an exception.
    */
    public final void add_locallink(
    double llx, double lly, double urx, double ury, int page, String optlist)
    throws PDFlibException
    {
	PDF_add_locallink(p, llx, lly, urx, ury, page, optlist);
    }

    /** Create a named destination on an arbitrary page in the current document.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void add_nameddest(String name, String optlist)
    throws PDFlibException
    {
	PDF_add_nameddest(p, name, optlist);
    }

    /** @deprecated Use PDF_create_annotation() instead.
	@exception com.pdflib.PDFlibException
	PDF output cannot be finished after an exception.
     */
    public final void add_note(
    double llx, double lly, double urx, double ury,
    String contents, String title, String icon, int open)
    throws PDFlibException
    {
	PDF_add_note(p, llx, lly, urx, ury, contents, title, icon, open);
    }

    /** @deprecated Use PDF_create_action() and PDF_create_annotation().
	@exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void add_pdflink(
    double llx, double lly, double urx, double ury,
    String filename, int page, String optlist)
    throws PDFlibException
    {
	PDF_add_pdflink(p, llx, lly, urx, ury, filename, page, optlist);
    }

    /** Add an existing image as thumbnail for the current page.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void add_thumbnail(int image)
    throws PDFlibException
    {
	PDF_add_thumbnail(p, image);
    }

    /** @deprecated Use PDF_create_action() and PDF_create_annotation().
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void add_weblink(
    double llx, double lly, double urx, double ury, String url)
    throws PDFlibException
    {
	PDF_add_weblink(p, llx, lly, urx, ury, url);
    }

    /** Draw a counterclockwise circular arc segment.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final void arc(double x, double y, double r, double alpha, double beta)
    throws PDFlibException
    {
	PDF_arc(p, x, y, r, alpha, beta);
    }

    /** Draw a clockwise circular arc segment.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void arcn(double x, double y, double r, double alpha, double beta)
    throws PDFlibException
    {
	PDF_arcn(p, x, y, r, alpha, beta);
    }

    /** @deprecated Use  PDF_create_annotation() instead.
	@exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final void attach_file(
    double llx, double lly, double urx, double ury, String filename,
    String description, String author, String mimetype, String icon)
    throws PDFlibException
    {
	PDF_attach_file(p, llx, lly, urx, ury, filename,
	    description, author, mimetype, icon);
    }

    /** Create a new PDF file subject to various options.
        @return -1 on error, and 1 otherwise.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final int begin_document(String filename, String optlist)
    throws PDFlibException
    {
        return PDF_begin_document(p, filename, optlist);
    }

    /** Start a Type 3 font definition.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void begin_font(
    String name, double a, double b, double c, double d, double e, double f,
	String optlist)
    throws PDFlibException
    {
	PDF_begin_font(p, name, a, b, c, d, e, f, optlist);
    }

    /** Start a glyph definition for a Type 3 font.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void begin_glyph(
    String glyphname, double wx, double llx, double lly, double urx, double ury)
    throws PDFlibException
    {
	PDF_begin_glyph(p, glyphname, wx, llx, lly, urx, ury);
    }

    /** Open a structure element or other content item with attributes supplied
        as options.
        @return An item handle which can be used in subsequent
	item-related calls.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final int begin_item(String tag, String optlist)
    throws PDFlibException
    {
        return PDF_begin_item(p, tag, optlist);
    }

    /** Start a layer for subsequent output on the page (requires PDF 1.5).
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void begin_layer(int layer)
    throws PDFlibException
    {
        PDF_begin_layer(p, layer);
    }

    /** @deprecated Use PDF_begin_page_ext().
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void begin_page(double width, double height)
    throws PDFlibException
    {
        PDF_begin_page(p, width, height);
    }

    /** Add a new page to the document, and specify various options.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void begin_page_ext(double width, double height, String optlist)
    throws PDFlibException
    {
        PDF_begin_page_ext(p, width, height, optlist);
    }

    /** Start a pattern definition.
        @return A pattern handle that can be used in subsequent calls to
	PDF_setcolor( ) during the enclosing document scope.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final int begin_pattern(
    double width, double height, double xstep, double ystep, int painttype)
    throws PDFlibException
    {
	return PDF_begin_pattern(p, width, height, xstep, ystep, painttype);
    }

    /** Start a template definition.
        @return  A template handle which can be used in subsequent
	image-related calls, especially PDF_ fit_image().
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final int begin_template(double width, double height)
    throws PDFlibException
    {
	return PDF_begin_template(p, width, height);
    }

    /** Draw a circle.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void circle(double x, double y, double r)
    throws PDFlibException
    {
	PDF_circle(p, x, y, r);
    }

    /** Use the current path as clipping path, and terminate the path.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void clip()
    throws PDFlibException
    {
	PDF_clip(p);
    }

    /** @deprecated Use PDF_end_document().
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void close()
    throws PDFlibException
    {
        PDF_close(p);
    }

    /** Close an image.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void close_image(int image)
    throws PDFlibException
    {
	PDF_close_image(p, image);
    }

    /** Close all open page handles, and close the input PDF document.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void close_pdi(int doc)
    throws PDFlibException
    {
	PDF_close_pdi(p, doc);
    }

    /** Close the page handle, and free all page-related resources.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void close_pdi_page(int page)
    throws PDFlibException
    {
	PDF_close_pdi_page(p, page);
    }

    /** Close the current path.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void closepath()
    throws PDFlibException
    {
	PDF_closepath(p);
    }

    /** Close the path, fill, and stroke it.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void closepath_fill_stroke()
    throws PDFlibException
    {
	PDF_closepath_fill_stroke(p);
    }

    /** Close the path, and stroke it.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void closepath_stroke()
    throws PDFlibException
    {
	PDF_closepath_stroke(p);
    }

    /** Concatenate a matrix to the current transformation matrix.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void concat(
    double a, double b, double c, double d, double e, double f)
    throws PDFlibException
    {
	PDF_concat(p, a, b, c, d, e, f);
    }

    /** Print text at the next line.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void continue_text(String text)
    throws PDFlibException
    {
	PDF_continue_text(p, text);
    }

    /** Create an action which can be applied to various objects and events.
        @return A template handle which can be used in subsequent image-related
	calls, especially PDF_fit_image().
	@exception com.pdflib.PDFlibException
	PDF output cannot be finished after an exception.
    */
    public final int create_action(String type, String optlist)
    throws PDFlibException
    {
	return PDF_create_action(p, type, optlist);
    }

    /** Create a rectangular annotation on the current page.
	@exception com.pdflib.PDFlibException
	PDF output cannot be finished after an exception.
    */
    public final void create_annotation(
    double llx, double lly, double urx, double ury, String type, String optlist)
    throws PDFlibException
    {
        PDF_create_annotation(p, llx, lly, urx, ury, type, optlist);
    }

    /** Create a bookmark subject to various options.
        @return A handle for the generated bookmark, which may be used with
	the parent option in subsequent calls.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final int create_bookmark(String text, String optlist)
    throws PDFlibException
    {
        return PDF_create_bookmark(p, text, optlist);
    }

    /** Create a form field on the current page subject to various options.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void create_field(double llx, double lly, double urx, double ury,
    String name, String type, String optlist)
    throws PDFlibException
    {
        PDF_create_field(p, llx, lly, urx, ury, name, type, optlist);
    }

    /** Create a form field group subject to various options.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void create_fieldgroup(
    String name, String optlist)
    throws PDFlibException
    {
        PDF_create_fieldgroup(p, name, optlist);
    }

    /** Create a graphics state object subject to various options.
        @return A handle for the generated bookmark, which may be used with
	the parent option in subsequent calls.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final int create_gstate(String optlist)
    throws PDFlibException
    {
        return PDF_create_gstate(p, optlist);
    }

    /** Create a named virtual read-only file from data provided in memory.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void create_pvf(String filename, byte[] data, String optlist)
    throws PDFlibException
    {
	PDF_create_pvf(p, filename, data, optlist);
    }

    /** Preprocess text for later formatting and create a textflow object.
        @return A textflow handle which can be used in calls to
	PDF_fit_textflow(), PDF_info_textflow(), and PDF_delete_textflow().
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final int create_textflow(String text, String optlist)
    throws PDFlibException
    {
        return PDF_create_textflow(p, text, optlist);
    }

    /** Draw a Bezier curve from the current point, using 3 more control points.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final void curveto(
    double x1, double y1, double x2, double y2, double x3, double y3)
    throws PDFlibException
    {
	PDF_curveto(p, x1, y1, x2, y2, x3, y3);
    }

    /** Create a new layer definition (requires PDF 1.5).
        @return A layer handle which can be used in calls to PDF_begin_layer()
	and PDF_set_layer_dependency().
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final int define_layer(
    String name, String optlist)
    throws PDFlibException
    {
	return PDF_define_layer(p, name, optlist);
    }

    /** Define hierarchical and group relationships among layers (requires
        PDF 1.5).
        @return This function returs -1 in case of an error, and 1 otherwise.
	Malformed option lists will throw an exception, however.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final void set_layer_dependency(
    String type, String optlist)
    throws PDFlibException
    {
	PDF_set_layer_dependency(p, type, optlist);
    }

    /** Delete a PDFlib object and free all internal resources.
        Never throws any PDFlib exception.
     */
    public final void delete()
    {
        PDF_delete(p);
        p = (long) 0;
    }

    /** Delete a named virtual file and free its data structures (but not the
        contents).
        @return -1 if the corresponding virtual file exists but is locked,
	and 1 otherwise.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final int delete_pvf(String filename)
    throws PDFlibException
    {
	return PDF_delete_pvf(p, filename);
    }

    /** Delete a textflow and the associated data structures.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void delete_textflow(int textflow)
    throws PDFlibException
    {
        PDF_delete_textflow(p, textflow);
    }

    /** Add a glyph name and/or Unicode value to a custom encoding.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void encoding_set_char(
    String encoding, int slot, String glyphname, int uv)
    throws PDFlibException
    {
        PDF_encoding_set_char(p, encoding, slot, glyphname, uv);
    }

    /** Close the generated PDF file and apply various options.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void end_document(String optlist)
    throws PDFlibException
    {
        PDF_end_document(p, optlist);
    }

    /** Terminate a Type 3 font definition.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void end_font()
    throws PDFlibException
    {
	PDF_end_font(p);
    }

    /** Terminate a glyph definition for a Type 3 font.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void end_glyph()
    throws PDFlibException
    {
	PDF_end_glyph(p);
    }

    /** Close a structure element or other content item.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void end_item(int id)
    throws PDFlibException
    {
	PDF_end_item(p, id);
    }

    /** Deactivate all active layers (requires PDF 1.5).
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void end_layer()
    throws PDFlibException
    {
	PDF_end_layer(p);
    }

    /** @deprecated Use PDF_end_page_ext().
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void end_page()
    throws PDFlibException
    {
        PDF_end_page(p);
    }

    /** Finish a page, and apply various options.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void end_page_ext(String optlist)
    throws PDFlibException
    {
        PDF_end_page_ext(p, optlist);
    }

    /** Finish a pattern definition.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void end_pattern()
    throws PDFlibException
    {
	PDF_end_pattern(p);
    }

    /** Finish a template definition.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void end_template()
    throws PDFlibException
    {
	PDF_end_template(p);
    }

    /** End the current path without filling or stroking it.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void endpath()
    throws PDFlibException
    {
	PDF_endpath(p);
    }

    /** Fill the interior of the path with the current fill color.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void fill()
    throws PDFlibException
    {
	PDF_fill(p);
    }

    /** Fill an image block with variable data according to its properties.
        @return -1 on error, and 1 otherwise.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final int fill_imageblock(
    int page, String blockname, int image, String optlist)
    throws PDFlibException
    {
	return PDF_fill_imageblock(p, page, blockname, image, optlist);
    }

    /** Fill a PDF block with variable data according to its properties.
        @return -1 on error, and 1 otherwise.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final int fill_pdfblock(
    int page, String blockname, int contents, String optlist)
    throws PDFlibException
    {
	return PDF_fill_pdfblock(p, page, blockname, contents, optlist);
    }

    /** Fill and stroke the path with the current fill and stroke color.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void fill_stroke()
    throws PDFlibException
    {
	PDF_fill_stroke(p);
    }

    /** Fill a text block with variable data according to its properties.
        @return -1 on error, and 1 otherwise.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final int fill_textblock(
    int page, String blockname, String text, String optlist)
    throws PDFlibException
    {
	return PDF_fill_textblock(p, page, blockname, text, optlist);
    }

    /** @deprecated Use  PDF_load_font().
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final int findfont(String fontname, String encoding, int options)
    throws PDFlibException
    {
	return PDF_findfont(p, fontname, encoding, options);
    }

    /** Place an image or template at on the page, subject to various options.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void fit_image(int image, double x, double y, String optlist)
    throws PDFlibException
    {
	PDF_fit_image(p, image, x, y, optlist);
    }

    /** Place an imported PDF page on the page subject to various options.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void fit_pdi_page(int page, double x, double y, String optlist)
    throws PDFlibException
    {
	PDF_fit_pdi_page(p, page, x, y, optlist);
    }

    /** Format the next portion of a textflow into a rectangular area.
        @return A string which specifies the reason for returning from the
	function.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final String fit_textflow(int textflow, double llx, double lly,
    double urx, double ury, String optlist)
    throws PDFlibException
    {
        return PDF_fit_textflow(p, textflow, llx, lly, urx, ury, optlist);
    }

    /** Place a single line of text at position (x, y) subject to
        various options.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void fit_textline(
    String text, double x, double y, String optlist)
    throws PDFlibException
    {
	PDF_fit_textline(p, text, x, y, optlist);
    }

    /** Get the name of the API function which threw the last exception
        or failed.
        @return The name of the function which threw an exception,
	or the name of the most recently called function which failed with
	an error code.
     */
    public final String get_apiname()
    {
        return PDF_get_apiname(p);
    }

    /** Get the contents of the PDF output buffer.
        @return A byte[] full of binary PDF data for consumption by the
	client.  The returned buffer must be used by the client before
	calling any other PDFlib function.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final byte[] get_buffer()
    throws PDFlibException
    {
        return PDF_get_buffer(p);
    }

    /** Get the descriptive text of the last thrown exception or the reason of
        a failed function call.
        @return Text containing the description of the last exception thrown,
	or the reason why the most recently called function failed with
	an error code.
     */
    public final String get_errmsg()
    {
        return PDF_get_errmsg(p);
    }

    /** Get the number of the last thrown exception or the reason of a failed
        function call.
        @return The number of an exception, or the reason code of the the most
	recently called function which failed with an error code.
     */
    public final int get_errnum()
    {
        return PDF_get_errnum(p);
    }

    /** Get the contents of some PDFlib parameter with string type.
        @return  The string value of the parameter. The returned string can be
	used until the end of the surrounding document scope.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final String get_parameter(String key, double modifier)
    throws PDFlibException
    {
	return PDF_get_parameter(p, key, modifier);
    }

    /** Get some PDI document parameter with string type.
        @return The string parameter retrieved from the document.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final String get_pdi_parameter(
    String key, int doc, int page, int reserved)
    throws PDFlibException
    {
	return PDF_get_pdi_parameter(p, key, doc, page, reserved);
    }

    /** Get some PDI document parameter with numerical type.
        @return The numerical value retrieved from the document.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final double get_pdi_value(
    String key, int doc, int page, int reserved)
    throws PDFlibException
    {
	return PDF_get_pdi_value(p, key, doc, page, reserved);
    }

    /** Get the value of some PDFlib parameter with numerical type.
        @return The numerical value of the parameter.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final double get_value(String key, double modifier)
    throws PDFlibException
    {
	return PDF_get_value(p, key, modifier);
    }

    /** Query the current state of a textflow.
        @return The value of some textflow parameter as requested by keyword.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final double info_textflow(int textflow, String keyword)
    throws PDFlibException
    {
        return PDF_info_textflow(p, textflow, keyword);
    }

    /** Reset all color and graphics state parameters to their defaults.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final void initgraphics()
    throws PDFlibException
    {
	PDF_initgraphics(p);
    }

    /** Draw a line from the current point to another point.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void lineto(double x, double y)
    throws PDFlibException
    {
	PDF_lineto(p, x, y);
    }

    /** Search for a font and prepare it for later use.
        @return A font handle for later use with PDF_setfont().
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final int load_font(String fontname, String encoding, String optlist)
    throws PDFlibException
    {
	return PDF_load_font(p, fontname, encoding, optlist);
    }

    /** Search for an ICC profile, and prepare it for later use.
        @return A profile handle which can be used in subsequent calls to
	PDF_load_image() or for setting profile-related parameters.
	The return value must be checked for -1 which signals an error.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final int load_iccprofile(String profilename, String optlist)
    throws PDFlibException
    {
	return PDF_load_iccprofile(p, profilename, optlist);
    }

    /** Open a disk-based or virtual image file subject to various options.
        @return An image handle which can be used in subsequent image-related
	calls. The return value must be checked for -1 which signals an error.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final int load_image(
    String imagetype, String filename, String optlist)
    throws PDFlibException
    {
	return PDF_load_image(p, imagetype, filename, optlist);
    }

    /** Find a built-in spot color name, or make a named spot color from the
        current fill color.
        @return A color handle which can be used in subsequent calls to
	PDF_setcolor() throughout the document.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final int makespotcolor(String spotname)
    throws PDFlibException
    {
	return PDF_makespotcolor(p, spotname);
    }

    /** Set the current point.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void moveto(double x, double y)
    throws PDFlibException
    {
	PDF_moveto(p, x, y);
    }

    /** Create a new PDFlib object with default settings.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public pdflib()
    throws PDFlibException
    {
        p = PDF_new();
    }

    /** @deprecated Use PDF_load_image().
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final int open_CCITT(
    String filename, int width, int height, int BitReverse, int K, int BlackIs1)
    throws PDFlibException
    {
	return PDF_open_CCITT(p, filename, width, height, BitReverse, K,
		BlackIs1);
    }

    /** @deprecated Use PDF_begin_document().
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final int open_file(String filename)
    throws PDFlibException
    {
        return PDF_open_file(p, filename);
    }

    /** @deprecated Use PDF_load_image() with virtual files.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final int open_image(
    String imagetype, String source, byte[] data, long length, int width,
    int height, int components, int bpc, String params)
    throws PDFlibException
    {
	return PDF_open_image(p, imagetype, source, data, length, width,
	    height, components, bpc, params);
    }

    /** @deprecated Use PDF_load_image().
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final int open_image_file(
    String imagetype, String filename, String stringparam, int intparam)
    throws PDFlibException
    {
	return PDF_open_image_file(p, imagetype, filename, stringparam,
		intparam);
    }

    /** Open a disk-based or virtual PDF document and prepare it for later use.
        @return A page handle which can be used for placing pages with
	PDF_fit_pdi_page(). A return value of -1 indicates that the page
	couldn't be opened.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final int open_pdi(String filename, String optlist, int reserved)
    throws PDFlibException
    {
	return PDF_open_pdi(p, filename, optlist, reserved);
    }

    /** Prepare a page for later use with PDF_fit_pdi_page().
        @return A page handle which can be used for placing pages with
	PDF_fit_pdi_page(). A return value of -1 indicates that the page
	couldn't be opened.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final int open_pdi_page(int doc, int pagenumber, String optlist)
    throws PDFlibException
    {
	return PDF_open_pdi_page(p, doc, pagenumber, optlist);
    }

    /** @deprecated Use PDF_fit_image().
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void place_image(int image, double x, double y, double scale)
    throws PDFlibException
    {
	PDF_place_image(p, image, x, y, scale);
    }

    /** @deprecated Use PDF_fit_pdi_page().
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void place_pdi_page(
    int page, double x, double y, double sx, double sy)
    throws PDFlibException
    {
	PDF_place_pdi_page(p, page, x, y, sx, sy);
    }

    /** Process certain elements of an imported PDF document.
        @return The value 1 if the function succeeded, or an error code of -1
	if the function call failed.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final double process_pdi(int doc, int page, String optlist)
    throws PDFlibException
    {
	return PDF_process_pdi(p, doc, page, optlist);
    }

    /** Draw a rectangle.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void rect(double x, double y, double width, double height)
    throws PDFlibException
    {
	PDF_rect(p, x, y, width, height);
    }

    /** Restore the most recently saved graphics state from the stack.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void restore()
    throws PDFlibException
    {
	PDF_restore(p);
    }

    /** Resume a page to add more content to it.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void resume_page(String optlist)
    throws PDFlibException
    {
	PDF_resume_page(p, optlist);
    }

    /** Rotate the coordinate system.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void rotate(double phi)
    throws PDFlibException
    {
	PDF_rotate(p, phi);
    }

    /** Save the current graphics state to a stack.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void save()
    throws PDFlibException
    {
	PDF_save(p);
    }

    /** Scale the coordinate system.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void scale(double sx, double sy)
    throws PDFlibException
    {
	PDF_scale(p, sx, sy);
    }

    /** @deprecated Use PDF_create_annotation().
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void set_border_color(double red, double green, double blue)
    throws PDFlibException
    {
	PDF_set_border_color(p, red, green, blue);
    }

    /** @deprecated Use PDF_create_annotation().
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final void set_border_dash(double b, double w)
    throws PDFlibException
    {
	PDF_set_border_dash(p, b, w);
    }

    /** @deprecated Use PDF_create_annotation().
        "dashed".
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final void set_border_style(String style, double width)
    throws PDFlibException
    {
	PDF_set_border_style(p, style, width);
    }

    /** Activate a gstate object.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void set_gstate(int gstate)
    throws PDFlibException
    {
	PDF_set_gstate(p, gstate);
    }

    /** Fill document information field key with value.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void set_info(String key, String value)
    throws PDFlibException
    {
	PDF_set_info(p, key, value);
    }

    /** Set some PDFlib parameter with string type.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void set_parameter(String key, String value)
    throws PDFlibException
    {
	PDF_set_parameter(p, key, value);
    }

    /** Set the position for text output on the page.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void set_text_pos(double x, double y)
    throws PDFlibException
    {
	PDF_set_text_pos(p, x, y);
    }

    /** Set the value of some PDFlib parameter with numerical type.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void set_value(String key, double value)
    throws PDFlibException
    {
	PDF_set_value(p, key, value);
    }

    /** Set the current color space and color.
        fstype is "fill", "stroke", or "fillstroke".
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final void setcolor(
    String fstype, String colorspace, double c1, double c2, double c3, double c4)
    throws PDFlibException
    {
	PDF_setcolor(p, fstype, colorspace, c1, c2, c3, c4);
    }

    /** Set the current dash pattern.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void setdash(double b, double w)
    throws PDFlibException
    {
	PDF_setdash(p, b, w);
    }

    /** Set a dash pattern defined by an option list.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void setdashpattern(String optlist)
    throws PDFlibException
    {
	PDF_setdashpattern(p, optlist);
    }

    /** Set the flatness parameter.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void setflat(double flatness)
    throws PDFlibException
    {
	PDF_setflat(p, flatness);
    }

    /** Set the current font in the specified size.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final void setfont(int font, double fontsize)
    throws PDFlibException
    {
	PDF_setfont(p, font, fontsize);
    }

    /** Set the linecap parameter.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void setlinecap(int linecap)
    throws PDFlibException
    {
	PDF_setlinecap(p, linecap);
    }

    /** Set the linejoin parameter.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void setlinejoin(int linejoin)
    throws PDFlibException
    {
	PDF_setlinejoin(p, linejoin);
    }

    /** Set the current linewidth.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void setlinewidth(double width)
    throws PDFlibException
    {
	PDF_setlinewidth(p, width);
    }

    /** Explicitly set the current transformation matrix.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void setmatrix(
    double a, double b, double c, double d, double e, double f)
    throws PDFlibException
    {
	PDF_setmatrix(p, a, b, c, d, e, f);
    }

    /** Set the miter limit.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void setmiterlimit(double miter)
    throws PDFlibException
    {
	PDF_setmiterlimit(p, miter);
    }

    /** @deprecated Use PDF_setdashpattern(). 
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void setpolydash(float[] dasharray)
    throws PDFlibException
    {
	PDF_setpolydash(p, dasharray);
    }

    /** Define a blend from the current fill color to another color (requires
        PDF 1.4 or above).
        to the supplied color.
        @return A shading handle that can be used in subsequent calls to
	PDF_shading_pattern() and PDF_shfill() during the enclosing document
	scope.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final int shading(
    String shtype, double x0, double y0, double x1, double y1,
    double c1, double c2, double c3, double c4, String optlist)
    throws PDFlibException
    {
	return PDF_shading(p, shtype, x0, y0, x1, y1, c1, c2, c3, c4, optlist);
    }

    /** Define a shading pattern using a shading object (requires PDF 1.4 or
        above).
        @return A pattern handle that can be used in subsequent calls to
	PDF_setcolor() during the enclosing document scope.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final int shading_pattern(int shading, String optlist)
    throws PDFlibException
    {
	return PDF_shading_pattern(p, shading, optlist);
    }

    /** Fill an area with a shading, based on a shading object (requires PDF 1.4
        or above).
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void shfill(int shading)
    throws PDFlibException
    {
	PDF_shfill(p, shading);
    }

    /** Print text in the current font and size at the current position.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void show(String text)
    throws PDFlibException
    {
	PDF_show(p, text);
    }

    /** @deprecated Use PDF_fit_textline() or PDF_fit_textflow().
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final int show_boxed(String text, double left, double bottom,
        double width, double height, String hmode, String feature)
    throws PDFlibException
    {
	return PDF_show_boxed(p, text, left, bottom, width, height,
	    	hmode, feature);
    }

    /** Print text in the current font.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void show_xy(String text, double x, double y)
    throws PDFlibException
    {
	PDF_show_xy(p, text, x, y);
    }

    /** Skew the coordinate system.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final void skew(double alpha, double beta)
    throws PDFlibException
    {
	PDF_skew(p, alpha, beta);
    }

    /** Return the width of text in an arbitrary font.
        @return The width of text in an arbitrary font which has been
	selected with PDF_load_font() and the supplied fontsize.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
     */
    public final double stringwidth(String text, int font, double fontsize)
    throws PDFlibException
    {
	return PDF_stringwidth(p, text, font, fontsize);
    }

    /** Stroke the path with the current color and line width, and clear it.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void stroke()
    throws PDFlibException
    {
	PDF_stroke(p);
    }

    /** Suspend the current page so that it can later be resumed.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void suspend_page(String optlist)
    throws PDFlibException
    {
	PDF_suspend_page(p, optlist);
    }

    /** Translate the origin of the coordinate system.
        @exception com.pdflib.PDFlibException
        PDF output cannot be finished after an exception.
    */
    public final void translate(double tx, double ty)
    throws PDFlibException
    {
	PDF_translate(p, tx, ty);
    }

    // ------------------------------------------------------------------------
    // private functions

    private long p;

    protected final void finalize()
    throws PDFlibException
    {
	PDF_delete(p);
	p = (long) 0;
    }

    private final static void classFinalize() {
	PDF_shutdown();
    }

    private final static native void PDF_boot();

    private final static native void PDF_activate_item(long jp, int jid) throws PDFlibException;
    private final static native int PDF_add_bookmark(long jp, String jarg1, int jarg2, int jarg3) throws PDFlibException;
    private final static native void PDF_add_launchlink(long jp, double jarg1, double jarg2, double jarg3, double jarg4, String jarg5) throws PDFlibException;
    private final static native void PDF_add_locallink(long jp, double jarg1, double jarg2, double jarg3, double jarg4, int jarg5, String jarg6) throws PDFlibException;
    private final static native void PDF_add_nameddest(long jp, String jname, String joptlist) throws PDFlibException;
    private final static native void PDF_add_note(long jp, double jarg1, double jarg2, double jarg3, double jarg4, String jarg5, String jarg6, String jarg7, int jarg8) throws PDFlibException;
    private final static native void PDF_add_pdflink(long jp, double jarg1, double jarg2, double jarg3, double jarg4, String jarg5, int jarg6, String jarg7) throws PDFlibException;
    private final static native void PDF_add_thumbnail(long jp, int jarg1) throws PDFlibException;
    private final static native void PDF_add_weblink(long jp, double jarg1, double jarg2, double jarg3, double jarg4, String jarg5) throws PDFlibException;
    private final static native void PDF_arc(long jp, double jarg1, double jarg2, double jarg3, double jarg4, double jarg5) throws PDFlibException;
    private final static native void PDF_arcn(long jp, double jarg1, double jarg2, double jarg3, double jarg4, double jarg5) throws PDFlibException;
    private final static native void PDF_attach_file(long jp, double jarg1, double jarg2, double jarg3, double jarg4, String jarg5, String jarg6, String jarg7, String jarg8, String jarg9) throws PDFlibException;
    private final static native int PDF_begin_document(long jp, String jarg1, String jarg2) throws PDFlibException;
    private final static native void PDF_begin_font(long jp, String jname, double ja, double jb, double jc, double jd, double je, double jf, String optlist) throws PDFlibException;
    private final static native void PDF_begin_glyph(long jp, String jname, double jwx, double jllx, double jlly, double jurx, double jury) throws PDFlibException;
    private final static native int PDF_begin_item(long jp, String jtag, String joptlist) throws PDFlibException;
    private final static native void PDF_begin_layer(long jp, int jlayer) throws PDFlibException;
    private final static native void PDF_begin_page(long jp, double jarg1, double jarg2) throws PDFlibException;
    private final static native void PDF_begin_page_ext(long jp, double jarg1, double jarg2, String jarg3) throws PDFlibException;
    private final static native int PDF_begin_pattern(long jp, double jarg1, double jarg2, double jarg3, double jarg4, int jarg5) throws PDFlibException;
    private final static native int PDF_begin_template(long jp, double jarg1, double jarg2) throws PDFlibException;
    private final static native void PDF_circle(long jp, double jarg1, double jarg2, double jarg3) throws PDFlibException;
    private final static native void PDF_clip(long jp) throws PDFlibException;
    private final static native void PDF_close_image(long jp, int jarg1) throws PDFlibException;
    private final static native void PDF_close(long jp) throws PDFlibException;
    private final static native void PDF_closepath_fill_stroke(long jp) throws PDFlibException;
    private final static native void PDF_closepath(long jp) throws PDFlibException;
    private final static native void PDF_closepath_stroke(long jp) throws PDFlibException;
    private final static native void PDF_close_pdi(long jp, int jarg1) throws PDFlibException;
    private final static native void PDF_close_pdi_page(long jp, int jarg1) throws PDFlibException;
    private final static native void PDF_concat(long jp, double a, double b, double c, double d, double e, double f) throws PDFlibException;
    private final static native void PDF_continue_text(long jp, String jarg1) throws PDFlibException;
    private final static native int PDF_create_action(long jp, String jtype, String joptlist) throws PDFlibException;
    private final static native void PDF_create_annotation(long jp, double jarg1, double jarg2, double jarg3, double jarg4, String jarg5, String jarg6) throws PDFlibException;
    private final static native int PDF_create_bookmark(long jp, String jarg1, String joptlist) throws PDFlibException;
    private final static native void PDF_create_field(long jp, double jarg1, double jarg2, double jarg3, double jarg4, String jname, String jtype, String joptlist) throws PDFlibException;
    private final static native void PDF_create_fieldgroup(long jp, String jname, String joptlist) throws PDFlibException;
    private final static native int PDF_create_gstate(long jp, String joptlist) throws PDFlibException;
    private final static native void PDF_create_pvf(long jp, String jfilename, byte[] jdata, String joptlist) throws PDFlibException;
    public final static native int PDF_create_textflow(long jp, String jtext, String joptlist) throws PDFlibException;
    private final static native void PDF_curveto(long jp, double jarg1, double jarg2, double jarg3, double jarg4, double jarg5, double jarg6) throws PDFlibException;
    private final static native int PDF_define_layer(long jp, String jname, String joptlist) throws PDFlibException;
    private final static native void PDF_set_layer_dependency(long jp, String jtype, String joptlist) throws PDFlibException;
    private final static native synchronized void PDF_delete(long jp);
    private final static native int PDF_delete_pvf(long jp, String jfilename) throws PDFlibException;
    public final static native void PDF_delete_textflow(long jp, int jtextflow) throws PDFlibException;
    private final static native void PDF_encoding_set_char(long jp, String jencoding, int jslot, String jglyphname, int juv) throws PDFlibException;
    private final static native int PDF_end_document(long jp, String jarg1) throws PDFlibException;
    private final static native void PDF_end_font(long jp) throws PDFlibException;
    private final static native void PDF_end_glyph(long jp) throws PDFlibException;
    private final static native void PDF_end_item(long jp, int jid) throws PDFlibException;
    private final static native void PDF_end_layer(long jp) throws PDFlibException;
    private final static native void PDF_end_page(long jp) throws PDFlibException;
    private final static native void PDF_end_page_ext(long jp, String jarg1) throws PDFlibException;
    private final static native void PDF_endpath(long jp) throws PDFlibException;
    private final static native void PDF_end_pattern(long jp) throws PDFlibException;
    private final static native void PDF_end_template(long jp) throws PDFlibException;
    private final static native int PDF_fill_imageblock(long jp, int jpage, String jblockname, int jimage, String joptlist) throws PDFlibException;
    private final static native void PDF_fill(long jp) throws PDFlibException;
    private final static native int PDF_fill_pdfblock(long jp, int jpage, String jblockname, int jcontents, String joptlist) throws PDFlibException;
    private final static native void PDF_fill_stroke(long jp) throws PDFlibException;
    private final static native int PDF_fill_textblock(long jp, int jpage, String jblockname, String jtext, String joptlist) throws PDFlibException;
    private final static native int PDF_findfont(long jp, String jarg1, String jarg2, int jarg3) throws PDFlibException;
    private final static native void PDF_fit_image(long jp, int jimage, double jx, double jy, String joptlist) throws PDFlibException;
    private final static native void PDF_fit_pdi_page(long jp, int jpage, double jx, double jy, String joptlist) throws PDFlibException;
    public final static native String PDF_fit_textflow(long jp, int jtextflow, double jllx, double jlly, double jurx, double jury, String joptlist) throws PDFlibException;
    private final static native void PDF_fit_textline(long jp, String jtext, double jx, double jy, String joptlist) throws PDFlibException;
    private final static native String PDF_get_apiname(long jp);
    private final static native byte[] PDF_get_buffer(long jp) throws PDFlibException;
    private final static native String PDF_get_errmsg(long jp);
    private final static native int PDF_get_errnum(long jp);
    private final static native String PDF_get_parameter(long jp, String key, double mod) throws PDFlibException;
    private final static native String PDF_get_pdi_parameter(long jp, String jarg2, int jarg3, int jarg4, int jarg5) throws PDFlibException;
    private final static native double PDF_get_pdi_value(long jp, String jarg1, int jarg2, int jarg3, int jarg4) throws PDFlibException;
    private final static native double PDF_get_value(long jp, String key, double mod) throws PDFlibException;
    public final static native double PDF_info_textflow(long jp, int jtextflow, String jkeyword) throws PDFlibException;
    private final static native void PDF_initgraphics(long jp) throws PDFlibException;
    private final static native void PDF_lineto(long jp, double jarg1, double jarg2) throws PDFlibException;
    private final static native int PDF_load_font(long jp, String jfontname, String jencoding, String joptlist) throws PDFlibException;
    private final static native int PDF_load_iccprofile(long jp, String jprofilename, String joptlist) throws PDFlibException;
    private final static native int PDF_load_image(long jp, String jimagetype, String jfilename, String joptlist) throws PDFlibException;
    private final static native int PDF_makespotcolor(long jp, String jarg1) throws PDFlibException;
    private final static native void PDF_moveto(long jp, double jarg1, double jarg2) throws PDFlibException;
    private final static native synchronized long PDF_new() throws PDFlibException;
    private final static native int PDF_open_CCITT(long jp, String jarg1, int jarg2, int jarg3, int jarg4, int jarg5, int jarg6) throws PDFlibException;
    private final static native int PDF_open_file(long jp, String jarg1) throws PDFlibException;
    private final static native int PDF_open_image_file(long jp, String jarg1, String jarg2, String jarg3, int jarg4) throws PDFlibException;
    private final static native int PDF_open_image(long jp, String jarg1, String jarg2, byte[] jarg3, long jarg4, int jarg5, int jarg6, int jarg7, int jarg8, String jarg9) throws PDFlibException;
    private final static native int PDF_open_pdi(long jp, String jarg1, String jarg2, int jarg3) throws PDFlibException;
    private final static native int PDF_open_pdi_page(long jp, int jarg1, int jarg2, String jarg3) throws PDFlibException;
    private final static native void PDF_place_image(long jp, int jarg1, double jarg2, double jarg3, double jarg4) throws PDFlibException;
    private final static native void PDF_place_pdi_page(long jp, int jarg1, double jarg2, double jarg3, double jarg4, double jarg5) throws PDFlibException;
    private final static native int PDF_process_pdi(long jp, int jdoc, int jpage, String joptlist) throws PDFlibException;
    private final static native void PDF_rect(long jp, double jarg1, double jarg2, double jarg3, double jarg4) throws PDFlibException;
    private final static native void PDF_restore(long jp) throws PDFlibException;
    private final static native void PDF_resume_page(long jp, String joptlist) throws PDFlibException;
    private final static native void PDF_rotate(long jp, double jarg1) throws PDFlibException;
    private final static native void PDF_save(long jp) throws PDFlibException;
    private final static native void PDF_scale(long jp, double jarg1, double jarg2) throws PDFlibException;
    private final static native void PDF_set_border_color(long jp, double jarg1, double jarg2, double jarg3) throws PDFlibException;
    private final static native void PDF_set_border_dash(long jp, double jarg1, double jarg2) throws PDFlibException;
    private final static native void PDF_set_border_style(long jp, String jarg1, double jarg2) throws PDFlibException;
    private final static native void PDF_setcolor(long jp, String jarg1, String jarg2, double jarg3, double jarg4, double jarg5, double jarg6) throws PDFlibException;
    private final static native void PDF_setdash(long jp, double jarg1, double jarg2) throws PDFlibException;
    private final static native void PDF_setdashpattern(long jp, String joptlist) throws PDFlibException;
    private final static native void PDF_setflat(long jp, double jarg1) throws PDFlibException;
    private final static native void PDF_setfont(long jp, int jarg1, double jarg2) throws PDFlibException;
    private final static native void PDF_set_gstate(long jp, int jhandle) throws PDFlibException;
    private final static native void PDF_set_info(long jp, String jarg1, String jarg2) throws PDFlibException;
    private final static native void PDF_setlinecap(long jp, int jarg1) throws PDFlibException;
    private final static native void PDF_setlinejoin(long jp, int jarg1) throws PDFlibException;
    private final static native void PDF_setlinewidth(long jp, double jarg1) throws PDFlibException;
    private final static native void PDF_setmatrix(long jp, double jarg1, double jarg2, double jarg3, double jarg4, double jarg5, double jarg6) throws PDFlibException;
    private final static native void PDF_setmiterlimit(long jp, double jarg1) throws PDFlibException;
    private final static native void PDF_set_parameter(long jp, String jarg1, String jarg2) throws PDFlibException;
    private final static native void PDF_setpolydash(long jp, float[] jarg1) throws PDFlibException;
    private final static native void PDF_set_text_pos(long jp, double jarg1, double jarg2) throws PDFlibException;
    private final static native void PDF_set_value(long jp, String jarg1, double jarg2) throws PDFlibException;
    private final static native int PDF_shading(long jp, String jshtype, double jx0, double jy0, double jx1, double jy1, double jc1, double jc2, double jc3, double jc4, String joptlist) throws PDFlibException;
    private final static native int PDF_shading_pattern(long jp, int jshading, String joptlist) throws PDFlibException;
    private final static native void PDF_shfill(long jp, int jshading) throws PDFlibException;
    private final static native int PDF_show_boxed(long jp, String jarg1, double jarg2, double jarg3, double jarg4, double jarg5, String jarg6, String jarg7) throws PDFlibException;
    private final static native void PDF_show(long jp, String jarg1) throws PDFlibException;
    private final static native void PDF_show_xy(long jp, String jarg1, double jarg2, double jarg3) throws PDFlibException;
    private final static native void PDF_shutdown();
    private final static native void PDF_skew(long jp, double jarg1, double jarg2) throws PDFlibException;
    private final static native double PDF_stringwidth(long jp, String jarg1, int jarg2, double jarg3) throws PDFlibException;
    private final static native void PDF_stroke(long jp) throws PDFlibException;
    private final static native void PDF_suspend_page(long jp, String joptlist) throws PDFlibException;
    private final static native void PDF_translate(long jp, double jarg1, double jarg2) throws PDFlibException;
}
