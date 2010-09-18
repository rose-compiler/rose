#ifndef _BUPC_EXTENSIONS_H_
#define _BUPC_EXTENSIONS_H_

#if !defined(__BERKELEY_UPC_FIRST_PREPROCESS__) && !defined(__BERKELEY_UPC_ONLY_PREPROCESS__)
#error This file should only be included during initial preprocess
#endif

#include <upc.h>

/* 
 * Non-standard Berkeley UPC extensions 
 */

#if UPCRI_LIBWRAP
  UPCRI_LIBWRAP_FN 
  void bupc_poll(void) {
    upc_fence;
    upcr_poll();
  }
  #undef upc_poll
  #define upc_poll bupc_poll
#endif

/* Loads a null-terminated string description of a shared pointer into a
 * buffer.  Returns 0 on success, or -1 (and sets errno) if an error occurs.
 * The buffer passed should be at least UPC_DUMP_MIN_LENGTH chars in length,
 * else an error will occur (with errno set to EINVAL).
 */
#define BUPC_DUMP_MIN_LENGTH 100
#if UPCRI_LIBWRAP
  UPCRI_LIBWRAP_FN
  int bupc_dump_shared(shared const void *_ptr, char *_buf, int _maxlen) {
    return _bupc_dump_shared(upcri_bless_SVP2shared(_ptr), _buf, _maxlen);
  }
#else
  #define bupc_dump_shared _bupc_dump_shared
  extern int bupc_dump_shared(shared const void *_ptr, char *_buf, int _maxlen);
#endif

/* for backward compatibility */
#define UPC_DUMP_MIN_LENGTH BUPC_DUMP_MIN_LENGTH
#define upc_dump_shared(ptr, buf, maxlen) bupc_dump_shared(ptr, buf, maxlen)

/* See Brian Wibecam's proposal
 * https://hermes.gwu.edu/cgi-bin/wa?A2=ind0909&L=upc&D=0&P=514&F=P
 *
 * bupc_cast() - return pointer if locally addressable or NULL (otherwise undefined)
 * bupc_castable() - return TRUE if locally addressable or NULL
 * bupc_thread_castable() - return TRUE if all of thread'd data locally addressable
 */
#if UPCRI_LIBWRAP
  UPCRI_LIBWRAP_FN
  void * bupc_cast(shared const void *_ptr) {
    return _bupc_cast(upcri_bless_SVP2shared(_ptr));
  }
  UPCRI_LIBWRAP_FN
  int bupc_castable(shared const void *_ptr) {
    return _bupc_castable(upcri_bless_SVP2shared(_ptr));
  }
#else
  #define bupc_cast _bupc_cast
  extern void * bupc_cast(shared const void *_ptr);
  #define bupc_castable _bupc_castable
  extern int bupc_castable(shared const void *_ptr);
#endif
#define bupc_thread_castable _bupc_thread_castable
extern int bupc_thread_castable(unsigned int _thr);

/* Converts a local pointer into a shared one, setting the thread and phase to
 * arbitrary values.
 * - 'ptr':  Must point to a valid address in the calling thread's shared
 *	     region.
 * - 'thread': thread that resulting shared ptr will point to.
 * - 'phase': the value for the phase of the resulting shared pointer.
 */
#if UPCRI_LIBWRAP
  UPCRI_LIBWRAP_FN
  shared void * bupc_local_to_shared(void *_ptr, int _thread, int _phase) {
    return upcri_bless_shared2SVP(
            _bupc_local_to_shared(_ptr, _thread, _phase));
  }
#else
  #define bupc_local_to_shared _bupc_local_to_shared
  shared void * bupc_local_to_shared(void *_ptr, int _thread, int _phase);
#endif

/* Provides support for pointer-to-shared addressing with blocksizes which
 * are not compile-time constant. 
 * - 'p': the base pointer
 * - 'blockelems': the block size (number of elements in a block)
 * - 'elemsz': the element size (usually sizeof(*p))
 * - 'elemincr': the positive or negative offset from the base pointer
 *
 * The following call:
 *     upc_ptradd(p, blockelems, sizeof(T), elemincr);
 * Returns a value q as if it had been computed:
 *     shared [blockelems] T *q = p;
 *     q += elemincr;
 * however, the blockelems argument is not required to be a compile-time constant.
 */
#if UPCRI_LIBWRAP
  UPCRI_LIBWRAP_FN
  shared void * bupc_ptradd(shared void *_p, size_t _blockelems, size_t _elemsz, ptrdiff_t _elemincr) {
    return upcri_bless_shared2SVP(
            _bupc_ptradd(upcri_bless_SVP2shared(_p), _blockelems, _elemsz, _elemincr));
  }
#else
  #define bupc_ptradd _bupc_ptradd
  shared void * bupc_ptradd(shared void *_p, size_t _blockelems, size_t _elemsz, ptrdiff_t _elemincr);
#endif

/* Retrieves value of an environment variable.  This function should be used
 * instead of getenv(), which is not guaranteed to return correct
 * results. 
 *
 * At present this function is only guaranteed to retrieve the same value
 * for all threads if the environment variable's name begins with 'UPC_' or
 * 'GASNET_'.
 *
 * The 'setenv()' and 'unsetenv' functions are not guaranteed to work in a
 * Berkeley UPC runtime environment, and should be avoided.
 */
char *bupc_getenv(const char *_name);
#define getenv(s) bupc_getenv(s)

/* void bupc_trace_printf((const char *msg, ...))
 *  outputs a message into the GASNet trace log, if it exists 
 *  note that two sets of parentheses are required when invoking this operation,
 *  in order to allow it to compile away completely for non-tracing builds.
 *  Ex:   double A[4] = ...; 
 *        int i = ...;
 *        bupc_trace_printf(("the value of A[%i] is: %f", i, A[i]));
 */
#ifdef GASNET_TRACE
  extern void upcri_trace_printf_user(const char *msg, ...);
 #if UPCRI_LIBWRAP
  #define bupc_trace_printf(parenthesized_args) \
     (upcri_trace_printf_user parenthesized_args)
 #else
  extern void upcri_srcpos(void);
  #define bupc_trace_printf(parenthesized_args) \
     (upcri_srcpos(), upcri_trace_printf_user parenthesized_args, ((void)0))
 #endif
#else
  #define bupc_trace_printf(parenthesized_args) ((void)0)
#endif

/* bupc_trace_{set,get}mask and bupc_stats_{set,get}mask
   Allow programmatic retrieval and modification of the trace and stats 
   communication profiling masks in effect for the calling thread.
   The initial values are determined by the GASNET_TRACEMASK and GASNET_STATSMASK 
   environment variables, and the input and output to the mask manipulation
   functions are character strings formatted as for those variables.

   bupc_trace_{get,set}tracelocal
   Allow the calling thread to programmatically enable/disable tracing of 
   local put/get operations, which correspond to pointer-to-shared accesses
   that actually have local affinity (and therefore invoke no network communication). 

   Different UPC threads may set different masks and tracelocal settings, but note 
   that in pthreaded UPC jobs all pthreads in a process share these values.
   These functions have no effect if trace and stats communication profiling are
   disabled at upcr configure time, or are not enabled for the current run.

   Ex: 
     bupc_trace_setmask("PGH");   // trace all puts, gets and UPC-level events
     bupc_trace_settracelocal(1); // include local puts and gets 
     // do something...
     bupc_trace_setmask("");      // stop tracing
 */
#if UPCRI_LIBWRAP
/* handled by upcr.h */
#else
extern void bupc_trace_setmask(const char *newmask);
extern void bupc_stats_setmask(const char *newmask);
extern const char *bupc_trace_getmask(void);
extern const char *bupc_stats_getmask(void);
extern int bupc_trace_gettracelocal(void);
extern void bupc_trace_settracelocal(int val);
#endif

#if UPCRI_LIBWRAP
/* handled by upcr.h */
#else
/* high-precision timer support */
#include <inttypes.h>
typedef uint64_t bupc_tick_t; /* an integral type for holding ticks */
#define BUPC_TICK_MAX ((bupc_tick_t)-1)
#define BUPC_TICK_MIN ((bupc_tick_t)0)
extern bupc_tick_t bupc_ticks_now(void); /* the current tick value */
extern uint64_t bupc_ticks_to_us(bupc_tick_t ticks); /* convert ticks to microsecs */
extern uint64_t bupc_ticks_to_ns(bupc_tick_t ticks); /* convert ticks to nanosecs */
/* estimated microsecond granularity (min time between distinct ticks)
 *    and microsecond overhead (time it takes to read a single tick value) */
extern double bupc_tick_granularityus(void);
extern double bupc_tick_overheadus(void);
#endif

/*---------------------------------------------------------------------------------*/
/* job topology queries */

/* bupc_thread_distance - runtime job layout query for hierarchical systems (eg clusters of SMPs)
 *
 * bupc_thread_distance takes two thread identifiers (whose values must be in
 * 0..THREADS-1, otherwise behavior is undefined), and returns an unsigned integral
 * value which represents an approximation of the abstract 'distance' between
 * the hardware entity which hosts the first thread, and the hardware entity
 * which hosts the memory with affinity to the second thread. In this context
 * 'distance' is intended to provide an approximate and relative measure of
 * expected best-case access time between the two entities in question. Several
 * abstract 'levels' of distance are provided as pre-defined constants for user
 * convenience, which represent monotonically non-decreasing 'distance':
 *
 * BUPC_THREADS_SAME (must be defined to 0)  
 *   implies threadX == threadY
 *
 * BUPC_THREADS_VERYNEAR
 *   implies threadX has the closest possible distance (fastest access) to
 *   threadY's memory without being the same actual thread
 *
 * BUPC_THREADS_NEAR
 *   implies distance not less than BUPC_THREADS_VERYNEAR, 
 *   but not more than BUPC_THREADS_FAR
 * 
 * BUPC_THREADS_FAR
 *   implies distance not less than BUPC_THREADS_NEAR, 
 *   but not more than BUPC_THREADS_VERYFAR
 *
 * BUPC_THREADS_VERYFAR
 *   implies threadX has the farthest possible distance (slowest access) to
 *   threadY's memory
 *
 * These constants have implementation-defined integral values which are
 * monotonically increasing in the order given above. Implementations may add
 * further intermediate level with values between BUPC_THREADS_VERYNEAR 
 * and BUPC_THREADS_VERYFAR (with no corresponding define) to represent
 * deeper hierarchies, so users should test against the constants using >= or
 * <= instead of ==.
 *
 * The intent of the interface is for users to not rely on the physical
 * significance of any particular level and simply test the differences to discover
 * which threads are *relatively* closer than others. Implementations are 
 * encouraged to document the physical significance of the various levels whenever
 * possible, however any code based on assuming exactly N levels of hierarchy
 * or a fixed significance for a particular level will probably not be
 * performance portable to different implementations or machines.
 *
 * The relation is symmettric, ie:
 * bupc_thread_distance(X,Y) == bupc_thread_distance(Y,X)
 *
 * but the relation is *not* transitive: 
 * bupc_thread_distance(X,Y) == A && bupc_thread_distance(Y,Z) == A
 * does *NOT* imply bupc_thread_distance(X,Z) == A
 *
 * Furthermore, the value of bupc_thread_distance(X,Y) is guaranteed to be
 * unchanged over the span of a single program execution, and the same value is
 * returned regardless of the thread invoking the query.
 *
 */

#if UPCRI_LIBWRAP
/* handled by upcr.h */
#else
unsigned int bupc_thread_distance(int threadX, int threadY);
#endif

/*---------------------------------------------------------------------------------*/
/* upc_sem_t is a shared datatype with incomplete type
   Must be manipulated via ptr
 */
struct bupc_sem_S;
typedef shared struct bupc_sem_S bupc_sem_t;

#if !UPCRI_LIBWRAP
/* bupc_sem_alloc - non-collectively allocate a semaphore with affinity to the calling thread,
    initialized to the logical value zero
   flags == 0 selects most general configuraton:
    BUPC_SEM_INTEGER | BUPC_SEM_MPRODUCER | BUPC_SEM_MCONSUMER
*/

bupc_sem_t *bupc_sem_alloc(int _flags);
void bupc_sem_free(bupc_sem_t *_s);

/* Semaphore signalling operations
    bupc_sem_post(N): atomically increment the logical value of semaphore s by 1 (N) 
    bupc_sem_wait(N): wait until the logical value of semaphore s is >= 1 (N), 
       then atomically decrement the value by that amount and return.
       If multiple threads are simultaneously blocked inside wait, (only valid for BUPC_SEM_MCONSUMER)
       then it is undefined the order in which they will be serviced (no fairness guarantees)
    bupc_sem_try(N): attempt to perform a bupc_sem_wait(N). If the operation can 
       succeed immediately, perform it and return 1. Otherwise, return 0.
   all of the following imply a upc_fence 
*/
void bupc_sem_post(bupc_sem_t *_s);
void bupc_sem_postN(bupc_sem_t *_s, size_t _n); /* only valid for INTEGER sems */

void bupc_sem_wait(bupc_sem_t *_s);
void bupc_sem_waitN(bupc_sem_t *_s, size_t _n); /* only valid for INTEGER sems */

int bupc_sem_try(bupc_sem_t *_s);
int bupc_sem_tryN(bupc_sem_t *_s, size_t _n); /* only valid for INTEGER sems */

/* perform a memput and increment _s by _n when it is complete
   requires upc_threadof(_s) == upc_threadof(_dst)
   Both functions MAY return before the transfer is complete at the target,
     the semaphore on the target will be incremented when the transfer is globally complete.
   No explicit notifications or guarantees are provided to the initiator regarding 
     the completion of the transfer at the target (remote completion).
   bupc_memput_signal returns as soon as the source memory is safe to overwrite (ie
   it blocks for *local* completion of the transfer), whereas bupc_memput_signal_async 
   MAY return earlier, while the source memory is still in use (and therefore not 
   safe to overwrite). Callers of bupc_memput_signal_async are responsible for 
   enforcing their own synchronization from the target to the initiatior to decide when
   the source memory is safe to overwrite.
 */
void bupc_memput_signal(shared void *_dst, const void *_src, size_t _nbytes, bupc_sem_t *_s, size_t _n);
void bupc_memput_signal_async(shared void *_dst, const void *_src, size_t _nbytes, bupc_sem_t *_s, size_t _n);

#else  /* UPCRI_LIBWRAP */

  UPCRI_LIBWRAP_FN
  bupc_sem_t *bupc_sem_alloc(int _flags) { 
    return upcri_bless_pshared2SVP(_bupc_sem_alloc(_flags));
  }
  UPCRI_LIBWRAP_FN
  void bupc_sem_free(bupc_sem_t *_s) {
    _bupc_sem_free(upcri_bless_SVP2pshared(_s));
  }
  #define UPCRI_LIBWRAP_SEM(name) \
  UPCRI_LIBWRAP_FN void bupc_sem_##name(bupc_sem_t *_s) { _bupc_sem_##name(upcri_bless_SVP2pshared(_s)); } \
  UPCRI_LIBWRAP_FN void bupc_sem_##name##N(bupc_sem_t *_s, size_t _n) { _bupc_sem_##name##N(upcri_bless_SVP2pshared(_s),_n); } 
  UPCRI_LIBWRAP_SEM(post)
  UPCRI_LIBWRAP_SEM(wait)
  UPCRI_LIBWRAP_FN int bupc_sem_try(bupc_sem_t *_s) { return _bupc_sem_try(upcri_bless_SVP2pshared(_s)); }
  UPCRI_LIBWRAP_FN int bupc_sem_tryN(bupc_sem_t *_s, size_t _n) { return _bupc_sem_tryN(upcri_bless_SVP2pshared(_s),_n); } 

  #define UPCRI_LIBWRAP_SEMPUT(name) \
  UPCRI_LIBWRAP_FN void bupc_##name(shared void *_dst, const void *_src, size_t _nbytes, bupc_sem_t *_s, size_t _n) { \
    _bupc_##name(upcri_bless_SVP2shared(_dst),_src,_nbytes,upcri_bless_SVP2pshared(_s),_n); \
  }
  UPCRI_LIBWRAP_SEMPUT(memput_signal)
  UPCRI_LIBWRAP_SEMPUT(memput_signal_async)

#endif

/*---------------------------------------------------------------------------------*/
/* Non-blocking memcpy extensions, a Berkeley UPC extension - 
 * see: Bonachea, D. "Proposal for Extending the UPC Memory Copy Library Functions"
 *  available at http://upc.lbl.gov/publications/
 */
#if !UPCRI_LIBWRAP
  struct _upcri_eop;
  typedef struct _upcri_eop *bupc_handle_t;
  #define BUPC_COMPLETE_HANDLE ((bupc_handle_t)NULL)

  bupc_handle_t bupc_memcpy_async(shared void *_dst, shared const void *_src, size_t _n);
  bupc_handle_t bupc_memget_async(       void *_dst, shared const void *_src, size_t _n);
  bupc_handle_t bupc_memput_async(shared void *_dst,        const void *_src, size_t _n);
  bupc_handle_t bupc_memset_async(shared void *_dst, int _c, size_t _n);

  void bupc_waitsync(bupc_handle_t h);
  int bupc_trysync(bupc_handle_t h);

  void bupc_waitsync_all (bupc_handle_t *_ph, size_t _numhandles);
  int  bupc_trysync_all  (bupc_handle_t *_ph, size_t _numhandles);
  void bupc_waitsync_some(bupc_handle_t *_ph, size_t _numhandles);
  int  bupc_trysync_some (bupc_handle_t *_ph, size_t _numhandles);

  /* implicit-handle variants */
  void bupc_memcpy_asynci(shared void *_dst, shared const void *_src, size_t _n);
  void bupc_memget_asynci(       void *_dst, shared const void *_src, size_t _n);
  void bupc_memput_asynci(shared void *_dst,        const void *_src, size_t _n);
  void bupc_memset_asynci(shared void *_dst, int _c, size_t _n);

  void bupc_waitsynci(void);
  int bupc_trysynci(void);

  void          bupc_begin_accessregion(void);
  bupc_handle_t bupc_end_accessregion(void);

#else /* UPCRI_LIBWRAP */
  UPCRI_LIBWRAP_FN
  bupc_handle_t bupc_memcpy_async(shared void *_dst, shared const void *_src, size_t _n) {
    return _bupc_memcpy_async(upcri_bless_SVP2shared(_dst), upcri_bless_SVP2shared(_src), _n);
  }
  UPCRI_LIBWRAP_FN
  bupc_handle_t bupc_memget_async(       void *_dst, shared const void *_src, size_t _n) {
    return _bupc_memget_async(_dst, upcri_bless_SVP2shared(_src), _n);
  }
  UPCRI_LIBWRAP_FN
  bupc_handle_t bupc_memput_async(shared void *_dst,        const void *_src, size_t _n) {
    return _bupc_memput_async(upcri_bless_SVP2shared(_dst), _src, _n);
  }
  UPCRI_LIBWRAP_FN
  bupc_handle_t bupc_memset_async(shared void *_dst, int _c, size_t _n) {
    return _bupc_memset_async(upcri_bless_SVP2shared(_dst), _c, _n);
  }

  #define bupc_waitsync _bupc_waitsync
  #define bupc_trysync  _bupc_trysync

  #define bupc_waitsync_all  _bupc_waitsync_all
  #define bupc_trysync_all   _bupc_trysync_all
  #define bupc_waitsync_some _bupc_waitsync_some
  #define bupc_trysync_some  _bupc_trysync_some

  UPCRI_LIBWRAP_FN
  void bupc_memcpy_asynci(shared void *_dst, shared const void *_src, size_t _n) {
    _bupc_memcpy_asynci(upcri_bless_SVP2shared(_dst), upcri_bless_SVP2shared(_src), _n);
  }
  UPCRI_LIBWRAP_FN
  void bupc_memget_asynci(       void *_dst, shared const void *_src, size_t _n) {
    _bupc_memget_asynci(_dst, upcri_bless_SVP2shared(_src), _n);
  }
  UPCRI_LIBWRAP_FN
  void bupc_memput_asynci(shared void *_dst,        const void *_src, size_t _n) {
    _bupc_memput_asynci(upcri_bless_SVP2shared(_dst), _src, _n);
  }
  UPCRI_LIBWRAP_FN
  void bupc_memset_asynci(shared void *_dst, int _c, size_t _n) {
    _bupc_memset_asynci(upcri_bless_SVP2shared(_dst), _c, _n);
  }

  #define bupc_waitsynci _bupc_waitsynci
  #define bupc_trysynci  _bupc_trysynci

  #define bupc_begin_accessregion   _bupc_begin_accessregion
  #define bupc_end_accessregion     _bupc_end_accessregion
#endif

#if !UPCRI_LIBWRAP
/*---------------------------------------------------------------------------------*/
#ifndef BUPC_SG_DESIGN_A
#define BUPC_SG_DESIGN_A      1
#endif
#ifndef BUPC_SG_DESIGN_B
#define BUPC_SG_DESIGN_B      1
#endif
#ifndef BUPC_STRIDED_DESIGN_A
#define BUPC_STRIDED_DESIGN_A 1
#endif
#ifndef BUPC_STRIDED_DESIGN_B
#define BUPC_STRIDED_DESIGN_B 1
#endif

#if BUPC_SG_DESIGN_A
  typedef struct bupc_pmemvec_S_trans {
    void *addr;
    size_t len;
  } bupc_pmemvec_t;

  typedef struct bupc_smemvec_S_trans {
    shared void *addr;
    size_t len;
  } bupc_smemvec_t;

  void bupc_memcpy_vlist(size_t _dstcount, bupc_smemvec_t const _dstlist[], 
                         size_t _srccount, bupc_smemvec_t const _srclist[]);
  void bupc_memput_vlist(size_t _dstcount, bupc_smemvec_t const _dstlist[], 
                         size_t _srccount, bupc_pmemvec_t const _srclist[]);
  void bupc_memget_vlist(size_t _dstcount, bupc_pmemvec_t const _dstlist[], 
                         size_t _srccount, bupc_smemvec_t const _srclist[]);

  bupc_handle_t bupc_memcpy_vlist_async(size_t _dstcount, bupc_smemvec_t const _dstlist[], 
                                        size_t _srccount, bupc_smemvec_t const _srclist[]);
  bupc_handle_t bupc_memput_vlist_async(size_t _dstcount, bupc_smemvec_t const _dstlist[], 
                                        size_t _srccount, bupc_pmemvec_t const _srclist[]);
  bupc_handle_t bupc_memget_vlist_async(size_t _dstcount, bupc_pmemvec_t const _dstlist[], 
                                        size_t _srccount, bupc_smemvec_t const _srclist[]);

#endif

#if BUPC_SG_DESIGN_B
  void bupc_memcpy_ilist(size_t _dstcount, shared       void * const _dstlist[], size_t _dstlen,
                         size_t _srccount, shared const void * const _srclist[], size_t _srclen);
  void bupc_memput_ilist(size_t _dstcount, shared       void * const _dstlist[], size_t _dstlen,
                         size_t _srccount,        const void * const _srclist[], size_t _srclen);
  void bupc_memget_ilist(size_t _dstcount,              void * const _dstlist[], size_t _dstlen, 
                         size_t _srccount, shared const void * const _srclist[], size_t _srclen);

  bupc_handle_t bupc_memcpy_ilist_async(size_t _dstcount, shared       void * const _dstlist[], 
                                        size_t _dstlen,
                                        size_t _srccount, shared const void * const _srclist[], 
                                        size_t _srclen);
  bupc_handle_t bupc_memput_ilist_async(size_t _dstcount, shared       void * const _dstlist[], 
                                        size_t _dstlen,
                                        size_t _srccount,        const void * const _srclist[], 
                                        size_t _srclen);
  bupc_handle_t bupc_memget_ilist_async(size_t _dstcount,              void * const _dstlist[], 
                                        size_t _dstlen, 
                                        size_t _srccount, shared const void * const _srclist[], 
                                        size_t _srclen);
#endif
/*---------------------------------------------------------------------------------*/
#if BUPC_STRIDED_DESIGN_A
  void bupc_memcpy_fstrided(shared void *_dstaddr,  size_t _dstchunklen, 
                            size_t _dstchunkstride, size_t _dstchunkcount,
                            shared void *_srcaddr,  size_t _srcchunklen, 
                            size_t _srcchunkstride, size_t _srcchunkcount);
  void bupc_memput_fstrided(shared void *_dstaddr,  size_t _dstchunklen, 
                            size_t _dstchunkstride, size_t _dstchunkcount,
                                   void *_srcaddr,  size_t _srcchunklen, 
                            size_t _srcchunkstride, size_t _srcchunkcount);
  void bupc_memget_fstrided(       void *_dstaddr,  size_t _dstchunklen, 
                            size_t _dstchunkstride, size_t _dstchunkcount,
                            shared void *_srcaddr,  size_t _srcchunklen, 
                            size_t _srcchunkstride, size_t _srcchunkcount);

  bupc_handle_t bupc_memcpy_fstrided_async(shared void *_dstaddr,  size_t _dstchunklen, 
                                          size_t _dstchunkstride, size_t _dstchunkcount,
                                          shared void *_srcaddr,  size_t _srcchunklen, 
                                          size_t _srcchunkstride, size_t _srcchunkcount);
  bupc_handle_t bupc_memput_fstrided_async(shared void *_dstaddr,  size_t _dstchunklen, 
                                          size_t _dstchunkstride, size_t _dstchunkcount,
                                                 void *_srcaddr,  size_t _srcchunklen, 
                                          size_t _srcchunkstride, size_t _srcchunkcount);
  bupc_handle_t bupc_memget_fstrided_async(       void *_dstaddr,  size_t _dstchunklen, 
                                          size_t _dstchunkstride, size_t _dstchunkcount,
                                          shared void *_srcaddr,  size_t _srcchunklen, 
                                          size_t _srcchunkstride, size_t _srcchunkcount);
#endif

#if BUPC_STRIDED_DESIGN_B
  void bupc_memcpy_strided(shared       void *_dstaddr, const size_t _dststrides[], 
                           shared const void *_srcaddr, const size_t _srcstrides[], 
                           const size_t _count[], size_t _stridelevels);
  void bupc_memput_strided(shared       void *_dstaddr, const size_t _dststrides[], 
                                  const void *_srcaddr, const size_t _srcstrides[], 
                           const size_t _count[], size_t _stridelevels);
  void bupc_memget_strided(             void *_dstaddr, const size_t _dststrides[], 
                           shared const void *_srcaddr, const size_t _srcstrides[], 
                           const size_t _count[], size_t _stridelevels);

  bupc_handle_t bupc_memcpy_strided_async(shared       void *_dstaddr, const size_t _dststrides[], 
                                          shared const void *_srcaddr, const size_t _srcstrides[], 
                                          const size_t _count[], size_t _stridelevels);
  bupc_handle_t bupc_memput_strided_async(shared       void *_dstaddr, const size_t _dststrides[], 
                                                 const void *_srcaddr, const size_t _srcstrides[], 
                                          const size_t _count[], size_t _stridelevels);
  bupc_handle_t bupc_memget_strided_async(             void *_dstaddr, const size_t _dststrides[], 
                                          shared const void *_srcaddr, const size_t _srcstrides[], 
                                                 const size_t _count[], size_t _stridelevels);
#endif
#endif

/*---------------------------------------------------------------------------------*/
/* bupc_atomic extensions */

#if UPCRI_LIBWRAP
  #define _BUPC_ATOMIC_DECL_FETCHOP(_type, _code, _suff, _op)                                   \
	UPCRI_LIBWRAP_FN                                                                        \
	_type bupc_atomic##_code##_fetch##_op##_##_suff(shared void *_dst, _type _val) {        \
	  return _bupc_atomic##_code##_fetch##_op##_##_suff(upcri_bless_SVP2shared(_dst), _val);\
	}
  #define _BUPC_ATOMIC_DECL2(_type, _code, _suff)                                  \
	UPCRI_LIBWRAP_FN                                                           \
	_type bupc_atomic##_code##_read_##_suff(shared void *_src) {               \
	  return _bupc_atomic##_code##_read_##_suff(upcri_bless_SVP2shared(_src)); \
	}                                                                          \
	UPCRI_LIBWRAP_FN                                                           \
	void bupc_atomic##_code##_set_##_suff(shared void *_dst, _type _val) {     \
	  _bupc_atomic##_code##_set_##_suff(upcri_bless_SVP2shared(_dst), _val);   \
	}                                                                          \
	UPCRI_LIBWRAP_FN                                                           \
	_type bupc_atomic##_code##_swap_##_suff(shared void *_dst, _type _val) {   \
	  return _bupc_atomic##_code##_swap_##_suff(upcri_bless_SVP2shared(_dst), _val);\
	}                                                                          \
	UPCRI_LIBWRAP_FN                                                           \
	_type bupc_atomic##_code##_fetchnot_##_suff(shared void *_dst) {           \
	  return _bupc_atomic##_code##_fetchnot_##_suff(upcri_bless_SVP2shared(_dst));\
	}                                                                          \
	UPCRI_LIBWRAP_FN                                                           \
	_type bupc_atomic##_code##_cswap_##_suff(shared void *_dst, _type _oldval, _type _newval) {\
	  return _bupc_atomic##_code##_cswap_##_suff(upcri_bless_SVP2shared(_dst), _oldval, _newval);\
	}                                                                          \
	_BUPC_ATOMIC_DECL_FETCHOP(_type, _code, _suff, add)                        \
	_BUPC_ATOMIC_DECL_FETCHOP(_type, _code, _suff, and)                        \
	_BUPC_ATOMIC_DECL_FETCHOP(_type, _code, _suff, or )                        \
	_BUPC_ATOMIC_DECL_FETCHOP(_type, _code, _suff, xor)
#else
  #define _BUPC_ATOMIC_DECL_FETCHOP(_type, _code, _suff, _op) \
	_type bupc_atomic##_code##_fetch##_op##_##_suff(shared void *_dst, _type _val);
  #define _BUPC_ATOMIC_DECL2(_type, _code, _suff)                                  \
	_type bupc_atomic##_code##_read_##_suff(shared void *_src);                \
	void bupc_atomic##_code##_set_##_suff(shared void *_dst, _type _val);      \
	_type bupc_atomic##_code##_swap_##_suff(shared void *_dst, _type _val);    \
	_type bupc_atomic##_code##_fetchnot_##_suff(shared void *_dst);            \
	_type bupc_atomic##_code##_cswap_##_suff(shared void *_dst, _type _oldval, _type _newval);\
	_BUPC_ATOMIC_DECL_FETCHOP(_type, _code, _suff, add)                        \
	_BUPC_ATOMIC_DECL_FETCHOP(_type, _code, _suff, and)                        \
	_BUPC_ATOMIC_DECL_FETCHOP(_type, _code, _suff, or )                        \
	_BUPC_ATOMIC_DECL_FETCHOP(_type, _code, _suff, xor)
#endif

#define _BUPC_ATOMIC_DECL(_type, _code) \
	_BUPC_ATOMIC_DECL2(_type, _code, relaxed) \
	_BUPC_ATOMIC_DECL2(_type, _code, strict)

_BUPC_ATOMIC_DECL(uint64_t, U64)
_BUPC_ATOMIC_DECL(int64_t,  I64)
_BUPC_ATOMIC_DECL(uint32_t, U32)
_BUPC_ATOMIC_DECL(int32_t,  I32)

/* Don't forget to add new types/ops to the BUPC_USE_UPC_NAMESPACE defines */

/*---------------------------------------------------------------------------------*/
/* allow user to easily enable use of the "upc_" namespace for Berkeley extensions */
#ifdef BUPC_USE_UPC_NAMESPACE
  #define upc_thread_distance     bupc_thread_distance
  #define UPC_THREADS_SAME        BUPC_THREADS_SAME     
  #define UPC_THREADS_VERYNEAR    BUPC_THREADS_VERYNEAR 
  #define UPC_THREADS_NEAR        BUPC_THREADS_NEAR     
  #define UPC_THREADS_FAR         BUPC_THREADS_FAR      
  #define UPC_THREADS_VERYFAR     BUPC_THREADS_VERYFAR  

  #define upc_sem_t               bupc_sem_t          
  #define upc_sem_alloc           bupc_sem_alloc      
  #define upc_sem_free            bupc_sem_free       
  #define upc_sem_post            bupc_sem_post       
  #define upc_sem_postN           bupc_sem_postN      
  #define upc_sem_wait            bupc_sem_wait       
  #define upc_sem_waitN           bupc_sem_waitN      
  #define upc_sem_try             bupc_sem_try        
  #define upc_sem_tryN            bupc_sem_tryN       
  #define upc_memput_signal       bupc_memput_signal        
  #define upc_memput_signal_async bupc_memput_signal_async  

  #define UPC_SEM_BOOLEAN   BUPC_SEM_BOOLEAN   
  #define UPC_SEM_INTEGER   BUPC_SEM_INTEGER   
  #define UPC_SEM_SPRODUCER BUPC_SEM_SPRODUCER 
  #define UPC_SEM_MPRODUCER BUPC_SEM_MPRODUCER 
  #define UPC_SEM_SCONSUMER BUPC_SEM_SCONSUMER 
  #define UPC_SEM_MCONSUMER BUPC_SEM_MCONSUMER 
  #define UPC_SEM_MAXVALUE  BUPC_SEM_MAXVALUE

  #define upc_handle_t        bupc_handle_t
  #define UPC_COMPLETE_HANDLE BUPC_COMPLETE_HANDLE

  #define upc_waitsync        bupc_waitsync
  #define upc_trysync         bupc_trysync
  #define upc_waitsync_all    bupc_waitsync_all
  #define upc_trysync_all     bupc_trysync_all
  #define upc_waitsync_some   bupc_waitsync_some
  #define upc_trysync_some    bupc_trysync_some

  #define upc_memcpy_async    bupc_memcpy_async
  #define upc_memget_async    bupc_memget_async
  #define upc_memput_async    bupc_memput_async
  #define upc_memset_async    bupc_memset_async

  #define upc_waitsynci     bupc_waitsynci
  #define upc_trysynci      bupc_trysynci
  #define upc_memget_asynci bupc_memget_asynci
  #define upc_memput_asynci bupc_memput_asynci
  #define upc_memcpy_asynci bupc_memcpy_asynci
  #define upc_memset_asynci bupc_memset_asynci

  #define upc_begin_accessregion bupc_begin_accessregion
  #define upc_end_accessregion   bupc_end_accessregion

  #if BUPC_SG_DESIGN_A
    #define upc_pmemvec_t       bupc_pmemvec_t
    #define upc_smemvec_t       bupc_smemvec_t

    #define upc_memcpy_vlist         bupc_memcpy_vlist
    #define upc_memput_vlist         bupc_memput_vlist
    #define upc_memget_vlist         bupc_memget_vlist
    #define upc_memcpy_vlist_async   bupc_memcpy_vlist_async
    #define upc_memput_vlist_async   bupc_memput_vlist_async
    #define upc_memget_vlist_async   bupc_memget_vlist_async
  #endif

  #if BUPC_SG_DESIGN_B
    #define upc_memcpy_ilist         bupc_memcpy_ilist
    #define upc_memput_ilist         bupc_memput_ilist
    #define upc_memget_ilist         bupc_memget_ilist
    #define upc_memcpy_ilist_async   bupc_memcpy_ilist_async
    #define upc_memput_ilist_async   bupc_memput_ilist_async
    #define upc_memget_ilist_async   bupc_memget_ilist_async
  #endif

  #if BUPC_STRIDED_DESIGN_A
    #define upc_memcpy_fstrided        bupc_memcpy_fstrided
    #define upc_memput_fstrided        bupc_memput_fstrided
    #define upc_memget_fstrided        bupc_memget_fstrided
    #define upc_memcpy_fstrided_async  bupc_memcpy_fstrided_async
    #define upc_memput_fstrided_async  bupc_memput_fstrided_async
    #define upc_memget_fstrided_async  bupc_memget_fstrided_async
  #endif

  #if BUPC_STRIDED_DESIGN_B
    #define upc_memcpy_strided        bupc_memcpy_strided
    #define upc_memput_strided        bupc_memput_strided
    #define upc_memget_strided        bupc_memget_strided
    #define upc_memcpy_strided_async  bupc_memcpy_strided_async
    #define upc_memput_strided_async  bupc_memput_strided_async
    #define upc_memget_strided_async  bupc_memget_strided_async
  #endif

  #define upc_atomicU64_read_relaxed        bupc_atomicU64_read_relaxed
  #define upc_atomicU64_read_strict         bupc_atomicU64_read_strict
  #define upc_atomicU64_set_relaxed         bupc_atomicU64_set_relaxed
  #define upc_atomicU64_set_strict          bupc_atomicU64_set_strict
  #define upc_atomicU64_fetchadd_relaxed    bupc_atomicU64_fetchadd_relaxed
  #define upc_atomicU64_fetchadd_strict     bupc_atomicU64_fetchadd_strict
  #define upc_atomicU64_fetchand_relaxed    bupc_atomicU64_fetchand_relaxed
  #define upc_atomicU64_fetchand_strict     bupc_atomicU64_fetchand_strict
  #define upc_atomicU64_fetchor_relaxed     bupc_atomicU64_fetchor_relaxed
  #define upc_atomicU64_fetchor_strict      bupc_atomicU64_fetchor_strict
  #define upc_atomicU64_fetchxor_relaxed    bupc_atomicU64_fetchxor_relaxed
  #define upc_atomicU64_fetchxor_strict     bupc_atomicU64_fetchxor_strict
  #define upc_atomicU64_fetchnot_relaxed    bupc_atomicU64_fetchnot_relaxed
  #define upc_atomicU64_fetchnot_strict     bupc_atomicU64_fetchnot_strict
  #define upc_atomicU64_swap_relaxed        bupc_atomicU64_swap_relaxed
  #define upc_atomicU64_swap_strict         bupc_atomicU64_swap_strict
  #define upc_atomicU64_cswap_relaxed       bupc_atomicU64_cswap_relaxed
  #define upc_atomicU64_cswap_strict        bupc_atomicU64_cswap_strict

  #define upc_atomicI64_read_relaxed        bupc_atomicI64_read_relaxed
  #define upc_atomicI64_read_strict         bupc_atomicI64_read_strict
  #define upc_atomicI64_set_relaxed         bupc_atomicI64_set_relaxed
  #define upc_atomicI64_set_strict          bupc_atomicI64_set_strict
  #define upc_atomicI64_fetchadd_relaxed    bupc_atomicI64_fetchadd_relaxed
  #define upc_atomicI64_fetchadd_strict     bupc_atomicI64_fetchadd_strict
  #define upc_atomicI64_fetchand_relaxed    bupc_atomicI64_fetchand_relaxed
  #define upc_atomicI64_fetchand_strict     bupc_atomicI64_fetchand_strict
  #define upc_atomicI64_fetchor_relaxed     bupc_atomicI64_fetchor_relaxed
  #define upc_atomicI64_fetchor_strict      bupc_atomicI64_fetchor_strict
  #define upc_atomicI64_fetchxor_relaxed    bupc_atomicI64_fetchxor_relaxed
  #define upc_atomicI64_fetchxor_strict     bupc_atomicI64_fetchxor_strict
  #define upc_atomicI64_fetchnot_relaxed    bupc_atomicI64_fetchnot_relaxed
  #define upc_atomicI64_fetchnot_strict     bupc_atomicI64_fetchnot_strict
  #define upc_atomicI64_swap_relaxed        bupc_atomicI64_swap_relaxed
  #define upc_atomicI64_swap_strict         bupc_atomicI64_swap_strict
  #define upc_atomicI64_cswap_relaxed       bupc_atomicI64_cswap_relaxed
  #define upc_atomicI64_cswap_strict        bupc_atomicI64_cswap_strict

  #define upc_atomicU32_read_relaxed        bupc_atomicU32_read_relaxed
  #define upc_atomicU32_read_strict         bupc_atomicU32_read_strict
  #define upc_atomicU32_set_relaxed         bupc_atomicU32_set_relaxed
  #define upc_atomicU32_set_strict          bupc_atomicU32_set_strict
  #define upc_atomicU32_fetchadd_relaxed    bupc_atomicU32_fetchadd_relaxed
  #define upc_atomicU32_fetchadd_strict     bupc_atomicU32_fetchadd_strict
  #define upc_atomicU32_fetchand_relaxed    bupc_atomicU32_fetchand_relaxed
  #define upc_atomicU32_fetchand_strict     bupc_atomicU32_fetchand_strict
  #define upc_atomicU32_fetchor_relaxed     bupc_atomicU32_fetchor_relaxed
  #define upc_atomicU32_fetchor_strict      bupc_atomicU32_fetchor_strict
  #define upc_atomicU32_fetchxor_relaxed    bupc_atomicU32_fetchxor_relaxed
  #define upc_atomicU32_fetchxor_strict     bupc_atomicU32_fetchxor_strict
  #define upc_atomicU32_fetchnot_relaxed    bupc_atomicU32_fetchnot_relaxed
  #define upc_atomicU32_fetchnot_strict     bupc_atomicU32_fetchnot_strict
  #define upc_atomicU32_swap_relaxed        bupc_atomicU32_swap_relaxed
  #define upc_atomicU32_swap_strict         bupc_atomicU32_swap_strict
  #define upc_atomicU32_cswap_relaxed       bupc_atomicU32_cswap_relaxed
  #define upc_atomicU32_cswap_strict        bupc_atomicU32_cswap_strict

  #define upc_atomicI32_read_relaxed        bupc_atomicI32_read_relaxed
  #define upc_atomicI32_read_strict         bupc_atomicI32_read_strict
  #define upc_atomicI32_set_relaxed         bupc_atomicI32_set_relaxed
  #define upc_atomicI32_set_strict          bupc_atomicI32_set_strict
  #define upc_atomicI32_fetchadd_relaxed    bupc_atomicI32_fetchadd_relaxed
  #define upc_atomicI32_fetchadd_strict     bupc_atomicI32_fetchadd_strict
  #define upc_atomicI32_fetchand_relaxed    bupc_atomicI32_fetchand_relaxed
  #define upc_atomicI32_fetchand_strict     bupc_atomicI32_fetchand_strict
  #define upc_atomicI32_fetchor_relaxed     bupc_atomicI32_fetchor_relaxed
  #define upc_atomicI32_fetchor_strict      bupc_atomicI32_fetchor_strict
  #define upc_atomicI32_fetchxor_relaxed    bupc_atomicI32_fetchxor_relaxed
  #define upc_atomicI32_fetchxor_strict     bupc_atomicI32_fetchxor_strict
  #define upc_atomicI32_fetchnot_relaxed    bupc_atomicI32_fetchnot_relaxed
  #define upc_atomicI32_fetchnot_strict     bupc_atomicI32_fetchnot_strict
  #define upc_atomicI32_swap_relaxed        bupc_atomicI32_swap_relaxed
  #define upc_atomicI32_swap_strict         bupc_atomicI32_swap_strict
  #define upc_atomicI32_cswap_relaxed       bupc_atomicI32_cswap_relaxed
  #define upc_atomicI32_cswap_strict        bupc_atomicI32_cswap_strict
#endif

/*---------------------------------------------------------------------------------*/

#endif 

