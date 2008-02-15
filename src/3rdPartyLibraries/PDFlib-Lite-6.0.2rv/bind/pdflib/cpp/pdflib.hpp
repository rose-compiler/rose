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

// $Id: pdflib.hpp,v 1.1 2005/10/11 17:18:08 vuduc2 Exp $
//
// in sync with pdflib.h 1.151.2.22
//
// C++ wrapper for PDFlib
//
//

#ifndef PDFLIB_HPP
#define PDFLIB_HPP

#include <string>

	using namespace std;

// We use PDF as a C++ class name, therefore hide the actual C struct
// name for PDFlib usage with C++.

#include "pdflib.h"

// The C++ class wrapper for PDFlib

class PDFlib {
public:
    class Exception
    {
    public:
	Exception(string errmsg, int errnum, string apiname, void *opaque);
	string get_errmsg();
	int get_errnum();
	string get_apiname();
	const void *get_opaque();
    private:
	string m_errmsg;
	int m_errnum;
	string m_apiname;
	void * m_opaque;
    }; // Exception

    PDFlib(allocproc_t allocproc = NULL,
	reallocproc_t reallocproc = NULL,
	freeproc_t freeproc = NULL,
	void *opaque = NULL) throw(Exception);

    ~PDFlib() throw();

    void activate_item(int id) throw(PDFlib::Exception);
    int add_bookmark(string text, int parent, int p_open)
    	throw(PDFlib::Exception);
    void add_launchlink(double llx, double lly, double urx, double ury,
	string filename) throw(PDFlib::Exception);
    void add_locallink(double llx, double lly, double urx, double ury, int page,
	string optlist) throw(PDFlib::Exception);
    void add_nameddest(string name, string optlist) throw(PDFlib::Exception);
    void add_note(double llx, double lly, double urx, double ury,
    	string contents,
	string title, string icon, int p_open) throw(PDFlib::Exception);
    void add_pdflink(double llx, double lly, double urx, double ury,
	string filename, int page, string optlist) throw(PDFlib::Exception);
    void add_thumbnail(int image) throw(PDFlib::Exception);
    void add_weblink(double llx, double lly, double urx, double ury,
	string url) throw(PDFlib::Exception);
    void arc(double x, double y, double r, double alpha, double beta)
	throw(PDFlib::Exception);
    void arcn(double x, double y, double r, double alpha, double beta)
	throw(PDFlib::Exception);
    void attach_file(double llx, double lly, double urx, double ury,
	string filename, string description, string author,
	string mimetype, string icon) throw(PDFlib::Exception);
    int begin_document(string filename, string optlist)
	throw(PDFlib::Exception);
    void begin_font(string fontname, double a, double b,
	double c, double d, double e, double f, string optlist)
	throw(PDFlib::Exception);
    void begin_glyph(string glyphname, double wx, double llx, double lly,
	double urx, double ury) throw(PDFlib::Exception);
    int begin_item(string tag, string optlist) throw(PDFlib::Exception);
    void begin_layer(int layer) throw(PDFlib::Exception);
    void begin_page(double width, double height) throw(PDFlib::Exception);
    void begin_page_ext(double width, double height, string optlist)
	throw(PDFlib::Exception);
    int begin_pattern(double width, double height, double xstep, double ystep,
	int painttype) throw(PDFlib::Exception);
    int begin_template(double width, double height) throw(PDFlib::Exception);
    void circle(double x, double y, double r) throw(PDFlib::Exception);
    void clip() throw(PDFlib::Exception);
    void close() throw(PDFlib::Exception);
    void close_image(int image) throw(PDFlib::Exception);
    void close_pdi(int doc) throw(PDFlib::Exception);
    void close_pdi_page(int page) throw(PDFlib::Exception);
    void closepath() throw(PDFlib::Exception);
    void closepath_fill_stroke() throw(PDFlib::Exception);
    void closepath_stroke() throw(PDFlib::Exception);
    void concat(double a, double b, double c, double d, double e, double f)
	throw(PDFlib::Exception);
    void continue_text(string text) throw(PDFlib::Exception);
    int create_action(string type, string optlist) throw(PDFlib::Exception);
    void create_annotation(double llx, double lly, double urx, double ury,
	string type, string optlist) throw(PDFlib::Exception);
    int create_bookmark(string text, string optlist) throw(PDFlib::Exception);
    void create_field(double llx, double lly, double urx, double ury,
	string name, string type, string optlist) throw(PDFlib::Exception);
    void create_fieldgroup(string name, string optlist)
    	throw(PDFlib::Exception);
    int create_gstate (string optlist) throw(PDFlib::Exception);
    void create_pvf(string filename, const void *data, size_t size,
    	string optlist) throw(PDFlib::Exception);
    int create_textflow(string text, string optlist) throw(PDFlib::Exception);
    void curveto(double x1, double y1, double x2, double y2, double x3,
    	double y3) throw(PDFlib::Exception);
    int define_layer(string name, string optlist) throw(PDFlib::Exception);
    int delete_pvf(string filename) throw(PDFlib::Exception);
    void delete_textflow(int textflow) throw(PDFlib::Exception);
    void encoding_set_char(string encoding, int slot, string glyphname, int uv)
	throw(PDFlib::Exception);
    void end_document(string optlist) throw(PDFlib::Exception);
    void end_font() throw(PDFlib::Exception);
    void end_glyph() throw(PDFlib::Exception);
    void end_item(int id) throw(PDFlib::Exception);
    void end_layer() throw(PDFlib::Exception);
    void end_page() throw(PDFlib::Exception);
    void end_page_ext(string optlist) throw(PDFlib::Exception);
    void end_pattern() throw(PDFlib::Exception);
    void end_template() throw(PDFlib::Exception);
    void endpath() throw(PDFlib::Exception);
    void fill() throw(PDFlib::Exception);
    int fill_imageblock(int page, string blockname, int image, string optlist)
	throw(PDFlib::Exception);
    int fill_pdfblock(int page, string blockname, int contents, string optlist)
	throw(PDFlib::Exception);
    int fill_textblock(int page, string blockname, string text, string optlist)
	throw(PDFlib::Exception);
    void fill_stroke() throw(PDFlib::Exception);
    int findfont(string fontname, string encoding, int embed)
    	throw(PDFlib::Exception);
    void fit_image (int image, double x, double y, string optlist)
	throw(PDFlib::Exception);
    void fit_pdi_page (int page, double x, double y, string optlist)
	throw(PDFlib::Exception);
    string fit_textflow(int textflow, double llx, double lly, double urx,
	double ury, string optlist) throw(PDFlib::Exception);
    void fit_textline(string text, double x, double y, string optlist)
	throw(PDFlib::Exception);
    string get_apiname() throw(PDFlib::Exception);
    const char * get_buffer(long *size) throw(PDFlib::Exception);
    string get_errmsg() throw(PDFlib::Exception);
    int get_errnum() throw(PDFlib::Exception);
    void * get_opaque() throw(PDFlib::Exception);
    string get_parameter(string key, double modifier) throw(PDFlib::Exception);
    double get_pdi_value(string key, int doc, int page, int reserved)
	throw(PDFlib::Exception);
    string get_pdi_parameter(string key, int doc, int page, int reserved,
    	int *len = NULL) throw(PDFlib::Exception);
    double get_value(string key, double modifier) throw(PDFlib::Exception);
    double info_textflow(int textflow, string keyword) throw(PDFlib::Exception);
    void initgraphics() throw(PDFlib::Exception);
    void lineto(double x, double y) throw(PDFlib::Exception);
    int load_font(string fontname, string encoding, string optlist)
	    throw(PDFlib::Exception);
    int load_iccprofile(string profilename, string optlist)
	throw(PDFlib::Exception);
    int load_image (string imagetype, string filename, string optlist)
	    throw(PDFlib::Exception);
    int makespotcolor(string spotname) throw(PDFlib::Exception);
    void moveto(double x, double y) throw(PDFlib::Exception);
    int open_CCITT(string filename, int width, int height, int BitReverse,
	int K, int BlackIs1) throw(PDFlib::Exception);
    int open_file(string filename) throw(PDFlib::Exception);
    int open_image(string imagetype, string source, const char *data, long len,
	int width, int height, int components, int bpc, string params)
	throw(PDFlib::Exception);
    int open_image_file(string imagetype, string filename,
	string stringparam, int intparam) throw(PDFlib::Exception);
    void open_mem(writeproc_t writeproc) throw(PDFlib::Exception);
    int open_pdi(string filename, string optlist, int reserved)
    	throw(PDFlib::Exception);
    int open_pdi_page(int doc, int pagenumber, string optlist)
    	throw(PDFlib::Exception);
    void place_image(int image, double x, double y, double p_scale)
	throw(PDFlib::Exception);
    void place_pdi_page(int page, double x, double y, double sx, double sy)
	throw(PDFlib::Exception);
    int process_pdi(int doc, int page, string optlist) throw(PDFlib::Exception);
    void rect(double x, double y, double width, double height)
    	throw(PDFlib::Exception);
    void restore() throw(PDFlib::Exception);
    void resume_page(string optlist) throw(PDFlib::Exception);
    void rotate(double phi) throw(PDFlib::Exception);
    void save() throw(PDFlib::Exception);
    void scale(double sx, double sy) throw(PDFlib::Exception);
    void set_border_color(double red, double green, double blue)
    	throw(PDFlib::Exception);
    void set_border_dash(double b, double w) throw(PDFlib::Exception);
    void set_border_style(string style, double width) throw(PDFlib::Exception);
    void setfont(int font, double fontsize) throw(PDFlib::Exception);
    void set_gstate(int gstate) throw(PDFlib::Exception);
    void set_info(string key, string value) throw(PDFlib::Exception);
    void set_layer_dependency(string type, string optlist)
    	throw(PDFlib::Exception);
    void set_parameter(string key, string value) throw(PDFlib::Exception);
    void set_text_pos(double x, double y) throw(PDFlib::Exception);
    void set_value(string key, double value) throw(PDFlib::Exception);
    void setcolor(string fstype, string colorspace,
	double c1, double c2, double c3, double c4) throw(PDFlib::Exception);
    void setdash(double b, double w) throw(PDFlib::Exception);
    void setdashpattern(string optlist) throw(PDFlib::Exception);
    void setflat(double flatness) throw(PDFlib::Exception);
    void setlinecap(int linecap) throw(PDFlib::Exception);
    void setlinejoin(int linejoin) throw(PDFlib::Exception);
    void setlinewidth(double width) throw(PDFlib::Exception);
    void setmatrix( double a, double b, double c, double d, double e, double f)
	throw(PDFlib::Exception);
    void setmiterlimit(double miter) throw(PDFlib::Exception);
    void setpolydash(float *darray, int length) throw(PDFlib::Exception);
    int shading (string shtype, double x0, double y0, double x1, double y1,
	double c1, double c2, double c3, double c4, string optlist)
	throw(PDFlib::Exception);
    int shading_pattern (int shade, string optlist) throw(PDFlib::Exception);
    void shfill (int shade) throw(PDFlib::Exception);
    void show(string text) throw(PDFlib::Exception);
    int show_boxed(string text, double left, double top,
	double width, double height, string hmode, string feature)
	throw(PDFlib::Exception);
    void show_xy(string text, double x, double y) throw(PDFlib::Exception);
    void skew(double alpha, double beta) throw(PDFlib::Exception);
    double stringwidth(string text, int font, double fontsize)
    	throw(PDFlib::Exception);
    void stroke() throw(PDFlib::Exception);
    void suspend_page(string optlist) throw(PDFlib::Exception);
    void translate(double tx, double ty) throw(PDFlib::Exception);
    string utf16_to_utf8(string utf16string) throw(PDFlib::Exception);
    string utf8_to_utf16(string utf8string, string format)
    	throw(PDFlib::Exception);
    void xshow(string text, const double *xadvancelist)
    	throw(PDFlib::Exception);

private:
    PDF *p;
    const PDFlib_api *m_PDFlib_api;
};

#endif	// PDFLIB_HPP
