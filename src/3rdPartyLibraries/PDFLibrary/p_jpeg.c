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

/* p_jpeg.c
 *
 * JPEG processing for PDFlib
 *
 */

#include <stdio.h>
#include <string.h>

#include "p_intern.h"

/* 
 * The following enum is stolen from the IJG JPEG library
 * Comments added by tm.
 * This table contains far too many names since PDFlib
 * is rather simple-minded about markers.
 */

typedef enum {		/* JPEG marker codes			*/
  M_SOF0  = 0xc0,	/* baseline DCT				*/
  M_SOF1  = 0xc1,	/* extended sequential DCT		*/
  M_SOF2  = 0xc2,	/* progressive DCT			*/
  M_SOF3  = 0xc3,	/* lossless (sequential)		*/
  
  M_SOF5  = 0xc5,	/* differential sequential DCT		*/
  M_SOF6  = 0xc6,	/* differential progressive DCT		*/
  M_SOF7  = 0xc7,	/* differential lossless		*/
  
  M_JPG   = 0xc8,	/* JPEG extensions			*/
  M_SOF9  = 0xc9,	/* extended sequential DCT		*/
  M_SOF10 = 0xca,	/* progressive DCT			*/
  M_SOF11 = 0xcb,	/* lossless (sequential)		*/
  
  M_SOF13 = 0xcd,	/* differential sequential DCT		*/
  M_SOF14 = 0xce,	/* differential progressive DCT		*/
  M_SOF15 = 0xcf,	/* differential lossless		*/
  
  M_DHT   = 0xc4,	/* define Huffman tables		*/
  
  M_DAC   = 0xcc,	/* define arithmetic conditioning table	*/
  
  M_RST0  = 0xd0,	/* restart				*/
  M_RST1  = 0xd1,	/* restart				*/
  M_RST2  = 0xd2,	/* restart				*/
  M_RST3  = 0xd3,	/* restart				*/
  M_RST4  = 0xd4,	/* restart				*/
  M_RST5  = 0xd5,	/* restart				*/
  M_RST6  = 0xd6,	/* restart				*/
  M_RST7  = 0xd7,	/* restart				*/
  
  M_SOI   = 0xd8,	/* start of image			*/
  M_EOI   = 0xd9,	/* end of image				*/
  M_SOS   = 0xda,	/* start of scan			*/
  M_DQT   = 0xdb,	/* define quantization tables		*/
  M_DNL   = 0xdc,	/* define number of lines		*/
  M_DRI   = 0xdd,	/* define restart interval		*/
  M_DHP   = 0xde,	/* define hierarchical progression	*/
  M_EXP   = 0xdf,	/* expand reference image(s)		*/
  
  M_APP0  = 0xe0,	/* application marker, used for JFIF	*/
  M_APP1  = 0xe1,	/* application marker			*/
  M_APP2  = 0xe2,	/* application marker			*/
  M_APP3  = 0xe3,	/* application marker			*/
  M_APP4  = 0xe4,	/* application marker			*/
  M_APP5  = 0xe5,	/* application marker			*/
  M_APP6  = 0xe6,	/* application marker			*/
  M_APP7  = 0xe7,	/* application marker			*/
  M_APP8  = 0xe8,	/* application marker			*/
  M_APP9  = 0xe9,	/* application marker			*/
  M_APP10 = 0xea,	/* application marker			*/
  M_APP11 = 0xeb,	/* application marker			*/
  M_APP12 = 0xec,	/* application marker			*/
  M_APP13 = 0xed,	/* application marker			*/
  M_APP14 = 0xee,	/* application marker, used by Adobe	*/
  M_APP15 = 0xef,	/* application marker			*/
  
  M_JPG0  = 0xf0,	/* reserved for JPEG extensions		*/
  M_JPG13 = 0xfd,	/* reserved for JPEG extensions		*/
  M_COM   = 0xfe,	/* comment				*/
  
  M_TEM   = 0x01,	/* temporary use			*/

  M_ERROR = 0x100	/* dummy marker, internal use only	*/
} JPEG_MARKER;

#define JPEG_BUFSIZE	1024

static void
pdf_data_source_JPEG_init(PDF *p, PDF_data_source *src)
{
  pdf_image	*image;

  image = (pdf_image *) src->private_data;

  src->buffer_start = (pdf_byte *)
  	p->malloc(p, JPEG_BUFSIZE, "PDF_data_source_JPEG_init");
  src->buffer_length = JPEG_BUFSIZE;

  fseek(image->fp, image->info.jpeg.startpos, SEEK_SET);
}

static pdf_bool
pdf_data_source_JPEG_fill(PDF *p, PDF_data_source *src)
{
  pdf_image	*image;

  (void) p;	/* avoid compiler warning "unreferenced parameter" */

  image = (pdf_image *) src->private_data;

  src->next_byte = src->buffer_start;
  src->bytes_available = fread(src->buffer_start, 1, JPEG_BUFSIZE, image->fp);

  if (src->bytes_available == 0)
    return pdf_false;
  else
    return pdf_true;
}

static void
pdf_data_source_JPEG_terminate(PDF *p, PDF_data_source *src)
{
  p->free(p, (void *) src->buffer_start);
}

/*
 * The following routine used to be a macro in its first incarnation:
 *
 * #define get_2bytes(fp) ((unsigned int) (getc(fp) << 8) + getc(fp))
 *
 * However, this is bad programming since C doesn't guarantee
 * the evaluation order of the getc() calls! As suggested by
 * Murphy's law, there are indeed compilers which produce the wrong
 * order of the getc() calls, e.g. the Metrowerks C compiler for BeOS.
 * Since there are only a few calls we don't care about the performance 
 * penalty and use a simplistic C function which does the right thing.
 */

/* read two byte parameter, MSB first */
static unsigned int
get_2bytes(FILE *fp)
{
    unsigned int val;
    val = (unsigned int) (getc(fp) << 8);
    val += (unsigned int) getc(fp);
    return val;
}

static int 
pdf_next_jpeg_marker(FILE *fp)
{ /* look for next JPEG Marker  */
  int c;

  if (feof(fp))
    return M_ERROR;                 /* dummy marker               */

  do {
    do {                            /* skip to FF 		  */
      c = getc(fp);
    } while (c != 0xFF);
    do {                            /* skip repeated FFs  	  */
      c = getc(fp);
    } while (c == 0xFF);
  } while (c == 0);                 /* repeat if FF/00 	      	  */

  return c;
}

/* open JPEG image and analyze marker */
int
pdf_open_JPEG_data(PDF *p, int imageslot, const char *filename, const char *stringparam, int intparam)
{
    int b, c, unit;
    unsigned long i, length;
#define APP_MAX 255
    unsigned char appstring[APP_MAX];
    int SOF_done = pdf_false;
    pdf_image *image;
    int mask = -1;
    pdf_bool adobeflag = pdf_false;

    image = &p->images[imageslot];

    if (stringparam && *stringparam) {
	if (!strcmp(stringparam, "mask")) {
	    pdf_error(p, PDF_ValueError, "Can't handle JPEG image mask");

	} else if (!strcmp(stringparam, "masked")) {
	    mask = intparam;
	    if (mask >= 0 &&
		(mask >= p->images_capacity || !p->images[mask].in_use ||
		p->images[mask].colorspace != ImageMask)) {
		    pdf_error(p, PDF_ValueError,
		    "Bad image mask (no %d) for image '%s'", mask, filename);
	    }
	}
	else
	    pdf_error(p, PDF_ValueError,
	    	"Unknown parameter %s in pdf_open_JPEG", stringparam);
    }

    image->mask		= mask;

    if ((image->fp = fopen(filename, READMODE)) == NULL) {
	if (p->debug['i'])
	    pdf_error(p, PDF_NonfatalError, "Couldn't open JPEG file '%s'", filename);
	return -1;		/* Couldn't open JPEG file */
    }

    image->mask			= mask;
    image->compression		= dct;

    image->src.init		= pdf_data_source_JPEG_init;
    image->src.fill		= pdf_data_source_JPEG_fill;
    image->src.terminate	= pdf_data_source_JPEG_terminate;
    image->src.private_data	= (void *) image;

  /* Tommy's special trick for Macintosh JPEGs: simply skip some  */
  /* hundred bytes at the beginning of the file!		  */
  do {
    do {                            /* skip if not FF 		  */
      c = getc(image->fp);
    } while (!feof(image->fp) && c != 0xFF);

    do {                            /* skip repeated FFs 	  */
      c = getc(image->fp);
    } while (c == 0xFF);

    /* remember start position */
    if ((image->info.jpeg.startpos = ftell(image->fp)) < 0L) {
	fclose(image->fp);
	if (p->debug['i'])
	    pdf_error(p, PDF_NonfatalError, "File problem with JPEG file '%s'", filename);
	return -1;
    }

    image->info.jpeg.startpos -= 2;	/* subtract marker length     */

    if (c == M_SOI) {
      fseek(image->fp, image->info.jpeg.startpos, SEEK_SET);
      break;
    }
  } while (!feof(image->fp));

#define BOGUS_LENGTH	768
  /* Heuristics: if we are that far from the start chances are
   * it is a TIFF file with embedded JPEG data which we cannot
   * handle - regard as hopeless...
   */
  if (feof(image->fp) || image->info.jpeg.startpos > BOGUS_LENGTH) {
    fclose(image->fp);
    if (p->debug['i'])
	pdf_error(p, PDF_NonfatalError, "File '%s' doesn't appear to be of type JPEG",
	    filename);
    return -1;
  }

  /* process JPEG markers */
  while (!SOF_done && (c = pdf_next_jpeg_marker(image->fp)) != M_EOI) {
    switch (c) {
      case M_ERROR:
      /* The following are not supported in PDF 1.3 */
      case M_SOF3:
      case M_SOF5:
      case M_SOF6:
      case M_SOF7:
      case M_SOF9:
      case M_SOF11:
      case M_SOF13:
      case M_SOF14:
      case M_SOF15:
	fclose(image->fp);
	if (p->debug['i'])
	    pdf_error(p, PDF_NonfatalError,
	    	"JPEG compression scheme '%d' in file '%s' is not supported in PDF 1.3",
		(int) c, filename);
	return -1;

      /*
       * SOF2 and SOF10 are progressive DCT which are not
       * supported prior to Acrobat 4.
       */
      case M_SOF2:
      case M_SOF10:
	if (p->compatibility == PDF_1_2) {
	    fclose(image->fp);
	    if (p->debug['i'])
		pdf_error(p, PDF_NonfatalError, 
		    "Progressive JPEG images are not supported in PDF 1.2");
	    return -1;
	}
	/* fallthrough */

      case M_SOF0:
      case M_SOF1:
	(void) get_2bytes(image->fp);    /* read segment length  */

	image->bpc		 = getc(image->fp);
	image->height            = (int) get_2bytes(image->fp);
	image->width             = (int) get_2bytes(image->fp);
	image->components        = getc(image->fp);

	SOF_done = pdf_true;
	break;

      case M_APP0:		/* check for JFIF marker with resolution */
	length = get_2bytes(image->fp);

	for (i = 0; i < length-2; i++) {	/* get contents of marker */
	  b = getc(image->fp);
	  if (i < APP_MAX)			/* store marker in appstring */
	    appstring[i] = (unsigned char) b;
	}

	/* Check for JFIF application marker and read density values
	 * per JFIF spec version 1.02.
	 */

#define ASPECT_RATIO	0	/* JFIF unit byte: aspect ratio only */
#define DOTS_PER_INCH	1	/* JFIF unit byte: dots per inch     */
#define DOTS_PER_CM	2	/* JFIF unit byte: dots per cm       */

#define PDF_STRING_JFIF	"\112\106\111\106"

	if (length >= 14 && !strncmp(PDF_STRING_JFIF, (char *) appstring, 4)) {
	  unit = appstring[7];		        /* resolution unit */
	  					/* resolution value */
	  image->dpi_x = (float) ((appstring[8]<<8) + appstring[9]);	
	  image->dpi_y = (float) ((appstring[10]<<8) + appstring[11]);	

	  if (image->dpi_x <= (float) 0.0 || image->dpi_y <= (float) 0.0) {
	    image->dpi_x = (float) 0.0;
	    image->dpi_y = (float) 0.0;
	    break;
	  }

	  switch (unit) {
	    case DOTS_PER_INCH:
	      break;

	    case DOTS_PER_CM:
	      image->dpi_x *= (float) 2.54;
	      image->dpi_y *= (float) 2.54;
	      break;

	    case ASPECT_RATIO:
	      image->dpi_x *= -1;
	      image->dpi_y *= -1;
	      break;

	    default:				/* unknown ==> ignore */
		/* */ ;
	  }
	}

        break;


      case M_APP14:				/* check for Adobe marker */
	length = get_2bytes(image->fp);

	for (i = 0; i < length-2; i++) {	/* get contents of marker */
	  b = getc(image->fp);
	  if (i < APP_MAX)			/* store marker in appstring */
	    appstring[i] = (unsigned char) b;
	}

	/* 
	 * Check for Adobe application marker. It is known (per Adobe's TN5116)
	 * to contain the string "Adobe" at the start of the APP14 marker.
	 */
#define PDF_STRING_Adobe	"\101\144\157\142\145"

	if (length >= 12 && !strncmp(PDF_STRING_Adobe, (char *) appstring, 5))
	  adobeflag = pdf_true;		/* set Adobe flag */

	break;

      case M_SOI:		/* ignore markers without parameters */
      case M_EOI:
      case M_TEM:
      case M_RST0:
      case M_RST1:
      case M_RST2:
      case M_RST3:
      case M_RST4:
      case M_RST5:
      case M_RST6:
      case M_RST7:
	break;


      default:			/* skip variable length markers */
	length = get_2bytes(image->fp);
	for (length -= 2; length > 0; length--)
	  (void) getc(image->fp);
	break;
    }
  }

  /* do some sanity checks with the parameters */
  if (image->height <= 0 || image->width <= 0 || image->components <= 0) {
    fclose(image->fp);
    if (p->debug['i'])
	pdf_error(p, PDF_NonfatalError,
	    "Bad image parameters in JPEG file '%s' (w=%d h=%d, colors=%d",
	    filename, image->width, image->height, image->components);
    return -1;
  }

  if (image->bpc != 8) {
    fclose(image->fp);
    if (p->debug['i'])
	pdf_error(p, PDF_NonfatalError, 
	    "Bad number of bits per pixel (%d) in JPEG file '%s'", image->bpc, filename);
    return -1;
  }

  switch (image->components) {
    case 1:
	image->colorspace = DeviceGray;
	break;

    case 3:
	image->colorspace = DeviceRGB;
	break;

    case 4:
	image->colorspace = DeviceCMYK;
	/* special handling of Photoshop-generated CMYK JPEG files */
	if (adobeflag)
	    image->invert = pdf_true;
	break;

    default:
	fclose(image->fp);
	if (p->debug['i'])
	    pdf_error(p, PDF_NonfatalError,
	    	"Unknown number of color components (%d) in JPEG file '%s'",
		image->components, filename);
	return -1;
  }

    image->in_use	= pdf_true;		/* mark slot as used */

    image->filename	= pdf_strdup(p, filename);

  pdf_put_image(p, imageslot, pdf_true);
  fclose(image->fp);

  return imageslot;
}
