###################################################
# These are Python testcodes (C++ using Python.h) #
###################################################

set(TESTCODES test2004_92.C)
set(ROSE_FLAGS --edg:no_warnings -w --edg:restrict)

foreach(file_to_test ${TESTCODES})
  add_test(
    NAME PYTHONTEST_${file_to_test}
    COMMAND testTranslator ${ROSE_FLAGS} -I${CMAKE_CURRENT_SOURCE_DIR}/pythonDir
                           -c ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
  set_tests_properties(PYTHONTEST_${file_to_test} PROPERTIES LABELS PYTHONTEST)
endforeach()
