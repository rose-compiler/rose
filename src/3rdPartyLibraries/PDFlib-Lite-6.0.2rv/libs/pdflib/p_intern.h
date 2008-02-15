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

/* $Id: p_intern.h,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * PDFlib internal definitions
 *
 */

#ifndef P_INTERN_H
#define P_INTERN_H

#include "pdflib.h"

#include "pc_util.h"
#include "pc_geom.h"
#include "pc_file.h"
#include "pc_sbuf.h"
#include "pc_font.h"

#include "p_keyconn.h"


/* ------------------------ PDFlib feature configuration  ------------------- */

/* changing the following is not recommended, and not supported */

/* BMP image support */
#define PDF_BMP_SUPPORTED

/* GIF image support */
#define PDF_GIF_SUPPORTED

/* JPEG image support */
#define PDF_JPEG_SUPPORTED

/* JPEG2000 image support */
#define PDF_JPX_SUPPORTED

/* PNG image support, requires HAVE_LIBZ */
#define HAVE_LIBPNG

/* TIFF image support */
#define HAVE_LIBTIFF

/* TrueType font support */
#define PDF_TRUETYPE_SUPPORTED

/* support proportional widths for the standard CJK fonts */
#define PDF_CJKFONTWIDTHS_SUPPORTED


/* -------------------------------- macros ------------------------------- */

/*
 * Allocation chunk sizes. These don't affect the generated documents
 * in any way. In order to save initial memory, however, you can lower
 * the values. Increasing the values will bring some performance gain
 * for large documents, but will waste memory for small ones.
 */
#define PAGES_CHUNKSIZE        512              /* pages */
#define PNODES_CHUNKSIZE        64              /* page tree nodes */
#define CONTENTS_CHUNKSIZE      64              /* page content streams */
#define FONTS_CHUNKSIZE         16              /* document fonts */
#define ENCODINGS_CHUNKSIZE      8              /* document encodings */
#define XOBJECTS_CHUNKSIZE     128              /* document xobjects */
#define IMAGES_CHUNKSIZE       128              /* document images */
#define OUTLINE_CHUNKSIZE      256              /* document outlines */
#define NAMES_CHUNKSIZE        256              /* names */
#define PDI_CHUNKSIZE           16              /* PDI instances */
#define COLORSPACES_CHUNKSIZE   16              /* color spaces */
#define PATTERN_CHUNKSIZE        4              /* pattern */
#define SHADINGS_CHUNKSIZE       4              /* shadings */
#define EXTGSTATE_CHUNKSIZE      4              /* external graphic states */
#define T3GLYPHS_CHUNKSIZE     256              /* type 3 font glyph table */
#define ICCPROFILE_CHUNKSIZE     4              /* ICC profiles */
#define STRINGLISTS_CHUNKSIZE  128              /* document stringlists */
#define ACTIONS_CHUNKSIZE       16              /* document actions */
#define ITEMS_CHUNKSIZE        256              /* PDFlib items */
#define ITEMS_KIDS_CHUNKSIZE    64              /* PDFlib item's kids */
#define ITEMS_MC_CHUNKSIZE      16              /* PDFlib item mc sequences */
#define LAYER_DEP_CHUNKSIZE     16              /* PDFlib layer dependencies */
#define RESLIST_CHUNKSIZE       16              /* per page resource list */
#define TEXTFLOW_CHUNKSIZE       4              /* textflows */

/* Acrobat 4 allows only 12 levels, but Acrobat 5 increases the limit to 28 */
#define PDF_MAX_SAVE_LEVEL      28              /* max number of save levels */

#define PDF_FILENAMELEN         1024            /* maximum file name length */
#define PDF_MAX_PARAMSTRING     256             /* image parameter string */
#define PDF_MAX_NAMESTRING      127             /* maximum name length */
#define PDF_MAX_EVENTS          16              /* maximum number of events */

/* Acrobat limit for page dimensions */
#define PDF_ACRO_MINPAGE       (3.0)           /* 1/24 inch = 0.106 cm */
#define PDF_ACRO_MAXPAGE       (14400.0)       /* 200  inch = 508 cm */


/* ------------------------ typedefs and enums --------------------------- */

/* PDFlib error numbers.
*/
enum
{
#define         pdf_genNames    1
#include        "p_generr.h"

    PDF_E_dummy
};

#define pdf_state_content       \
    (pdf_state) (pdf_state_page | pdf_state_pattern | \
                 pdf_state_template | pdf_state_glyph)

#define pdf_state_all							\
    (pdf_state) (pdf_state_object | pdf_state_document | pdf_state_page |  \
                 pdf_state_pattern | pdf_state_template | pdf_state_path | \
                 pdf_state_font | pdf_state_glyph)

#define PDF_STATE_STACK_SIZE    4

/* function-like macros.
** must behave well wherever function calls are syntactically legal.
*/
#define PDF_GET_STATE(p)                                                \
        ((p)->state_stack[(p)->state_sp])

#define PDF_SET_STATE(p, s)						\
        ((p)->state_stack[(p)->state_sp] = (s))

/* statement-like macros.
** must behave well wherever statements are syntactically legal.
*/
#define PDF_CHECK_STATE(p, s)						\
        if ((((p)->state_stack[(p)->state_sp] & (s)) != 0)) {		\
	} else pdc_error((p)->pdc,					\
	    PDF_E_DOC_SCOPE, pdf_current_scope(p), 0, 0, 0)

#define PDF_PUSH_STATE(p, fn, s)                                        \
        if ((p)->state_sp == PDF_STATE_STACK_SIZE - 1)                  \
            pdc_error((p)->pdc, PDF_E_INT_SSTACK_OVER, fn, 0, 0, 0);	\
        else                                                            \
            (p)->state_stack[++(p)->state_sp] = (s)

#define PDF_POP_STATE(p, fn)                                            \
        if ((p)->state_sp == 0)                                         \
            pdc_error((p)->pdc, PDF_E_INT_SSTACK_UNDER, fn, 0, 0, 0);	\
        else                                                            \
            --(p)->state_sp


/* -------------------------- structs ------------------------------ */

#ifndef PDI_DEFINED
#define PDI_DEFINED
typedef struct PDI_s PDI;	/* The opaque PDI type */
#endif

typedef struct
{
    pdc_bool	info_mode;
    PDI *	pi;
    pdc_byte *	data;

    void *	callback_ctx;
    size_t    (*readproc)(void *callback_ctx, void *buffer, size_t size);
    int       (*seekproc)(void *callback_ctx, long offset);
} pdf_pdi;

/* Opaque types which are detailed in the respective modules
   in alphabetical order */
typedef struct pdf_action_s pdf_action;
typedef struct pdf_annot_s pdf_annot;
typedef struct pdf_category_s pdf_category;
typedef struct pdf_colorspace_s pdf_colorspace;
typedef struct pdf_cstate_s pdf_cstate;
typedef struct pdf_dest_s pdf_dest;
typedef struct pdf_document_s pdf_document;
typedef struct pdf_encoding_s pdf_encoding;
typedef struct pdf_extgstateresource_s pdf_extgstateresource;
typedef struct pdf_font_options_s pdf_font_options;
typedef struct pdf_formfields_s pdf_formfields;
typedef struct pdf_iccprofile_s pdf_iccprofile;
typedef struct pdf_image_s pdf_image;
typedef struct pdf_info_s pdf_info;
typedef struct pdf_layers_s pdf_layers;
typedef struct pdf_linearopts_s pdf_linearopts;
typedef struct pdf_name_s pdf_name;
typedef struct pdf_outline_s pdf_outline;
typedef struct pdf_pages_s pdf_pages;
typedef struct pdf_pattern_s pdf_pattern;
typedef struct pdf_res_s pdf_res;
typedef struct pdf_reslist_s pdf_reslist;
typedef struct pdf_shading_s pdf_shading;
typedef struct pdf_tags_s pdf_tags;
typedef struct pdf_textflow_s pdf_textflow;
typedef struct pdf_text_options_s pdf_text_options;
typedef struct pdf_tstate_s pdf_tstate;
typedef struct pdf_virtfile_s pdf_virtfile;
typedef struct pdf_widget_s pdf_widget;
typedef struct pdf_xobject_s pdf_xobject;


/* -------------------- special graphics state -------------------- */
typedef struct {
    pdc_matrix  ctm;            /* current transformation matrix */
    pdc_scalar  x;              /* current x coordinate */
    pdc_scalar  y;              /* current y coordinate */

    pdc_scalar  startx;         /* starting x point of the subpath */
    pdc_scalar  starty;         /* starting y point of the subpath */

    pdc_scalar  lwidth;         /* line width */
    int         lcap;           /* line cap style */
    int         ljoin;          /* line join style */
    pdc_scalar  miter;          /* miter limit */
    pdc_scalar  flatness;       /* path flatness */
    pdc_bool    dashed;         /* line dashing in effect */
} pdf_gstate;

/* ---------------------- page/pattern/template ----------------------- */
typedef struct
{
    /* graphics, text, and color state.
    */
    int			sl;                             /* current save level */
    pdf_gstate		gstate[PDF_MAX_SAVE_LEVEL];     /* graphics state */
    pdf_tstate		*tstate;                        /* text state */
    pdf_cstate	        *cstate;                        /* color state */

    pdf_text_options    *currto;        /* current text options */
    pdf_fillrule        fillrule;       /* nonzero or evenodd fill rule */
} pdf_ppt;

/* Force graphics or color operator output, avoiding the optimization
 * which checks whether the new value might be the same as the old.
 * This is especially required for Type 3 glyph descriptions which
 * inherit the surrounding page description's gstate parameters,
 * and therefore even must write default values.
 */
#define PDF_FORCE_OUTPUT() (PDF_GET_STATE(p) == pdf_state_glyph)

/*
 * *************************************************************************
 * The core PDF context descriptor
 * *************************************************************************
 */

struct PDF_s {
    /* -------------------------- general stuff ------------------------ */
    unsigned long       magic;          /* poor man's integrity check */
    void	(*freeproc)(PDF *p, void *mem);
    pdc_core    *pdc;                   /* core context */
    int         compatibility;          /* PDF version number * 10 */

    char        *binding;               /* name of the language binding */
    pdc_bool    objorient;              /* binding object orientated */
    pdc_bool    hastobepos;             /* return value has to be positiv */
    pdc_bool    ptfrun;                 /* within a PTF run */
    pdc_bool    charref;                /* HTML character references will
                                         * be accepted */

    pdf_state   state_stack[PDF_STATE_STACK_SIZE];
    int         state_sp;               /* state stack pointer */

    /* ------------------- PDF Catalog dictionary  --------------------- */
    pdf_document *document;             /* document struct */


    /* ------------------- PDF Info dictionary entries ----------------- */
    pdf_info     *userinfo;             /* list of user-defined entries */

    /* -------------- I/O, error handling and memory management ------------- */
    size_t	(*writeproc)(PDF *p, void *data, size_t size);
    void        (*errorhandler)(PDF *p, int level, const char* msg);
    void        *opaque;                /* user-specific, opaque data */

    /* ------------------------- PDF import ---------------------------- */
    pdf_pdi     *pdi;                   /* PDI context array */
    int         pdi_capacity;           /* currently allocated size */
    pdc_usebox  pdi_usebox;
    pdc_bool	pdi_strict;		/* strict PDF parser mode */
    pdc_sbuf *  pdi_sbuf;               /* string buffer for pdi parms */

    /* ------------------------ resource stuff ------------------------- */
    pdf_category *resources;            /* anchor for the resource list */
    pdc_bool     resfilepending;        /* to read resource file is pending */
    char         *resourcefilename;     /* name of the resource file */
    char         *prefix;               /* prefix for resource file names */

    /* ---------------- virtual file system stuff ----------------------- */
    pdf_virtfile *filesystem;           /* anchor for the virtual file system */

    /* ------------ stuff for hypertext functions ---------- */
    pdc_encoding    hypertextencoding;  /* encoding of hypertexts */
    pdc_text_format hypertextformat;    /* format of hypertexts */
    int             hypertextcodepage;  /* OEM multi byte code-page number */
    pdc_bool        usercoordinates;    /* interprete rectangle coordinates */
                                        /* of hypertext funcs. in user space */


    /* ------------------- PDF output bookkeeping ------------------- */
    pdc_id      procset_id;              /* id of constant ProcSet array */
    pdc_output  *out;                   /* output manager */
    pdc_id      length_id;              /* id of current stream's length*/
    pdf_flush_state flush;              /* flush state */

    /* ------------------- page bookkeeping ------------------- */
    pdf_pages  *doc_pages;		/* document wide page management */

    /* ------------------- document resources ------------------- */
    pdc_font    *fonts;                 /* all fonts in document */
    int         fonts_capacity;         /* currently allocated size */
    int         fonts_number;           /* next available font number */
    pdc_t3font *t3font;                 /* type 3 font info */

    pdf_textflow *textflows;            /* all textflows */
    int         textflows_capacity;     /* currently allocated size */

    pdf_xobject *xobjects;              /* all xobjects in document */
    int         xobjects_capacity;      /* currently allocated size */
    int         xobjects_number;        /* next available xobject slot */

    pdf_colorspace *colorspaces;        /* all color space resources */
    int         colorspaces_capacity;   /* currently allocated size */
    int         colorspaces_number;     /* next available color space number */


    pdf_pattern *pattern;               /* all pattern resources */
    int         pattern_capacity;       /* currently allocated size */
    int         pattern_number;         /* next available pattern number */

    pdf_shading *shadings;               /* all shading resources */
    int         shadings_capacity;       /* currently allocated size */
    int         shadings_number;         /* next available shading number */

    pdf_extgstateresource *extgstates;  /* all ext. graphic state resources */
    int         extgstates_capacity;    /* currently allocated size */
    int         extgstates_number;      /* next available extgstate number */

    pdf_image  *images;                 /* all images in document */
    int         images_capacity;        /* currently allocated size */

    pdf_action *actions;                /* all actions in document */
    int         actions_capacity;       /* currently allocated size */
    int         actions_number;         /* next available action number */

    /* ------------------ utilities ------------------- */
    char        ***stringlists;         /* string lists */
    int         stringlists_capacity;   /* currently allocated size */
    int         stringlists_number;     /* next available string list number */
    int         *stringlistsizes;       /* sizes of string lists */
    int         utilstrlist_index;      /* index of utility string list */
    int         utilstring_number;      /* next available utility string */

    /* ------------------- encodings ------------------- */
    pdf_encoding *encodings;            /* all encodings in document */
    int         encodings_capacity;     /* currently allocated size */
    int         encodings_number;       /* next available encoding slot */

    /* ------------------- document outline tree ------------------- */
    int         outline_capacity;       /* currently allocated size */
    int         outline_count;          /* total number of outlines */
    pdf_outline *outlines;              /* dynamic array of outlines */

    /* ------------------- name tree ------------------- */
    pdf_name   *names;                  /* page ids */
    int         names_capacity;
    int         names_number;      	/* next available names number */

    /* -------------- page/pattern/template specific stuff -------------- */
    pdf_ppt *	curr_ppt;		/* current ppt descriptor */
    pdc_id      res_id;                 /* id of this pattern/templ res dict */

    pdc_scalar  ydirection;             /* direction of y axis of default */
                                        /* system rel. to viewport (1 or -1) */

    pdf_renderingintent rendintent;     /* RenderingIntent */

    pdc_bool    preserveoldpantonenames;/* preserve old PANTONE names */
    pdc_bool    spotcolorlookup;        /* use internal look-up table for
                                         * color values */

    /* ------------------------ template stuff ----------------------- */
    int         templ;                  /* current template if in templ. state*/

    /* --------------- other font and text stuff ---------------- */

    pdf_font_options    *currfo;        /* current font settings */

    pdc_text_format     textformat;     /* text storage format */
    pdc_bool		in_text;	/* currently in BT/ET section */

    /* ------------------------ miscellaneous ------------------------ */
    char        debug[256];             /* debug flags */



    /* ------- deprecated stuff because of deprecated parameter ---------- */
    pdf_borderstyle     border_style;
    pdc_scalar          border_width;
    pdc_scalar          border_red;
    pdc_scalar          border_green;
    pdc_scalar          border_blue;
    pdc_scalar          border_dash1;
    pdc_scalar          border_dash2;
    pdf_dest            *bookmark_dest;
    char                *launchlink_parameters;
    char                *launchlink_operation;
    char                *launchlink_defaultdir;

};

/* Data source for images, compression, ASCII encoding, fonts, etc. */
typedef struct PDF_data_source_s PDF_data_source;
struct PDF_data_source_s {
    pdc_byte            *next_byte;
    size_t              bytes_available;
    void                (*init)(PDF *, PDF_data_source *src);
    int                 (*fill)(PDF *, PDF_data_source *src);
    void                (*terminate)(PDF *, PDF_data_source *src);

    pdc_byte            *buffer_start;
    size_t              buffer_length;
    void                *private_data;
    long                offset;         /* start of data to read */
    long                length;         /* length of data to read */
    long                total;          /* total bytes read so far */
};

/* ------ Private functions for library-internal use only --------- */


/*
   (((((OpenVMS porting note)))))

   Symbols are restricted to <= 31 bytes on OpenVMS systems....
   Please truncate new function names to fit this silly restriction!

   (((((OpenVMS porting note)))))
*/



/**********************
 *
 *  p_actions.c
 *
 **********************/

int pdf__create_action(PDF *p, const char *type, const char *optlist);

void pdf_init_actions(PDF *p);
void pdf_cleanup_actions(PDF *p);
pdc_bool pdf_parse_and_write_actionlist(PDF *p, pdf_event_object eventobj,
                pdc_id *act_idlist, const char *optlist);
pdc_bool pdf_write_action_entries(PDF *p, pdf_event_object eventobj,
                pdc_id *act_idlist);


/**********************
 *
 *  p_annots.c
 *
 **********************/

void pdf__add_launchlink(PDF *p, pdc_scalar llx, pdc_scalar lly,
        pdc_scalar urx, pdc_scalar ury, const char *filename);
void pdf__add_locallink(PDF *p, pdc_scalar llx, pdc_scalar lly,
        pdc_scalar urx, pdc_scalar ury, int page, const char *optlist);
void pdf__add_note(PDF *p, pdc_scalar llx, pdc_scalar lly,
        pdc_scalar urx, pdc_scalar ury, const char *contents, int len_cont,
        const char *title, int len_title, const char *icon, int open);
void pdf__add_pdflink(PDF *p, pdc_scalar llx, pdc_scalar lly,
        pdc_scalar urx, pdc_scalar ury, const char *filename, int page,
        const char *optlist);
void pdf__add_weblink(PDF *p, pdc_scalar llx, pdc_scalar lly,
        pdc_scalar urx, pdc_scalar ury, const char *url);
void pdf__attach_file(PDF *p, pdc_scalar llx, pdc_scalar lly,
        pdc_scalar urx, pdc_scalar ury, const char *filename, int len_filename,
        const char *description, int len_descr, const char *author,
	int len_auth, const char *mimetype, const char *icon);
void pdf__create_annotation(PDF *p, pdc_scalar llx, pdc_scalar lly,
        pdc_scalar urx, pdc_scalar ury, const char *type, const char *optlist);
void pdf__set_border_color(PDF *p, pdc_scalar red, pdc_scalar green,
        pdc_scalar blue);
void pdf__set_border_dash(PDF *p, pdc_scalar b, pdc_scalar w);
void pdf__set_border_style(PDF *p, const char *style, pdc_scalar width);

void pdf_init_annot_params(PDF *p);
void pdf_cleanup_annot_params(PDF *p);
pdc_id pdf_write_annots_root(PDF *p, pdf_annot *annlist,
        pdf_widget *widgetlist);
void pdf_write_page_annots(PDF *p, pdf_annot *list);
void pdf_cleanup_page_annots(PDF *p, pdf_annot *list);

void pdf_create_link(PDF *p, const char *type, pdc_scalar llx, pdc_scalar lly,
        pdc_scalar urx, pdc_scalar ury, const char *annopts,
        const char *utext, int len);



/**********************
 *
 *  p_color.c
 *
 **********************/


void pdf__setcolor(PDF *p, const char *fstype, const char *colorspace,
        pdc_scalar c1, pdc_scalar c2, pdc_scalar c3, pdc_scalar c4);

void pdf_init_cstate(PDF *p);
void pdf_save_cstate(PDF *p);
void pdf_cleanup_page_cstate(PDF *p, pdf_ppt *ppt);
void pdf_init_colorspaces(PDF *p);
void pdf_write_page_colorspaces(PDF *p);
void pdf_mark_page_colorspace(PDF *p, int n);
void pdf_write_doc_colorspaces(PDF *p);
void pdf_write_colorspace(PDF *p, int slot, pdc_bool direct);
void pdf_cleanup_colorspaces(PDF *p);
void pdf_write_colormap(PDF *p, int slot);


/**********************
 *
 *  p_document.c
 *
 **********************/

int pdf__begin_document(PDF *p, const char *filename, int len,
                const char *optlist);

void pdf__begin_document_callback(PDF *p, writeproc_t writeproc,
                const char *optlist);

void pdf__end_document(PDF *p, const char *optlist);

void pdf_cleanup_document(PDF *p);
void pdf_fix_openmode(PDF *p);
void pdf_insert_name(PDF *p, const char *name, pdf_nametree_type type,
                pdc_id obj_id);
pdc_id pdf_write_metadata(PDF *p, const char *metadata);
char *pdf_parse_and_write_metadata(PDF *p, const char *optlist, pdc_bool output,
                int *outlen);

/* deprecated functions: */
void pdf_set_flush(PDF *p, const char *flush);
void pdf_set_uri(PDF *p, const char *uri);
void pdf_set_compatibility(PDF *p, const char *compatibility);
void pdf_set_openmode(PDF *p, const char *openmode);
void pdf_set_openaction(PDF *p, const char *openaction);
void pdf_set_viewerpreference(PDF *p, const char *viewerpreference);
const char *pdf__get_buffer(PDF *p, long *size);


/**********************
 *
 *  p_draw.c
 *
 **********************/


void pdf__arc(PDF *p, pdc_scalar x, pdc_scalar y,
                pdc_scalar r, pdc_scalar alpha, pdc_scalar beta);
void pdf__arcn(PDF *p, pdc_scalar x, pdc_scalar y,
                pdc_scalar r, pdc_scalar alpha, pdc_scalar beta);
void pdf__circle(PDF *p, pdc_scalar x, pdc_scalar y, pdc_scalar r);
void pdf__clip(PDF *p);
void pdf__closepath(PDF *p);
void pdf__closepath_fill_stroke(PDF *p);
void pdf__closepath_stroke(PDF *p);
void pdf__curveto(PDF *p, pdc_scalar x_1, pdc_scalar y_1,
                pdc_scalar x_2, pdc_scalar y_2, pdc_scalar x_3, pdc_scalar y_3);
void pdf__endpath(PDF *p);
void pdf__fill(PDF *p);
void pdf__fill_stroke(PDF *p);
void pdf__lineto(PDF *p, pdc_scalar x, pdc_scalar y);
void pdf__rlineto(PDF *p, pdc_scalar x, pdc_scalar y);
void pdf__moveto(PDF *p, pdc_scalar x, pdc_scalar y);
void pdf__rcurveto(PDF *p, pdc_scalar x_1, pdc_scalar y_1,
                pdc_scalar x_2, pdc_scalar y_2, pdc_scalar x_3, pdc_scalar y_3);
void pdf__rect(PDF *p, pdc_scalar x, pdc_scalar y,
                pdc_scalar width, pdc_scalar height);
void pdf__rmoveto(PDF *p, pdc_scalar x, pdc_scalar y);
void pdf__stroke(PDF *p);

void pdf_rrcurveto(PDF *p, pdc_scalar x_1, pdc_scalar y_1,
                pdc_scalar x_2, pdc_scalar y_2, pdc_scalar x_3, pdc_scalar y_3);
void pdf_hvcurveto(PDF *p, pdc_scalar x_1, pdc_scalar x_2,
                pdc_scalar y_2, pdc_scalar y_3);
void pdf_vhcurveto(PDF *p, pdc_scalar y_1, pdc_scalar x_2,
                pdc_scalar y_2, pdc_scalar x_3);


/**********************
 *
 *  p_encoding.c
 *
 **********************/

const char *pdf__encoding_get_glyphname(PDF *p, const char *encoding, int slot);
int pdf__encoding_get_unicode(PDF *p, const char *encoding, int slot);
void pdf__encoding_set_char(PDF *p, const char *encoding, int slot,
        const char *glyphname, int uv);

pdc_encodingvector *pdf_generate_encoding(PDF *p, const char *encoding);
pdc_ushort pdf_glyphname2unicode(PDF *p, const char *glyphname);
const char *pdf_unicode2glyphname(PDF *p, pdc_ushort uv);
pdc_ushort pdf_insert_glyphname(PDF *p, const char *glyphname);
const char *pdf_insert_unicode(PDF *p, pdc_ushort uv);
pdc_ushort pdf_register_glyphname(PDF *p, const char *glyphname, pdc_ushort uv);
pdc_encoding pdf_insert_encoding(PDF *p, const char *encoding,
        int *codepage, pdc_bool verbose);
pdc_encoding pdf_find_encoding(PDF *p, const char *encoding);
const char *pdf_get_user_encoding(PDF *p, pdc_encoding enc);
void pdf_set_encoding_glyphnames(PDF *p, pdc_encoding enc);
pdc_encoding pdf_get_hypertextencoding(PDF *p, const char *encoding,
        int *codepage, pdc_bool verbose);
pdc_encodingvector *pdf_get_encoding_vector(PDF *p, pdc_encoding enc);
pdc_bool pdf_get_encoding_isstdflag(PDF *p, pdc_encoding enc);
pdc_bool pdf_is_encoding_subset(PDF *p, pdc_encodingvector *testev,
        pdc_encodingvector *refev);
void pdf_init_encoding_ids(PDF *p);
void pdf_init_encodings(PDF *p);
void pdf_grow_encodings(PDF *p);
void pdf_cleanup_encodings(PDF *p);




/**********************
 *
 *  p_filter.c
 *
 **********************/

int pdf_data_source_buf_fill(PDF *p, PDF_data_source *src);
void pdf_data_source_file_init(PDF *p, PDF_data_source *src);
int pdf_data_source_file_fill(PDF *p, PDF_data_source *src);
void pdf_data_source_file_terminate(PDF *p, PDF_data_source *src);
void pdf_copy_stream(PDF *p, PDF_data_source *src, pdc_bool compress);


/**********************
 *
 *  p_font.c
 *
 **********************/

int pdf__get_glyphid(PDF *p, int font, int code);
int pdf__load_font(PDF *p, const char *fontname, int len,
        const char *encoding, const char *optlist);

void pdf_init_fonts(PDF *p);
void pdf_grow_fonts(PDF *p);
int  pdf_init_newfont(PDF *p);
void pdf_write_page_fonts(PDF *p);
void pdf_mark_page_font(PDF *p, int n);
void pdf_write_doc_fonts(PDF *p);
void pdf_cleanup_fonts(PDF *p);


/**********************
 *
 *  p_gstate.c
 *
 **********************/

void pdf__concat(PDF *p, pdc_scalar a, pdc_scalar b, pdc_scalar c, pdc_scalar d,
                pdc_scalar e, pdc_scalar f);
void pdf__initgraphics(PDF *p);
void pdf__restore(PDF *p);
void pdf__rotate(PDF *p, pdc_scalar phi);
void pdf__save(PDF *p);
void pdf__scale(PDF *p, pdc_scalar sx, pdc_scalar sy);
void pdf__setdash(PDF *p, pdc_scalar b, pdc_scalar w);
void pdf__setdashpattern(PDF *p, const char *optlist);
void pdf__setflat(PDF *p, pdc_scalar flatness);
void pdf__setlinecap(PDF *p, int linecap);
void pdf__setlinejoin(PDF *p, int linejoin);
void pdf__setlinewidth(PDF *p, pdc_scalar width);
void pdf__setmatrix(PDF *p, pdc_scalar a, pdc_scalar b, pdc_scalar c,
                pdc_scalar d, pdc_scalar e, pdc_scalar f);
void pdf__setmiterlimit(PDF *p, pdc_scalar miter);
void pdf__skew(PDF *p, pdc_scalar alpha, pdc_scalar beta);
void pdf__translate(PDF *p, pdc_scalar tx, pdc_scalar ty);

void pdf_setmatrix_e(PDF *p, pdc_matrix *n);
void pdf_init_gstate(PDF *p);
void pdf_concat_raw(PDF *p, pdc_matrix *m);
void pdf_reset_gstate(PDF *p);
void pdf_set_topdownsystem(PDF *p, pdc_scalar height);


/**********************
 *
 *  p_hyper.c
 *
 **********************/

int pdf__add_bookmark(PDF *p, const char *text, int len, int parent, int open);
void pdf__add_nameddest(PDF *p, const char *name, int len, const char *optlist);
int pdf__create_bookmark(PDF *p, const char *text, int len,
        const char *optlist);
void pdf__set_info(PDF *p, const char *key, const char *value, int len);

pdf_dest *pdf_init_destination(PDF *p);
pdf_dest *pdf_parse_destination_optlist(PDF *p, const char *optlist,
        int page, pdf_destuse destuse, pdc_bool verbose);
void pdf_cleanup_destination(PDF *p, pdf_dest *dest);
void pdf_write_destination(PDF *p, pdf_dest *dest);
pdf_dest *pdf_get_option_destname(PDF *p, pdc_resopt *resopts,
        pdc_encoding hypertextencoding);
void pdf_init_outlines(PDF *p);
void pdf_write_outlines(PDF *p);
void pdf_write_outline_root(PDF *p);
void pdf_cleanup_outlines(PDF *p);
void pdf_feed_digest_info(PDF *p);
pdc_id pdf_write_info(PDF *p);
void pdf_cleanup_info(PDF *p);




/**********************
 *
 *  p_image.c
 *
 **********************/

void pdf__add_thumbnail(PDF *p, int image);
void pdf__close_image(PDF *p, int image);
void pdf__fit_image(PDF *p, int image, pdc_scalar x, pdc_scalar y,
        const char *optlist);
int pdf__load_image(PDF *p, const char *imagetype, const char *filename,
        const char *optlist);

void pdf_grow_images(PDF *p);
void pdf_put_image(PDF *p, int im, pdc_bool firststrip,
	pdc_bool checkcontentstream);
void pdf_put_inline_image(PDF *p, int im);
void pdf_init_images(PDF *p);
void pdf_cleanup_images(PDF *p);
void pdf_cleanup_image(PDF *p, int im);
void pdf_get_image_size(PDF *p, int im, pdc_scalar *width, pdc_scalar *height);
void pdf_get_image_resolution(PDF *p, int im, pdc_scalar *dpi_x,
        pdc_scalar *dpi_y);






/**********************
 *
 *  p_object.c
 *
 **********************/

void pdf__delete(PDF *p);

PDF *pdf__new(errorproc_t errorhandler, allocproc_t allocproc,
        reallocproc_t reallocproc, freeproc_t freeproc, void *opaque);

const char *pdf_current_scope(PDF *p);


/**********************
 *
 *  p_page.c
 *
 **********************/

void pdf__begin_page(PDF *p, pdc_scalar width, pdc_scalar height);
void pdf__begin_page_ext(PDF *p, pdc_scalar width, pdc_scalar height,
        const char *optlist);
void pdf__end_page_ext(PDF *p, const char *optlist);
void pdf__resume_page(PDF *p, const char *optlist);
void pdf__suspend_page(PDF *p, const char *optlist);
void pdf_pg_resume(PDF *p, int pageno);
void pdf_pg_suspend(PDF *p);

void pdf_init_pages(PDF *p, const char **groups, int n_groups);
void pdf_init_pages2(PDF *p);
void pdf_check_suspended_pages(PDF *p);
void pdf_cleanup_pages(PDF *p);
pdc_id pdf_get_page_id(PDF *p, int n);
int pdf_current_page(PDF *p);
int pdf_current_page_id(PDF *p);
int pdf_last_page(PDF *p);
int pdf_search_page_fwd(PDF *p, int start_page, pdc_id id);
int pdf_search_page_bwd(PDF *p, int start_page, pdc_id id);
int pdf_xlat_pageno(PDF *p, int pageno, const char *groupname);

double pdf_get_pageheight(PDF *p);
const pdc_rectangle *pdf_get_pagebox(PDF *p, pdf_pagebox box);
void pdf_set_pagebox_llx(PDF *p, pdf_pagebox box, double llx);
void pdf_set_pagebox_lly(PDF *p, pdf_pagebox box, double lly);
void pdf_set_pagebox_urx(PDF *p, pdf_pagebox box, double urx);
void pdf_set_pagebox_ury(PDF *p, pdf_pagebox box, double ury);
void pdf_set_pagebox(PDF *p, pdf_pagebox box, double llx, double lly,
	double urx, double ury);

pdf_annot *pdf_get_annots_list(PDF *p);
void pdf_set_annots_list(PDF *p, pdf_annot *ann);
pdc_id pdf_get_thumb_id(PDF *p);
void pdf_set_thumb_id(PDF *p, pdc_id id);

void pdf_begin_contents_section(PDF *p);
void pdf_end_contents_section(PDF *p);
void pdf_add_reslist(PDF *p, pdf_reslist *rl, int num);
pdc_id pdf_write_pagelabels(PDF *p);



/**********************
 *
 *  p_parameter.c
 *
 **********************/

const char *pdf__get_parameter(PDF *p, const char *key, double modifier);
double pdf__get_value(PDF *p, const char *key, double modifier);
void pdf__set_parameter(PDF *p, const char *key, const char *value);
void pdf__set_value(PDF *p, const char *key, double value);


/**********************
 *
 *  p_pattern.c
 *
 **********************/

int pdf__begin_pattern(PDF *p,
        pdc_scalar width, pdc_scalar height, pdc_scalar xstep, pdc_scalar ystep,
        int painttype);

void pdf__end_pattern(PDF *p);

void pdf_init_pattern(PDF *p);
void pdf_write_page_pattern(PDF *p);
void pdf_get_page_patterns(PDF *p, pdf_reslist *rl);
void pdf_mark_page_pattern(PDF *p, int n);
void pdf_cleanup_pattern(PDF *p);
void pdf_grow_pattern(PDF *p);




/**********************
 *
 *  p_resource.c
 *
 **********************/

void pdf__create_pvf(PDF *p, const char *filename,
        const void *data, size_t size, const char *optlist);
int pdf__delete_pvf(PDF *p, const char *filename);

void pdf_add_resource(PDF *p, const char *category, const char *resource);
void pdf_cleanup_resources(PDF *p);
char *pdf_find_resource(PDF *p, const char *category, const char *name);
void pdf_lock_pvf(PDF *p, const char *filename);
void pdf_unlock_pvf(PDF *p, const char *filename);
void pdf_cleanup_filesystem(PDF *p);
const char *pdf_convert_filename(PDF *p, const char *filename, int len,
        const char *paramname, pdc_bool withbom);
pdc_file *pdf_fopen(PDF *p, const char *filename, const char *qualifier,
        int flags);
pdc_file *pdf_fopen_name(PDF *p, const char *filename, char *fullname,
        const char *qualifier, int flags);


/**********************
 *
 *  p_shading.c
 *
 **********************/

int pdf__shading(PDF *p, const char *shtype, pdc_scalar x_0, pdc_scalar y_0,
        pdc_scalar x_1, pdc_scalar y_1, pdc_scalar c_1, pdc_scalar c_2,
        pdc_scalar c_3, pdc_scalar c_4, const char *optlist);
int pdf__shading_pattern(PDF *p, int shading, const char *optlist);
void pdf__shfill(PDF *p, int shading);

void pdf_init_shadings(PDF *p);
void pdf_write_page_shadings(PDF *p);
void pdf_get_page_shadings(PDF *p, pdf_reslist *rl);
void pdf_mark_page_shading(PDF *p, int n);
void pdf_cleanup_shadings(PDF *p);
int pdf_get_shading_painttype(PDF *p);




/**********************
 *
 *  p_template.c
 *
 **********************/

int pdf__begin_template(PDF *p, pdc_scalar width, pdc_scalar height,
        const char *optlist);
void pdf__end_template(PDF *p);
int pdf_embed_image(PDF *p, int im);


/**********************
 *
 *  p_text.c
 *
 **********************/

void pdf__fit_textline(PDF *p, const char *text, int len,
        pdc_scalar x, pdc_scalar y, const char *optlist);
void pdf__setfont(PDF *p, int font, pdc_scalar fontsize);
void pdf__set_text_pos(PDF *p, pdc_scalar x, pdc_scalar y);
void pdf__show_text(PDF *p, const char *text, int len, pdc_bool cont);
void pdf__xshow(PDF *p, const char *text, int len,
        const pdc_scalar *xadvancelist);
int pdf__show_boxed(PDF *p, const char *text, int len,
        pdc_scalar left, pdc_scalar top, pdc_scalar width, pdc_scalar height,
        const char *hmode, const char *feature);
pdc_scalar pdf__stringwidth(PDF *p, const char *text, int len,
        int font, pdc_scalar size);

void pdf_init_tstate(PDF *p);
void pdf_cleanup_page_tstate(PDF *p, pdf_ppt *ppt);
void pdf_save_tstate(PDF *p);
void pdf_restore_currto(PDF *p);
void pdf_set_tstate(PDF *p, pdc_scalar value, pdf_text_optflags flag);
double pdf_get_tstate(PDF *p, pdf_text_optflags tflag);
void pdf_end_text(PDF *p);
void pdf_reset_tstate(PDF *p);
int  pdf_get_font(PDF *p);
void pdf_put_fieldtext(PDF *p, const char *text, int font);




/**********************
 *
 *  p_type3.c
 *
 **********************/

void pdf__begin_font(PDF *p, const char *fontname, int len,
        pdc_scalar a, pdc_scalar b, pdc_scalar c, pdc_scalar d,
        pdc_scalar e, pdc_scalar f, const char *optlist);
void pdf__begin_glyph(PDF *p, const char *glyphname, pdc_scalar wx,
        pdc_scalar llx, pdc_scalar lly, pdc_scalar urx, pdc_scalar ury);
void pdf__end_font(PDF *p);
void pdf__end_glyph(PDF *p);

int pdf_get_t3colorized(PDF *p);


/**********************
 *
 *  p_util.c
 *
 **********************/

const char *pdf__utf16_to_utf8(PDF *p, const char *utf16string, int len,
        int *outlen);
const char *pdf__utf8_to_utf16(PDF *p, const char *utf8string,
        const char *format, int *outlen);

void pdf_check_textformat(PDF *p, pdc_text_format textformat);
void pdf_check_hypertextformat(PDF *p, pdc_text_format hypertextformat);
void pdf_check_hypertextencoding(PDF *p, pdc_encoding hypertextencoding);
char *pdf_convert_name(PDF *p, const char *name, int len, pdc_bool withbom);
void pdf_put_pdfname(PDF *p, const char *name);
pdc_encoding pdf_get_hypertextencoding_opt(PDF *p, pdc_resopt *resopts,
        int *codepage, pdc_bool verbose);
char *pdf_convert_hypertext_depr(PDF *p, const char *text, int len);
char *pdf_convert_hypertext(PDF *p, const char *text, int len,
        pdc_text_format hypertextformat, pdc_encoding hypertextencoding,
        int codepage, int *outlen, pdc_bool oututf8, pdc_bool verbose);
void pdf_put_hypertext(PDF *p, const char *text);
void pdf_put_pdffilename(PDF *p, const char *text);
void pdf_check_handle(PDF *p, int value, pdc_opttype type);
void pdf_set_clientdata(PDF *p, pdc_clientdata *clientdata);
void pdf_init_stringlists(PDF *p);
int pdf_insert_stringlist(PDF *p, char **stringlist, int ns);
void pdf_cleanup_stringlists(PDF *p);
void pdf_insert_utilstring(PDF *p, const char *utilstring, pdc_bool pdiparam);
int pdf_get_opt_textlist(PDF *p, const char *keyword, pdc_resopt *resopts,
       pdc_encoding enc, pdc_bool ishypertext, const char *fieldname,
       char **text, char ***textlist);
char *pdf_get_opt_utf8name(PDF *p, const char *keyword, pdc_resopt *resopts);
void pdf_set_trace_options(PDF *p, const char *optlist);
void pdf_enable_trace(PDF *p);



/**********************
 *
 *  p_xgstate.c
 *
 **********************/

int pdf__create_gstate(PDF *p, const char *optlist);
void pdf__set_gstate(PDF *p, int gstate);

void pdf_init_extgstates(PDF *p);
void pdf_write_page_extgstates(PDF *p);
void pdf_get_page_extgstates(PDF *p, pdf_reslist *rl);
void pdf_mark_page_extgstate(PDF *p, int n);
void pdf_write_doc_extgstates(PDF *p);
void pdf_cleanup_extgstates(PDF *p);
pdc_id  pdf_get_gstate_id(PDF *p, int gstate);

#endif  /* P_INTERN_H */


