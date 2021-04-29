set(TESTCODES
  testNameQalTypeElab_01.C testNameQalTypeElab_02.C testNameQalTypeElab_03.C
  testNameQalTypeElab_04.C testNameQalTypeElab_05.C testNameQalTypeElab_06.C
  testNameQalTypeElab_07.C testNameQalTypeElab_08.C testNameQalTypeElab_09.C
  testNameQalTypeElab_10.C testNameQalTypeElab_11.C testNameQalTypeElab_12.C
  testNameQalTypeElab_13.C testNameQalTypeElab_14.C testNameQalTypeElab_15.C
  testNameQalTypeElab_16.C testNameQalTypeElab_17.C testNameQalTypeElab_18.C
  testNameQalTypeElab_22.C testNameQalTypeElab_23.C testNameQalTypeElab_24.C
  testNameQalTypeElab_25.C testNameQalTypeElab_26.C testNameQalTypeElab_27.C
  testNameQalTypeElab_28.C testNameQalTypeElab_29.C testNameQalTypeElab_30.C
  testNameQalTypeElab_31.C testNameQalTypeElab_32.C testNameQalTypeElab_33.C
  testNameQalTypeElab_34.C testNameQalTypeElab_35.C testNameQalTypeElab_36.C
  testNameQalTypeElab_37.C testNameQalTypeElab_38.C testNameQalTypeElab_39.C
  testNameQalTypeElab_40.C)

# File option to accumulate performance information about the compilation
set(PERFORMANCE_REPORT_OPTION -rose:compilationPerformanceFile
  ${CMAKE_BINARY_DIR}/Cxx_ROSE_PERFORMANCE_DATA.csv)

set(ROSE_FLAGS --edg:no_warnings -w -rose:verbose 0
  ${PERFORMANCE_REPORT_OPTION})

## Include the path to A++ and the transformation specification
set(TESTCODE_INCLUDES
  -I${CMAKE_SOURCE_DIR}/tests/nonsmoke/functional/CompileTests/A++Code
  -I${CMAKE_CURRENT_SOURCE_DIR})

foreach(file_to_test ${TESTCODES})
  add_test(
    NAME NAMEQUALIFICATIONTEST_${file_to_test}
    COMMAND testTranslator ${ROSE_FLAGS} ${TESTCODE_INCLUDES}
     -c ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
  set_tests_properties(NAMEQUALIFICATIONTEST_${file_to_test} PROPERTIES LABELS NAMEQUALIFICATIONTEST)
endforeach()
