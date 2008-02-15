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

/* $Id: p_bmp.c,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * BMP processing for PDFlib
 *
 */

#include "p_intern.h"
#include "p_color.h"
#include "p_image.h"

#ifndef PDF_BMP_SUPPORTED

pdc_bool
pdf_is_BMP_file(PDF *p, pdc_file *fp)
{
    (void) p;
    (void) fp;

    return pdc_false;
}

int
pdf_process_BMP_data(
    PDF *p,
    int imageslot)
{
    (void) imageslot;

    pdc_warning(p->pdc, PDF_E_UNSUPP_IMAGE, "BMP", 0, 0, 0);
    return -1;
}

#else  /* !PDF_BMP_SUPPORTED */

/* for documentation only */
#if 0

/* BMP file header structure */
typedef struct
{
    pdc_ushort  bfType;           /* Magic number for file */
    pdc_uint32  bfSize;           /* Size of file */
    pdc_ushort  bfReserved1;      /* Reserved */
    pdc_ushort  bfReserved2;      /* ... */
    pdc_uint32  bfOffBits;        /* Offset to bitmap data */
}
BITMAPFILEHEADER;

/* BMP file info structure */
typedef struct
{
    pdc_uint32  biSize;           /* Size of info header */
    pdc_sint32  biWidth;          /* Width of image */
    pdc_sint32  biHeight;         /* Height of image */
    pdc_ushort  biPlanes;         /* Number of color planes */
    pdc_ushort  biBitCount;       /* Number of bits per pixel */
    pdc_uint32  biCompression;    /* Type of compression to use */
    pdc_uint32  biSizeImage;      /* Size of image data */
    pdc_sint32  biXPelsPerMeter;  /* X pixels per meter */
    pdc_sint32  biYPelsPerMeter;  /* Y pixels per meter */
    pdc_uint32  biClrUsed;        /* Number of colors used */
    pdc_uint32  biClrImportant;   /* Number of important colors */
}
BITMAPINFOHEADER;

#endif

#define PDF_GET_BYTE(pos)   *pos,                   pos += sizeof(pdc_byte)
#define PDF_GET_SHORT(pos)  pdc_get_le_short(pos),  pos += sizeof(pdc_short)
#define PDF_GET_USHORT(pos) pdc_get_le_ushort(pos), pos += sizeof(pdc_ushort)
#define PDF_GET_LONG(pos)   pdc_get_le_long(pos),   pos += sizeof(pdc_sint32)
#define PDF_GET_ULONG(pos)  pdc_get_le_ulong(pos),  pos += sizeof(pdc_uint32)

#define PDF_BMP_STRING     "\102\115"   /* "BM" */

#define PDF_BMP_RGB                 0   /* No compression - straight BGR data */
#define PDF_BMP_RLE8                1   /* 8-bit run-length compression */
#define PDF_BMP_RLE4                2   /* 4-bit run-length compression */
#define PDF_BMP_BITFIELDS           3   /* RGB bitmap with RGB masks */

#define PDF_BMP_FILE_HEADSIZE      14   /* File header size */
#define PDF_BMP_INFO_HEAD2SIZE     12   /* Info header size BMP Version 2 */
#define PDF_BMP_INFO_HEAD3SIZE     40   /* Info header size BMP Version 3 */
#define PDF_BMP_INFO_HEAD4SIZE    108   /* Info header size BMP Version 4 */

static void
pdf_data_source_BMP_init(PDF *p, PDF_data_source *src)
{
    static const char *fn = "pdf_data_source_BMP_init";
    pdf_image *image = (pdf_image *) src->private_data;

    src->buffer_length = image->info.bmp.rowbytes_buf;
    src->buffer_start = (pdc_byte *)
        pdc_calloc(p->pdc, image->info.bmp.rowbytes_pad, fn);
    src->bytes_available = image->info.bmp.rowbytes_pdf;
    src->next_byte = src->buffer_start;
}

static pdc_bool
pdf_data_source_BMP_fill(PDF *p, PDF_data_source *src)
{
    pdf_image *image = (pdf_image *) src->private_data;
    int i, j;

    if (image->info.bmp.bpp == 16)
    {
        if (image->info.bmp.pos < image->info.bmp.end)
        {
            pdc_ushort pixel, ppixel;
            int ilast = image->info.bmp.rowbytes_buf - 3;

            i = 0;
            for (j = 0; j < (int) image->info.bmp.rowbytes; j += 2)
            {
                pixel = PDF_GET_USHORT(image->info.bmp.pos);

                if (i <= ilast)
                {
                    ppixel = (pixel & image->info.bmp.redmask);
                    ppixel = (ppixel >> image->info.bmp.redmove);
                    src->buffer_start[i] = (pdc_byte) ((ppixel * 255) /
                                               image->info.bmp.redmax);
                    i++;

                    ppixel = (pixel & image->info.bmp.greenmask);
                    ppixel = (ppixel >> image->info.bmp.greenmove);
                    src->buffer_start[i] = (pdc_byte) ((ppixel * 255) /
                                               image->info.bmp.greenmax);
                    i++;

                    ppixel = (pixel & image->info.bmp.bluemask);
                    ppixel = (ppixel >> image->info.bmp.bluemove);
                    src->buffer_start[i] = (pdc_byte) ((ppixel * 255) /
                                               image->info.bmp.bluemax);
                    i++;
                }
            }
        }
        else
        {
            src->bytes_available = 0;
        }
    }

    else if (image->info.bmp.bpp == 32 ||
             image->info.bmp.compression == PDF_BMP_RGB)
    {
        size_t avail;

        /* Read 1 padded row from file */
        avail = pdc_fread(src->buffer_start, 1, image->info.bmp.rowbytes_pad,
                          image->fp);
        if (avail > 0)
        {
            /* Fill up remaining bytes */
            if (avail < image->info.bmp.rowbytes_pad)
            {
                for (i = (int) avail; i < (int) src->buffer_length; i++)
                    src->buffer_start[i] = 0;
            }

            if (image->colorspace == DeviceRGB)
            {
                /* Swap red and blue */
                if (image->info.bmp.bpp == 32)
                {
                    pdc_uint32 pixel, ppixel;
                    pdc_byte *pos = src->buffer_start;

                    i = 0;
                    for (j = 0; j < (int) image->info.bmp.rowbytes_buf; j += 4)
                    {
                        pixel = PDF_GET_ULONG(pos);

                        ppixel = (pixel & image->info.bmp.redmask);
                        ppixel = (ppixel >> image->info.bmp.redmove);
                        src->buffer_start[i] = (pdc_byte) ((ppixel * 255) /
                                                   image->info.bmp.redmax);
                        i++;

                        ppixel = (pixel & image->info.bmp.greenmask);
                        ppixel = (ppixel >> image->info.bmp.greenmove);
                        src->buffer_start[i] = (pdc_byte) ((ppixel * 255) /
                                                   image->info.bmp.greenmax);
                        i++;

                        ppixel = (pixel & image->info.bmp.bluemask);
                        ppixel = (ppixel >> image->info.bmp.bluemove);
                        src->buffer_start[i] = (pdc_byte) ((ppixel * 255) /
                                                   image->info.bmp.bluemax);
                        i++;
                    }
                }
                else
                {
                    for (j = 0; j < (int) image->info.bmp.rowbytes_buf; j += 3)
                    {
                        pdc_byte c = src->buffer_start[j];
                        src->buffer_start[j] = src->buffer_start[j + 2];
                        src->buffer_start[j + 2] = c;
                    }
                }
            }
        }
        else
        {
            src->bytes_available = 0;
        }
    }

    /* Compression methods RLE8 and RLE4 */
    else
    {
        int col = 0;
        int fnibble = 1;
        pdc_byte c, cc, ccc, cn[2], ccn;

        if (image->info.bmp.pos < image->info.bmp.end)
        {
            if (image->info.bmp.skiprows)
            {
                for (; col < (int) image->info.bmp.rowbytes; col++)
                    src->buffer_start[col] = 0;
                image->info.bmp.skiprows--;
            }
            else
            {
                while (1)
                {
                    c = *image->info.bmp.pos;
                    image->info.bmp.pos++;
                    if (image->info.bmp.pos >= image->info.bmp.end)
                        goto PDF_BMP_CORRUPT;
                    cc = *image->info.bmp.pos;

                    if (c != 0)
                    {
                        /* Repeat c time pixel value */
                        if (image->info.bmp.compression == PDF_BMP_RLE8)
                        {
                            for (i = 0; i < (int) c; i++)
                            {
                                if (col >= (int) image->info.bmp.rowbytes)
                                    goto PDF_BMP_CORRUPT;
                                src->buffer_start[col] = cc;
                                col++;
                            }
                        }
                        else
                        {
                            cn[0] = (pdc_byte) ((cc & 0xF0) >> 4);
                            cn[1] = (pdc_byte) (cc & 0x0F);
                            for (i = 0; i < (int) c; i++)
                            {
                                if (col >= (int) image->info.bmp.rowbytes)
                                    goto PDF_BMP_CORRUPT;
                                ccn = cn[i%2];
                                if (fnibble)
                                {
                                    fnibble = 0;
                                    src->buffer_start[col] =
                                        (pdc_byte) (ccn << 4);
                                }
                                else
                                {
                                    fnibble = 1;
                                    src->buffer_start[col] |= ccn;
                                    col++;
                                }
                            }
                        }
                    }
                    else if (cc > 2)
                    {
                        /* cc different pixel values */
                        if (image->info.bmp.compression == PDF_BMP_RLE8)
                        {
                            for (i = 0; i < (int) cc; i++)
                            {
                                image->info.bmp.pos++;
                                if (image->info.bmp.pos >= image->info.bmp.end)
                                    goto PDF_BMP_CORRUPT;
                                if (col >= (int) image->info.bmp.rowbytes)
                                    goto PDF_BMP_CORRUPT;
                                src->buffer_start[col] = *image->info.bmp.pos;
                                col++;
                            }
                        }
                        else
                        {
                            for (i = 0; i < (int) cc; i++)
                            {
                                if (!(i%2))
                                {
                                    image->info.bmp.pos++;
                                    if (image->info.bmp.pos >=
                                        image->info.bmp.end)
                                        goto PDF_BMP_CORRUPT;
                                    ccc = *image->info.bmp.pos;
                                    cn[0] = (pdc_byte) ((ccc & 0xF0) >> 4);
                                    cn[1] = (pdc_byte) (ccc & 0x0F);
                                }
                                if (col >= (int) image->info.bmp.rowbytes)
                                    goto PDF_BMP_CORRUPT;
                                ccn = cn[i%2];
                                if (fnibble)
                                {
                                    fnibble = 0;
                                    src->buffer_start[col] =
                                        (pdc_byte) (ccn << 4);
                                }
                                else
                                {
                                    fnibble = 1;
                                    src->buffer_start[col] |= ccn;
                                    col++;
                                }
                            }
                            if (cc % 2) cc++;
                            cc /= 2;
                        }

                        /* Odd number of bytes */
                        if (cc % 2)
                            image->info.bmp.pos++;
                    }
                    else if (cc < 2)
                    {
                        /* End of scan line or end of bitmap data*/
                        for (; col < (int) image->info.bmp.rowbytes; col++)
                            src->buffer_start[col] = 0;
                    }
                    else if (cc == 2)
                    {
                        int cola;

                        /* Run offset marker */
                        if (image->info.bmp.pos >= image->info.bmp.end - 1)
                            goto PDF_BMP_CORRUPT;
                        image->info.bmp.pos++;
                        c = *image->info.bmp.pos;
                        image->info.bmp.pos++;
                        cc = *image->info.bmp.pos;

                        /* Fill current row */
                        cola = col;
                        for (; col < (int) image->info.bmp.rowbytes; col++)
                            src->buffer_start[col] = 0;
                        if (col - cola != (int) c)
                            goto PDF_BMP_CORRUPT;

                        /* Number of rows to be skipped */
                        image->info.bmp.skiprows = (size_t) cc;
                    }

                    image->info.bmp.pos++;
                    if (col >= (int) image->info.bmp.rowbytes)
                    {
                        /* Skip end of scan line marker */
                        if (image->info.bmp.pos < image->info.bmp.end - 1)
                        {
                            c = *image->info.bmp.pos;
                            cc = *(image->info.bmp.pos + 1);
                            if(cc == 0 && cc <= 1)
                                image->info.bmp.pos += 2;
                        }
                        break;
                    }
                }
            }
        }
        else
        {
            src->bytes_available = 0;
        }
    }

    return (src->bytes_available ? pdc_true : pdc_false);

    PDF_BMP_CORRUPT:
    pdc_error(p->pdc, PDF_E_IMAGE_CORRUPT, "BMP",
        pdc_errprintf(p->pdc, "%.*s", PDC_ET_MAXSTRLEN, image->filename), 0, 0);
    src->bytes_available = 0;
    return pdc_false;
}

static void
pdf_data_source_BMP_terminate(PDF *p, PDF_data_source *src)
{
    pdf_image *image = (pdf_image *) src->private_data;

    pdc_free(p->pdc, (void *) src->buffer_start);
    if (image->info.bmp.bitmap != NULL)
        pdc_free(p->pdc, (void *) image->info.bmp.bitmap);
}

pdc_bool
pdf_is_BMP_file(PDF *p, pdc_file *fp)
{
    pdc_byte buf[2];

    (void) p;

    if (pdc_fread(buf, 1, 2, fp) < 2 ||
        strncmp((const char *) buf, PDF_BMP_STRING, 2) != 0)
    {
        pdc_fseek(fp, 0L, SEEK_SET);
        return pdc_false;
    }
    return pdc_true;
}

int
pdf_process_BMP_data(
    PDF *p,
    int imageslot)
{
    static const char *fn = "pdf_process_BMP_data";
    pdc_byte buf[256], *pos, *cmap, bdummy;
    pdf_image *image = &p->images[imageslot];
    pdc_file *fp = image->fp;
    pdc_uint32 uldummy, infosize = 0, offras = 0, planes = 0, bitmapsize = 0;
    pdc_uint32 ncolors = 0, importcolors = 0, compression = PDF_BMP_RGB;
    pdc_ushort usdummy, bpp = 0, bpp_pdf = 0;
    pdc_sint32 width = 0, height = 0, dpi_x = 0, dpi_y = 0;
    pdc_uint32 redmask = 0, greenmask = 0, bluemask = 0, ccmax;
    size_t nbytes;
    pdf_colorspace cs;
    pdf_colormap colormap;
    int i, slot, colsize = 0, errcode = 0;

    /* Error reading magic number or not a BMP file */
    if (pdf_is_BMP_file(p, image->fp) == pdc_false)
    {
        errcode = PDC_E_IO_BADFORMAT;
        goto PDF_BMP_ERROR;
    }

    /* read file header without FileType field + */
    /* Size field of info header */
    pos = &buf[2];
    nbytes = PDF_BMP_FILE_HEADSIZE - 2 + 4;
    if (!PDC_OK_FREAD(fp, pos, nbytes))
    {
        errcode = PDF_E_IMAGE_CORRUPT;
        goto PDF_BMP_ERROR;
    }
    uldummy = PDF_GET_ULONG(pos);
    usdummy = PDF_GET_USHORT(pos);
    usdummy = PDF_GET_USHORT(pos);
    offras = PDF_GET_ULONG(pos);
    infosize = PDF_GET_ULONG(pos);

    /* no support of later version than 3 */
    if (infosize != PDF_BMP_INFO_HEAD2SIZE &&
        infosize != PDF_BMP_INFO_HEAD3SIZE)
    {
        errcode = PDF_E_BMP_VERSUNSUPP;
        goto PDF_BMP_ERROR;
    }

    /* info header */
    pos = buf;
    nbytes = infosize - 4;
    if (!PDC_OK_FREAD(fp, pos, nbytes))
    {
        errcode = PDF_E_IMAGE_CORRUPT;
        goto PDF_BMP_ERROR;
    }
    if (infosize == PDF_BMP_INFO_HEAD2SIZE)
    {
        width = PDF_GET_SHORT(pos);
        height = PDF_GET_SHORT(pos);
        planes = PDF_GET_USHORT(pos);
        bpp = PDF_GET_USHORT(pos);
        colsize = 3;
    }
    else if (infosize == PDF_BMP_INFO_HEAD3SIZE)
    {
        width = PDF_GET_LONG(pos);
        height = PDF_GET_LONG(pos);
        planes = PDF_GET_USHORT(pos);
        bpp = PDF_GET_USHORT(pos);
        compression = PDF_GET_ULONG(pos);
        bitmapsize = PDF_GET_ULONG(pos);
        dpi_x = PDF_GET_LONG(pos);
        dpi_y = PDF_GET_LONG(pos);
        ncolors = PDF_GET_ULONG(pos);
        importcolors = PDF_GET_ULONG(pos);
        colsize = 4;
    }

    /* compressed BMP images by bitfields */
    if (compression == PDF_BMP_BITFIELDS)
    {
        pos = buf;
        nbytes = 3 * sizeof(pdc_uint32);
        if (!PDC_OK_FREAD(fp, pos, nbytes))
        {
            errcode = PDF_E_IMAGE_CORRUPT;
            goto PDF_BMP_ERROR;
        }
        else
        {
            redmask = PDF_GET_ULONG(pos);
            greenmask = PDF_GET_ULONG(pos);
            bluemask = PDF_GET_ULONG(pos);
        }
    }

    pdc_trace_protocol(p->pdc, 5, trc_image,
                       "\t\t\tinfosize = %d\n"
                       "\t\t\twidth = %d\n"
                       "\t\t\theight = %d\n"
                       "\t\t\tplanes = %d\n"
                       "\t\t\tbpp = %d\n"
                       "\t\t\tcompression = %d\n"
                       "\t\t\tbitmapsize = %d\n"
                       "\t\t\tdpi_x = %d\n"
                       "\t\t\tdpi_y = %d\n"
                       "\t\t\tncolors = %d\n"
                       "\t\t\timportcolors = %d\n"
                       "\t\t\tcolsize = %d\n"
                       "\t\t\tredmask   = 0x%08X\n"
                       "\t\t\tgreenmask = 0x%08X\n"
                       "\t\t\tbluemask  = 0x%08X\n",
                       infosize, width, height, planes, bpp, compression,
                       bitmapsize, dpi_x, dpi_y, ncolors, importcolors,
                       colsize, redmask, greenmask, bluemask);

    image->bpc = bpp;
    image->width = width;
    image->height = -height;
    image->dpi_x = (pdc_scalar) (PDC_INCH2METER * dpi_x);
    image->dpi_y = (pdc_scalar) (PDC_INCH2METER * dpi_y);
    image->info.bmp.bpp = bpp;
    bpp_pdf = bpp;

    /* color map only for bpp = 1, 4, 8 */
    if (bpp < 16)
    {
        if (!ncolors)
            ncolors = (pdc_uint32) (1 << bpp);
        if (ncolors > (offras - PDF_BMP_FILE_HEADSIZE - infosize) / colsize)
        {
            errcode = PDF_E_IMAGE_CORRUPT;
            goto PDF_BMP_ERROR;
        }

        /* allocate and read color map */
        nbytes = colsize * ncolors;
        cmap = (pdc_byte *) pdc_malloc(p->pdc, nbytes, fn);
        if (!PDC_OK_FREAD(fp, cmap, nbytes))
        {
            errcode = PDF_E_IMAGE_CORRUPT;
            goto PDF_BMP_ERROR;
        }

        /* set color map (bgr) */
        pos = cmap;
        for (i = 0; i < (int) ncolors; i++)
        {
            colormap[i][2] = PDF_GET_BYTE(pos);
            colormap[i][1] = PDF_GET_BYTE(pos);
            colormap[i][0] = PDF_GET_BYTE(pos);
            if (infosize == PDF_BMP_INFO_HEAD3SIZE)
            {
                bdummy = PDF_GET_BYTE(pos);
            }
        }
        pdc_free(p->pdc, cmap);

        pdc_trace_protocol(p->pdc, 5, trc_image,
                           "\t\t\tcolor map with %d colors generated\n",
                           ncolors);

        image->components = 1;

	    cs.type = Indexed;
	    cs.val.indexed.base = DeviceRGB;
	    cs.val.indexed.palette_size = (int) ncolors;
	    cs.val.indexed.colormap = &colormap;
	    cs.val.indexed.colormap_id = PDC_BAD_ID;
	    slot = pdf_add_colorspace(p, &cs, pdc_false);

	    image->colorspace = (pdf_colorspacetype) slot;


    }
    else
    {
        image->colorspace = DeviceRGB;
        image->components = 3;
        image->bpc = 8;
        if (bpp == 16)
        {
            bpp = 24;
            bpp_pdf = 24;
            if (compression == PDF_BMP_RGB)
            {
                redmask   = 0x00007C00;
                greenmask = 0x000003E0;
                bluemask  = 0x0000001F;
            }
        }
        if (bpp == 32)
        {
            bpp_pdf = 24;
            if (compression == PDF_BMP_RGB)
            {
                redmask   = 0x00FF0000;
                greenmask = 0x0000FF00;
                bluemask  = 0x000000FF;
            }
        }

        /* maximum and movement */
        if (image->info.bmp.bpp != 24)
        {
            for (i = 0; i < 32; i++)
            {
                ccmax = (redmask >> i);
                if (ccmax & 0x00000001)
                    break;
            }
            image->info.bmp.redmask = redmask;
            image->info.bmp.redmax = ccmax;
            image->info.bmp.redmove = i;


            for (i = 0; i < 32; i++)
            {
                ccmax = (greenmask >> i);
                if (ccmax & 0x00000001)
                    break;
            }
            image->info.bmp.greenmask = greenmask;
            image->info.bmp.greenmax = ccmax;
            image->info.bmp.greenmove = i;

            for (i = 0; i < 32; i++)
            {
                ccmax = (bluemask >> i);
                if (ccmax & 0x00000001)
                    break;
            }
            image->info.bmp.bluemask = bluemask;
            image->info.bmp.bluemax = ccmax;
            image->info.bmp.bluemove = i;
        }
    }

    if (image->imagemask)
    {
	if (image->components != 1) {
	    errcode = PDF_E_IMAGE_BADMASK;
	    goto PDF_BMP_ERROR;
	}

        if (p->compatibility <= PDC_1_3) {
            if (image->components != 1 || image->bpc != 1) {
                errcode = PDF_E_IMAGE_MASK1BIT13;
                goto PDF_BMP_ERROR;
            }
        } else if (image->bpc > 1) {
            /* images with more than one bit will be written as /SMask,
             * and don't require an /ImageMask entry.
             */
            image->imagemask = pdc_false;
        }
        image->colorspace = DeviceGray;
    }


    /* we invert this flag later */
    if (image->ignoremask)
        image->transparent = pdc_true;

    /* number of bytes per row  */
    image->info.bmp.rowbytes_pdf = (size_t) ((bpp_pdf * width + 7) / 8);
    image->info.bmp.rowbytes_buf = (size_t) ((bpp * width + 7) / 8);
    if (bpp == 4)
        image->info.bmp.rowbytes = image->info.bmp.rowbytes_buf;
    else if (image->info.bmp.bpp == 16)
        image->info.bmp.rowbytes =
                   (size_t) (4 * ((image->info.bmp.bpp * width + 31) / 32));
    else
        image->info.bmp.rowbytes = (size_t) ((bpp * width) / 8);
    image->info.bmp.rowbytes_pad = (size_t) (4 * ((bpp * width + 31) / 32));
    image->info.bmp.compression = compression;
    image->info.bmp.skiprows = 0;
    image->info.bmp.bitmap = NULL;

    pdc_trace_protocol(p->pdc, 5, trc_image,
                       "\t\t\tinternal variables:\n"
                       "\t\t\t\tbpp_pdf = %d\n"
                       "\t\t\t\trowbytes = %d\n"
                       "\t\t\t\trowbytes_buf = %d\n"
                       "\t\t\t\trowbytes_pdf = %d\n"
                       "\t\t\t\trowbytes_pad = %d\n",
    bpp_pdf, image->info.bmp.rowbytes, image->info.bmp.rowbytes_buf,
    image->info.bmp.rowbytes_pdf, image->info.bmp.rowbytes_pad);

    /* read whole bitmap */
    if (image->info.bmp.bpp < 24 && (image->info.bmp.bpp == 16 ||
        image->info.bmp.compression != PDF_BMP_RGB))
    {
        if (!bitmapsize)
        {
            bitmapsize = (int) (pdc_file_size(fp) - pdc_ftell(fp));
            pdc_trace_protocol(p->pdc, 5, trc_image,
                               "\t\t\tcalculated bitmapsize = %d\n",
                               bitmapsize);
        }

        image->info.bmp.bitmap =
            (pdc_byte *) pdc_malloc(p->pdc, bitmapsize, fn);
        if (!PDC_OK_FREAD(fp, image->info.bmp.bitmap, bitmapsize))
        {
            pdc_free(p->pdc, (void *) image->info.bmp.bitmap);
            errcode = PDF_E_IMAGE_CORRUPT;
            goto PDF_BMP_ERROR;
        }
        image->info.bmp.pos = image->info.bmp.bitmap;
        image->info.bmp.end = image->info.bmp.bitmap + bitmapsize;
    }

#if 0
if (image->info.bmp.bpp == 16)
goto PDF_BMP_ERROR;
#endif

    /* offset bitmap data */
    pdc_fseek(image->fp, (pdc_sint32) offras, SEEK_SET);

    /* put image data */
    image->src.init = pdf_data_source_BMP_init;
    image->src.fill = pdf_data_source_BMP_fill;
    image->src.terminate = pdf_data_source_BMP_terminate;
    image->src.private_data  = (void *) image;

    image->use_raw = pdc_false;
    image->in_use = pdc_true;

    pdf_put_image(p, imageslot, pdc_true, pdc_true);

    return imageslot;

    PDF_BMP_ERROR:
    {
        const char *stemp =
            pdc_errprintf(p->pdc, "%.*s", PDC_ET_MAXSTRLEN, image->filename);
        switch (errcode)
        {
            case PDF_E_IMAGE_MASK1BIT13:
            case PDF_E_BMP_VERSUNSUPP:
            case PDF_E_BMP_COMPUNSUPP:
            case PDF_E_IMAGE_BADMASK:
		pdc_set_errmsg(p->pdc, errcode, stemp, 0, 0, 0);
		break;

            case PDC_E_IO_BADFORMAT:
		pdc_set_errmsg(p->pdc, errcode, stemp, "BMP", 0, 0);
		break;

            case PDF_E_IMAGE_CORRUPT:
		pdc_set_errmsg(p->pdc, errcode, "BMP", stemp, 0, 0);
		break;

	    case 0: 		/* error code and message already set */
		break;
        }
    }

    if (image->verbose)
	pdc_error(p->pdc, -1, 0, 0, 0, 0);

    return -1;
}

#endif /* PDF_BMP_SUPPORTED */

