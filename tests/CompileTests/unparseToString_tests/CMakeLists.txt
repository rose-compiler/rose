add_executable(unparseToString unparseToString.C )
target_link_libraries(unparseToString ROSE_DLL EDG ${link_with_libraries} )

set(TESTCODES unparseToString_Type-Input.C)
set(ROSE_FLAGS --edg:no_warnings -w --edg:restrict)

foreach(file_to_test ${TESTCODES})
  add_test(
    NAME ut_${file_to_test}
    COMMAND unparseToString ${ROSE_FLAGS}
            -c ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
  add_test(
    NAME ua_${file_to_test}
    COMMAND unparseToString --all ${ROSE_FLAGS}
            -c ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
endforeach()
