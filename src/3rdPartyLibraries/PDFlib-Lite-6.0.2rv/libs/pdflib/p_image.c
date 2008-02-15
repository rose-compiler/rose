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

/* $Id: p_image.c,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * PDFlib image routines
 *
 */

#define P_IMAGE_C

#include "p_intern.h"
#include "p_color.h"
#include "p_defopt.h"
#include "p_font.h"
#include "p_image.h"
#include "p_layer.h"
#include "p_tagged.h"

static void
pdf_init_image_struct(PDF *p, pdf_image *image)
{
    (void) p;

    /********** option variables *************/
    image->verbose      = pdc_true;
    image->bitreverse   = pdc_false;
    image->bpc          = pdc_undef;
    image->components   = pdc_undef;
    image->height_pixel = pdc_undef;
    image->ignoremask   = pdc_false;
    image->ignoreorient = pdc_false;
    image->doinline     = pdc_false;
    image->interpolate  = pdc_false;
    image->invert       = pdc_false;
    image->jpegoptimize = pdc_true;
    image->passthrough  = !p->debug[(int) 'P'];
    image->K            = 0;
    image->imagemask    = pdc_false;
    image->mask         = pdc_undef;
    image->ri           = AutoIntent;
    image->page         = 1;
    image->reference    = pdf_ref_direct;
    image->width_pixel  = pdc_undef;
    image->iconname     = (char *) NULL;
    /*****************************************/

    image->orientation  = 1;
    image->transparent  = pdc_false;
    image->compression  = pdf_comp_none;
    image->predictor	= pred_default;
    image->in_use	= pdc_false;
    image->fp           = (pdc_file *) NULL;
    image->filename	= (char *) NULL;
    image->params	= (char *) NULL;
    image->dpi_x	= 0;
    image->dpi_y	= 0;
    image->strips	= 1;
    image->rowsperstrip	= 1;
    image->colorspace   = pdc_undef;
    image->dochandle    = pdc_undef;        /* this means "not a PDI page" */
    image->use_raw	= pdc_false;
    image->pixelmode	= pdc_undef;
    image->type		= pdf_img_auto;
    image->transval[0]	= 0;
    image->transval[1]	= 0;
    image->transval[2]	= 0;
    image->transval[3]	= 0;
}

void
pdf_init_images(PDF *p)
{
    int im;

    p->images_capacity = IMAGES_CHUNKSIZE;

    p->images = (pdf_image *)
    	pdc_malloc(p->pdc,
	    sizeof(pdf_image) * p->images_capacity, "pdf_init_images");

    for (im = 0; im < p->images_capacity; im++)
	pdf_init_image_struct(p, &(p->images[im]));
}

void
pdf_grow_images(PDF *p)
{
    int im;

    p->images = (pdf_image *) pdc_realloc(p->pdc, p->images,
	sizeof(pdf_image) * 2 * p->images_capacity, "pdf_grow_images");

    for (im = p->images_capacity; im < 2 * p->images_capacity; im++)
	pdf_init_image_struct(p, &(p->images[im]));

    p->images_capacity *= 2;
}

void
pdf_cleanup_image(PDF *p, int im)
{
    pdf_image *image = &p->images[im];

    /* clean up parameter string if necessary */
    if (image->params) {
        pdc_free(p->pdc, image->params);
        image->params = NULL;
    }

    if (image->filename) {
        pdc_free(p->pdc, image->filename);
        image->filename = NULL;
    }

    if (image->fp) {
        pdc_fclose(image->fp);
        image->fp = NULL;
    }

    if (image->iconname) {
        pdc_free(p->pdc, image->iconname);
        image->iconname = NULL;
    }


    /* type-specific cleanups */
    if (image->type == pdf_img_gif)
	pdf_cleanup_gif(p, image);
    if (image->type == pdf_img_jpeg)
        pdf_cleanup_jpeg(p, image);

    /* free the image slot and prepare for next use */
    pdf_init_image_struct(p, image);
}

void
pdf_cleanup_images(PDF *p)
{
    int im;

    if (!p->images)
	return;

    /* Free images which the caller left open */

    /* When we think of inter-document survival of images,
    ** we MUST NOT FORGET that the current TIFF algorithm
    ** depends on contiguous image slots for the image strips!
    */
    for (im = 0; im < p->images_capacity; im++)
    {
        if (p->images[im].in_use)              /* found used slot */
	    pdf_cleanup_image(p, im);	/* free image descriptor */
    }

    pdc_free(p->pdc, p->images);
    p->images = NULL;
}

void
pdf_init_xobjects(PDF *p)
{
    int idx;

    p->xobjects_number = 0;

    if (p->xobjects == (pdf_xobject *) 0)
    {
	p->xobjects_capacity = XOBJECTS_CHUNKSIZE;

	p->xobjects = (pdf_xobject *)
	    pdc_malloc(p->pdc, sizeof(pdf_xobject) * p->xobjects_capacity,
	    "pdf_init_xobjects");
    }

    for (idx = 0; idx < p->xobjects_capacity; idx++)
	p->xobjects[idx].flags = 0;
}

int
pdf_new_xobject(PDF *p, pdf_xobj_type type, pdc_id obj_id)
{
    static const char fn[] = "pdf_new_xobject";
    int i, slot = p->xobjects_number++;

    if (slot == p->xobjects_capacity)
    {
	p->xobjects = (pdf_xobject *) pdc_realloc(p->pdc, p->xobjects,
	    sizeof(pdf_xobject) * 2 * p->xobjects_capacity, fn);

	for (i = p->xobjects_capacity; i < 2 * p->xobjects_capacity; i++)
	    p->xobjects[i].flags = 0;

	p->xobjects_capacity *= 2;
    }

    if (obj_id == PDC_NEW_ID)
	obj_id = pdc_begin_obj(p->out, PDC_NEW_ID);

    p->xobjects[slot].obj_id = obj_id;
    p->xobjects[slot].type = type;
    p->xobjects[slot].flags = xobj_flag_used;

    return slot;
}

pdc_id
pdf_get_xobject(PDF *p, int im)
{
    if (im >= 0 && im < p->images_capacity)
    {
        pdf_image *img = &p->images[im];

        if (img->in_use)
            return p->xobjects[img->no].obj_id;
    }
    return PDC_BAD_ID;
}

void
pdf_write_xobjects(PDF *p)
{
    if (p->xobjects_number > 0)
    {
	pdc_bool hit = pdc_false;
	int i;

	for (i = 0; i < p->xobjects_number; ++i)
	{
	    if (p->xobjects[i].flags & xobj_flag_write)
	    {
		if (!hit)
		{
		    pdc_puts(p->out, "/XObject");
		    pdc_begin_dict(p->out);
		    hit = pdc_true;
		}

		pdc_printf(p->out, "/I%d", i);
		pdc_objref(p->out, "", p->xobjects[i].obj_id);
		p->xobjects[i].flags &= ~xobj_flag_write;
	    }
	}

	if (hit)
	    pdc_end_dict(p->out);
    }
}

void
pdf_get_page_xobjects(PDF *p, pdf_reslist *rl)
{
    int i;

    for (i = 0; i < p->xobjects_number; i++) {
	if (p->xobjects[i].flags & xobj_flag_write) {
	    p->xobjects[i].flags &= ~xobj_flag_write;
	    pdf_add_reslist(p, rl, i);
	}
    }
}

void
pdf_mark_page_xobject(PDF *p, int n)
{
    p->xobjects[n].flags |= xobj_flag_write;
}

void
pdf_cleanup_xobjects(PDF *p)
{
    if (p->xobjects) {
	pdc_free(p->pdc, p->xobjects);
	p->xobjects = NULL;
    }
}

void
pdf_put_inline_image(PDF *p, int im)
{
    pdf_image	*image;
    pdc_matrix m;
    PDF_data_source *src;
    int		i;

    image = &p->images[im];

    /* Image object */

    image->no = -1;

    pdf__save(p);

    pdc_scale_matrix(image->width, image->height, &m);

    pdf_concat_raw(p, &m);

    pdc_puts(p->out, "BI");

    pdc_printf(p->out, "/W %d", (int) image->width);
    pdc_printf(p->out, "/H %d", (int) image->height);

    if (image->imagemask == pdc_true) {
	pdc_puts(p->out, "/IM true");

    } else if (image->colorspace != pdc_undef) {

	pdc_printf(p->out, "/BPC %d", image->bpc);

	switch (p->colorspaces[image->colorspace].type) {
	    case DeviceGray:
		pdc_printf(p->out, "/CS/G");
		break;

	    case DeviceRGB:
		pdc_printf(p->out, "/CS/RGB");
		break;

	    case DeviceCMYK:
		pdc_printf(p->out, "/CS/CMYK");
		break;

	    default:
		pdc_error(p->pdc, PDF_E_INT_BADCS,
		    pdc_errprintf(p->pdc, "%d", image->colorspace), 0, 0, 0);
		break;
	}
    }

    if (image->compression != pdf_comp_none) {
	pdc_printf(p->out, "/F/%s",
            pdc_get_keyword(image->compression, pdf_shortfilter_pdfkeylist));
    }

    /* prepare precompressed (raw) image data */
    if (image->use_raw &&
        (image->params ||
	 image->predictor != pred_default ||
         image->compression == pdf_comp_ccitt)) {

	pdc_printf(p->out, "/DP[<<");

        /* write EarlyChange */
        if (image->params)
            pdc_puts(p->out, image->params);

        if (image->compression == pdf_comp_ccitt) {
            if (image->K != 0)
                pdc_printf(p->out, "/K %d", image->K);
        }

        if (image->compression == pdf_comp_flate ||
            image->compression == pdf_comp_lzw) {
	    if (image->predictor != pred_default) {
		pdc_printf(p->out, "/Predictor %d", (int) image->predictor);
		pdc_printf(p->out, "/Columns %d", (int) image->width);
		if (image->bpc != 8)
		    pdc_printf(p->out, "/BitsPerComponent %d", image->bpc);

		if (image->components != 1)	/* 1 is default */
		    pdc_printf(p->out, "/Colors %d", image->components);
	    }
	}

        if (image->compression == pdf_comp_ccitt) {
	    if ((int) image->width != 1728)	/* CCITT default width */
		pdc_printf(p->out, "/Columns %d", (int) image->width);

	    pdc_printf(p->out, "/Rows %d", (int) fabs(image->height));
	}
	pdc_puts(p->out, ">>]");		/* DecodeParms dict and array */
    }

    if (image->ri != AutoIntent) {
        pdc_printf(p->out, "/Intent/%s",
            pdc_get_keyword(image->ri, pdf_renderingintent_pdfkeylist));
    }

    if (image->interpolate) {
        pdc_puts(p->out, "/I true");
    }

    if (image->invert) {
	pdc_puts(p->out, "/D[1 0");
	for (i = 1; i < image->components; i++)
	    pdc_puts(p->out, " 1 0");
	pdc_puts(p->out, "]ID\n");

    } else {
	pdc_puts(p->out, " ID\n");
    }

    /* Write the actual image data to the content stream */

    src = &image->src;

    /* We can't use pdf_copy_stream() here because it automatically
     * generates a stream object, which is not correct for inline
     * image data.
     */
    if (src->init)
	src->init(p, src);

    while (src->fill(p, src))
	pdc_write(p->out, src->next_byte, src->bytes_available);

    if (src->terminate)
	src->terminate(p, src);

    pdc_puts(p->out, "EI\n");

    pdf__restore(p);

    /* Do the equivalent of PDF_close_image() since the image handle
     * cannot be re-used anyway.
     */
    pdf_cleanup_image(p, im);
}

void
pdf_put_image(PDF *p, int im, pdc_bool firststrip, pdc_bool checkcontentstream)
{
    pdc_id	length_id;
    pdf_image	*image;
    int		i;
    pdf_compression	compression;

    image = &p->images[im];

    /* Images may also be written to the output before the first page */
    if (checkcontentstream && PDF_GET_STATE(p) == pdf_state_page)
	pdf_end_contents_section(p);


    /* Image object */

    image->no = pdf_new_xobject(p, image_xobject, PDC_NEW_ID);

    pdc_begin_dict(p->out); 		/* XObject */

    pdc_puts(p->out, "/Subtype/Image\n");

    pdc_printf(p->out, "/Width %d\n", (int) image->width);
    pdc_printf(p->out, "/Height %d\n", (int) fabs(image->height));

    /*
     * Transparency handling
     */

    /* Masking by color: single transparent color value */
    if (image->transparent && image->colorspace != pdc_undef) {
	pdf_colorspace *cs = &p->colorspaces[image->colorspace];

	switch (cs->type) {
	    case Indexed:
	    case DeviceGray:
	    pdc_printf(p->out,"/Mask[%d %d]\n",
		(int) image->transval[0], (int) image->transval[0]);
	    break;


	    case DeviceRGB:
	    pdc_printf(p->out,"/Mask[%d %d %d %d %d %d]\n",
		(int) image->transval[0], (int) image->transval[0],
		(int) image->transval[1], (int) image->transval[1],
		(int) image->transval[2], (int) image->transval[2]);
	    break;

	    case DeviceCMYK:
	    pdc_printf(p->out,"/Mask[%d %d %d %d %d %d %d %d]\n",
		(int) image->transval[0], (int) image->transval[0],
		(int) image->transval[1], (int) image->transval[1],
		(int) image->transval[2], (int) image->transval[2],
		(int) image->transval[3], (int) image->transval[3]);
	    break;

	    default:
	    pdc_error(p->pdc, PDF_E_INT_BADCS,
		pdc_errprintf(p->pdc, "%d",
		    (int) p->colorspaces[image->colorspace].type), 0, 0, 0);
	}

    /* Masking by position: separate bitmap mask */
    } else if (image->mask != pdc_undef && p->images[image->mask].bpc > 1) {
        pdc_objref(p->out, "/SMask",
            p->xobjects[p->images[image->mask].no].obj_id);

    } else if (image->mask != pdc_undef) {
        pdc_objref(p->out, "/Mask",
            p->xobjects[p->images[image->mask].no].obj_id);
    }

    /*
     * /BitsPerComponent is optional for image masks according to the
     * PDF reference, but some viewers require it nevertheless.
     * We must therefore always write it.
     */
    if (image->type != pdf_img_jpeg2000)
        pdc_printf(p->out, "/BitsPerComponent %d\n", image->bpc);

    if (image->imagemask) {
	pdc_puts(p->out, "/ImageMask true\n");
        if (image->type == pdf_img_jpeg2000)
            pdc_puts(p->out, "/SMaskInData 1\n");

    } else if (image->colorspace != pdc_undef) {

	switch (p->colorspaces[image->colorspace].type) {
	    case DeviceGray:
		break;

	    case DeviceRGB:
		break;

	    case DeviceCMYK:
		break;

	    case Indexed:
		break;



	    default:
		pdc_error(p->pdc, PDF_E_INT_BADCS,
		    pdc_errprintf(p->pdc, "%d", image->colorspace), 0, 0, 0);
        }

        pdc_puts(p->out, "/ColorSpace");
        pdf_write_colorspace(p, image->colorspace, pdc_false);
        pdc_puts(p->out, "\n");
    }

    if (image->invert) {
        pdc_puts(p->out, "/Decode[1 0");
        for (i = 1; i < image->components; i++)
            pdc_puts(p->out, " 1 0");
	pdc_end_array(p->out);
    }

    if (image->ri != AutoIntent) {
        pdc_printf(p->out, "/Intent/%s\n",
            pdc_get_keyword(image->ri, pdf_renderingintent_pdfkeylist));
    }

    if (image->interpolate) {
        pdc_puts(p->out, "/Interpolate true\n");
    }

    /* special case: referenced image data instead of direct data */
    if (image->reference != pdf_ref_direct) {

        if (image->compression != pdf_comp_none) {
	    pdc_printf(p->out, "/FFilter[/%s]\n",
                pdc_get_keyword(image->compression, pdf_filter_pdfkeylist));
	}

        if (image->compression == pdf_comp_ccitt) {
	    pdc_puts(p->out, "/FDecodeParms[<<");

	    if ((int) image->width != 1728)	/* CCITT default width */
		pdc_printf(p->out, "/Columns %d", (int) image->width);

	    pdc_printf(p->out, "/Rows %d", (int) fabs(image->height));

            if (image->K != 0)
                pdc_printf(p->out, "/K %d", image->K);

	    pdc_puts(p->out, ">>]\n");

	}

	if (image->reference == pdf_ref_file) {

	    /* LATER: make image file name platform-neutral:
	     * Change : to / on the Mac
	     * Change \ to / on Windows
	     */
	    pdc_puts(p->out, "/F");
	    pdc_put_pdfstring(p->out, image->filename, 0);
            pdc_puts(p->out, "/Length 0");

	} else if (image->reference == pdf_ref_url) {

	    pdc_puts(p->out, "/F<</FS/URL/F");
	    pdc_put_pdfstring(p->out, image->filename, 0);
	    pdc_puts(p->out, ">>/Length 0");
	}

	pdc_end_dict(p->out);		/* XObject */

	/* We must avoid pdc_begin/end_pdfstream() here in order to
	 * generate a really empty stream.
	 */
	pdc_puts(p->out, "stream\n");	/* dummy image stream */
	pdc_puts(p->out, "endstream\n");

        pdc_end_obj(p->out);                    /* XObject */

	if (PDF_GET_STATE(p) == pdf_state_page)
	    pdf_begin_contents_section(p);

	return;
    }

    compression = image->compression;

    /*
     * Now the (more common) handling of actual image
     * data to be included in the PDF output.
     */

    /* force compression if not a recognized precompressed image format */
    if ((!image->use_raw || compression == pdf_comp_none) &&
	pdc_get_compresslevel(p->out))
        compression = pdf_comp_flate;

    if (compression != pdf_comp_none)
	pdc_printf(p->out, "/Filter/%s\n",
                   pdc_get_keyword(compression, pdf_filter_pdfkeylist));

    /* prepare precompressed (raw) image data; avoid empty DecodeParms */
    if (image->use_raw &&
        (image->params ||
	 image->predictor != pred_default ||
         compression == pdf_comp_ccitt)) {

	pdc_printf(p->out, "/DecodeParms<<");

        /* write EarlyChange */
        if (image->params)
            pdc_puts(p->out, image->params);

        if (compression == pdf_comp_ccitt) {
            if (image->K != 0)
                pdc_printf(p->out, "/K %d", image->K);
        }

        if (compression == pdf_comp_flate || compression == pdf_comp_lzw) {
	    if (image->predictor != pred_default) {
		pdc_printf(p->out, "/Predictor %d", (int) image->predictor);
		pdc_printf(p->out, "/Columns %d", (int) image->width);
		if (image->bpc != 8)
		    pdc_printf(p->out, "/BitsPerComponent %d", image->bpc);

		if (image->components != 1)	/* 1 is default */
		    pdc_printf(p->out, "/Colors %d", image->components);
	    }
	}

        if (compression == pdf_comp_ccitt) {
	    if ((int) image->width != 1728)	/* CCITT default width */
		pdc_printf(p->out, "/Columns %d", (int) image->width);

	    pdc_printf(p->out, "/Rows %d", (int) fabs(image->height));
	}

	pdc_puts(p->out, ">>\n");		/* DecodeParms dict */
    }


    /* Write the actual image data */
    length_id = pdc_alloc_id(p->out);

    pdc_objref(p->out, "/Length", length_id);
    pdc_end_dict(p->out);		/* XObject */

    /* image data */

    /*
     * We must check "image->compression" here since this describes the
     * actual status of the input data, as opposed to "compression"
     * which describes the desired status of the output data.
     */

    pdf_copy_stream(p, &image->src,
            !image->use_raw || image->compression == pdf_comp_none);

    pdc_end_obj(p->out);	/* XObject */

    pdc_put_pdfstreamlength(p->out, length_id);

    if (p->flush & pdf_flush_content)
	pdc_flush_stream(p->out);

    /*
     * Write colormap information for indexed color spaces
     */
    if (firststrip && image->colorspace != pdc_undef &&
        p->colorspaces[image->colorspace].type == Indexed) {
	pdf_write_colormap(p, image->colorspace);
    }

    if (checkcontentstream && PDF_GET_STATE(p) == pdf_state_page)
	pdf_begin_contents_section(p);

    if (p->flush & pdf_flush_content)
	pdc_flush_stream(p->out);
}

void
pdf__fit_image(PDF *p, int im, pdc_scalar x, pdc_scalar y, const char *optlist)
{
    pdf_image *image;
    int legal_states;

    pdf_check_handle(p, im, pdc_imagehandle);

    image = &p->images[im];

    if (PDF_GET_STATE(p) == pdf_state_glyph && !pdf_get_t3colorized(p) &&
        image->imagemask == pdc_false)
        legal_states = pdf_state_page | pdf_state_pattern | pdf_state_template;
    else
        legal_states = pdf_state_content;
    PDF_CHECK_STATE(p, legal_states);

    if (PDF_GET_STATE(p) == pdf_state_template && im == p->templ)
        pdc_error(p->pdc, PDF_E_TEMPLATE_SELF,
            pdc_errprintf(p->pdc, "%d", im), 0, 0, 0);

    pdf_place_xobject(p, im, x, y, optlist);
}

void
pdf_init_xobject_options(PDF *p, pdf_xobject_options *xo)
{
    xo->adjustpage = pdc_false;
    xo->blind = pdc_false;
    xo->filename = NULL;
    xo->flags = 0;
    xo->imagewarning = p->debug[(int) 'i'];
    xo->ignoreorientation = pdc_false;
    xo->im = -1;
    xo->mask = 0;
    xo->dpi[0] = 0;
    xo->dpi[1] = 0;
    xo->page = 1;
    xo->scale[0] = 1;
    xo->scale[1] = 1;
}

void
pdf_get_xobject_options(PDF *p, pdf_xobject_options *xo, pdc_resopt *resopts)
{
    int inum;

    (void) p;

    if (!(xo->flags & is_block))
    {
        pdc_get_optvalues("adjustpage", resopts, &xo->adjustpage, NULL);

        pdc_get_optvalues("blind", resopts, &xo->blind, NULL);
    }

    if (xo->flags & is_image)
    {
        if (pdc_get_optvalues("ignoreorientation", resopts,
						&xo->ignoreorientation, NULL))
            xo->mask |= (1L << xo_ignoreorientation);

        inum = pdc_get_optvalues("dpi", resopts, xo->dpi, NULL);
        if (inum)
        {
            if (inum == 1)
                xo->dpi[1] = xo->dpi[0];
            xo->mask |= (1L << xo_dpi);
        }
    }

    if (xo->flags & is_block)
    {
        if (pdc_get_optvalues("imagewarning", resopts, &xo->imagewarning, NULL))
            xo->mask |= (1L << xo_imagewarning);
    }

    inum = pdc_get_optvalues("scale", resopts, xo->scale, NULL);
    if (inum)
    {
        if (inum == 1)
            xo->scale[1] = xo->scale[0];
        xo->mask |= (1L << xo_scale);
    }
}

/* definitions of fit xobject options */
static const pdc_defopt pdf_fit_xobject_options[] =
{
    PDF_XOBJECT_OPTIONS1
    PDF_XOBJECT_OPTIONS2
    PDF_XOBJECT_OPTIONS3
    PDF_FIT_OPTIONS1
    PDF_FIT_OPTIONS2
    PDC_OPT_TERMINATE
};

void
pdf_place_xobject(PDF *p, int im, pdc_scalar x, pdc_scalar y,
                  const char *optlist)
{
    pdf_xobject_options xo;
    pdf_fit_options fit;
    pdf_image *image = &p->images[im];

    /* initialize */
    pdf_init_xobject_options(p, &xo);
    xo.im = im;
    if (p->xobjects[image->no].type == image_xobject)
    {
        xo.flags |= is_image;
        xo.dpi[0] = dpi_internal;
        xo.dpi[1] = dpi_internal;
        xo.ignoreorientation = image->ignoreorient;
    }
    pdf_init_fit_options(p, &fit);
    fit.flags |= is_image;
    fit.refpoint[0] = x;
    fit.refpoint[1] = y;

    /* parsing option list */
    if (optlist && strlen(optlist))
    {
        pdc_resopt *resopts;
        pdc_clientdata data;

        pdf_set_clientdata(p, &data);
        resopts = pdc_parse_optionlist(p->pdc, optlist,
                      pdf_fit_xobject_options, &data, pdc_true);
        if (!resopts)
            return;

        pdf_get_xobject_options(p, &xo, resopts);
        pdf_get_fit_options(p, &fit, resopts);
    }

    /* put out xobject */
    if (!xo.blind)
    {
        pdf_end_text(p);
        pdf_begin_contents_section(p);



        pdf__save(p);
    }

    pdf_fit_xobject_internal(p, &xo, &fit, NULL);

    if (!xo.blind)
        pdf__restore(p);
}

void
pdf_fit_xobject_internal(PDF *p, pdf_xobject_options *xo, pdf_fit_options *fit,
                         pdc_matrix *immatrix)
{
    pdf_image *image = &p->images[xo->im];
    pdf_xobject_options xo_save;
    pdf_fit_options fit_save;
    pdc_matrix m, mm, sm, ctm_save;
    pdc_vector imgscale, elemsize, elemscale, relpos, polyline[5];
    pdc_box fitbox, elembox;
    pdc_scalar ss, scaley, rowsize = 1, lastratio = 1;
    pdc_scalar dpi_x, dpi_y, tx = 0, ty = 0;
    int indangle, indmirror;
    int is, ip, islast;
    int imageno;

    if (image->mask != pdc_undef)
    {
        ctm_save = p->curr_ppt->gstate[p->curr_ppt->sl].ctm;
        xo_save = *xo;
        fit_save = *fit;
    }

    /* TIFF image orientation */
    if (image->orientation != 1 && !xo->ignoreorientation)
    {
        /* Tag Orientation     =  1,   2,   3,   4,   5,   6,   7,   8  */
        const int addangle[8]  = {0,   0, 180, 180,  90, 270, 270,  90};
        const int rowmirror[8] = {1,  -1,   1,  -1,  -1,   1,  -1,   1};

        is = image->orientation - 1;

	fit->orientate += addangle[is];
	if (fit->orientate >= 360)
	    fit->orientate -= 360;
	xo->scale[0] *= rowmirror[is];
    }

    /* Compensate inverted direction handling in TIFFReadRGBAImageOriented() */
    if (!image->use_raw && image->pixelmode == pdc_true)
	xo->scale[1] *= -1;

    /* calculation of image scale and size */
    imgscale.x = xo->scale[0];
    imgscale.y = xo->scale[1];
    if (xo->flags & is_image)
    {
        if (xo->dpi[0] == dpi_internal)
        {
            dpi_x = image->dpi_x;
            dpi_y = image->dpi_y;
            if (dpi_x > 0 && dpi_y > 0)
            {
                imgscale.x *= 72.0 / dpi_x;
                imgscale.y *= 72.0 / dpi_y;
            }
            else if (dpi_x < 0 && dpi_y < 0)
            {
                imgscale.y *= dpi_y / dpi_x;
            }
        }
        else if (xo->dpi[0] > 0)
        {
            imgscale.x *= 72.0 / xo->dpi[0];
            imgscale.y *= 72.0 / xo->dpi[1];
        }
        rowsize = imgscale.y * image->rowsperstrip;
        imgscale.x *= image->width;
        imgscale.y *= image->height;
        lastratio = (imgscale.y / rowsize) - (image->strips - 1);
        elemsize.x = fabs(imgscale.x);
        elemsize.y = fabs(imgscale.y);
    }
    else
    {
        elemsize.x = fabs(imgscale.x * image->width);
        elemsize.y = fabs(imgscale.y * image->height);
    }

    /* mirroring */
    indmirror = 0;
    if (imgscale.x < 0 && imgscale.y < 0)
        indmirror = 2;
    else if (imgscale.x < 0)
        indmirror = 1;
    else if (imgscale.y < 0)
        indmirror = 3;

    /* orientation */
    indangle = fit->orientate / 90;
    if (indangle % 2)
    {
        ss = elemsize.x;
        elemsize.x = elemsize.y;
        elemsize.y = ss;
    }

    /* box for fitting */
    fitbox.ll.x = 0;
    fitbox.ll.y = 0;
    fitbox.ur.x = fit->boxsize[0];
    fitbox.ur.y = fit->boxsize[1];

    /* relative position */
    relpos.x = fit->position[0] / 100.0;
    relpos.y = fit->position[1] / 100.0;

    /* calculate image box */
    if (fit->fitmethod == pdc_tauto)
        fit->fitmethod = pdc_meet;
    pdc_place_element(fit->fitmethod, 1.0, &fitbox, &relpos,
                      &elemsize, &elembox, &elemscale);



    /* adjust page size */
    if (!xo->blind && xo->adjustpage && PDF_GET_STATE(p) == pdf_state_page)
    {
	pdc_scalar      urx, ury, width, height, theight;

        urx = 2 * fit->refpoint[0] + elembox.ur.x;
        ury = 2 * fit->refpoint[1] + elembox.ur.y;
	pdc_transform_point(&p->curr_ppt->gstate[p->curr_ppt->sl].ctm,
			    urx, ury, &width, &theight);
	height = (p->ydirection > 0) ? theight
			: pdf_get_pageheight(p) - p->ydirection * theight;

	if (height < p->ydirection * theight / 2.0)
	    pdc_error(p->pdc, PDF_E_IMAGE_NOADJUST, 0, 0, 0, 0);

        width = fabs(width);
        height = fabs(height);

	if ((width < PDF_ACRO_MINPAGE || width > PDF_ACRO_MAXPAGE ||
	     height < PDF_ACRO_MINPAGE || height > PDF_ACRO_MAXPAGE))
		pdc_warning(p->pdc, PDF_E_PAGE_SIZE_ACRO, 0, 0, 0, 0);

	pdf_set_pagebox(p, pdf_mediabox, 0, 0, width, height);
	pdf_set_pagebox(p, pdf_cropbox, 0, 0, 0, 0);
    }

    /* reference point */
    pdc_translation_matrix(fit->refpoint[0], fit->refpoint[1], &m);

    /* clipping */
    if (!xo->blind &&
        (fit->fitmethod == pdc_clip || fit->fitmethod == pdc_slice))
    {
        pdf_concat_raw(p, &m);
        pdf__rect(p, 0, 0, fit->boxsize[0], fit->boxsize[1]);
        pdf__clip(p);
        pdc_identity_matrix(&m);
    }

    /* optional rotation */
    if (fabs(fit->rotate) > PDC_FLOAT_PREC)
    {
        pdc_rotation_matrix(p->ydirection * fit->rotate, &mm);
        pdc_multiply_matrix(&mm, &m);
    }

    /* translation of element box */
    elembox.ll.y *= p->ydirection;
    elembox.ur.y *= p->ydirection;
    pdc_box2polyline(&elembox, polyline);
    ip = indangle + indmirror;
    if (ip >= 4) ip -= 4;
    tx = polyline[ip].x;
    ty = polyline[ip].y;
    pdc_translation_matrix(tx, ty, &mm);
    pdc_multiply_matrix(&mm, &m);

    /* orientation of image */
    if (fit->orientate != 0)
    {
        pdc_rotation_matrix(p->ydirection * fit->orientate, &mm);
        pdc_multiply_matrix(&mm, &m);
        if (indangle % 2)
        {
            ss = elemscale.x;
            elemscale.x = elemscale.y;
            elemscale.y = ss;
        }
    }

    /* scaling of image */
    if (image->strips == 1)
        scaley = p->ydirection * imgscale.y * elemscale.y;
    else
        scaley = p->ydirection * rowsize * elemscale.y;
    pdc_scale_matrix(imgscale.x * elemscale.x, scaley, &mm);
    pdc_multiply_matrix(&mm, &m);

    /* ctm */
    if (xo->blind)
    {
        if (immatrix == NULL)
            mm = p->curr_ppt->gstate[p->curr_ppt->sl].ctm;
        else
            mm = *immatrix;
        pdc_multiply_matrix(&m, &mm);
    }
    else
    {
        pdf_concat_raw(p, &m);
        mm = p->curr_ppt->gstate[p->curr_ppt->sl].ctm;
    }

    pdc_trace_protocol(p->pdc, 5, trc_image,
                       "\t\t\tEmbedding matrix components of image %s\"%s\":\n"
                       "\t\t\ta = %f\n"
                       "\t\t\tb = %f\n"
                       "\t\t\tc = %f\n"
                       "\t\t\td = %f\n"
                       "\t\t\te = %f\n"
                       "\t\t\tf = %f\n",
                       image->imagemask ? "mask " : "",
                       image->filename, mm.a, mm.b, mm.c, mm.d, mm.e, mm.f);

    /* check image orientation */
    if (immatrix != NULL)
    {
        *immatrix = mm;
        return;
    }
    if (image->mask != pdc_undef)
    {
        sm = ctm_save;
        xo_save.im = image->mask;
        xo_save.blind = pdc_true;
        pdf_fit_xobject_internal(p, &xo_save, &fit_save, &sm);

        if ((sm.a * mm.a < 0) || (sm.b * mm.b < 0) ||
            (sm.c * mm.c < 0) || (sm.d * mm.d < 0))
        {
            pdc_error(p->pdc, PDF_E_IMAGE_NOMATCH,
                      p->images[image->mask].filename, image->filename, 0, 0);
        }
    }

    if (!(xo->flags & is_image) && !xo->blind)
    {
        pdf_reset_gstate(p);
        pdf_reset_tstate(p);
    }


    if (!xo->blind)
    {
        /* last strip first */
        if (image->strips > 1 && lastratio != 1.0)
        {
            pdc_scale_matrix(1, lastratio, &m);
            pdf_concat_raw(p, &m);
        }

        /* put out image strips separately if available */
        islast = image->strips - 1;
        imageno = image->no + islast;
        for (is = islast; is >= 0; is--)
        {
            pdc_printf(p->out, "/I%d Do\n", imageno);
            p->xobjects[imageno].flags |= xobj_flag_write;
            if (image->strips > 1 && is > 0)
            {
                pdc_translation_matrix(0, 1, &m);
                if (is == islast && lastratio != 1.0)
		{
		    pdc_scale_matrix(1, (1.0 / lastratio), &sm);
                    pdc_multiply_matrix(&sm, &m);
		}
                pdf_concat_raw(p, &m);
                imageno--;
            }
        }
        if (image->mask != pdc_undef)
            p->xobjects[p->images[image->mask].no].flags |= xobj_flag_write;
    }
}

void
pdf_get_image_size(PDF *p, int im, pdc_scalar *width, pdc_scalar *height)
{
    pdf_image *image;

    pdf_check_handle(p, im, pdc_imagehandle);
    image = &p->images[im];

    if (image->orientation < 5 || image->ignoreorient)
    {
        if (width)
            *width = image->width;
        if (height)
            *height = fabs(image->height);
    }
    else
    {
        if (width)
            *width = fabs(image->height);
        if (height)
            *height = image->width;
    }
}

void
pdf_get_image_resolution(PDF *p, int im, pdc_scalar *dpi_x, pdc_scalar *dpi_y)
{
    pdf_image *image;

    pdf_check_handle(p, im, pdc_imagehandle);
    image = &p->images[im];

    if (image->orientation < 5 || image->ignoreorient)
    {
        if (dpi_x)
            *dpi_x = image->dpi_x;
        if (dpi_y)
            *dpi_y = image->dpi_y;
    }
    else
    {
        if (dpi_x)
            *dpi_x = image->dpi_y;
        if (dpi_y)
            *dpi_y = image->dpi_x;
    }
}


#define MAX_THUMBNAIL_SIZE	106

void
pdf__add_thumbnail(PDF *p, int im)
{
    pdf_image *image;

    pdf_check_handle(p, im, pdc_imagehandle);

    if (pdf_get_thumb_id(p) != PDC_BAD_ID)
	pdc_error(p->pdc, PDF_E_IMAGE_THUMB, 0, 0, 0, 0);

    image = &p->images[im];

    if (image->strips > 1)
	pdc_error(p->pdc, PDF_E_IMAGE_THUMB_MULTISTRIP,
	    pdc_errprintf(p->pdc, "%d", im), 0, 0, 0);

    if (image->width > MAX_THUMBNAIL_SIZE || image->height > MAX_THUMBNAIL_SIZE)
	pdc_error(p->pdc, PDF_E_IMAGE_THUMB_SIZE,
	    pdc_errprintf(p->pdc, "%d", im),
	    pdc_errprintf(p->pdc, "%d", MAX_THUMBNAIL_SIZE), 0, 0);

    if (image->colorspace != (int) DeviceGray &&
        image->colorspace != (int) DeviceRGB &&
        image->colorspace != (int) Indexed)
	pdc_error(p->pdc, PDF_E_IMAGE_THUMB_CS,
	    pdc_errprintf(p->pdc, "%d", im), 0, 0, 0);

    /* Add the image to the thumbnail key of the current page.  */
    pdf_set_thumb_id(p, p->xobjects[image->no].obj_id);
}

void
pdf__close_image(PDF *p, int image)
{
    pdf_check_handle(p, image, pdc_imagehandle);
    pdf_cleanup_image(p, image);
}

/* allowed values for options 'bpc' */
static const pdc_keyconn pdf_bpcvalues[] =
{
    {"1", 1}, {"2", 2}, {"4", 4}, {"8", 8}, {"16", 16}, {NULL, 0}
};

/* allowed values for options 'components' */
static const pdc_keyconn pdf_compvalues[] =
{
    {"1", 1}, {"3", 3}, {"4", 4}, {NULL, 0}
};

#define PDF_ICCOPT_FLAG PDC_OPT_UNSUPP

#define PDF_OPIOPT_FLAG PDC_OPT_UNSUPP

/* definitions of open image options */
static const pdc_defopt pdf_open_image_options[] =
{
    {"hypertextencoding", pdc_stringlist,  PDC_OPT_NONE, 1, 1,
      0.0, PDF_MAX_NAMESTRING, NULL},

    {"bitreverse", pdc_booleanlist, 0, 1, 1, 0.0, 0.0, NULL},

    {"bpc", pdc_integerlist, PDC_OPT_INTLIST, 1, 1, 1.0, 16.0, pdf_bpcvalues},

    {"components", pdc_integerlist, PDC_OPT_INTLIST, 1, 1, 1.0, 4.0,
      pdf_compvalues},

    {"height", pdc_integerlist, 0, 1, 1, 1.0, PDC_INT_MAX,  NULL},

    {"width", pdc_integerlist, 0, 1, 1, 1.0, PDC_INT_MAX, NULL},

    {"honoriccprofile", pdc_booleanlist, PDF_ICCOPT_FLAG, 1, 1, 0.0, 0.0, NULL},

    /* ordering of the next three options is significant */

    {"iccprofile", pdc_iccprofilehandle, PDF_ICCOPT_FLAG, 1, 1, 1.0, 0.0, NULL},

    {"colorize", pdc_colorhandle, PDC_OPT_IGNOREIF1, 1, 1, 0.0, 0.0, NULL},

    {"mask", pdc_booleanlist, PDC_OPT_IGNOREIF2, 1, 1, 0.0, 0.0, NULL},

    {"ignoremask", pdc_booleanlist, 0, 1, 1, 0.0, 0.0, NULL},

    {"ignoreorientation", pdc_booleanlist, 0, 1, 1, 0.0, 0.0, NULL},

    {"imagewarning", pdc_booleanlist, 0, 1, 1, 0.0, 0.0, NULL},

    {"inline", pdc_booleanlist, 0, 1, 1, 0.0, 0.0, NULL},

    {"interpolate", pdc_booleanlist, 0, 1, 1, 0.0, 0.0, NULL},

    {"invert", pdc_booleanlist, 0, 1, 1, 0.0, 0.0, NULL},

    {"jpegoptimize", pdc_booleanlist, 0, 1, 1, 0.0, 0.0, NULL},

    {"passthrough", pdc_booleanlist, 0, 1, 1, 0.0, 0.0, NULL},

    {"K", pdc_integerlist, 0, 1, 1, -1.0, 1.0, NULL},

    {"masked", pdc_imagehandle, 0, 1, 1, 0.0, 0.0, NULL},

    {"page", pdc_integerlist, 0, 1, 1, 1.0, PDC_INT_MAX, NULL},

    {"renderingintent", pdc_keywordlist, 0, 1, 1, 0.0, 0.0,
      pdf_renderingintent_pdfkeylist},

    {"reftype", pdc_keywordlist, 0, 1, 1, 0.0, 0.0, pdf_reftype_keys},

    {"template", pdc_booleanlist, 0, 1, 1, 0.0, 0.0, NULL},

    {"iconname", pdc_stringlist, 0, 1, 1, 1.0, PDF_MAX_NAMESTRING, NULL},

    {"OPI-1.3", pdc_stringlist, PDF_OPIOPT_FLAG, 1, 1, 0.0, 32000.0, NULL},

    {"OPI-2.0", pdc_stringlist, PDF_OPIOPT_FLAG | PDC_OPT_IGNOREIF1,
      1, 1, 0.0, 32000.0, NULL},

    PDC_OPT_TERMINATE
};

int
pdf__load_image(
    PDF *p,
    const char *type,
    const char *filename,
    const char *optlist)
{
    const char *keyword = NULL, *stemp1 = NULL, *stemp2 = NULL, *stemp3 = NULL;
    char qualname[32], uptype[16];
    pdc_clientdata data;
    pdc_resopt *resopts;
    pdc_encoding htenc;
    pdf_image_type imgtype;
    int colorize = pdc_undef;
    pdf_image *image;
    pdc_bool indjpeg = pdc_false;
    pdc_bool templ = pdc_false;
    pdc_bool verbose = p->debug[(int) 'i'];
    int legal_states = 0;
    int k, inum, imageslot, retval = -1, errcode = 0;

    if (type == NULL || *type == '\0')
        pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "type", 0, 0, 0);

    /* parsing image type */
    k = pdc_get_keycode_ci(type, pdf_image_keylist);
    if (k == PDC_KEY_NOTFOUND)
        pdc_error(p->pdc, PDC_E_ILLARG_STRING, "type", type, 0, 0);
    imgtype = (pdf_image_type) k;
    type = pdc_get_keyword(imgtype, pdf_image_keylist);

    /* filename must be already converted to UTF-8 */
    pdc_trace_protocol(p->pdc, 1, trc_image,
        "\tImage file: \"%s\"\n", filename);

    /* automatic check */
    if (imgtype == pdf_img_auto)
    {
        pdc_file *fp;
        pdf_tiff_info tiff_info;

        if ((fp = pdf_fopen(p, filename, "", PDC_FILE_BINARY)) == NULL)
	{
	    if (verbose)
		pdc_error(p->pdc, -1, 0, 0, 0, 0);

            return -1;
	}

        if (pdf_is_BMP_file(p, fp))
            imgtype = pdf_img_bmp;
        else if (pdf_is_GIF_file(p, fp))
            imgtype = pdf_img_gif;
        else if (pdf_is_PNG_file(p, fp))
            imgtype = pdf_img_png;
        else if (pdf_is_TIFF_file(p, fp, &tiff_info, pdc_true))
            imgtype = pdf_img_tiff;
        else if (pdf_is_JPEG_file(p, fp))
            imgtype = pdf_img_jpeg;
        else if (pdf_is_JPX_file(p, fp))
            imgtype = pdf_img_jpeg2000;
        else
        {
            pdc_fclose(fp);

	    pdc_set_errmsg(p->pdc, PDF_E_IMAGE_UNKNOWN, filename, 0, 0, 0);

            if (verbose)
                pdc_error(p->pdc, -1, 0, 0, 0, 0);

            return -1;
        }
        pdc_fclose(fp);
        type = pdc_get_keyword(imgtype, pdf_image_keylist);
    }

    for (k = 0; k < (int) strlen(type); k++)
        uptype[k] = toupper((int) type[k]);
    uptype[k] = 0;
    pdc_trace_protocol(p->pdc, 1, trc_image,
        "\tImage type: \"%s\" detected\n", uptype);

    if (imgtype == pdf_img_jpeg2000)
    {
        pdc_set_errmsg(p->pdc, PDF_E_UNSUPP_JPEG2000, 0, 0, 0, 0);

        if (verbose)
            pdc_error(p->pdc, -1, 0, 0, 0, 0);

        return -1;
    }

    /* find free slot */
    for (imageslot = 0; imageslot < p->images_capacity; imageslot++)
        if (!p->images[imageslot].in_use)
            break;

    if (imageslot == p->images_capacity)
        pdf_grow_images(p);
    image = &p->images[imageslot];

    /* copy filename */
    image->filename = pdc_strdup(p->pdc, filename);

    /* inherit global flags */
    image->verbose = verbose;
    image->ri = p->rendintent;

    /* parsing optlist */
    if (optlist && strlen(optlist))
    {
        pdf_set_clientdata(p, &data);
        resopts = pdc_parse_optionlist(p->pdc, optlist, pdf_open_image_options,
                                       &data, image->verbose);
        if (!resopts)
            return -1;

        /* save and check options */
        keyword = "imagewarning";
        pdc_get_optvalues(keyword, resopts, &image->verbose, NULL);
        verbose = image->verbose;

        keyword = "reftype";
        if (pdc_get_optvalues(keyword, resopts, &inum, NULL))
        {
            image->reference = (pdf_ref_type) inum;
            if (image->reference != pdf_ref_direct &&
                imgtype != pdf_img_ccitt &&
                imgtype != pdf_img_jpeg &&
                imgtype != pdf_img_raw)
            {
                if (image->verbose)
                    pdc_warning(p->pdc, PDF_E_IMAGE_OPTUNSUPP, keyword, uptype,
                                0, 0);
                image->reference = pdf_ref_direct;
            }
        }
        indjpeg = (imgtype == pdf_img_jpeg &&
                   image->reference != pdf_ref_direct) ? pdc_true : pdc_false;

        keyword = "bpc";
        if (pdc_get_optvalues(keyword, resopts, &image->bpc, NULL))
        {
            if (image->verbose && imgtype != pdf_img_raw && !indjpeg)
                pdc_warning(p->pdc, PDF_E_IMAGE_OPTUNREAS, keyword, uptype,
                            0, 0);
            if (image->bpc == 16)
            {
                if (p->compatibility < PDC_1_5)
                {
                    errcode = PDC_E_OPT_VERSION;
                    stemp1 = "bpc=16";
                    stemp2 = pdc_get_pdfversion(p->pdc, p->compatibility);
                    goto PDF_IMAGE_ERROR;
                }
            }
        }

        keyword = "components";
        if (pdc_get_optvalues(keyword, resopts, &image->components, NULL))
        {
            if (image->verbose && imgtype != pdf_img_raw && !indjpeg)
                pdc_warning(p->pdc, PDF_E_IMAGE_OPTUNREAS, keyword, uptype,
                            0, 0);
        }

        keyword = "height";
        if (pdc_get_optvalues(keyword, resopts, &image->height_pixel, NULL))
        {
            if (image->verbose && imgtype != pdf_img_ccitt &&
                                  imgtype != pdf_img_raw && !indjpeg)
                pdc_warning(p->pdc, PDF_E_IMAGE_OPTUNREAS, keyword, uptype,
                            0, 0);
        }

        keyword = "width";
        if (pdc_get_optvalues(keyword, resopts, &image->width_pixel, NULL))
        {
            if (image->verbose && imgtype != pdf_img_raw &&
                                  imgtype != pdf_img_ccitt && !indjpeg)
                pdc_warning(p->pdc, PDF_E_IMAGE_OPTUNREAS, keyword, uptype,
                            0, 0);
        }

        keyword = "bitreverse";
        if (pdc_get_optvalues(keyword, resopts, &image->bitreverse, NULL))
        {
            if (image->verbose && image->bitreverse &&
               (imgtype != pdf_img_ccitt || image->reference != pdf_ref_direct))
                pdc_warning(p->pdc, PDF_E_IMAGE_OPTUNREAS, keyword, uptype,
                            0, 0);
        }

        keyword = "colorize";
        if (pdc_get_optvalues(keyword, resopts, &colorize, NULL))
        {
            if (image->verbose && imgtype == pdf_img_jpeg2000)
                pdc_warning(p->pdc, PDF_E_IMAGE_OPTUNREAS, keyword, uptype,
                            0, 0);
        }


        keyword = "ignoremask";
        if (pdc_get_optvalues(keyword, resopts, &image->ignoremask, NULL))
        {
            if (image->verbose && (imgtype == pdf_img_bmp ||
                                   imgtype == pdf_img_ccitt ||
                                   imgtype == pdf_img_raw))
                pdc_warning(p->pdc, PDF_E_IMAGE_OPTUNSUPP, keyword, uptype,
                            0, 0);
        }

        keyword = "ignoreorient";
        if (pdc_get_optvalues(keyword, resopts, &image->ignoreorient, NULL))
        {
            if (image->verbose && imgtype == pdf_img_tiff)
                pdc_warning(p->pdc, PDF_E_IMAGE_OPTUNSUPP, keyword, uptype,
                            0, 0);
        }

        keyword = "inline";
        if (pdc_get_optvalues(keyword, resopts,
                              &image->doinline, NULL) && image->doinline)
        {
            if (imgtype != pdf_img_ccitt &&
                imgtype != pdf_img_jpeg &&
                imgtype != pdf_img_raw)
            {
                if (image->verbose)
                    pdc_warning(p->pdc,
                    PDF_E_IMAGE_OPTUNSUPP, keyword, uptype, 0, 0);
                image->doinline = pdc_false;
            }
            else if (image->verbose && image->reference != pdf_ref_direct)
            {
                pdc_warning(p->pdc, PDF_E_IMAGE_OPTUNREAS, keyword, uptype,
                            0, 0);
                image->doinline = pdc_false;
            }
        }

        keyword = "interpolate";
        pdc_get_optvalues(keyword, resopts, &image->interpolate, NULL);

        keyword = "invert";
        if (pdc_get_optvalues(keyword, resopts, &image->invert, NULL))
        {
            if (image->verbose && imgtype == pdf_img_jpeg2000)
                pdc_warning(p->pdc, PDF_E_IMAGE_OPTUNREAS, keyword, uptype,
                            0, 0);
        }

        keyword = "jpegoptimize";
        pdc_get_optvalues(keyword, resopts, &image->jpegoptimize, NULL);

        keyword = "passthrough";
        pdc_get_optvalues(keyword, resopts, &image->passthrough, NULL);

        keyword = "K";
        if (pdc_get_optvalues(keyword, resopts, &image->K, NULL))
        {
            if (image->verbose && imgtype != pdf_img_ccitt)
                pdc_warning(p->pdc, PDF_E_IMAGE_OPTUNREAS, keyword, uptype,
                            0, 0);
        }

        keyword = "mask";
        pdc_get_optvalues(keyword, resopts, &image->imagemask, NULL);


        keyword = "masked";
        if (pdc_get_optvalues(keyword, resopts, &image->mask, NULL))
        {

            if (!p->images[image->mask].in_use ||
                 p->images[image->mask].strips != 1 ||
                (p->compatibility <= PDC_1_3 &&
                (p->images[image->mask].imagemask != pdc_true ||
                 p->images[image->mask].bpc != 1)))
            {
                errcode = PDF_E_IMAGE_OPTBADMASK;
                stemp1 = keyword;
                stemp2 = pdc_errprintf(p->pdc, "%d", image->mask);
                goto PDF_IMAGE_ERROR;
            }
        }

        keyword = "renderingintent";
        if (pdc_get_optvalues(keyword, resopts, &inum, NULL))
            image->ri = (pdf_renderingintent) inum;

        keyword = "page";
        if (pdc_get_optvalues(keyword, resopts, &image->page, NULL))
        {
            if (imgtype != pdf_img_tiff)
            {
                if (image->page == 1)
                {
                    if (image->verbose)
                        pdc_warning(p->pdc, PDF_E_IMAGE_OPTUNSUPP, keyword,
                                    uptype, 0, 0);
                }
                else
                {
                    errcode = PDF_E_IMAGE_NOPAGE;
                    stemp1 = pdc_errprintf(p->pdc, "%d", image->page);
                    stemp2 = uptype;
                    stemp3 = pdc_errprintf(p->pdc, "%s", image->filename);
                    goto PDF_IMAGE_ERROR;
                }
            }
        }

        keyword = "template";
        if (pdc_get_optvalues(keyword, resopts, &templ, NULL))
        {
            if (templ && image->doinline)
            {
                if (image->verbose)
                    pdc_warning(p->pdc, PDC_E_OPT_IGNORE, keyword, "inline",
                                0, 0);
                templ = pdc_false;
            }
        }

        htenc =
            pdf_get_hypertextencoding_opt(p, resopts, NULL, image->verbose);
        keyword = "iconname";
        if (pdf_get_opt_textlist(p, keyword, resopts, htenc, pdc_true, NULL,
                                 &image->iconname, NULL))
        {
            if (image->doinline)
            {
                image->iconname = NULL;
                if (image->verbose)
                    pdc_warning(p->pdc, PDC_E_OPT_IGNORE, keyword, "inline",
                                0, 0);
            }
            else
            {
                pdc_save_lastopt(resopts, PDC_OPT_SAVE1ELEM);
                templ = pdc_true;
            }
        }


    }

    /* precise scope diagnosis */
    if (image->doinline)
        legal_states = pdf_state_content;
    else if (templ)
        legal_states = pdf_state_document;
    else
        legal_states = pdf_state_document | pdf_state_page | pdf_state_font;
    PDF_CHECK_STATE(p, legal_states);

    /* required options */
    if (imgtype == pdf_img_raw || imgtype == pdf_img_ccitt || indjpeg)
    {
        keyword = "";
        if (image->height_pixel == pdc_undef)
            keyword = "height";
        else if (image->width_pixel == pdc_undef)
            keyword = "width";
        else
        {
            image->width = image->width_pixel;
            image->height = image->height_pixel;
        }

        if (imgtype == pdf_img_ccitt)
        {
            image->components = 1;
            image->bpc = 1;
        }
        if (image->bpc == pdc_undef)
            keyword = "bpc";
        else if (image->components == pdc_undef)
            keyword = "components";

        if (*keyword)
        {
            errcode = PDC_E_OPT_NOTFOUND;
            stemp1 = keyword;
            goto PDF_IMAGE_ERROR;
        }
    }


    /* set colorspace */
    if (colorize != pdc_undef)
    {
        image->colorspace = colorize;
    }
    else if (image->imagemask == pdc_true)
    {
        image->colorspace = (int) DeviceGray;
    }
    else
    {
        switch(image->components)
        {
            case 1:
            image->colorspace = DeviceGray;
            break;

            case 3:
            image->colorspace = DeviceRGB;
            break;

            case 4:
            image->colorspace = DeviceCMYK;
            break;

            default:
            break;
        }
    }

    /* try to open image file */
    if (image->reference == pdf_ref_direct)
    {
	strcpy(qualname, uptype);
	strcat(qualname, " ");
        image->fp = pdf_fopen(p, image->filename, qualname, PDC_FILE_BINARY);
        if (image->fp == NULL)
            goto PDF_IMAGE_ERROR;
    }


    /* set image type */
    image->type = imgtype;

    /* call working function */
    switch (imgtype)
    {
        case pdf_img_bmp:
        retval = pdf_process_BMP_data(p, imageslot);
        break;

        case pdf_img_ccitt:
        retval = pdf_process_CCITT_data(p, imageslot);
        break;

        case pdf_img_gif:
        retval = pdf_process_GIF_data(p, imageslot);
        break;

        case pdf_img_jpeg:
        retval = pdf_process_JPEG_data(p, imageslot);
        break;

        case pdf_img_jpeg2000:
        retval = pdf_process_JPX_data(p, imageslot);
        break;

        case pdf_img_png:
        retval = pdf_process_PNG_data(p, imageslot);
        break;

        default:
        case pdf_img_raw:
        retval = pdf_process_RAW_data(p, imageslot);
        break;

        case pdf_img_tiff:
        retval = pdf_process_TIFF_data(p, imageslot);
        break;
    }

    /* cleanup */
    if (retval == -1)
    {
        pdf_cleanup_image(p, imageslot);
    }
    else
    {
        if (image->fp)
            pdc_fclose(image->fp);
        image->fp = NULL;

        /* trace protocol */
        if (pdc_trace_protocol_is_enabled(p->pdc, 1, trc_image))
        {
            pdc_scalar width, height, dpi_x, dpi_y;

            pdf_get_image_size(p, imageslot, &width, &height);
            pdf_get_image_resolution(p, imageslot, &dpi_x, &dpi_y);

            pdc_trace(p->pdc, "\tImage width: %g pixel\n"
                              "\tImage height: %g pixel\n"
                              "\tImage x resolution: %g dpi\n"
                              "\tImage y resolution: %g dpi\n",
                              width, height, dpi_x, dpi_y);

            if (imgtype == pdf_img_tiff)
                pdc_trace(p->pdc, "\tOrientation tag: %d\n",
                                  image->orientation);

        }

        if (templ)
        {
            retval = pdf_embed_image(p, imageslot);
            pdf_cleanup_image(p, imageslot);
        }
    }

    return retval;

    PDF_IMAGE_ERROR:
    pdf_cleanup_image(p, imageslot);
    if (errcode)
        pdc_set_errmsg(p->pdc, errcode, stemp1, stemp2, stemp3, 0);
    if (verbose)
        pdc_error(p->pdc, -1, 0, 0, 0, 0);

    return retval;
}




