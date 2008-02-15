#!/usr/bin/python
# $Id: hello.py,v 1.1 2005/10/11 17:18:09 vuduc2 Exp $
#
# PDFlib client: hello example in Python
#

from sys import *
from pdflib_py import *

# create a new PDFlib object
p = PDF_new()

if PDF_begin_document(p, "hello.pdf", "") == -1:
    print "Error: " + PDF_get_errmsg(p) + "\n"
    exit(2)

# This line is required to avoid problems on Japanese systems
PDF_set_parameter(p, "hypertextencoding", "winansi")

PDF_set_info(p, "Author", "Thomas Merz")
PDF_set_info(p, "Creator", "hello.py")
PDF_set_info(p, "Title", "Hello world (Python)")

PDF_begin_page_ext(p, 595, 842, "")

font = PDF_load_font(p, "Helvetica-Bold", "winansi", "")

PDF_setfont(p, font, 24)
PDF_set_text_pos(p, 50, 700)
PDF_show(p, "Hello world!")
PDF_continue_text(p, "(says Python)")
PDF_end_page_ext(p, "")

PDF_end_document(p, "")	

PDF_delete(p)
