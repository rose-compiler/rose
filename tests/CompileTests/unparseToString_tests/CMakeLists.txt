add_executable(unparseToString_Type unparseToString_Type.C )
target_link_libraries(unparseToString_Type ROSE_DLL edg33 ${link_with_libraries} )

add_executable(unparseToString_All unparseToString_All.C )
target_link_libraries(unparseToString_All ROSE_DLL edg33 ${link_with_libraries} )

set(TESTCODES unparseToString_Type-Input.C)
set(ROSE_FLAGS --edg:no_warnings -w --edg:restrict)

foreach(file_to_test ${TESTCODES})
  add_test(
    NAME ${file_to_test}
    COMMAND unparseToString_All ${ROSE_FLAGS}
            -c ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
endforeach()
