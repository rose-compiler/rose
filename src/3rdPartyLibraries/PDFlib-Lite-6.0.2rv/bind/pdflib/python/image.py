#!/usr/bin/python
# $Id: image.py,v 1.1 2005/10/11 17:18:09 vuduc2 Exp $
#
# PDFlib client: image example in Python
#

from sys import *
from pdflib_py import *

imagefile = "nesrin.jpg"

# This is where font/image/PDF input files live. Adjust as necessary.
searchpath = "../data"


p = PDF_new()

if PDF_begin_document(p, "image.pdf", "") == -1:
    print "Error: " + PDF_get_errmsg(p) + "\n"
    exit(2)

PDF_set_parameter(p, "SearchPath", searchpath)

# This line is required to avoid problems on Japanese systems
PDF_set_parameter(p, "hypertextencoding", "winansi")

PDF_set_info(p, "Creator", "image.py")
PDF_set_info(p, "Author", "Thomas Merz")
PDF_set_info(p, "Title", "image sample (Python)")

image = PDF_load_image(p, "auto", imagefile, "")

if image == -1:
    print "Error: " + PDF_get_errmsg(p) + "\n"
    exit(3)

# dummy page size, will be adjusted by PDF_fit_image()
PDF_begin_page_ext(p, 10, 10, "")
PDF_fit_image(p, image, 0, 0, "adjustpage")
PDF_close_image(p, image)
PDF_end_page_ext(p, "")

PDF_end_document(p, "")

PDF_delete(p)
