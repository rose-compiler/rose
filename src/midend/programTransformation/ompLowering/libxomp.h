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


extern void XOMP_barrier (void);
extern void XOMP_critical_start (void** data); 
extern void XOMP_critical_end (void** data);
extern bool XOMP_single(void);
extern bool XOMP_master(void);

/*
extern void GOMP_barrier (void);

extern void GOMP_critical_name_start (void **);
extern void GOMP_critical_name_end (void **);
extern void GOMP_atomic_start (void);
extern void GOMP_atomic_end (void);

extern bool GOMP_loop_static_start (long, long, long, long, long *, long *);
extern bool GOMP_loop_dynamic_start (long, long, long, long, long *, long *);
extern bool GOMP_loop_guided_start (long, long, long, long, long *, long *);
extern bool GOMP_loop_runtime_start (long, long, long, long *, long *);

extern bool GOMP_loop_ordered_static_start (long, long, long, long, long *, long *);
extern bool GOMP_loop_ordered_dynamic_start (long, long, long, long, long *, long *);
extern bool GOMP_loop_ordered_guided_start (long, long, long, long, long *, long *);
extern bool GOMP_loop_ordered_runtime_start (long, long, long, long *, long *);

extern bool GOMP_loop_static_next (long *, long *);
extern bool GOMP_loop_dynamic_next (long *, long *);
extern bool GOMP_loop_guided_next (long *, long *);
extern bool GOMP_loop_runtime_next (long *, long *);

extern bool GOMP_loop_ordered_static_next (long *, long *);
extern bool GOMP_loop_ordered_dynamic_next (long *, long *);
extern bool GOMP_loop_ordered_guided_next (long *, long *);
extern bool GOMP_loop_ordered_runtime_next (long *, long *);

extern void GOMP_parallel_loop_static_start (void (*)(void *), void *,
                                             unsigned, long, long, long, long);
extern void GOMP_parallel_loop_dynamic_start (void (*)(void *), void *,
                                             unsigned, long, long, long, long);
extern void GOMP_parallel_loop_guided_start (void (*)(void *), void *,
                                             unsigned, long, long, long, long);
extern void GOMP_parallel_loop_runtime_start (void (*)(void *), void *,
                                              unsigned, long, long, long);
extern void GOMP_loop_end (void);
extern void GOMP_loop_end_nowait (void);

extern bool GOMP_loop_ull_static_start (bool, unsigned long long,
                                        unsigned long long,
                                        unsigned long long,
                                        unsigned long long,
                                        unsigned long long *,
                                        unsigned long long *);
extern bool GOMP_loop_ull_dynamic_start (bool, unsigned long long,
                                         unsigned long long,
                                         unsigned long long,
                                         unsigned long long,
                                         unsigned long long *,
                                         unsigned long long *);
extern bool GOMP_loop_ull_guided_start (bool, unsigned long long,
                                        unsigned long long,
                                        unsigned long long,
                                        unsigned long long,
                                        unsigned long long *,
                                        unsigned long long *);
extern bool GOMP_loop_ull_runtime_start (bool, unsigned long long,
                                         unsigned long long,
                                         unsigned long long,
                                         unsigned long long *,
                                         unsigned long long *);

extern bool GOMP_loop_ull_ordered_static_start (bool, unsigned long long,
                                                unsigned long long,
                                                unsigned long long,
                                                unsigned long long,
                                                unsigned long long *,
                                                unsigned long long *);
extern bool GOMP_loop_ull_ordered_dynamic_start (bool, unsigned long long,
                                                 unsigned long long,
                                                 unsigned long long,
                                                 unsigned long long,
                                                 unsigned long long *,
                                                 unsigned long long *);
extern bool GOMP_loop_ull_ordered_guided_start (bool, unsigned long long,
                                                unsigned long long,
                                                unsigned long long,
                                                unsigned long long,
                                                unsigned long long *,
                                                unsigned long long *);
extern bool GOMP_loop_ull_ordered_runtime_start (bool, unsigned long long,
                                                 unsigned long long,
                                                 unsigned long long,
                                                 unsigned long long *,
                                                 unsigned long long *);

extern bool GOMP_loop_ull_static_next (unsigned long long *,
                                       unsigned long long *);
extern bool GOMP_loop_ull_dynamic_next (unsigned long long *,
                                        unsigned long long *);
extern bool GOMP_loop_ull_guided_next (unsigned long long *,
                                       unsigned long long *);
extern bool GOMP_loop_ull_runtime_next (unsigned long long *,
                                        unsigned long long *);

extern bool GOMP_loop_ull_ordered_static_next (unsigned long long *,
                                               unsigned long long *);
extern bool GOMP_loop_ull_ordered_dynamic_next (unsigned long long *,
                                                unsigned long long *);

extern void GOMP_ordered_start (void);
extern void GOMP_ordered_end (void);

extern void GOMP_parallel_start (void (*) (void *), void *, unsigned);
extern void GOMP_parallel_end (void);

extern void GOMP_task (void (*) (void *), void *, void (*) (void *, void *),
                       long, long, bool, unsigned);
extern void GOMP_taskwait (void);

extern unsigned GOMP_sections_start (unsigned);
extern unsigned GOMP_sections_next (void);
extern void GOMP_parallel_sections_start (void (*) (void *), void *,
                                          unsigned, unsigned);
extern void GOMP_sections_end (void);
extern void GOMP_sections_end_nowait (void);

extern void *GOMP_single_copy_start (void);
extern void GOMP_single_copy_end (void *);

*/

#ifdef __cplusplus
 }
#endif
 
#endif /* LIB_XOMP_H */



 
