<?php
/* $Id: quickreference.php,v 1.1 2005/10/11 17:18:09 vuduc2 Exp $
 *
 * PDFlib+PDI client: mini imposition demo
 */

$infile    = "reference.pdf";
/* This is where font/image/PDF input files live. Adjust as necessary. */
$searchpath = "../data";
$maxrow    = 2;
$maxcol    = 2;
$width     = 500.0;
$height    = 770.0;
$endpage   = 0;

$p = PDF_new();

/*  open new PDF file; insert a file name to create the PDF on disk */
if (PDF_begin_document($p, "", "") == 0) {
    die("Error: " . PDF_get_errmsg($p));
}

PDF_set_parameter($p, "SearchPath", $searchpath);

/* This line is required to avoid problems on Japanese systems */
PDF_set_parameter($p, "hypertextencoding", "winansi");

PDF_set_info($p, "Creator", "quickreference.php");
PDF_set_info($p, "Author", "Thomas Merz");
PDF_set_info($p, "Title", "mini imposition demo (php)");

$manual = PDF_open_pdi($p, $infile, "", 0);
if (!$manual) {
    die("Error: " . PDF_get_errmsg($p));
}

$row = 0;
$col = 0;

PDF_set_parameter($p, "topdown", "true");

$endpage = PDF_get_pdi_value($p, "/Root/Pages/Count", $manual, 0, 0);

for ($pageno = 1; $pageno <= $endpage; $pageno++) {
    if ($row == 0 && $col == 0) {
	PDF_begin_page_ext($p, $width, $height, "");
	$font = PDF_load_font($p, "Helvetica-Bold", "winansi", "");
	PDF_setfont($p, $font, 18);
	PDF_set_text_pos($p, 24, 24);
	PDF_show($p, "PDFlib Quick Reference");
    }

    $page = PDF_open_pdi_page($p, $manual, $pageno, "");

    if (!$page) {
	die("Error: " . PDF_get_errmsg($p));
    }

    $optlist = sprintf("scale %f", 1/$maxrow);

    PDF_fit_pdi_page($p, $page, 
	$width/$maxcol*$col, ($row + 1) * $height/$maxrow, $optlist);
    PDF_close_pdi_page($p, $page);

    $col++;
    if ($col == $maxcol) {
	$col = 0;
	$row++;
    }
    if ($row == $maxrow) {
	$row = 0;
	PDF_end_page_ext($p, "");
    }
}

/* finish the last partial page */
if ($row != 0 || $col != 0) {
    PDF_end_page_ext($p, "");
}

PDF_end_document($p, "");
PDF_close_pdi($p, $manual);

$buf = PDF_get_buffer($p);
$len = strlen($buf);

header("Content-type: application/pdf");
header("Content-Length: $len");
header("Content-Disposition: inline; filename=quickreference_php.pdf");
print $buf;

PDF_delete($p);
?>
