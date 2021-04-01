add_executable( SizeofTest_GNU SizeofTest_GNU.C )
target_link_libraries(SizeofTest_GNU ROSE_DLL EDG ${link_with_libraries} )
install(TARGETS SizeofTest_GNU DESTINATION bin)

add_executable( SizeofTest_Intel SizeofTest_Intel.C )
target_link_libraries(SizeofTest_Intel ROSE_DLL EDG ${link_with_libraries} )
install(TARGETS SizeofTest_Intel DESTINATION bin)

set(TESTCODES inputCode_SizeofTest.C)

foreach(file_to_test ${TESTCODES})
  add_test(
    NAME SIZEOFOPTEST_${file_to_test}_GNU
    COMMAND SizeofTest_GNU -c ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
  set_tests_properties(SIZEOFOPTEST_${file_to_test}_GNU PROPERTIES LABELS SIZEOFOPTEST)
  add_test(
    NAME SIZEOFOPTEST_${file_to_test}_Intel
    COMMAND SizeofTest_Intel -c ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
  set_tests_properties(SIZEOFOPTEST_${file_to_test}_Intel PROPERTIES LABELS SIZEOFOPTEST)
endforeach()

