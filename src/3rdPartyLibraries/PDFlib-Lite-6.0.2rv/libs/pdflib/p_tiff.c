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

/* $Id: p_tiff.c,v 1.1 2005/10/11 17:18:15 vuduc2 Exp $
 *
 * TIFF processing for PDFlib
 *
 */

#include "p_intern.h"
#include "p_color.h"
#include "p_image.h"

#ifndef HAVE_LIBTIFF

pdc_bool
pdf_is_TIFF_file(PDF *p, pdc_file *fp, pdf_tiff_info *tiff, pdc_false)
{
    (void) p;
    (void) fp;
    (void) tiff;

    return pdc_false;
}

int
pdf_process_TIFF_data(
    PDF *p,
    int imageslot)
{
    (void) imageslot;

    pdc_warning(p->pdc, PDF_E_UNSUPP_IMAGE, "TIFF", 0, 0, 0);
    return -1;
}

#else

#include "tiffiop.h"
static tsize_t
pdf_libtiff_read(void* fd, tdata_t buf, tsize_t size)
{
    pdc_file *fp = (pdc_file *) fd;

    return ((tsize_t) pdc_fread(buf, 1, (size_t) size, fp));
}

static toff_t
pdf_libtiff_seek(void* fd, toff_t off, int whence)
{
    pdc_file *fp = (pdc_file *) fd;

    return ((toff_t) pdc_fseek(fp, (long) off, whence));
}

static int
pdf_libtiff_close(void* fd)
{
    (void) fd;

    /* pdc_fclose(fp); this happens in caller function */

    return 0;
}

static toff_t
pdf_libtiff_size(void* fd)
{
    pdc_file *fp = (pdc_file *) fd;

    return (toff_t) pdc_file_size(fp);
}

static void *
pdf_libtiff_malloc(TIFF *t, tsize_t size)
{
    PDF *p = (PDF*) t->pdflib_opaque;
    return pdc_calloc(p->pdc, (size_t)size, "libtiff");
}

static void *
pdf_libtiff_realloc(TIFF *t, tdata_t mem, tsize_t size)
{
    PDF *p = (PDF*) t->pdflib_opaque;
    return(pdc_realloc(p->pdc, (void*)mem, (size_t)size, "libtiff"));
}

static void
pdf_libtiff_free(TIFF *t, tdata_t mem)
{
    PDF *p = (PDF*) t->pdflib_opaque;
    pdc_free(p->pdc, (void*)mem);
}

static void
pdf_data_source_TIFF_init(PDF *p, PDF_data_source *src)
{
    pdf_image	*image;

    image = (pdf_image *) src->private_data;

    if (image->strips == 1)
	image->info.tiff.cur_line = 0;

    if (image->use_raw) {
	/* malloc is done in the fill function */
	src->buffer_length = (size_t) 0;
	src->buffer_start = (pdc_byte *) NULL;
    } else {
	src->buffer_length = (size_t) (image->components * image->width);
	src->buffer_start = (pdc_byte *)
	    pdc_malloc(p->pdc, src->buffer_length, "pdf_data_source_TIFF_init");
    }
}

/* Convert the a and b samples of Lab data from signed to unsigned. */

static void
pdf_signed_to_unsigned(pdc_byte *buf, size_t count)
{
    size_t i;

    for(i=0; i < count; i+=3)
    {
	buf[i+1] ^= 0x80;
	buf[i+2] ^= 0x80;
    }
}

#define MYTIFF		image->info.tiff.tif

static pdc_bool
pdf_data_source_TIFF_fill(PDF *p, PDF_data_source *src)
{
    pdf_image	*image;
    int		col;
    pdc_byte	*dest;
    uint16	fillorder;
    uint32	*s, *bc;

    image = (pdf_image *) src->private_data;

    if (image->use_raw) {
	if (image->info.tiff.cur_line == image->strips)
	    return pdc_false;

	TIFFGetField(MYTIFF, TIFFTAG_STRIPBYTECOUNTS, &bc);

	if (bc[image->info.tiff.cur_line] > src->buffer_length) {
	    src->buffer_length = bc[image->info.tiff.cur_line];
	    src->buffer_start = (pdc_byte *)
		pdc_realloc(p->pdc, src->buffer_start,
		src->buffer_length, "pdf_data_source_TIFF_fill");
	}

	if (TIFFReadRawStrip(MYTIFF, (tstrip_t) image->info.tiff.cur_line,
			(tdata_t) src->buffer_start,
			(tsize_t) bc[image->info.tiff.cur_line]) == -1) {

	    pdc_free(p->pdc, (void *) src->buffer_start);
	    TIFFClose(MYTIFF);
            image->fp = NULL;
	    pdc_error(p->pdc, PDF_E_IMAGE_CORRUPT, "TIFF", "?", 0, 0);
	}

	src->next_byte = src->buffer_start;
	src->bytes_available = bc[image->info.tiff.cur_line];

	/* special handling for uncompressed 16-bit images */
	if (MYTIFF->tif_header.tiff_magic == TIFF_LITTLEENDIAN &&
            image->compression == pdf_comp_none && image->bpc == 16)
	{
	    TIFFSwabArrayOfShort((uint16 *) src->buffer_start,
		    (unsigned long) src->bytes_available/2);
	}

	if (TIFFGetField(MYTIFF, TIFFTAG_FILLORDER, &fillorder)
	    && (fillorder == FILLORDER_LSB2MSB)) {
	    TIFFReverseBits((unsigned char *) src->buffer_start,
		(unsigned long) src->bytes_available);
	}

	/* The a and b values of (uncompressed) Lab must be adjusted */
	if (p->colorspaces[image->colorspace].type == Lab)
	{
	    pdf_signed_to_unsigned(src->buffer_start, src->bytes_available);
	}

	if (image->strips > 1) {
	    /* only a single strip of a multi-strip image */
	    image->info.tiff.cur_line = image->strips;
	} else
	    image->info.tiff.cur_line++;

    } else {
	if (image->info.tiff.cur_line++ == image->height)
	    return pdc_false;

	src->next_byte = src->buffer_start;
	src->bytes_available = src->buffer_length;

	dest = src->buffer_start;
	s = image->info.tiff.raster +
	    ((int)image->height - image->info.tiff.cur_line) *
	    (int) image->width;

	switch (image->components) {
	  case 1:
	    for (col = 0; col < image->width; col++, s++) {
		*dest++ = (pdc_byte) TIFFGetR(*s);
	    }
	    break;

	  case 3:
	    for (col = 0; col < image->width; col++, s++) {
		*dest++ = (pdc_byte) TIFFGetR(*s);
		*dest++ = (pdc_byte) TIFFGetG(*s);
		*dest++ = (pdc_byte) TIFFGetB(*s);
	    }
	    break;

          case 4:
	    for (col = 0; col < image->width; col++, s++) {
		unsigned char* t = (unsigned char*)&(*s);
		*dest++ = (pdc_byte) t[0];
		*dest++ = (pdc_byte) t[1];
		*dest++ = (pdc_byte) t[2];
		*dest++ = (pdc_byte) t[3];
	    }
	    break;

	  default:
            pdc_error(p->pdc, PDF_E_IMAGE_BADCOMP,
		  pdc_errprintf(p->pdc, "%d", image->components),
		  image->filename, 0, 0);
	}
    }

    return pdc_true;
}

static void
pdf_data_source_TIFF_terminate(PDF *p, PDF_data_source *src)
{
    pdc_free(p->pdc, (void *) src->buffer_start);
}

static int
pdf_check_colormap(int n, uint16* r, uint16* g, uint16* b)
{
    while (n-- > 0)
	if (*r++ >= 256 || *g++ >= 256 || *b++ >= 256)
		return(16);
    return(8);
}

pdc_bool
pdf_is_TIFF_file(PDF *p, pdc_file *fp, pdf_tiff_info *tiff_info, pdc_bool check)
{
    const char *filename;

    /* Suppress all TIFFlib error messages */
    (void) TIFFSetErrorHandler(NULL);

    /* Suppress all TIFFlib warnings unless the client wants it for debugging */
    if (!p->debug[(int) 'T'])
    {
	(void) TIFFSetWarningHandler(NULL);
    }

    filename = pdc_file_name(fp);
    tiff_info->tif = TIFFClientOpen(filename, "rc",
            (void *)fp,
            pdf_libtiff_read, NULL,
            pdf_libtiff_seek, pdf_libtiff_close, pdf_libtiff_size,
            NULL, NULL, (void *)p,
            pdf_libtiff_malloc, pdf_libtiff_realloc, pdf_libtiff_free,
            NULL, NULL);
    if (tiff_info->tif == NULL) {
        pdc_fseek(fp, 0L, SEEK_SET);
        return pdc_false;
    }
    if (check)
        TIFFClose(tiff_info->tif);
    return pdc_true;
}

int
pdf_process_TIFF_data(
    PDF *p,
    int imageslot)
{
    static const char *fn = "pdf_process_TIFF_data";
    uint32 w, h;
    uint16 unit, bpc, compression, photometric, extra, *sinfo;
    uint16 orientation, planarconfig;
    uint16 *rmap, *gmap, *bmap;
    tsample_t components;
    pdf_image *image;
    float res_x, res_y;  /* sic! */
    pdf_colorspace cs;
    int slot;
    int errint = 0;
    int errcode = 0;
    pdc_bool isopen = pdc_false;

    image = &p->images[imageslot];

    if (!pdf_is_TIFF_file(p, image->fp, &image->info.tiff, pdc_false))
    {
        errcode = PDF_E_IMAGE_CORRUPT;
        goto PDF_TIFF_ERROR;
    }

    MYTIFF->tif_fd = (FILE*) image->fp;
    isopen = pdc_true;

    if (image->page != 1) {
        if (TIFFSetDirectory(MYTIFF, (tdir_t) (image->page - 1)) != 1 )
	{
            errint = image->page;
            errcode = PDF_E_IMAGE_NOPAGE;
            goto PDF_TIFF_ERROR;
	}
    }

    TIFFGetFieldDefaulted(MYTIFF, TIFFTAG_ORIENTATION, &orientation);
    image->orientation = orientation;

    TIFFGetField(MYTIFF, TIFFTAG_COMPRESSION, &compression);

    TIFFGetField(MYTIFF, TIFFTAG_IMAGEWIDTH, &w);
    image->width        = (pdc_scalar) w;

    TIFFGetField(MYTIFF, TIFFTAG_IMAGELENGTH, &h);
    image->height       = (pdc_scalar) h;

    TIFFGetFieldDefaulted(MYTIFF, TIFFTAG_BITSPERSAMPLE, &bpc);
    image->bpc		= bpc;

    TIFFGetFieldDefaulted(MYTIFF, TIFFTAG_SAMPLESPERPIXEL, &components);
    image->components	= components;

    TIFFGetFieldDefaulted(MYTIFF, TIFFTAG_EXTRASAMPLES, &extra, &sinfo);

    TIFFGetFieldDefaulted(MYTIFF, TIFFTAG_PLANARCONFIG, &planarconfig);

    photometric = 255;	/* dummy value */
    TIFFGetField(MYTIFF, TIFFTAG_PHOTOMETRIC, &photometric);

    /* fetch the resolution values if found in the file */
    if (TIFFGetField(MYTIFF, TIFFTAG_XRESOLUTION, &res_x) &&
	TIFFGetField(MYTIFF, TIFFTAG_YRESOLUTION, &res_y) &&
	TIFFGetFieldDefaulted(MYTIFF, TIFFTAG_RESOLUTIONUNIT, &unit) &&
	res_x > 0 && res_y > 0) {

	if (unit == RESUNIT_INCH) {
	    image->dpi_x = res_x;
	    image->dpi_y = res_y;

	} else if (unit == RESUNIT_CENTIMETER) {
	    image->dpi_x = res_x * 2.54;
	    image->dpi_y = res_y * 2.54;

	} else if (unit == RESUNIT_NONE) {
	    image->dpi_x = -res_x;
	    image->dpi_y = -res_y;
	}

#define PDF_REALLY_BIG_DPI	10000

	/* Guard against obviously wrong values */
	if (unit != RESUNIT_NONE &&
	    (image->dpi_x <= 1 ||
	    image->dpi_y <= 1 ||
	    image->dpi_x > PDF_REALLY_BIG_DPI ||
	    image->dpi_y > PDF_REALLY_BIG_DPI))

	    image->dpi_x = image->dpi_y = 0;	/* unknown */
    }


    /* libtiff cannot handle JPEG-compressed TIFFs with separate image planes */
    if (planarconfig == PLANARCONFIG_SEPARATE &&
	(compression == COMPRESSION_OJPEG || compression == COMPRESSION_JPEG))
    {
	errcode = PDF_E_TIFF_UNSUPP_JPEG_SEPARATE;
	goto PDF_TIFF_ERROR;
    }

    /* ------------------------------------------------------------
     * We assume pass-through mode in the beginning, and disable it
     * for image types where it doesn't work.
     * ---------------------------------------------------------- */

    image->use_raw = image->passthrough;

    /* Pass-through is not implemented for tiled images */
    if (TIFFIsTiled(MYTIFF))
	image->use_raw = pdc_false;


    /* Can't handle these colorspaces in raw mode */
    if (photometric == PHOTOMETRIC_YCBCR ||
	photometric == PHOTOMETRIC_CIELAB ||
	photometric == PHOTOMETRIC_MASK)
    {
	    image->use_raw = pdc_false;
    }

    /* Can't pass through extra bits or use multiple data sources in raw mode */
    if (extra != 0 ||
       (planarconfig == PLANARCONFIG_SEPARATE && components > 1))
    {
	image->components -= extra;	/* ignore the extra channels */
	image->use_raw = pdc_false;
    }

    /* PDF doesn't support other values of the color depth */
    if (bpc != 1 && bpc != 2 && bpc != 4 && bpc != 8 && bpc != 16)
	image->use_raw = pdc_false;

    /*
     * We can pass through 16-bit data only with PDF 1.5 and big-endian data.
     * For little-endian data, LZW-compressed images work out of the box,
     * uncompressed images require short swapping, while Flate-compressed
     * images don't work either way for some reason.
     *
     * It's not nice to pull the endianness directly from the TIFF structure,
     * but there doesn't seem to be a public interface for it.
     */

    if (image->bpc == 16 &&
	(p->compatibility < PDC_1_5 ||
	(MYTIFF->tif_header.tiff_magic == TIFF_LITTLEENDIAN &&
        image->compression == pdf_comp_flate)))
    {
	image->use_raw = pdc_false;
    }

    /*
     * Disable pass-through for unknown compression schemes,
     * and collect the necessary parameters for well-known schemes.
     */

    if (image->use_raw == pdc_true)
    {
	uint32 group3opts;
	uint16 predictor;

    switch ((int) compression) {
	case COMPRESSION_CCITTRLE:
	case COMPRESSION_CCITTRLEW:
	    image->params = (char *) pdc_malloc(p->pdc, PDF_MAX_PARAMSTRING,
                fn);

	    strcpy(image->params, "/EndOfBlock false");
	    strcat(image->params, "/EncodedByteAlign true");

	    if (photometric == PHOTOMETRIC_MINISBLACK)
		strcat(image->params, "/BlackIs1 true");

            image->compression = pdf_comp_ccitt;
	    break;

	case COMPRESSION_CCITTFAX3:
	    image->params = (char*) pdc_malloc(p->pdc, PDF_MAX_PARAMSTRING, fn);
	    strcpy(image->params, "");

	    /* The following contains three code segments which are
	     * disabled.
	     * Apparently, and contrary to my reading of the specs,
	     * the following can not be deduced from the respective
	     * TIFF entry or option:
	     * - I expected /EndOfBlock and /EndOfLine to be always
	     *   true for CCITTFAX3 images.
	     *
	     * - /EncodedByteAlign can not reliably be deduced from
	     *   GROUP3OPT_FILLBITS;
	     *
	     * - /BlackIs1 can not reliably be deduced from
	     *   PHOTOMETRIC_MINISBLACK;
	     *
	     * From practical experience, the respective lines are
	     * disabled, but I don't have any clear explanation for this.
	     * A few TIFF images still don't work with this setting,
	     * unfortunately.
	     */

	    /* SEE ABOVE!
	    strcpy(image->params, "/EndOfBlock false");
	    strcat(image->params, "/EndOfLine true");
	    */
	    /*
	    strcat(image->params, "/DamagedRowsBeforeError 1");
	    */

	    if (TIFFGetField(MYTIFF, TIFFTAG_GROUP3OPTIONS, &group3opts)) {
		/* /K = 0 (= G3,1D) is default */
		if (group3opts & GROUP3OPT_2DENCODING)
		    strcat(image->params, "/K 1");

		/* SEE ABOVE!
		if (group3opts & GROUP3OPT_FILLBITS)
		    strcat(image->params, "/EncodedByteAlign true");
		*/
	    }

	    /* SEE ABOVE!
	    if ((photometric == PHOTOMETRIC_MINISBLACK))
		strcat(image->params, "/BlackIs1 true");
	    */

            image->compression = pdf_comp_ccitt;
	    break;

	case COMPRESSION_CCITTFAX4:
	    image->params = (char*) pdc_malloc(p->pdc, PDF_MAX_PARAMSTRING, fn);

	    strcpy(image->params, "/K -1");

	    if (photometric == PHOTOMETRIC_MINISBLACK)
		strcat(image->params, "/BlackIs1 true");

            image->compression = pdf_comp_ccitt;
	    break;

	case COMPRESSION_NONE:
	    if (photometric == PHOTOMETRIC_MINISWHITE)
		image->invert = !image->invert;

            image->compression = pdf_comp_none;
	    break;

	case COMPRESSION_LZW:
	    if (TIFFGetField(MYTIFF, TIFFTAG_PREDICTOR, &predictor)) {
		if (predictor != pred_default && predictor != pred_tiff) {
		    image->use_raw = pdc_false;
		    break;
		} else
		    image->predictor = (pdf_predictor) predictor;
	    }

	    if (photometric == PHOTOMETRIC_MINISWHITE)
		image->invert = !image->invert;

            image->compression = pdf_comp_lzw;
	    break;

	case COMPRESSION_PACKBITS:
	    if (photometric == PHOTOMETRIC_MINISWHITE)
		image->invert = !image->invert;

            image->compression = pdf_comp_runlength;
	    break;

	case COMPRESSION_DEFLATE:
	case COMPRESSION_ADOBE_DEFLATE:
	    if (TIFFGetField(MYTIFF, TIFFTAG_PREDICTOR, &predictor)) {
		if (predictor != pred_default && predictor != pred_tiff) {
		    image->use_raw = pdc_false;
		    break;
		} else
		    image->predictor = (pdf_predictor) predictor;
	    }

            image->compression = pdf_comp_flate;
	    break;

	default:
	    image->use_raw = pdc_false;
    }
    }

    if (image->use_raw)
    {
	/* pass-through mode: directly copy chunks of strip data */
	image->strips = (int) TIFFNumberOfStrips(MYTIFF);
    }
    else
    {
	/* Fallback: use TIFFlib to retrieve pixel data */
	image->bpc = 8;
	image->strips = 1;
        image->compression = pdf_comp_none;

	/* palette images are automatically converted to RGB by TIFFlib */
	if (image->components == 1 &&
	    TIFFGetField(MYTIFF, TIFFTAG_COLORMAP, &rmap, &gmap, &bmap))
	{
	    image->components = 3;
	}
    }

    if (image->imagemask)
    {
	if (image->components != 1) {
	    errcode = PDF_E_IMAGE_BADMASK;
	    goto PDF_TIFF_ERROR;
	}

	if (p->compatibility == PDC_1_3) {
	    if (image->components != 1 || image->bpc != 1) {
		errcode = PDF_E_IMAGE_MASK1BIT13;
		goto PDF_TIFF_ERROR;
	    }
	} else if (image->bpc > 1) {
	    /* images with more than one bit will be written as /SMask,
	     * and don't require an /ImageMask entry.
	     */
	    image->imagemask = pdc_false;
	}
    }

    if (image->mask != pdc_undef)
    {
        if (image->strips != 1) {
            errcode = PDF_E_TIFF_MASK_MULTISTRIP;
            goto PDF_TIFF_ERROR;
        }
    }

    if (image->colorspace == pdc_undef)
    {
	uint16 inkset;

        switch (image->components) {
            case 1:
                image->colorspace = DeviceGray;
                break;

            case 3:
		image->colorspace = DeviceRGB;
                break;

            case 4:
                if (photometric == PHOTOMETRIC_SEPARATED) {

		    /* Can't handle CMYK with mask */
		    if (extra != 0)
		    {
			errint = image->components;
			errcode = PDF_E_TIFF_CMYK_MASK;
			goto PDF_TIFF_ERROR;
		    }

                    TIFFGetFieldDefaulted(MYTIFF, TIFFTAG_INKSET, &inkset);
                    if (inkset != INKSET_CMYK) {
                        errint = inkset;
                        errcode = PDF_E_TIFF_UNSUPP_SEP_NONCMYK;
                        goto PDF_TIFF_ERROR;
                    }
                    image->colorspace = DeviceCMYK;
                } else {
                    /* if it's not separated it must be RGB with alpha */
                    image->components = 3;
                    image->colorspace = DeviceRGB;
                    image->compression = pdf_comp_none;
                }
                break;

            default:
                errint = image->components;
                errcode = PDF_E_IMAGE_BADCOMP;
                goto PDF_TIFF_ERROR;
        }
    }


    image->src.private_data	= (void *) image;
    image->src.init		= pdf_data_source_TIFF_init;
    image->src.fill		= pdf_data_source_TIFF_fill;
    image->src.terminate	= pdf_data_source_TIFF_terminate;

    if (image->use_raw) {
	uint32 row, rowsperstrip;
	int strip;

	/* must handle colormap ourselves */
	if (photometric == PHOTOMETRIC_PALETTE) {
	    int i;
	    pdf_colormap colormap;

	    if (!TIFFGetField(MYTIFF, TIFFTAG_COLORMAP, &rmap, &gmap, &bmap)) {
                errcode = PDF_E_IMAGE_COLORMAP;
                goto PDF_TIFF_ERROR;
	    }

	    cs.type = Indexed;
	    cs.val.indexed.palette_size = 1 << bpc;
	    cs.val.indexed.colormap = &colormap;
	    cs.val.indexed.colormap_id = PDC_BAD_ID;

	    cs.val.indexed.base = DeviceRGB;

#define CVT(x) (uint16) (((x) * 255) / ((1L<<16)-1))
	    /* TODO: properly deal with 16-bit palette entries in PDF 1.5 */
	    if (pdf_check_colormap(cs.val.indexed.palette_size,
		rmap, gmap, bmap) == 16)
	    {
                /* convert colormap to 8 bit values  */
		for (i = 0; i < cs.val.indexed.palette_size; i++) {
		    rmap[i] = CVT(rmap[i]);
		    gmap[i] = CVT(gmap[i]);
		    bmap[i] = CVT(bmap[i]);
		}
	    }
#undef CVT

	    for (i = 0; i < cs.val.indexed.palette_size; i++) {
		colormap[i][0] = (pdc_byte) rmap[i];
		colormap[i][1] = (pdc_byte) gmap[i];
		colormap[i][2] = (pdc_byte) bmap[i];
	    }

	    image->components = 1;

		slot = pdf_add_colorspace(p, &cs, pdc_false);
		image->colorspace = (pdf_colorspacetype) slot;


	}


	if (image->strips > image->height)
	    image->strips = (int) image->height;

	if (TIFFGetFieldDefaulted(MYTIFF,
	    TIFFTAG_ROWSPERSTRIP, &rowsperstrip) == 1 && (int)rowsperstrip!= -1)
	    image->rowsperstrip = (int) rowsperstrip;
	else
	    image->rowsperstrip = (int) image->height;

	/*
	 * The first strip must be handled separately because it carries the
	 * colormap for indexed images. Other strips reuse this colormap.
	 */
	image->info.tiff.cur_line = 0;
        image->height = (pdc_scalar)
	    (image->rowsperstrip > (int) h ? (int) h : image->rowsperstrip);

	/*
	 * Images may also be written to the output before the first page
	 * We do this ourselves (instead of in pdf_put_image() to avoid
	 * many empty contents sections for multi-strip images.
	 */
	if (PDF_GET_STATE(p) == pdf_state_page)
	    pdf_end_contents_section(p);

	pdf_put_image(p, imageslot, pdc_true, pdc_false);

	for (row = (uint32) image->rowsperstrip, strip = 1;
		row < h; row += (uint32) image->rowsperstrip, strip++) {

            image->height = (pdc_scalar) (row+image->rowsperstrip > h ?
                                     (int) (h - row) : image->rowsperstrip);

	    /*
	     * tell pdf_data_source_TIFF_fill() to read only data of the
	     * current strip
	     */
	    image->info.tiff.cur_line = strip;
	    pdf_put_image(p, imageslot, pdc_false, pdc_false);
	}

        image->height = (pdc_scalar) h;
	image->no -= (image->strips - 1);	/* number of first strip */

	/* Special handling for multi-strip images (see comment above) */
	if (PDF_GET_STATE(p) == pdf_state_page)
	    pdf_begin_contents_section(p);

    } else {	/* !use_raw */
	size_t npixels;


	image->info.tiff.raster = (uint32 *) NULL;


	/*
	 * Retrieve full scan lines from TIFFlib for these color spaces,
	 * and Gray, RGB, or CMYK pixel data otherwise.
	 */
	if (p->colorspaces[image->colorspace].type == DeviceCMYK ||
	    (p->colorspaces[image->colorspace].type == ICCBased &&
		image->components == 4))
	{
	    image->pixelmode = pdc_false;
	} else
	{
	    image->pixelmode = pdc_true;
	}

	if (image->pixelmode)
	{
	    npixels = (size_t) (w * h);

	    image->info.tiff.raster = (uint32 *) pdc_malloc(p->pdc,
		(size_t) (npixels * sizeof (uint32)), "pdf_open_TIFF");

	    if (!TIFFReadRGBAImageOriented(MYTIFF,
		    w, h, image->info.tiff.raster, orientation, 1)) {
		pdc_free(p->pdc, (void *) image->info.tiff.raster);
                errcode = PDC_E_IO_READ;
                goto PDF_TIFF_ERROR;
	    }
	} else {
	    int linecounter = 0;

	    npixels = (size_t) (TIFFScanlineSize(MYTIFF) * h);
	    image->info.tiff.raster = (uint32 *)
		pdc_malloc(p->pdc, (size_t) npixels, "pdf_open_TIFF");

	    while (linecounter < image->height) {
		if (TIFFReadScanline(MYTIFF,
		    (tdata_t) (image->info.tiff.raster +
		    ((int)image->height - linecounter - 1) * (int)image->width),
		    (uint32) linecounter, (tsample_t) 0) == -1) {

		    pdc_free(p->pdc, (void *) image->info.tiff.raster);
                    errcode = PDC_E_IO_READ;
                    goto PDF_TIFF_ERROR;
		}
		linecounter++;
	    }
	}

	pdf_put_image(p, imageslot, pdc_true, pdc_true);

	if (image->info.tiff.raster)
	    pdc_free(p->pdc, (void *) image->info.tiff.raster);
    }

    image->in_use = pdc_true;			/* mark slot as used */

    TIFFClose(MYTIFF);

    return imageslot;

    PDF_TIFF_ERROR:
    if (isopen)
        TIFFClose(MYTIFF);
    {
        const char *stemp =
            pdc_errprintf(p->pdc, "%.*s", PDC_ET_MAXSTRLEN,  image->filename);
        switch (errcode)
        {
            case PDC_E_IO_READ:
            case PDF_E_IMAGE_ICC:
            case PDF_E_IMAGE_ICC2:
            case PDF_E_IMAGE_MASK1BIT13:
            case PDF_E_IMAGE_COLORIZE:
            case PDF_E_TIFF_MASK_MULTISTRIP:
            case PDF_E_IMAGE_COLORMAP:
            case PDF_E_IMAGE_BADMASK:
            case PDF_E_TIFF_CMYK_MASK:
            case PDF_E_TIFF_UNSUPP_JPEG_SEPARATE:
		pdc_set_errmsg(p->pdc, errcode, stemp, 0, 0, 0);
		break;

            case PDF_E_IMAGE_CORRUPT:
		pdc_set_errmsg(p->pdc, errcode, "TIFF", stemp, 0, 0);
		break;

            case PDF_E_IMAGE_BADCOMP:
		pdc_set_errmsg(p->pdc, errcode,
		    pdc_errprintf(p->pdc, "%d", errint), stemp, 0, 0);
		break;

            case PDF_E_IMAGE_NOPAGE:
		pdc_set_errmsg(p->pdc, errcode,
		    pdc_errprintf(p->pdc, "%d", errint), "TIFF", stemp, 0);
		break;

            case PDF_E_TIFF_UNSUPP_SEP_NONCMYK:
		pdc_set_errmsg(p->pdc, errcode,
		    stemp, pdc_errprintf(p->pdc, "%d", errint), 0, 0);
		break;

	    case 0: 		/* error code and message already set */
		break;
        }
    }

    if (image->verbose)
	pdc_error(p->pdc, -1, 0, 0, 0, 0);

    return -1;
}

#undef MYTIFF
#endif	/* HAVE_LIBTIFF */
