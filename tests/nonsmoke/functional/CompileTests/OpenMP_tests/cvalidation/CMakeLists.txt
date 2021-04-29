##########################################################################
# These are OpenMP C testcodes targeting aspects of OpenMP specific to C #
##########################################################################

set(TESTCODES for_firstprivate.c for_lastprivate.c)

if(with-ROSE_LONG_MAKE_CHECK_RULE)
  list(APPEND TESTCODES
    for_ordered.c for_private.c for_reduction.c for_schedule_dynamic.c
    for_schedule_guided.c for_schedule_static.c has_openmp.c omp_atomic.c
    omp_barrier.c omp_copyin.c omp_critical.c omp_flush.c omp_get_num_threads.c
    omp_in_parallel.c omp_lock.c omp_master_thread.c omp_nested.c omp_nest_lock.c
    omp_nest_testlock.c omp_num_threads.c omp_testlock.c omp_threadprivate.c
    omp_ticks_time.c omp_time.c parallel_for_firstprivate.c
    parallel_for_lastprivate.c parallel_for_ordered.c parallel_for_private.c
    parallel_for_reduction.c parallel_section_firstprivate.c
    parallel_section_lastprivate.c parallel_section_private.c
    parallel_section_reduction.c section_firstprivate.c section_lastprivate.c
    section_private.c section_reduction.c single.c single_copyprivate.c
    single_nowait.c single_private.c main.c omp_pragma_example1.c
    omp_pragma_example2.c)
endif()

set(ROSE_FLAGS -rose:C_only --edg:no_warnings -w -rose:verbose 0 --edg:restrict
               -rose:openmp -rose:openmp:ast_only)

foreach(file_to_test ${TESTCODES})
  add_test(
    NAME OMPTEST_${file_to_test}
    COMMAND parseOmp ${ROSE_FLAGS} -I${CMAKE_SOURCE_DIR}/src/frontend/SageIII
                     -c ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
  set_tests_properties(OMPTEST_${file_to_test} PROPERTIES LABELS OMPTEST)
endforeach()
