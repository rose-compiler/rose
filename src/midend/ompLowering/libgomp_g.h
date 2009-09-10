/*  The libgomp's interface to the compiler 
 *
 *  Basically this is a copy of gcc_release/libgomp/libgomp_g.h
 *
 *  */
#ifndef LIBGOMP_G_H 
#define LIBGOMP_G_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
extern void GOMP_barrier (void);

extern void GOMP_critical_start (void); 
extern void GOMP_critical_end (void);
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

//extern void GOMP_parallel_start (void (*) (void *), void *, unsigned);
// Avoid warning like  passing argument from incompatible pointer type
extern void GOMP_parallel_start (void (*) (void **), void *, unsigned);
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

extern bool GOMP_single_start (void);
extern void *GOMP_single_copy_start (void);
extern void GOMP_single_copy_end (void *);


#ifdef __cplusplus
 }
#endif
 
#endif /* LIBGOMP_G_H */



 
