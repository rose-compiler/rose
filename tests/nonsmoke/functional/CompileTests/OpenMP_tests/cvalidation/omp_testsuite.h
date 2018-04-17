#include <stdio.h>
#ifndef OMP_TESTSUITE_H
#define OMP_TESTSUITE_H

#define LOOPCOUNT 10000

int check_omp_critical(FILE * logFile);
int check_omp_atomic(FILE * logFile);
int check_omp_barrier(FILE * logFile);
int check_omp_flush(FILE * logFile);
int check_omp_critical(FILE * logFile);
int check_omp_atomic(FILE * logFile);
int check_omp_nested(FILE * logFile);
int check_for_ordered(FILE * logFile);
int check_for_reduction(FILE * logFile);
int check_for_private(FILE * logFile);
int check_for_firstprivate(FILE * logFile);
int check_for_lastprivate(FILE * logFile);
int check_for_schedule(FILE * logFile);
int check_for_schedule_static(FILE * logFile);
int check_for_schedule_dynamic(FILE * logFile);
int check_for_schedule_guided(FILE * logFile);
int check_has_openmp(FILE * logFile);
int check_omp_get_num_threads(FILE * logFile);
int check_omp_in_parallel(FILE * logFile);
int check_omp_lock(FILE * logFile);
int check_omp_testlock(FILE * logFile);
int check_omp_nest_lock(FILE * logFile);
int check_omp_nest_testlock(FILE * logFile);
int check_omp_master_thread(FILE * logFile);
int checkomp_get_num_threads(FILE * logFile);
int check_for_private(FILE * logFile);
int check_parallel_for_ordered(FILE * logFile);
int check_parallel_for_reduction(FILE * logFile);
int check_parallel_for_private(FILE * logFile);
int check_parallel_for_firstprivate(FILE * logFile);
int check_parallel_for_lastprivate(FILE * logFile);
int check_parallel_section_reduction(FILE * logFile);
int check_parallel_section_private(FILE * logFile);
int check_parallel_section_firstprivate(FILE * logFile);
int check_parallel_section_lastprivate(FILE * logFile);
int check_for_schedule(FILE * logFile);
int check_section_reduction(FILE * logFile);
int check_section_private(FILE * logFile);
int check_section_firstprivate(FILE * logFile);
int check_section_lastprivate(FILE * logFile);
int check_single(FILE * logFile);
int check_single_private(FILE * logFile);
int check_single_nowait(FILE * logFile);
int check_single_copyprivate(FILE * logFile);
int check_omp_threadprivate(FILE * logFile);
int check_omp_copyin(FILE * logFile);
int check_omp_copyprivate(FILE * logFile);
/*By C. Liao, to be consistent and facilitate scripting
*/
int check_omp_time(FILE * logFile);
int check_omp_ticks_time(FILE * logFile);
int check_omp_num_threads(FILE * logFile);


int crosscheck_omp_critical(FILE * logFile);
int crosscheck_omp_atomic(FILE * logFile);
int crosscheck_omp_barrier(FILE * logFile);
int crosscheck_omp_flush(FILE * logFile);
int crosscheck_omp_critical(FILE * logFile);
int crosscheck_omp_atomic(FILE * logFile);
int crosscheck_omp_nested(FILE * logFile);
int crosscheck_for_ordered(FILE * logFile);
int crosscheck_for_reduction(FILE * logFile);
int crosscheck_for_private(FILE * logFile);
int crosscheck_for_firstprivate(FILE * logFile);
int crosscheck_for_lastprivate(FILE * logFile);
int crosscheck_for_schedule(FILE * logFile);
int crosscheck_for_schedule_static(FILE * logFile);
int crosscheck_for_schedule_dynamic(FILE * logFile);
int crosscheck_for_schedule_guided(FILE * logFile);
int crosscheck_has_openmp(FILE * logFile);
int crosscheck_omp_get_num_threads(FILE * logFile);
int crosscheck_omp_in_parallel(FILE * logFile);
int crosscheck_omp_lock(FILE * logFile);
int crosscheck_omp_testlock(FILE * logFile);
int crosscheck_omp_nest_lock(FILE * logFile);
int crosscheck_omp_nest_testlock(FILE * logFile);
int crosscheck_omp_master_thread(FILE * logFile);
int crosscheck_for_num_threads(FILE * logFile);
int crosscheckomp_get_num_threads(FILE * logFile);
int crosscheck_for_private(FILE * logFile);
int crosscheck_parallel_for_ordered(FILE * logFile);
int crosscheck_parallel_for_reduction(FILE * logFile);
int crosscheck_parallel_for_private(FILE * logFile);
int crosscheck_parallel_for_firstprivate(FILE * logFile);
int crosscheck_parallel_for_lastprivate(FILE * logFile);
int crosscheck_parallel_section_reduction(FILE * logFile);
int crosscheck_parallel_section_private(FILE * logFile);
int crosscheck_parallel_section_firstprivate(FILE * logFile);
int crosscheck_parallel_section_lastprivate(FILE * logFile);
int crosscheck_for_schedule(FILE * logFile);
int crosscheck_section_reduction(FILE * logFile);
int crosscheck_section_private(FILE * logFile);
int crosscheck_section_firstprivate(FILE * logFile);
int crosscheck_section_lastprivate(FILE * logFile);
int crosscheck_single(FILE * logFile);
int crosscheck_single_private(FILE * logFile);
int crosscheck_single_nowait(FILE * logFile);
int crosscheck_single_copyprivate(FILE * logFile);
int crosscheck_omp_threadprivate(FILE * logFile);
int crosscheck_omp_copyin(FILE * logFile);
int crosscheck_single_copyprivate(FILE * logFile);

/* change names to be consistent with others, by Liao
int omp_crosscheck_time(FILE * logFile);
int omp_crosscheck_ticks_time(FILE * logFile);
int omp_crosscheck_num_threads(FILE * logFile);
*/
int crosscheck_omp_time(FILE * logFile);
int crosscheck_omp_ticks_time(FILE * logFile);
int crosscheck_omp_num_threads(FILE * logFile);


typedef int (*a_ptr_to_test_function)(FILE * logFile);

#endif
