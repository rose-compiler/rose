/* $Id: hello.c,v 1.1 2005/10/11 17:18:07 vuduc2 Exp $
 *
 * PDFlib client: hello example in C
 */

#include <stdio.h>
#include <stdlib.h>

#include "pdflib.h"

int
main(void)
{
    PDF *p;
    int font;

    /* create a new PDFlib object */
    if ((p = PDF_new()) == (PDF *) 0)
    {
        printf("Couldn't create PDFlib object (out of memory)!\n");
        return(2);
    }

    PDF_TRY(p) {
	if (PDF_begin_document(p, "hello.pdf", 0, "") == -1) {
	    printf("Error: %s\n", PDF_get_errmsg(p));
	    return(2);
	}

	/* This line is required to avoid problems on Japanese systems */
	PDF_set_parameter(p, "hypertextencoding", "host");

	PDF_set_info(p, "Creator", "hello.c");
	PDF_set_info(p, "Author", "Thomas Merz");
	PDF_set_info(p, "Title", "Hello, world (C)!");

	PDF_begin_page_ext(p, a4_width, a4_height, "");

	/* Change "host" encoding to "winansi" or whatever you need! */
	font = PDF_load_font(p, "Helvetica-Bold", 0, "host", "");

	PDF_setfont(p, font, 24);
	PDF_set_text_pos(p, 50, 700);
	PDF_show(p, "Hello, world!");
	PDF_continue_text(p, "(says C)");
	PDF_end_page_ext(p, "");

	PDF_end_document(p, "");
    }

    PDF_CATCH(p) {
        printf("PDFlib exception occurred in hello sample:\n");
        printf("[%d] %s: %s\n",
	    PDF_get_errnum(p), PDF_get_apiname(p), PDF_get_errmsg(p));
        PDF_delete(p);
        return(2);
    }

    PDF_delete(p);

    return 0;
}
