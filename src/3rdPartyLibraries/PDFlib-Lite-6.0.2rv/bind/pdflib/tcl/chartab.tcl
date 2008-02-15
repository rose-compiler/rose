#!/bin/sh
# $Id: chartab.tcl,v 1.1 2005/10/11 17:18:09 vuduc2 Exp $
#
# PDFlib client: character table example in Tcl
#

# Hide the exec to Tcl but not to the shell by appending a backslash\
exec tclsh "$0" ${1+"$@"}

# The lappend line is unnecessary if PDFlib has been installed
# in the Tcl package directory
set auto_path [linsert $auto_path 0 .libs .]

package require pdflib 6.0

# Adjust as you need.
set fontname   "LuciduxSans-Oblique"

#  This is where font/image/PDF input files live. Adjust as necessary
set searchpath "../data"

# list of encodings to use
set encnames   "iso8859-1 iso8859-2 iso8859-15"

set embedflag  "embedding"
set embednote  "embedded"
# or without embedding
#set embedflag  ""
#set embednote  "not embedded"

set fontsize 16
set top 700
set left 50
set xincr [expr {2 * $fontsize}]
set yincr [expr {2 * $fontsize}]

# create a new PDFlib object
set p [PDF_new]

if { [catch {
    if {[PDF_begin_document $p "chartab.pdf" "destination {type fitwindow page 1}"] == -1} {
	puts stderr "Error: [PDF_get_errmsg $p]"
	exit
    }

    PDF_set_parameter $p "fontwarning" "true"
    PDF_set_parameter $p "SearchPath" $searchpath

    PDF_set_info $p "Creator" "chartab.tcl"
    PDF_set_info $p "Author" "Thomas Merz"
    PDF_set_info $p "Title" "Character table (Tcl)"

    # loop over all encodings
    foreach encoding $encnames {

        PDF_begin_page_ext $p 595 842 ""

	# print the heading and generate the bookmark
        set font [PDF_load_font $p "Helvetica" "unicode" ""]
        PDF_setfont $p $font $fontsize

        # title and bookmark
        set text "$fontname ($encoding) $embednote"

        PDF_show_xy $p $text [expr {$left - $xincr}] [expr {$top + 3 * $yincr}]
        PDF_create_bookmark $p $text "" 

        # print the row and column captions
        PDF_setfont $p $font [expr {2 * $fontsize / 3}]

        # character code labels
        for {set row 0} {$row < 16} {incr row} {
            set text [format "x%X" $row]
            PDF_show_xy $p $text \
                [expr {$left + $row * $xincr}] [expr {$top + $yincr}]

            set text [format "%Xx" $row]
            PDF_show_xy $p $text \
                [expr {$left - $xincr}] [expr {$top - $row * $yincr}]
        }

        # print the character table
        set font [PDF_load_font $p $fontname $encoding $embedflag]
        PDF_setfont $p $font $fontsize

        set x $left
        set y $top
        for {set row 0} {$row < 16} {incr row} {
            for {set col 0} {$col < 16} {incr col} {
                set text [format "%c" [expr {int(16*$row + $col)}]]
                PDF_show_xy $p $text $x $y
                incr x $xincr
            }
            set x $left
            incr y -$yincr
        }

        PDF_end_page_ext $p ""
    }

    PDF_end_document $p ""

} result] } {
    puts stderr "$result"
}

PDF_delete $p
