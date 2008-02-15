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

/* $Id: pc_core.h,v 1.1 2005/10/11 17:18:13 vuduc2 Exp $
 *
 * PDFlib core services:
 * - memory management
 * - exception handling
 * - internal try/catch
 * - debug tracing
 */

#ifndef PC_CORE_H
#define PC_CORE_H

/* Built-in metric support */
#define PDF_BUILTINMETRIC_SUPPORTED

/* Built-in encoding support */
#define PDF_BUILTINENCODING_SUPPORTED


#define PDF_FEATURE_NOT_PUBLIC


/* ------------------------- general  ------------------------- */

typedef struct pdc_core_s pdc_core;

typedef int            pdc_bool;
typedef long           pdc_id;
typedef char           pdc_char;
typedef unsigned char  pdc_byte;
typedef unsigned char  pdc_uchar;
typedef short          pdc_short;
typedef unsigned short pdc_ushort;
typedef long           pdc_long;
typedef unsigned long  pdc_ulong;

typedef short          pdc_sint16;
typedef unsigned short pdc_uint16;
typedef int            pdc_sint32;
typedef unsigned int   pdc_uint32;

/* TODO2GB:	this is the signed 64-bit integer type for >2GB files.
**		 must be platform & compiler specific.
*/
#if	defined(_LARGEFILE_SOURCE)
    #if defined(WIN32)
	typedef __int64	pdc_off_t;
    #else
#include <sys/types.h>
	typedef off_t	pdc_off_t;
    #endif
#else
	typedef long	pdc_off_t;
#endif

/* use this one for casts from "off_t" to "long" - so we can "grep"
** for critical places.
*/
typedef long pdc_off_t1;


#define pdc_undef      -1
#define pdc_false       0
#define pdc_true	1

#define	PDC_1_1			11		/* PDF 1.1 = Acrobat 2 */
#define	PDC_1_2			12		/* PDF 1.2 = Acrobat 3 */
#define	PDC_1_3			13		/* PDF 1.3 = Acrobat 4 */
#define	PDC_1_4			14		/* PDF 1.4 = Acrobat 5 */
#define	PDC_1_5			15		/* PDF 1.5 = Acrobat 6 */
#define	PDC_1_6			16		/* PDF 1.6 = Acrobat 7 */
#define PDC_X_X_LAST		16


typedef void  (*pdc_error_fp)(void *opaque, int type, const char *msg);
typedef void* (*pdc_alloc_fp)(void *opaque, size_t size, const char *caller);
typedef void* (*pdc_realloc_fp)(void *opaque, void *mem, size_t size,
						const char *caller);
typedef void  (*pdc_free_fp)(void *opaque, void *mem);

pdc_core *pdc_init_core(pdc_error_fp errorhandler, pdc_alloc_fp allocproc,
    pdc_realloc_fp reallocproc, pdc_free_fp freeproc, void *opaque);

void pdc_delete_core(pdc_core *pdc);

/* this is used by pdflib and pdi sources, so i put it here.
** WARNING: any changes here will break existing PDI code!
*/
typedef enum {
    use_none = 0, use_art, use_bleed, use_crop, use_media, use_trim
} pdc_usebox;

/* ------------------------- memory management  ------------------------- */

void	*pdc_malloc(pdc_core *pdc, size_t size, const char *caller);
void	*pdc_realloc(pdc_core *pdc, void *mem, size_t size, const char *caller);
void	*pdc_calloc(pdc_core *pdc, size_t size, const char *caller);
void	pdc_free(pdc_core *pdc, void *mem);

#define PDC_TMPMEM	1

typedef void (*pdc_destructor)(void *opaque, void *mem);

void    pdc_insert_mem_tmp(pdc_core *pdc, void *memory, void *opaque,
            pdc_destructor destr);
void	*pdc_malloc_tmp(pdc_core *pdc, size_t size, const char *caller,
	    void *opaque, pdc_destructor destr);
void	*pdc_realloc_tmp(pdc_core *pdc, void *mem, size_t size,
	    const char *caller);
void	*pdc_calloc_tmp(pdc_core *pdc, size_t size, const char *caller,
	    void *opaque, pdc_destructor destr);
void	pdc_free_tmp(pdc_core *pdc, void *mem);

/* --------------------------- exception handling --------------------------- */

#define PDC_ET_NOPREFIX (1<<0)  /* API funcname without prefix (e.g. PDF_) */
#define PDC_ET_NOSUFFIX (1<<1)  /* API funcname without suffix 2 */

/* maximal length of strings for %.*s in pdc_errprintf format
*/
#define PDC_ET_MAXSTRLEN 256

/* per-library error table base numbers.
*/
#define PDC_ET_CORE	1000
#define PDC_ET_PDFLIB	2000
#define PDC_ET_PDI	4000
#define PDC_ET_PLOP	5000
#define PDC_ET_PDPAGE	6000

#define PDC_ET_LAST	6000

/* core error numbers.
*/
enum
{
#define		pdc_genNames	1
#include	"pc_generr.h"

    PDC_E_dummy
};

typedef struct
{
    int		nparms;		/* number of error parameters	*/
    int		errnum;		/* error number			*/
    const char *errmsg;		/* default error message	*/
    const char *ce_msg;		/* custom error message		*/
} pdc_error_info;

void		pdc_register_errtab(pdc_core *pdc, int et, pdc_error_info *ei,
		    int n_entries);

pdc_bool        pdc_enter_api(pdc_core *pdc, const char *apiname, int flags);
pdc_bool	pdc_in_error(pdc_core *pdc);
void		pdc_set_warnings(pdc_core *pdc, pdc_bool on);

const char *	pdc_errprintf(pdc_core *pdc, const char *format, ...);

void		pdc_set_errmsg(pdc_core *pdc, int errnum, const char *parm1,
		    const char *parm2, const char *parm3, const char *parm4);

void		pdc_error(pdc_core *pdc, int errnum, const char *parm1,
		    const char *parm2, const char *parm3, const char *parm4);

void		pdc_warning(pdc_core *pdc, int errnum, const char *parm1,
		    const char *parm2, const char *parm3, const char *parm4);

int		pdc_get_errnum(pdc_core *pdc);
const char *	pdc_get_errmsg(pdc_core *pdc);
const char *	pdc_get_apiname(pdc_core *pdc);

/* ----------------------------- try/catch ---------------------------- */

#include <setjmp.h>

typedef struct
{
    jmp_buf	jbuf;
} pdc_jmpbuf;

pdc_jmpbuf *	pdc_jbuf(pdc_core *pdc);
void		pdc_exit_try(pdc_core *pdc);
int		pdc_catch_intern(pdc_core *pdc);
int		pdc_catch_extern(pdc_core *pdc);
void		pdc_rethrow(pdc_core *pdc);

#define PDC_TRY(pdc)		if (setjmp(pdc_jbuf(pdc)->jbuf) == 0)

#define PDC_EXIT_TRY(pdc)	pdc_exit_try(pdc)

#define PDC_CATCH(pdc)		if (pdc_catch_intern(pdc))

#define PDC_RETHROW(pdc)	pdc_rethrow(pdc)


/* --------------------------- debug trace  --------------------------- */

/* pdcore protocol classes */
typedef enum
{
    trc_other = 0,      /* other classes */
    trc_api,            /* API function call protocol */
    trc_optlist,        /* optlist protocol */
    trc_memory,         /* memory protocol */
    trc_warning,        /* protocol of disabled warnings */
    trc_resource,       /* resource protocol */
    trc_filesearch,     /* file search protocol */
    trc_encoding,       /* encoding end textformat protocol */
    trc_image,          /* image and template protocol */
    trc_font            /* font protocol */
}
pdc_protocol_class;

/* string code kinds */
typedef enum
{
    strform_readable,
    strform_readable0,
    strform_octal,
    strform_hexa,
    strform_java
}
pdc_strform_kind;

# ifndef DEBUG_TRACE_FILE
#  if defined(MVS)
#   define DEBUG_TRACE_FILE     "pdflog"
#  elif defined(MAC) || defined(AS400)
#   define DEBUG_TRACE_FILE     "PDFlib.log"
#  elif defined(WIN32)
#   define DEBUG_TRACE_FILE     "/PDFlib.log"
#  else
#   define DEBUG_TRACE_FILE     "/tmp/PDFlib.log"
#  endif
# endif
# define PDF_TRACE(ARGS)	pdc_trace ARGS

void	pdc_set_tracefile(pdc_core *pdc, const char *filename);
pdc_bool pdc_set_trace_options(pdc_core *pdc, const char *optlist);
void	pdc_set_trace(pdc_core *pdc, const char *client);
const char *pdc_print_tracestring(pdc_core *pdc, const char *str, int len);
void    pdc_trace(pdc_core *pdc, const char *fmt, ...);
void    pdc_trace_enter_api(pdc_core *pdc, const char *funame,
                            const char *fmt, va_list args);
void    pdc_trace_exit_api(pdc_core *pdc, pdc_bool cleanup,
                           const char *fmt, ...);
pdc_bool pdc_trace_protocol_is_enabled(pdc_core *pdc, int level, int pclass);
void    pdc_trace_protocol(pdc_core *pdc, int level, int pclass,
                           const char *fmt, ...);
#ifdef	PDC_DEBUG
void	pdc_enable_hexdump(pdc_core *pdc);
void	pdc_disable_hexdump(pdc_core *pdc);
void	pdc_hexdump(pdc_core *pdc, const char *msg, const char *text, int tlen);
#endif /* PDC_DEBUG */


/* ------------ client structs handling ------------ */

void    pdc_set_pglyphtab_ptr(pdc_core *pdc, void *pglyphtab_ptr);
void *  pdc_get_pglyphtab_ptr(pdc_core *pdc);


/* ----------- service function to get PDF version string  -------------- */

const char *pdc_get_pdfversion(pdc_core *pdc, int compatibility);


/* --------------------------- float digits  --------------------------- */

void    pdc_set_floatdigits(pdc_core *pdc, int val);
int     pdc_get_floatdigits(pdc_core *pdc);


/* --------------------------- scope  --------------------------- */

/*
 * An arbitrary number used for sanity checks.
 * Actually, we use the hex representation of pi in order to avoid
 * the more common patterns.
 */

#define PDC_MAGIC	((unsigned long) 0x126960A1)

#endif	/* PC_CORE_H */
