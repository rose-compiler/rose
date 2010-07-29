/*  
 * A common layer for both gomp and omni runtime library
 *  Liao 1/20/2009
 *  */
#ifndef LIB_XOMP_H 
#define LIB_XOMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

//enum omp_rtl_enum {
//  e_gomp,
//  e_omni,
//  e_last_rtl
//};
//
//extern omp_rtl_enum rtl_type;

//Runtime library initialization routine
extern void XOMP_init (int argc, char ** argv);

// Runtime library termination routine
extern void XOMP_terminate (int exitcode);

extern void XOMP_parallel_start (void (*func) (void *), void *data, unsigned numThread);
extern void XOMP_parallel_end (void);

extern void XOMP_task (void (*) (void *), void *, void (*) (void *, void *),
                       long, long, bool, unsigned);
extern void XOMP_taskwait (void);

// scheduler functions, union of runtime libary functions
// empty body if not used by one
// scheduler initialization, only meaningful used for OMNI
extern void XOMP_loop_static_init(int lower, int upper, int stride, int chunk_size);
extern void XOMP_loop_dynamic_init(int lower, int upper, int stride, int chunk_size);
extern void XOMP_loop_guided_init(int lower, int upper, int stride, int chunk_size);
extern void XOMP_loop_runtime_init(int lower, int upper, int stride);

//ordered case
extern void XOMP_loop_ordered_static_init(int lower, int upper, int stride, int chunk_size);
extern void XOMP_loop_ordered_dynamic_init(int lower, int upper, int stride, int chunk_size);
extern void XOMP_loop_ordered_guided_init(int lower, int upper, int stride, int chunk_size);
extern void XOMP_loop_ordered_runtime_init(int lower, int upper, int stride);


// if (start), 
// mostly used because of gomp, omni will just call  XOMP_loop_xxx_next();
extern bool XOMP_loop_static_start (long, long, long, long, long *, long *);
extern bool XOMP_loop_dynamic_start (long, long, long, long, long *, long *);
extern bool XOMP_loop_guided_start (long, long, long, long, long *, long *);
extern bool XOMP_loop_runtime_start (long, long, long, long *, long *);

extern bool XOMP_loop_ordered_static_start (long, long, long, long, long *, long *);
extern bool XOMP_loop_ordered_dynamic_start (long, long, long, long, long *, long *);
extern bool XOMP_loop_ordered_guided_start (long, long, long, long, long *, long *);
extern bool XOMP_loop_ordered_runtime_start (long, long, long, long *, long *);

// next
extern bool XOMP_loop_static_next (long *, long *);
extern bool XOMP_loop_dynamic_next (long *, long *);
extern bool XOMP_loop_guided_next (long *, long *);
extern bool XOMP_loop_runtime_next (long *, long *);

extern bool XOMP_loop_ordered_static_next (long *, long *);
extern bool XOMP_loop_ordered_dynamic_next (long *, long *);
extern bool XOMP_loop_ordered_guided_next (long *, long *);
extern bool XOMP_loop_ordered_runtime_next (long *, long *);

//--------------end of  loop functions 

extern void XOMP_barrier (void);
extern void XOMP_critical_start (void** data); 
extern void XOMP_critical_end (void** data);
extern bool XOMP_single(void);
extern bool XOMP_master(void);

extern void XOMP_atomic_start (void);
extern void XOMP_atomic_end (void);

extern void XOMP_loop_end (void);
extern void XOMP_loop_end_nowait (void);
   // --- end loop functions ---
// flush without variable list
extern void XOMP_flush_all ();
// omp flush with variable list, flush one by one, given each's start address and size
extern void XOMP_flush_one (char * startAddress, int nbyte);


// omp ordered directive
extern void XOMP_ordered_start (void);
extern void XOMP_ordered_end (void);

#ifdef __cplusplus
 }
#endif
 
#endif /* LIB_XOMP_H */



 
