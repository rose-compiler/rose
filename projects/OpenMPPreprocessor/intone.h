/* Intone RTL header file */

#ifndef __INTONE_H
#define __INTONE_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined (IRIXR8000) || defined (IRIXN32)
#define THREADS_OUT_IS_FOP
#endif

/*#define __INTONE_FREE	0
#define __INTONE_BUSY	1*/

#ifndef MIN
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#endif
#ifndef MAX
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#endif

#define STATIC          0
#define DYNAMIC		1
#define GUIDED          2
#define RUNTIME         4
#define ORDERED        16

#define MAXLOOPS        64
#define MAXTHNUM        256

#define __LOOP_FREE     0
#define __LOOP_BUSY     1

/*#define TRUE		1
#define FALSE		0*/

/*
typedef unsigned long __intone_address_t
*/

struct loop_desc_t {
        int start;
        int end;
        int step;
        int chunk;
        int realchunk;
	int remainder;
        int scheduletype;
        int ordered;
        int busy;
        int loopno;
        int team;
        int dummy;
        char padding0 [80+128];
#ifdef THREADS_OUT_IS_FOP
        struct fop_address * p_threads_out;
        char padding1[120+128];
#else
	volatile int threads_out;
        char padding1 [124+128];
#endif
	int next_iter;      /* when schedule type is dynamic */
        char padding2 [124+128];
 volatile int actual_iter;  /* ordered needs a shared variable in the loop
                               descriptor */
        char padding3 [124+128];
};

extern struct loop_desc_t loops[MAXLOOPS]
#if defined (LINUXPENTIUM) || defined (NTPENTIUM) || defined (LINUXPENTIUM_NDSM)
__attribute__ ((aligned (256)))
#endif
 ;
#if defined (IRIXR8000) || defined (IRIXN32)
#pragma align_symbol (loops, 256)
#endif

struct th_iter_desc_t {
        int loopid;
        int loopno;
	int actual_iter;
	int last_iter;
	int vpid_turn;
        int next_iter;      /* when schedule type is static */
	char padding [104+128];
};

extern struct th_iter_desc_t th_iters[MAXTHNUM]
#if defined (LINUXPENTIUM) || defined (NTPENTIUM) || defined (LINUXPENTIUM_NDSM)
__attribute__ ((aligned (256)))
#endif
 ;
#if defined (IRIXR8000) || defined (IRIXN32)
#pragma align_symbol (th_iters, 256)
#endif


#define __INTONE_BARRIER    1
#define __INTONE_NO_BARRIER 0

/* INTONE internal functions */ 
int in__tone_cpus_current_(void);
int in__tone_get_thread_num_(void);
int in__tone_is_master_(void);
int in__tone_in_parallel (void);

/* Parallel regions */

void in__tone_spawnparallel_(
	void (* __intone_function_) (),
	int * __intone_nargs,
	int * __intone_nprocs,
	...);


/* Work-sharing constructs */
void in__tone_begin_for_ (
	int *__intone_start,
	int *__intone_end,
	int *__intone_step,
	int *__intone_chunk,
	int *__intone_scheduletype);

int in__tone_next_iters_ (
        int * __intone_lstart,
        int * __intone_lend,
        int * __intone_last);

void in__tone_end_for_ (
	int * __intone_barrier_needed);


/* Synchronization */
void in__tone_barrier_();
#if defined (IRIXR8000) || defined (AIX4POWER3)
unsigned long in__tone_atomic_update_8_ (
	long * __intone_address,
	long * __intone_oldvalue,
	long * __intone_newvalue);
#endif
#if defined (IRIXN32) || \
    defined (LINUXPENTIUM) || defined (NTPENTIUM) || defined (LINUXPENTIUM_NDSM)
unsigned long long in__tone_atomic_update_8_ (
	unsigned long long * __intone_address,
	unsigned long long * __intone_oldvalue,
	unsigned long long * __intone_newvalue);
#endif
int in__tone_atomic_update_4_ (
	int * __intone_address,
	int * __intone_oldvalue,
	int * __intone_newvalue);

void in__tone_global_flush_();
void in__tone_selective_flush_(void *s);

typedef void * in__tone_lock_t;

void in__tone_init_lock_(in__tone_lock_t *__intone_lock);
void in__tone_set_lock_(in__tone_lock_t *__intone_lock);
void in__tone_unset_lock_(in__tone_lock_t *__intone_lock);
int in__tone_test_lock_(in__tone_lock_t *__intone_lock);
void in__tone_destroy_lock_(in__tone_lock_t *__intone_lock);

void in__tone_init_lock (in__tone_lock_t *__intone_lock);
void in__tone_set_lock (in__tone_lock_t *__intone_lock);
void in__tone_unset_lock (in__tone_lock_t *__intone_lock);
int in__tone_test_lock (in__tone_lock_t *__intone_lock);
void in__tone_destroy_lock (in__tone_lock_t *__intone_lock);

#if 0 
esborrar
/* Lock pool definitions */

struct __intone_lockdata {
   unsigned long _lockdata[CACHE_LINE_SIZE/sizeof(long)];
};

struct __intone_lockpool {
   unsigned int poolbits[CACHE_LINE_SIZE/sizeof(int)];
   struct __intone_lockdata[1];
};

#define __INTONE_NLOCKS_PER_POOL 
#endif

void in__tone_enter_ordered_(int * __intone_iter);
void in__tone_leave_ordered_(int * __intone_iter);

void in__tone_allocate_ (
	int * __intone_size,
	int * __intone_nelems,
	int * __intone_return_status_wanted,
	void ** __intone_mem,
	int * __intone_return_status);
void in__tone_deallocate_ (
	int * __intone_size,
	int * __intone_nelems,
	int * __intone_return_status_wanted,
	void ** __intone_mem,
	int * __intone_return_status);

/* Initialisation */
/*
int in__tone_init(int argc, char **argv);
*/
void in__tone_init(void);

/*
void omp_set_dynamic_ (int * on)
int omp_get_dynamic_ ()
void omp_set_nested_ (int * on)
int omp_get_nested_ ()
int omp_get_num_procs_ ()
void omp_set_num_threads (int * ncpus)
int omp_get_max_threads_ ()
int omp_get_num_threads_ ()
int omp_get_num_threads ()
int omp_get_thread_num ()
int omp_get_thread_num_ ()
void omp_init_lock_ (spin_t * lock)
void omp_destroy_lock_ (spin_t * lock)
void omp_set_lock_ (spin_t * lock)
void omp_unset_lock_ (spin_t * lock)
int omp_test_lock_ (spin_t * lock)
*/

#ifdef __INTONE_NTH

extern void omp_init_lock  (in__tone_lock_t * __intone_lock);
extern void omp_init_lock_ (in__tone_lock_t * __intone_lock);

extern void omp_set_lock (in__tone_lock_t * __intone_lock);
extern void omp_set_lock_ (in__tone_lock_t * __intone_lock);

extern int  omp_test_lock (in__tone_lock_t * __intone_lock);
extern int  omp_test_lock_ (in__tone_lock_t * __intone_lock);

extern void omp_unset_lock (in__tone_lock_t * __intone_lock);
extern void omp_unset_lock_ (in__tone_lock_t * __intone_lock);

extern void omp_destroy_lock (in__tone_lock_t * __intone_lock);
extern void omp_destroy_lock_ (in__tone_lock_t * __intone_lock);

#endif


#define MAX_INTONE_SPAWNPARALLEL_ARGS (MAX_WORK_ARGS)

int in__tone_set_specific (int key, void * data);
int __intone_set_specific (int key, void * data);
                /* returns -1 if the key is out of range
                   and 0 otherwise */

int in__tone_set_specific_ (int * key, void ** data);
int __intone_set_specific_ (int * key, void ** data);


void * in__tone_get_specific (int key);
void * __intone_get_specific (int key);
                /* returns NULL if the key is out of range
                   and the previous recorded value otherwise */

void * in__tone_get_specific_ (int * key);
void * __intone_get_specific_ (int * key);

int in__tone_key_create (int * key, void (* destroy_func) (void *));
int __intone_key_create (int * key, void (* destroy_func) (void *));

int in__tone_key_create_ (int * key, void (* destroy_func) (void *));
int __intone_key_create_ (int * key, void (* destroy_func) (void *));


#define INTONE_MAXKEY (NTH_USER_DATA_SIZE/sizeof (void *))

/* Command line management */
extern int __Argc;
extern char ** __Argv;
#if defined (LINUXPENTIUM) || defined (LINUXPENTIUM_NDSM) || \
    defined (NTPENTIUM)
extern int f__xargc;
#endif


#ifdef __cplusplus
}
#endif
#endif
