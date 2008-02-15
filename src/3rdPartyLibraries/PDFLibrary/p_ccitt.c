/*---------------------------------------------------------------------------*
 |        PDFlib - A library for dynamically generating PDF documents        |
 +---------------------------------------------------------------------------+
 |        Copyright (c) 1997-2000 Thomas Merz. All rights reserved.          |
 +---------------------------------------------------------------------------+
 |    This software is NOT in the public domain.  It can be used under two   |
 |    substantially different licensing terms:                               |
 |                                                                           |
 |    The commercial license is available for a fee, and allows you to       |
 |    - ship a commercial product based on PDFlib                            |
 |    - implement commercial Web services with PDFlib                        |
 |    - distribute (free or commercial) software when the source code is     |
 |      not made available                                                   |
 |    Details can be found in the file PDFlib-license.pdf.                   |
 |                                                                           |
 |    The "Aladdin Free Public License" doesn't require any license fee,     |
 |    and allows you to                                                      |
 |    - develop and distribute PDFlib-based software for which the complete  |
 |      source code is made available                                        |
 |    - redistribute PDFlib non-commercially under certain conditions        |
 |    - redistribute PDFlib on digital media for a fee if the complete       |
 |      contents of the media are freely redistributable                     |
 |    Details can be found in the file aladdin-license.pdf.                  |
 |                                                                           |
 |    These conditions extend to ports to other programming languages.       |
 |    PDFlib is distributed with no warranty of any kind. Commercial users,  |
 |    however, will receive warranty and support statements in writing.      |
 *---------------------------------------------------------------------------*/

/* p_ccitt.c
 *
 * CCITT (Fax G3 and G4) processing for PDFlib
 *
 */

#include <stdio.h>
#include <string.h>

#include "p_intern.h"

#define CCITT_BUFSIZE    1024

/* 
 * Do a bit-reversal of all bytes in the buffer.
 * This is supported for some clients which provide
 * CCITT-compressed data in a byte-reversed format.
 */

static void
pdf_reverse_bit_order(unsigned char *buffer, size_t size)
{
    size_t i;

    /* table for fast byte reversal */
    static const pdf_byte reverse[256] = {
	    0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
	    0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
	    0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
	    0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
	    0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
	    0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
	    0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
	    0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
	    0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
	    0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
	    0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
	    0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
	    0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
	    0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
	    0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
	    0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
	    0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
	    0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
	    0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
	    0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
	    0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
	    0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
	    0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
	    0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
	    0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
	    0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
	    0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
	    0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
	    0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
	    0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
	    0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
	    0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
    };

    if (buffer != NULL) {
	for (i = 0; i < size; i++) {
	    buffer[i] = reverse[buffer[i]];
	}
    }
}

static void
pdf_data_source_CCITT_init(PDF *p, PDF_data_source *src)
{
  src->buffer_start = (pdf_byte *)
	    p->malloc(p, CCITT_BUFSIZE, "PDF_data_source_CCITT_init");
  src->buffer_length = CCITT_BUFSIZE;
}

static pdf_bool
pdf_data_source_CCITT_fill(PDF *p, PDF_data_source *src)
{
    pdf_image	*image;

    (void) p;	/* avoid compiler warning "unreferenced parameter" */

    image = (pdf_image *) src->private_data;

    src->next_byte = src->buffer_start;
    src->bytes_available = fread(src->buffer_start, 1, CCITT_BUFSIZE, image->fp);

    if (src->bytes_available == 0)
    return pdf_false;

    if (image->info.ccitt.BitReverse)
	pdf_reverse_bit_order(src->buffer_start, src->bytes_available);

    return pdf_true;
}

static void
pdf_data_source_CCITT_terminate(PDF *p, PDF_data_source *src)
{
  p->free(p, (void *) src->buffer_start);
}


PDFLIB_API int PDFLIB_CALL
PDF_open_CCITT(PDF *p, const char *filename, int width, int height,
		int BitReverse, int K, int BlackIs1)
{
    pdf_image *image;
    int im;
    char scratch[30];

    if (PDF_SANITY_CHECK_FAILED(p))
	return -1;

    for (im = 0; im < p->images_capacity; im++)
	if (!p->images[im].in_use)		/* found free slot */
	    break;

    if (im == p->images_capacity) 
	pdf_grow_images(p);

    image = &p->images[im];

    if ((image->fp = fopen(filename, READMODE)) == NULL) {
	if (p->debug['i']) {
	    pdf_error(p, PDF_NonfatalError, "Couldn't open CCITT file '%s'", filename);
	}
        return -1;
    }

    /* Grab the image parameters and pack them into image struct */
    image->filename     		= pdf_strdup(p, filename);

    /* CCITT specific information */
    image->width          		= width;
    image->height         		= height;
    image->info.ccitt.BitReverse	= BitReverse;

    if (BlackIs1 == 0 && K == 0)	/* default values */
	image->params = NULL;
    else {
	scratch[0] = '\0';
	if (K != 0) {
	    sprintf(scratch, "/K %d", K);
	}
	if (BlackIs1 == 1)
	    strcat(scratch, "/BlackIs1 true");
	image->params = pdf_strdup(p, scratch);
    }

    /* The following are fixed for CCITT images */
    image->compression  	= ccitt;
    image->colorspace		= DeviceGray;
    image->components		= 1;
    image->bpc			= 1;

    image->src.init             = pdf_data_source_CCITT_init;
    image->src.fill             = pdf_data_source_CCITT_fill;
    image->src.terminate        = pdf_data_source_CCITT_terminate;
    image->src.private_data     = (void *) image;

    image->in_use 		= pdf_true;		/* mark slot as used */

    pdf_put_image(p, im, pdf_true);
    fclose(image->fp);

    return im;
}
