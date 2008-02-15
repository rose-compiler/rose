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

/* $Id: p_jpeg.c,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * JPEG processing for PDFlib
 *
 */

#include "p_intern.h"
#include "p_color.h"
#include "p_image.h"

#ifndef PDF_JPEG_SUPPORTED

pdc_bool
pdf_is_JPEG_file(PDF *p, pdc_file *fp)
{
    (void) p;
    (void) fp;

    return pdc_false;
}

int
pdf_process_JPEG_data(
    PDF *p,
    int imageslot)
{
    (void) imageslot;

    pdc_warning(p->pdc, PDF_E_UNSUPP_IMAGE, "JPEG", 0, 0, 0);
    return -1;
}

#else

#include "jinclude.h"
#include "jpeglib.h"

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
  M_APP1  = 0xe1,	/* application marker, used for Exif	*/
  M_APP2  = 0xe2,	/* application marker, used for FlashPix*
                         * and ICC Profiles                     */
  M_APP3  = 0xe3,	/* application marker			*/
  M_APP4  = 0xe4,	/* application marker			*/
  M_APP5  = 0xe5,	/* application marker			*/
  M_APP6  = 0xe6,	/* application marker			*/
  M_APP7  = 0xe7,	/* application marker			*/
  M_APP8  = 0xe8,	/* application marker, used for SPIFF	*/
  M_APP9  = 0xe9,	/* application marker			*/
  M_APP10 = 0xea,	/* application marker			*/
  M_APP11 = 0xeb,	/* application marker			*/
  M_APP12 = 0xec,	/* application marker			*/
  M_APP13 = 0xed,	/* application marker, used by Photoshop*/
  M_APP14 = 0xee,	/* application marker, used by Adobe	*/
  M_APP15 = 0xef,	/* application marker			*/

  M_JPG0  = 0xf0,	/* reserved for JPEG extensions		*/
  M_JPG13 = 0xfd,	/* reserved for JPEG extensions		*/
  M_COM   = 0xfe,	/* comment				*/

  M_TEM   = 0x01 	/* temporary use			*/

} JPEG_MARKER;

#define JPEG_SEGLIST_CHUNKSIZE  64
#define JPEG_MARKER_LEN         2
#define JPEG_LENGTH_LEN         2
#define JPEG_BUFSIZE            0xFFFF

struct pdf_jpeg_segment_s
{
    long pos;           /* position of segment */
    size_t length;      /* length of segement in byte */
};

static void
pdf_register_JPEG_segment(PDF *p, pdf_image *image, long pos, size_t length)
{
    static const char fn[] = "pdf_register_JPEG_segment";
    pdf_jpeg_info *jpeg = &image->info.jpeg;
    size_t len;

    pdc_trace_protocol(p->pdc, 5, trc_image,
            "\t\t\tKeep segment, position = 0x%lX, length = 0x%lX(%ld)\n",
            pos, length, length);

    while(length > 0)
    {
        len = length;
        if (len > JPEG_BUFSIZE)
            len = JPEG_BUFSIZE;

        if (jpeg->number >= jpeg->capacity)
        {
            if (jpeg->capacity == 0)
            {
                jpeg->capacity = JPEG_SEGLIST_CHUNKSIZE;
                jpeg->seglist = (pdf_jpeg_segment *) pdc_malloc(p->pdc,
			       jpeg->capacity * sizeof(pdf_jpeg_segment), fn);
            }
            else
            {
                jpeg->capacity += JPEG_SEGLIST_CHUNKSIZE;
                jpeg->seglist = (pdf_jpeg_segment *) pdc_realloc(p->pdc,
		 jpeg->seglist, jpeg->capacity* sizeof(pdf_jpeg_segment), fn);
            }
        }
        jpeg->seglist[jpeg->number].pos = pos;
        jpeg->seglist[jpeg->number].length = len;
        jpeg->number++;

        length -= len;
        pos += len;
    }
}

static void
pdf_data_source_JPEG_init(PDF *p, PDF_data_source *src)
{
    static const char fn[] = "pdf_data_source_JPEG_init";
    pdf_image *image;
    pdf_jpeg_info *jpeg;

    image = (pdf_image *) src->private_data;
    jpeg = &image->info.jpeg;

    jpeg->capacity = jpeg->number;
    jpeg->number = 0;

    src->buffer_start = (pdc_byte *) pdc_malloc(p->pdc, JPEG_BUFSIZE, fn);
    src->buffer_length = JPEG_BUFSIZE;
}

static pdc_bool
pdf_data_source_JPEG_fill(PDF *p, PDF_data_source *src)
{
    pdf_image *image;
    pdf_jpeg_info *jpeg;
    size_t length;
    long pos;

    (void) p;

    image = (pdf_image *) src->private_data;
    jpeg = &image->info.jpeg;

    if (jpeg->number < jpeg->capacity)
    {
        pos = jpeg->seglist[jpeg->number].pos;
        length = jpeg->seglist[jpeg->number].length;
        jpeg->number++;

        pdc_fseek(image->fp, pos, SEEK_SET);

        src->next_byte = src->buffer_start;
        src->bytes_available =
            pdc_fread(src->buffer_start, 1, length, image->fp);
    }
    else
    {
        src->bytes_available = 0;
    }

    if (src->bytes_available == 0)
        return pdc_false;
    else
        return pdc_true;
}

static void
pdf_data_source_JPEG_terminate(PDF *p, PDF_data_source *src)
{
    pdc_free(p->pdc, (void *) src->buffer_start);
}

/**********************************************************************
 *
 * Decompression data source routines for the case of
 * reading JPEG data from a PDFlib virtual file in
 * JPEG library - analogous to ../libs/jpeg/jdatasrc.c
 *
 **********************************************************************/

typedef struct
{
    struct jpeg_source_mgr pub;  /* public fields */
    pdc_file *infile;
    PDF *p;			 /* required for logging only */
}
pdf_source_mgr;

typedef pdf_source_mgr * pdf_src_ptr;

static void
pdf_init_JPEG_source (j_decompress_ptr cinfo)
{
    (void) cinfo;
}

static boolean
pdf_fill_JPEG_input_buffer (j_decompress_ptr cinfo)
{
    pdf_src_ptr src = (pdf_src_ptr) cinfo->src;
    JOCTET *buffer;
    size_t nbytes;

    buffer = (JOCTET *) pdc_freadall(src->infile, &nbytes, NULL);

    src->pub.next_input_byte = buffer;
    src->pub.bytes_in_buffer = nbytes;

    return TRUE;
}

static void
pdf_skip_JPEG_input_data (j_decompress_ptr cinfo, long num_bytes)
{
    pdf_src_ptr src = (pdf_src_ptr) cinfo->src;

    src->pub.next_input_byte += (size_t) num_bytes;
    src->pub.bytes_in_buffer -= (size_t) num_bytes;
}

static void
pdf_term_JPEG_source (j_decompress_ptr cinfo)
{
    (void) cinfo;
}

static void
pdf_jpeg_pdcread_src (j_decompress_ptr cinfo, PDF *p, pdc_file * infile)
{
    pdf_src_ptr src;

    cinfo->src = (struct jpeg_source_mgr *)
            (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
                                        SIZEOF(pdf_source_mgr));

    src = (pdf_src_ptr) cinfo->src;
    src->pub.init_source = pdf_init_JPEG_source;
    src->pub.fill_input_buffer = pdf_fill_JPEG_input_buffer;
    src->pub.skip_input_data = pdf_skip_JPEG_input_data;
    src->pub.resync_to_restart = jpeg_resync_to_restart;
    src->pub.term_source = pdf_term_JPEG_source;
    src->infile = infile;
    src->p = p;
    src->pub.bytes_in_buffer = 0;
    src->pub.next_input_byte = NULL;
}

/**********************************************************************
 *
 * Compression data destination routines for the case of
 * emitting JPEG data to a open PDFlib PDF file in
 * JPEG library - analogous to ../libs/jpeg/jdatadst.c
 *
 **********************************************************************/

typedef struct
{
    struct jpeg_destination_mgr pub;
    PDF *p;
    JOCTET *buffer;
}
pdf_destination_mgr;

typedef pdf_destination_mgr * pdf_dest_ptr;

#define OUTPUT_BUF_SIZE  4096

static void
pdf_init_JPEG_destination (j_compress_ptr cinfo)
{
    pdf_dest_ptr dest = (pdf_dest_ptr) cinfo->dest;

    dest->buffer = (JOCTET *)
        (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
                                  OUTPUT_BUF_SIZE * SIZEOF(JOCTET));

    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
}

static boolean
pdf_empty_JPEG_output_buffer (j_compress_ptr cinfo)
{
    pdf_dest_ptr dest = (pdf_dest_ptr) cinfo->dest;

    pdc_write(dest->p->out, dest->buffer, OUTPUT_BUF_SIZE);

    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;

    return TRUE;
}

static void
pdf_term_JPEG_destination (j_compress_ptr cinfo)
{
    pdf_dest_ptr dest = (pdf_dest_ptr) cinfo->dest;
    size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

    if (datacount)
        pdc_write(dest->p->out, dest->buffer, datacount);
}

static void
pdf_jpeg_pdcwrite_dest (j_compress_ptr cinfo, PDF *p)
{
    pdf_dest_ptr dest;

    cinfo->dest = (struct jpeg_destination_mgr *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
                                  SIZEOF(pdf_destination_mgr));

    dest = (pdf_dest_ptr) cinfo->dest;
    dest->pub.init_destination = pdf_init_JPEG_destination;
    dest->pub.empty_output_buffer = pdf_empty_JPEG_output_buffer;
    dest->pub.term_destination = pdf_term_JPEG_destination;
    dest->p = p;
}

/**********************************************************************/

#define PDF_JMSG_LENGTH_MAX  200

/*
 * Private replacement for libjpeg's error message function.
 * It serves two purposes:
 * - avoid libjpeg writing to stderr
 * - write the message to the log file if logging is enabled
 */

static void
pdf_output_message(j_common_ptr cinfo)
{
    char buffer[PDF_JMSG_LENGTH_MAX];

    /* we use this method only for decompression objects */
    j_decompress_ptr cinfo2 = (j_decompress_ptr) cinfo;
    pdf_source_mgr *src = (pdf_source_mgr *) cinfo2->src;

    if (!pdc_trace_protocol_is_enabled(src->p->pdc, 5, trc_image))
	return;

    /* Create the message */
    (*cinfo->err->format_message) (cinfo, buffer);
    
    pdc_trace(src->p->pdc, "\t\tlibjpeg: %s\n", buffer);
}
#undef PDF_JMSG_LENGTH_MAX

static pdc_bool
pdf_data_source_JPEG_fill_transcode(PDF *p, PDF_data_source *src)
{
    pdf_image *image = (pdf_image *) src->private_data;

    struct jpeg_decompress_struct srcinfo;
    struct jpeg_compress_struct dstinfo;
    jvirt_barray_ptr * src_coef_arrays;
    struct jpeg_error_mgr jsrcerr, jdsterr;

    /* Initialize the JPEG decompression object with default error handling. */
    srcinfo.err = jpeg_std_error(&jsrcerr);

    /* Hook up our own message handler for logging */
    srcinfo.err->output_message = pdf_output_message;

    /* Extended libjpeg tracing if PDFlib logging is enabled */
    if (pdc_trace_protocol_is_enabled(p->pdc, 5, trc_image))
	srcinfo.err->trace_level = 1;

    jpeg_create_decompress(&srcinfo);

    /* Initialize the JPEG compression object with default error handling. */
    dstinfo.err = jpeg_std_error(&jdsterr);
    jpeg_create_compress(&dstinfo);

    /* Specify data source for decompression analogous to jpeg_stdio_src */
    pdf_jpeg_pdcread_src(&srcinfo, p, image->fp);

    /* Read file header */
    (void) jpeg_read_header(&srcinfo, TRUE);

    /* Read source file as DCT coefficients */
    src_coef_arrays = jpeg_read_coefficients(&srcinfo);

    /* Initialize destination compression parameters from source values */
    jpeg_copy_critical_parameters(&srcinfo, &dstinfo);

    /* Specify data destination for compression analogous to jpeg_stdio_dest */
    pdf_jpeg_pdcwrite_dest(&dstinfo, p);

    /* Start compressor (note no image data is actually written here) */
    jpeg_write_coefficients(&dstinfo, src_coef_arrays);

    /* Finish compression and release memory */
    jpeg_finish_compress(&dstinfo);
    jpeg_destroy_compress(&dstinfo);
    (void) jpeg_finish_decompress(&srcinfo);
    jpeg_destroy_decompress(&srcinfo);

    /* All done. */
    if (jsrcerr.num_warnings + jdsterr.num_warnings != 0)
    {
        const char *stemp =
            pdc_errprintf(p->pdc, "%.*s", PDC_ET_MAXSTRLEN, image->filename);

	if (pdc_trace_protocol_is_enabled(p->pdc, 5, trc_image))
	{
	    if (jsrcerr.num_warnings > 0)
		pdc_trace(p->pdc,
		    "\t\t\tlibjpeg total: %d corrupt data warning(s)\n",
		    jsrcerr.num_warnings);

	    if (jdsterr.num_warnings > 0)
		pdc_trace(p->pdc, "\t\t\tlibjpeg: %d warning(s) for output\n",
		    jdsterr.num_warnings);
	}

        pdc_set_errmsg(p->pdc, PDF_E_JPEG_TRANSCODE, stemp, 0, 0, 0);
        if (image->verbose)
            pdc_error(p->pdc, -1, 0, 0, 0, 0);
    }

    return pdc_false;
}

static pdc_ushort
get_ushort(pdc_file *fp)
{
    pdc_byte c[2];

    c[0] = (pdc_byte) pdc_fgetc(fp);
    c[1] = (pdc_byte) pdc_fgetc(fp);

    return pdc_get_be_ushort(c);
}

pdc_bool
pdf_is_JPEG_file(PDF *p, pdc_file *fp)
{
    long pos = 0L;
    int c;

    (void) p;

#if !defined(MVS) || !defined(I370)
    /* Tommy's special trick for Macintosh JPEGs: simply skip some  */
    /* hundred bytes at the beginning of the file!                  */
    do {
        do {                            /* skip if not FF           */
            c = pdc_fgetc(fp);
        } while (!pdc_feof(fp) && c != 0xFF);

        if (pdc_feof(fp)) {
            pdc_fseek(fp, 0L, SEEK_SET);
            return pdc_false;
        }

        do {                            /* skip repeated FFs        */
            c = pdc_fgetc(fp);
        } while (c == 0xFF);

        /* remember start position */
        if ((pos = (pdc_off_t1) pdc_ftell(fp)) < 0L) {
            pdc_fseek(fp, 0L, SEEK_SET);
            return pdc_false;
        }

        pos -= JPEG_MARKER_LEN;     /* subtract marker length     */

        if (c == M_SOI) {
            pdc_fseek(fp, pos, SEEK_SET);
            break;
        }
    } while (!pdc_feof(fp));
#endif  /* !MVS || !I370 */

#define BOGUS_LENGTH    768
    /* Heuristics: if we are that far from the start chances are
     * it is a TIFF file with embedded JPEG data which we cannot
     * handle - regard as hopeless...
     */
    if (pdc_feof(fp) || pos > BOGUS_LENGTH) {
        pdc_fseek(fp, 0L, SEEK_SET);
        return pdc_false;
    }

    return pdc_true;
}

/* open JPEG image and analyze marker */
int
pdf_process_JPEG_data(
    PDF *p,
    int imageslot)
{
    static const char fn[] = "pdf_process_JPEG_data";
    int c, unit;
    unsigned long length, len, slen;
#define APP_MAX 255
    pdc_byte appstring[APP_MAX];
    pdc_byte *app13 = NULL;
    const char *filename = NULL;
    pdc_bool ismem = pdc_false;
    void  *filebase = NULL;
    size_t filelen;
    pdc_byte *s;
    pdf_image *image;
    pdc_bool adobeflag = pdc_false;
    pdc_bool marker_found = pdc_false;
    pdc_bool markers_done = pdc_false;
    pdc_bool need_transcode = pdc_false;
    pdc_bool traceprotocol = pdc_false;
    long pos = 0, endpos = 0;
    int errint = 0;
    int errcode = 0;

    image = &p->images[imageslot];
    image->compression = pdf_comp_dct;
    image->use_raw = pdc_true;
    image->info.jpeg.virtfile = NULL;
    image->info.jpeg.seglist = NULL;
    image->info.jpeg.capacity = 0;
    image->info.jpeg.number = 0;

    need_transcode = !image->passthrough;
    traceprotocol = pdc_trace_protocol_is_enabled(p->pdc, 5, trc_image);

    if (traceprotocol)
    {
	pdc_trace(p->pdc, "\t\t\tjpegoptimize = %s\n",
		image->jpegoptimize ? "true" : "false");
    }

    /* jpeg file not available */
    if (image->reference != pdf_ref_direct)
    {

        image->in_use = pdc_true;                   /* mark slot as used */
        pdf_put_image(p, imageslot, pdc_true, pdc_true);
        return imageslot;
    }

    if (!pdc_file_isvirtual(image->fp))
    {
        /* read whole file */
        filebase = (void *) pdc_freadall(image->fp, &filelen, &ismem);
        if (filebase == NULL)
        {
            errcode = PDC_E_IO_READ;
            goto PDF_JPEG_ERROR;
        }
        pdc_fclose(image->fp);

        /* temporary memory */
        pdc_insert_mem_tmp(p->pdc, filebase, 0, 0);

        /* create virtual image file */
        filename = "__jpeg__image__data__";
        pdf__create_pvf(p, filename, filebase, filelen, "");
        image->info.jpeg.virtfile = filename;

        if (traceprotocol)
        {
            pdc_trace(p->pdc, "\t\t\tVirtual file created, "
                              "length = 0x%lX(%ld)\n", filelen, filelen);
        }

        /* open virtual file */
        image->fp = pdf_fopen(p, filename, "", PDC_FILE_BINARY);
    }

    if (pdf_is_JPEG_file(p, image->fp) == pdc_false)
    {
        errcode = PDF_E_IMAGE_CORRUPT;
        goto PDF_JPEG_ERROR;
    }

    /* JPEG marker loop */
    while (1)
    {
        /* look for next JPEG Marker  */
        if (!markers_done)
        {
            do  /* repeat if FF/00 */
            {
                do  /* skip to FF */
                {
                    if (pdc_feof(image->fp))
                    {
                        errcode = PDF_E_IMAGE_CORRUPT;
                        goto PDF_JPEG_ERROR;
                    }
                    c = pdc_fgetc(image->fp);
                }
                while (c != 0xFF);

                do  /* skip repeated FFs */
                {
                    if (pdc_feof(image->fp))
                    {
                        errcode = PDF_E_IMAGE_CORRUPT;
                        goto PDF_JPEG_ERROR;
                    }
                    c = pdc_fgetc(image->fp);
                }
                while (c == 0xFF);
            }
            while (c == 0);

            /* start of new segment */
            pos = (pdc_off_t1) pdc_ftell(image->fp) - JPEG_MARKER_LEN;

            /* skip garbage at the start of image data */
            if (!marker_found && pos > 0)
            {
                if (traceprotocol)
                {
                    pdc_trace(p->pdc, "\t\t\t0x%lX(%ld) bytes garbage "
                                      "at start of image\n", pos, pos);
                }

                /* we must create a new virtual file */
                if (image->info.jpeg.virtfile == 0)
                {
                    /* read whole file */
                    filebase = (void *) pdc_freadall(image->fp,
                                                     &filelen, &ismem);
                    if (filebase == NULL)
                    {
                        errcode = PDC_E_IO_READ;
                        goto PDF_JPEG_ERROR;
                    }

                    /* temporary memory */
                    pdc_insert_mem_tmp(p->pdc, filebase, 0, 0);

                    filename = "__jpeg__image__data__";
                }
                else
                {
                    /* delete virtual file */
                    pdf__delete_pvf(p, image->info.jpeg.virtfile);
                }

                /* [re]create virtual file */
                filelen -= pos;
                memmove(filebase, (char *) filebase + pos, filelen);
                pdf__create_pvf(p, filename, filebase, filelen, "");
                image->info.jpeg.virtfile = filename;

                if (traceprotocol)
                {
                    pdc_trace(p->pdc, "\t\t\tVirtual file created, "
                                      "length = 0x%lX(%ld)\n",
                                      filelen, filelen);
                }
                /* [re]open virtual file */
                pdc_fclose(image->fp);
                image->fp = pdf_fopen(p, filename, "", PDC_FILE_BINARY);
            }
            marker_found = pdc_true;
            length = 0;
        }
        else
        {
            /* enforcing end of image */
            pos = (pdc_off_t1) pdc_ftell(image->fp);
            pdc_fseek(image->fp, 0L, SEEK_END);
            endpos = (pdc_off_t1) pdc_ftell(image->fp) - JPEG_MARKER_LEN;
            length = endpos - pos;
            c = M_EOI;
        }

        /* analyzing JPEG Marker */
        switch (c)
        {
            /* markers which are not supported in PDF 1.3 and above */
            case M_SOF3:
            case M_SOF5:
            case M_SOF6:
            case M_SOF7:
            case M_SOF9:
            case M_SOF11:
            case M_SOF13:
            case M_SOF14:
            case M_SOF15:
            {
                if (traceprotocol)
                {
                    pdc_trace(p->pdc, "\t\t\tMarker 0x%X(SOF%d) found - "
                                      "not supported\n", c, c - M_SOF0);
                }
                errint = c;
                errcode = PDF_E_JPEG_COMPRESSION;
            }
            goto PDF_JPEG_ERROR;

            /* markers without any parameters */
            case M_SOI:
            case M_TEM:
            case M_EOI:
            case M_RST0:
            case M_RST1:
            case M_RST2:
            case M_RST3:
            case M_RST4:
            case M_RST5:
            case M_RST6:
            case M_RST7:
            {
                if (traceprotocol)
                {
                    pdc_trace(p->pdc, "\t\t\tMarker 0x%X", c);
                    if (c == M_EOI)
                        pdc_trace(p->pdc, "(EOI)");
                    pdc_trace(p->pdc, " found - no contents\n");
                }
                pdf_register_JPEG_segment(p, image, pos,
                                         (size_t) (length + JPEG_MARKER_LEN));
            }
            break;

            /* skip segment if jpegoptimize = true, otherwise keep */
            case M_APP0:
            case M_APP1:
            case M_APP2:
            case M_APP3:
            case M_APP4:
            case M_APP5:
            case M_APP6:
            case M_APP7:
            case M_APP8:
            case M_APP9:
            case M_APP10:
            case M_APP11:
            case M_APP12:
            case M_APP13:
            case M_APP14:
            case M_APP15:
            case M_COM:
            {
                if (traceprotocol)
                {
                    pdc_trace(p->pdc, "\t\t\tMarker 0x%X", c);
                    if (c == M_COM)
                        pdc_trace(p->pdc, "(COM) found\n");
                    else
                        pdc_trace(p->pdc, "(APP%d) found\n", c - M_APP0);
                }

                length = get_ushort(image->fp);
                if (!image->jpegoptimize)
                    pdf_register_JPEG_segment(p, image, pos,
                                          (size_t) (length + JPEG_MARKER_LEN));
                else if (traceprotocol)
                    pdc_trace(p->pdc, "\t\t\tSkip segment, position=0x%lX, "
                                      "length=0x%lX(%ld)\n",
                                      pos, length, length);

                length -= JPEG_LENGTH_LEN;
            }
            break;

            /* keep segment unconditionally */
            case M_SOF0:
            case M_SOF1:
            case M_SOF2:
            case M_SOF10:
            case M_SOS:
            default:
            {
                if (traceprotocol)
                {
                    pdc_trace(p->pdc, "\t\t\tMarker 0x%X", c);
                    if (c == M_SOS)
                        pdc_trace(p->pdc, "(SOS) found\n");
                    else if (c <= M_SOF15)
                        pdc_trace(p->pdc, "(SOF%d) found\n", c - M_SOF0);
                    else
                        pdc_trace(p->pdc, " found\n");
                }

                length = get_ushort(image->fp);
                pdf_register_JPEG_segment(p, image, pos,
                                         (size_t) (length + JPEG_MARKER_LEN));
                length -= JPEG_LENGTH_LEN;
            }
            break;
        }

        /* end of image */
        if (c == M_EOI)
        {
            if (traceprotocol)
            {
                pdc_trace(p->pdc, "\t\t\tEnd of image\n");
            }
            break;
        }

        /* processing JPEG Marker */
        switch (c)
        {
            /* check for frame header markers */
            case M_SOF0:
            case M_SOF1:
            case M_SOF2:
            case M_SOF10:
            {
                image->bpc = pdc_fgetc(image->fp);
                image->height = (pdc_scalar) get_ushort(image->fp);
                image->width = (pdc_scalar) get_ushort(image->fp);
                image->components = pdc_fgetc(image->fp);
                length -= 6;

                /*
                 * No need to read more markers since multiscan detection
                 * not required for single-component images.
                 */
                if (image->components == 1)
                    markers_done = pdc_true;

                if (traceprotocol)
                {
                    pdc_trace(p->pdc, "\t\t\t\tbpc = %d\n", image->bpc);
                    pdc_trace(p->pdc, "\t\t\t\theight = %g\n", image->height);
                    pdc_trace(p->pdc, "\t\t\t\twidth = %g\n", image->width);
                    pdc_trace(p->pdc, "\t\t\t\tcomponents = %d\n",
                                      image->components);
                }
            }
            break;

            /* check for JFIF marker with resolution */
            case M_APP0:
            {
                len = MIN(APP_MAX, length);
                if (!PDC_OK_FREAD(image->fp, appstring, len))
                {
                    errcode = PDF_E_IMAGE_CORRUPT;
                    goto PDF_JPEG_ERROR;
                }
                length -= len;

                /* Check for JFIF application marker and read density values
                 * per JFIF spec version 1.02.
                 */

#define PDF_STRING_JFIF  "\x4A\x46\x49\x46"

                slen = strlen(PDF_STRING_JFIF);
                if (len > slen &&
                    !strncmp(PDF_STRING_JFIF, (char *) appstring, slen))
                {
                    /* resolution unit and resolution */
                    unit = appstring[7];
                    image->dpi_x = (pdc_scalar)
                                       pdc_get_be_ushort(&appstring[8]);
                    image->dpi_y = (pdc_scalar)
                                       pdc_get_be_ushort(&appstring[10]);

#define JFIF_ASPECT_RATIO       0    /* JFIF unit byte: aspect ratio only */
#define JFIF_DOTS_PER_INCH      1    /* JFIF unit byte: dots per inch     */
#define JFIF_DOTS_PER_CM        2    /* JFIF unit byte: dots per cm       */

                    switch (unit)
                    {
                        case JFIF_DOTS_PER_INCH:
                        break;

                        case JFIF_DOTS_PER_CM:
                        image->dpi_x *= 100 * PDC_INCH2METER;
                        image->dpi_y *= 100 * PDC_INCH2METER;
                        break;

                        case JFIF_ASPECT_RATIO:
                        image->dpi_x *= -1;
                        image->dpi_y *= -1;
                        break;

                        /* unknown ==> ignore */
                        default:
                        break;
                    }

                    if (traceprotocol)
                    {
                        pdc_trace(p->pdc, "\t\t\t\tJFIF marker found\n");
                        pdc_trace(p->pdc, "\t\t\t\tJFIF x resolution = %g\n",
                                          image->dpi_x);
                        pdc_trace(p->pdc, "\t\t\t\tJFIF y resolution = %g\n",
                                          image->dpi_y);
                    }
                }
            }
            break;


            /* check for Photoshop marker */
            case M_APP13:
            {
                len = length;
                app13 = (pdc_byte *) pdc_malloc_tmp(p->pdc, len, fn, 0, 0);
                if (!PDC_OK_FREAD(image->fp, app13, len))
                {
                    errcode = PDF_E_IMAGE_CORRUPT;
                    goto PDF_JPEG_ERROR;
                }
                length -= len;

#define PS_HEADER_LEN		14
#define PDF_STRING_Photoshop    "\x50\x68\x6F\x74\x6F\x73\x68\x6F\x70"
#define PDF_STRING_8BIM         "\x38\x42\x49\x4D"
#define RESOLUTION_INFO_ID	0x03ED	/* resolution info resource block */
#define PS_FIXED_TO_FLOAT(h, l)	((float) (h) + ((float) (l)/(1<<16)))

                /* valid Photoshop marker */
                slen = strlen(PDF_STRING_Photoshop);
                if (len > slen &&
                    !strncmp(PDF_STRING_Photoshop, (char *) app13, slen))
                {
                    if (traceprotocol)
                    {
                        pdc_trace(p->pdc, "\t\t\t\tPhotoshop marker found\n");
                    }

                    /* walk all image resource blocks
                     * and look for ResolutionInfo
                     */
                    for (s = app13 + PS_HEADER_LEN; s < app13 + len; /* */)
                    {
                        pdc_ushort type;
                        pdc_uint32 size;

                        slen = strlen(PDF_STRING_8BIM);
                        if (strncmp((char *) s, PDF_STRING_8BIM, slen))
                            break;	/* out of sync */
                        s += slen;

                        type = pdc_get_be_ushort(s);
                        s += sizeof(pdc_ushort);
			if (traceprotocol)
			{
			    pdc_trace(p->pdc, "\t\t\t\tresource type=0x%X\n",
			    	type);
			}

                        s += *s + ((*s & 1) ? 1 : 2);	/* resource name */

                        size = pdc_get_be_ulong(s);
                        s += sizeof(pdc_uint32);

                        if (type == RESOLUTION_INFO_ID && size >= 16)
                        {
                              image->dpi_x = (pdc_scalar)
                                  PS_FIXED_TO_FLOAT((s[0]<<8) + s[1],
                                                    (s[2]<<8) + s[3]);
                              image->dpi_y = (pdc_scalar)
                                  PS_FIXED_TO_FLOAT((s[8]<<8) + s[9],
                                                    (s[10]<<8) + s[11]);
                              break;
                        }

                        s += size + ((size & 1) ? 1 : 0); 	/* Data */
                    }
                }

                pdc_free_tmp(p->pdc, app13);
                app13 = NULL;
            }
            break;

            /* check for Adobe marker */
            case M_APP14:
            {
                len = MIN(APP_MAX, length);
                if (!PDC_OK_FREAD(image->fp, appstring, len))
                {
                    errcode = PDF_E_IMAGE_CORRUPT;
                    goto PDF_JPEG_ERROR;
                }
                length -= len;

                /*
                 * Check for Adobe application marker. It is known
                 * (per Adobe's TN5116)
                 * to contain the string "Adobe" at the start
                 * of the APP14 marker.
                 */

#define PDF_STRING_Adobe   "\x41\x64\x6F\x62\x65"

                slen = strlen(PDF_STRING_Adobe);
                if (len > slen &&
                    !strncmp(PDF_STRING_Adobe, (char *) appstring, slen))
                {
                    if (traceprotocol)
                    {
			pdc_byte *val = appstring+slen;

                        pdc_trace(p->pdc, "\t\t\t\tAdobe marker found\n");

			if (len >= 12)
			{
                        pdc_trace(p->pdc, "\t\t\t\tversion = 0x%02X 0x%02X\n",
				(unsigned char) val[0], (unsigned char) val[1]);
                        pdc_trace(p->pdc, "\t\t\t\tflags0 = 0x%02X 0x%02X\n",
				(unsigned char) val[2], (unsigned char) val[3]);
                        pdc_trace(p->pdc, "\t\t\t\tflags1 = 0x%02X 0x%02X\n",
				(unsigned char) val[4], (unsigned char) val[5]);
                        pdc_trace(p->pdc, "\t\t\t\tcolor transform = 0x%02X\n",
				val[6]);
			}
                    }

		    adobeflag = pdc_true;
                }
            }
            break;

            /* check for start of scan marker */
            case M_SOS:
            {
                pdc_byte comps = pdc_fgetc(image->fp);
                length -= 1;

                if (traceprotocol)
                {
                    pdc_trace(p->pdc, "\t\t\t\tNumber of components in scan = "
                                      "%d\n", comps);
                }

                /*
                 * If the scan doesn't contain all components it must be
                 * a multiscan image, which doesn't work in Acrobat.
                 */

                if (comps < image->components)
                {
                    if (traceprotocol)
                    {
                        pdc_trace(p->pdc, "\t\t\t\tMulti scan image\n");
                    }
                    need_transcode = pdc_true;
                }

                /* account for the "number of components in scan"
                 * byte just read
                 */

                markers_done = pdc_true;
            }
            break;

            default:
            break;
        }

        /* jump to the next marker */
        if (length > 0)
        {
            if (pdc_fseek(image->fp, (long) length, SEEK_CUR) == -1)
            {
                errcode = PDF_E_IMAGE_CORRUPT;
                goto PDF_JPEG_ERROR;
            }
        }
    }

    /* do some sanity checks with the parameters */
    if (image->height <= 0 || image->width <= 0 || image->components <= 0)
    {
        errcode = PDF_E_IMAGE_CORRUPT;
        goto PDF_JPEG_ERROR;
    }

    if (image->bpc != 8)
    {
        errint = image->bpc;
        errcode = PDF_E_IMAGE_BADDEPTH;
        goto PDF_JPEG_ERROR;
    }

    {
        switch (image->components) {
            case 1:
            /* spot color may have been applied */
            if (image->colorspace == pdc_undef)
                image->colorspace = DeviceGray;
            break;

            case 3:
            image->colorspace = DeviceRGB;
            break;

            case 4:
            image->colorspace = DeviceCMYK;
            break;

            default:
            errint = image->components;
            errcode = PDF_E_IMAGE_BADCOMP;
            goto PDF_JPEG_ERROR;
        }
    }


    if (image->imagemask)
    {
	if (image->components != 1)
        {
	    errcode = PDF_E_IMAGE_BADMASK;
	    goto PDF_JPEG_ERROR;
	}

	if (p->compatibility <= PDC_1_3)
        {
	    errcode = PDF_E_IMAGE_MASK1BIT13;
	    goto PDF_JPEG_ERROR;
	}
        else
        {
	    /* images with more than one bit will be written as /SMask,
	     * and don't require an /ImageMask entry.
	     */
	    image->imagemask = pdc_false;
	}
    }

    /* special handling of Photoshop-generated CMYK JPEG files */
    if (adobeflag && p->colorspaces[image->colorspace].type == DeviceCMYK)
    {
	image->invert = !image->invert;
	need_transcode = pdc_true;
    }

    image->in_use	= pdc_true;		/* mark slot as used */

    if (need_transcode)
    {
	if (traceprotocol)
	{
	    pdc_trace(p->pdc, "\t\t\ttranscoding image\n");
	}
	image->src.init		= NULL;
	image->src.fill		= pdf_data_source_JPEG_fill_transcode;
	image->src.terminate	= NULL;
    }
    else
    {
	image->src.init		= pdf_data_source_JPEG_init;
	image->src.fill		= pdf_data_source_JPEG_fill;
	image->src.terminate	= pdf_data_source_JPEG_terminate;
    }

    image->src.private_data	= (void *) image;

    if (image->doinline)
        pdf_put_inline_image(p, imageslot);
    else
        pdf_put_image(p, imageslot, pdc_true, pdc_true);

    pdf_cleanup_jpeg(p, image);

    return imageslot;

    PDF_JPEG_ERROR:
    {
        const char *stemp =
            pdc_errprintf(p->pdc, "%.*s", PDC_ET_MAXSTRLEN, image->filename);

        if (app13 != NULL)
            pdc_free_tmp(p->pdc, app13);

        switch (errcode)
        {
            case PDC_E_IO_READ:
            case PDF_E_IMAGE_ICC:
            case PDF_E_IMAGE_ICC2:
            case PDF_E_IMAGE_COLORIZE:
            case PDF_E_IMAGE_BADMASK:
            pdc_set_errmsg(p->pdc, errcode, stemp, 0, 0, 0);
            break;

            case PDC_E_IO_BADFORMAT:
            pdc_set_errmsg(p->pdc, errcode, stemp, "JPEG", 0, 0);
            break;

            case PDF_E_IMAGE_CORRUPT:
            pdc_set_errmsg(p->pdc, errcode, "JPEG", stemp, 0, 0);
            break;

            case PDF_E_JPEG_COMPRESSION:
            case PDF_E_IMAGE_BADDEPTH:
            case PDF_E_IMAGE_BADCOMP:
            pdc_set_errmsg(p->pdc, errcode,
                pdc_errprintf(p->pdc, "%d", errint), stemp, 0, 0);
            break;

	    case 0: 		/* error code and message already set */
            break;
        }
    }

    if (image->verbose)
	pdc_error(p->pdc, -1, 0, 0, 0, 0);

    return -1;
}

void
pdf_cleanup_jpeg(PDF *p, pdf_image *image)
{
    if (image->info.jpeg.virtfile != NULL)
    {
        (void) pdf__delete_pvf(p, image->info.jpeg.virtfile);
        image->info.jpeg.virtfile = NULL;
    }

    if (image->info.jpeg.seglist != NULL)
    {
        pdc_free(p->pdc, image->info.jpeg.seglist);
        image->info.jpeg.seglist = NULL;
    }
}


#endif	/* PDF_JPEG_SUPPORTED */
