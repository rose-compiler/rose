/* $Id: image.c,v 1.1 2005/10/11 17:18:07 vuduc2 Exp $
 *
 * PDFlib client: image example in C
 */

#include <stdio.h>
#include <stdlib.h>

#include "pdflib.h"

int
main(void)
{
    PDF *p;
    int image;
    char *imagefile = "nesrin.jpg";

    /* This is where font/image/PDF input files live. Adjust as necessary. */
    char *searchpath = "../data";

    /* create a new PDFlib object */
    if ((p = PDF_new()) == (PDF *) 0)
    {
        printf("Couldn't create PDFlib object (out of memory)!\n");
        return(2);
    }

    PDF_TRY(p){
	if (PDF_begin_document(p, "image.pdf", 0, "") == -1) {
	    printf("Error: %s\n", PDF_get_errmsg(p));
	    return(2);
	}

	PDF_set_parameter(p, "SearchPath", searchpath);

	/* This line is required to avoid problems on Japanese systems */
	PDF_set_parameter(p, "hypertextencoding", "host");

	PDF_set_info(p, "Creator", "image.c");
	PDF_set_info(p, "Author", "Thomas Merz");
	PDF_set_info(p, "Title", "image sample (C)");

	image = PDF_load_image(p, "auto", imagefile, 0, "");

	if (image == -1) {
	    printf("Error: %s\n", PDF_get_errmsg(p));
	    return(3);
	}

	/* dummy page size, will be adjusted by PDF_fit_image() */
	PDF_begin_page_ext(p, 10, 10, "");
	PDF_fit_image(p, image, 0.0, 0.0, "adjustpage");
	PDF_close_image(p, image);
	PDF_end_page_ext(p, "");

	PDF_end_document(p, "");
    }

    PDF_CATCH(p) {
        printf("PDFlib exception occurred in image sample:\n");
        printf("[%d] %s: %s\n",
	    PDF_get_errnum(p), PDF_get_apiname(p), PDF_get_errmsg(p));
        PDF_delete(p);
        return(2);
    }

    PDF_delete(p);

    return 0;
}
