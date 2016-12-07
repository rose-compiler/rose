// the Omni runtime library's interface to the compiler
// TODO: sort the list

// by C. Liao, June 14, 2006


#ifdef __cplusplus
extern "C" {
#endif

extern void _ompc_init(int argc, char *argv[]);
extern void _ompc_terminate (int);
extern void _ompc_do_parallel(void (*func)(void **),void *args);
//extern unsigned __ompc_output(char [],...);
extern void _ompc_default_sched(int *lb, int *ub, int *step);
extern void _ompc_reduction(void *in_p,void *out_p,int type, int op);
extern void _ompc_barrier(void);
extern int _ompc_do_single(void);
extern int _ompc_is_master(void);
extern void _ompc_flush(char *dst, int nbyte);

//extern void * __ompc_lock_critical;
extern void _ompc_enter_critical(void **);
extern void _ompc_exit_critical(void **);
extern void _ompc_atomic_lock(void);
extern void _ompc_atomic_unlock(void);
extern int _ompc_is_last(void);
extern void * _ompc_get_thdprv(void ***thdprv_p,int size,void *datap);
extern void _ompc_copyin_thdprv(void *datap,void *global_datap,int size);
// scheduling
extern void _ompc_default_sched(int *lb, int *ub, int *step);
extern void _ompc_static_bsched(int *lb, int *ub, int *step);
extern void _ompc_static_sched_init(int lb, int up, int step, int chunk_size);
extern void _ompc_dynamic_sched_init(int lb, int up, int step, int chunk_size);
extern void _ompc_guided_sched_init(int lb, int up, int step, int chunk_size);
extern void _ompc_runtime_sched_init(int lb, int up, int step);
extern int _ompc_static_sched_next(int *lb, int *ub);
extern int _ompc_dynamic_sched_next(int *lb, int *ub);
extern int _ompc_guided_sched_next(int *lb, int *ub);
extern int _ompc_runtime_sched_next(int *lb, int *ub);

// section
extern void _ompc_section_init(int n_sections);
extern int _ompc_section_id();

extern double omp_get_wtick();

// DQ (12/6/2016): We want to provide n explicit declaration of this function to avoid implicit function declaration warning: -Wimplicit-function-declaration.
extern void _ompc_init_ordered(int lb,int step);

#ifdef __cplusplus
 }
#endif

