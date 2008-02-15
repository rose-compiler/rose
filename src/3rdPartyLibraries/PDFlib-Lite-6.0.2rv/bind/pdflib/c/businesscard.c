/* $Id: businesscard.c,v 1.1 2005/10/11 17:18:07 vuduc2 Exp $
 *
 * PDFlib client: block processing example in C
 */

#include <stdio.h>
#include <stdlib.h>

#include "pdflib.h"

int
main(void)
{
    PDF 	*p;
    int 	i, blockcontainer, page;
    char 	*infile = "boilerplate.pdf";

    /*
     * This is where font/image/PDF input files live. Adjust as necessary.
     *
     * Note that this directory must also contain the LuciduxSans font outline
     * and metrics files.
     */
    char *searchpath = "../data";

    typedef struct { char *name; char *value; } blockdata;

    blockdata data[] = {
    { "name",			"Victor Kraxi" },
    { "business.title",		"Chief Paper Officer" },
    { "business.address.line1",	"17, Aviation Road" },
    { "business.address.city",	"Paperfield" },
    { "business.telephone.voice","phone +1 234 567-89" },
    { "business.telephone.fax",	"fax +1 234 567-98" },
    { "business.email",		"victor@kraxi.com" },
    { "business.homepage",	"www.kraxi.com" },
    };

#define BLOCKCOUNT (sizeof(data)/sizeof(data[0]))

    /* create a new PDFlib object */
    if ((p = PDF_new()) == (PDF *) 0)
    {
        printf("Couldn't create PDFlib object (out of memory)!\n");
        return(2);
    }

    PDF_TRY(p) {
	if (PDF_begin_document(p, "businesscard.pdf", 0, "") == -1) {
	    printf("Error: %s\n", PDF_get_errmsg(p));
	    return(2);
	}

	/* Set the search path for fonts and PDF files */
	PDF_set_parameter(p, "SearchPath", searchpath);

	/* This line is required to avoid problems on Japanese systems */
	PDF_set_parameter(p, "hypertextencoding", "host");

	PDF_set_info(p, "Creator", "businesscard.c");
	PDF_set_info(p, "Author", "Thomas Merz");
	PDF_set_info(p, "Title","PDFlib block processing sample (C)");

	blockcontainer = PDF_open_pdi(p, infile, "", 0);
	if (blockcontainer == -1) {
	    printf("Error: %s\n", PDF_get_errmsg(p));
	    return(2);
        }

	page = PDF_open_pdi_page(p, blockcontainer, 1, "");
	if (page == -1) {
	    printf("Error: %s\n", PDF_get_errmsg(p));
	    return(2);
	}

	PDF_begin_page_ext(p, 20, 20, "");		/* dummy page size */

	/* This will adjust the page size to the block container's size. */
	PDF_fit_pdi_page(p, page, 0, 0, "adjustpage");

	/* Fill all text blocks with dynamic data */
	for (i = 0; i < (int) BLOCKCOUNT; i++) {
	    if (PDF_fill_textblock(p, page, data[i].name, data[i].value, 0,
		"embedding encoding=host") == -1) {
		printf("Warning: %s\n", PDF_get_errmsg(p));
	    }
	}

	PDF_end_page_ext(p, "");
	PDF_close_pdi_page(p, page);

	PDF_end_document(p, "");
	PDF_close_pdi(p, blockcontainer);
    }

    PDF_CATCH(p) {
        printf("PDFlib exception occurred in businesscard sample:\n");
        printf("[%d] %s: %s\n",
	    PDF_get_errnum(p), PDF_get_apiname(p), PDF_get_errmsg(p));
        PDF_delete(p);
        return(2);
    }

    PDF_delete(p);

    return 0;
}
