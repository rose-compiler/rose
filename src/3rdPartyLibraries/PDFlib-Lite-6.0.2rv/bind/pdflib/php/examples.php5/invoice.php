<?php
/* $Id: invoice.php,v 1.1 2005/10/11 17:18:09 vuduc2 Exp $
 *
 * PDFlib client: invoice example in PHP
 */

$left = 55;
$right = 530;
$fontsize = 12;
$pagewidth = 595;
$pageheight = 842;
$fontsize = 12;
$infile  =  "stationery.pdf";
$baseopt =  "ruler        {   30 45     275   375    475} " .
			"tabalignment { right left right right right} " .
			"hortabmethod ruler fontsize 12 ";


/* This is where font/image/PDF input files live. Adjust as necessary. */
$searchpath = "../data";
$closingtext =
	"Terms of payment: <fillcolor={rgb 1 0 0}>30 days net. " .
	"<fillcolor={gray 0}>90 days warranty starting at the day of sale. " .
	"This warranty covers defects in workmanship only. " .
	"<fontname=Helvetica-BoldOblique encoding=host>Kraxi Systems, Inc. " .
	"<resetfont>will, at its option, repair or replace the " .
	"product under the warranty. This warranty is not transferable. " .
	"No returns or exchanges will be accepted for wet products.";

$data = array(  array("name"=>"Super Kite", 	"price"=>20,	"quantity"=>2),
		array("name"=>"Turbo Flyer", 	"price"=>40, 	"quantity"=>5),
		array("name"=>"Giga Trasch", 	"price"=>180, 	"quantity"=>1),
		array("name"=>"Bare Bone Kit", 	"price"=>50, 	"quantity"=>3),
		array("name"=>"Nitty Gritty", 	"price"=>20, 	"quantity"=>10),
		array("name"=>"Pretty Dark Flyer","price"=>75, 	"quantity"=>1),
		array("name"=>"Free Gift", 	"price"=>0, 	"quantity"=>1)
	    );

$months = array( "January", "February", "March", "April", "May", "June",
	    "July", "August", "September", "October", "November", "December");

try {
    $p = new PDFlib();

    /*  open new PDF file; insert a file name to create the PDF on disk */
    if ($p->begin_document("", "") == 0) {
	die("Error: " . $p->get_errmsg());
    }

    $p->set_parameter("SearchPath", $searchpath);

    /* This line is required to avoid problems on Japanese systems */
    $p->set_parameter("hypertextencoding", "winansi");

    $p->set_info("Creator", "invoice.php");
    $p->set_info("Author", "Thomas Merz");
    $p->set_info("Title", "PDFlib invoice generation demo (PHP)");

    $stationery = $p->open_pdi($infile, "", 0);
    if ($stationery == 0){
	die("Error: " . $p->get_errmsg());
    }

    $page = $p->open_pdi_page($stationery, 1, "");
    if ($page == 0){
	die("Error: " . $p->get_errmsg());
    }


    $boldfont = $p->load_font("Helvetica-Bold", "winansi", "");
    $regularfont = $p->load_font("Helvetica", "winansi", "");
    $leading = $fontsize + 2;

    /* Establish coordinates with the origin in the upper left corner. */
    $p->begin_page_ext($pagewidth, $pageheight, "topdown");

    $p->fit_pdi_page($page, 0, $pageheight, "");
    $p->close_pdi_page($page);

    $p->setfont($regularfont, $fontsize);

    /* print the address */
    $y = 170;
    $p->set_value("leading", $leading);

    $p->show_xy("John Q. Doe", $left, $y);
    $p->continue_text("255 Customer Lane");
    $p->continue_text("Suite B");
    $p->continue_text("12345 User Town");
    $p->continue_text("Everland");

    /* print the header and date */

    $p->setfont($boldfont, $fontsize);
    $y = 300;
    $p->show_xy("INVOICE", $left, $y);
    $time = localtime();
    $buf = sprintf("%s %d, %d", $months[$time[4]], $time[3], $time[5]+1900);
    $p->fit_textline($buf, $right, $y, "position {100 0}");


    /* print the invoice header line */
    $y = 370;
    $buf = sprintf("\tITEM\tDESCRIPTION\tQUANTITY\tPRICE\tAMOUNT");
    $optlist = sprintf("%s font %d ", $baseopt, $boldfont);
    $textflow = $p->create_textflow($buf, $optlist);

    if ($textflow == 0){
	die("Error: " . $p->get_errmsg());
    }

    $p->fit_textflow($textflow, $left, $y-$leading, $right, $y, "");
    $p->delete_textflow($textflow);

    $y += 2*$leading;
    $total = 0;
    $optlist = sprintf("%s font %d ", $baseopt, $regularfont);

    for ($i = 0; $i < count($data); $i++){
	$sum = $data[$i]{"price"}*$data[$i]{"quantity"};
	$buf = sprintf("\t%d\t%s\t%d\t%.2f\t%.2f", $i+1, $data[$i]{"name"},
	    $data[$i]{"quantity"}, $data[$i]{"price"}, $sum);

	$textflow = $p->create_textflow($buf, $optlist);

	if ($textflow == 0){
	    die("Error: " . $p->get_errmsg());
	}

	$p->fit_textflow($textflow, $left, $y-$leading, $right, $y, "");
	$p->delete_textflow($textflow);

	$y += $leading;
	$total +=$sum;
    }

    $y += $leading;

    $p->setfont($boldfont, $fontsize);
    $p->fit_textline(sprintf("%.2f",$total), $right, $y, "position {100 0}");

    /* Print the closing text */

    $y +=5*$leading;
    $optlist  = "alignment=justify leading=120% ";
    $optlist .= "fontname=Helvetica fontsize=12 encoding=winansi";

    $textflow = $p->create_textflow($closingtext, $optlist);

    if ($textflow == 0){
	die("Error: " . $p->get_errmsg());
    }

    $p->fit_textflow($textflow, $left, $y+6*$leading, $right, $y, "");
    $p->delete_textflow($textflow);

    $p->end_page_ext("");
    $p->end_document("");
    $p->close_pdi($stationery);

    $buf = $p->get_buffer();
    $len = strlen($buf);

    header("Content-type: application/pdf");
    header("Content-Length: $len");
    header("Content-Disposition: inline; filename=invoice.pdf");
    print $buf;

}
catch (PDFlibException $e) {
    die("PDFlib exception occurred in invoice sample:\n" .
	"[" . $e->get_errnum() . "] " . $e->get_apiname() . ": " .
	$e->get_errmsg() . "\n");
}
catch (Exception $e) {
    die($e);
}

$p = 0;
?>
