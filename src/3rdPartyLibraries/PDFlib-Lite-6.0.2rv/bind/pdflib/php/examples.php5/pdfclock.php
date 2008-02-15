<?php
/* $Id: pdfclock.php,v 1.1 2005/10/11 17:18:09 vuduc2 Exp $
 *
 * A little PDFlib application to draw an analog clock.
 */

$RADIUS = 200.0;
$MARGIN = 20.0;

try {
    $p = new PDFlib();

    /*  open new PDF file; insert a file name to create the PDF on disk */
    if ($p->begin_document("", "") == 0) {
	die("Error: " . $p->get_errmsg());
    }

    /* This line is required to avoid problems on Japanese systems */
    $p->set_parameter("hypertextencoding", "winansi");

    $p->set_info("Creator", "pdfclock.php");
    $p->set_info("Author", "Rainer Schaaf");
    $p->set_info("Title", "PDF clock (PHP)");

    $p->begin_page_ext(2 * ($RADIUS + $MARGIN), 2 * ($RADIUS + $MARGIN), "");

    $p->translate($RADIUS + $MARGIN, $RADIUS + $MARGIN);
    $p->setcolor("fillstroke", "rgb", 0.0, 0.0, 1.0, 0.0);
    $p->save();

    /* minute strokes */
    $p->setlinewidth(2.0);
    for ($alpha = 0; $alpha < 360; $alpha += 6)
    {
	$p->rotate(6.0);
	$p->moveto($RADIUS, 0.0);
	$p->lineto($RADIUS-$MARGIN/3, 0.0);
	$p->stroke();
    }

    $p->restore();
    $p->save();

    /* 5 minute strokes */
    $p->setlinewidth(3.0);
    for ($alpha = 0; $alpha < 360; $alpha += 30)
    {
	$p->rotate(30.0);
	$p->moveto($RADIUS, 0.0);
	$p->lineto($RADIUS-$MARGIN, 0.0);
	$p->stroke();
    }

    $ltime = getdate();

    /* draw hour hand */
    $p->save();
    $p->rotate(-(($ltime['minutes']/60.0)+$ltime['hours']-3.0)*30.0);
    $p->moveto(-$RADIUS/10, -$RADIUS/20);
    $p->lineto($RADIUS/2, 0.0);
    $p->lineto(-$RADIUS/10, $RADIUS/20);
    $p->closepath();
    $p->fill();
    $p->restore();

    /* draw minute hand */
    $p->save();
    $p->rotate(-(($ltime['seconds']/60.0)+$ltime['minutes']-15.0)*6.0);
    $p->moveto(-$RADIUS/10, -$RADIUS/20);
    $p->lineto($RADIUS * 0.8, 0.0);
    $p->lineto(-$RADIUS/10, $RADIUS/20);
    $p->closepath();
    $p->fill();
    $p->restore();

    /* draw second hand */
    $p->setcolor("fillstroke", "rgb", 1.0, 0.0, 0.0, 0.0);
    $p->setlinewidth(2);
    $p->save();
    $p->rotate(-(($ltime['seconds'] - 15.0) * 6.0));
    $p->moveto(-$RADIUS/5, 0.0);
    $p->lineto($RADIUS, 0.0);
    $p->stroke();
    $p->restore();

    /* draw little circle at center */
    $p->circle(0, 0, $RADIUS/30);
    $p->fill();

    $p->restore();
    $p->end_page_ext("");

    $p->end_document("");

    $buf = $p->get_buffer();
    $len = strlen($buf);

    header("Content-type: application/pdf");
    header("Content-Length: $len");
    header("Content-Disposition: inline; filename=pdfclock.pdf");
    print $buf;

}
catch (PDFlibException $e) {
    die("PDFlib exception occurred in pdfclock sample:\n" .
	"[" . $e->get_errnum() . "] " . $e->get_apiname() . ": " .
	$e->get_errmsg() . "\n");
}
catch (Exception $e) {
    die($e);
}

$p = 0;
?>
