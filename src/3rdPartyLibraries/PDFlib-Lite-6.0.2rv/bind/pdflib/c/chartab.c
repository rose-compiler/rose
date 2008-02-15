/* $Id: chartab.c,v 1.1 2005/10/11 17:18:07 vuduc2 Exp $
 *
 * PDFlib client: character table in C
 */

#include <stdio.h>
#include <stdlib.h>

#include "pdflib.h"

int
main(void)
{
    /* change these as required */
    const char *fontname = "LuciduxSans-Oblique";

    /* This is where font/image/PDF input files live. Adjust as necessary. */
    char *searchpath = "../data";

    /* list of encodings to use */
    const char *encodings[] = { "iso8859-1", "iso8859-2", "iso8859-15" };

    /* whether or not to embed the font */
    int embed = 1;

#define ENCODINGS ((int) ((sizeof(encodings)/sizeof(encodings[0]))))

    char buf[256];
    double x, y;
    PDF *p;
    int row, col, font, page;

#define FONTSIZE	16
#define TOP		700
#define LEFT		50
#define YINCR		2*FONTSIZE
#define XINCR		2*FONTSIZE

    /* create a new PDFlib object */
    if ((p = PDF_new()) == (PDF *) 0)
    {
        printf("Couldn't create PDFlib object (out of memory)!\n");
        return(2);
    }

    PDF_TRY(p)
    {
	if (PDF_begin_document(p, "chartab.pdf", 0,
		"destination {type fitwindow page 1}") == -1)
	{
	    printf("Error: %s\n", PDF_get_errmsg(p));
	    return(2);
	}

        PDF_set_parameter(p, "fontwarning", "true");

	PDF_set_parameter(p, "SearchPath", searchpath);

	/* This line is required to avoid problems on Japanese systems */
	PDF_set_parameter(p, "hypertextencoding", "host");

	PDF_set_info(p, "Creator", "chartab.c");
	PDF_set_info(p, "Author", "Thomas Merz");
	PDF_set_info(p, "Title", "Character table (C)");

	/* loop over all encodings */
	for (page = 0; page < ENCODINGS; page++)
	{
	    PDF_begin_page_ext(p, a4_width, a4_height, "");

	    /* print the heading and generate the bookmark */
	    /* Change "host" encoding to "winansi" or whatever you need! */
	    font = PDF_load_font(p, "Helvetica", 0, "host", "");
	    PDF_setfont(p, font, FONTSIZE);
	    sprintf(buf, "%s (%s) %sembedded",
		fontname, encodings[page], embed ? "" : "not ");

	    PDF_show_xy(p, buf, LEFT - XINCR, TOP + 3 * YINCR);
	    (void) PDF_create_bookmark(p, buf, 0, "");

	    /* print the row and column captions */
	    PDF_setfont(p, font, 2 * FONTSIZE/3);

	    for (row = 0; row < 16; row++)
	    {
		sprintf(buf, "x%X", row);
		PDF_show_xy(p, buf, LEFT + row*XINCR, TOP + YINCR);

		sprintf(buf, "%Xx", row);
		PDF_show_xy(p, buf, LEFT - XINCR, TOP - row * YINCR);
	    }

	    /* print the character table */
	    font = PDF_load_font(p, fontname, 0, encodings[page],
		embed ? "embedding": "");
	    PDF_setfont(p, font, FONTSIZE);

	    y = TOP;
	    x = LEFT;

	    for (row = 0; row < 16; row++)
	    {
		for (col = 0; col < 16; col++) {
		    sprintf(buf, "%c", 16*row + col);
		    PDF_show_xy(p, buf, x, y);
		    x += XINCR;
		}
		x = LEFT;
		y -= YINCR;
	    }

	    PDF_end_page_ext(p, "");
	}

	PDF_end_document(p, "");
    }
    PDF_CATCH(p)
    {
        printf("PDFlib exception occurred in chartab sample:\n");
        printf("[%d] %s: %s\n",
	    PDF_get_errnum(p), PDF_get_apiname(p), PDF_get_errmsg(p));
        PDF_delete(p);
        return(2);
    }

    PDF_delete(p);

    return 0;
}
