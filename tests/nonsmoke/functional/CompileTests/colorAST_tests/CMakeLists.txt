add_executable( testAstVisualization testAstVisualization.C )
target_link_libraries(testAstVisualization ROSE_DLL EDG ${link_with_libraries} )
install(TARGETS testAstVisualization DESTINATION bin)

set(TESTCODES inputExample.C)
set(ROSE_FLAGS --edg:no_warnings -w --edg:restrict)

foreach(file_to_test ${TESTCODES})
  add_test(
    NAME ${file_to_test}
    COMMAND testAstVisualization ${ROSE_FLAGS}
            -c ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
  set_tests_properties(${file_to_test} PROPERTIES LABELS COLORASTTEST)
endforeach()
