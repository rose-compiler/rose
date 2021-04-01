set(TESTCODES
  12.3.2c.cc 13.1a.cc 13.1d.cc 3.4.3a.cc 7.3.1.2b.cc 7.3.3d.cc 7.3.4b.cc)
set(ROSE_FLAGS --edg:no_warnings -w)

foreach(file_to_test ${TESTCODES})
  add_test(
    NAME ELSATEST_${file_to_test}
    COMMAND testTranslator
      ${ROSE_FLAGS} ${ROSE_INCLUDE_FLAGS} -I${CMAKE_CURRENT_SOURCE_DIR} -c
      ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
  set_tests_properties(ELSATEST_${file_to_test} PROPERTIES LABELS ELSATEST)
endforeach()
