#ifndef _OMP_H_DEF
#define _OMP_H_DEF

/*
From the specification 3.0, Chapter 3.1, page 108
What to put into this file:
* The prototypes of all the runtime library routines, with "C" linkage
* The type omp_lock_t
* The type omp_nest_lock_t
* The type omp_sched_t

Specification 3.0 page 302
D.1 Example of the omp.h Header File
 * */

typedef void *omp_lock_t;  /* represented as a pointer */
typedef void *omp_nest_lock_t; /* represented as a pointer */

typedef enum omp_sched_t
{
  omp_sched_static = 1,
  omp_sched_dynamic = 2,
  omp_sched_guided = 3,
  omp_sched_auto = 4
} omp_sched_t;


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Execution Environment Functions
 */
void omp_set_num_threads(int num);
int omp_get_num_threads(void);
int omp_get_max_threads(void);
int omp_get_thread_num(void);
int omp_get_num_procs(void);

int omp_in_parallel(void);
void omp_set_dynamic(int dynamic_thds);
int omp_get_dynamic(void);

void omp_set_nested(int n_nested);
int omp_get_nested(void);

/* 
 * Timer routine
 */
double omp_get_wtime(void);
double omp_get_wtick(void);

/*
 * Lock Functions
 */
void omp_init_lock(omp_lock_t *lock);
void omp_init_nest_lock(omp_nest_lock_t *lock);
void omp_destroy_lock(omp_lock_t *lock);
void omp_destroy_nest_lock(omp_nest_lock_t *lock);
void omp_set_lock(omp_lock_t *lock);
void omp_set_nest_lock(omp_nest_lock_t *lock);
void omp_unset_lock(omp_lock_t *lock);
void omp_unset_nest_lock(omp_nest_lock_t *lock);
int omp_test_lock(omp_lock_t *lock);
int omp_test_nest_lock(omp_nest_lock_t *lock);

/*
 * Other internal variables
 */
void omp_set_schedule (omp_sched_t, int);
void omp_get_schedule (omp_sched_t *, int *);
int omp_get_thread_limit (void);
void omp_set_max_active_levels (int);
int omp_get_max_active_levels (void);
int omp_get_level (void);
int omp_get_ancestor_thread_num (int);
int omp_get_team_size (int);
int omp_get_active_level (void);

/*
 * FORTRAN Execution Environment Function Wrappers
 * Fortran stuff should be handled in omp_lib.h
void omp_set_num_threads_(int *num);
int omp_get_num_threads_(void);
int omp_get_max_threads_(void);
int omp_get_thread_num_(void);
int omp_get_num_procs_(void);
int omp_in_parallel_(void);
void omp_set_dynamic_(int *dynamic_thds);
int omp_get_dynamic_(void);
void omp_set_nested_(int *n_nested);
int omp_get_nested_(void);

 * FORTRAN Lock Function Wrappers
typedef unsigned int _omf77Lock_t;
void omp_init_lock_(_omf77Lock_t *lock);
void omp_init_nest_lock_(_omf77Lock_t *lock);
void omp_destroy_lock_(_omf77Lock_t *lock);
void omp_destroy_nest_lock_(_omf77Lock_t *lock);
void omp_set_lock_(_omf77Lock_t *lock);
void omp_set_nest_lock_(_omf77Lock_t *lock);
void omp_unset_lock_(_omf77Lock_t *lock);
void omp_unset_nest_lock_(_omf77Lock_t *lock);
int omp_test_lock_(_omf77Lock_t *lock);
int omp_test_nest_lock_(_omf77Lock_t *lock);

*/
#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* _OMP_H_DEF */

