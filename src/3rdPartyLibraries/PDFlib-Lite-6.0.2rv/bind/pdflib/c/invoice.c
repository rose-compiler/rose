/* $Id: invoice.c,v 1.1 2005/10/11 17:18:07 vuduc2 Exp $
 *
 * PDFlib/PDI client: invoice generation demo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "pdflib.h"

int
main(void)
{
    PDF *	p;
    int		i, stationery, page, regularfont, boldfont;
    char *	infile = "stationery.pdf";

    /* This is where font/image/PDF input files live. Adjust as necessary. */
    char *searchpath = "../data";

    const double left = 55;
    const double right = 530;

    time_t	timer;
    struct tm	ltime;
    double	fontsize = 12, leading, y;
    double	sum, total;
    double	pagewidth = 595, pageheight = 842;
    char	buf[256];
    char	optlist[256];
    char	baseopt[256] =
	"ruler        {   30 45     275   375   475} "
	"tabalignment {right left right right right} "
	"hortabmethod ruler fontsize 12 ";
    int		textflow;
    char	*closingtext =
	"Terms of payment: <fillcolor={rgb 1 0 0}>30 days net. "
	"<fillcolor={gray 0}>90 days warranty starting at the day of sale. "
	"This warranty covers defects in workmanship only. "
	"<fontname=Helvetica-BoldOblique encoding=host>Kraxi Systems, Inc. "
	"<resetfont>will, at its option, repair or replace the "
	"product under the warranty. This warranty is not transferable. "
	"No returns or exchanges will be accepted for wet products.";

    typedef struct { char *name; double price; int quantity; } articledata;

    articledata data[] = {
	{ "Super Kite",		20,	2},
	{ "Turbo Flyer",	40,	5},
	{ "Giga Trash",		180,	1},
	{ "Bare Bone Kit",	50,	3},
	{ "Nitty Gritty",	20,	10},
	{ "Pretty Dark Flyer",	75,	1},
	{ "Free Gift",		0,	1},
    };

#define ARTICLECOUNT (sizeof(data)/sizeof(data[0]))

    static const char *months[] = {
	"January", "February", "March", "April", "May", "June",
	"July", "August", "September", "October", "November", "December"
    };

    /* create a new PDFlib object */
    if ((p = PDF_new()) == (PDF *) 0)
    {
        printf("Couldn't create PDFlib object (out of memory)!\n");
        return(2);
    }

    PDF_TRY(p) {
	if (PDF_begin_document(p, "invoice.pdf", 0, "") == -1) {
	    printf("Error: %s\n", PDF_get_errmsg(p));
	    return(2);
	}

	PDF_set_parameter(p, "SearchPath", searchpath);

	/* This line is required to avoid problems on Japanese systems */
	PDF_set_parameter(p, "hypertextencoding", "host");

	PDF_set_info(p, "Creator", "invoice.c");
	PDF_set_info(p, "Author", "Thomas Merz");
	PDF_set_info(p, "Title", "PDFlib invoice generation demo (C)");

	stationery = PDF_open_pdi(p, infile, "", 0);
	if (stationery == -1) {
	    printf("Error: %s\n", PDF_get_errmsg(p));
	    return(2);
	}

	page = PDF_open_pdi_page(p, stationery, 1, "");
	if (page == -1) {
	    printf("Error: %s\n", PDF_get_errmsg(p));
	    return(2);
	}

	boldfont = PDF_load_font(p, "Helvetica-Bold", 0, "host", "");
	regularfont = PDF_load_font(p, "Helvetica", 0, "host", "");
	leading = fontsize + 2;

	/* Establish coordinates with the origin in the upper left corner. */
	PDF_begin_page_ext(p, pagewidth, pageheight, "topdown");

	PDF_fit_pdi_page(p, page, 0, pageheight, "");
	PDF_close_pdi_page(p, page);

	PDF_setfont(p, regularfont, fontsize);

	/* Print the address */
	y = 170;
	PDF_set_value(p, "leading", leading);

	PDF_show_xy(p, "John Q. Doe", left, y);
	PDF_continue_text(p, "255 Customer Lane");
	PDF_continue_text(p, "Suite B");
	PDF_continue_text(p, "12345 User Town");
	PDF_continue_text(p, "Everland");

	/* Print the header and date */

	PDF_setfont(p, boldfont, fontsize);
	y = 300;
	PDF_show_xy(p, "INVOICE",	left, y);

	time(&timer);
	ltime = *localtime(&timer);
	sprintf(buf, "%s %d, %d",
		    months[ltime.tm_mon], ltime.tm_mday, ltime.tm_year + 1900);
	PDF_fit_textline(p, buf, 0, right, y, "position {100 0}");

	/* Print the invoice header line */
	y = 370;
	sprintf(buf, "\tITEM\tDESCRIPTION\tQUANTITY\tPRICE\tAMOUNT");

	sprintf(optlist, "%s font %d ", baseopt, boldfont);

	textflow = PDF_create_textflow(p, buf, 0, optlist);

	if (textflow == -1)
	{
	    printf("Error: %s\n", PDF_get_errmsg(p));
	    return(2);
	}

	PDF_fit_textflow(p, textflow, left, y-leading, right, y, "");
	PDF_delete_textflow(p, textflow);

	/* Print the article list */

	y += 2*leading;
	total = 0;

	sprintf(optlist, "%s font %d ", baseopt, regularfont);

	for (i = 0; i < (int) ARTICLECOUNT; i++) {
	    sum = data[i].price * data[i].quantity;

	    sprintf(buf, "\t%d\t%s\t%d\t%.2f\t%.2f",
	    	i+1, data[i].name, data[i].quantity, data[i].price, sum);

	    textflow = PDF_create_textflow(p, buf, 0, optlist);

	    if (textflow == -1)
	    {
		printf("Error: %s\n", PDF_get_errmsg(p));
		return(2);
	    }

	    PDF_fit_textflow(p, textflow, left, y-leading, right, y, "");
	    PDF_delete_textflow(p, textflow);

	    y += leading;
	    total += sum;
	}

	y += leading;

	PDF_setfont(p, boldfont, fontsize);
	sprintf(buf, "%.2f", total);
	PDF_fit_textline(p, buf, 0, right, y, "position {100 0}");

	/* Print the closing text */

	y += 5*leading;

	strcpy(optlist, "alignment=justify leading=120% ");
	strcat(optlist, "fontname=Helvetica fontsize=12 encoding=host ");

	textflow = PDF_create_textflow(p, closingtext, 0, optlist);

	if (textflow == -1)
	{
	    printf("Error: %s\n", PDF_get_errmsg(p));
	    return(2);
	}

	PDF_fit_textflow(p, textflow, left, y + 6*leading, right, y, "");
	PDF_delete_textflow(p, textflow);

	PDF_end_page_ext(p, "");
	PDF_end_document(p, "");
	PDF_close_pdi(p, stationery);
    }

    PDF_CATCH(p) {
        printf("PDFlib exception occurred in invoice sample:\n");
        printf("[%d] %s: %s\n",
	    PDF_get_errnum(p), PDF_get_apiname(p), PDF_get_errmsg(p));
        PDF_delete(p);
        return(2);
    }

    PDF_delete(p);

    return 0;
}
