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

/* p_stream.c
 *
 * Stream handling routines for PDFlib 
 *
 */

#include <string.h>
#include <stdarg.h>

#include "p_intern.h"

#define LINEBUFLEN		256
#define COMPR_BUFLEN		1024

#ifdef HAVE_LIBZ
/* zlib compression */

/* wrapper for p->malloc for use in zlib */
static voidpf
pdf_zlib_alloc(voidpf p, uInt items, uInt size)
{
    return (voidpf) ((PDF *)p)->malloc((PDF *)p, items * size, "zlib");
}

#endif	/* HAVE_LIBZ */

void
pdf_init_stream(PDF *p)
{
    /*
     * This may be left over from the previous run. We deliberately
     * don't reuse the previous buffer in order to avoid potentially
     * unwanted growth of the allocated buffer due to a single large
     * document in a longer series of documents.
     */
    if (p->stream.basepos)
	p->free(p, (void *) p->stream.basepos);

    p->stream.basepos	=(pdf_byte *)p->malloc(p, STREAM_CHUNKSIZE, "pdf_open_stream");
    p->stream.curpos	= p->stream.basepos;
    p->stream.maxpos	= p->stream.basepos + STREAM_CHUNKSIZE;
    p->stream.base_offset= 0L;
    p->stream.compress	= pdf_false;

#ifdef HAVE_LIBZ
    p->stream.z.zalloc	= (alloc_func) pdf_zlib_alloc;
    p->stream.z.zfree	= (free_func) p->free;
    p->stream.z.opaque	= (voidpf) p;
#endif
}

static void
pdf_check_stream(PDF *p, size_t len)
{
    size_t max;
    int cur;
    
    if (p->stream.curpos + len <= p->stream.maxpos)
	return;

    if (p->stream.flush & PDF_FLUSH_HEAVY) {
	pdf_flush_stream(p);

	if (p->stream.curpos + len <= p->stream.maxpos)
	    return;
    }

    max = (size_t) (2 * (p->stream.maxpos - p->stream.basepos));
    cur = p->stream.curpos - p->stream.basepos;

    p->stream.basepos = (pdf_byte *)
	p->realloc(p, (void *) p->stream.basepos, max, "pdf_check_stream");
    p->stream.maxpos = p->stream.basepos + max;
    p->stream.curpos = p->stream.basepos + cur;

    pdf_check_stream(p, len);
}

void
pdf_flush_stream(PDF *p)
{
    size_t size;

    /* safeguard against recursive I/O errors and other situations */
    if (p->in_error || !p->writeproc || p->stream.compress)
	return;

    size = (size_t) (p->stream.curpos - p->stream.basepos);

    if (p->writeproc(p, (void *) p->stream.basepos, size) != size) {
	p->free(p, p->stream.basepos);
	p->stream.basepos = NULL;
	pdf_error(p, PDF_IOError, "Couldn't write output (disk full?)");
    }

    p->stream.base_offset += (size_t) (p->stream.curpos - p->stream.basepos);
    p->stream.curpos = p->stream.basepos;
}

void
pdf_close_stream(PDF *p)
{
    /* this may happen in rare cases */
    if (!p->stream.basepos)
	return;

    if (p->stream.basepos) {
	p->free(p, (void *) p->stream.basepos);
	p->stream.basepos = NULL;
    }
}

long
pdf_tell(PDF *p)
{
    return(p->stream.base_offset + p->stream.curpos - p->stream.basepos);
}

#ifdef HAVE_LIBZ
void
pdf_compress_init(PDF *p)
{
    if (deflateInit(&p->stream.z, p->compress) != Z_OK)
	pdf_error(p, PDF_SystemError, "Compression error (deflateInit)");

    p->stream.compress = pdf_true;
}

void
pdf_compress_end(PDF *p)
{
    int status;

    /* Finish the stream */
    do {
	pdf_check_stream(p, 128);
	p->stream.z.next_out	= (Bytef *) p->stream.curpos;
	p->stream.z.avail_out	= (uInt) (p->stream.maxpos - p->stream.curpos);

	status = deflate(&(p->stream.z), Z_FINISH);
	p->stream.curpos = p->stream.z.next_out;

	if (status != Z_STREAM_END && status != Z_OK)
	    pdf_error(p, PDF_SystemError, "Compression error (Z_FINISH)");

    } while (status != Z_STREAM_END);

    if (deflateEnd(&p->stream.z) != Z_OK)
	pdf_error(p, PDF_SystemError, "Compression error (deflateEnd)");

    p->stream.compress = pdf_false;
}
#endif /* HAVE_LIBZ */

/* **************************** output functions ************************ */

void
pdf_write(PDF *p, const void *data, size_t size)
{
#ifdef HAVE_LIBZ
    if (p->stream.compress) {
	p->stream.z.avail_in	= (uInt) size;
	p->stream.z.next_in	= (Bytef *) data;
	p->stream.z.avail_out	= 0;

	while (p->stream.z.avail_in > 0) {
	    if (p->stream.z.avail_out == 0) {
		/* estimate output buffer size */
		pdf_check_stream(p, (size_t) (p->stream.z.avail_in/4 + 16));
		p->stream.z.next_out	= (Bytef *) p->stream.curpos;
		p->stream.z.avail_out	= (uInt) (p->stream.maxpos - p->stream.curpos);
	    }

	    if (deflate(&(p->stream.z), Z_NO_FLUSH) != Z_OK)
		pdf_error(p, PDF_SystemError, "Compression error (Z_NO_FLUSH)");

	   p->stream.curpos = p->stream.z.next_out;
	}
    }else {
#endif /* HAVE_LIBZ */

	pdf_check_stream(p, size);
	memcpy(p->stream.curpos, data, size);
	p->stream.curpos += size;

#ifdef HAVE_LIBZ
    }
#endif /* HAVE_LIBZ */
}

void
pdf_puts(PDF *p, const char *s)
{
#ifdef PDFLIB_EBCDIC

    char buffer[LINEBUFLEN];

    strcpy(buffer, s);
    pdf_make_ascii(buffer);
    pdf_write(p, (void *) buffer, strlen(buffer));

#else

    pdf_write(p, (void *) s, strlen(s));

#endif	/* PDFLIB_EBCDIC */
}

void
pdf_putc(PDF *p, char c)
{
    pdf_write(p, (void *) &c, (size_t) 1);
}

void
pdf_printf(PDF *p, const char *fmt, ...)
{
    char	buf[LINEBUFLEN];	/* formatting buffer */
    va_list ap;

    va_start(ap, fmt);

    vsprintf(buf, fmt, ap);
    pdf_puts(p, buf);

    va_end(ap);
}

#ifdef PDFLIB_EBCDIC

/*
 * Platforms other than AS/400 and OS/390 will have to decide
 * on their own which codepage to use. This code is deliberately
 * set up such that a linker error will result if no code page
 * has been explicitly selected.
 */

#if PDF_CODEPAGE == 819

/* EBCDIC code page 819 for OS/400 */
static unsigned const char ebcdic2ascii_table[256] = "\
\000\001\002\003\234\011\206\177\227\215\216\013\014\015\016\017\
\020\021\022\023\235\020\010\207\030\031\222\217\034\035\036\037\
\200\201\202\203\204\012\027\033\210\211\212\213\214\005\006\007\
\220\221\026\223\224\225\226\004\230\231\232\233\024\025\236\032\
\040\240\342\344\340\341\343\345\347\361\133\056\074\050\053\174\
\046\351\352\353\350\355\356\357\354\337\135\044\052\051\073\254\
\055\057\302\304\300\301\303\305\307\321\246\054\045\137\076\077\
\370\311\312\313\310\315\316\317\314\140\072\043\100\047\075\042\
\330\141\142\143\144\145\146\147\150\151\253\273\360\375\376\261\
\260\152\153\154\155\156\157\160\161\162\252\272\346\270\306\244\
\265\176\163\164\165\166\167\170\171\172\241\277\320\335\336\256\
\136\243\245\267\251\247\266\274\275\276\133\135\257\250\264\327\
\173\101\102\103\104\105\106\107\110\111\255\364\366\362\363\365\
\175\112\113\114\115\116\117\120\121\122\271\373\374\371\372\377\
\134\367\123\124\125\126\127\130\131\132\262\324\326\322\323\325\
\060\061\062\063\064\065\066\067\070\071\263\333\334\331\332\237\
";

#endif /* PDF_CODEPAGE == 819 */

#if PDF_CODEPAGE == 1047

/* EBCDIC code page 1047, Unix system services for OS/390 (Open Edition) */
static unsigned const char ebcdic2ascii_table[256] = "\
\000\001\002\003\234\011\206\177\227\215\216\013\014\015\016\017\
\020\021\022\023\235\012\010\207\030\031\222\217\034\035\036\037\
\200\201\202\203\204\205\027\033\210\211\212\213\214\005\006\007\
\220\221\026\223\224\225\226\004\230\231\232\233\024\025\236\032\
\040\240\342\344\340\341\343\345\347\361\242\056\074\050\053\174\
\046\351\352\353\350\355\356\357\354\337\041\044\052\051\073\136\
\055\057\302\304\300\301\303\305\307\321\246\054\045\137\076\077\
\370\311\312\313\310\315\316\317\314\140\072\043\100\047\075\042\
\330\141\142\143\144\145\146\147\150\151\253\273\360\375\376\261\
\260\152\153\154\155\156\157\160\161\162\252\272\346\270\306\244\
\265\176\163\164\165\166\167\170\171\172\241\277\320\133\336\256\
\254\243\245\267\251\247\266\274\275\276\335\250\257\135\264\327\
\173\101\102\103\104\105\106\107\110\111\255\364\366\362\363\365\
\175\112\113\114\115\116\117\120\121\122\271\373\374\371\372\377\
\134\367\123\124\125\126\127\130\131\132\262\324\326\322\323\325\
\060\061\062\063\064\065\066\067\070\071\263\333\334\331\332\237\
";

#endif /* PDF_CODEPAGE == 1047 */

/* The EBCDIC to ASCII in-place string conversion routine */

void
pdf_ebcdic2ascii(char *s)
{
    for (/* */; *s; s++)
	*s = (char) ebcdic2ascii_table[(unsigned char) (*s)];
}
#endif /* PDFLIB_EBCDIC */
