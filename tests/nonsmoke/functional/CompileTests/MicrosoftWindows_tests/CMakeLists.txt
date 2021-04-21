set(TESTCODES simple_windows_prog1.C tibbar_packer.C)

# File option to accumulate performance information about the compilation
set(PERFORMANCE_REPORT_OPTION -rose:compilationPerformanceFile
  ${CMAKE_BINARY_DIR}/MSWindows_ROSE_PERFORMANCE_DATA.csv)

# Note that when we compile 32 bit code on a 64 bit machine the generated code should
# not be compiled as 64 bit code so we used the option: -rose:skipfinalCompileStep
set(ROSE_FLAGS --edg:restrict --edg:no_warnings -w -rose:skipfinalCompileStep
  ${PERFORMANCE_REPORT_OPTION})

foreach(file_to_test ${TESTCODES})
  add_test(
    NAME ${file_to_test}
    COMMAND testTranslator ${ROSE_WINE_INCLUDES} ${ROSE_FLAGS}
      -c ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
  set_tests_properties(${file_to_test} PROPERTIES LABELS MSTEST)
endforeach()
