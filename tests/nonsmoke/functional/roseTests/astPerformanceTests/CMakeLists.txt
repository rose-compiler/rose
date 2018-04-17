################################################################################
# testPerformance -- tests how fast the "new" operator works
################################################################################
add_executable(testPerformance testPerformance.C)
target_link_libraries(testPerformance ROSE_DLL EDG ${link_with_libraries})

################################################################################
# rosePerformanceTest
################################################################################
add_executable(rosePerformanceTest rosePerformanceTest.C)
target_link_libraries(rosePerformanceTest ROSE_DLL EDG ${link_with_libraries})

install(TARGETS testPerformance rosePerformanceTest DESTINATION bin)

if (NOT CYGWIN)
  add_test(
    NAME testPerformance
    COMMAND testPerformance ${CMAKE_CURRENT_SOURCE_DIR}/tests.conf
  )

  add_test(
    NAME rosePerformanceTest
    COMMAND rosePerformanceTest "-rose:compilationPerformanceFile ROSE_PERFORMANCE_DATA.csv -c ${CMAKE_CURRENT_SOURCE_DIR}/input.C"
  )
endif()

################################################################################
# astThreadedCreation -- creates/deletes nodes with lots of threads
################################################################################
if (enable-binary-analysis AND HAVE_PTHREAD_H)
  add_executable(astThreadedCreation astThreadedCreation.C)
  target_link_libraries(astThreadedCreation ROSE_DLL EDG ${link_with_libraries})

  add_test(
    NAME astThreadedCreation
    COMMAND astThreadedCreation ${CMAKE_CURRENT_SOURCE_DIR}/tests.conf
  )
endif()
