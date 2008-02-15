#!/bin/sh
# $Id: businesscard.tcl,v 1.1 2005/10/11 17:18:09 vuduc2 Exp $
#
# PDFlib client: block processing example in tcl
#

# Hide the exec to Tcl but not to the shell by appending a backslash\
exec tclsh "$0" ${1+"$@"}

# The lappend line is unnecessary if PDFlib has been installed
# in the Tcl package directory
set auto_path [linsert $auto_path 0 .libs .]

package require pdflib 6.0

set infile "boilerplate.pdf"

global blockName blockValue

proc block_add {id name value} {
    global blockName blockValue
    set blockName($id) $name
    set blockValue($id) $value
}

block_add 0 "name"                    "Victor Kraxi"
block_add 1 "business.title"          "Chief Paper Officer"
block_add 2 "business.address.line1"  "17, Aviation Road"
block_add 3 "business.address.city"   "Paperfield"
block_add 4 "business.telephone.voice" "phone +1 234 567-89"
block_add 5 "business.telephone.fax"  "fax +1 234 567-98"
block_add 6 "business.email"          "victor@kraxi.com"
block_add 7 "business.homepage"       "www.kraxi.com"

set BLOCKCOUNT 8

set p [PDF_new]

if {[PDF_begin_document $p "businesscard.pdf" ""] == -1} {
    puts stderr "Error: [PDF_get_errmsg $p]"
    exit
}

# Set the search path for fonts and PDF files
PDF_set_parameter $p "SearchPath" "../data"

PDF_set_info $p "Creator" "businesscard.tcl"
PDF_set_info $p "Author" "Thomas Merz"
PDF_set_info $p "Title" "PDFlib block processing sample (Tcl)"

set blockcontainer [PDF_open_pdi $p $infile "" 0]
if {$blockcontainer == -1} {
    puts stderr "Error: % [PDF_get_errmsg $p]"
    exit
}

set page [PDF_open_pdi_page $p $blockcontainer 1 ""]
if {$page == -1} {
    puts stderr "Error: [PDF_get_errmsg $p]"
    exit
}

# dummy page size
PDF_begin_page_ext $p 20 20 ""

# This will adjust the page size to the block container's size.
PDF_fit_pdi_page $p $page 0 0 "adjustpage"

# Fill all text blocks with dynamic data
for { set i 0} {$i < $BLOCKCOUNT} {set i [expr $i + 1]} {
    if {[PDF_fill_textblock $p $page $blockName($i) $blockValue($i) \
        "embedding encoding=unicode"] == -1} {
	puts stderr "Warning: [PDF_get_errmsg $p]"
    }
}

PDF_end_page_ext $p ""
PDF_close_pdi_page $p $page

# close PDF document
PDF_end_document $p ""
PDF_close_pdi $p $blockcontainer

# delete the PDFlib object
PDF_delete $p
