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

/* p_image.c
 *
 * PDFlib image routines
 *
 */

#include <string.h>
#include <math.h>

#include "p_intern.h"

static const char *pdf_filter_names[] = {
    "", "LZWDecode", "RunLengthDecode", "CCITTFaxDecode",
    "DCTDecode", "FlateDecode"
};

static const char *pdf_colorspace_names[] = {
    "DeviceGray", "DeviceRGB", "DeviceCMYK", "CalGray", "CalRGB", "Lab", 
    "Indexed", "Pattern", "Separation"
};

static void
pdf_init_image_struct(PDF *p, pdf_image *image)
{
    image->transparent	= pdf_false;
    image->reference	= pdf_ref_direct;
    image->compression	= none;
    image->invert	= pdf_false;
    image->predictor	= 1;
    image->in_use	= pdf_false;
    image->filename	= (char *) NULL;
    image->mask		= -1;
    image->params	= (char *) NULL;
    image->dpi_x	= (float) 0;
    image->dpi_y	= (float) 0;
    image->strips	= 1;
    image->rowsperstrip	= 1;
    image->colormap	= (pdf_colormap *) NULL;
    image->colormap_id	= BAD_ID;

    if (p)
	return;
}

void
pdf_init_images(PDF *p)
{
    int im;

    p->images_capacity = IMAGES_CHUNKSIZE;

    p->images = (pdf_image *) 
    	p->malloc(p, sizeof(pdf_image) * p->images_capacity, "pdf_init_images");

    for (im = 0; im < p->images_capacity; im++)
	pdf_init_image_struct(p, &(p->images[im]));
}

void
pdf_grow_images(PDF *p)
{
    int im;

    p->images = (pdf_image *) p->realloc(p, p->images,
	sizeof(pdf_image) * 2 * p->images_capacity, "pdf_grow_images");

    for (im = p->images_capacity; im < 2 * p->images_capacity; im++)
	pdf_init_image_struct(p, &(p->images[im]));

    p->images_capacity *= 2;
}

void
pdf_cleanup_images(PDF *p)
{
    int im;

    if (!p->images)
	return;

    /* Free images which the caller left open */
    for (im = 0; im < p->images_capacity; im++)
	if (p->images[im].in_use)		/* found used slot */
	    PDF_close_image(p, im);	/* free image descriptor */

    if (p->images)
	p->free(p, p->images);

    p->images = NULL;
}

void
pdf_init_xobjects(PDF *p)
{
    int index;

    p->xobjects_number	= 0;
    p->xobjects_capacity = XOBJECTS_CHUNKSIZE;

    p->xobjects = (pdf_xobject *) 
    	p->malloc(p, sizeof(pdf_xobject) * p->xobjects_capacity,
	"pdf_init_xobjects");

    for (index = 0; index < p->xobjects_capacity; index++)
	p->xobjects[index].used_on_current_page = pdf_false;
}

void
pdf_grow_xobjects(PDF *p)
{
    int index;

    p->xobjects = (pdf_xobject *) p->realloc(p, p->xobjects,
	sizeof(pdf_xobject) * 2 * p->xobjects_capacity, "pdf_grow_xobjects");

    for (index = p->xobjects_capacity; index < 2 * p->xobjects_capacity; index++)
	p->xobjects[index].used_on_current_page = pdf_false;

    p->xobjects_capacity *= 2;
}

void
pdf_write_xobjects(PDF *p)
{
    int index;

    if (p->xobjects_number > 0) {
	pdf_puts(p, "/XObject");

	pdf_begin_dict(p);	/* XObject */

	for (index = 0; index < p->xobjects_number; index++) {
	    if (p->xobjects[index].used_on_current_page) {
		pdf_printf(p, "/I%d %ld 0 R\n", index, 
			p->xobjects[index].obj_id);
		p->xobjects[index].used_on_current_page = pdf_false;
	    }
	}

	pdf_end_dict(p);	/* XObject */
    }
}

void
pdf_cleanup_xobjects(PDF *p)
{
    if (p->xobjects) {
	p->free(p, p->xobjects);
	p->xobjects = NULL;
    }
}

/* methods for constructing a data source from a memory buffer */

/* dummy for use in PDF_data_source */
static void
pdf_noop(PDF *p, PDF_data_source *src)
{
    (void) p;	/* avoid compiler warning "unreferenced parameter" */
    (void) src;	/* avoid compiler warning "unreferenced parameter" */
}

static int
pdf_data_source_buf_fill(PDF *p, PDF_data_source *src)
{
    (void) p;	/* avoid compiler warning "unreferenced parameter" */

    if (src->next_byte == NULL) {
	src->next_byte		= src->buffer_start;
	src->bytes_available	= src->buffer_length;
	return pdf_true;
    }

    return pdf_false;
}

/* structure for 32-bit aligned memory data */
typedef struct {
    size_t cur_scanline;
    size_t num_scanlines;
    size_t scanline_widthbytes;
    size_t scanline_widthbytes_aligned;
} FILL_ALIGN, *PFILL_ALIGN;

static int
pdf_data_source_buf_fill_aligned(PDF *p, PDF_data_source *src)
{
    PFILL_ALIGN pfa = (PFILL_ALIGN) src->private_data;

    (void) p;	/* avoid compiler warning "unreferenced parameter" */

    if (pfa->cur_scanline < pfa->num_scanlines) {
	src->next_byte =
	    src->buffer_start +
		pfa->scanline_widthbytes_aligned * pfa->cur_scanline;
	src->bytes_available = pfa->scanline_widthbytes;
	pfa->cur_scanline++;
	return pdf_true;
    }

    return pdf_false;
}

void
pdf_put_image(PDF *p, int im, pdf_bool firststrip)
{
    id		length_id;
    long	length;
    pdf_image	*image;
    int		i;

    image = &p->images[im];

    switch (image->colorspace) {
        case ImageMask:
        case DeviceGray:
        case Indexed:
        case DeviceRGB:
        case DeviceCMYK:
	    break;

	default:
	    pdf_error(p, PDF_SystemError, "Unknown color space");
	    break;
    }

    /* Images may also be written to the output before the first page */
    if (p->state == pdf_state_page_description)
	pdf_end_contents_section(p);

    /* Image object */

    image->no = p->xobjects_number++;
    p->xobjects[image->no].obj_id = pdf_begin_obj(p, NEW_ID);	/* XObject */

    if (p->xobjects_number >= p->xobjects_capacity)
	pdf_grow_xobjects(p);

    pdf_begin_dict(p); 		/* XObject */

    pdf_puts(p, "/Type/XObject\n");
    pdf_puts(p, "/Subtype/Image\n");

    pdf_printf(p,"/Width %d\n", image->width);
    pdf_printf(p,"/Height %d\n", image->height);
    pdf_printf(p,"/BitsPerComponent %d\n", image->bpc);

    /* 
     * Transparency handling
     */

    /* Masking by color: single transparent color value */
    if (image->transparent) {
	if (image->colorspace == Indexed || image->colorspace == DeviceGray)
	    pdf_printf(p,"/Mask[%d %d]\n",
		image->transval[0], image->transval[0]);

	else if (image->colorspace == DeviceRGB)
	    pdf_printf(p,"/Mask[%d %d %d %d %d %d]\n",
		image->transval[0], image->transval[0],
		image->transval[1], image->transval[1],
		image->transval[2], image->transval[2]);

	else if (image->colorspace == DeviceCMYK)
	    pdf_printf(p,"/Mask[%d %d %d %d %d %d %d %d]\n",
		image->transval[0], image->transval[0],
		image->transval[1], image->transval[1],
		image->transval[2], image->transval[2],
		image->transval[3], image->transval[3]);
	else
	    pdf_error(p, PDF_SystemError,
	    	"Unknown color space with transparency");

    /* Masking by position: separate bitmap mask */
    } else if (image->mask != -1) {
	pdf_printf(p, "/Mask %ld 0 R\n", p->xobjects[image->mask].obj_id);
    }

    switch (image->colorspace) {
	case ImageMask:
	    pdf_puts(p, "/ImageMask true\n");
	    break;

	case Indexed:
	    if (firststrip)
		image->colormap_id = pdf_alloc_id(p);

	    pdf_puts(p, "/ColorSpace[/Indexed/DeviceRGB ");
	    pdf_printf(p, "%d %ld 0 R]\n", image->palette_size - 1, image->colormap_id);
	    break;
	
	default:
	    pdf_printf(p, "/ColorSpace/%s\n",
		pdf_colorspace_names[image->colorspace]);
	    break;
    }

    /* special case: referenced image data instead of direct data */
    if (image->reference != pdf_ref_direct) {

	if (image->compression != none) {
	    pdf_printf(p, "/FFilter[/%s]\n",
		    pdf_filter_names[image->compression]);
	}

	if (image->compression == ccitt) {
	    pdf_puts(p, "/FDecodeParms[<<");

	    if (image->width != 1728)	/* CCITT default width */
		pdf_printf(p, "/Columns %d", image->width);

	    pdf_printf(p, "/Rows %d", image->height);

	    /* write CCITT parameters if required */
	    if (image->params)
		pdf_puts(p, image->params);

	    pdf_puts(p, ">>]\n");

	}

	if (image->reference == pdf_ref_file) {

	    /* TODO: make image file name platform-neutral:
	     * Change : to / on the Mac
	     * Change \ to / on Windows
	     */
	    pdf_printf(p, "/F(%s)/Length 0", image->filename);

	} else if (image->reference == pdf_ref_url) {

	    pdf_printf(p, "/F<</FS/URL/F(%s)>>/Length 0", image->filename);
	}

	pdf_end_dict(p);		/* XObject */

	pdf_begin_stream(p);		/* dummy image data */
	pdf_end_stream(p);		/* dummy image data */

	if (p->state == pdf_state_page_description)
	    pdf_begin_contents_section(p);

	return;
    }

    /* 
     * Now the (more common) handling of actual image
     * data to be included in the PDF output.
     */

    /* do we need a filter (either ASCII or decompression)? */

    if (p->debug['a']) {
	pdf_puts(p, "/Filter[/ASCIIHexDecode");
	if (image->compression != none)
	    pdf_printf(p, "/%s", pdf_filter_names[image->compression]);
	pdf_puts(p, "]\n");

    } else {
	/* not a recognized precompressed image format */
	if (image->compression == none && p->compress)
	    image->compression = flate;

	if (image->compression != none)
	    pdf_printf(p, "/Filter[/%s]\n",
		    pdf_filter_names[image->compression]);
    }

    if (image->compression == lzw) {
	pdf_printf(p, "/DecodeParms[%s<<", (p->debug['a'] ? "null" : ""));

	/* write EarlyChange parameter if required */
	if (image->params)
	    pdf_puts(p, image->params);

	if (image->predictor == 2) {
	    pdf_printf(p, "/Predictor 2");
	    pdf_printf(p, "/Columns %d", image->width);
	    pdf_printf(p, "/BitsPerComponent %d", image->bpc);
	    switch (image->colorspace) {
		case DeviceGray:
		    pdf_puts(p, "/Colors 1");
		    break;
		case DeviceRGB:
		    pdf_puts(p, "/Colors 3");
		    break;
		case DeviceCMYK:
		    pdf_puts(p, "/Colors 4");
		    break;
		default:
		    pdf_error(p, PDF_SystemError,
			"Unknown colorspace (%d)", image->colorspace);
		    break;
	    }
	}
	pdf_puts(p, ">>]\n");
    }

    if (image->compression == ccitt) {
	pdf_printf(p, "/DecodeParms[%s<<", (p->debug['a'] ? "null" : ""));

	if (image->width != 1728)	/* CCITT default width */
	    pdf_printf(p, "/Columns %d", image->width);

	pdf_printf(p, "/Rows %d", image->height);

	/* write CCITT parameters if required */
	if (image->params)
	    pdf_puts(p, image->params);

	pdf_puts(p, ">>]\n");
    }

    if (image->invert) {
	pdf_puts(p, "/Decode[1 0");
	for (i = 1; i < image->components; i++)
	    pdf_puts(p, " 1 0");
	pdf_puts(p, "]\n");
    }

    /* Write the actual image data */
    length_id = pdf_alloc_id(p);

    pdf_printf(p,"/Length %ld 0 R\n", length_id);
    pdf_end_dict(p);		/* XObject */

    pdf_begin_stream(p);	/* image data */
    p->start_contents_pos = pdf_tell(p);

    /* image data */

    if (p->debug['a'])
	pdf_ASCIIHexEncode(p, &image->src);
    else {
	if (image->compression == flate)
	    pdf_compress(p, &image->src);
	else
	    pdf_copy(p, &image->src);
    }

    length = pdf_tell(p) - p->start_contents_pos;

    pdf_end_stream(p);	/* image data */
    pdf_end_obj(p);	/* XObject */

    pdf_begin_obj(p, length_id);		/* Length object */
    pdf_printf(p,"%ld\n", length);
    pdf_end_obj(p);

    if (p->stream.flush & PDF_FLUSH_CONTENT)
	pdf_flush_stream(p);

    /* Image data done */

    /*
     * Write colormap information for indexed color spaces
     */
    if (firststrip && image->colorspace == Indexed) {
	pdf_begin_obj(p, image->colormap_id);		/* colormap object */
	pdf_begin_dict(p); 		

	if (p->debug['a'])
	    pdf_puts(p, "/Filter[/ASCIIHexDecode]\n");
	else if (p->compress)
	    pdf_puts(p, "/Filter[/FlateDecode]\n");

	/* Length of colormap object */
	length_id = pdf_alloc_id(p);
	pdf_printf(p,"/Length %ld 0 R\n", length_id);
	pdf_end_dict(p);

	pdf_begin_stream(p);			/* colormap data */
	p->start_contents_pos = pdf_tell(p);

	if (image->components != 1) {
	    pdf_error(p, PDF_SystemError,
	    	"Bogus indexed colorspace (%d color components)",
		image->components);
	}

	image->src.init		= pdf_noop;
	image->src.fill		= pdf_data_source_buf_fill;
	image->src.terminate	= pdf_noop;

	image->src.buffer_start	= (unsigned char *) image->colormap;
	image->src.buffer_length= (size_t) (image->palette_size * 3);

	image->src.bytes_available = 0;
	image->src.next_byte	= NULL;

	/* Write colormap data */
	if (p->debug['a'])
	    pdf_ASCIIHexEncode(p, &image->src);
	else {
	    pdf_compress(p, &image->src);
	}

	length = pdf_tell(p) - p->start_contents_pos;

	pdf_end_stream(p);			/* Colormap data */
	pdf_end_obj(p);				/* Colormap object */

	pdf_begin_obj(p, length_id);		/* Length object for colormap */
	pdf_printf(p, "%ld\n", length);
	pdf_end_obj(p);				/* Length object for colormap */
    }

    if (p->state == pdf_state_page_description)
	pdf_begin_contents_section(p);

    if (p->stream.flush & PDF_FLUSH_CONTENT)
	pdf_flush_stream(p);
}

PDFLIB_API void PDFLIB_CALL
PDF_place_image(PDF *p, int im, float x, float y, float scale)
{
    pdf_matrix m;
    pdf_image *image;
    int row;
    int imageno;

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (im < 0 || im >= p->images_capacity || !p->images[im].in_use)
    	pdf_error(p, PDF_ValueError,
		"Bad image number %d in PDF_place_image", im);

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_place_image)");

    image = &p->images[im];

    if (scale == 0.0)
	pdf_error(p, PDF_ValueError,
		"Scaling factor 0 for image %s", image->filename);

    switch (image->colorspace) {
        case ImageMask:
        case DeviceGray:
	    p->procset	|= ImageB;
	    break;

	/*
	 * It appears that indexed images require both, although this is
	 * not documented.
	 */
        case Indexed:
	    p->procset	|= ImageI;
	    p->procset	|= ImageC;
	    break;

        case DeviceRGB:
        case DeviceCMYK:
	    p->procset	|= ImageC;
	    break;

	default:
	    pdf_error(p, PDF_SystemError,
	    	"Bogus colorspace (%d) in PDF_place_image", (int) image->colorspace);
    }

    pdf_end_text(p);
    pdf_end_path(p, pdf_true);

    pdf_begin_contents_section(p);

    imageno = image->no;	/* number of first strip */

    if (image->strips == 1)
	image->rowsperstrip = image->height;

    for (row = 0; row < image->height; row += image->rowsperstrip, imageno++) {
	int height;	/* height of the current strip */

	height = (row + image->rowsperstrip > image->height ? 
		    image->height - row : image->rowsperstrip);

	PDF_save(p);

	m.a = image->width * scale;
	m.d = height * scale;
	m.b = m.c = (float) 0.0;
	m.e = x;
	m.f = y + scale * (image->height - row - height);
	pdf_concat_raw(p, m);

	pdf_printf(p, "/I%d Do\n", imageno);
	p->xobjects[imageno].used_on_current_page = pdf_true;

	if (image->mask != -1)
	    p->xobjects[image->mask].used_on_current_page = pdf_true;

	PDF_restore(p);
    }
}

#ifdef PDF_THUMBNAILS_SUPPORTED
/*
 * The following function is only supplied for Leonard Rosenthol's
 * PDFWriter clone which has access to thumbnail JPEG images for the page.
 * The would-be thumbnail image must be acquired like other PDFlib images
 * (via PDF_open_JPEG(), for example), and hooked in as thumbnail image
 * sometime during a page description. Not all pages need to have their
 * own thumbnail image.
 *
 * Note that the PDF spec calls for a maximum thumbnail size of
 * 106 x 106 pixels, and demands DeviceGray or indexed DeviceRGB.
 * While the former doesn't seem to be a strict condition, the latter is.
 *
 * Strictly speaking, re-using Xobjects as thumbnails is a little kludgey
 * since thumbnails are not supposed to have /Type and /Subtype entries,
 * but Acrobat simply doesn't care.
 */

PDFLIB_API void PDFLIB_CALL
PDF_add_thumbnail(PDF *p, int im)
{
    pdf_image *image;

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (im < 0 || im >= p->images_capacity || !p->images[im].in_use)
    	pdf_error(p, PDF_ValueError,
		"Bad image number %d in PDF_add_thumbnail", im);

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_add_thumbnail)");
    
    image = &p->images[im];

    if (image->width > 106 || image->height > 106)
    	pdf_error(p, PDF_NonfatalError, "Thumbnail image too large", im);

    /* now for the real work :-) */
    p->thumb_id = p->xobjects[image->no].obj_id;
}
#endif	/* PDF_THUMBNAILS_SUPPORTED */

PDFLIB_API void PDFLIB_CALL
PDF_close_image(PDF *p, int im)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (im < 0 || im >= p->images_capacity || !p->images[im].in_use)
    	pdf_error(p, PDF_ValueError,
		"Bad image number %d in PDF_close_image", im);

    /* clean up parameter string if necessary */
    if (p->images[im].params) {
	p->free(p, p->images[im].params);
	p->images[im].params = NULL;
    }

    if (p->images[im].filename)
	p->free(p, p->images[im].filename);

    /* free the image slot and prepare for next use */
    pdf_init_image_struct(p, &(p->images[im]));
}

/* interface for using image data directly in memory */

PDFLIB_API int PDFLIB_CALL
PDF_open_image(PDF *p, const char *type, const char *source, const char *data, long length, int width, int height, int components, int bpc, const char *params)
{
    pdf_image *image;
    int im;

    if (PDF_SANITY_CHECK_FAILED(p))
	return -1;

    if (type == NULL || *type == '\0')
    	pdf_error(p, PDF_ValueError, "No image type in PDF_open_image");

    if (source == NULL || *source == '\0')
    	pdf_error(p, PDF_ValueError, "No image source in PDF_open_image");

    if (!strcmp(type, "raw") && data == NULL)
    	pdf_error(p, PDF_ValueError, "Bad raw image pointer in PDF_open_image");

    if (strcmp(type, "ccitt") && strcmp(type, "raw")
	    && params != NULL && *params != '\0')
    	pdf_error(p, PDF_NonfatalError,
	    "Unnecessary CCITT parameter in PDF_open_image");

    for (im = 0; im < p->images_capacity; im++)
	if (!p->images[im].in_use)		/* found free slot */
	    break;

    if (im == p->images_capacity) 
	pdf_grow_images(p);

    image = &p->images[im];

    if (!strcmp(type, "jpeg")) {

	image->compression = dct;

    } else if (!strcmp(type, "ccitt")) {
	image->compression = ccitt;

	if (length < 0L) {
	    image->info.ccitt.BitReverse = pdf_true;
	    length = -length;
	}

	if (params != NULL && *params != '\0')
	    image->params = pdf_strdup(p, params);
	else
	    image->params = NULL;

    } else if (!strcmp(type, "raw")) {

	image->compression = none;

    } else
	pdf_error(p, PDF_ValueError,
	    "Unknown image type '%s' in PDF_open_image", type);

    switch (components) {
	case 1:
	    if (params && !strcmp(params, "mask")) {
		if (strcmp(type, ("raw")) || bpc != 1 || components != 1)
		    pdf_error(p, PDF_ValueError,
			"Unsuitable image mask in PDF_open_image");
		image->colorspace = ImageMask;
	    } else
		image->colorspace = DeviceGray;
	    break;

	case 3:
	    image->colorspace = DeviceRGB;
	    break;

	case 4:
	    image->colorspace = DeviceCMYK;
	    break;

	default:
	    pdf_error(p, PDF_ValueError,
	    	"Bogus number of components (%d) in PDF_open_image",
		components);
    }

    image->width		= width;
    image->height		= height;
    image->bpc			= bpc;
    image->components		= components;
    image->in_use		= pdf_true;		/* mark slot as used */

    if (!strcmp(source, "memory")) {	/* in-memory image data */
	if (image->compression == none && length != (long)
	    	(height * ((width * components * bpc + 7) / 8)))
	    pdf_error(p, PDF_ValueError,
		"Bogus image data length '%ld' in PDF_open_image", length);

	image->src.init		= pdf_noop;
	image->src.fill		= pdf_data_source_buf_fill;
	image->src.terminate	= pdf_noop;

	image->src.buffer_start	= (unsigned char *) data;
	image->src.buffer_length= (size_t) length;

	image->src.bytes_available = 0;
	image->src.next_byte	= NULL;

    } else if (!strcmp(source, "memory32")) {
	/* 32 bit aligned in-memory image data */
	FILL_ALIGN fa;

	if (image->compression == none && length != (long)
	    	(height * ((width * components * bpc + 7) / 8)))
	    pdf_error(p, PDF_ValueError,
		"Bogus image data length '%ld' in PDF_open_image", length);

	image->src.buffer_start	= (unsigned char *) data;
	image->src.buffer_length= (size_t) length;

	image->src.init		= pdf_noop;
	image->src.fill		= pdf_data_source_buf_fill_aligned;
	image->src.terminate	= pdf_noop;

	fa.cur_scanline = 0;
	fa.num_scanlines = (size_t) height;
	fa.scanline_widthbytes = (size_t) ((width * components * bpc + 7) / 8);
	/* dword align */
	fa.scanline_widthbytes_aligned = (fa.scanline_widthbytes + 3) & ~0x3;

	image->src.private_data = (void *) &fa;
	image->src.bytes_available = 0;
	image->src.next_byte	= NULL;

    } else if (!strcmp(source, "fileref")) {	/* file reference */
	if (p->compatibility == PDF_1_2)
	    pdf_error(p, PDF_RuntimeError,
		"External image file references are not supported in PDF 1.2");

	image->reference	= pdf_ref_file;
	image->filename		= pdf_strdup(p, data);

    } else if (!strcmp(source, "url")) {	/* url reference */
	if (p->compatibility == PDF_1_2)
	    pdf_error(p, PDF_RuntimeError,
		"External image URLs are not supported in PDF 1.2");

	image->reference	= pdf_ref_url;
	image->filename		= pdf_strdup(p, data);

    } else			/* error */
	pdf_error(p, PDF_ValueError,
	    "Bogus image data source '%s' in PDF_open_image", source);

    pdf_put_image(p, im, pdf_true);
    return im;
}

PDFLIB_API int PDFLIB_CALL
PDF_open_image_file(PDF *p, const char *type, const char *filename, const char *stringparam, int intparam)
{
    int imageslot;
    int ret;

    if (PDF_SANITY_CHECK_FAILED(p))
	return -1;

    if (type == NULL || *type == '\0')
	pdf_error(p, PDF_ValueError,
	    "Bad image file type for image %s", filename);

    if (filename == NULL || *filename == '\0')
	pdf_error(p, PDF_ValueError, "Bad image file name for %s image", type);

    if (stringparam &&
    	p->compatibility == PDF_1_2 && !strcmp(stringparam, "masked"))
	pdf_error(p, PDF_RuntimeError,
		"Masked images are not supported in PDF 1.2");


    for (imageslot = 0; imageslot < p->images_capacity; imageslot++)
	if (!p->images[imageslot].in_use)		/* found free slot */
	    break;

    if (imageslot == p->images_capacity) 
	pdf_grow_images(p);

    if (!strcmp(type, "png") || !strcmp(type, "PNG")) {
#ifdef HAVE_LIBPNG
	ret = pdf_open_PNG_data(p, imageslot, filename, stringparam, intparam);

	if (ret == -1)
	    pdf_init_image_struct(p, &p->images[imageslot]);
	return ret;
#else
	pdf_error(p, PDF_NonfatalError,
	    "PNG images not supported in this configuration");
	return -1;
#endif
    }

    if (!strcmp(type, "gif") || !strcmp(type, "GIF")) {
	ret = pdf_open_GIF_data(p, imageslot, filename, stringparam, intparam);

	if (ret == -1)
	    pdf_init_image_struct(p, &p->images[imageslot]);
	return ret;
    }

    if (!strcmp(type, "tiff") || !strcmp(type, "TIFF")) {
#ifdef HAVE_LIBTIFF
	ret = pdf_open_TIFF_data(p, imageslot, filename, stringparam, intparam);

	if (ret == -1)
	    pdf_init_image_struct(p, &p->images[imageslot]);
	return ret;
#else
	pdf_error(p, PDF_NonfatalError,
	    "TIFF images not supported in this configuration");
	return -1;
#endif
    }

    if (!strcmp(type, "jpeg") || !strcmp(type, "JPEG")) {
	ret = pdf_open_JPEG_data(p, imageslot, filename, stringparam, intparam);

	if (ret == -1)
	    pdf_init_image_struct(p, &p->images[imageslot]);
	return ret;
    }

    pdf_error(p, PDF_ValueError,
	"Image type %s for image file %s not supported", type, filename);

    /* never reached */
    return -1;
}
