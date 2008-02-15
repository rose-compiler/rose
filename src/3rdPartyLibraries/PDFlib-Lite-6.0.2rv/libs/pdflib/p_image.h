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

/* $Id: p_image.h,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * Header file for the PDFlib image subsystem
 *
 */

#ifndef P_IMAGE_H
#define P_IMAGE_H

#ifdef HAVE_LIBTIFF
#include "tiffio.h"
#endif

#ifdef HAVE_LIBPNG
#include "png.h"
#endif

/* image type */
typedef enum
{
    pdf_img_auto,
    pdf_img_bmp,
    pdf_img_ccitt,
    pdf_img_gif,
    pdf_img_jpeg,
    pdf_img_jpeg2000,
    pdf_img_png,
    pdf_img_raw,
    pdf_img_tiff
}
pdf_image_type;

/* compression type */
typedef enum
{
    pdf_comp_none,
    pdf_comp_lzw,
    pdf_comp_runlength,
    pdf_comp_ccitt,
    pdf_comp_dct,
    pdf_comp_flate,
    pdf_comp_jbig2,
    pdf_comp_jpx
}
pdf_compression;

/* image reference */
typedef enum
{
    pdf_ref_direct,
    pdf_ref_file,
    pdf_ref_url
}
pdf_ref_type;

typedef enum
{
    pred_default = 1,
    pred_tiff = 2,
    pred_png = 15
}
pdf_predictor;

#ifdef P_IMAGE_C

const pdc_keyconn pdf_image_keylist[] =
{
    {"auto",      pdf_img_auto},
    {"bmp",       pdf_img_bmp},
    {"ccitt",     pdf_img_ccitt},
    {"gif",       pdf_img_gif},
    {"jpeg",      pdf_img_jpeg},
    {"jpeg2000",  pdf_img_jpeg2000},
    {"png",       pdf_img_png},
    {"raw",       pdf_img_raw},
    {"tiff",      pdf_img_tiff},
    {NULL, 0}
};

const pdc_keyconn pdf_filter_pdfkeylist[] =
{
    {"",                 pdf_comp_none},
    {"LZWDecode",        pdf_comp_lzw},
    {"RunLengthDecode",  pdf_comp_runlength},
    {"CCITTFaxDecode",   pdf_comp_ccitt},
    {"DCTDecode",        pdf_comp_dct},
    {"FlateDecode",      pdf_comp_flate},
    {"JBIG2Decode",      pdf_comp_jbig2},
    {"JPXDecode",        pdf_comp_jpx},
    {NULL, 0}
};

const pdc_keyconn pdf_shortfilter_pdfkeylist[] =
{
    {"",      pdf_comp_none},
    {"LZW",   pdf_comp_lzw},
    {"RL",    pdf_comp_runlength},
    {"CCF",   pdf_comp_ccitt},
    {"DCT",   pdf_comp_dct},
    {"Fl",    pdf_comp_flate},
    {NULL, 0}
};

static const pdc_keyconn pdf_reftype_keys[] =
{
    {"direct",  pdf_ref_direct},
    {"fileref", pdf_ref_file},
    {"url",     pdf_ref_url},
    {NULL, 0}
};

#endif /* P_IMAGE_C */

/* BMP specific image information */
typedef struct pdf_bmp_info_t {
    pdc_uint32          compression;    /* BMP compression */
    pdc_uint32          redmask;        /* red mask */
    pdc_ushort          redmax;         /* red maximal value */
    pdc_ushort          redmove;        /* red mask's movement */
    pdc_uint32          greenmask;      /* green mask */
    pdc_ushort          greenmax;       /* green maximal value */
    pdc_ushort          greenmove;      /* green mask's movement */
    pdc_uint32          bluemask;       /* blue mask */
    pdc_ushort          bluemax;        /* blue maximal value */
    pdc_ushort          bluemove;       /* blue mask's movement */
    pdc_ushort          bpp;            /* bits per pixel */
    size_t              rowbytes;       /* length of row data */
    size_t              rowbytes_pad;   /* padded length of row data */
    size_t              rowbytes_buf;   /* buffer for row data */
    size_t              rowbytes_pdf;   /* length of row data for PDF */
    size_t              skiprows;       /* number of rows to be skipped */
    pdc_byte           *bitmap;         /* bitmap buffer */
    pdc_byte           *end;            /* first byte above bitmap buffer */
    pdc_byte           *pos;            /* current position in bitmap buffer */
} pdf_bmp_info;

typedef struct pdf_jpeg_segment_s pdf_jpeg_segment;

/* JPEG specific image information */
typedef struct pdf_jpeg_info_t {
    const char         *virtfile;       /* temporary virtual file name */
    pdf_jpeg_segment   *seglist;        /* list of segments to be copy */
    int                 capacity;       /* currently allocated size */
    int                 number;         /* next available segment number */
} pdf_jpeg_info;

/* GIF specific image information */
typedef struct pdf_gif_info_t {
    int			useGlobalColormap;
    int			interlace;

    /* LZW decompression state */
    int			ZeroDataBlock;
    int			curbit;
    int			lastbit;
    int			get_done;
    int			last_byte;
    int			return_clear;
    int			*sp;
    int			code_size, set_code_size;
    int			max_code, max_code_size;
    int			clear_code, end_code;
    pdc_byte		buf[280];
    int			firstcode;
    int			oldcode;

    /* These are dynamically malloc'ed to avoid wasting 64KB for each image */
#define MAX_LWZ_BITS            12
#define GIF_TABLE_ELEMENTS	(1<< MAX_LWZ_BITS)
    int			(*table)[GIF_TABLE_ELEMENTS];
    int			*stack;
} pdf_gif_info;


/* PNG specific image information */
typedef struct pdf_png_info_t {
    size_t		nbytes;		/* number of bytes left		*/
    					/* in current IDAT chunk	*/
#ifdef HAVE_LIBPNG
    png_structp		png_ptr;
    png_infop		info_ptr;
    png_uint_32		rowbytes;
    pdc_byte		*raster;
    int			cur_line;
#endif	/* HAVE_LIBPNG */
} pdf_png_info;


/* TIFF specific image information */
typedef struct pdf_tiff_info_t {
#ifdef HAVE_LIBTIFF
    TIFF		*tif;		/* pointer to TIFF data structure */
    uint32		*raster;	/* frame buffer */
#endif	/* HAVE_LIBTIFF */

    int			cur_line;	/* current image row or strip */
} pdf_tiff_info;

/* CCITT specific image information */
typedef struct pdf_ccitt_info_t {
    int			BitReverse;	/* reverse all bits prior to use */
} pdf_ccitt_info;

/* The image descriptor */
struct pdf_image_s {
    pdc_file 		*fp;		/* image file pointer */
    char		*filename;	/* image file name or url */
    /* width and height in pixels, or in points for PDF pages and templates */
    pdc_scalar          width;          /* image width */
    pdc_scalar          height;         /* image height */
    int                 orientation;    /* image orientation according TIFF */
    pdf_compression	compression;	/* image compression type */
    int	                colorspace;	/* image color space */

    /*************************** option variables *****************************/
    pdc_bool            verbose;        /* put out warning/error messages */
    pdc_bool            bitreverse;     /* bitwise reversal of all bytes */
    int                 bpc;            /* bits per color component */
    int                 components;     /* number of color components */
    int                 height_pixel;   /* image height in pixel */
    int                 width_pixel;    /* image width in pixel */
    pdc_bool            ignoremask;     /* ignore any transparency information*/
    pdc_bool            ignoreorient;   /* ignore orientation TIFF tag */
    pdc_bool            doinline;       /* inline image */
    pdc_bool            interpolate;    /* interpolate image   */
    pdc_bool            invert;         /* reverse black and white */
    pdc_bool            jpegoptimize;   /* skip application segments of JPEG */
    pdc_bool            passthrough;    /* pass through mode for TIFF, JPEG */
    int                 K;              /* encoding type of CCITT */
    pdc_bool            imagemask;     	/* create a mask from a 1-bit image */
    int                 mask;           /* image number of image mask */
    pdf_renderingintent ri;             /* rendering intent of image */
    int                 page;           /* page number of TIFF image */
    pdf_ref_type        reference;      /* kind of image data reference */
    char               *iconname;       /* icon name for template images */
    /**************************************************************************/

    pdc_bool		transparent;	/* image is transparent */
    pdc_byte		transval[4];	/* transparent color values */
    pdf_predictor	predictor;	/* predictor for lzw and flate */

    pdc_scalar          dpi_x;          /* horiz. resolution in dots per inch */
    pdc_scalar          dpi_y;          /* vert. resolution in dots per inch */
    					/* dpi is 0 if unknown */

    pdc_bool		in_use;		/* image slot currently in use */

    char		*params;	/* for TIFF */
    int			strips;		/* number of strips in image */
    int			rowsperstrip;	/* number of rows per strip */
    int                 pagehandle;     /* PDI page handle */
    int			dochandle;	/* PDI document handle */
    pdc_usebox		usebox;
    pdc_bool		use_raw;	/* use raw (compressed) image data */
    /* Only relevant for use_raw = false */
    pdc_bool		pixelmode;	/* Use TIFFReadRGBAImageOriented() ? */

    pdf_image_type	type;		/* image type, used for cleanup */
    /* image format specific information */
    union {
        pdf_bmp_info    bmp;
	pdf_jpeg_info	jpeg;
	pdf_gif_info	gif;
	pdf_png_info	png;
	pdf_tiff_info	tiff;
	pdf_ccitt_info	ccitt;
    } info;

    int			no;		/* PDF image number */
    PDF_data_source	src;
};

/* xobject types */
typedef enum {
    image_xobject = 1 << 0,
    form_xobject = 1 << 1,
    pdi_xobject = 1 << 2
} pdf_xobj_type;

typedef enum {
    xobj_flag_used = 1 << 0,		/* in use */
    xobj_flag_write = 1 << 1		/* write at end of page */
} pdf_xobj_flags;

/* A PDF xobject */
struct pdf_xobject_s {
    pdc_id		obj_id;		/* object id of this xobject */
    int			flags;		/* bit mask of pdf_xobj_flags */
    pdf_xobj_type	type;		/* type of this xobject */
};

/* p_bmp.c */
int      pdf_process_BMP_data(PDF *p, int imageslot);
pdc_bool pdf_is_BMP_file(PDF *p, pdc_file *fp);

/* p_ccitt.c */
int      pdf_process_CCITT_data(PDF *p, int imageslot);
int      pdf_process_RAW_data(PDF *p, int imageslot);

/* p_gif.c */
int      pdf_process_GIF_data(PDF *p, int imageslot);
pdc_bool pdf_is_GIF_file(PDF *p, pdc_file *fp);
void	 pdf_cleanup_gif(PDF *p, pdf_image *image);

/* p_jpeg.c */
int      pdf_process_JPEG_data(PDF *p, int imageslot);
pdc_bool pdf_is_JPEG_file(PDF *p, pdc_file *fp);
void     pdf_cleanup_jpeg(PDF *p, pdf_image *image);

/* p_jpx.c */
int      pdf_process_JPX_data(PDF *p, int imageslot);
pdc_bool pdf_is_JPX_file(PDF *p, pdc_file *fp);
void     pdf_cleanup_jpx(PDF *p, pdf_image *image);

/* p_png.c */
int	 pdf_process_PNG_data(PDF *p, int imageslot);
pdc_bool pdf_is_PNG_file(PDF *p, pdc_file *fp);

/* p_tiff.c */
int      pdf_process_TIFF_data(PDF *p, int imageslot);
pdc_bool pdf_is_TIFF_file(PDF *p, pdc_file *fp, pdf_tiff_info *tiff,
                          pdc_bool check);


/* p_image.c */
pdc_id pdf_get_xobject(PDF *p, int im);
void pdf_init_xobjects(PDF *p);
void pdf_write_xobjects(PDF *p);
void pdf_place_xobject(PDF *p, int im, pdc_scalar x, pdc_scalar y,
        const char *optlist);
int pdf_new_xobject(PDF *p, pdf_xobj_type type, pdc_id obj_id);
void pdf_get_page_xobjects(PDF *p, pdf_reslist *rl);
void pdf_mark_page_xobject(PDF *p, int n);
void pdf_cleanup_xobjects(PDF *p);


#endif /* P_IMAGE_H */
