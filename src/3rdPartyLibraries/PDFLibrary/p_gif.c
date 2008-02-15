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

/* p_gif.c
 *
 * GIF processing for PDFlib
 *
 */

#include <string.h>

#include "p_intern.h"

#define LOCALCOLORMAP		0x80
#define BitSet(byteval, bitval)	(((byteval) & (bitval)) == (bitval))
#define	ReadOK(file,buffer,len)	(fread(buffer, len, 1, file) != 0)
#define LM_to_uint(a,b)			(((b)<<8)|(a))

static int ReadColorMap(FILE *fp, int number, pdf_colormap buffer);
static int DoExtension(PDF *p, pdf_image *image, int label);
static int GetDataBlock(PDF *p, pdf_image *image, unsigned char  *buf);

static void
pdf_data_source_GIF_init(PDF *p, PDF_data_source *src)
{
    pdf_image		*image = (pdf_image *) src->private_data;

    src->buffer_length	= 260;	/* max. GIF "data sub-block" length */

    src->buffer_start	= (pdf_byte*) p->malloc(p, src->buffer_length,
				"pdf_data_source_GIF_init");
    src->bytes_available= 0;
    src->next_byte	= src->buffer_start;

    /* init the LZW transformation vars */
    image->info.gif.c_size = 9;		/* initial code size	*/
    image->info.gif.t_size = 257;	/* initial "table" size	*/
    image->info.gif.i_buff = 0;		/* input buffer		*/
    image->info.gif.i_bits = 0;		/* input buffer empty	*/
    image->info.gif.o_bits = 0;		/* output buffer empty	*/
} /* pdf_data_source_GIF_init */

static pdf_bool
pdf_data_source_GIF_fill(PDF *p, PDF_data_source *src)
{
#define c_size	image->info.gif.c_size
#define t_size	image->info.gif.t_size
#define i_buff	image->info.gif.i_buff
#define i_bits	image->info.gif.i_bits
#define o_buff	image->info.gif.o_buff
#define o_bits	image->info.gif.o_bits

    pdf_image *		image = (pdf_image *) src->private_data;
    FILE *		fp = image->fp;
    int			n_bytes = getc(fp);	/* # of bytes to read	*/
    unsigned char *	o_curr = src->buffer_start;
    int			c_mask = (1 << c_size) - 1;
    char		filenamebuf[PDF_FILENAMELEN];
    pdf_bool		flag13 = pdf_false;

    src->bytes_available = 0;

    if (n_bytes == EOF)
    {
	strncpy(filenamebuf, image->filename, PDF_FILENAMELEN-1);
	filenamebuf[PDF_FILENAMELEN-1] = '\0';

	PDF_close_image(p, image - p->images);
	pdf_error(p, PDF_IOError, "Unexpected end of GIF file '%s'",
		filenamebuf);
    }

    if (n_bytes == 0)
	return pdf_false;

    for (/* */ ; /* */ ; /* */)
    {
	int w_bits = c_size;	/* number of bits to write */
	int code;

	/* get at least c_size bits into i_buff	*/
	while (i_bits < c_size)
	{
	    if (n_bytes-- == 0)
	    {
		src->bytes_available = (size_t) (o_curr - src->buffer_start);
		return pdf_true;
	    }
	    i_buff |= getc(fp) << i_bits;	/* EOF will be caught later */
	    i_bits += 8;
	}
	code = i_buff & c_mask;
	i_bits -= c_size;
	i_buff >>= c_size;

	if (flag13 && code != 256 && code != 257)
	{
	    strncpy(filenamebuf, image->filename, PDF_FILENAMELEN-1);
	    filenamebuf[PDF_FILENAMELEN-1] = '\0';

	    PDF_close_image(p, image - p->images);
	    pdf_error(p, PDF_IOError,
		"LZW code size overflow in GIF file '%s'", filenamebuf);
	}

	if (o_bits > 0)
	{
	    o_buff |= code >> (c_size - 8 + o_bits);
	    w_bits -= 8 - o_bits;
	    *(o_curr++) = (unsigned char) o_buff;
	}
	if (w_bits >= 8)
	{
	    w_bits -= 8;
	    *(o_curr++) = (unsigned char) (code >> w_bits);
	}
	o_bits = w_bits;
	if (o_bits > 0)
	    o_buff = code << (8 - o_bits);

	++t_size;
	if (code == 256)	/* clear code */
	{
	    c_size = 9;
	    c_mask = (1 << c_size) - 1;
	    t_size = 257;
	    flag13 = pdf_false;
	}

	if (code == 257)	/* end code */
	{
	    src->bytes_available = (size_t) (o_curr - src->buffer_start);
	    return pdf_true;
	}

	if (t_size == (1 << c_size))
	{
	    if (++c_size > 12)
	    {
		--c_size;
		flag13 = pdf_true;
	    }
	    else
		c_mask = (1 << c_size) - 1;
	}
    } /* for (;;) */

#undef	c_size
#undef	t_size
#undef	i_buff
#undef	i_bits
#undef	o_buff
#undef	o_bits
} /* pdf_data_source_GIF_fill */

static void
pdf_data_source_GIF_terminate(PDF *p, PDF_data_source *src)
{
    p->free(p, (void *) src->buffer_start);
}

int
pdf_open_GIF_data(PDF *p, int imageslot, const char *filename, const char *stringparam, int intparam)
{
    unsigned char	buf[16];
    char	c;
    int		imageCount = 0;
    char	version[4];
    int		imageNumber = 1;	/* we read only the first image */
    pdf_image	*image;
    int		mask = -1;

    image = &p->images[imageslot];

    if (stringparam && *stringparam) {
	if (!strcmp(stringparam, "mask"))
	    pdf_error(p, PDF_ValueError, "Can't handle GIF image mask");

	else if (!strcmp(stringparam, "masked")) {
	    mask = intparam;

	    if (mask < 0)
		pdf_error(p, PDF_ValueError,
		    "Invalid image mask (no %d) for image '%s'", mask,
		    	filename);

	    if (mask >= p->images_capacity || !p->images[mask].in_use ||
		p->images[mask].colorspace != ImageMask) {
		pdf_error(p, PDF_ValueError,
		    "Bad image mask (no %d) for image '%s'", mask, filename);
	    }
	} else if (!strcmp(stringparam, "ignoremask")) {
	    /* we invert this flag later */
	    image->transparent = pdf_true;
	} else
	    pdf_error(p, PDF_ValueError,
	    	"Unknown parameter %s in pdf_open_GIF_data", stringparam);
    }

    image->colormap 	= (pdf_colormap*) p->malloc(p, sizeof(pdf_colormap),
    					"pdf_open_GIF_data");
    image->mask		= mask;

    if ((image->fp = fopen(filename, READMODE)) == NULL) {
	if (p->debug['i']) {
	    pdf_error(p, PDF_NonfatalError,
	    "Couldn't open GIF file '%s'", filename);
	}
	return -1;	/* Couldn't open GIF file */
    }

#define PDF_STRING_GIF	"\107\111\106"
#define PDF_STRING_87a	"\070\067\141"
#define PDF_STRING_89a	"\070\071\141"

    /* Error reading magic number or not a GIF file */
    if (!ReadOK(image->fp, buf, 6) || 
	strncmp((const char *) buf, PDF_STRING_GIF, 3) != 0) {
	fclose(image->fp);
	if (p->debug['i']) {
	    pdf_error(p, PDF_NonfatalError,
		"File '%s' doesn't appear to be a GIF file", filename);
	}
	return -1;
    }

    strncpy(version, (const char *) buf + 3, 3);
    version[3] = '\0';

    /* Bad version number */
    if ((strcmp(version, PDF_STRING_87a) != 0) &&
        (strcmp(version, PDF_STRING_89a) != 0)) {
	fclose(image->fp);
	if (p->debug['i']) {
	    pdf_error(p, PDF_NonfatalError,
		"File '%s' doesn't appear to be a GIF file", filename);
	}
	return -1;
    }

    /* Failed to read screen descriptor */
    if (! ReadOK(image->fp, buf, 7)) {
	fclose(image->fp);
	if (p->debug['i']) {
	    pdf_error(p, PDF_NonfatalError,
		"File '%s' doesn't appear to be a GIF file", filename);
	}
	return -1;
    }

    /* size of the global color table*/
    image->palette_size = 2 << (buf[4] & 0x07);

    if (BitSet(buf[4], LOCALCOLORMAP)) {	/* Global Colormap */
	if (ReadColorMap(image->fp, image->palette_size, *image->colormap)) {
	    fclose(image->fp);	/* Error reading global colormap */
	    if (p->debug['i']) {
		pdf_error(p, PDF_NonfatalError,
		    "Error reading global colormap in GIF file '%s'", filename);
	    }
	    return -1;
	}
    }

    /* translate the aspect ratio to PDFlib notation */
    if (buf[6] != 0) {
	image->dpi_x = -(buf[6] + ((float) 15.0)) / ((float) 64.0);
	image->dpi_y = (float) -1.0;
    }

    for (/* */ ; /* */ ; /* */) {
	/* EOF / read error in image data */
	if (!ReadOK(image->fp, &c, 1)) {
	    fclose(image->fp);
	    if (p->debug['i']) {
		pdf_error(p, PDF_NonfatalError,
		    "Read error (image data) in GIF file '%s'", filename);
	    }
	    return -1;
	}

	if (c == PDF_SEMICOLON) {		/* GIF terminator */
	    /* Not enough images found in file */
	    if (imageCount < imageNumber) {
		fclose(image->fp);
		if (p->debug['i']) {
		    pdf_error(p, PDF_NonfatalError,
		    "Couldn't find image number %d in GIF file '%s'",
			imageCount, filename);
		}
		return -1;
	    }
	    break;
	}

	if (c == PDF_EXCLAM) { 	/* Extension */
	    if (!ReadOK(image->fp, &c, 1)) {
		/* EOF / read error on extension function code */
		fclose(image->fp);
		if (p->debug['i']) {
		    pdf_error(p, PDF_NonfatalError,
			"Read error on extension function code in GIF file '%s'",
			filename);
		}
		return -1;
	    }
	    DoExtension(p, image, (int) c);
	    continue;
	}

	if (c != PDF_COMMA) {		/* Not a valid start character */
	    /* Bogus character, ignoring */
	    continue;
	}

	++imageCount;

	if (! ReadOK(image->fp, buf, 9)) {
	    /* Couldn't read left/top/width/height */
	    fclose(image->fp);
	    if (p->debug['i']) {
		pdf_error(p, PDF_NonfatalError,
		    "Couldn't read left/top/width/height in GIF file '%s'",
		    filename);
	    }
	    return -1;
	}

	image->colorspace	= Indexed;
	image->components	= 1;
	image->bpc		= 8;
	image->width		= LM_to_uint(buf[4],buf[5]);
	image->height		= LM_to_uint(buf[6],buf[7]);

#define INTERLACE		0x40
	if (BitSet(buf[8], INTERLACE)) {
	    fclose(image->fp);
	    if (p->debug['i']) {
		pdf_error(p, PDF_NonfatalError,
		    "Interlaced image in GIF file '%s' not supported", filename);
	    }
	    return -1;
	}

	if (BitSet(buf[8], LOCALCOLORMAP)) {
	    if (ReadColorMap(image->fp, image->palette_size, *image->colormap)) {
		fclose(image->fp);
		if (p->debug['i']) {
		    pdf_error(p, PDF_NonfatalError,
		    "Error reading local colormap in GIF file '%s'", filename);
		}
		return -1;
	    }
	}

	/* read the "LZW initial code size".
	*/
	if (!ReadOK(image->fp, buf, 1)) {
	    fclose(image->fp);
	    if (p->debug['i']) {
		pdf_error(p, PDF_NonfatalError,
		    "Error reading data block in GIF file '%s'", filename);
	    }
	    return -1;
	}
	if (buf[0] != 8) {
	    fclose(image->fp);
	    if (p->debug['i']) {
		pdf_error(p, PDF_NonfatalError,
		    "Color depth other than 8 bit not supported in GIF file '%s'",
		    filename);
	    }
	    return -1;
	}

	if (imageCount == 1)
	    break;
    }
    
    image->src.init		= pdf_data_source_GIF_init;
    image->src.fill		= pdf_data_source_GIF_fill;
    image->src.terminate	= pdf_data_source_GIF_terminate;
    image->src.private_data	= (void *) image;

    image->in_use		= pdf_true;		/* mark slot as used */
    image->filename		= pdf_strdup(p, filename);
    image->compression		= lzw;

    image->params = (char *) p->malloc(p, PDF_MAX_PARAMSTRING, "pdf_open_GIF_data");
    strcpy(image->params, "/EarlyChange 0");

    pdf_put_image(p, imageslot, pdf_true);
    fclose(image->fp);
    p->free(p, image->colormap);

    return imageslot;
} /* pdf_open_GIF_data */

static int
ReadColorMap(FILE *fp, int number, pdf_colormap buffer)
{
    int		i;
    unsigned char	rgb[3];

    /* TODO: optimize to return !ReadOk(fp, buffer, 3*i); */
    for (i = 0; i < number; ++i) {
	if (! ReadOK(fp, rgb, sizeof(rgb))) {
	    return pdf_true;		/* yk: true == error */
	}

	buffer[i][0] = rgb[0] ;
	buffer[i][1] = rgb[1] ;
	buffer[i][2] = rgb[2] ;
    }
    return pdf_false;			/* yk: false == ok.  */
} /* ReadColorMap */

static int
DoExtension(PDF *p, pdf_image *image, int label)
{
    pdf_byte            buf[256];

    switch ((unsigned char) label) {
	case 0x01:		/* Plain Text Extension */
	    break;

	case 0xff:		/* Application Extension */
	    break;

	case 0xfe:		/* Comment Extension */
	    while (GetDataBlock(p, image, (unsigned char*) buf) != 0) {
		/* */
	    }
	    return pdf_false;

	case 0xf9:		/* Graphic Control Extension */
	    (void) GetDataBlock(p, image, (unsigned char*) buf);

	    if ((buf[0] & 0x1) != 0) {
		image->transparent = !image->transparent;
		image->transval[0] = buf[3];
	    }

	    while (GetDataBlock(p, image, (unsigned char*) buf) != 0) {
		    /* */ ;
	    }
	    return pdf_false;

	default:
	    break;
    }

    while (GetDataBlock(p, image, (unsigned char*) buf) != 0) {
	    /* */ ;
    }

    return pdf_false;
} /* DoExtension */

static int
GetDataBlock(PDF *p, pdf_image *image, unsigned char *buf)
{
    char		filenamebuf[PDF_FILENAMELEN];
    unsigned char	count;
    FILE *fp = image->fp;

    if (!ReadOK(fp, &count, 1))
    {
	strncpy(filenamebuf, image->filename, PDF_FILENAMELEN-1);
	filenamebuf[PDF_FILENAMELEN-1] = '\0';

	PDF_close_image(p, image - p->images);
	pdf_error(p, PDF_IOError, "Corrupt GIF file '%s'", filenamebuf);
    }

    if ((count != 0) && (!ReadOK(fp, buf, count))) {
	strncpy(filenamebuf, image->filename, PDF_FILENAMELEN-1);
	filenamebuf[PDF_FILENAMELEN-1] = '\0';

	PDF_close_image(p, image - p->images);
	pdf_error(p, PDF_IOError, "Corrupt GIF file '%s'", filenamebuf);
    }

    return count;
} /* GetDataBlock */
