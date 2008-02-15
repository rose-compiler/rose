/*---------------------------------------------------------------------------*
 |              PDFlib - A library for generating PDF on the fly             |
 +---------------------------------------------------------------------------+
 | Copyright (c) 1997-2005 Thomas Merz and PDFlib GmbH. All rights reserved. |
 +---------------------------------------------------------------------------+
 |                                                                           |
 |    This software is subject to the PDFlib license. It is NOT in the       |
 |    public domain. Extended versions and commercial licenses are           |
 |    available, please check http://www.pdflib.com.                         |
 |                                                                           |
 *---------------------------------------------------------------------------*/

/* $Id: p_jpx.c,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * JPEG2000 processing for PDFlib
 *
 */

#include "p_intern.h"
#include "p_color.h"
#include "p_image.h"

#ifndef PDF_JPX_SUPPORTED

pdc_bool
pdf_is_JPX_file(PDF *p, pdc_file *fp)
{
    (void) p;
    (void) fp;

    return pdc_false;
}

int
pdf_process_JPX_data(
    PDF *p,
    int imageslot)
{
    (void) imageslot;

    pdc_warning(p->pdc, PDF_E_UNSUPP_IMAGE, "JPEG2000", 0, 0, 0);
    return -1;
}

#else


pdc_bool
pdf_is_JPX_file(PDF *p, pdc_file *fp)
{
    (void) p;
    (void) fp;

    return pdc_false;
}

int
pdf_process_JPX_data(
    PDF *p,
    int imageslot)
{
    (void) imageslot;

    pdc_warning(p->pdc, PDF_E_UNSUPP_IMAGE, "JPEG2000", 0, 0, 0);
    return -1;
}



#endif /* PDF_JPX_SUPPORTED */

