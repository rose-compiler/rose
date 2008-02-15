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

/* p_png.c
 *
 * PNG processing for PDFlib
 *
 */

#include "p_intern.h"

#ifndef HAVE_LIBPNG

int pdf_prevent_compiler_error_for_empty_file;

#else

/*
 * use the libpng portability aid in an attempt to overcome version differences
 */
#ifndef png_jmpbuf
#define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif

static void
pdf_data_source_PNG_init(PDF *p, PDF_data_source *src)
{
  pdf_image	*image;

  image = (pdf_image *) src->private_data;

  image->info.png.cur_line = 0;
  src->buffer_length = image->info.png.rowbytes;

  if (p)
    return;
}

static pdf_bool
pdf_data_source_PNG_fill(PDF *p, PDF_data_source *src)
{
    pdf_image	*image;

    (void) p;	/* avoid compiler warning "unreferenced parameter" */

    image = (pdf_image *) src->private_data;

    if (image->info.png.cur_line == image->height)
	return pdf_false;

    src->next_byte = image->info.png.raster +
    image->info.png.cur_line * image->info.png.rowbytes;

    src->bytes_available = src->buffer_length;

    image->info.png.cur_line++;

    return pdf_true;
}

static void
pdf_data_source_PNG_terminate(PDF *p, PDF_data_source *src)
{
    (void) p;	/* avoid compiler warning "unreferenced parameter" */
    (void) src;	/* avoid compiler warning "unreferenced parameter" */

    /* do nothing */
    return;
}

/* 
 * We suppress libpng's warning message by suppyling 
 * our own error and warning handlers
*/
static void
pdf_libpng_warning_handler(png_structp png_ptr, png_const_charp message)
{
    (void) png_ptr;	/* avoid compiler warning "unreferenced parameter" */
    (void) message;	/* avoid compiler warning "unreferenced parameter" */

    /* do nothing */
    return;
}

static void
pdf_libpng_error_handler(png_structp png_ptr, png_const_charp message)
{
    (void) png_ptr;	/* avoid compiler warning "unreferenced parameter" */
    (void) message;	/* avoid compiler warning "unreferenced parameter" */

    longjmp(png_jmpbuf(png_ptr), 1);
}

int
pdf_open_PNG_data(PDF *p, int imageslot, const char *filename, const char *stringparam, int intparam)
{
    pdf_byte sig[8];
    png_uint_32 width, height, ui;
    png_bytep *row_pointers, trans;
    png_color_8p sig_bit;
    png_color_16p trans_values;
    int bit_depth, color_type, i, num_trans, mask = -1;
    float dpi_x, dpi_y;
    pdf_image *image;

    image = &p->images[imageslot];

    if (stringparam && !strcmp(stringparam, "masked")) {
	mask = intparam;
	if (mask >= 0 &&
	    (mask >= p->images_capacity || !p->images[mask].in_use ||
	    p->images[mask].colorspace != ImageMask))
	    pdf_error(p, PDF_ValueError,
		"Bad image mask (no %d) for image '%s'", mask, filename);
    }

    /* 
     * There may be old versions of libpng around. We try to be
     * prepared for both kinds of library interfaces.
     */

#if PNG_LIBPNG_VER > 88 

    /* 
     * We can't install our own memory handlers in libpng since the
     * PNG library must have been specially built to support this.
     */

    image->info.png.png_ptr =
	    png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
		pdf_libpng_error_handler, pdf_libpng_warning_handler);

    if (!image->info.png.png_ptr) {
	pdf_error(p, PDF_MemoryError,
	    "Couldn't allocate memory for PNG handler");
    }

    image->info.png.info_ptr = png_create_info_struct(image->info.png.png_ptr);

    if (image->info.png.info_ptr == NULL) {
	png_destroy_read_struct(&image->info.png.png_ptr,
	    (png_infopp) NULL, (png_infopp) NULL);
	pdf_error(p, PDF_MemoryError,
	    "Couldn't allocate memory for PNG handler");
    }

#else	/* old libpng version */

    image->info.png.png_ptr =
	    (png_structp) p->malloc(p, sizeof(png_struct), "pdf_open_PNG/png_ptr");

    png_read_init(pp);

    image->info.png.info_ptr =
	    (png_infop) p->malloc(p, sizeof(png_info), "pdf_open_PNG/info");

#endif	/* old libpng version */

    if (setjmp(png_jmpbuf(image->info.png.png_ptr))) {
	fclose(image->fp);
	png_destroy_read_struct(&image->info.png.png_ptr,
	    &image->info.png.info_ptr, NULL);
	if (p->debug['i']) {
	    pdf_error(p, PDF_NonfatalError, "Error reading PNG file '%s'", filename);
	}
	return -1;
    }

    /* now open the image file */
    if ((image->fp = fopen(filename, READMODE)) == NULL) {
	if (p->debug['i']) {
	    pdf_error(p, PDF_NonfatalError, "Couldn't open PNG file '%s'", filename);
	}
	return -1;	/* Couldn't open PNG file */
    }

    if (fread(sig, 1, 8, image->fp) == 0 || !png_check_sig(sig, 8)) {
	fclose(image->fp);
	if (p->debug['i']) {
	    pdf_error(p, PDF_NonfatalError, "File '%s' doesn't appear to be a PNG file",
	    filename);
	}
	return -1;
    }

    png_init_io(image->info.png.png_ptr, image->fp);
    png_set_sig_bytes(image->info.png.png_ptr, 8);
    png_read_info(image->info.png.png_ptr, image->info.png.info_ptr);
    png_get_IHDR(image->info.png.png_ptr, image->info.png.info_ptr,
	    &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

    image->width	= (int) width;
    image->height	= (int) height;

    /* reduce 16-bit images to 8 bit since PDF stops at 8 bit */
    if (bit_depth == 16) {
	png_set_strip_16(image->info.png.png_ptr);
	bit_depth = 8;
    }

    image->bpc		= bit_depth;

    /* 
     * Since PDF doesn't support a real alpha channel but only binary
     * tranparency ("poor man's alpha"), we do our best and treat
     * alpha values of up to 50% as transparent, and values above 50%
     * as opaque.
     *
     * Since this behaviour is not exactly what the image author had in mind, 
     * it should probably be made user-configurable.
     *
     */
#define ALPHA_THRESHOLD 128

    switch (color_type) {
	case PNG_COLOR_TYPE_GRAY_ALPHA:
	    /* TODO: construct mask from alpha channel */
	    /*
	    png_set_IHDR(image->info.png.png_ptr, image->info.png.info_ptr,
	    	width, height, bit_depth, PNG_COLOR_MASK_ALPHA,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT);
	    */
	    png_set_strip_alpha(image->info.png.png_ptr);
	    /* fall through */

	case PNG_COLOR_TYPE_GRAY:
	    if (png_get_sBIT(image->info.png.png_ptr,
	    		image->info.png.info_ptr, &sig_bit)) {
		png_set_shift(image->info.png.png_ptr, sig_bit);
	    }

	    image->colorspace	= DeviceGray;
	    image->components	= 1;
	    break;

	case PNG_COLOR_TYPE_RGB_ALPHA:
	    /* TODO: construct mask from alpha channel */
	    png_set_strip_alpha(image->info.png.png_ptr);
	    /* fall through */

	case PNG_COLOR_TYPE_RGB:
	    image->colorspace	= DeviceRGB;
	    image->components	= 3;

	    break;

	case PNG_COLOR_TYPE_PALETTE:
	    png_get_PLTE(image->info.png.png_ptr, image->info.png.info_ptr,
			(png_colorp*) &image->colormap, &image->palette_size);

	    image->colorspace	= Indexed;
	    image->components	= 1;

	    break;
    }

    if (stringparam && *stringparam) {
	if (!strcmp(stringparam, "mask")) {
	    if (image->components != 1 || image->bpc != 1) {
		fclose(image->fp);
		pdf_error(p, PDF_ValueError, "Unsuitable PNG image mask (not a bitmap)");
	    }
	    image->colorspace = ImageMask;

	} else if (!strcmp(stringparam, "masked")) {
	    mask = intparam;
	    if (mask >= 0 &&
		(mask >= p->images_capacity || !p->images[mask].in_use ||
		p->images[mask].colorspace != ImageMask)) {
		    fclose(image->fp);
		    pdf_error(p, PDF_ValueError,
		    "Bad image mask (no %d) for image '%s'", mask, filename);
	    }
	} else if (!strcmp(stringparam, "ignoremask")) {
	    /* we invert this flag later */
	    image->transparent = pdf_true;
	} else {
	    fclose(image->fp);
	    pdf_error(p, PDF_ValueError,
	    	"Unknown parameter %s in pdf_open_PNG", stringparam);
	}
    }

    image->mask		= mask;

    /* let libpng expand interlaced images */
    (void) png_set_interlace_handling(image->info.png.png_ptr);

    /* read the physical dimensions chunk to find the resolution values */
    dpi_x = (float) png_get_x_pixels_per_meter(image->info.png.png_ptr,
    		image->info.png.info_ptr);
    dpi_y = (float) png_get_y_pixels_per_meter(image->info.png.png_ptr,
    		image->info.png.info_ptr);

    if (dpi_x != 0 && dpi_y != 0) {	/* absolute values */
	image->dpi_x = dpi_x;
	image->dpi_y = dpi_y;

    } else {				/* aspect ratio */
	image->dpi_x = (float) -1.0;
	image->dpi_y = -png_get_pixel_aspect_ratio(image->info.png.png_ptr,
			    image->info.png.info_ptr);
    }

    /* read the transparency chunk */
    if (png_get_valid(image->info.png.png_ptr, image->info.png.info_ptr,
    	PNG_INFO_tRNS)) {
	png_get_tRNS(image->info.png.png_ptr, image->info.png.info_ptr,
	    &trans, &num_trans, &trans_values);
	if (num_trans > 0) {
	    if (color_type == PNG_COLOR_TYPE_GRAY) {
		image->transparent = !image->transparent;
		/* TODO: scale down 16-bit transparency values ? */
		image->transval[0] = (pdf_byte) trans_values[0].gray;

	    } else if (color_type == PNG_COLOR_TYPE_RGB) {
		image->transparent = !image->transparent;
		/* TODO: scale down 16-bit transparency values ? */
		image->transval[0] = (pdf_byte) trans_values[0].red;
		image->transval[1] = (pdf_byte) trans_values[0].green;
		image->transval[2] = (pdf_byte) trans_values[0].blue;

	    } else if (color_type == PNG_COLOR_TYPE_PALETTE) {
		/* we use the first transparent entry in the tRNS palette */
		for (i = 0; i < num_trans; i++) {
		    if ((pdf_byte) trans[i] < ALPHA_THRESHOLD) {
			image->transparent = !image->transparent;
			image->transval[0] = (pdf_byte) i;
			break;
		    }
		}
	    }
	}
    }

    png_read_update_info(image->info.png.png_ptr, image->info.png.info_ptr);

    image->info.png.rowbytes =
	png_get_rowbytes(image->info.png.png_ptr, image->info.png.info_ptr);

    image->info.png.raster = (pdf_byte *)
	p->malloc(p,image->info.png.rowbytes * height, "pdf_open_PNG/raster");

    row_pointers = (png_bytep *)
	p->malloc(p, height * sizeof(png_bytep), "pdf_open_PNG/pointers");

    for (ui = 0; ui < height; ui++) {
	row_pointers[ui] =
	    image->info.png.raster + ui * image->info.png.rowbytes;
    }

    /* fetch the actual image data */
    png_read_image(image->info.png.png_ptr, row_pointers);

    image->in_use		= pdf_true;		/* mark slot as used */
    image->filename		= pdf_strdup(p, filename);

    image->src.init		= pdf_data_source_PNG_init;
    image->src.fill		= pdf_data_source_PNG_fill;
    image->src.terminate	= pdf_data_source_PNG_terminate;
    image->src.private_data	= (void *) image;

    pdf_put_image(p, imageslot, pdf_true);

    fclose(image->fp);

    p->free(p, image->info.png.raster);
    p->free(p, row_pointers);

#if PNG_LIBPNG_VER > 88 

    png_destroy_read_struct(&image->info.png.png_ptr,
	    &image->info.png.info_ptr, NULL);

#else

    p->free(p, image->info.png.png_ptr);
    p->free(p, image->info.png.info_ptr);

#endif

    return imageslot;
}

#endif	/* !HAVE_LIBPNG */
