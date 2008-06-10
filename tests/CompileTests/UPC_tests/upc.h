/* UPC utility library functions, based on UPC 1.2 Specifications
 *
 * Functions are listed as the same order of the Specification.
 *
 * Liao
 * 6/9/2008
 */
 
#ifndef upc_utilities_INCLUDED
#define upc_utilities_INCLUDED

#include <stdio.h> /*for size_t*/
/*7.2.1 Termination of all threads*/
extern void upc_global_exit(int status);

/* 7.2.2 Shared memory allocation functions*/
extern shared void *upc_global_alloc (size_t, size_t);
extern shared void *upc_all_alloc (size_t, size_t);
extern shared void *upc_alloc(size_t);
extern shared void *upc_local_alloc (size_t, size_t); /*Deprecated*/
extern void upc_free (shared void *);

/* 7.2.3 Pointer-to-shared manipulation functions */
extern size_t upc_threadof(shared void *);
extern size_t upc_phaseof (shared void *);
extern shared void *upc_resetphase(shared void *);
extern size_t upc_addrfield (shared void *);
extern size_t upc_affinitysize(size_t, size_t, size_t);

/* 7.2.4 Lock functions */
typedef shared void upc_lock_t;
extern upc_lock_t *upc_global_lock_alloc(void);
extern upc_lock_t *upc_all_lock_alloc ();
extern void upc_lock_free(upc_lock_t *ptr);
extern void upc_lock(upc_lock_t*);
extern int upc_lock_attempt (upc_lock_t *);
extern void upc_unlock (upc_lock_t *);

/* 7.2.5 Shared string handling functions */
extern void upc_memcpy(shared void * restrict, shared const void * restrict, size_t);
extern void upc_memget(void * restrict, shared const void * restrict, size_t);
extern void upc_memput(shared void * restrict, const void * restrict, size_t);
extern void upc_memset(shared void *, int, size_t);

#endif

