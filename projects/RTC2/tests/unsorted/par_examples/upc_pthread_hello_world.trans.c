/* --- UPCR system headers --- */ 
#include "upcr.h" 
#include "whirl2c.h"
#include "upcr_proxy.h"
/*******************************************************
 * C file translated from WHIRL Tue Jun  5 11:49:45 2012
 *******************************************************/

/* UPC Runtime specification expected: 3.6 */
#define UPCR_WANT_MAJOR 3
#define UPCR_WANT_MINOR 6
/* UPC translator version: release 2.14.2, built on May 14 2012 at 17:11:26, host africanlily.llnl.gov darwin-i686/32, Apple gcc v4.2.1 (Apple Inc. build 5646) */
/* Included code from the initialization script */
#include "upcr_geninclude/stddef.h"
#include</Users/vanka1/research/compilers/bupc/berkeley_upc-2.14.2/opt/include/upcr_preinclude/upc_bits.h>
#include "upcr_geninclude/stdlib.h"
#include "upcr_geninclude/inttypes.h"
#include "upcr_geninclude/stdio.h"
#include</usr/include/pthread.h>
#line 1 "upc_pthread_hello_world.w2c.h"
/* Include builtin types and operators */

#ifndef UPCR_TRANS_EXTRA_INCL
#define UPCR_TRANS_EXTRA_INCL
extern int upcrt_gcd (int _a, int _b);
extern int _upcrt_forall_start(int _start_thread, int _step, int _lo, int _scale);
#define upcrt_forall_start(start_thread, step, lo, scale)  \
       _upcrt_forall_start(start_thread, step, lo, scale)
int32_t UPCR_TLD_DEFINE_TENTATIVE(upcrt_forall_control, 4, 4);
#define upcr_forall_control upcrt_forall_control
#ifndef UPCR_EXIT_FUNCTION
#define UPCR_EXIT_FUNCTION() ((void)0)
#endif
#if UPCR_RUNTIME_SPEC_MAJOR > 3 || (UPCR_RUNTIME_SPEC_MAJOR == 3 && UPCR_RUNTIME_SPEC_MINOR >= 8)
  #define UPCRT_STARTUP_SHALLOC(sptr, blockbytes, numblocks, mult_by_threads, elemsz, typestr) \
      { &(sptr), (blockbytes), (numblocks), (mult_by_threads), (elemsz), #sptr, (typestr) }
#else
  #define UPCRT_STARTUP_SHALLOC(sptr, blockbytes, numblocks, mult_by_threads, elemsz, typestr) \
      { &(sptr), (blockbytes), (numblocks), (mult_by_threads) }
#endif
#define UPCRT_STARTUP_PSHALLOC UPCRT_STARTUP_SHALLOC

/**** Autonb optimization ********/

extern void _upcrt_memput_nb(upcr_shared_ptr_t _dst, const void *_src, size_t _n);
#define upcrt_memput_nb(dst, src, n) \
       (upcri_srcpos(), _upcrt_memput_nb(dst, src, n))

#endif


/* Types */
/* File-level vars and routines */
extern void * pthread_hello_world(void *);

extern int user_main();


#define UPCR_SHARED_SIZE_ 8
#define UPCR_PSHARED_SIZE_ 8

void UPCRI_ALLOC_upc_pthread_hello_world_2672127685(void) { 
UPCR_BEGIN_FUNCTION();

UPCR_SET_SRCPOS("_upc_pthread_hello_world_2672127685_ALLOC",0);
}

void UPCRI_INIT_upc_pthread_hello_world_2672127685(void) { 
UPCR_BEGIN_FUNCTION();
UPCR_SET_SRCPOS("_upc_pthread_hello_world_2672127685_INIT",0);
}

#line 5 "upc_pthread_hello_world.upc"
extern void * pthread_hello_world(
  void * m)
#line 5 "upc_pthread_hello_world.upc"
{
#line 5 "upc_pthread_hello_world.upc"
  UPCR_BEGIN_FUNCTION();
  char * _bupc__casttmp0;
  
#line 6 "upc_pthread_hello_world.upc"
  _bupc__casttmp0 = (char *) m;
#line 6 "upc_pthread_hello_world.upc"
  printf("%s: hello_world\n", _bupc__casttmp0);
#line 7 "upc_pthread_hello_world.upc"
  UPCR_EXIT_FUNCTION();
#line 7 "upc_pthread_hello_world.upc"
  return(void *) 0U;
} /* pthread_hello_world */


#line 10 "upc_pthread_hello_world.upc"
extern int user_main()
#line 10 "upc_pthread_hello_world.upc"
{
#line 10 "upc_pthread_hello_world.upc"
  UPCR_BEGIN_FUNCTION();
  struct _opaque_pthread_t * tx;
  
#line 12 "upc_pthread_hello_world.upc"
  if(((int) upcr_mythread () ) == 0)
#line 12 "upc_pthread_hello_world.upc"
  {
#line 13 "upc_pthread_hello_world.upc"
    pthread_create(&tx, (const struct _opaque_pthread_attr_t *) 0U, &pthread_hello_world, "ThreadX");
  }
#line 15 "upc_pthread_hello_world.upc"
  printf("Hello from %d of %d\n", ((int) upcr_mythread () ), ((int) upcr_threads () ));
#line 16 "upc_pthread_hello_world.upc"
  upcr_barrier(-1622839611, 1);
#line 17 "upc_pthread_hello_world.upc"
  printf("Second Hello World from %d of %d\n", ((int) upcr_mythread () ), ((int) upcr_threads () ));
#line 18 "upc_pthread_hello_world.upc"
  pthread_exit((void *) 0U);
#line 19 "upc_pthread_hello_world.upc"
  UPCR_EXIT_FUNCTION();
#line 19 "upc_pthread_hello_world.upc"
  return 1;
} /* user_main */

#line 1 "_SYSTEM"
/**************************************************************************/
/* upcc-generated strings for configuration consistency checks            */

GASNETT_IDENT(UPCRI_IdentString_upc_pthread_hello_world_o_1338922185_GASNetConfig_gen, 
 "$GASNetConfig: (/var/folders/kd/kdVECwdpHSeKwaqanpfDlU++Zak/-Tmp-//upcc-vanka1-92380-1338922185/upc_pthread_hello_world.trans.c) RELEASE=1.18.2,SPEC=1.8,CONDUIT=SMP(SMP-1.9/SMP-1.1),THREADMODEL=PAR,SEGMENT=FAST,PTR=32bit,align,nopshm,nodebug,notrace,nostats,nodebugmalloc,nosrclines,timers_native,membars_native,atomics_native,atomic32_native,atomic64_native $");
GASNETT_IDENT(UPCRI_IdentString_upc_pthread_hello_world_o_1338922185_UPCRConfig_gen,
 "$UPCRConfig: (/var/folders/kd/kdVECwdpHSeKwaqanpfDlU++Zak/-Tmp-//upcc-vanka1-92380-1338922185/upc_pthread_hello_world.trans.c) VERSION=2.14.2,PLATFORMENV=shared,SHMEM=pthreads,SHAREDPTRREP=packed/p22:t10:a32,TRANS=berkeleyupc,nodebug,nogasp,notv,dynamicthreads $");
GASNETT_IDENT(UPCRI_IdentString_upc_pthread_hello_world_o_1338922185_translatetime, 
 "$UPCTranslateTime: (upc_pthread_hello_world.o) Tue Jun  5 11:49:45 2012 $");
GASNETT_IDENT(UPCRI_IdentString_upc_pthread_hello_world_o_1338922185_GASNetConfig_obj, 
 "$GASNetConfig: (upc_pthread_hello_world.o) " GASNET_CONFIG_STRING " $");
GASNETT_IDENT(UPCRI_IdentString_upc_pthread_hello_world_o_1338922185_UPCRConfig_obj,
 "$UPCRConfig: (upc_pthread_hello_world.o) " UPCR_CONFIG_STRING UPCRI_THREADCONFIG_STR " $");
GASNETT_IDENT(UPCRI_IdentString_upc_pthread_hello_world_o_1338922185_translator, 
 "$UPCTranslator: (upc_pthread_hello_world.o) /Users/vanka1/research/compilers/bupc/berkeley_upc-2.14.2/trans/targ (africanlily.llnl.gov) $");
GASNETT_IDENT(UPCRI_IdentString_upc_pthread_hello_world_o_1338922185_upcver, 
 "$UPCVersion: (upc_pthread_hello_world.o) 2.14.2 $");
GASNETT_IDENT(UPCRI_IdentString_upc_pthread_hello_world_o_1338922185_compileline, 
 "$UPCCompileLine: (upc_pthread_hello_world.o) /Users/vanka1/research/compilers/bupc/berkeley_upc-2.14.2/opt/bin/upcc.pl -trans -pthreads upc_pthread_hello_world.upc $");
GASNETT_IDENT(UPCRI_IdentString_upc_pthread_hello_world_o_1338922185_compiletime, 
 "$UPCCompileTime: (upc_pthread_hello_world.o) " __DATE__ " " __TIME__ " $");
#ifndef UPCRI_CC /* ensure backward compatilibity for http netcompile */
#define UPCRI_CC <unknown>
#endif
GASNETT_IDENT(UPCRI_IdentString_upc_pthread_hello_world_o_1338922185_backendcompiler, 
 "$UPCRBackendCompiler: (upc_pthread_hello_world.o) " _STRINGIFY(UPCRI_CC) " $");

#ifdef GASNETT_CONFIGURE_MISMATCH
  GASNETT_IDENT(UPCRI_IdentString_upc_pthread_hello_world_o_1338922185_configuremismatch, 
   "$UPCRConfigureMismatch: (upc_pthread_hello_world.o) 1 $");
  GASNETT_IDENT(UPCRI_IdentString_upc_pthread_hello_world_o_1338922185_configuredcompiler, 
   "$UPCRConfigureCompiler: (upc_pthread_hello_world.o) " GASNETT_PLATFORM_COMPILER_IDSTR " $");
  GASNETT_IDENT(UPCRI_IdentString_upc_pthread_hello_world_o_1338922185_buildcompiler, 
   "$UPCRBuildCompiler: (upc_pthread_hello_world.o) " PLATFORM_COMPILER_IDSTR " $");
#endif

/**************************************************************************/
GASNETT_IDENT(UPCRI_IdentString_upc_pthread_hello_world_o_1338922185_transver_2,
 "$UPCTranslatorVersion: (upc_pthread_hello_world.o) 2.14.2, built on May 14 2012 at 17:11:26, host africanlily.llnl.gov darwin-i686/32, Apple gcc v4.2.1 (Apple Inc. build 5646) $");
