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

/* p_hyper.c
 *
 * PDFlib routines for hypertext stuff: bookmarks, document info, transitions
 *
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "p_intern.h"

/* We can't work with pointers in the outline objects because
 * the complete outline block may be reallocated. Therefore we use
 * this simple mechanism for achieving indirection.
 */
#define COUNT(index)	(p->outlines[index].count)
#define OPEN(index)	(p->outlines[index].open)
#define LAST(index)	(p->outlines[index].last)
#define PARENT(index)	(p->outlines[index].parent)
#define FIRST(index)	(p->outlines[index].first)
#define SELF(index)	(p->outlines[index].self)
#define PREV(index)	(p->outlines[index].prev)
#define NEXT(index)	(p->outlines[index].next)

void
pdf_init_outlines(PDF *p)
{
    p->outline_count	= 0;
}

/* Free outline entries */
void
pdf_cleanup_outlines(PDF *p)
{
    int i;

    if (!p->outlines || p->outline_count == 0)
	return;

    /* outlines[0] is the outline root object */
    for (i = 0; i <= p->outline_count; i++)
	if (p->outlines[i].text)
	    p->free(p, p->outlines[i].text);

    p->free(p, (void*) p->outlines);

    p->outlines = NULL;
}

static void
pdf_write_outline_dict(PDF *p, int entry)
{
    pdf_desttype type;
    long page;

    pdf_begin_obj(p, SELF(entry));	/* outline object */
    pdf_begin_dict(p);

    pdf_printf(p, "/Parent %ld 0 R\n", SELF(PARENT(entry)));

    type = p->outlines[entry].dest.type;
    page = p->pages[p->outlines[entry].dest.page];

    /* outline destination */
    if (type == retain) {
	pdf_printf(p, "/Dest[%ld 0 R/XYZ null null 0]\n", page);
    } else if (type == fitpage) {
	pdf_printf(p, "/Dest[%ld 0 R/Fit]\n", page);
    } else if (type == fitwidth) {
	pdf_printf(p, "/Dest[%ld 0 R/FitH 2000]\n", page);
    } else if (type == fitheight) {
	pdf_printf(p, "/Dest[%ld 0 R/FitV 0]\n", page);
    } else if (type == fitbbox) {
	pdf_printf(p, "/Dest[%ld 0 R/FitB]\n", page);
    } else
	pdf_error(p, PDF_SystemError, "Unknown outline destination type");

    pdf_puts(p, "/Title");	/* outline text */
    pdf_quote_string(p, p->outlines[entry].text);
    pdf_putc(p, PDF_NEWLINE);

    if (PREV(entry))
	pdf_printf(p, "/Prev %ld 0 R\n", PREV(entry));
    if (NEXT(entry))
	pdf_printf(p, "/Next %ld 0 R\n", NEXT(entry));

    if (FIRST(entry)) {
	pdf_printf(p, "/First %ld 0 R\n", SELF(FIRST(entry)));
	pdf_printf(p, "/Last %ld 0 R\n", SELF(LAST(entry)));
    }
    if (COUNT(entry)) {
	if (OPEN(entry))
	    pdf_printf(p, "/Count %d\n", COUNT(entry));	/* open */
	else
	    pdf_printf(p, "/Count %d\n", -COUNT(entry));/* closed */
    }

    pdf_end_dict(p);
    pdf_end_obj(p);			/* outline object */
}

void
pdf_write_outlines(PDF *p)
{
    int i;

    if (p->outline_count == 0)		/* no outlines: return */
	return;

    pdf_begin_obj(p, p->outlines[0].self);	/* root outline object */
    pdf_begin_dict(p);

    if (p->outlines[0].count != 0)
	pdf_printf(p, "/Count %d\n", COUNT(0));
    pdf_printf(p, "/First %ld 0 R\n", SELF(FIRST(0)));
    pdf_printf(p, "/Last %ld 0 R\n", SELF(LAST(0)));

    pdf_end_dict(p);
    pdf_end_obj(p);				/* root outline object */

    for (i = 1; i <= p->outline_count; i++)
	pdf_write_outline_dict(p, i);
}

PDFLIB_API int PDFLIB_CALL
PDF_add_bookmark(PDF *p, const char *text, int parent, int open)
{
    pdf_outline *self;			/* newly created outline */

    if (PDF_SANITY_CHECK_FAILED(p))
	return 0;

    if (text == NULL || *text == '\0')
	pdf_error(p, PDF_ValueError, "Null bookmark text");

    if (p->state != pdf_state_page_description)
	pdf_error(p, PDF_RuntimeError,
		"Wrong order of function calls (PDF_add_bookmark)");

    if (parent < 0 || parent > p->outline_count)
	pdf_error(p, PDF_ValueError,
		"Bad parent id %d for subordinate bookmark", parent);

    /* create the root outline object */
    if (p->outline_count == 0) {
	p->outlines = (pdf_outline *) p->calloc(p, 
	    sizeof(pdf_outline) * OUTLINE_CHUNKSIZE, "PDF_add_bookmark");
	p->outline_capacity = OUTLINE_CHUNKSIZE;

	/* populate the root outline object */
	p->outlines[0].self	= pdf_alloc_id(p);
	p->outlines[0].count	= 0;
	p->outlines[0].parent	= 0;
	p->outlines[0].open	= 1;

	/* set the open mode show bookmarks if we have at least one,
	 * and the client didn't already set his own open mode.
	 */
	if (p->open_mode == open_auto)
	    p->open_mode = open_bookmarks;
    }

    /*
     * It's crucial to increase p->outline_count only after 
     * successfully having realloc()ed. Otherwise the error handler
     * may try to free too much if the realloc goes wrong.
     */
    if (p->outline_count+1 >= p->outline_capacity) { /* used up all space */
	p->outlines = (pdf_outline *) p->realloc(p, p->outlines,
			sizeof(pdf_outline) * 2 * p->outline_capacity,
			"PDF_add_bookmark");
	p->outline_capacity *= 2;
    }

    p->outline_count++;

    self = &p->outlines[p->outline_count];

    /*
     * If the error handler pops in in pdf_strdup below, we don't
     * want to have the cleanup function try to free self->text.
     */
    self->text		= NULL;

    self->text		= pdf_strdup(p, text);
#ifdef PDFLIB_EBCDIC
    if (!pdf_is_unicode(self->text))
        pdf_make_ascii(self->text);
#endif
    self->dest.page	= p->current_page;
    self->dest.type	= p->bookmark_dest;
    self->self		= pdf_alloc_id(p);
    self->first		= 0;
    self->last		= 0;
    self->prev		= 0;
    self->next		= 0;
    self->count		= 0;
    self->open		= open;
    self->parent	= parent;

    /* insert new outline at the end of the chain or start a new chain */
    if (FIRST(parent) == 0) {
	FIRST(parent) = p->outline_count;
    } else {
	self->prev = SELF(LAST(parent));
	NEXT(LAST(parent))= self->self;
    }

    /* insert new outline as last child of parent in all cases */
    LAST(parent) = p->outline_count;

    /* increase the number of open sub-entries for all relevant ancestors */
    do {
	COUNT(parent)++;
    } while (OPEN(parent) && (parent = PARENT(parent)) != 0);

    return (p->outline_count);		/* caller may use this as handle */
}

void
pdf_init_info(PDF *p)
{
    p->Keywords		= NULL;
    p->Subject		= NULL;
    p->Title		= NULL;
    p->Creator		= NULL;
    p->Author		= NULL;
    p->userkey		= NULL;
    p->userval		= NULL;

    p->info_id		= pdf_alloc_id(p);	/* Info object */
}

/* Set Info dictionary entries */
PDFLIB_API void PDFLIB_CALL
PDF_set_info(PDF *p, const char *key, const char *val)
{
    char *key_buf, *val_buf;

    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (key == NULL || !*key || val == NULL || !*val) {
	pdf_error(p, PDF_NonfatalError, "Empty key or value in PDF_set_info");
	return;
    }

    /* this is needed in all cases */
    val_buf = pdf_strdup(p, val);
    key_buf = pdf_strdup(p, key);

#ifdef PDFLIB_EBCDIC
    if (!pdf_is_unicode(val_buf))
	pdf_make_ascii(val_buf);

    /* key_buf must only be converted below after the comparisons are done */
#endif

    if (!strcmp(key_buf, "Keywords")) {
	p->Keywords = val_buf;
    } else if (!strcmp(key_buf, "Subject")) {
	p->Subject = val_buf;
    } else if (!strcmp(key_buf, "Title")) {
	p->Title = val_buf;
    } else if (!strcmp(key_buf, "Creator")) {
	p->Creator = val_buf;
    } else if (!strcmp(key_buf, "Author")) {
	p->Author = val_buf;
    } else {
	/* user-defined keyword */
	p->userval = val_buf;
	p->userkey = key_buf;
	return;
    }

    p->free(p, key_buf);
}

void
pdf_write_info(PDF *p)
{
    time_t	timer;
    struct tm	ltime;

    pdf_begin_obj(p, p->info_id);	/* Info object */

    pdf_begin_dict(p);

    /*
     * Although it would be syntactically correct, we must not remove
     * the space characters after the dictionary keys since this
     * would break the PDF properties feature in Windows Explorer.
     */

    if (p->Keywords) {
	pdf_puts(p, "/Keywords ");
	pdf_quote_string(p, p->Keywords);
	pdf_putc(p, PDF_NEWLINE);
    }
    if (p->Subject) {
	pdf_puts(p, "/Subject ");
	pdf_quote_string(p, p->Subject);
	pdf_putc(p, PDF_NEWLINE);
    }
    if (p->Title) {
	pdf_puts(p, "/Title ");
	pdf_quote_string(p, p->Title);
	pdf_putc(p, PDF_NEWLINE);
    }
    if (p->Creator) {
	pdf_puts(p, "/Creator ");
	pdf_quote_string(p, p->Creator);
	pdf_putc(p, PDF_NEWLINE);
    }
    if (p->Author) {
	pdf_puts(p, "/Author ");
	pdf_quote_string(p, p->Author);
	pdf_putc(p, PDF_NEWLINE);
    }
    if (p->userval) {
	pdf_printf(p, "/%s ", p->userkey);
	pdf_quote_string(p, p->userval);
	pdf_putc(p, PDF_NEWLINE);
    }

    time(&timer);
    ltime = *localtime(&timer);
    pdf_printf(p, "/CreationDate (D:%04d%02d%02d%02d%02d%02d)\n",
	    ltime.tm_year + 1900, ltime.tm_mon + 1, ltime.tm_mday,
	    ltime.tm_hour, ltime.tm_min, ltime.tm_sec);

    /*
     * If you change the /Producer entry your license to use
     * PDFlib will be void!
     */

    if (p->binding)
	pdf_printf(p, "/Producer (PDFlib %s \\(%s/%s\\))\n",
	    PDFLIB_VERSIONSTRING, p->binding, PDF_PLATFORM);
    else
	pdf_printf(p, "/Producer (PDFlib %s \\(%s\\))\n",
	    PDFLIB_VERSIONSTRING, PDF_PLATFORM);

    pdf_end_dict(p);
    pdf_end_obj(p);			/* Info object */
}

void
pdf_cleanup_info(PDF *p)
{
    /* Free Info dictionary entries */
    if (p->Keywords) {
	p->free(p, p->Keywords);
	p->Keywords = NULL;
    }
    if (p->Subject) {
	p->free(p, p->Subject);
	p->Subject = NULL;
    }
    if (p->Title) {
	p->free(p, p->Title);
	p->Title = NULL;
    }
    if (p->Creator) {
	p->free(p, p->Creator);
	p->Creator = NULL;
    }
    if (p->Author) {
	p->free(p, p->Author);
	p->Author = NULL;
    }
    if (p->userkey) {
	p->free(p, p->userkey);
	p->userkey = NULL;
    }
    if (p->userval) {
	p->free(p, p->userval);
	p->userval = NULL;
    }
}

/* Page transition effects */

void
pdf_init_transition(PDF *p)
{
    p->transition = trans_none;
    p->duration = 0;
}

/* set page display duration for current and future pages */
void
pdf_set_duration(PDF *p, float t)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    p->duration = t;
}

/* set transition mode for current and future pages */
void
pdf_set_transition(PDF *p, const char *type)
{
    if (PDF_SANITY_CHECK_FAILED(p))
	return;

    if (type == NULL || *type == '\0')
	p->transition = trans_none;
    else if (!strcmp(type, "none"))
	p->transition = trans_none;
    else if (!strcmp(type, "split"))
	p->transition = trans_split;
    else if (!strcmp(type, "blinds"))
	p->transition = trans_blinds;
    else if (!strcmp(type, "box"))
	p->transition = trans_box;
    else if (!strcmp(type, "wipe"))
	p->transition = trans_wipe;
    else if (!strcmp(type, "dissolve"))
	p->transition = trans_dissolve;
    else if (!strcmp(type, "glitter"))
	p->transition = trans_glitter;
    else if (!strcmp(type, "replace"))
	p->transition = trans_replace;
    else
	pdf_error(p, PDF_ValueError, "Unknown page transition type '%s'", type);
}
