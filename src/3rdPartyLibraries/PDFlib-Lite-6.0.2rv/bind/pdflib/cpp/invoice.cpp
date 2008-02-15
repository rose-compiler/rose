// $Id: invoice.cpp,v 1.2 2008/01/08 02:56:19 dquinlan Exp $
//
// PDFlib client: invoice example in C++
//

#include <iostream>

#include <time.h>
#include <string.h>

#if !defined(WIN32) && !defined(MAC)
#include <unistd.h>
#endif

#include "pdflib.hpp"

int
main(void)
{
    try {
	int         i, stationery, page, regularfont, boldfont;
	string      infile = "stationery.pdf";
	// This is where font/image/PDF input files live. Adjust as necessary.
	string      searchpath = "../data";
	const double left = 55;
	const double right = 530;
	time_t      timer;
	struct tm   ltime;
	double      fontsize = 12, leading, y;
	double      sum, total;
	double      pagewidth = 595, pageheight = 842;
	char        buf[256];
	char        optlist[256];
	char        baseopt[256] =
	    "ruler        {   30 45     275   375   475} "
	    "tabalignment {right left right right right} "
	    "hortabmethod ruler fontsize 12 ";
	int         textflow;
	PDFlib      p;
	string      closingtext =
	    "Terms of payment: <fillcolor={rgb 1 0 0}>30 days net. "
	    "<fillcolor={gray 0}>90 days warranty starting at the day of sale. "
	    "This warranty covers defects in workmanship only. "
	    "<fontname=Helvetica-BoldOblique encoding=host>Kraxi Systems, Inc. "
	    "<resetfont>will, at its option, repair or replace the "
	    "product under the warranty. This warranty is not transferable. "
	    "No returns or exchanges will be accepted for wet products.";

	struct articledata {
	    articledata(string n, double pr, int q):
		name(n), price(pr), quantity(q){}
	    string name;
	    double price;
	    int quantity;
	};

	articledata data[] = {
	    articledata("Super Kite",         20,     2),
	    articledata("Turbo Flyer",        40,     5),
	    articledata("Giga Trash",         180,    1),
	    articledata("Bare Bone Kit",      50,     3),
	    articledata("Nitty Gritty",       20,     10),
	    articledata("Pretty Dark Flyer",  75,     1),
	    articledata("Free Gift",          0,      1),
	};

#define ARTICLECOUNT (sizeof(data)/sizeof(data[0]))

	static const string months[] = {
	    "January", "February", "March", "April", "May", "June",
	    "July", "August", "September", "October", "November", "December"
	};


        if (p.begin_document("invoice.pdf", "") == -1) {
	    cerr << "Error: " << p.get_errmsg() << endl;
            return(2);
        }

	p.set_parameter("SearchPath", searchpath);

	// This line is required to avoid problems on Japanese systems
	p.set_parameter("hypertextencoding", "host");

        p.set_info("Creator", "invoice.cpp");
        p.set_info("Author", "Thomas Merz");
        p.set_info("Title", "PDFlib invoice generation demo (C++)");

        stationery = p.open_pdi(infile, "", 0);
        if (stationery == -1) {
	    cerr << "Error: " << p.get_errmsg() << endl;
            return(2);
        }

        page = p.open_pdi_page(stationery, 1, "");
        if (page == -1) {
	    cerr << "Error: " << p.get_errmsg() << endl;
            return(2);
        }

        boldfont = p.load_font("Helvetica-Bold", "host", "");
        regularfont = p.load_font("Helvetica", "host", "");
        leading = fontsize + 2;

        // Establish coordinates with the origin in the upper left corner.
        p.begin_page_ext(pagewidth, pageheight, "topdown");

        p.fit_pdi_page(page, 0, pageheight, "");
        p.close_pdi_page(page);

        p.setfont(regularfont, 12);

        // Print the address
        y = 170;
        p.set_value("leading", leading);

        p.show_xy("John Q. Doe", left, y);
        p.continue_text("255 Customer Lane");
        p.continue_text("Suite B");
        p.continue_text("12345 User Town");
        p.continue_text("Everland");

        // Print the header and date

        p.setfont(boldfont, 12);
        y = 300;
        p.show_xy("INVOICE",       left, y);

        time(&timer);
        ltime = *localtime(&timer);
        sprintf(buf, "%s %d, %d", months[ltime.tm_mon].c_str(),
			    ltime.tm_mday, ltime.tm_year + 1900);
        p.fit_textline(buf, right, y, "position {100 0}");

        // Print the invoice header line
        p.setfont(boldfont, 12);

        // "position {0 0}" is left-aligned, "position {100 0}" right-aligned
        y = 370;
	sprintf(buf, "\tITEM\tDESCRIPTION\tQUANTITY\tPRICE\tAMOUNT");

	sprintf(optlist, "%s font %d ", baseopt, boldfont);

	textflow = p.create_textflow(buf, optlist);
        if (textflow == -1) {
	    cerr << "Error: " << p.get_errmsg() << endl;
            return(2);
	}
 
	p.fit_textflow(textflow, left, y-leading, right, y, "");
	p.delete_textflow(textflow);

        // Print the article list

        p.setfont(regularfont, 12);
        y += 2*leading;
        total = 0;

	sprintf(optlist, "%s font %d ", baseopt, regularfont);

        for (i = 0; i < (int)ARTICLECOUNT; i++) {
            sum = data[i].price * data[i].quantity;

	    sprintf(buf, "\t%d\t%s\t%d\t%.2f\t%.2f", 
		i+1, (char *)data[i].name.c_str(), data[i].quantity, 
		data[i].price, sum);

	    textflow = p.create_textflow(buf, optlist);
	    if (textflow == -1) {
		cerr << "Error: " << p.get_errmsg() << endl;
		return(2);
	    }
	    p.fit_textflow(textflow, left, y-leading, right, y, "");
	    p.delete_textflow(textflow);

            y += leading;
            total += sum;
        }

        y += leading;
        p.setfont(boldfont, 12);
        sprintf(buf, "%.2f", total);
        p.fit_textline(buf, right, y, "position {100 0}");

        // Print the closing text

        y += 5*leading;
	strcpy(optlist, "alignment=justify leading=120% ");
	strcat(optlist, "fontname=Helvetica fontsize=12 encoding=host ");

	textflow = p.create_textflow(closingtext, optlist);
	if (textflow == -1) {
	    cerr << "Error: " << p.get_errmsg() << endl;
	    return(2);
	}
	p.fit_textflow(textflow, left, y+6*leading, right, y, "");
	p.delete_textflow(textflow);

        p.end_page_ext("");
        p.end_document("");
        p.close_pdi(stationery);
    }

    catch (PDFlib::Exception &ex) {
	cerr << "PDFlib exception occurred in invoice sample: " << endl;
	cerr << "[" << ex.get_errnum() << "] " << ex.get_apiname()
	    << ": " << ex.get_errmsg() << endl;
	return 2;
    }

    return 0;
}
