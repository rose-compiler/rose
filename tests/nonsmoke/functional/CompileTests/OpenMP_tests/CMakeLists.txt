add_executable(parseOmp parseOmp.C)
target_link_libraries(parseOmp ROSE_DLL EDG ${link_with_libraries})

if(enable-c)
  add_subdirectory(cvalidation)
endif()

if(enable-fortran)
  add_subdirectory(fortran)
endif()

if(NOT enable-c)
  return()
endif()

set(TESTCODES
	3loops.c jacobi.c alignment.c array_init.c atomic.c atoms-2.c barrier.c
	collapse.c copyin.c copyprivate2.c copyprivate3.c copyprivate.c critical.c
	critical_dead.c critical_orphaned.c dijkstra_open_mp.c dynamicChunk.c empty.c
	endif.c endif2.c endif3.c nowait.c expressions.c falsesharing.c firstprivate.c
	firstPrivateArray.c firstlastprivate.c flush.c flush_exampleA_21_1c.c
	full_verify.c get_max_threads.c hello.c hello-1.c hello-2.c hello-ordered.c
	init.c lastprivate0.c lastprivate.c lastprivateOrphaned.c limits_threads.c
	linebreak.c lockarray.c loop1.c lu_factorization.c master.c masterSingle.c
	matrix_vector.c md_open_mp.c multiple_return.c nestedpar1.c nestedpar.c omp1.c
	ompfor.c ompfor2.c ompfor3.c ompfor4.c ompfor5.c ompfor6.c ompfor-default.c
	ompfor-decremental.c ompfor-static.c ompGetNumThreads.c omp_sections.c
	ordered2.c ordered.c orphaned-directives.c parallel.c parallel-if.c
	parallel-if-numthreads.c parallel-numthreads.c parallel-reduction.c
	parallel-reduction2.c parallelfor.c parallelfor2.c parallelsections.c
	preprocessingInfo.c private.c privatej.c private-duplicate.c recursive.c
	reduction2.c reduction.c reduction-classic.c rice1.c section.c section1.c
	set_num_threads.c shared.c single.c single2.c single_copyprivate.c sizeof.c
	spmd1.c staticChunk.c subteam2.c subteam.c task_array.c task_largenumber.c
	task_orphaned.c task_untied.c task_untied2.c task_untied3.c task_untied4.c
	task_underIf.c task_wait.c task_wait2.c twoRegions.c threadprivate2.c
	threadprivate3.c threadprivate.c threadProcessor.c upperCase.c variables.c
	classMember.cpp hello1.cpp helloNested.cpp memberFunction.cpp
	objectPrivate.cpp objectFirstPrivate.cpp objectLastprivate.cpp
	orphanedAtomic.cpp preprocessingInfo2.cpp task_link.cpp task_link2.cpp
	task_tree.cpp)

set(OMP_ACC_TESTCODES
  axpy_ompacc.c axpy_ompacc2.c matrixmultiply-ompacc.c jacobi-ompacc.c
  jacobi-ompacc-opt1.c)

set(ROSE_FLAGS -rose:openmp:ast_only --edg:no_warnings -w -rose:verbose 0
               --edg:restrict)

# used to find omp.h, added it as one of  rose headers
set(TESTCODE_INCLUDES -I${CMAKE_SOURCE_DIR}/src/frontend/SageIII)

foreach(file_to_test ${TESTCODES})
  add_test(
    NAME OMPTEST_${file_to_test}
    COMMAND parseOmp ${ROSE_FLAGS} ${TESTCODE_INCLUDES} -c
            ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
  set_tests_properties(OMPTEST_${file_to_test} PROPERTIES LABELS OMPTEST)
endforeach()

foreach(file_to_test ${OMP_ACC_TESTCODES})
  add_test(
    NAME OMPACCTEST_${file_to_test}
    COMMAND parseOmp -rose:openmp:ast_only -rose:skipfinalCompileStep
                     --edg:no_warnings -w -rose:verbose 0 --edg:restrict
                     -c ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
  set_tests_properties(OMPACCTEST_${file_to_test} PROPERTIES LABELS OMPACCTEST)
endforeach()


# Liao 11/18/2009
# Test for a bug which happens when more than two source files show in the command line
# the translator should now be able to avoid duplicated processing of pragma statements
add_test(
  NAME OMPTEST_bonds-2
  COMMAND parseOmp ${ROSE_FLAGS} ${TESTCODE_INCLUDES}
          -c ${CMAKE_CURRENT_SOURCE_DIR}/atoms-2.c
          ${CMAKE_CURRENT_SOURCE_DIR}/bonds-2.c)
  set_tests_properties(OMPTEST_bonds-2 PROPERTIES LABELS OMPTEST)

add_test(
  NAME macroIds
  COMMAND parseOmp ${ROSE_FLAGS} ${TESTCODE_INCLUDES}
          -rose:collectAllCommentsAndDirectives
          ${CMAKE_CURRENT_SOURCE_DIR}/macroIds.c)
  set_tests_properties(macroIds PROPERTIES LABELS OMPTEST)
