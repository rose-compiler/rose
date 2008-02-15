<?php
/* $Id: chartab.php,v 1.1 2005/10/11 17:18:09 vuduc2 Exp $
 *
 * PDFlib client: chartab example in PHP
 */

/* change these as required */
$fontname = "LuciduxSans-Oblique";

/* This is where font/image/PDF input files live. Adjust as necessary. */ 
$searchpath = "../data"; 

/* list of encodings to use */
$encodings = array( "iso8859-1", "iso8859-2", "iso8859-15" );

/* whether or not to embed the font */
$embed = 1;

define("FONTSIZE", 	16);
define("TOP",		700);
define("LEFT", 		50);
define("YINCR", 	2*FONTSIZE);
define("XINCR",		2*FONTSIZE);

try {
    $p = new PDFlib();

    /*  open new PDF file; insert a file name to create the PDF on disk */
    if ($p->begin_document("", "destination {type fitwindow page 1}") == 0) {
	die("Error: " . $p->get_errmsg());
    }

    $p->set_parameter("fontwarning", "true");
    $p->set_parameter("SearchPath", $searchpath);

    /* This line is required to avoid problems on Japanese systems */
    $p->set_parameter("hypertextencoding", "winansi");

    $p->set_info("Creator", "chartab.php");
    $p->set_info("Author", "Thomas Merz");
    $p->set_info("Title", "Character table (PHP)");

    /* loop over all encodings */
    for ($page = 0; $page < count($encodings); $page++)
    {
	$p->begin_page_ext(595, 842, "");

	/* print the heading and generate the bookmark */
	$font = $p->load_font("Helvetica", "winansi", "");
	$p->setfont($font, FONTSIZE);
	$buf = sprintf("%s (%s) %sembedded",
	    $fontname, $encodings[$page], $embed ? "" : "not ");

	$p->show_xy($buf, LEFT - XINCR, TOP + 3 * YINCR);
	$p->create_bookmark($buf, "");

	/* print the row and column captions */
	$p->setfont($font, 2 * FONTSIZE/3);

	for ($row = 0; $row < 16; $row++)
	{
	    $buf = sprintf("x%X", $row);
	    $p->show_xy($buf, LEFT + $row*XINCR, TOP + YINCR);

	    $buf = sprintf("%Xx", $row);
	    $p->show_xy($buf, LEFT - XINCR, TOP - $row * YINCR);
	}

	/* print the character table */
	$font = $p->load_font($fontname, $encodings[$page],
	    $embed ? "embedding": "");
	$p->setfont($font, FONTSIZE);

	$y = TOP;
	$x = LEFT;

	for ($row = 0; $row < 16; $row++)
	{
	    for ($col = 0; $col < 16; $col++) {
		$buf = sprintf("%c", 16*$row + $col);
		$p->show_xy($buf, $x, $y);
		$x += XINCR;
	    }
	    $x = LEFT;
	    $y -= YINCR;
	}

	$p->end_page_ext("");
    }


    $p->end_document("");

    $buf = $p->get_buffer();
    $len = strlen($buf);

    header("Content-type: application/pdf");
    header("Content-Length: $len");
    header("Content-Disposition: inline; filename=chartab.pdf");
    print $buf;

}
catch (PDFlibException $e) {
    die("PDFlib exception occurred in chartab sample:\n" .
	"[" . $e->get_errnum() . "] " . $e->get_apiname() . ": " .
	$e->get_errmsg() . "\n");
}
catch (Exception $e) {
    die($e);
}

$p = 0;
?>
