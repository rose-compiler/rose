add_executable(unparseToString unparseToString.C )
target_link_libraries(unparseToString ROSE_DLL EDG ${link_with_libraries} )

set(ROSE_FLAGS --edg:no_warnings -w --edg:restrict)

set(TESTCODE_DIR ${CMAKE_SOURCE_DIR}/tests/CompileTests/Cxx_tests)

set(TESTCODE_INCLUDES -I${CMAKE_SOURCE_DIR}/tests/CompileTests/A++Code
                      -I${TESTCODE_DIR})

# This populates the list ROSE__CXX_TESTS
include(${CMAKE_CURRENT_SOURCE_DIR}/../Cxx_tests/Cxx_Testcodes.cmake)
foreach(file_to_test ${ROSE__CXX_TESTS})
  add_test(
    NAME ut_${file_to_test}
    COMMAND unparseToString ${ROSE_FLAGS} ${TESTCODE_INCLUDES}
            -c ${TESTCODE_DIR}/${file_to_test})
  add_test(
    NAME ua_${file_to_test}
    COMMAND unparseToString --all ${ROSE_FLAGS} ${TESTCODE_INCLUDES}
            -c ${TESTCODE_DIR}/${file_to_test})

  set_tests_properties(ua_${file_to_test}
    PROPERTIES DEPENDS ut_${file_to_test} )
endforeach()
