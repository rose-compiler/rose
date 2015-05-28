# this defines ROSE__CXX_TESTS and ROSE__CXX_TESTS__BOOST
include(${CMAKE_CURRENT_SOURCE_DIR}/Cxx_Testcodes.cmake)

set(EXAMPLE_TESTCODES_REQUIRED_TO_PASS
  ${ROSE__CXX_TESTS} ${ROSE_CXX_TESTS__BOOST})

list(APPEND EXAMPLE_TESTCODES_REQUIRED_TO_PASS
  lulesh.C luleshTALC.C test2013_253.C test2013_255.C test2013_256.C
  test2013_257.C test2013_258.C test2013_259.C test2013_261.C
  test2013_271.C test2013_272.C test2013_273.C test2013_274.C test2013_275.C
  test2013_276.C test2013_277.C test2013_278.C test2013_279.C test2013_280.C
  test2013_281.C test2013_282.C)

# DQ (5/28/2015): Removed test2013_257.C and test2013_260.C to avoid race condition.
# test2013_257.C
# test2013_260.C

if(NOT "${EDG_VERSION}" EQUAL "4.7")
  # DQ (6/12/2013): This fails to compile with EDG 4.7
  # (EDG complains: line 13: error: constructor or destructor may not have its
  #  address taken)
  # Skip this test code while we report the problem to EDG (and investigate
  # more in internally).
  list(APPEND EXAMPLE_TESTCODES_REQUIRED_TO_PASS test2012_68.C)
endif()

# Configure-time conditional
if(with-ROSE_LONG_MAKE_CHECK_RULE)
  # This is the long test of ROSE (takes a while!)
  set(TESTCODES ${EXAMPLE_TESTCODES_REQUIRED_TO_PASS})
else() # !ROSE_USE_LONG_MAKE_CHECK_RULE
  # Shorten the list of code to compile to simplify checks using "make distcheck"
  # Note that the tests test2001_02.C and test2001_03.C are required because the
  # tests in ../C_subset_of_Cxx use them and that test2004_116.C is required
  # because the tests in ROSE/tests/roseTests/programTransformationTests use it.
  set(TESTCODES ${EXAMPLE_TESTCODES_REQUIRED_TO_PASS_SHORT})
endif() # ROSE_USE_LONG_MAKE_CHECK_RULE

set(SKIP_UNPARSE_C_ASM_STATEMENTS -rose:skip_unparse_asm_commands)

# DQ (5/8/2011): Adding -rose:skipfinalCompileStep so that we can test the new name qualification support that is not yet hooked up to the unparser.
set(ROSE_FLAGS --edg:no_warnings -w -rose:verbose 0
    ${SKIP_UNPARSE_C_ASM_STATEMENTS}
    -I${CMAKE_CURRENT_SOURCE_DIR}
    -I${CMAKE_SOURCE_DIR}/tests/CompileTests/A++Code)

foreach(testcode ${TESTCODES})
  compile_test(${testcode} TRUE)
endforeach()

# This requires a special makefile rule
add_test(
  NAME test2005_56
  COMMAND testTranslator ${ROSE_FLAGS}
          -DTEST_STRING_MACRO="Thu Apr 14 08:18:33 PDT 2005"
          -c ${CMAKE_CURRENT_SOURCE_DIR}/test2005_56.C
          -o test2005_56.o
)

# DQ (9/13/2013): Added new test for -rose:unparse_includes (which has a bug in
# how it unparses C++ style comments and should maybe not be unparsing
# declarations that are front-end specific (e.g. builtin function declarations).
add_test(
  NAME unparse_includes_option
  COMMAND testTranslator ${ROSE_FLAGS}
	        -rose:unparse_includes -rose:verbose 0 -I${CMAKE_BINARY_DIR}
          -c ${CMAKE_CURRENT_SOURCE_DIR}/test2013_257.C
          -o test2013_257_unparse_headers.o
)

# DQ (9/16/2013): Test linking the resulting translated file. Phil Miller
# reported an error and the issue is that va_start() and va_end() must be
# unparsed to be __builtin_va_start() and __builtin_va_end() (at least for GNU).
add_test(
  NAME unparse_includes_option_with_link
  COMMAND testTranslator ${ROSE_FLAGS}
          -rose:unparse_includes -rose:verbose 0 -I${CMAKE_BINARY_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/test2013_260.C
          -o test2013_260_unparse_headers
)

# DQ (9/17/2013): Added autoconf style link test.
add_test(
  NAME common_configure_test_with_link_part_1
  COMMAND testTranslator ${ROSE_FLAGS}
	        -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/conftest.C
          -o conftest_configure_test_part_1
)

# DQ (9/22/2013): Added autoconf style link test.
add_test(
  NAME common_configure_test_with_link_part_2
  COMMAND testTranslator ${ROSE_FLAGS}
	        -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/conftest_2.C
          -o conftest_configure_test_part_2
)

# DQ (9/22/2013): Added autoconf style link test.
add_test(
  NAME common_configure_test_with_link_part_3
  COMMAND testTranslator ${ROSE_FLAGS}
	        -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/conftest_3.C
          -o conftest_configure_test_part_3
)
