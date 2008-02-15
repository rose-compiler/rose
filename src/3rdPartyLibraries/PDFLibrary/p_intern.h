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

/* p_intern.h
 *
 * PDFlib internal definitions
 *
 */

#ifndef P_INTERN_H
#define P_INTERN_H

#include <stdio.h>

#include "pdflib.h"
#include "p_config.h"

#ifdef HAVE_LIBTIFF
#include <tiffio.h>
#endif

#ifdef HAVE_LIBZ
#include <zlib.h>
#endif

#ifdef HAVE_LIBPNG
#include <png.h>
#endif

#define pdf_false	0
#define pdf_true	1
typedef int pdf_bool;

#define RESOURCEFILE		"PDFLIBRESOURCE"
#define DEFAULTRESOURCEFILE	"pdflib.upr"

#define	PDF_1_2			12
#define	PDF_1_3			13

/*
 * Allocation chunk sizes. These don't affect the generated documents
 * in any way. In order to save initial memory, however, you can lower 
 * the values. Increasing the values will bring some performance gain
 * for large documents.
 */
#define PAGES_CHUNKSIZE		512		/* pages */
#define ID_CHUNKSIZE		2048		/* object ids */
#define CONTENTS_CHUNKSIZE	64		/* page content streams */
#define FONTS_CHUNKSIZE		16		/* document fonts */
#define XOBJECTS_CHUNKSIZE	128		/* document xobjects */
#define IMAGES_CHUNKSIZE	128		/* document images */
#define OUTLINE_CHUNKSIZE	256		/* document outlines */
#define STREAM_CHUNKSIZE	65536		/* output buffer for in-core generation*/

/* The following shouldn't require any changes */
#define ENC_MAX			32		/* max number of encodings */
#define PDF_MAX_SAVE_LEVEL	10		/* max number of save levels */
#define FLOATBUFSIZE		20		/* buffer length for floats */
#define PDF_FILENAMELEN		1024		/* maximum file name length */
#define PDF_MAX_PARAMSTRING	128		/* image parameter string */
#define PDF_DEFAULT_COMPRESSION	6		/* default zlib level */

/* configurable flush points */
#define PDF_FLUSH_NONE		0		/* end of document */
#define PDF_FLUSH_PAGE		(1<<0)		/* after page */
#define PDF_FLUSH_CONTENT	(1<<1)		/* font, xobj, annots */
#define PDF_FLUSH_RESERVED1	(1<<2)		/* reserved */
#define PDF_FLUSH_RESERVED2	(1<<3)		/* reserved */
#define PDF_FLUSH_HEAVY		(1<<4)		/* before realloc attempt */

/*
 * An arbitrary number used as a sanity check for the PDF pointer.
 * Actually, we use the hex representation of pi in order to avoid
 * the more common patterns.
 */
#define PDF_MAGIC	((unsigned long) 0x126960A1)

/*
 * When there's no PDF pointer we don't have access to an error handler.
 * Since the client completely screwed up, we refuse to do
 * anything in the hope the client will recognize his trouble.
 */

#define PDF_SANITY_CHECK_FAILED(p)	(p == NULL || p->magic != PDF_MAGIC)

/* Border styles for links */
typedef enum {
    border_solid, border_dashed, border_beveled,
    border_inset, border_underlined
} pdf_border_style;

/* Transition types for page transition effects */
typedef enum {
    trans_none, trans_split, trans_blinds, trans_box, 
    trans_wipe, trans_dissolve, trans_glitter, trans_replace
} pdf_transition;

/* icons for file attachments and text annotations */
typedef enum {
    icon_file_graph, icon_file_paperclip,
    icon_file_pushpin, icon_file_tag,

    icon_text_comment, icon_text_insert,
    icon_text_note, icon_text_paragraph,
    icon_text_newparagraph, icon_text_key,
    icon_text_help
} pdf_icon;

/*
 * Symbolic names for predefined font encodings. 0 and above are used
 * as indices in the pdf_encoding_vector array.
 */
typedef enum {
    cid = -2, builtin = -1, winansi = 0, macroman = 1, ebcdic = 3
} pdf_encoding;

typedef enum { ImageB = 1, ImageC = 2, ImageI = 4, Text = 8 } pdf_procset;

/* The color spaces are PDF standard except ImageMask, which we use
 * internally.
 */
typedef enum {
    DeviceGray, DeviceRGB, DeviceCMYK, 
    CalGray, CalRGB, Lab, 
    Indexed, PatternCS, Separation,
    ImageMask
} pdf_colorspace;

/* the different types of graphics object within a page description */
typedef enum { c_none, c_page, c_path, c_text } pdf_content_type;

typedef long id;

typedef struct {
    char *apiname;	/* PDFlib's name of the encoding at the API */
    char *pdfname;	/* PDF's official name of the encoding */
    char *chars[256];	/* the actual character names */
} pdf_encodingvector;

typedef enum { none, lzw, runlength, ccitt, dct, flate } pdf_compression;

typedef unsigned char pdf_byte;

typedef pdf_byte pdf_colormap[256][3];

#define NEW_ID		0L
#define BAD_ID		-1L

/* Note: pdf_begin_obj() is a function */
#define pdf_end_obj(p)		pdf_puts(p, "endobj\n")

#define pdf_begin_dict(p)	pdf_puts(p, "<<")
#define pdf_end_dict(p)		pdf_puts(p, ">>\n")

#define pdf_begin_stream(p)	pdf_puts(p, "stream\n")
#define pdf_end_stream(p)	pdf_puts(p, "endstream\n")

/* some ASCII characters, deliberately defined as hex codes */
#define PDF_NEWLINE		((char) 0x0a)		/* ASCII '\n' */
#define PDF_RETURN		((char) 0x0d)		/* ASCII '\r' */
#define PDF_SPACE		((char) 0x20)		/* ASCII ' '  */
#define PDF_BACKSLASH		((char) 0x5c)		/* ASCII '\\' */
#define PDF_PARENLEFT		((char) 0x28)		/* ASCII '('  */
#define PDF_PARENRIGHT		((char) 0x29)		/* ASCII ')'  */
#define PDF_EXCLAM		((char) 0x21)		/* ASCII '!'  */
#define PDF_COMMA		((char) 0x2c)		/* ASCII ','  */
#define PDF_SEMICOLON		((char) 0x3b)		/* ASCII ';'  */
#define PDF_n			((char) 0x6e)		/* ASCII 'n'  */
#define PDF_r			((char) 0x72)		/* ASCII 'r'  */
#define PDF_z			((char) 0x7a)		/* ASCII 'z'  */

#ifdef PDFLIB_EBCDIC
#define pdf_make_ascii(s)	pdf_ebcdic2ascii(s)
#else
#define pdf_make_ascii(s)	(s)
#endif

/* The Unicode byte order mark (BOM) signals Unicode strings */
#define PDF_BOM0		0376		/* '\xFE' */
#define PDF_BOM1		0377		/* '\xFF' */

/*
 * check whether the string is plain C or unicode by looking for the BOM
 * s must not be NULL.
 */
#define pdf_is_unicode(s) \
	(((unsigned char *)(s))[0] != 0 &&		\
	 ((unsigned char *)(s))[0] == PDF_BOM0 &&	\
	 ((unsigned char *)(s))[1] == PDF_BOM1)

/* Document open modes */
typedef enum {
    open_auto, open_none, open_bookmarks, open_thumbnails, open_fullscreen
} pdf_openmode;

/* Destination types for internal and external links */
typedef enum { retain, fitpage, fitwidth, fitheight, fitbbox } pdf_desttype;

/* Destination structure for bookmarks and links */
typedef struct {
    pdf_desttype type;
    int page;
} pdf_dest;

typedef struct { float llx, lly, urx, ury; } PDF_rectangle;

/* A PDF xobject */
typedef struct {
    id		obj_id;			/* object id of this xobject */
    pdf_bool	used_on_current_page;	/* this xobject used on current page */
} pdf_xobject;

typedef struct pdf_res_s pdf_res;

struct pdf_res_s {
    char		*name;
    char		*filename;
    pdf_res		*next;
};

typedef struct pdf_category_s pdf_category;

struct pdf_category_s {
    char		*category;
    pdf_res		*kids;
    pdf_category	*next;
};

typedef struct pdf_outline_s pdf_outline;
struct pdf_outline_s {
    id			self;		/* id of this outline object */
    id			prev;		/* previous entry at this level */
    id			next;		/* next entry at this level */
    int			parent;		/* ancestor's index */
    int			first;		/* first sub-entry */
    int			last;		/* last sub-entry */
    char 		*text;		/* bookmark text */
    int			count;		/* number of open sub-entries */
    int			open;		/* whether or not to display children */
    pdf_dest		dest;		/* destination */
};

/* Internal PDFlib states for error checking */
typedef enum {
    pdf_state_null, pdf_state_open,
    pdf_state_page_description, pdf_state_path, pdf_state_text
} pdf_state;

typedef struct { float a, b, c, d, e, f; } pdf_matrix;

/* Annotation types */
typedef enum {
    ann_text, ann_locallink,
    ann_pdflink, ann_weblink,
    ann_launchlink, ann_attach
} pdf_annot_type;

/* Fill rules */
typedef enum {pdf_fill_winding, pdf_fill_evenodd} pdf_fillrule;

/* Annotations */
typedef struct pdf_annot_s pdf_annot;
struct pdf_annot_s {
    pdf_annot_type	type;		/* used for all annotation types */
    PDF_rectangle	rect;		/* used for all annotation types */
    id			obj_id;		/* used for all annotation types */
    pdf_annot		*next;		/* used for all annotation types */

    pdf_icon		icon;		/* attach and text */
    char		*filename;	/* attach, launchlink, pdflink,weblink*/
    char		*contents;	/* text, attach, pdflink */
    char		*mimetype;	/* attach */

    char		*title;		/* text */
    int			open;		/* text */
    pdf_dest		dest;		/* locallink, pdflink */

    /* -------------- annotation border style and color -------------- */
    pdf_border_style	border_style;
    float		border_width;
    float		border_red;
    float		border_green;
    float		border_blue;
    float		border_dash1;
    float		border_dash2;
};

struct pdf_stream_s {
    pdf_byte	*basepos;		/* start of this chunk */
    pdf_byte	*curpos;		/* next write position */
    pdf_byte	*maxpos;		/* maximum position of chunk */

    size_t	base_offset;		/* base offset of this chunk */

    int		flush;			/* flushing strategy */

    pdf_bool	compress;		/* in compression state */

#ifdef HAVE_LIBZ
    z_stream	z;			/* zlib compression stream */
#endif
};

typedef struct pdf_image_s pdf_image;
typedef struct pdf_font_s pdf_font;
typedef struct pdf_stream_s pdf_stream;

/* -------------------- special graphics state -------------------- */
typedef struct {
#ifdef CTM_NYI
    pdf_matrix	ctm;		/* current transformation matrix */
#endif
    float	x;		/* current x coordinate */
    float	y;		/* current y coordinate */

    float	startx;		/* starting x point of the subpath */
    float	starty;		/* starting y point of the subpath */

    /* other general gstate parameters nyi */

} pdf_gstate;

/* ------------------------ text state ---------------------------- */
typedef struct {
    float	c;		/* character spacing */
    float	w;		/* word spacing */
    float	h;		/* horizontal scaling */
    float	l;		/* leading */
    int		f;		/* slot number of the current font */
    float	fs;		/* font size */
    pdf_matrix	m;		/* text matrix */
    int		mode;		/* text rendering mode */
    float	rise;		/* text rise */
    pdf_matrix	lm;		/* text line matrix */
} pdf_tstate;

typedef struct {
    pdf_colorspace	cs;
    union {
	float		gray;
	struct {
	    float	r;
	    float	g;
	    float	b;
	} rgb; 
#ifdef PDF_CMYK_SUPPORTED
	struct {
	    float	c;
	    float	m;
	    float	y;
	    float	k;
	} cmyk; 
#endif
    } val;
} pdf_color;
    
/* ----------------------- color state --------------------------- */
typedef struct {
    pdf_color	fill;
    pdf_color	stroke;
} pdf_cstate;

/*
 * *************************************************************************
 * The core PDF document descriptor
 * *************************************************************************
 */

struct PDF_s {
    /* -------------------------- general stuff ------------------------ */
    unsigned long	magic;		/* poor man's integrity check */

    char	*binding;		/* name of the language binding */
    char	*prefix;		/* prefix for resource file names */
    int		compatibility;		/* PDF version number * 10 */

    /* ------------------- PDF Info dictionary entries ----------------- */
    char	*Keywords;
    char	*Subject;
    char	*Title;
    char	*Creator;
    char	*Author;
    /* user-defined key and value */
    char	*userkey;		/* ASCII string */
    char	*userval;		/* Unicode string */

    /* ------------------------- PDF output ---------------------------- */
    FILE	*fp;
    char	*filename;
    pdf_stream	stream;
    /* writeproc == NULL means we're generating PDF in-core */
    size_t	(*writeproc)(PDF *p, void *data, size_t size);

    /* -------------- error handling and memory management ------------- */
    pdf_bool	in_error;		/* pdf_true if in error handler */
    void 	(*errorhandler)(PDF *p, int level, const char* msg);
    void	*(*malloc)(PDF *p, size_t size, const char *caller);
    void	*(*calloc)(PDF *p, size_t size, const char *caller);
    void	*(*realloc)(PDF *p, void *mem, size_t size, const char *caller);
    void	 (*free)(PDF *p, void *mem);
    void	*opaque;		/* user-specific, opaque data */

    /* ------------------------ resource stuff ------------------------- */
    pdf_category *resources;		/* anchor for the resource list */
    char	*resourcefilename;	/* name of the resource file */
    pdf_bool	resourcefile_loaded;	/* already loaded the resource file */

    /* ------------------- object and id bookkeeping ------------------- */
    id		root_id;
    id		info_id;
    id		pages_id;
    pdf_openmode open_mode;		/* document open mode */
    pdf_desttype open_action;		/* open action/first page display */
    pdf_desttype bookmark_dest;		/* destination type for bookmarks */

    long	*file_offset;		/* the objects' file offsets */
    int		file_offset_capacity;
    id		currentobj;

    id		*pages;			/* page ids */
    int		pages_capacity;
    int 	current_page;		/* current page number (1-based) */

    /* ------------------- document resources ------------------- */
    pdf_font	*fonts;			/* all fonts in document */
    int		fonts_capacity;		/* currently allocated size */
    int		fonts_number;		/* next available font number */

    pdf_xobject *xobjects;		/* all xobjects in document */
    int		xobjects_capacity;	/* currently allocated size */
    int		xobjects_number;	/* next available xobject number */

    pdf_image  *images;			/* all images in document */
    int		images_capacity;	/* currently allocated size */

    /* ------------------- document outline tree ------------------- */
    int		outline_capacity;	/* currently allocated size */
    int		outline_count;		/* total number of outlines */
    pdf_outline	*outlines;		/* dynamic array of outlines */

    /* ------------------- page specific stuff ------------------- */
    pdf_state	state;			/* state within the library */
    id		res_id;			/* id of this page's res dict */
    id		contents_length_id;	/* id of current cont section's length*/
    id		*contents_ids;		/* content sections' chain */
    int		contents_ids_capacity;	/* # of content sections */
    id		next_content;		/* # of current content section */
    pdf_content_type	contents;	/* type of current content section */
    pdf_transition	transition;	/* type of page transition */
    float	duration;		/* duration of page transition */

    pdf_annot	*annots;		/* annotation chain */

    int		procset;		/* procsets for this page */
    long	start_contents_pos;	/* start offset of contents section */

    float	width;			/* current page's width */
    float	height;			/* current page's height */
    id		thumb_id;		/* id of this page's thumb, or BAD_ID */

    int		sl;				/* current save level */
    pdf_gstate	gstate[PDF_MAX_SAVE_LEVEL];	/* graphics state */
    pdf_tstate	tstate[PDF_MAX_SAVE_LEVEL];	/* text state */
    pdf_cstate	cstate[PDF_MAX_SAVE_LEVEL];	/* color state */

    /* ------------ other text and graphics-related stuff ------------ */
    pdf_bool	leading_done;		/* leading value already set */
    pdf_bool	underline;		/* underline text */
    pdf_bool	overline;		/* overline text */
    pdf_bool	strikeout;		/* strikeout text */

    pdf_fillrule	fillrule;	/* nonzero or evenodd fill rule */

    /* -------------- annotation border style and color -------------- */
    pdf_border_style	border_style;
    float		border_width;
    float		border_red;
    float		border_green;
    float		border_blue;
    float		border_dash1;
    float		border_dash2;

    /* ------------------------ miscellaneous ------------------------ */
    int		chars_on_this_line;
    int		compress;		/* zlib compression level */
    char	debug[128];		/* debug flags */

    /* the list of known encoding vectors */
    pdf_encodingvector *encodings[ENC_MAX];
};

/* Data source for images, compression, ASCII encoding, fonts, etc. */
typedef struct PDF_data_source_s PDF_data_source;
struct PDF_data_source_s {
    pdf_byte 		*next_byte;
    size_t		bytes_available;
    void		(*init)(PDF *, PDF_data_source *src);
    int			(*fill)(PDF *, PDF_data_source *src);
    void		(*terminate)(PDF *, PDF_data_source *src);

    pdf_byte		*buffer_start;
    size_t		buffer_length;
    void		*private_data;
};

/* JPEG specific image information */
typedef struct pdf_jpeg_info_t {
    long		startpos;	/* start of image data in file */
} pdf_jpeg_info;

/* GIF specific image information */
typedef struct pdf_gif_info_t {
    int			c_size;		/* current code size (9..12)	*/
    int			t_size;		/* current table size (257...)	*/
    int			i_buff;		/* input buffer			*/
    int			i_bits;		/* # of bits in i_buff		*/
    int			o_buff;		/* output buffer		*/
    int			o_bits;		/* # of bits in o_buff		*/
} pdf_gif_info;

#ifdef HAVE_LIBPNG

/* PNG specific image information */
typedef struct pdf_png_info_t {
	png_structp	png_ptr;
	png_infop	info_ptr;
	png_uint_32	rowbytes;
	pdf_byte	*raster;
	int		cur_line;
} pdf_png_info;

#endif	/* HAVE_LIBPNG */

#ifdef HAVE_LIBTIFF

/* TIFF specific image information */
typedef struct pdf_tiff_info_t {
    TIFF		*tif;		/* pointer to TIFF data structure */
    uint32		*raster;	/* frame buffer */
    int			cur_line;	/* current image row, or strip for multi-strip */
    pdf_bool		use_raw;	/* use raw (compressed) image data */
} pdf_tiff_info;

#endif	/* HAVE_LIBTIFF */

/* CCITT specific image information */
typedef struct pdf_ccitt_info_t {
    int			BitReverse;	/* reverse all bits prior to use */
} pdf_ccitt_info;

/* Type of image reference */
typedef enum {pdf_ref_direct, pdf_ref_file, pdf_ref_url} pdf_ref_type;

/* The image descriptor */
struct pdf_image_s {
    FILE		*fp;		/* image file pointer */
    char		*filename;	/* image file name or url */
    pdf_ref_type	reference;	/* kind of image data reference */
    int			width;		/* image width in pixels */
    int			height;		/* image height in pixels */
    int			bpc;		/* bits per color component */
    int			components;	/* number of color components */
    pdf_compression	compression;	/* image compression type */
    pdf_colorspace	colorspace;	/* image color space */

    pdf_bool		invert;		/* invert: reverse black and white */
    pdf_bool		transparent;	/* image is transparent */
    pdf_byte		transval[4];	/* transparent color values */
    int			mask;		/* image number of image mask, or -1 */
    int			predictor;	/* predictor for lzw and flate compression */

    int			palette_size;	/* # of entries (not bytes!) in color palette */
    pdf_colormap 	*colormap;	/* pointer to color palette */

    float		dpi_x;		/* horiz. resolution in dots per inch */
    float		dpi_y;		/* vert. resolution in dots per inch */
    					/* dpi is 0 if unknown */

    pdf_bool		in_use;		/* image slot currently in use */

    char		*params;	/* K and BlackIs1 for TIFF and CCITT */
    int			strips;		/* number of strips in image */
    int			rowsperstrip;	/* number of rows per strip */
    id			colormap_id;	/* reuse colormap for multiple strips */

    /* image format specific information */
    union {
	pdf_jpeg_info	jpeg;

	pdf_gif_info	gif;

#ifdef HAVE_LIBPNG
	pdf_png_info	png;
#endif

#ifdef HAVE_LIBTIFF
	pdf_tiff_info	tiff;
#endif

	pdf_ccitt_info	ccitt;
    } info;

    int			no;		/* PDF image number */
    PDF_data_source	src;
};

/* ------ Private functions for library-internal use only --------- */

/* p_basic.c */
void	pdf_begin_contents_section(PDF *p);
void	pdf_end_contents_section(PDF *p);
void	pdf_error(PDF *, int level, const char *fmt, ...);
id	pdf_begin_obj(PDF *p, id obj_id);
id	pdf_alloc_id(PDF *p);
void	pdf_grow_pages(PDF *p);

/* p_color.c */
void	pdf_init_cstate(PDF *p);

/* p_draw.c */
void	pdf_end_path(PDF *p, pdf_bool force_endpath);

/* p_text.c */
void	pdf_init_tstate(PDF *p);
void	pdf_begin_text(PDF *p, pdf_bool setpos);
void	pdf_end_text(PDF *p);
void	pdf_quote_string(PDF *p, const char *string);
void	pdf_set_leading(PDF *p, float leading);
void	pdf_set_text_rise(PDF *p, float rise);
void	pdf_set_horiz_scaling(PDF *p, float scale);
void	pdf_set_text_rendering(PDF *p, int mode);
void	pdf_set_char_spacing(PDF *p, float spacing);
void	pdf_set_word_spacing(PDF *p, float spacing);
const char *pdf_get_fontname(PDF *p);
const char *pdf_get_fontencoding(PDF *p);
float	pdf_get_fontsize(PDF *p);
int	pdf_get_font(PDF *p);


/* p_gstate.c */
void	pdf_init_gstate(PDF *p);
void	pdf_concat_raw(PDF *p, pdf_matrix m);

/* p_image.c */
void	pdf_init_images(PDF *p);
void	pdf_cleanup_images(PDF *p);
void	pdf_init_xobjects(PDF *p);
void	pdf_write_xobjects(PDF *p);
void	pdf_grow_xobjects(PDF *p);
void	pdf_cleanup_xobjects(PDF *p);
void	pdf_put_image(PDF *p, int im, pdf_bool firststrip);
void	pdf_grow_images(PDF *p);


/* p_filter.c */
void	pdf_ASCII85Encode(PDF *p, PDF_data_source *src);
void	pdf_ASCIIHexEncode(PDF *p, PDF_data_source *src);

void	pdf_data_source_file_init(PDF *p, PDF_data_source *src);
int	pdf_data_source_file_fill(PDF *p, PDF_data_source *src);
void	pdf_data_source_file_terminate(PDF *p, PDF_data_source *src);

void	pdf_copy(PDF *p, PDF_data_source *src);
void	pdf_compress(PDF *p, PDF_data_source *src);

/* p_font.c */
void	pdf_init_fonts(PDF *p);
void	pdf_init_font_struct(PDF *p, pdf_font *font);
void	pdf_write_page_fonts(PDF *p);
void	pdf_write_doc_fonts(PDF *p);
void	pdf_cleanup_fonts(PDF *p);
void	pdf_make_fontflags(PDF *p, pdf_font *font);

/* p_afm.c */
pdf_bool	pdf_get_metrics_afm(PDF *p, pdf_font *font, const char *fontname, int enc, const char *filename);
void	pdf_cleanup_font_struct(PDF *p, pdf_font *font);

/* p_pfm.c */
pdf_bool pdf_get_metrics_pfm(PDF *p, pdf_font *font, const char *fontname, int enc, const char *filename);

/* p_annots.c */
void	pdf_init_annots(PDF *p);
void	pdf_init_page_annots(PDF *p);
void	pdf_write_page_annots(PDF *p);
void	pdf_cleanup_page_annots(PDF *p);

/* p_hyper.c */
void	pdf_init_transition(PDF *p);
void	pdf_init_outlines(PDF *p);
void	pdf_write_outlines(PDF *p);
void	pdf_cleanup_outlines(PDF *p);

void	pdf_init_info(PDF *p);
void	pdf_write_info(PDF *p);
void	pdf_cleanup_info(PDF *p);

void	pdf_set_transition(PDF *p, const char *type);
void	pdf_set_duration(PDF *p, float t);

/* p_stream.c */
long	pdf_tell(PDF *p);
void	pdf_write(PDF *p, const void *data, size_t size);
void	pdf_puts(PDF *p, const char *s);
void	pdf_putc(PDF *p, char c);
void	pdf_printf(PDF *p, const char *fmt, ...);

void	pdf_init_stream(PDF *p);
void	pdf_flush_stream(PDF *p);
void	pdf_close_stream(PDF *p);

#ifdef PDFLIB_EBCDIC
void	pdf_ebcdic2ascii(char *s);
#endif

#ifdef HAVE_LIBZ
void	pdf_compress_init(PDF *p);
void	pdf_compress_end(PDF *p);
#endif

/* p_util.c */
const char   *pdf_float(char *buf, float f);
char   *pdf_strdup(PDF *p, const char *text);
size_t	pdf_strlen(const char *text);

char   *pdf_find_resource(PDF *p, const char *category, const char *resourcename, pdf_bool loadupr);
void	pdf_add_resource(PDF *p, const char *category, const char *resource,
		const char *filename, const char *prefix);
void	pdf_cleanup_resources(PDF *p);
int	pdf_load_encoding(PDF *p, const char *filename, const char *encoding);

#ifdef HAVE_LIBTIFF
/* p_tiff.c */
int pdf_open_TIFF_data(PDF *p, int imageslot, const char *filename, const char *stringparam, int intparam);
#endif

/* p_gif.c */
int pdf_open_GIF_data(PDF *p, int imageslot, const char *filename, const char *stringparam, int intparam);

/* p_jpeg.c */
int pdf_open_JPEG_data(PDF *p, int imageslot, const char *filename, const char *stringparam, int intparam);

#ifdef HAVE_LIBPNG
/* p_png.c */
int pdf_open_PNG_data(PDF *p, int imageslot, const char *filename, const char *stringparam, int intparam);
#endif

#endif	/* P_INTERN_H */
