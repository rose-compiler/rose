<?php
/* $Id: image.php,v 1.1 2005/10/11 17:18:09 vuduc2 Exp $
 *
 * PDFlib client: image example in PHP
 */

/* This is where font/image/PDF input files live. Adjust as necessary. */ 
$searchpath = "../data"; 

$p = PDF_new();

PDF_set_parameter($p, "SearchPath", $searchpath);

/*  open new PDF file; insert a file name to create the PDF on disk */
if (PDF_begin_document($p, "", "") == 0) {
    die("Error: " . PDF_get_errmsg($p));
}

/* This line is required to avoid problems on Japanese systems */
PDF_set_parameter($p, "hypertextencoding", "winansi");

PDF_set_info($p, "Creator", "image.php");
PDF_set_info($p, "Author", "Rainer Schaaf");
PDF_set_info($p, "Title", "image sample (PHP)");

$imagefile = "nesrin.jpg";

$image = PDF_load_image($p, "auto", $imagefile, "");
if (!$image) {
    die("Error: " . PDF_get_errmsg($p));
}

/* dummy page size, will be adjusted by PDF_fit_image() */
PDF_begin_page_ext($p, 10, 10, "");
PDF_fit_image($p, $image, 0, 0, "adjustpage");
PDF_close_image($p, $image);
PDF_end_page_ext($p, "");

PDF_end_document($p, "");

$buf = PDF_get_buffer($p);
$len = strlen($buf);

header("Content-type: application/pdf");
header("Content-Length: $len");
header("Content-Disposition: inline; filename=image.pdf");
print $buf;

PDF_delete($p);
?>
