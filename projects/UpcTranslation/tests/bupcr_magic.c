//#define UPCR_NO_SRCPOS
#include <upcr.h>
/*
GASNETT_IDENT(UPCRI_IdentString_DefaultPthreadCount, 
 "$UPCRDefaultPthreadCount: 2 $");
*/
extern char volatile UPCRI_IdentString_DefaultPthreadCount[]; 
char volatile UPCRI_IdentString_DefaultPthreadCount[] = "$UPCRDefaultPthreadCount: 2 $"; 
extern char *_UPCRI_IdentString_DefaultPthreadCount_identfn() 
{ return (char*)UPCRI_IdentString_DefaultPthreadCount; } 
//static int _dummy_UPCRI_IdentString_DefaultPthreadCount = sizeof(_dummy_UPCRI_IdentString_DefaultPthreadCount);
static int _dummy_UPCRI_IdentString_DefaultPthreadCount = sizeof(int);

/* declaration global variables for TLD data 
 *   just global variables, not in a struct any more??
 */
#if UPCRI_USING_TLD
  #undef UPCR_TLD_DEFINE
  #define UPCR_TLD_DEFINE(name, size, align) extern char name[size];
  #define UPCR_TRANSLATOR_TLD(type, name, initval) extern type name;
  #include "global.tld"
  #include <upcr_translator_tld.h>
  #undef UPCR_TLD_DEFINE
  #undef UPCR_TRANSLATOR_TLD
#endif /* UPCRI_USING_TLD */

/*
 * Initialize the global variables for TLD
 * */
upcri_pthreadinfo_t*
upcri_linkergenerated_tld_init()
{
    //struct upcri_tld_t *_upcri_p = UPCRI_XMALLOC(struct upcri_tld_t, 1);
    struct upcri_tld_t *_upcri_p = upcri_checkmalloc(sizeof(struct upcri_tld_t)*1);

#if UPCRI_USING_TLD
  #define UPCR_TRANSLATOR_TLD(type, name, initval) memcpy(&_upcri_p->name, &name, sizeof(type));
  #include <upcr_translator_tld.h>
  #define UPCR_TLD_DEFINE(name, size, align) memcpy(&_upcri_p->name, &name, size);
  #include "global.tld"
#endif /* UPCRI_USING_TLD */
    return (upcri_pthreadinfo_t *) _upcri_p;
}

/* need to add all per file allocation functions into here
  e.g. UPCRI_ALLOC_extern_2353585087();
*/
static
void perfile_allocs() 
{
}

/* need to add all per file init functions into here
 e.g. UPCRI_INIT_extern_2353585087(); 
*/ 
static
void perfile_inits()
{
}

void (*UPCRL_trans_extra_procinit)();
void (*UPCRL_trans_extra_threadinit)();

static
void static_init(void *start, uintptr_t len)
{
    UPCR_BEGIN_FUNCTION();
    /* we ignore the start/len params, since we allocate all static 
     * data off the heap */

    if (UPCRL_trans_extra_procinit) {
      upcr_notify(0, UPCR_BARRIERFLAG_ANONYMOUS);
      upcr_wait(0, UPCR_BARRIERFLAG_ANONYMOUS);
      if (upcri_mypthread() == 0) (*UPCRL_trans_extra_procinit)();
      upcr_notify(0, UPCR_BARRIERFLAG_ANONYMOUS);
      upcr_wait(0, UPCR_BARRIERFLAG_ANONYMOUS);
    }

    /* Call per-file alloc/init functions */
    perfile_allocs();

    /* Do a barrier to make sure all allocations finish, before calling
     * initialization functions.  */
    /* UPCR_SET_SRCPOS() was already done in perfile_allocs() */ ;
    upcr_notify(0, UPCR_BARRIERFLAG_ANONYMOUS);
    upcr_wait(0, UPCR_BARRIERFLAG_ANONYMOUS);

    /* now set any initial values (also for TLD) */
    perfile_inits();

    if (UPCRL_trans_extra_threadinit) {
      upcr_notify(0, UPCR_BARRIERFLAG_ANONYMOUS);
      upcr_wait(0, UPCR_BARRIERFLAG_ANONYMOUS);
      (*UPCRL_trans_extra_threadinit)();
      upcr_notify(0, UPCR_BARRIERFLAG_ANONYMOUS);
      upcr_wait(0, UPCR_BARRIERFLAG_ANONYMOUS);
    }
}

extern void upcri_init_heaps(void *start, uintptr_t len);
extern void upcri_init_cache(void *start, uintptr_t len);

/* Magic linker variables, for initialization */
upcr_thread_t   UPCRL_static_thread_count       = 0;
uintptr_t       UPCRL_default_shared_size       = ((uint64_t)64) << 20;
uintptr_t       UPCRL_default_shared_offset     = ((uint64_t)0) << 20;
/* uint64_t gasnet_max_segsize = 0; */ /* not set */
int             UPCRL_progress_thread           = 0;
uintptr_t	UPCRL_default_cache_size        = 0;
int		UPCRL_attach_flags              = UPCR_ATTACH_ENV_OVERRIDE|UPCR_ATTACH_SIZE_WARN;
const char *	UPCRL_main_name;
upcr_thread_t   UPCRL_default_pthreads_per_node = 2;
int             UPCRL_segsym_pow2_opt		= 0;
void (*UPCRL_pre_spawn_init)()                       = NULL;
void (*UPCRL_per_pthread_init)()                     = NULL;
void (*UPCRL_static_init)(void *, uintptr_t)         = &static_init;
void (*UPCRL_heap_init)(void * start, uintptr_t len) = &upcri_init_heaps;
void (*UPCRL_cache_init)(void *start, uintptr_t len) = &upcri_init_cache;
void (*UPCRL_mpi_init)(int *pargc, char ***pargv);
void (*UPCRL_mpi_finalize)();
void (*UPCRL_profile_finalize)();
/* 
 * We can also generate the following code inside the file with the original user main() function
 * upcTranslation::generateNewMainFunction()
 */ 
extern int user_main(int, char**);
const char * UPCRL_main_name = "user_main";

int main(int argc, char **argv)
{
    bupc_init_reentrant(&argc, &argv, &user_main);
    return 0;
}

/* ------ Helper functions for UPC forall optimizations -------- */

/*
 *  Compute the GCD of two integers
 *  FIXME: does this work for negative ints?
 */
int upcrt_gcd(int a, int b) {
  return (b == 0) ? a : upcrt_gcd(b, a % b);
}

/* Compute val such that (i = val) is the first iteration in the forall loop 
 * that MYTHREAD will execute.
 * start_thread is the thread that executes the first ieration (i.e., i = lower_bound)
 * step is the step of the loop.
 * lo is the lower bound of the loop (the initial value of i)
 */
int _upcrt_forall_start(int start_thread, int step, int lo UPCRI_PT_ARG) {

  int threads = upcr_threads();
  int mythread = upcr_mythread();
  int gcd_val = upcrt_gcd(step, threads);
  int dist = start_thread - mythread;
  int ans;
  if (dist % gcd_val != 0) {
    /* this thread never executes the loop */
    return step > 0 ? (int)~(((unsigned int)1) << (8*sizeof(int)-1)) /*INT_MAX*/ :
                      (int)(((unsigned int)1) << (8*sizeof(int)-1)) /*INT_MIN*/;
  }
  if (step > 0) {
    for (ans = lo; ans < lo + step * threads; ans += step) {
      if (dist == 0) {
        /* fprintf(stderr, "th %d: forall_start(%d, %d, %d) = %d\n", mythread, start_thread, step, lo, ans); */
        return ans;
      }
      dist = (dist + step) % threads;
    }
  } else {
    for (ans = lo; ans > lo + step * threads; ans += step) {
      if (dist == 0) {
        /* fprintf(stderr, "th %d: forall_start(%d, %d, %d) = %d\n", mythread, start_thread, step, lo, ans); */
        return ans;
      }
      dist = (dist + step) % threads;
    }
  }

  abort(); /* shouldn't reach here */
  return 0;
}
