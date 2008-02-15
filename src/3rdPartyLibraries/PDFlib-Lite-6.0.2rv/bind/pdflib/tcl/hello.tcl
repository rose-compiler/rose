#!/bin/sh
# $Id: hello.tcl,v 1.1 2005/10/11 17:18:09 vuduc2 Exp $
#
# PDFlib client: hello example in Tcl
#

# Hide the exec to Tcl but not to the shell by appending a backslash\
exec tclsh "$0" ${1+"$@"}

# The lappend line is unnecessary if PDFlib has been installed
# in the Tcl package directory
set auto_path [linsert $auto_path 0 .libs .]

package require pdflib 6.0

set p [PDF_new]

if {[PDF_begin_document $p "hello.pdf" ""] == -1} {
    puts stderr "Error:  [PDF_get_errmsg $p]"
    exit
}

PDF_set_info $p "Creator" "hello.tcl"
PDF_set_info $p "Author" "Thomas Merz"
PDF_set_info $p "Title" "Hello world (Tcl)"

PDF_begin_page_ext $p 595 842 ""

set font [PDF_load_font $p "Helvetica-Bold" "unicode" ""]

PDF_setfont $p $font 24.0
PDF_set_text_pos $p 50 700
PDF_show $p "Hello world!"
PDF_continue_text $p "(says Tcl)"
PDF_end_page_ext $p ""

PDF_end_document $p ""

PDF_delete $p
