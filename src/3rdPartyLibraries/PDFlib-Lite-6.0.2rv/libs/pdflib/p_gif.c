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

/* $Id: p_gif.c,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * GIF processing for PDFlib
 *
 */

/*
 * This module contains modified parts of the giftopnm.c progam in the
 * netpbm package. It contained the following copyright notice:
 */

/* +-------------------------------------------------------------------+ */
/* | Copyright 1990 - 1994, David Koblas. (koblas@netcom.com)          | */
/* |   Permission to use, copy, modify, and distribute this software   | */
/* |   and its documentation for any purpose and without fee is hereby | */
/* |   granted, provided that the above copyright notice appear in all | */
/* |   copies and that both that copyright notice and this permission  | */
/* |   notice appear in supporting documentation.  This software is    | */
/* |   provided "as is" without express or implied warranty.           | */
/* +-------------------------------------------------------------------+ */

#include "p_intern.h"
#include "p_color.h"
#include "p_image.h"

#ifndef PDF_GIF_SUPPORTED

pdc_bool
pdf_is_GIF_file(PDF *p, pdc_file *fp)
{
    (void) p;
    (void) fp;

    return pdc_false;
}

int
pdf_process_GIF_data(
    PDF *p,
    int imageslot)
{
    (void) imageslot;

    pdc_warning(p->pdc, PDF_E_UNSUPP_IMAGE, "GIF", 0, 0, 0);
    return -1;
}

#else

#define LOCALCOLORMAP		0x80
#define BitSet(byteval, bitval)	(((byteval) & (bitval)) == (bitval))

static int ReadColorMap(pdc_core *pdc, pdc_file *fp,
                        int number, pdf_colormap *buffer);
static int DoExtension(PDF *p, pdf_image *image, int label);
static int GetDataBlock(PDF *p, pdf_image *image, unsigned char  *buf);
static void ReadImage(PDF *p, pdf_image *image, PDF_data_source *src);

static void
pdf_data_source_GIF_init(PDF *p, PDF_data_source *src)
{
    pdf_image		*image;

    image = (pdf_image *) src->private_data;

    src->buffer_length	= (size_t) (image->width * image->height * 1);
    src->buffer_start	= (pdc_byte *) pdc_malloc(p->pdc, src->buffer_length,
				"pdf_data_source_GIF_init");
    src->bytes_available= 0;
    src->next_byte	= NULL;
}

static pdc_bool
pdf_data_source_GIF_fill(PDF *p, PDF_data_source *src)
{
    pdf_image	*image;

    if (src->next_byte != NULL)		/* all finished in one turn */
	return pdc_false;

    image = (pdf_image *) src->private_data;

    src->next_byte = src->buffer_start;
    src->bytes_available = src->buffer_length;

    ReadImage(p, image, src);

    return pdc_true;
}

static void
pdf_data_source_GIF_terminate(PDF *p, PDF_data_source *src)
{
    pdc_free(p->pdc, (void *) src->buffer_start);
}

#define PDF_STRING_GIF  "\107\111\106"
#define PDF_STRING_87a  "\070\067\141"
#define PDF_STRING_89a  "\070\071\141"

pdc_bool
pdf_is_GIF_file(PDF *p, pdc_file *fp)
{
    unsigned char buf[3];

    (void) p;

    if (!PDC_OK_FREAD(fp, buf, 3) ||
        strncmp((const char *) buf, PDF_STRING_GIF, 3) != 0) {
        pdc_fseek(fp, 0L, SEEK_SET);
        return pdc_false;
    }
    return pdc_true;
}

int
pdf_process_GIF_data(
    PDF *p,
    int imageslot)
{
    unsigned char	buf[16];
    char	c;
    int		imageCount = 0;
    char	version[4];
    int         errcode = 0;
    pdf_image	*image;
    pdf_colorspace cs;
    pdf_colormap colormap;
    int slot;

    image = &p->images[imageslot];

    image->info.gif.stack = NULL;
    image->info.gif.table = NULL;

    /* we invert this flag later */
    if (image->ignoremask)
	image->transparent = pdc_true;

    if (image->page == pdc_undef)
        image->page = 1;

    /* Error reading magic number or not a GIF file */
    if (pdf_is_GIF_file(p, image->fp) == pdc_false) {
        errcode = PDC_E_IO_BADFORMAT;
        goto PDF_GIF_ERROR;
    }

    /* Version number */
    if (! PDC_OK_FREAD(image->fp, buf, 3)) {
        errcode = PDC_E_IO_BADFORMAT;
        goto PDF_GIF_ERROR;
    }
    strncpy(version, (const char *) buf, 3);
    version[3] = '\0';
    if ((strcmp(version, PDF_STRING_87a) != 0) &&
        (strcmp(version, PDF_STRING_89a) != 0)) {
        errcode = PDC_E_IO_BADFORMAT;
        goto PDF_GIF_ERROR;
    }

    /* Failed to read screen descriptor */
    if (! PDC_OK_FREAD(image->fp, buf, 7)) {
        errcode = PDC_E_IO_BADFORMAT;
        goto PDF_GIF_ERROR;
    }

    cs.type = Indexed;
    /* size of the global color table */
    cs.val.indexed.palette_size = 2 << (buf[4] & 0x07);
    cs.val.indexed.base = DeviceRGB;
    cs.val.indexed.colormap = &colormap;
    cs.val.indexed.colormap_id = PDC_BAD_ID;

    if (BitSet(buf[4], LOCALCOLORMAP)) {	/* Global Colormap */
        if (ReadColorMap(p->pdc, image->fp,
                         cs.val.indexed.palette_size, &colormap)) {
            errcode = PDF_E_IMAGE_COLORMAP;
            goto PDF_GIF_ERROR;
	}
    }

    /* translate the aspect ratio to PDFlib notation */
    if (buf[6] != 0) {
        image->dpi_x = (pdc_scalar) (-(buf[6] + 15.0) / 64.0);
	image->dpi_y = -1.0;
    }

    for (/* */ ; /* */ ; /* */) {
	/* EOF / read error in image data */
        if (!PDC_OK_FREAD(image->fp, &c, 1)) {
            errcode = PDC_E_IO_READ;
            goto PDF_GIF_ERROR;
	}

#define PDF_SEMICOLON		((char) 0x3b)		/* ASCII ';'  */

	if (c == PDF_SEMICOLON) {		/* GIF terminator */
	    /* Not enough images found in file */
	    if (imageCount < image->page) {
                if (!imageCount)
                    errcode = PDF_E_IMAGE_CORRUPT;
                else
                    errcode = PDF_E_IMAGE_NOPAGE;
                goto PDF_GIF_ERROR;
	    }
	    break;
	}

#define PDF_EXCLAM		((char) 0x21)		/* ASCII '!'  */

	if (c == PDF_EXCLAM) { 	/* Extension */
            if (!PDC_OK_FREAD(image->fp, &c, 1)) {
		/* EOF / read error on extension function code */
                errcode = PDC_E_IO_READ;
                goto PDF_GIF_ERROR;
	    }
	    DoExtension(p, image, (int) c);
	    continue;
	}

#define PDF_COMMA		((char) 0x2c)		/* ASCII ','  */

	if (c != PDF_COMMA) {		/* Not a valid start character */
	    /* Bogus character, ignoring */
	    continue;
	}

	++imageCount;

        if (! PDC_OK_FREAD(image->fp, buf, 9)) {
	    /* Couldn't read left/top/width/height */
            errcode = PDC_E_IO_READ;
            goto PDF_GIF_ERROR;
	}

	image->components	= 1;
	image->bpc		= 8;
        image->width            = (pdc_scalar) pdc_get_le_ushort(&buf[4]);
        image->height           = (pdc_scalar) pdc_get_le_ushort(&buf[6]);

#define INTERLACE		0x40
	image->info.gif.interlace= BitSet(buf[8], INTERLACE);

	if (image->imagemask)
	{
	    if (p->compatibility <= PDC_1_3) {
		errcode = PDF_E_IMAGE_MASK1BIT13;
		goto PDF_GIF_ERROR;
	    } else {
		/* images with more than one bit will be written as /SMask,
		 * and don't require an /ImageMask entry.
		 */
		image->imagemask = pdc_false;
	    }
	    image->colorspace = DeviceGray;
	}

	if (BitSet(buf[8], LOCALCOLORMAP)) {
	    /* The local color map may have a different size */
	    cs.val.indexed.palette_size = 2 << (buf[8] & 0x07);

            if (ReadColorMap(p->pdc, image->fp,
                             cs.val.indexed.palette_size, &colormap))
	    {
                errcode = PDF_E_IMAGE_COLORMAP;
                goto PDF_GIF_ERROR;
	    }
	}

	if (imageCount == image->page)
	    break;
    }

    image->src.init		= pdf_data_source_GIF_init;
    image->src.fill		= pdf_data_source_GIF_fill;
    image->src.terminate	= pdf_data_source_GIF_terminate;
    image->src.private_data	= (void *) image;

    image->compression          = pdf_comp_none;
    image->use_raw  		= pdc_false;

    image->in_use               = pdc_true;             /* mark slot as used */

	slot = pdf_add_colorspace(p, &cs, pdc_false);
	image->colorspace = (pdf_colorspacetype) slot;



    pdf_put_image(p, imageslot, pdc_true, pdc_true);

    return imageslot;

    PDF_GIF_ERROR:
    {
        const char *stemp =
            pdc_errprintf(p->pdc, "%.*s", PDC_ET_MAXSTRLEN, image->filename);
        switch (errcode)
        {
            case PDC_E_IO_READ:
            case PDF_E_IMAGE_COLORMAP:
		pdc_set_errmsg(p->pdc, errcode, stemp, 0, 0, 0);
		break;

            case PDC_E_IO_BADFORMAT:
		pdc_set_errmsg(p->pdc, errcode, stemp, "GIF", 0, 0);
		break;

            case PDF_E_IMAGE_CORRUPT:
		pdc_set_errmsg(p->pdc, errcode, "GIF", stemp, 0, 0);
		break;

            case PDF_E_IMAGE_NOPAGE:
		pdc_set_errmsg(p->pdc, errcode,
		    pdc_errprintf(p->pdc, "%d", image->page), "GIF", stemp, 0);
		break;

	    case 0: 		/* error code and message already set */
		break;
        }
    }

    if (image->verbose)
	pdc_error(p->pdc, -1, 0, 0, 0, 0);

    return -1;
} /* pdf_open_GIF_data */

static int
ReadColorMap(pdc_core *pdc, pdc_file *fp, int number, pdf_colormap *buffer)
{
    int		i;
    unsigned char	rgb[3];

    (void) pdc;

    for (i = 0; i < number; ++i) {
        if (! PDC_OK_FREAD(fp, rgb, sizeof(rgb))) {
	    return pdc_true;		/* yk: true == error */
	}

	(*buffer)[i][0] = rgb[0] ;
	(*buffer)[i][1] = rgb[1] ;
	(*buffer)[i][2] = rgb[2] ;
    }
    return pdc_false;			/* yk: false == ok.  */
} /* ReadColorMap */

static int
DoExtension(PDF *p, pdf_image *image, int label)
{
    pdc_byte            buf[256];

    switch ((unsigned char) label) {
	case 0x01:		/* Plain Text Extension */
	    break;

	case 0xff:		/* Application Extension */
	    break;

	case 0xfe:		/* Comment Extension */
	    while (GetDataBlock(p, image, (unsigned char*) buf) != 0) {
		/* */
	    }
	    return pdc_false;

	case 0xf9:		/* Graphic Control Extension */
	    (void) GetDataBlock(p, image, (unsigned char*) buf);

	    if ((buf[0] & 0x1) != 0) {
		image->transparent = !image->transparent;
		image->transval[0] = buf[3];
	    }

	    while (GetDataBlock(p, image, (unsigned char*) buf) != 0) {
		    /* */ ;
	    }
	    return pdc_false;

	default:
	    break;
    }

    while (GetDataBlock(p, image, (unsigned char*) buf) != 0) {
	    /* */ ;
    }

    return pdc_false;
} /* DoExtension */

/*
 * A bunch of formely static variables which are now kept in the
 * image structure in order to keep the GIF reader thread-safe.
 */

/* for GetDataBlock() */
#define ZeroDataBlock	(image->info.gif.ZeroDataBlock)

/* for initLWZ() */
#define curbit		(image->info.gif.curbit)
#define lastbit		(image->info.gif.lastbit)
#define get_done	(image->info.gif.get_done)
#define last_byte 	(image->info.gif.last_byte)
#define return_clear 	(image->info.gif.return_clear)

#define sp		(image->info.gif.sp)
#define code_size	(image->info.gif.code_size)
#define set_code_size	(image->info.gif.set_code_size)
#define max_code	(image->info.gif.max_code)
#define max_code_size	(image->info.gif.max_code_size)
#define clear_code	(image->info.gif.clear_code)
#define end_code	(image->info.gif.end_code)

/* for nextCode() */
#define buf 		(image->info.gif.buf)

/* for nextLWZ() */
#define stack		(image->info.gif.stack)
#define table		(image->info.gif.table)
#define firstcode	(image->info.gif.firstcode)
#define oldcode		(image->info.gif.oldcode)

static int
GetDataBlock(PDF *p, pdf_image *image, unsigned char *lbuf)
{
    unsigned char	count;
    pdc_file *fp = image->fp;

    if (!PDC_OK_FREAD(fp, &count, 1))
	return -1;	/* Error in getting DataBlock size */

    ZeroDataBlock = (count == 0);

    if ((count != 0) && (!PDC_OK_FREAD(fp, lbuf, count)))
    {
	/* Error in reading DataBlock */
        const char *stemp =
            pdc_errprintf(p->pdc, "%.*s", PDC_ET_MAXSTRLEN, image->filename);

        pdc_error(p->pdc, PDF_E_IMAGE_CORRUPT, "GIF", stemp, 0, 0);
    }

    return count;
} /* GetDataBlock */

static void
initLWZ(PDF *p, pdf_image *image, int input_code_size)
{
#define GIF_TABLE_SIZE (sizeof(int [2][GIF_TABLE_ELEMENTS]))
#define GIF_STACK_SIZE (sizeof(int [GIF_TABLE_ELEMENTS*2]))

    table = (int(*)[GIF_TABLE_ELEMENTS])
    		pdc_malloc(p->pdc, GIF_TABLE_SIZE, "initLWZ");
    stack = (int *) pdc_malloc(p->pdc, GIF_STACK_SIZE, "initLWZ");

    set_code_size = input_code_size;
    code_size     = set_code_size + 1;
    clear_code    = 1 << set_code_size ;
    end_code      = clear_code + 1;
    max_code_size = 2 * clear_code;
    max_code      = clear_code + 2;

    curbit = lastbit = 0;
    last_byte = 2;
    get_done = pdc_false;

    return_clear = pdc_true;

    sp = stack;
}

/*
 * We clean up after decompressing the image; in rare cases (exception
 * caused by damaged compressed data) this may also be called when
 * cleaning up the full image struct.
 */
void
pdf_cleanup_gif(PDF *p, pdf_image *image)
{
    if (table)
    {
	pdc_free(p->pdc, table);
	table = NULL;
    }
    if (stack)
    {
	pdc_free(p->pdc, stack);
	stack = NULL;
    }
}

static int
nextCode(PDF *p, pdf_image *image, int codesize)
{
    static const int maskTbl[16] = {
	0x0000, 0x0001, 0x0003, 0x0007,
	0x000f, 0x001f, 0x003f, 0x007f,
	0x00ff, 0x01ff, 0x03ff, 0x07ff,
	0x0fff, 0x1fff, 0x3fff, 0x7fff,
    };
    int	    i, j, ret, end;

    if (return_clear) {
	return_clear = pdc_false;
	return clear_code;
    }

    end = curbit + codesize;

    if (end >= lastbit) {
	int	count;

	if (get_done) {
	    if (curbit >= lastbit)
	    {
		/*
		ERROR("ran off the end of my bits" );
		*/
		const char *stemp =
		    pdc_errprintf(p->pdc, "%.*s", PDC_ET_MAXSTRLEN,
			image->filename);
		pdc_fclose(image->fp);
		pdc_error(p->pdc, PDF_E_IMAGE_CORRUPT, "GIF", stemp, 0, 0);
	    }
	    return -1;
	}

	if (last_byte >= 2)
	{
	    buf[0] = buf[last_byte-2];
	    buf[1] = buf[last_byte-1];
	}

	if ((count = GetDataBlock(p, image, &buf[2])) == 0)
	    get_done = pdc_true;

	last_byte = 2 + count;
	curbit = (curbit - lastbit) + 16;
	lastbit = (2+count)*8 ;

	end = curbit + codesize;
    }

    j = end / 8;
    i = curbit / 8;

    if (i == j)
	ret = buf[i];
    else if (i + 1 == j)
	ret = buf[i] | (buf[i+1] << 8);
    else
	ret = buf[i] | (buf[i+1] << 8) | (buf[i+2] << 16);

    ret = (ret >> (curbit % 8)) & maskTbl[codesize];

    curbit += codesize;

    return ret;
}

#define readLWZ(p, image) ((sp > stack) ? *--sp : nextLWZ(p, image))

static int
nextLWZ(PDF *p, pdf_image *image)
{
   int	code, incode;
   int	i;

    while ((code = nextCode(p, image, code_size)) >= 0) {
       if (code == clear_code) {
	    for (i = 0; i < clear_code; ++i) {
		table[0][i] = 0;
		table[1][i] = i;
	    }
	    for (; i < (1<<MAX_LWZ_BITS); ++i)
		table[0][i] = table[1][i] = 0;
	    code_size = set_code_size+1;
	    max_code_size = 2*clear_code;
	    max_code = clear_code+2;
	    sp = stack;
	    do {
		firstcode = oldcode = nextCode(p, image, code_size);
	    } while (firstcode == clear_code);

	    return firstcode;
       }
       if (code == end_code) {
	   int             count;
	   unsigned char   lbuf[260];

	   if (ZeroDataBlock)
	       return -2;

	   while ((count = GetDataBlock(p, image, lbuf)) > 0)
	       ;

	   if (count != 0)
		/* */ ;
	       /* INFO_MSG(("missing EOD in data stream")); */

	   return -2;
       }

       incode = code;

       if (code >= max_code) {
	   *sp++ = firstcode;
	   code = oldcode;
       }

       while (code >= clear_code) {
	   *sp++ = table[1][code];
	   if (code == table[0][code])
	   {
	       /* ERROR("circular table entry BIG ERROR"); */
		const char *stemp =
		    pdc_errprintf(p->pdc, "%.*s", PDC_ET_MAXSTRLEN,
			image->filename);
		pdc_fclose(image->fp);
		pdc_error(p->pdc, PDF_E_IMAGE_CORRUPT, "GIF", stemp, 0, 0);
	    }
	   code = table[0][code];
       }

       *sp++ = firstcode = table[1][code];

       if ((code = max_code) <(1<<MAX_LWZ_BITS)) {
	   table[0][code] = oldcode;
	   table[1][code] = firstcode;
	   ++max_code;
	   if ((max_code >= max_code_size) &&
	       (max_code_size < (1<<MAX_LWZ_BITS))) {
	       max_code_size *= 2;
	       ++code_size;
	   }
       }

       oldcode = incode;

       if (sp > stack)
	   return *--sp;
   }
   return code;
}

static void
ReadImage(PDF *p, pdf_image *image, PDF_data_source *src)
{
    unsigned char	c;
    int			v;
    unsigned int	xpos = 0, ypos = 0;
    pdc_byte		*dp;
    unsigned int	h = (unsigned int) image->height;
    unsigned int	w = (unsigned int) image->width;

    /*
     *  Initialize the Compression routines
     */
    ZeroDataBlock = pdc_false;

    if (!PDC_OK_FREAD(image->fp, &c, 1))
    {
        const char *stemp =
            pdc_errprintf(p->pdc, "%.*s", PDC_ET_MAXSTRLEN, image->filename);
	pdc_fclose(image->fp);
        pdc_error(p->pdc, PDF_E_IMAGE_CORRUPT, "GIF", stemp, 0, 0);
    }

    initLWZ(p, image, c);

    if (image->info.gif.interlace) {
	int		i;
	int		pass = 0, step = 8;

	for (i = 0; i < (int) h; i++) {
	    dp = &src->buffer_start[w * ypos];
	    for (xpos = 0; xpos < w; xpos++) {
		if ((v = readLWZ(p, image)) < 0)
		    goto fini;

		*dp++ = v;
	    }
	    if ((ypos += step) >= h) {
		do {
		    if (pass++ > 0)
			step /= 2;
		    ypos = step / 2;
		} while (ypos > h);
	    }
	}
    } else {
	dp = src->buffer_start;
	for (ypos = 0; ypos < h; ypos++) {
	    for (xpos = 0; xpos < w; xpos++) {
		if ((v = readLWZ(p, image)) < 0)
		    goto fini;

		*dp++ = v;
	    }
	}
    }

fini:
    if (readLWZ(p, image) >= 0)
	/* Too much input data in GIF file '%s', ignoring extra. */
	;

    pdf_cleanup_gif(p, image);
}

#undef fresh
#undef code_size
#undef set_code_size
#undef max_code
#undef max_code_size
#undef firstcode
#undef oldcode
#undef clear_code
#undef end_code
#undef sp
#undef table
#undef stack

#endif  /* PDF_GIF_SUPPORTED */
