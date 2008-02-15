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

/* $Id: pc_core.c,v 1.1 2005/10/11 17:18:13 vuduc2 Exp $
 *
 * PDFlib core services
 *
 */

#include "pc_util.h"

#define PDF_NonfatalError 11
#define PDF_UnknownError  12

#if defined(__ia64__) && defined (__linux__)
#define PDC_ALIGN16
#endif


/* TODO: how to make this dynamic?
** exception during pdc_core_init():
**	- out of memory in pdc_sb_new()
** exception during exception:
**	- out of memory in pdc_sb_vprintf()
**	- format error in pdc_sb_vprintf()
*/
#define PDC_ERRPARM_SIZE	2048
#define PDC_ERRBUF_SIZE		(5 * PDC_ERRPARM_SIZE)
#define PDC_XSTACK_INISIZE	10

#define PDC_CLASSLIST_SIZE      32

#define N_ERRTABS		(PDC_ET_LAST / 1000)

/* temporary free store.
*/
typedef struct
{
    void  *		mem;
    pdc_destructor	destr;
    void  *		opaque;
} pdc_tmpmem;

typedef struct
{
    pdc_tmpmem *	tmpmem;
    int			capacity;
    int			size;
} pdc_tmpmem_list;

static void pdc_tmlist_init(pdc_tmpmem_list *tm_list);
static void pdc_tmlist_cleanup(pdc_core *pdc, pdc_tmpmem_list *tm_list);


/* exception handling frame.
*/
typedef struct
{
    pdc_jmpbuf		jbuf;
} pdc_xframe;

typedef struct
{
    pdc_error_info *	ei;
    int			n_entries;
} error_table;


/* ------------------------ the core core structure ---------------------- */

struct pdc_core_s {
    /* ------------ try/catch ------------ */
    pdc_xframe *	x_stack;
#ifdef PDC_ALIGN16
    char *		x_alias;
#endif
    int			x_ssize;
    int			x_sp;		/* exception stack pointer	*/

    /* ------------ error handling ------------ */
    pdc_bool		warnings_enabled;
    pdc_bool		in_error;
    char		errbuf[PDC_ERRBUF_SIZE];
    char		errparms[4][PDC_ERRPARM_SIZE];
    int			epcount;
    int			errnum;
    pdc_bool		x_thrown;	/* exception thrown and not caught */
    char *	        apiname;
    pdc_error_fp	errorhandler;	/* client error handler		*/
    void *		opaque;		/* client specific, opaque data */

    error_table		err_tables[N_ERRTABS];

    /* ------------------ tracing ---------------- */
    pdc_bool		tracestatus;    /* trace status */
                                        /* = pdc_undef: new file
                                         * = pdc_true:  enabled
                                         * = pdc_false: disabled */
    char *		tracefilename;	/* name of the trace file */
    pdc_bool            traceflush;     /* trace file will be opened and
                                         * and closed immediately */
    FILE *              tracefp;        /* traceflush = false: file handle */
    pdc_strform_kind    tracestrform;   /* format for trace strings */
    int                 tracemaxchar;   /* maximal number of characters
                                         * of trace strings */
    char                traceclasslist[PDC_CLASSLIST_SIZE];
                                        /* list of levels for logging classes.
                                         * Index 0 - 9:  reserved for pdcore.
                                         * (see enum pdc_protocol_class)
                                         * Index 10 - 63: reserved for client.
                                         */
    pdc_bool            traceclassapi;  /* only api class has level > 0 */
    int			floatdigits;	/* floating point output precision */
#ifdef	PDC_DEBUG
    pdc_bool		hexdump;	/* hexdump feature enabled? */
#endif /* PDC_DEBUG */

    /* ------------ memory management ------------ */
    pdc_alloc_fp	allocproc;
    pdc_realloc_fp	reallocproc;
    pdc_free_fp		freeproc;
    pdc_tmpmem_list	tm_list;

    /* ------------ encoding handling ------------ */
    void *              pglyphtab;      /* private glyph table */
};



/* ----------- default memory management & error handling ----------- */

static void *
default_malloc(void *opaque, size_t size, const char *caller)
{
    (void) opaque;
    (void) caller;

    return malloc(size);
}

static void *
default_realloc(void *opaque, void *mem, size_t size, const char *caller)
{
    (void) opaque;
    (void) caller;

    return realloc(mem, size);
}

static void
default_free(void *opaque, void *mem)
{
    (void) opaque;

    free(mem);
}

static void
default_errorhandler(void *opaque, int errnum, const char *msg)
{
    (void) opaque;

    if (errnum == PDF_NonfatalError)
    {
	fprintf(stderr, "PDFlib warning (ignored): %s\n", msg);
    }
    else
    {
	fprintf(stderr, "PDFlib exception (fatal): %s\n", msg);
	exit(99);
    }
}

pdc_bool
pdc_enter_api(pdc_core *pdc, const char *apiname, int flags)
{
    char *name = NULL;

    if (pdc->in_error)
	return pdc_false;

    if (flags & PDC_ET_NOPREFIX)
        name = strchr(apiname, '_');
    if (name)
        name++;
    else
        name = (char *) apiname;

    if (pdc->apiname)
        pdc_free(pdc, pdc->apiname);
    pdc->apiname = pdc_strdup(pdc, name);

    if (flags & PDC_ET_NOSUFFIX)
    {
        size_t len = strlen(pdc->apiname);
        len--;
        if (len && pdc->apiname[len] == '2')
            pdc->apiname[len] = 0;
    }

    pdc->errnum = 0;
    return pdc_true;
}

void
pdc_set_warnings(pdc_core *pdc, pdc_bool on)
{
    pdc->warnings_enabled = on;
}

pdc_bool
pdc_in_error(pdc_core *pdc)
{
    return pdc->in_error;
}


/* --------------------- error table management --------------------- */

static pdc_error_info	core_errors[] =
{
#define		pdc_genInfo	1
#include	"pc_generr.h"
};

#define N_CORE_ERRORS	(sizeof core_errors / sizeof (pdc_error_info))


static void
pdc_panic(pdc_core *pdc, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vsprintf(pdc->errbuf, fmt, ap);
    va_end(ap);

    pdc->errnum = PDF_UnknownError;
    (*pdc->errorhandler)(pdc->opaque, pdc->errnum, pdc->errbuf);
} /* pdc_panic */


static void
check_parms(pdc_core *pdc, pdc_error_info *ei)
{
    const char *msg = ei->errmsg;
    const char *dollar;

    while ((dollar = strchr(msg, '$')) != (char *) 0)
    {
	if (isdigit((int) dollar[1]))
	{
	    int n = dollar[1] - '0';

	    if (ei->nparms < n || n < 1)
		pdc_panic(pdc, "illegal parameter '$%d' in error message %d",
				    n, ei->errnum);
	}
	else if (dollar[1] != '$')
	{
	    pdc_panic(pdc,
		"illegal '$' in error message %d", ei->errnum);
	}

	msg = dollar + 1;
    }
} /* check_parms */


void
pdc_register_errtab(pdc_core *pdc, int et, pdc_error_info *ei, int n_entries)
{
    int i;
    int n = (et / 1000) - 1;

    if (n < 0 || N_ERRTABS <= n || et % 1000 != 0)
	pdc_panic(pdc, "tried to register unknown error table %d", et);

    /* ignore multiple registrations of the same table.
    */
    if (pdc->err_tables[n].ei != (pdc_error_info *) 0)
	return;

    pdc->err_tables[n].ei = ei;
    pdc->err_tables[n].n_entries = n_entries;

    check_parms(pdc, &ei[0]);

    for (i = 1; i < n_entries; ++i)
    {
	if (ei[i].errnum <= ei[i-1].errnum)
	{
	    pdc_panic(pdc,
		"duplicate or misplaced error number %d", ei[i].errnum);
	}

	/* an error table may span several blocks.
	*/
	if ((ei[i].errnum / 1000) - 1 > n)
	{
	    pdc->err_tables[n].n_entries = i;	/* correct old block size */

	    n = (ei[i].errnum / 1000) - 1;	/* new block number */

	    if (N_ERRTABS <= n)
		pdc_panic(pdc, "invalid error number %d", ei[i].errnum);

	    ei += i;				/* start of new block */
	    n_entries -= i;			/* size of new block */
	    i = 0;
	    pdc->err_tables[n].ei = ei;
	    pdc->err_tables[n].n_entries = n_entries;
	}

	check_parms(pdc, &ei[i]);
    }
} /* pdc_register_errtab */


/* pdc_init_core() never throws exceptions.
** it returns NULL if there's not enough memory.
*/
pdc_core *
pdc_init_core(
    pdc_error_fp errorhandler,
    pdc_alloc_fp allocproc,
    pdc_realloc_fp reallocproc,
    pdc_free_fp freeproc,
    void *opaque)
{
    static const char fn[] = "pdc_init_core";

    pdc_core *pdc;
    int i;

    /* if allocproc is NULL, we use pdc's default memory handling.
    */
    if (allocproc == (pdc_alloc_fp) 0)
    {
	allocproc	= default_malloc;
	reallocproc	= default_realloc;
	freeproc	= default_free;
    }

    if (errorhandler == (pdc_error_fp) 0)
	errorhandler = default_errorhandler;

    pdc = (pdc_core *) (*allocproc)(opaque, sizeof (pdc_core), fn);

    if (pdc == (pdc_core *) 0)
	return (pdc_core *) 0;

    pdc->errorhandler	= errorhandler;
    pdc->allocproc	= allocproc;
    pdc->reallocproc	= reallocproc;
    pdc->freeproc	= freeproc;
    pdc->opaque		= opaque;

    pdc->tracestatus    = pdc_undef;
    pdc->tracefilename	= NULL;
    pdc->traceflush     = pdc_true;
    pdc->tracefp        = NULL;
    pdc->tracestrform   = strform_readable;
    pdc->tracemaxchar   = 0;
    memset(pdc->traceclasslist, 0, PDC_CLASSLIST_SIZE);
    pdc->traceclasslist[trc_api] = 1;
    pdc->traceclassapi  = pdc_true;

    pdc->floatdigits	= 4;
#ifdef	PDC_DEBUG
    pdc->hexdump	= pdc_true;
#endif

    /* initialize error & exception handling.
    */
    pdc->warnings_enabled = pdc_true;
    pdc->in_error = pdc_false;
    pdc->x_thrown = pdc_false;
    pdc->epcount = 0;
    pdc->errnum = 0;
    pdc->apiname = NULL;
    pdc->x_sp = -1;
    pdc->x_ssize = PDC_XSTACK_INISIZE;

#ifdef PDC_ALIGN16
    pdc->x_alias = (char *)
	(*allocproc)(opaque, 16 + pdc->x_ssize * sizeof (pdc_xframe), fn);

    if (pdc->x_alias == (char *) 0)
	pdc->x_stack = (pdc_xframe *) 0;
    else
	pdc->x_stack = (pdc_xframe *)
	    (((unsigned long) pdc->x_alias + 16) & 0xFFFFFFFFFFFFFFF0);
#else
    pdc->x_stack = (pdc_xframe *)
	(*allocproc)(opaque, pdc->x_ssize * sizeof (pdc_xframe), fn);
#endif

    if (pdc->x_stack == (pdc_xframe *) 0)
    {
	(*freeproc)(opaque, pdc);
	return (pdc_core *) 0;
    }

    pdc_tmlist_init(&pdc->tm_list);

    /* initialize error tables.
    */
    for (i = 0; i < N_ERRTABS; ++i)
	pdc->err_tables[i].ei = (pdc_error_info *) 0;

    pdc_register_errtab(pdc, PDC_ET_CORE, core_errors, N_CORE_ERRORS);


    /* initialize private glyph table */
    pdc->pglyphtab = NULL;

    return pdc;
}

void
pdc_delete_core(pdc_core *pdc)
{
    pdc_time    ltime;

    pdc_localtime(&ltime);
    pdc_trace(pdc, "[%04d-%02d-%02d %02d:%02d:%02d]\n",
        ltime.year + 1900, ltime.month + 1, ltime.mday,
        ltime.hour, ltime.minute, ltime.second);

    if (pdc->apiname)
        pdc_free(pdc, pdc->apiname);

    pdc_tmlist_cleanup(pdc, &pdc->tm_list);

    if (pdc->tm_list.capacity != 0)
	pdc_free(pdc, pdc->tm_list.tmpmem);

#ifdef PDC_ALIGN16
    pdc_free(pdc, pdc->x_alias);
#else
    pdc_free(pdc, pdc->x_stack);
#endif

    pdc_cleanup_pglyphtab(pdc);
    pdc->pglyphtab = NULL;

    if (pdc->tracefp != NULL && pdc->tracefp != stdout && pdc->tracefp !=stderr)
        fclose(pdc->tracefp);
    pdc->tracefp = NULL;
    if (pdc->tracefilename)
        pdc_free(pdc, pdc->tracefilename);
    pdc->tracefilename = NULL;
    pdc->tracestatus = pdc_false;

    pdc_free(pdc, pdc);
}

/* --------------------------- memory management --------------------------- */

void *
pdc_malloc(pdc_core *pdc, size_t size, const char *caller)
{
    void *ret;

    /* the behavior of malloc(0) is undefined in ANSI C, and may
     * result in a NULL pointer return value which makes PDFlib bail out.
     */
    if (size == (size_t) 0 || (long) size < 0L) {
	size = (size_t) 1;
	pdc_warning(pdc, PDC_E_INT_ALLOC0, caller, 0, 0, 0);
    }

    if ((ret = (*pdc->allocproc)(pdc->opaque, size, caller)) == (void *) 0)
    {
	pdc_error(pdc, PDC_E_MEM_OUT, caller, 0, 0, 0);
    }

    pdc_trace_protocol(pdc, 1, trc_memory,
                       "\t%p malloced, size=%d, called from \"%s\"\n",
                       ret, size, caller);

    return ret;
}

/* We cook up our own calloc routine, using the caller-supplied
 * malloc and memset.
 */
void *
pdc_calloc(pdc_core *pdc, size_t size, const char *caller)
{
    void *ret;

    if (size == (size_t) 0 || (long) size < 0L) {
	size = (size_t) 1;
	pdc_warning(pdc, PDC_E_INT_ALLOC0, caller, 0, 0, 0);
    }

    if ((ret = (*pdc->allocproc)(pdc->opaque, size, caller)) == (void *) 0)
    {
	pdc_error(pdc, PDC_E_MEM_OUT, caller, 0, 0, 0);
    }

    pdc_trace_protocol(pdc, 1, trc_memory,
                       "\t%p calloced, size=%d, called from \"%s\"\n",
                       ret, size, caller);

    memset(ret, 0, size);
    return ret;
}

void *
pdc_realloc(pdc_core *pdc, void *mem, size_t size, const char *caller)
{
    void *ret;

    if (size == (size_t) 0 || (long) size < 0L) {
        size = (size_t) 1;
        pdc_warning(pdc, PDC_E_INT_ALLOC0, caller, 0, 0, 0);
    }

    if ((ret = (*pdc->reallocproc)(pdc->opaque, mem, size, caller))
         == (void *) 0)
	pdc_error(pdc, PDC_E_MEM_OUT, caller, 0, 0, 0);

    pdc_trace_protocol(pdc, 1, trc_memory,
                       "\t%p realloced to\n"
                       "\t%p new, size=%d, called from \"%s\"\n",
                       mem, ret, size, caller);

    return ret;
}

void
pdc_free(pdc_core *pdc, void *mem)
{
    pdc_trace_protocol(pdc, 1, trc_memory, "\t%p freed\n", mem);

    /* just in case the freeproc() isn't that ANSI compatible...
    */
    if (mem != NULL)
	(*pdc->freeproc)(pdc->opaque, mem);

}

/* -------------------- temporary free store management -------------------- */

static void
pdc_tmlist_init(pdc_tmpmem_list *tm_list)
{
    tm_list->size = tm_list->capacity = 0;
}

static void
pdc_tmlist_grow(pdc_core *pdc, pdc_tmpmem_list *tm_list)
{
    static const char	fn[] = "pdc_tmlist_grow";
    static const int	chunksize = 20;

    if (tm_list->capacity == 0)
    {
	tm_list->capacity = chunksize;
	tm_list->tmpmem = (pdc_tmpmem *) pdc_malloc(pdc,
	    (size_t) (tm_list->capacity * sizeof (pdc_tmpmem)), fn);
    }
    else
    {
	tm_list->capacity += chunksize;
	tm_list->tmpmem = (pdc_tmpmem *) pdc_realloc(pdc, tm_list->tmpmem,
	    (size_t) (tm_list->capacity * sizeof (pdc_tmpmem)), fn);
    }
}

static void
pdc_tmlist_cleanup(pdc_core *pdc, pdc_tmpmem_list *tm_list)
{
    int i;

    for (i = 0; i < tm_list->size; ++i)
    {
	if (tm_list->tmpmem[i].destr)
	    tm_list->tmpmem[i].destr(pdc, tm_list->tmpmem[i].mem);

	pdc_free(pdc, tm_list->tmpmem[i].mem);
    }

    tm_list->size = 0;
}

void
pdc_insert_mem_tmp(
    pdc_core *          pdc,
    void *              memory,
    void *              opaque,
    pdc_destructor      destr)
{
    pdc_tmpmem_list *tm_list = &pdc->tm_list;

    if (tm_list->size == tm_list->capacity)
        pdc_tmlist_grow(pdc, tm_list);

    tm_list->tmpmem[tm_list->size].mem = memory;
    tm_list->tmpmem[tm_list->size].destr = destr;
    tm_list->tmpmem[tm_list->size].opaque = opaque;
    ++tm_list->size;
}

void *
pdc_malloc_tmp(
    pdc_core *          pdc,
    size_t              size,
    const char *        caller,
    void *              opaque,
    pdc_destructor      destr)
{
    void *memory = pdc_malloc(pdc, size, caller);

    pdc_insert_mem_tmp(pdc, memory, opaque, destr);

    return memory;
}

void *
pdc_calloc_tmp(
    pdc_core *		pdc,
    size_t		size,
    const char *	caller,
    void *		opaque,
    pdc_destructor	destr)
{
    void *memory = pdc_calloc(pdc, size, caller);

    pdc_insert_mem_tmp(pdc, memory, opaque, destr);

    return memory;
}

void *
pdc_realloc_tmp(pdc_core *pdc, void *mem, size_t size, const char *caller)
{
    int i;
    pdc_tmpmem_list *tm_list = &pdc->tm_list;

    for (i = tm_list->size - 1; 0 <= i; --i)
	if (tm_list->tmpmem[i].mem == mem)
	    return tm_list->tmpmem[i].mem = pdc_realloc(pdc, mem, size, caller);

    pdc_error(pdc, PDC_E_INT_REALLOC_TMP, caller, 0, 0, 0);
    return (void *) 0;
}

void
pdc_free_tmp(pdc_core *pdc, void *mem)
{
    int i, j;
    pdc_tmpmem_list *tm_list = &pdc->tm_list;

    /* we search the list backwards since chances are good
    ** that the most recently allocated items are freed first.
    */
    for (i = tm_list->size - 1; 0 <= i; --i)
    {
	if (tm_list->tmpmem[i].mem == mem)
	{
	    if (tm_list->tmpmem[i].destr)
		tm_list->tmpmem[i].destr(
		    tm_list->tmpmem[i].opaque, tm_list->tmpmem[i].mem);

	    pdc_free(pdc, tm_list->tmpmem[i].mem);
	    tm_list->tmpmem[i].mem = (void *) 0;

            --tm_list->size;
            for (j = i; j < tm_list->size; j++)
                tm_list->tmpmem[j] = tm_list->tmpmem[j + 1];

	    return;
	}
    }

    pdc_error(pdc, PDC_E_INT_FREE_TMP, 0, 0, 0, 0);
}


/* --------------------------- exception handling --------------------------- */

const char *pdc_errprintf(pdc_core *pdc, const char *fmt, ...)
{
    va_list ap;

    if (pdc->epcount < 0 || pdc->epcount > 3)
        pdc->epcount = 0;

    va_start(ap, fmt);
    vsprintf(pdc->errparms[pdc->epcount], fmt, ap);
    va_end(ap);

    return pdc->errparms[pdc->epcount++];
}

static pdc_error_info *
get_error_info(pdc_core *pdc, int errnum)
{
    int n = (errnum / 1000) - 1;

    if (0 <= n && n < N_ERRTABS && pdc->err_tables[n].ei != 0)
    {
	error_table *etab = &pdc->err_tables[n];
	int i;

	/* LATER: binary search. */
	for (i = 0; i < etab->n_entries; ++i)
	{
	    if (etab->ei[i].errnum == errnum)
		return &etab->ei[i];
	}
    }

    pdc_panic(pdc, "Internal error: unknown error number %d", errnum);

    return (pdc_error_info *) 0;	/* for the compiler */
} /* get_error_info */


static void
make_errmsg(
    pdc_core *		pdc,
    pdc_error_info *	ei,
    const char *	parm1,
    const char *	parm2,
    const char *	parm3,
    const char *	parm4)
{
    const char *src = ei->ce_msg ? ei->ce_msg : ei->errmsg;
    char *	dst = pdc->errbuf;
    const char *dollar;

    pdc->epcount = 0;

    /* copy *src to *dst, replacing "$N" with *parmN.
    */
    while ((dollar = strchr(src, '$')) != (char *) 0)
    {
	const char *parm = (const char *) 0;

	memcpy(dst, src, (size_t) (dollar - src));
	dst += dollar - src;
	src = dollar + 1;

	switch (*src)
	{
	    case '1':	parm = (parm1 ? parm1 : "?");	break;
	    case '2':	parm = (parm2 ? parm2 : "?");	break;
	    case '3':	parm = (parm3 ? parm3 : "?");	break;
	    case '4':	parm = (parm4 ? parm4 : "?");	break;

	    case 0:	break;

	    default:	*(dst++) = *(src++);
			break;
	}

	if (parm != (const char *) 0)
	{
	    ++src;
	    strcpy(dst, parm);
	    dst += strlen(parm);
	}
    }

    strcpy(dst, src);
} /* make_errmsg */


void
pdc_set_errmsg(
    pdc_core *  pdc,
    int         errnum,
    const char *parm1,
    const char *parm2,
    const char *parm3,
    const char *parm4)
{
    pdc_error_info *ei = get_error_info(pdc, errnum);

    make_errmsg(pdc, ei, parm1, parm2, parm3, parm4);
    pdc->errnum = errnum;

    pdc_trace_protocol(pdc, 1, trc_warning,
        "\tError %d: \"%s\"\n", pdc->errnum, pdc->errbuf);
} /* pdc_set_errmsg */


void
pdc_error(
    pdc_core *	pdc,
    int		errnum,
    const char *parm1,
    const char *parm2,
    const char *parm3,
    const char *parm4)
{
    if (pdc->in_error)		/* avoid recursive errors. */
	return;
    else
    {
	pdc->in_error = pdc_true;
	pdc->x_thrown = pdc_true;
    }

    if (errnum != -1)
    {
	pdc_error_info *ei = get_error_info(pdc, errnum);

	make_errmsg(pdc, ei, parm1, parm2, parm3, parm4);
	pdc->errnum = errnum;
    }

    pdc_trace(pdc, "\n[+++ Exception %d in %s, %s +++]\n",
        pdc->errnum, (pdc->errnum == 0 || !pdc->apiname) ? "" : pdc->apiname,
	(pdc->x_sp == -1 ?  "Error handler active" : "try/catch active"));
    pdc_trace(pdc, "[\"%s\"]\n\n", pdc->errbuf);

    if (pdc->x_sp == -1)
    {
	char errbuf[PDC_ERRBUF_SIZE];

	sprintf(errbuf, "[%d] %s: %s",
	    pdc->errnum, pdc_get_apiname(pdc), pdc->errbuf);

	pdc->errorhandler(pdc->opaque, PDF_UnknownError, errbuf);

	/*
	 * The error handler must never return. If it does, it is severely
	 * broken. We cannot remedy this, so we exit.
	 */
	 exit(99);

    }
    else
    {
	longjmp(pdc->x_stack[pdc->x_sp].jbuf.jbuf, 1);
    }

} /* pdc_error */


void
pdc_warning(
    pdc_core *	pdc,
    int		errnum,
    const char *parm1,
    const char *parm2,
    const char *parm3,
    const char *parm4)
{
    if (pdc->in_error)
	return;

    if (pdc->warnings_enabled == pdc_false)
    {
        if (pdc_trace_protocol_is_enabled(pdc, 1, trc_warning))
        {
            if (errnum != -1)
            {
                pdc_error_info *ei = get_error_info(pdc, errnum);

                make_errmsg(pdc, ei, parm1, parm2, parm3, parm4);
                pdc->errnum = errnum;
            }

            pdc_trace(pdc, "\tWarning %d: \"%s\"\n", pdc->errnum, pdc->errbuf);
        }

        return;
    }

    pdc->in_error = pdc_true;
    pdc->x_thrown = pdc_true;

    if (errnum != -1)
    {
	pdc_error_info *ei = get_error_info(pdc, errnum);

	make_errmsg(pdc, ei, parm1, parm2, parm3, parm4);
	pdc->errnum = errnum;
    }

    pdc_trace(pdc, "\n[+++ Warning %d in %s, %s +++]\n",
        pdc->errnum, (pdc->errnum == 0 || !pdc->apiname) ? "" : pdc->apiname,
	(pdc->x_sp == -1 ?  "Error handler active" : "try/catch active"));
    pdc_trace(pdc, "[\"%s\"]\n\n", pdc->errbuf);

    if (pdc->x_sp == -1)
    {
	char errbuf[PDC_ERRBUF_SIZE];

	sprintf(errbuf, "[%d] %s: %s",
	    pdc->errnum, pdc_get_apiname(pdc), pdc->errbuf);

	(*pdc->errorhandler)(pdc->opaque, PDF_NonfatalError, errbuf);
    }
    else
    {
	longjmp(pdc->x_stack[pdc->x_sp].jbuf.jbuf, 1);
    }

    /* a client-supplied error handler may return after a warning */
    pdc->in_error = pdc_false;

} /* pdc_warning */


pdc_jmpbuf *
pdc_jbuf(pdc_core *pdc)
{
    static const char fn[] = "pdc_jbuf";

    if (++pdc->x_sp == pdc->x_ssize)
    {
	pdc_xframe *aux;

#ifdef PDC_ALIGN16
	char *cp = (char *) (*pdc->allocproc)(pdc->opaque,
			16 + 2 * pdc->x_ssize * sizeof (pdc_xframe), fn);

	if (cp == (char *) 0)
	{
	    aux = (pdc_xframe *) 0;
	}
	else
	{
	    pdc_free(pdc, pdc->x_alias);
	    pdc->x_alias = cp;
	    aux = (pdc_xframe *)
		(((unsigned long) cp + 16) & 0xFFFFFFFFFFFFFFF0);

	    memcpy(aux, pdc->x_stack, pdc->x_ssize * sizeof (pdc_xframe));
	}
#else
	aux = (pdc_xframe *) (*pdc->reallocproc)(
			pdc->opaque, pdc->x_stack,
			2 * pdc->x_ssize * sizeof (pdc_xframe), fn);
#endif

	if (aux == (pdc_xframe *) 0)
	{
	    --pdc->x_sp;
	    pdc->errnum = PDC_E_MEM_OUT;
	    pdc->x_thrown = pdc_true;
	    pdc->in_error = pdc_true;
	    strcpy(pdc->errbuf, "out of memory");
	    longjmp(pdc->x_stack[pdc->x_sp].jbuf.jbuf, 1);
	}

	pdc->x_stack = aux;
	pdc->x_ssize *= 2;
    }

    pdc->x_thrown = pdc_false;
    return &pdc->x_stack[pdc->x_sp].jbuf;
} /* pdc_jbuf */

void
pdc_exit_try(pdc_core *pdc)
{
    if (pdc->x_sp == -1)
    {
	strcpy(pdc->errbuf, "exception stack underflow");
	pdc->errnum = PDC_E_INT_XSTACK;
	(*pdc->errorhandler)(pdc->opaque, PDF_UnknownError, pdc->errbuf);
    }
    else
	--pdc->x_sp;
} /* pdc_exit_try */

int
pdc_catch_intern(pdc_core *pdc)
{
    pdc_bool result;

    if (pdc->x_sp == -1)
    {
	strcpy(pdc->errbuf, "exception stack underflow");
	pdc->errnum = PDC_E_INT_XSTACK;
	(*pdc->errorhandler)(pdc->opaque, PDF_UnknownError, pdc->errbuf);
    }
    else
	--pdc->x_sp;

    result = pdc->x_thrown;
    pdc->in_error = pdc_false;
    pdc->x_thrown = pdc_false;

    return result;
} /* pdc_catch_intern */

int
pdc_catch_extern(pdc_core *pdc)
{
    pdc_bool result;

    if (pdc->x_sp == -1)
    {
	strcpy(pdc->errbuf, "exception stack underflow");
	pdc->errnum = PDC_E_INT_XSTACK;
	(*pdc->errorhandler)(pdc->opaque, PDF_UnknownError, pdc->errbuf);
    }
    else
	--pdc->x_sp;

    result = pdc->x_thrown;
    pdc->x_thrown = pdc_false;

    return result;
} /* pdc_catch_extern */

void
pdc_rethrow(pdc_core *pdc)
{
    pdc_error(pdc, -1, 0, 0, 0, 0);
} /* pdc_rethrow */


int
pdc_get_errnum(pdc_core *pdc)
{
    return pdc->errnum;
}

const char *
pdc_get_errmsg(pdc_core *pdc)
{
    return (pdc->errnum == 0) ? "" : pdc->errbuf;
}

const char *
pdc_get_apiname(pdc_core *pdc)
{
    return (pdc->errnum == 0 || !pdc->apiname) ? "" : pdc->apiname;
}


/* --------------------------- debug trace  --------------------------- */

void
pdc_set_tracefile(pdc_core *pdc, const char *filename)
{
    if (!filename || !*filename)
        return;

    if (pdc->tracefilename)
        pdc_free(pdc, pdc->tracefilename);

    pdc->tracefilename = pdc_strdup(pdc, filename);
    pdc->tracestatus = pdc_undef;
}

static const pdc_keyconn pdc_strform_keylist[] =
{
    {"readable",   strform_readable},
    {"readable0",  strform_readable0},
    {"octal",      strform_octal},
    {"hex",        strform_hexa},
    {"java",       strform_java},
    {NULL, 0}
};

static const pdc_keyconn pdf_protoclass_keylist[] =
{
    {"other",      trc_other},
    {"api",        trc_api},
    {"optlist",    trc_optlist},
    {"memory",     trc_memory},
    {"warning",    trc_warning},
    {"resource",   trc_resource},
    {"filesearch", trc_filesearch},
    {"encoding",   trc_encoding},
    {"image",      trc_image},
    {"font",       trc_font},
    {NULL, 0}
};

static const pdc_defopt pdc_trace_options[] =
{
    {"enable", pdc_booleanlist, PDC_OPT_NONE, 1, 1,
      0.0, 0.0, NULL},

    {"disable", pdc_booleanlist, PDC_OPT_NONE, 1, 1,
      0.0, 0.0, NULL},

    {"flush", pdc_booleanlist, PDC_OPT_NONE, 1, 1,
      0.0, 0.0, NULL},

    {"remove", pdc_booleanlist, PDC_OPT_NONE, 1, 1,
      0.0, 0.0, NULL},

    {"filename", pdc_stringlist, PDC_OPT_NONE, 1, 1,
      0.0, 1024, NULL},

    {"stringformat", pdc_keywordlist, PDC_OPT_NONE, 1, 1,
      0.0, 0.0, pdc_strform_keylist},

    {"stringlimit", pdc_integerlist, PDC_OPT_NONE, 1, 1,
      0.0, PDF_MAXSTRINGSIZE, NULL},

    {"classes", pdc_stringlist, PDC_OPT_EVENNUM |PDC_OPT_SUBOPTLIST,
      1, 2 * PDC_CLASSLIST_SIZE, 1.0, 64, NULL},

    PDC_OPT_TERMINATE
};

pdc_bool
pdc_set_trace_options(pdc_core *pdc, const char *optlist)
{
    pdc_resopt *resopts = NULL;
    char **strlist, *keyword;
    pdc_bool enable = pdc_true;
    pdc_bool remfile = pdc_false;
    pdc_char level;
    int inum, i, pclass = 0, sumlevel = 0;

    if (optlist && strlen(optlist))
    {
        resopts = pdc_parse_optionlist(pdc, optlist, pdc_trace_options,
                                       NULL, pdc_true);

        enable = pdc_true;
        pdc_get_optvalues("enable", resopts, &enable, NULL);
        if (pdc_get_optvalues("disable", resopts, &inum, NULL))
            enable = !inum;

        pdc_get_optvalues("flush", resopts, &pdc->traceflush, NULL);

        pdc_get_optvalues("remove", resopts, &remfile, NULL);

        if (pdc_get_optvalues("filename", resopts, NULL, &strlist))
            pdc_set_tracefile(pdc, strlist[0]);

        if (pdc_get_optvalues("stringformat", resopts, &inum, NULL))
            pdc->tracestrform = (pdc_strform_kind) inum;

        pdc_get_optvalues("stringlimit", resopts, &pdc->tracemaxchar, NULL);

        inum = pdc_get_optvalues("classes", resopts, NULL, &strlist);
        if (inum)
        {
            for (i = 0; i < inum; i++, i++)
            {
                if (!pdc_stricmp(strlist[i], "other"))
                {
                    i++;
                    if (pdc_str2integer(strlist[i],
                                        PDC_INT_CHAR | PDC_INT_UNSIGNED,
                                        &level))
                    {
                        memset(pdc->traceclasslist, (int)level,
                               PDC_CLASSLIST_SIZE);
                    }
                    break;
                }
            }
            for (i = 0; i < inum; i++)
            {
                keyword = strlist[i];
                pclass = pdc_get_keycode_ci(keyword, pdf_protoclass_keylist);
                if (pclass == PDC_KEY_NOTFOUND)
                    pdc_error(pdc, PDC_E_OPT_ILLKEYWORD,
                              "classes", keyword, 0, 0);
                i++;
                if (!pdc_str2integer(strlist[i],
                                     PDC_INT_CHAR | PDC_INT_UNSIGNED,
                                     &level))
                    pdc_error(pdc, PDC_E_OPT_ILLINTEGER,
                              keyword, strlist[i], 0, 0);

                pdc->traceclasslist[pclass] = level;
            }

            for (i = 0; i < PDC_CLASSLIST_SIZE; i++)
                sumlevel += (int) pdc->traceclasslist[i];
            pdc->traceclassapi =
                (sumlevel == 1 && pdc->traceclasslist[trc_api]) ?
                pdc_true : pdc_false;
        }

        pdc_cleanup_optionlist(pdc, resopts);
    }

    if (!enable)
        pdc_set_trace(pdc, NULL);

    if (pdc->tracefp != NULL && pdc->tracefp != stdout && pdc->tracefp !=stderr)
        fclose(pdc->tracefp);
    pdc->tracefp = NULL;

    if (enable)
    {
        if (pdc->tracefilename == NULL)
            pdc->tracefilename = pdc_strdup(pdc, DEBUG_TRACE_FILE);

        if (remfile && strcmp(pdc->tracefilename, "-"))
            remove(pdc->tracefilename);

        if (!pdc->traceflush)
        {
            if (!strcmp(pdc->tracefilename, "stdout"))
                pdc->tracefp = stdout;
            else if (!strcmp(pdc->tracefilename, "stderr"))
                pdc->tracefp = stderr;
            else
                pdc->tracefp = fopen(pdc->tracefilename, APPENDMODE);
            if (pdc->tracefp == NULL)
            {
                pdc->tracestatus = pdc_false;
                pdc_error(pdc, PDC_E_IO_WROPEN, "log ", pdc->tracefilename,
                          0, 0);
            }
        }
    }

    return enable;
}

const char *
pdc_print_tracestring(pdc_core *pdc, const char *str, int len)
{
    if (pdc->tracestatus == pdc_true)
        return pdc_strprint(pdc, str, len, pdc->tracemaxchar,
                            pdc->tracestrform, pdc_false);
    else
        return "";
}

/* trace function without any class level check and decorations
 */
static void
pdc_trace_output(pdc_core *pdc, const char *fmt, va_list ap)
{
    if (pdc->traceflush)
    {
        FILE *fp = NULL;

        if (!strcmp(pdc->tracefilename, "stdout"))
            fp = stdout;
        else if (!strcmp(pdc->tracefilename, "stderr"))
            fp = stderr;
        else
            fp = fopen(pdc->tracefilename, APPENDMODE);

        if (fp == NULL)
        {
            pdc->tracestatus = pdc_false;
            pdc_error(pdc, PDC_E_IO_WROPEN, "log ", pdc->tracefilename,
                      0, 0);
        }

        pdc_vfprintf(pdc, fp, fmt, ap);

        if (fp != stdout && fp != stderr)
            fclose(fp);
    }
    else
    {
        pdc_vfprintf(pdc, pdc->tracefp, fmt, ap);
        fflush(pdc->tracefp);
    }
}

void
pdc_trace(pdc_core *pdc, const char *fmt, ...)
{
    if (pdc && pdc->tracestatus == pdc_true)
    {
        va_list ap;

        va_start(ap, fmt);
        pdc_trace_output(pdc, fmt, ap);
        va_end(ap);
    }
}


/* start or stop (client == NULL) a trace for the supplied client program
 */
void
pdc_set_trace(pdc_core *pdc, const char *client)
{
    const char *separstr =
         "[ --------------------------------------------------------- ]\n";

    pdc_bool	old_tracestatus = pdc->tracestatus;
    pdc_time	ltime;

    pdc_localtime(&ltime);

    if (old_tracestatus == pdc_true && client == NULL)
    {
        pdc_trace(pdc, "\n");
        pdc_trace(pdc, separstr);
    }

    pdc->tracestatus = client ? pdc_true : pdc_false;

    if (old_tracestatus == pdc_undef && pdc->tracestatus == pdc_true)
    {
        pdc_trace(pdc, separstr);
        pdc_trace(pdc, "[ %s on %s (%s) ", client, PDF_PLATFORM,
                  PDC_ISBIGENDIAN ? "be" : "le");
        pdc_trace(pdc, "%04d-%02d-%02d %02d:%02d:%02d ]\n",
            ltime.year + 1900, ltime.month + 1, ltime.mday,
            ltime.hour, ltime.minute, ltime.second);

        if (pdc->traceclassapi)
            pdc_trace(pdc, "[ Use  %%s/\\[[^]]*\\]//g  and  %%s/)$/);"
                           "/  in vi to compile it ]\n");
        pdc_trace(pdc, separstr);
    }
}

/* standard trace protocol functions for api function calls
 */
void
pdc_trace_enter_api(pdc_core *pdc, const char *funame,
                    const char *fmt, va_list args)
{
    if (pdc && pdc->tracestatus == pdc_true && pdc->traceclasslist[trc_api])
    {
        /* time stamp */
        if (pdc->traceclasslist[trc_api] > 1)
        {
            pdc_time ltime;

            pdc_localtime(&ltime);
            pdc_trace(pdc, "[%02d:%02d:%02d] ",
                      ltime.hour, ltime.minute, ltime.second);
        }

        /* function name */
        pdc_trace(pdc, "%s", funame);

        /* function arg list */
        pdc_trace_output(pdc, fmt, args);

        /* linefeed if not only API calls */
        if (!pdc->traceclassapi)
            pdc_trace(pdc, "\n");
    }
}

void
pdc_trace_exit_api(pdc_core *pdc, pdc_bool cleanup, const char *fmt, ...)
{
    if (pdc->tracestatus == pdc_true && pdc->traceclasslist[trc_api] &&
        fmt != NULL)
    {
        va_list ap;

        va_start(ap, fmt);
        pdc_trace_output(pdc, fmt, ap);
        va_end(ap);
    }

    if (cleanup)
        pdc_tmlist_cleanup(pdc, &pdc->tm_list);
}

/* general trace protocol functions
 */
pdc_bool
pdc_trace_protocol_is_enabled(pdc_core *pdc, int level, int pclass)
{
    return pdc->tracestatus == pdc_true && level <= pdc->traceclasslist[pclass];
}

void
pdc_trace_protocol(pdc_core *pdc, int level, int pclass, const char *fmt, ...)
{
    if (pdc && pdc->tracestatus == pdc_true &&
        level <= pdc->traceclasslist[pclass])
    {
        va_list ap;

        va_start(ap, fmt);
        pdc_trace_output(pdc, fmt, ap);
        va_end(ap);
    }
}


/* ------------ encoding handling ------------ */

void
pdc_set_pglyphtab_ptr(pdc_core *pdc, void *pglyphtab_ptr)
{
    pdc->pglyphtab = pglyphtab_ptr;
}

void *
pdc_get_pglyphtab_ptr(pdc_core *pdc)
{
    return pdc->pglyphtab;
}



/* ----------- service function to get PDF version string  -------------- */

const char *
pdc_get_pdfversion(pdc_core *pdc, int compatibility)
{
    return pdc_errprintf(pdc, "%d.%d", compatibility / 10, compatibility % 10);
}


/* --------------------------- float digits  --------------------------- */

void
pdc_set_floatdigits(pdc_core *pdc, int val)
{
    pdc->floatdigits = val;
}

int
pdc_get_floatdigits(pdc_core *pdc)
{
    return pdc->floatdigits;
}



#ifdef	PDC_DEBUG
/* --------------------------- debug hexdump  --------------------------- */
void
pdc_enable_hexdump(pdc_core *pdc)
{
    pdc->hexdump = pdc_true;
}

void
pdc_disable_hexdump(pdc_core *pdc)
{
    pdc->hexdump = pdc_false;
}

void
pdc_hexdump(pdc_core *pdc, const char *msg, const char *text, int tlen)
{
    if (pdc->hexdump)
    {
	int i, k;

	if (tlen == 1)
	{
	    printf("%s: %02X '%c'\n", msg,
			(unsigned char) text[0],
			isprint(text[0]) ? text[0] : '.');
	}
	else
	{
	    printf("%s:\n", msg);

	    for (i = 0; i < tlen; i += 16)
	    {
		for (k = 0; k < 16; ++k)
		    if (i + k < tlen)
			printf("%02X ", (unsigned char) text[i + k]);
		    else
			printf("   ");

		printf(" ");
		for (k = 0; k < 16; ++k)
		    if (i + k < tlen)
			printf("%c", isprint(text[i + k]) ? text[i + k] : '.');
		    else
			printf("   ");

		printf("\n");
	    }
	}
    }
}

#endif /* PDC_DEBUG */
