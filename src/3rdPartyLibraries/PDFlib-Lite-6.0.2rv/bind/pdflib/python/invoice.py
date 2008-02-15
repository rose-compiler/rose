#!/usr/bin/python
# $Id: invoice.py,v 1.1 2005/10/11 17:18:09 vuduc2 Exp $
#
# PDFlib client: invoice generation demo
#

from sys import *
import time
import fpformat
from pdflib_py import *

infile = "stationery.pdf"

# This is where font/image/PDF input files live. Adjust as necessary.
searchpath = "../data"

left = 55
right = 530
fontsize = 12
pagewidth = 595
pageheight = 842
baseopt = \
        "ruler        {   30 45     275   375   475} " +\
	"tabalignment {right left right right right} " +\
	"hortabmethod ruler fontsize 12 "

closingtext = \
    "Terms of payment: <fillcolor={rgb 1 0 0}>30 days net. " +\
    "<fillcolor={gray 0}>90 days warranty starting at the day of sale. " +\
    "This warranty covers defects in workmanship only. " +\
    "<fontname=Helvetica-BoldOblique encoding=host>Kraxi Systems, Inc. " +\
    "<resetfont>will, at its option, repair or replace the " +\
    "product under the warranty. This warranty is not transferable. " +\
    "No returns or exchanges will be accepted for wet products."

data_name = [
 "Super Kite",
 "Turbo Flyer",
 "Giga Trash",
 "Bare Bone Kit",
 "Nitty Gritty",
 "Pretty Dark Flyer",
 "Free Gift" ]
data_price = [ 20, 40, 180, 50, 20, 75, 0]
data_quantity = [ 2, 5, 1, 3, 10, 1, 1]


ARTICLECOUNT = 6

p = PDF_new()

if PDF_begin_document(p, "invoice.pdf", "") == -1:
    print "Error: " + PDF_get_errmsg(p) + "\n"
    exit(2)

PDF_set_parameter(p, "SearchPath", searchpath)

# This line is required to avoid problems on Japanese systems
PDF_set_parameter(p, "hypertextencoding", "winansi")

PDF_set_info(p, "Creator", "invoice.py")
PDF_set_info(p, "Author", "Thomas Merz")
PDF_set_info(p, "Title", "PDFlib invoice generation demo (Python)")

stationery = PDF_open_pdi(p, infile, "", 0)
if stationery == -1:
    print "Error: " + PDF_get_errmsg(p) + "\n"
    exit(2)

page = PDF_open_pdi_page(p, stationery, 1, "")
if page == -1:
    print "Error: " + PDF_get_errmsg(p) + "\n"
    exit(2)

boldfont = PDF_load_font(p, "Helvetica-Bold", "winansi", "")
regularfont = PDF_load_font(p, "Helvetica", "winansi", "")
leading = fontsize + 2

# Establish coordinates with the origin in the upper left corner.
PDF_begin_page_ext(p, pagewidth, pageheight, "topdown")

PDF_fit_pdi_page(p, page, 0, pageheight, "")
PDF_close_pdi_page(p, page)

PDF_setfont(p, regularfont, fontsize)

# Print the address
y = 170
PDF_set_value(p, "leading", leading)

PDF_show_xy(p, "John Q. Doe", left, y)
PDF_continue_text(p, "255 Customer Lane")
PDF_continue_text(p, "Suite B")
PDF_continue_text(p, "12345 User Town")
PDF_continue_text(p, "Everland")

# Print the header and date

PDF_setfont(p, boldfont, fontsize)
y = 300
PDF_show_xy(p, "INVOICE",	left, y)

buf = time.strftime("%x", time.gmtime(time.time()))
PDF_fit_textline(p, buf, right, y, "position {100 0}")

# Print the invoice header line
PDF_setfont(p, boldfont, fontsize)

# "position {0 0}" is left-aligned, "position {100 0}" right-aligned
y = 370
buf = "\tITEM\tDESCRIPTION\tQUANTITY\tPRICE\tAMOUNT"
optlist = baseopt + " font " + repr(boldfont)

textflow = PDF_create_textflow(p, buf, optlist);

if textflow == -1:
    print "Error: " + PDF_get_errmsg(p) + "\n"
    exit(2)

PDF_fit_textflow(p, textflow, left, y-leading, right, y, "");
PDF_delete_textflow(p, textflow);

# Print the article list

y += 2*leading
total = 0

optlist = baseopt + " font " + repr(regularfont)

for i in range(0, ARTICLECOUNT, 1):
    sum = data_price[i] * data_quantity[i]

    buf = "\t" + repr(i+1) + "\t" + data_name[i] + "\t" + \
	repr(data_quantity[i]) + "\t" + fpformat.fix(data_price[i], 2) + \
	"\t" + fpformat.fix(sum, 2)

    textflow = PDF_create_textflow(p, buf, optlist)

    if textflow == -1:
	print "Error: " + PDF_get_errmsg(p) + "\n"
	exit(2)

    PDF_fit_textflow(p, textflow, left, y-leading, right, y, "")
    PDF_delete_textflow(p, textflow)

    y += leading;
    total += sum;


y += leading

PDF_setfont(p, boldfont, fontsize)
buf =  fpformat.fix(total, 2)
PDF_fit_textline(p, buf, right, y, "position {100 0}")

# Print the closing text

y += 5*leading

optlist = "alignment=justify leading=120% " + \
 "fontname=Helvetica fontsize=12 encoding=host "

textflow = PDF_create_textflow(p, closingtext, optlist)

if textflow == -1:
    print "Error: " + PDF_get_errmsg(p) + "\n"
    exit(2)

PDF_fit_textflow(p, textflow, left, y + 6*leading, right, y, "")
PDF_delete_textflow(p, textflow)

PDF_end_page_ext(p, "")
PDF_end_document(p, "")
PDF_close_pdi(p, stationery)

PDF_delete(p)
