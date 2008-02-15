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

/* p_annots.c
 *
 * PDFlib routines for annnotations
 *
 */

#include <string.h>

#include "p_intern.h"

static const char *pdf_border_style_names[] = {
    "S",	/* solid border */
    "D",	/* dashed border */
    "B",	/* beveled (three-dimensional) border */
    "I",	/* inset border */
    "U"		/* underlined border */
};

static const char *pdf_icon_names[] = {
    /* embedded file icon names */
    "Graph", "Paperclip", "Pushpin", "Tag",

    /* text annotation icon names */
    "Comment", "Insert", "Note", "Paragraph", "NewParagraph", "Key", "Help"
};

/* flags for annotation properties */
typedef enum {
    pdf_ann_flag_invisible	= 1,
    pdf_ann_flag_hidden		= 2,
    pdf_ann_flag_print		= 4,
    pdf_ann_flag_nozoom		= 8,
    pdf_ann_flag_norotate	= 16,
    pdf_ann_flag_noview		= 32,
    pdf_ann_flag_readonly	= 64
} pdf_ann_flag;

void
pdf_init_annots(PDF *p)
{
    /* annotation border style defaults */
    p->border_style	= border_solid;
    p->border_width	= (float) 1.0;
    p->border_red	= (float) 0.0;
    p->border_green	= (float) 0.0;
    p->border_blue	= (float) 0.0;
    p->border_dash1	= (float) 3.0;
    p->border_dash2	= (float) 3.0;
}

/* Write annotation border style and color */
static void
pdf_write_border_style(PDF *p, pdf_annot *ann)
{
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE], buf3[FLOATBUFSIZE];

    /* don't write the default values */
    if (ann->border_style == border_solid && ann->border_width == (float) 1.0 &&
	p->border_red == (float) 0.0 && p->border_green == (float) 0.0 &&
	p->border_blue == (float) 0.0 &&
	p->border_dash1 == (float) 3.0 && p->border_dash2 == (float) 3.0)
	return;

    pdf_puts(p, "/BS");
    pdf_begin_dict(p);			/* BS dict */
    pdf_puts(p, "/Type/Border\n");

    if (ann->border_style != border_solid)
	pdf_printf(p, "/S/%s\n", pdf_border_style_names[ann->border_style]);

    if (ann->border_style == border_dashed)
	pdf_printf(p, "/D[%s %s]\n",
	    pdf_float(buf1, ann->border_dash1),
	    pdf_float(buf2, ann->border_dash2));

    pdf_end_dict(p);			/* BS dict */

    /* Write the Border key in old-style PDF 1.1 format */
    pdf_printf(p, "/Border[0 0 %s", pdf_float(buf1, ann->border_width));

    if (ann->border_style == border_dashed &&
	(ann->border_dash1 != (float) 0.0 || ann->border_dash2 != (float) 0.0))
	/* set dashed border */
	pdf_printf(p, "[%s %s]",
		pdf_float(buf1, ann->border_dash1),
		pdf_float(buf2, ann->border_dash2));

    pdf_puts(p, "]\n");

    /* write annotation color */
    pdf_printf(p, "/C[%s %s %s]\n",
	    pdf_float(buf1, ann->border_red),
	    pdf_float(buf2, ann->border_green),
	    pdf_float(buf3, ann->border_blue));
}

void
pdf_write_page_annots(PDF *p)
{
    pdf_annot	*ann;
    long	length, start_pos;
    id		length_id;
    char buf1[FLOATBUFSIZE], buf2[FLOATBUFSIZE];
    char buf3[FLOATBUFSIZE], buf4[FLOATBUFSIZE];
    PDF_data_source src;

    for (ann = p->annots; ann != NULL; ann = ann->next) {
	pdf_begin_obj(p, ann->obj_id);	/* Annotation object */
	pdf_begin_dict(p);		/* Annotation dict */

	pdf_puts(p, "/Type/Annot\n");
	switch (ann->type) {
	    case ann_text:
		pdf_puts(p, "/Subtype/Text\n");
		pdf_printf(p, "/Rect[%s %s %s %s]\n", 
		pdf_float(buf1, ann->rect.llx), pdf_float(buf2, ann->rect.lly),
		pdf_float(buf3, ann->rect.urx), pdf_float(buf4, ann->rect.ury));

		pdf_write_border_style(p, ann);

		if (ann->open)
		    pdf_puts(p, "/Open true\n");

		if (ann->icon != icon_text_note)	/* note is default */
		    pdf_printf(p, "/Name/%s\n", pdf_icon_names[ann->icon]);

		/* Contents key is required, but may be empty */
		pdf_puts(p, "/Contents");

		if (ann->contents) {
		    pdf_quote_string(p, ann->contents);
		    pdf_putc(p, PDF_NEWLINE);
		} else
		    pdf_puts(p, "()\n"); /* empty contents is OK */

		/* title is optional */
		if (ann->title) {
		    pdf_puts(p, "/T");
		    pdf_quote_string(p, ann->title);
		    pdf_putc(p, PDF_NEWLINE);
		}

		break;

	    case ann_locallink:
		pdf_puts(p, "/Subtype/Link\n");
		pdf_printf(p, "/Rect[%s %s %s %s]\n", 
		pdf_float(buf1, ann->rect.llx), pdf_float(buf2, ann->rect.lly),
		pdf_float(buf3, ann->rect.urx), pdf_float(buf4, ann->rect.ury));

		pdf_write_border_style(p, ann);

		/* preallocate page object id for a later page */
		if (ann->dest.page > p->current_page) {
		    while (ann->dest.page >= p->pages_capacity)
			pdf_grow_pages(p);

		    /* if this page has already been used as a link target
		     * it will already have an object id.
		     */
		    if (p->pages[ann->dest.page] == BAD_ID)
			p->pages[ann->dest.page] = pdf_alloc_id(p);
		}

		if (ann->dest.type == retain) {
		    pdf_printf(p, "/Dest[%ld 0 R/XYZ null null 0]\n",
			    p->pages[ann->dest.page]);
		} else if (ann->dest.type == fitpage) {
		    pdf_printf(p, "/Dest[%ld 0 R/Fit]\n",
			    p->pages[ann->dest.page]);
		} else if (ann->dest.type == fitwidth) {
		    pdf_printf(p, "/Dest[%ld 0 R/FitH 0]\n",
			    p->pages[ann->dest.page]);
		} else if (ann->dest.type == fitheight) {
		    pdf_printf(p, "/Dest[%ld 0 R/FitV 0]\n",
			    p->pages[ann->dest.page]);
		} else if (ann->dest.type == fitbbox) {
		    pdf_printf(p, "/Dest[%ld 0 R/FitB]\n",
			    p->pages[ann->dest.page]);
		}

		break;

	    case ann_pdflink:
		pdf_puts(p, "/Subtype/Link\n");
		pdf_printf(p, "/Rect[%s %s %s %s]\n", 
		pdf_float(buf1, ann->rect.llx), pdf_float(buf2, ann->rect.lly),
		pdf_float(buf3, ann->rect.urx), pdf_float(buf4, ann->rect.ury));

		pdf_write_border_style(p, ann);

		pdf_puts(p, "/A");
		pdf_begin_dict(p);			/* A dict */
		pdf_puts(p, "/Type/Action/S/GoToR\n");

		if (ann->dest.type == retain) {
		    pdf_printf(p, "/D[%d 0 R/XYZ null null 0]\n",
			    ann->dest.page-1);	/* zero-based */
		} else if (ann->dest.type == fitpage) {
		    /* zero-based */
		    pdf_printf(p, "/D[%d 0 R/Fit]\n", ann->dest.page-1);
		} else if (ann->dest.type == fitwidth) {
		    /* Trick: we don't know the height of a future page yet,
		     * so we use a "large" value for top which will do for
		     * most pages. If it doesn't work, not much harm is done.
		     */
		    /* zero-based */
		    pdf_printf(p, "/D[%d 0 R/FitH 2000]\n", ann->dest.page-1);
		} else if (ann->dest.type == fitheight) {
		    /* zero-based */
		    pdf_printf(p, "/D[%d 0 R/FitV 0]\n", ann->dest.page-1);
		} else if (ann->dest.type == fitbbox) {
		    /* zero-based */
		    pdf_printf(p, "/D[%d 0 R/FitB]\n", ann->dest.page-1);
		}

		pdf_puts(p, "/F");
		pdf_begin_dict(p);			/* F dict */
		pdf_puts(p, "/Type/FileSpec\n");
		pdf_printf(p, "/F(%s)\n", ann->filename);
		pdf_end_dict(p);			/* F dict */

		pdf_end_dict(p);			/* A dict */

		break;

	    case ann_launchlink:
		pdf_puts(p, "/Subtype/Link\n");
		pdf_printf(p, "/Rect[%s %s %s %s]\n", 
		pdf_float(buf1, ann->rect.llx), pdf_float(buf2, ann->rect.lly),
		pdf_float(buf3, ann->rect.urx), pdf_float(buf4, ann->rect.ury));

		pdf_write_border_style(p, ann);

		pdf_puts(p, "/A");
		pdf_begin_dict(p);			/* A dict */
		pdf_puts(p, "/Type/Action/S/Launch\n");

		pdf_puts(p, "/F");
		pdf_begin_dict(p);			/* F dict */
		pdf_puts(p, "/Type/FileSpec\n");
		pdf_printf(p, "/F(%s)\n", ann->filename);
		pdf_end_dict(p);			/* F dict */

		pdf_end_dict(p);			/* A dict */

		break;

	    case ann_weblink:
		pdf_puts(p, "/Subtype/Link\n");
		pdf_printf(p, "/Rect[%s %s %s %s]\n", 
		pdf_float(buf1, ann->rect.llx), pdf_float(buf2, ann->rect.lly),
		pdf_float(buf3, ann->rect.urx), pdf_float(buf4, ann->rect.ury));

		pdf_write_border_style(p, ann);

		pdf_printf(p, "/A<</S/URI/URI(%s)>>\n", 
				    ann->filename);
		break;

	    case ann_attach:
		pdf_puts(p, "/Subtype/FileAttachment\n");
		pdf_printf(p, "/Rect[%s %s %s %s]\n", 
		pdf_float(buf1, ann->rect.llx), pdf_float(buf2, ann->rect.lly),
		pdf_float(buf3, ann->rect.urx), pdf_float(buf4, ann->rect.ury));

		if (ann->icon != icon_file_pushpin)	/* pushpin is default */
		    pdf_printf(p, "/Name/%s\n",
		    	pdf_icon_names[ann->icon]);

		if (ann->title) {
		    pdf_puts(p, "/T");
		    pdf_quote_string(p, ann->title);
		    pdf_putc(p, PDF_NEWLINE);
		}

		if (ann->contents) {
		    pdf_puts(p, "/Contents");
		    pdf_quote_string(p, ann->contents);
		    pdf_putc(p, PDF_NEWLINE);
		}
		    
		/* the icon is too small without these flags (=28) */
		pdf_printf(p, "/F %d\n",
			pdf_ann_flag_print |
			pdf_ann_flag_nozoom | 
			pdf_ann_flag_norotate);

		pdf_puts(p, "/FS");
		pdf_begin_dict(p);			/* FS dict */
		pdf_puts(p, "/Type/FileSpec\n");

		pdf_printf(p, "/F(%s)\n", ann->filename);

		/* alloc id for the actual embedded file stream */
		ann->obj_id = pdf_alloc_id(p);
		pdf_printf(p, "/EF<</F %ld 0 R>>\n", ann->obj_id);
		pdf_end_dict(p);			/* FS dict */

		break;

	    default:
		pdf_error(p, PDF_SystemError, "Unknown annotation type %d",
				ann->type);
	}

	pdf_end_dict(p);		/* Annotation dict */
	pdf_end_obj(p);			/* Annotation object */
    }

    /* Write the actual embedded files with preallocated ids */
    for (ann = p->annots; ann != NULL; ann = ann->next) {
	if (ann->type != ann_attach)
	    continue;

	pdf_begin_obj(p, ann->obj_id);	/* EmbeddedFile */
	pdf_puts(p, "<</Type/EmbeddedFile\n");

	if (ann->mimetype)
	    pdf_printf(p, "/Subtype (%s)\n", ann->mimetype);

	if (p->compress)
	    pdf_puts(p, "/Filter/FlateDecode\n");

	length_id = pdf_alloc_id(p);
	pdf_printf(p, "/Length %ld 0 R\n", length_id);
	pdf_end_dict(p);		/* F dict */

	pdf_begin_stream(p);		/* Embedded file stream */
	start_pos = pdf_tell(p);

	/* write the file in the PDF */
	src.private_data = (void *) ann->filename;
	src.init	= pdf_data_source_file_init;
	src.fill	= pdf_data_source_file_fill;
	src.terminate	= pdf_data_source_file_terminate;

	pdf_compress(p, &src);

	length = pdf_tell(p) - start_pos;
	pdf_end_stream(p);		/* Embedded file stream */
	pdf_end_obj(p);			/* EmbeddedFile object */

	pdf_begin_obj(p, length_id);	/* Length object */
	pdf_printf(p, "%ld\n", length);
	pdf_end_obj(p);			/* Length object */

	if (p->stream.flush & PDF_FLUSH_CONTENT)
	    pdf_flush_stream(p);
    }
}

void
pdf_init_page_annots(PDF *p)
{
    p->annots = NULL;
}

void
pdf_cleanup_page_annots(PDF *p)
{
    pdf_annot *ann, *old;

    for (ann = p->annots; ann != (pdf_annot *) NULL; /* */ ) {
	switch (ann->type) {
	    case ann_text:
		if (ann->contents)
		    p->free(p, ann->contents);
		if (ann->title)
		    p->free(p, ann->title);
		break;

	    case ann_locallink:
		break;

	    case ann_launchlink:
		p->free(p, ann->filename);
		break;

	    case ann_pdflink:
		p->free(p, ann->filename);
		break;

	    case ann_weblink:
		p->free(p, ann->filename);
		break;

	    case ann_attach:
		p->free(p, ann->filename);
		if (ann->contents)
		    p->free(p, ann->contents);
		if (ann->title)
		    p->free(p, ann->title);
		if (ann->mimetype)
		    p->free(p, ann->mimetype);
		break;

	    default:
		pdf_error(p, PDF_SystemError, "Unknown annotation type %d",
				ann->type);
	}
	old = ann;
	ann = old->next;
	p->free(p, old);
    }
    p->annots = NULL;
}

/* Insert new annotation at the end of the annots chain */
static void
pdf_add_annot(PDF *p, pdf_annot *ann)
{
    pdf_annot *last;

    /* fetch current border state from p */
    ann->border_style	= p->border_style;
    ann->border_width	= p->border_width;
    ann->border_red	= p->border_red;
    ann->border_green	= p->border_green;
    ann->border_blue	= p->border_blue;
    ann->border_dash1	= p->border_dash1;
    ann->border_dash2	= p->border_dash2;

    ann->next = NULL;

    if (p->annots == NULL)
	p->annots = ann;
    else {
	for (last = p->annots; last->next != NULL; /* */ )
	    last = last->next;
	last->next = ann;
    }
}

/* Attach an arbitrary file to the PDF. Note that the actual
 * embedding takes place in PDF_end_page().
 * description, author, and mimetype may be NULL.
 */
PDFLIB_API void PDFLIB_CALL
PDF_attach_file(PDF *p, float llx, float lly, float urx, float ury, const char *filename, const char *description, const char *author, const char *mimetype, const char *icon)
{
    pdf_annot *ann;

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->compatibility == PDF_1_2)
	pdf_error(p, PDF_RuntimeError,
		"File attachments are not supported in PDF 1.2");

    if (filename == NULL)
	pdf_error(p, PDF_ValueError, "Empty file name for file attachment");

    ann = (pdf_annot *) p->malloc(p, sizeof(pdf_annot),"PDF_attach_file");

    ann->type	  = ann_attach;
    ann->rect.llx = llx;
    ann->rect.lly = lly;
    ann->rect.urx = urx;
    ann->rect.ury = ury;

    if (icon == NULL)
	ann->icon = icon_file_pushpin;
    else if (!strcmp(icon, "graph"))
	ann->icon = icon_file_graph;
    else if (!strcmp(icon, "paperclip"))
	ann->icon = icon_file_paperclip;
    else if (!strcmp(icon, "pushpin"))
	ann->icon = icon_file_pushpin;
    else if (!strcmp(icon, "tag"))
	ann->icon = icon_file_tag;
    else
	pdf_error(p, PDF_ValueError, "Unknown icon type '%s'for embedded file",
			icon);

    ann->filename = (char *) pdf_strdup(p, filename);

    if (description != NULL)
	ann->contents = (char *) pdf_strdup(p, description);

    if (author != NULL)
	ann->title = (char *) pdf_strdup(p, author);

    if (mimetype != NULL)
	ann->mimetype = (char *) pdf_strdup(p, mimetype);

    pdf_add_annot(p, ann);
}

PDFLIB_API void PDFLIB_CALL
PDF_add_note(PDF *p, float llx, float lly, float urx, float ury, const char *contents, const char *title, const char *icon, int open)
{
    pdf_annot *ann;

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_add_note)");

    ann = (pdf_annot *) p->malloc(p, sizeof(pdf_annot), "pdf_add_note");

    ann->type	  = ann_text;
    ann->open	  = open;
    ann->rect.llx = llx;
    ann->rect.lly = lly;
    ann->rect.urx = urx;
    ann->rect.ury = ury;

    if (p->compatibility == PDF_1_2 && icon != NULL && *icon != '\0')
	pdf_error(p, PDF_RuntimeError,
		"Note icons are not supported in PDF 1.2");

    if (icon == NULL || *icon == '\0')
	ann->icon = icon_text_note;
    else if (!strcmp(icon, "comment"))
	ann->icon = icon_text_comment;
    else if (!strcmp(icon, "insert"))
	ann->icon = icon_text_insert;
    else if (!strcmp(icon, "note"))
	ann->icon = icon_text_note;
    else if (!strcmp(icon, "paragraph"))
	ann->icon = icon_text_paragraph;
    else if (!strcmp(icon, "newparagraph"))
	ann->icon = icon_text_newparagraph;
    else if (!strcmp(icon, "key"))
	ann->icon = icon_text_key;
    else if (!strcmp(icon, "help"))
	ann->icon = icon_text_help;
    else
	pdf_error(p, PDF_ValueError,
		"Unknown icon type '%s' for text note", icon);
	
    /* title may be NULL */
    if (title != NULL) {
	ann->title = pdf_strdup(p, title);
#ifdef PDFLIB_EBCDIC
	if (!pdf_is_unicode(ann->title))
	    pdf_make_ascii(ann->title);
#endif
    } else
	ann->title = NULL;

    /* It is legal to create an empty text annnotation */
    if (contents != NULL) {
	ann->contents = pdf_strdup(p, contents);
#ifdef PDFLIB_EBCDIC
	if (!pdf_is_unicode(ann->contents))
	    pdf_make_ascii(ann->contents);
#endif
    } else
	ann->contents = NULL;

    pdf_add_annot(p, ann);
}

/* Add a link to another PDF file */
PDFLIB_API void PDFLIB_CALL
PDF_add_pdflink(PDF *p, float llx, float lly, float urx, float ury, const char *filename, int page, const char *desttype)
{
    pdf_annot *ann;

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_add_pdflink)");

    if (filename == NULL)
	pdf_error(p, PDF_ValueError, "NULL filename in PDF_add_pdflink");

    ann = (pdf_annot *) p->malloc(p, sizeof(pdf_annot), "PDF_add_pdflink");

    ann->filename = pdf_strdup(p, filename);

    ann->type	  	= ann_pdflink;
    ann->dest.page 	= page;

    if (desttype == NULL)
	ann->dest.type 	= fitpage;
    else if (!strcmp(desttype, "retain"))
	ann->dest.type 	= retain;
    else if (!strcmp(desttype, "fitpage"))
	ann->dest.type 	= fitpage;
    else if (!strcmp(desttype, "fitwidth"))
	ann->dest.type 	= fitwidth;
    else if (!strcmp(desttype, "fitheight"))
	ann->dest.type 	= fitheight;
    else if (!strcmp(desttype, "fitbbox"))
	ann->dest.type 	= fitbbox;
    else
	pdf_error(p, PDF_ValueError,
		"Unknown destination type '%s' in PDF_add_pdflink", desttype);

    ann->rect.llx = llx;
    ann->rect.lly = lly;
    ann->rect.urx = urx;
    ann->rect.ury = ury;


    pdf_add_annot(p, ann);
}

/* Add a link to another file of an arbitrary type */
PDFLIB_API void PDFLIB_CALL
PDF_add_launchlink(PDF *p, float llx, float lly, float urx, float ury, const char *filename)
{
    pdf_annot *ann;

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_add_launchlink)");

    if (filename == NULL)
	pdf_error(p, PDF_ValueError, "NULL filename in PDF_add_launchlink");

    ann = (pdf_annot *) p->malloc(p, sizeof(pdf_annot), "PDF_add_launchlink");

    ann->filename = pdf_strdup(p, filename);

    ann->type	  = ann_launchlink;

    ann->rect.llx = llx;
    ann->rect.lly = lly;
    ann->rect.urx = urx;
    ann->rect.ury = ury;


    pdf_add_annot(p, ann);
}

/* Add a link to a destination in the current PDF file */
PDFLIB_API void PDFLIB_CALL
PDF_add_locallink(PDF *p, float llx, float lly, float urx, float ury, int page, const char *desttype)
{
    pdf_annot *ann;

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_add_locallink)");

    ann = (pdf_annot *) p->malloc(p, sizeof(pdf_annot), "PDF_add_locallink");

    ann->type	  	= ann_locallink;
    ann->dest.page	= page;

    if (desttype == NULL)
	ann->dest.type 	= fitpage;
    else if (!strcmp(desttype, "retain"))
	ann->dest.type 	= retain;
    else if (!strcmp(desttype, "fitpage"))
	ann->dest.type 	= fitpage;
    else if (!strcmp(desttype, "fitwidth"))
	ann->dest.type 	= fitwidth;
    else if (!strcmp(desttype, "fitheight"))
	ann->dest.type 	= fitheight;
    else if (!strcmp(desttype, "fitbbox"))
	ann->dest.type 	= fitbbox;
    else
	pdf_error(p, PDF_ValueError,	
	    "Unknown destination type '%s' in PDF_add_locallink", desttype);

    ann->rect.llx = llx;
    ann->rect.lly = lly;
    ann->rect.urx = urx;
    ann->rect.ury = ury;

    pdf_add_annot(p, ann);
}

/* Add a link to an arbitrary Internet resource (URL) */
PDFLIB_API void PDFLIB_CALL
PDF_add_weblink(PDF *p, float llx, float lly, float urx, float ury, const char *url)
{
    pdf_annot *ann;

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_add_weblink)");

    if (url == NULL || *url == '\0')
	pdf_error(p, PDF_ValueError, "NULL URL in PDF_add_weblink");

    ann = (pdf_annot *) p->malloc(p, sizeof(pdf_annot), "PDF_add_weblink");

    ann->filename = pdf_strdup(p, url);

    ann->type	  = ann_weblink;
    ann->rect.llx = llx;
    ann->rect.lly = lly;
    ann->rect.urx = urx;
    ann->rect.ury = ury;

    pdf_add_annot(p, ann);
}

PDFLIB_API void PDFLIB_CALL
PDF_set_border_style(PDF *p, const char *style, float width)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (style == NULL)
	p->border_style = border_solid;
    else if (!strcmp(style, "solid"))
	p->border_style = border_solid;
    else if (!strcmp(style, "dashed"))
	p->border_style = border_dashed;
    else
	pdf_error(p, PDF_ValueError,
		"Unknown annotation border style '%s'", style);

    if (width < 0.0)
	pdf_error(p, PDF_ValueError,
		"Negative annotation border width %f", width);

    p->border_width = width;
}

PDFLIB_API void PDFLIB_CALL
PDF_set_border_color(PDF *p, float red, float green, float blue)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (red < 0.0 || red > 1.0)
	pdf_error(p, PDF_ValueError,
		"Bogus red color value %f for annotation border", red);
    if (green < 0.0 || green > 1.0)
	pdf_error(p, PDF_ValueError,
		"Bogus green color value %f for annotation border", green);
    if (blue < 0.0 || blue > 1.0)
	pdf_error(p, PDF_ValueError,
		"Bogus blue color value %f for annotation border", blue);

    p->border_red = red;
    p->border_green = green;
    p->border_blue = blue;
}

PDFLIB_API void PDFLIB_CALL
PDF_set_border_dash(PDF *p, float b, float w)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (b < 0.0)
	pdf_error(p, PDF_ValueError,
		"Negative first annotation border dash value %f", b);
    if (w < 0.0)
	pdf_error(p, PDF_ValueError,
		"Negative second annotation border dash value %f", w);

    p->border_dash1 = b;
    p->border_dash2 = w;
}
