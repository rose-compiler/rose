/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/dapdebug.h,v 1.33 2009/12/03 18:53:16 dmh Exp $
 *********************************************************************/
#ifndef DEBUG_H
#define DEBUG_H

#undef XX

#include <stdarg.h>
#include <assert.h>

#undef DBG
#undef DEBUG
#ifdef XX
#define DBG
#define DEBUG
#endif

#undef DD
#undef SHOWDDX

/* Warning: setting CATCHERROR has significant performance impact */
#undef CATCHERROR
#ifdef DEBUG
#undef CATCHERROR
#define CATCHERROR
#endif

#define PANIC(msg) assert(dappanic(msg));
#define PANIC1(msg,arg) assert(dappanic(msg,arg));
#define PANIC2(msg,arg1,arg2) assert(dappanic(msg,arg1,arg2));

#define ASSERT(expr) if(!(expr)) {PANIC(#expr);} else {}

extern int ncdap3debug;

extern int dappanic(const char* fmt, ...);

/*
Provide wrapped versions of calloc and malloc.
The wrapped version panics if memory
is exhausted.  It also guarantees that the
memory has been zero'd.
*/

#define ecalloc(x,y) dapcalloc(x,y)
#define emalloc(x)   dapcalloc(x,1)
#define efree(x) dapfree(x)
extern void* dapcalloc(size_t size, size_t nelems);
extern void* dapmalloc(size_t size);
extern void  dapfree(void*);

#define MEMCHECK(var,throw) {if((var)==NULL) return (throw);}

#ifdef CATCHERROR
/* Place breakpoint on dapbreakpoint to catch errors close to where they occur*/
#define THROW(e) dapthrow(e)
#define THROWCHK(e) (void)dapthrow(e)

extern int dapbreakpoint(int err);
extern int dapthrow(int err);
#else
#define THROW(e) (e)
#define THROWCHK(e)
#endif

#endif /*DEBUG_H*/

