<?php
/* $Id: businesscard.php,v 1.1 2005/10/11 17:18:09 vuduc2 Exp $
 *
 * PDFlib client: businesscard example in PHP
 */

$infile = "boilerplate.pdf";

/* This is where font/image/PDF input files live. Adjust as necessary.
 *
 * Note that this directory must also contain the LuciduxSans font outline
 * and metrics files.
 */
$searchpath = "../data";

$data = array(  "name"				=> "Victor Kraxi",
		"business.title"		=> "Chief Paper Officer",
		"business.address.line1" 	=> "17, Aviation Road",
		"business.address.city"		=> "Paperfield",
		"business.telephone.voice"	=> "phone +1 234 567-89",
		"business.telephone.fax"	=> "fax +1 234 567-98",
		"business.email"		=> "victor@kraxi.com",
		"business.homepage"		=> "www.kraxi.com"
	);

try {
    $p = new PDFlib();

    /*  open new PDF file; insert a file name to create the PDF on disk */
    if ($p->begin_document("", "") == 0) {
	die("Error: " . $p->get_errmsg());
    }

    /* Set the search path for fonts and PDF files */
    $p->set_parameter("SearchPath", $searchpath);

    /* This line is required to avoid problems on Japanese systems */
    $p->set_parameter("hypertextencoding", "winansi");

    $p->set_info("Creator", "businesscard.php");
    $p->set_info("Author", "Thomas Merz");
    $p->set_info("Title", "PDFlib block processing sample (PHP)");

    $blockcontainer = $p->open_pdi($infile, "", 0);
    if ($blockcontainer == 0){
	die ("Error: " . $p->get_errmsg());
    }

    $page = $p->open_pdi_page($blockcontainer, 1, "");
    if ($page == 0){
	die ("Error: " . $p->get_errmsg());
    }

    $p->begin_page_ext(20, 20, "");		/* dummy page size */

    /* This will adjust the page size to the block container's size. */
    $p->fit_pdi_page($page, 0, 0, "adjustpage");

    /* Fill all text blocks with dynamic data */
    foreach ($data as $key => $value){
	if ($p->fill_textblock($page, $key, $value,
	    "embedding encoding=winansi") == 0) {
	    printf("Warning: %s\n ", $p->get_errmsg());
	}
    }

    $p->end_page_ext("");
    $p->close_pdi_page($page);

    $p->end_document("");
    $p->close_pdi($blockcontainer);

    $buf = $p->get_buffer();
    $len = strlen($buf);

    header("Content-type: application/pdf");
    header("Content-Length: $len");
    header("Content-Disposition: inline; filename=businesscard.pdf");
    print $buf;

}
catch (PDFlibException $e) {
    die("PDFlib exception occurred in businesscard sample:\n" .
	"[" . $e->get_errnum() . "] " . $e->get_apiname() . ": " .
	$e->get_errmsg() . "\n");
}
catch (Exception $e) {
    die($e);
}

$p = 0;
?>
