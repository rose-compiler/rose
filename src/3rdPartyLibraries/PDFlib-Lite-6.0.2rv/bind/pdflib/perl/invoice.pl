#!/usr/bin/perl
# $Id: invoice.pl,v 1.1 2005/10/11 17:18:09 vuduc2 Exp $
#
# PDFlib client: invoice generation demo
#

use pdflib_pl 6.0;

$left = 55;
$right = 530;
$fontsize = 12;
$pagewidth = 595;
$pageheight = 842;
$fontsize = 12;
$searchpath = "../data";
$infile = "stationery.pdf";
$baseopt =  "ruler        {   30 45     275   375   475} " .
	    "tabalignment {right left right right right} " . 
	    "hortabmethod ruler fontsize 12 ";

# This is where font/image/PDF input files live. Adjust as necessary.
$closingtext = 
	"Terms of payment: <fillcolor={rgb 1 0 0}>30 days net. " .
	"<fillcolor={gray 0}>90 days warranty starting at the day of sale. " .
	"This warranty covers defects in workmanship only. " .
	"<fontname=Helvetica-BoldOblique encoding=host>Kraxi Systems, Inc. " . 
	"<resetfont>will, at its option, repair or replace the " .
	"product under the warranty. This warranty is not transferable. " .
	"No returns or exchanges will be accepted for wet products.";

@data = (   {name=>"Super Kite", 	price=>20,	quantity=>2},
	    {name=>"Turbo Flyer", 	price=>40, 	quantity=>5},
	    {name=>"Giga Trasch", 	price=>180, 	quantity=>1},
	    {name=>"Bare Bone Kit", 	price=>50, 	quantity=>3},
	    {name=>"Nitty Gritty", 	price=>20, 	quantity=>10},
	    {name=>"Pretty Dark Flyer",	price=>75, 	quantity=>1},
	    {name=>"Free Gift", 	price=>0, 	quantity=>1}
	);

@months = ( "January", "February", "March", "April", "May", "June",
	    "July", "August", "September", "October", "November", "December");

$p = PDF_new();

eval {
    if (PDF_begin_document($p, "invoice.pdf", "") == -1){
	printf("Error: %s\n", PDF_get_errmsg($p));
	exit;
    }

    PDF_set_parameter($p, "SearchPath", $searchpath);

    # This line is required to avoid problems on Japanese systems
    PDF_set_parameter($p, "hypertextencoding", "winansi");

    PDF_set_info($p, "Creator", "invoice.pl");
    PDF_set_info($p, "Author", "Thomas Merz");
    PDF_set_info($p, "Title", "PDFlib invoice generation demo (Perl)");

    $stationery = PDF_open_pdi($p, $infile, "", 0);
    if ($stationery == -1){
	printf("Error: %s\n", PDF_get_errmsg($p));
	exit;
    }

    $page = PDF_open_pdi_page($p, $stationery, 1, "");
    if ($page == -1){
	printf("Error: %s\n", PDF_get_errmsg($p));
	exit;
    }


    $boldfont = PDF_load_font($p, "Helvetica-Bold", "winansi", "");
    $regularfont = PDF_load_font($p, "Helvetica", "winansi", "");
    $leading = $fontsize + 2;

    # Establish coordinates with the origin in the upper left corner.
    PDF_set_parameter($p, "topdown", "true");

    PDF_begin_page_ext($p, $pagewidth, $pageheight, "");	# A4 page

    PDF_fit_pdi_page($p, $page, 0, $pageheight, "");
    PDF_close_pdi_page($p, $page);

    PDF_setfont($p, $regularfont, $fontsize);

    # print the address
    $y = 170;
    PDF_set_value($p, "leading", $leading);

    PDF_show_xy($p, "John Q. Doe", $left, $y);
    PDF_continue_text($p, "255 Customer Lane");
    PDF_continue_text($p, "Suite B");
    PDF_continue_text($p, "12345 User Town");
    PDF_continue_text($p, "Everland");

    # print the header and date

    PDF_setfont($p, $boldfont, $fontsize);
    $y = 300;
    PDF_show_xy($p, "INVOICE", $left, $y);

    $buf = sprintf("%s %d, %d", $months[(localtime)[4]], (localtime)[3],
    (localtime)[5]+1900);
    PDF_fit_textline($p, $buf, $right, $y, "position {100 0}");


    # print the invoice header line
    $y = 370;
    $buf = sprintf("\tITEM\tDESCRIPTION\tQUANTITY\tPRICE\tAMOUNT"); 
    $optlist = sprintf("%s font %d", $baseopt, $boldfont);

    $textflow = PDF_create_textflow($p, $buf, $optlist);

    if ($textflow == -1){
	printf("Error: %s\n", PDF_get_errmsg($p));
	exit;
    }

    PDF_fit_textflow($p, $textflow, $left, $y-$leading, $right, $y, "");
    PDF_delete_textflow($p, $textflow);

    # print the article list

    $y += 2*$leading;
    $total = 0;

    $optlist = sprintf("%s font %d", $baseopt, $regularfont);

    for ($i = 0; $i <= $#data; $i++){
	$sum = $data[$i]{price}*$data[$i]{quantity};

	$buf = sprintf("\t%d\t%s\t%d\t%.2f\t%.2f",
	    $i+1, $data[$i]{name}, $data[$i]{quantity}, $data[$i]{price}, $sum);

	$textflow = PDF_create_textflow($p, $buf, $optlist);

	if ($textflow == -1){
	    printf("Error: %s\n", PDF_get_errmsg($p));
	    exit;
	}

	PDF_fit_textflow($p, $textflow, $left, $y-$leading, $right, $y, "");
	PDF_delete_textflow($p, $textflow);

	$y += $leading;
	$total +=$sum;
    }

    $y += $leading;
    PDF_setfont($p, $boldfont, $fontsize);
    PDF_fit_textline($p,sprintf("%.2f",$total), $right, $y, "position {100 0}");

    # Print the closing text

    $y +=5*$leading;
    $optlist =  "alignment=justify leading=120% " .
		"fontname=Helvetica fontsize=12 encoding=winansi ";

    $textflow = PDF_create_textflow($p, $closingtext, $optlist);

    if ($textflow == -1){
	printf("Error: %s\n", PDF_get_errmsg($p));
	exit;
    }

    PDF_fit_textflow($p, $textflow, $left, $y + 6*$leading, $right, $y, "");
    PDF_delete_textflow($p, $textflow);

    PDF_end_page_ext($p, "");
    PDF_end_document($p, "");
    PDF_close_pdi($p, $stationery);
};
if ($@) {
    printf("invoice: PDFlib Exception occurred:\n");
    printf(" $@\n");
    exit;
}

PDF_delete($p);
