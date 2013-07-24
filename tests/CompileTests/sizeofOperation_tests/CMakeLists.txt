add_executable( SizeofTest_GNU SizeofTest_GNU.C )
target_link_libraries(SizeofTest_GNU ROSE_DLL edg33 ${link_with_libraries} )

add_executable( SizeofTest_Intel SizeofTest_Intel.C )
target_link_libraries(SizeofTest_Intel ROSE_DLL edg33 ${link_with_libraries} )

set(TESTCODES inputCode_SizeofTest.C)

foreach(file_to_test ${TESTCODES})
  add_test(
    NAME ${file_to_test}_GNU
    COMMAND SizeofTest_GNU -c ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
  add_test(
    NAME ${file_to_test}_Intel
    COMMAND SizeofTest_Intel -c ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
endforeach()

