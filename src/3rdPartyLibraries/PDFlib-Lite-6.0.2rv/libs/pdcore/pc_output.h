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

/* $Id: pc_output.h,v 1.1 2005/10/11 17:18:13 vuduc2 Exp $
 *
 * PDFlib output routines
 *
 */

#ifndef PC_OUTPUT_H
#define PC_OUTPUT_H

/* --------------------------- General --------------------------- */

/* Acrobat viewers change absolute values < 1/65536 to zero */
#define PDF_SMALLREAL   (0.000015)

/* maximum capacity of a dictionary, in entries */
#define PDF_MAXDICTSIZE   (4095)

/* maximum capacity of an array, in elements */
#define PDF_MAXARRAYSIZE  (8191)

/* maximum capacity of a string, in bytes */
#define PDF_MAXSTRINGSIZE  (65535)

/* some ASCII characters and strings, deliberately defined as hex/oct codes */

#define PDF_NEWLINE		((char) 0x0A)		/* ASCII '\n' */
#define PDF_RETURN		((char) 0x0D)		/* ASCII '\r' */
#define PDF_SPACE		((char) 0x20)		/* ASCII ' '  */
#define PDF_HASH                ((char) 0x23)           /* ASCII '#'  */
#define PDF_PARENLEFT           ((char) 0x28)           /* ASCII '('  */
#define PDF_PARENRIGHT          ((char) 0x29)           /* ASCII ')'  */
#define PDF_PLUS                ((char) 0x2B)           /* ASCII '+'  */
#define PDF_SLASH               ((char) 0x2F)           /* ASCII '/'  */
#define PDF_COLON               ((char) 0x3A)           /* ASCII ':'  */
#define PDF_BACKSLASH           ((char) 0x5C)           /* ASCII '\\' */

#define PDF_A                   ((char) 0x41)           /* ASCII 'A'  */
#define PDF_n                   ((char) 0x6E)           /* ASCII 'n'  */
#define PDF_r                   ((char) 0x72)           /* ASCII 'r'  */

#define PDF_STRING_0123456789ABCDEF	\
	"\060\061\062\063\064\065\066\067\070\071\101\102\103\104\105\106"

typedef struct pdc_output_s pdc_output;

/* output control.
*/
typedef struct
{
    /* exactly one of the members 'filename', 'fp', and 'writeproc'
    ** must be supplied, the others must be NULL:
    **
    ** filename		use supplied file name to create a named output file
    **			filename == "" means generate output in-core
    ** fp		use supplied FILE * to write to file
    ** writeproc	use supplied procedure to write output data
    */
    const char *filename;
    FILE *	fp;
    size_t    (*writeproc)(pdc_output *out, void *data, size_t size);
#if defined(MVS) || defined(MVS_TEST)
    int		blocksize;		/* file record size */
#endif
} pdc_outctl;

/* --------------------------- Setup --------------------------- */

pdc_output *	pdc_boot_output(pdc_core *pdc);
void		pdc_init_outctl(pdc_outctl *oc);
pdc_bool	pdc_init_output(void *opaque, pdc_output *out,
		    int compatibility, pdc_outctl *oc);
void		pdc_cleanup_output(pdc_output *out, pdc_bool keep_buf);
void *		pdc_get_opaque(pdc_output *out);

/* --------------------------- Digest --------------------------- */

void		pdc_init_digest(pdc_output *out);
void		pdc_update_digest(pdc_output *out, unsigned char *input,
		    unsigned int len);
void		pdc_finish_digest(pdc_output *out, pdc_bool settime);
unsigned char * pdc_get_digest(pdc_output *out);


/* --------------------------- Objects and ids --------------------------- */

pdc_id	pdc_alloc_id(pdc_output *out);
pdc_id	pdc_map_id(pdc_output *out, pdc_id old_id);
void	pdc_mark_free(pdc_output *out, pdc_id obj_id);

pdc_id	pdc_begin_obj(pdc_output *out, pdc_id obj_id);
#define pdc_end_obj(out)		pdc_puts(out, "endobj\n")

#define PDC_NEW_ID	0L
#define PDC_BAD_ID	-1L
#define PDC_FREE_ID	-2L


/* --------------------------- Strings --------------------------- */
/* output a string (including parentheses) and quote all required characters */
void	pdc_put_pdfstring(pdc_output *out, const char *text, int len);
void    pdc_put_pdffilename(pdc_output *out, const char *text, int len);


/* --------------------------- Names --------------------------- */
/* output a PDF name (including leading slash) and quote all required chars */
void    pdc_put_pdfname(pdc_output *out, const char *text, size_t len);


/* --------------------------- Arrays  --------------------------- */
#define pdc_begin_array(out)	pdc_puts(out, "[")
#define pdc_end_array(out)      pdc_puts(out, "]\n")
#define pdc_end_array_c(out)    pdc_puts(out, "]")


/* --------------------------- Dictionaries --------------------------- */
#define pdc_begin_dict(out)	pdc_puts(out, "<<")
#define pdc_end_dict(out)       pdc_puts(out, ">>\n")
#define pdc_end_dict_c(out)     pdc_puts(out, ">>")


/* --------------------------- Object References --------------------------- */
#define pdc_objref(out, name, obj_id)       \
		pdc_printf(out, "%s %ld 0 R\n", name, obj_id)

#define pdc_objref_c(out, obj_id)       \
		pdc_printf(out, " %ld 0 R", obj_id)

/* --------------------------- Streams --------------------------- */
void		pdc_begin_pdfstream(pdc_output *out);
void		pdc_end_pdfstream(pdc_output *out);
pdc_off_t	pdc_get_pdfstreamlength(pdc_output *out);
void		pdc_put_pdfstreamlength(pdc_output *out, pdc_id length_id);

int		pdc_get_compresslevel(pdc_output *out);
void		pdc_set_compresslevel(pdc_output *out, int compresslevel);


/* --------------------------- Document sections  --------------------------- */
void		pdc_write_xref(pdc_output *out);

void		pdc_write_digest(pdc_output *out);
void		pdc_write_trailer(pdc_output *out, pdc_id info_id,
		    pdc_id root_id, int root_gen, long xref_size,
		    pdc_off_t xref_prev, pdc_off_t xref_pos);
void		pdc_write_eof(pdc_output *out);


/* --------------------------- Low-level output --------------------------- */
void		pdc_flush_stream(pdc_output *out);
pdc_off_t	pdc_tell_out(pdc_output *out);
void		pdc_close_output(pdc_output *out);
							/* TODO2GB: size_t? */
char *		pdc_get_stream_contents(pdc_output *out, pdc_off_t *size);
int		pdc_stream_not_empty(pdc_output *out);

void		pdc_write(pdc_output *out, const void *data, size_t size);
void		pdc_puts(pdc_output *out, const char *s);
void		pdc_putc(pdc_output *out, const char c);


/* ------------------------- High-level output ------------------------- */
void		pdc_printf(pdc_output *out, const char *fmt, ...);

#endif	/* PC_OUTPUT_H */

