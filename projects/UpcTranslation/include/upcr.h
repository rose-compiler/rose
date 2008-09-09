#ifndef UPCR_H
#define UPCR_H

/*A minimum header to support the UPC translation using ROSE
 * when a full installation of the Berkeley UPC runtime system is 
 * not available
 *
 * Warning! The declarations here are not meant to be accurate!
 * They are only used to prevent compiler warnings.
 * 
 * By Liao 9/5/2008
 * */
#include <stdio.h>
extern void  abort(void);

/* portable_inttypes.h */
typedef short             int32_t;
typedef unsigned short   uint32_t;
typedef unsigned long    uint64_t;
typedef unsigned int    uintptr_t; 

#ifndef NULL
#define NULL 0
#endif

/* upcr_sptr.h */

typedef struct shared_ptr_struct {
    uintptr_t   s_addr;   /* First field to speed pointer use	    */
    uint32_t    s_thread; 
    uint32_t    s_phase;
} upcr_shared_ptr_t;

typedef struct shared_ptr_struct upcr_pshared_ptr_t;

/* upcr_init.h */
#define UPCR_ATTACH_ENV_OVERRIDE  1
#define UPCR_ATTACH_REQUIRE_SIZE  2
#define UPCR_ATTACH_SIZE_WARN   4
void bupc_init_reentrant(int *pargc, char ***pargv,int (*user_func)(int, char**));

typedef struct upcr_startup_shalloc_S {
   upcr_shared_ptr_t *sptr_addr;
   size_t blockbytes;
   size_t numblocks;
   int    mult_by_threads;
   size_t elemsz;
   const char *namestr;
   const char *typestr;
} upcr_startup_shalloc_t;

typedef struct upcr_startup_pshalloc_S {
   upcr_pshared_ptr_t *psptr_addr;
   size_t blockbytes;
   size_t numblocks;
   int    mult_by_threads;
   size_t elemsz;
   const char *namestr;
   const char *typestr;
} upcr_startup_pshalloc_t;

extern void upcr_startup_pshalloc(upcr_startup_pshalloc_t *infos, 
			    size_t count);
/* upcr_threads.h */
typedef unsigned int upcr_thread_t;
typedef struct upcri_pthreadinfo {
  upcr_thread_t mythread;
} upcri_pthreadinfo_t;

//#define UPCR_TLD_DEFINE(name, size, align) name
#define UPCR_TLD_DEFINE(name, size, align)  char name[size];
#define UPCR_TLD_DEFINE_TENTATIVE(name, size, align) name

struct upcri_tld_t {
  upcri_pthreadinfo_t tinfo; /* MUST be first member of struct */
#include "global.tld"
//#  include <upcr_translator_tld.h>
};
extern upcri_pthreadinfo_t * upcri_mypthreadinfo();
#define UPCR_TLD_ADDR(name) ((void *)&((struct upcri_tld_t *)upcri_mypthreadinfo())->name)

extern void UPCR_BEGIN_FUNCTION();
extern upcr_thread_t upcr_mythread();
extern upcr_thread_t upcri_mypthread();
extern upcr_thread_t upcr_threads();


/* upcr_barrier.h */
#define UPCR_BARRIERFLAG_ANONYMOUS 1
extern void upcr_notify(int, int);
extern void upcr_wait(int,int);

/* upcr_alloc.h */

//extern struct upcri_tld_t * UPCRI_XMALLOC(struct upcri_tld_t *, int);  
extern void * upcri_checkmalloc(size_t bytes);


/* ueth.h */
//typedef unsigned int    uintptr_t;

/* bupc_assistant.h */
typedef struct uda_thread_      uda_thread;

/* bupc_totalview_sprt.h */
#define UPCRI_PT_ARG_ALONE  uda_thread *thread
#define UPCRI_PT_ARG	    , UPCRI_PT_ARG_ALONE


/* upcr_err.h */
#define UPCR_SET_SRCPOS(file,line)

#endif  // end if def header string
