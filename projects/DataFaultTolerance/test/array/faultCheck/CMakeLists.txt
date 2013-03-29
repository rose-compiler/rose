include_directories(
  ${ROSE_SOURCE_DIR}/projects/DataFaultTolerance/includes
  ${ROSE_SOURCE_DIR}/projects/DataFaultTolerance/src/element
)

#CXX_TESTCODES_REQUIRED_TO_PASS
add_executable(rose_ErrtestSample rose_ErrtestSample.C)
target_link_libraries(rose_ErrtestSample faulttolerancearray)
add_executable(rose_testSample rose_testSample.C)
target_link_libraries(rose_testSample faulttolerancearray)

#TESTCODE_CURRENTLY_FAILING
#add_executable(rose_test_10Elem rose_test_10Elem.C)
#add_executable(rose_testCode rose_testCode.C)
#add_executable(rose_test_1Elem rose_test_1Elem.C)
