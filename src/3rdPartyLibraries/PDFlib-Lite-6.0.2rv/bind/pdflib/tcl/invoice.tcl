#!/bin/sh
# $Id: invoice.tcl,v 1.1 2005/10/11 17:18:09 vuduc2 Exp $
#
# PDFlib client: invoice generation demo
#

# Hide the exec to Tcl but not to the shell by appending a backslash\
exec tclsh "$0" ${1+"$@"}

# The lappend line is unnecessary if PDFlib has been installed
# in the Tcl package directory
set auto_path [linsert $auto_path 0 .libs .]

package require pdflib 6.0

set infile "stationery.pdf"

# This is where font/image/PDF input files live. Adjust as necessary.
set searchpath  "../data"

set left 55.0
set right 530.0

set fontsize 12.0
set pagewidth 595.0
set pageheight 842.0

set baseopt \
"ruler        \{   30 45     275   375   475\} \
tabalignment \{right left right right right\} \
hortabmethod ruler fontsize 12 "

set closingtext  \
"Terms of payment: <fillcolor={rgb 1 0 0}>30 days net.\
<fillcolor={gray 0}>90 days warranty starting at the day of sale.\
This warranty covers defects in workmanship only.\
<fontname=Helvetica-BoldOblique encoding=unicode>Kraxi Systems, Inc.\
<resetfont>will, at its option, repair or replace the\
product under the warranty. This warranty is not transferable.\
No returns or exchanges will be accepted for wet products."

global articleName articlePrice articleQuantity

proc article_add {id name price quantity} {
global articleName articlePrice articleQuantity
set articleName($id) $name
set articlePrice($id) $price
set articleQuantity($id) $quantity
}

article_add 0 "Super Kite"		20	2
article_add 1 "Turbo Flyer"		40	5
article_add 2 "Giga Trash"		180	1
article_add 3 "Bare Bone Kit"		50	3
article_add 4 "Nitty Gritty"		20	10
article_add 5 "Pretty Dark Flyer"	75	1
article_add 6 "Free Gift"		0	1

set ARTICLECOUNT 7

# create a new PDFlib object
set p [PDF_new]

if {[PDF_begin_document $p "invoice.pdf" ""] == -1} {
    puts stderr "Error: [PDF_get_errmsg $p]"
    exit
}

PDF_set_parameter $p "SearchPath" $searchpath

PDF_set_info $p "Creator" "invoice.tcl"
PDF_set_info $p "Author" "Thomas Merz"
PDF_set_info $p "Title" "PDFlib invoice generation demo (TCL)"

set stationery [PDF_open_pdi $p $infile "" 0]
if {$stationery == -1} {
    puts stderr "Error: [PDF_get_errmsg $p]"
    exit
}

set page [PDF_open_pdi_page $p $stationery 1 ""]
if {$page == -1} {
    puts stderr "Error: [PDF_get_errmsg $p]"
    exit
}

set boldfont [PDF_load_font $p "Helvetica-Bold" "unicode" ""]
set regularfont [PDF_load_font $p "Helvetica" "unicode" ""]
set leading [expr $fontsize + 2]

# Establish coordinates with the origin in the upper left corner.
PDF_begin_page_ext $p $pagewidth $pageheight "topdown"

PDF_fit_pdi_page $p $page 0 $pageheight ""
PDF_close_pdi_page $p $page

PDF_setfont $p $regularfont $fontsize

# Print the address
set y 170
PDF_set_value $p "leading" $leading

PDF_show_xy $p "John Q. Doe" $left $y
PDF_continue_text $p "255 Customer Lane"
PDF_continue_text $p "Suite B"
PDF_continue_text $p "12345 User Town"
PDF_continue_text $p "Everland"

# Print the header and date

PDF_setfont $p $boldfont $fontsize
set y 300
PDF_show_xy $p "INVOICE" $left $y

set buf [clock format [clock seconds] -format "%B %d, %Y"]
PDF_fit_textline $p $buf $right $y "position {100 0}"

# Print the invoice header line

set y 370
set buf "\tITEM\tDESCRIPTION\tQUANTITY\tPRICE\tAMOUNT"

set optlist [format "%s font %d" $baseopt $boldfont]

set textflow [PDF_create_textflow $p $buf $optlist]

if {$textflow == -1} {
    puts stderr "Error: [PDF_get_errmsg $p]"
    exit
}

PDF_fit_textflow $p $textflow $left [expr $y-$leading] $right $y ""
PDF_delete_textflow $p $textflow

# Print the article list

set y [expr $y + 2*$leading]
set total 0

set optlist [format "%s font %d" $baseopt $regularfont]

for {set i 0} {$i < $ARTICLECOUNT} {set i [expr $i + 1]} {
    set sum [ expr $articlePrice($i) * $articleQuantity($i)]

    set buf [format "\t%d\t%s\t%d\t%.2f\t%.2f" [expr $i+1] $articleName($i) \
		$articleQuantity($i) $articlePrice($i) $sum]

    set textflow [PDF_create_textflow $p $buf $optlist]

    if {$textflow == -1} {
	puts stderr "Error: [PDF_get_errmsg $p]"
	exit
    }

    PDF_fit_textflow $p $textflow $left [expr $y-$leading] $right $y ""
    PDF_delete_textflow $p $textflow

    set y [expr $y + $leading]
    set total [expr $total + $sum]
}

set y [expr $y + $leading]
PDF_setfont $p $boldfont $fontsize
set buf [format "%.2f" $total]
PDF_fit_textline $p $buf $right $y "position {100 0}"

# Print the closing text

set y [expr $y + 5*$leading]

set optlist "alignment=justify leading=120% "
append optlist "fontname=Helvetica fontsize=12 encoding=unicode "

set textflow [PDF_create_textflow $p $closingtext $optlist]

if {$textflow == -1} {
    puts stderr "Error: [PDF_get_errmsg $p]"
    exit
}

PDF_fit_textflow $p $textflow $left [expr $y+6*$leading] $right $y ""
PDF_delete_textflow $p $textflow

PDF_end_page_ext $p ""
PDF_end_document $p ""
PDF_close_pdi $p $stationery

PDF_delete $p
