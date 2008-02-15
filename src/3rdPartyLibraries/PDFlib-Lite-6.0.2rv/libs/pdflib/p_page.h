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

/* $Id: p_page.h,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * Header file for the PDFlib page system
 *
 */

#ifndef P_PAGE_H
#define P_PAGE_H

/* the "pageno" parameter for function pdf_set_pagelabel() can take
** negative values, indicating the calling function.
*/
#define PDF_FC_BEGIN_DOCUMENT	-1
#define PDF_FC_END_DOCUMENT	-2
void pdf_set_pagelabel(PDF *p, const char *optlist, int pageno);

pdc_id pdf_write_pages_tree(PDF *p);

void pdf_set_transition(PDF *p, const char *type);
void pdf_set_duration(PDF *p, double t);

#endif /* P_PAGE_H */
