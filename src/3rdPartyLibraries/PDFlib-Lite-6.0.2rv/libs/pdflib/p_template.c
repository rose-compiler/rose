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

/* $Id: p_template.c,v 1.1 2005/10/11 17:18:15 vuduc2 Exp $
 *
 * PDFlib template routines
 *
 */

#include "p_intern.h"
#include "p_image.h"
#include "p_font.h"

int
pdf_embed_image(PDF *p, int im)
{
    pdf_image *image = &p->images[im];
    char optlist[2048], *ol;
    pdc_scalar width, height;
    int templ;

    width = image->width;
    height = fabs(image->height);

    /* create option list */
    optlist[0] = 0;
    ol = optlist;


    if (image->iconname)
        sprintf(ol, "iconname {%s}", image->iconname);

    /* create template */
    templ = pdf__begin_template(p, width, height, optlist);

    /* fit image */
    sprintf(optlist, "boxsize {%g %g} fitmethod meet", width, height);
    pdf__fit_image(p, im, 0, 0, optlist);

    /* end template */
    pdf__end_template(p);

    return templ;
}

#define PDF_OPIOPT_FLAG PDC_OPT_UNSUPP

/* definitions of begin template options */
static const pdc_defopt pdf_begin_template_options[] =
{
    {"OPI-1.3", pdc_stringlist, PDF_OPIOPT_FLAG, 1, 1, 0.0, 32000.0, NULL},

    {"OPI-2.0", pdc_stringlist, PDF_OPIOPT_FLAG | PDC_OPT_IGNOREIF1,
      1, 1, 0.0, 32000.0, NULL},

    {"iconname", pdc_stringlist, 0, 1, 1, 1.0, PDF_MAX_NAMESTRING, NULL},

    PDC_OPT_TERMINATE
};

/* Start a new template definition. */
int
pdf__begin_template(PDF *p, pdc_scalar width, pdc_scalar height,
                    const char *optlist)
{
    pdf_image *image;
    pdc_resopt *resopts;
    const char *keyword;
    char *iconname = NULL;
    int im = -1;

    if (width <= 0)
	pdc_error(p->pdc, PDC_E_ILLARG_POSITIVE, "width", 0, 0, 0);

    if (height <= 0)
	pdc_error(p->pdc, PDC_E_ILLARG_POSITIVE, "height", 0, 0, 0);

    for (im = 0; im < p->images_capacity; im++)
	if (!p->images[im].in_use)		/* found free slot */
	    break;

    if (im == p->images_capacity)
	pdf_grow_images(p);

    image		= &p->images[im];
    image->in_use       = pdc_true;             /* mark slot as used */

    /* parsing optlist */
    resopts = pdc_parse_optionlist(p->pdc, optlist, pdf_begin_template_options,
                                   NULL, pdc_false);

    /* save and check options */
    if (resopts)
    {

        keyword = "iconname";
        if (pdc_get_optvalues(keyword, resopts, NULL, NULL))
            iconname = (char *) pdc_save_lastopt(resopts, PDC_OPT_SAVE1ELEM);

        pdc_cleanup_optionlist(p->pdc, resopts);
    }

    pdf_pg_suspend(p);
    PDF_SET_STATE(p, pdf_state_template);

    /* form xobject */
    image->no		= pdf_new_xobject(p, form_xobject, PDC_NEW_ID);
    image->width	= width;
    image->height	= height;

    p->templ		= im;		/* remember the current template id */

    pdc_begin_dict(p->out);				/* form xobject dict*/
    pdc_puts(p->out, "/Type/XObject\n");
    pdc_puts(p->out, "/Subtype/Form\n");

    /* contrary to the PDF reference /FormType and /Matrix are required! */
    pdc_printf(p->out, "/FormType 1\n");
    pdc_printf(p->out, "/Matrix[1 0 0 1 0 0]\n");

    p->res_id = pdc_alloc_id(p->out);
    pdc_objref(p->out, "/Resources", p->res_id);

    pdc_printf(p->out, "/BBox[0 0 %f %f]\n", width, height);


    p->length_id = pdc_alloc_id(p->out);
    pdc_objref(p->out, "/Length", p->length_id);

    if (pdc_get_compresslevel(p->out))
	pdc_puts(p->out, "/Filter/FlateDecode\n");

    pdc_end_dict(p->out);				/* form xobject dict*/

    pdc_begin_pdfstream(p->out);

    /* top-down y-coordinates */
    pdf_set_topdownsystem(p, height);

    /* insert icon name */
    if (iconname)
    {
        pdc_id obj_id = pdf_get_xobject(p, im);
        pdf_insert_name(p, iconname, names_ap, obj_id);
    }

    return im;
}

/* Finish the template definition. */
void
pdf__end_template(PDF *p)
{
    /* check whether pdf__save() and pdf__restore() calls are balanced */
    if (p->curr_ppt->sl > 0)
        pdc_error(p->pdc, PDF_E_GSTATE_UNMATCHEDSAVE, 0, 0, 0, 0);

    pdf_end_text(p);
    pdc_end_pdfstream(p->out);
    pdc_end_obj(p->out);                        /* form xobject */

    pdc_put_pdfstreamlength(p->out, p->length_id);

    pdc_begin_obj(p->out, p->res_id);           /* Resource object */
    pdc_begin_dict(p->out);                     /* Resource dict */

    pdf_write_page_fonts(p);                    /* Font resources */

    pdf_write_page_colorspaces(p);              /* Color space resources */

    pdf_write_page_pattern(p);                  /* Pattern resources */

    pdf_write_xobjects(p);                      /* XObject resources */

    pdf_write_page_extgstates(p);               /* ExtGState resources */

    pdc_end_dict(p->out);                       /* resource dict */
    pdc_end_obj(p->out);                        /* resource object */

    pdf_pg_resume(p, -1);

    if (p->flush & pdf_flush_content)
        pdc_flush_stream(p->out);
}
