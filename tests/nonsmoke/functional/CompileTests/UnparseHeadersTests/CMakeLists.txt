set(TestUnparseHeaders_SRCS
    TestUnparseHeaders.C
    UnparseHeadersTransformVisitor.C)
add_executable(TestUnparseHeaders ${TestUnparseHeaders_SRCS})
target_link_libraries(TestUnparseHeaders ROSE_DLL EDG ${link_with_libraries} )

set(TESTCODES
  test1/Simple1.C test2/Simple2.C test3/Simple3.C test4/Simple4.C
  test5/Simple5.C test9/Simple9.C test10/Simple10.C test11/Simple11.C
  test12/Simple12.C test13/Simple13.C test14/Simple14.C
)

set(TESTCODES_FOR_DEFAULT_UNPARSE_FOLDER test0/Simple0.C)

# Note that although all tests are run with the same -I paths, it is fine as
# long as we ensure that the file names inside the -I paths are unique for each
# test.
set(TEST_INCLUDE_PATHS -I${CMAKE_CURRENT_SOURCE_DIR}/test8/subdir)

set(ROSE_FLAGS --edg:no_warnings --edg:restrict -w
  -rose:verbose 1)

foreach(file_to_test ${TESTCODES})

  string(REGEX MATCH "(test[0-9]+)" TestDir "${file_to_test}")
  string(REPLACE "/" "_" testName "${file_to_test}")

  add_test(
    NAME prepare_${testName}
    COMMAND ${CMAKE_COMMAND} -E remove_directory
    ${CMAKE_CURRENT_BINARY_DIR}/${TestDir}_unparsedHeaders)
  set_tests_properties(prepare_${testName} PROPERTIES LABELS UNPARSEHEADERTEST)

  add_test(
    NAME ${testName}
    COMMAND TestUnparseHeaders ${TEST_INCLUDE_PATHS} -rose:unparseHeaderFiles
      -rose:unparseHeaderFilesRootFolder ${TestDir}_unparsedHeaders
      ${ROSE_FLAGS} -c ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})

  set_tests_properties(${testName} PROPERTIES DEPENDS prepare_${testName} LABELS UNPARSEHEADERTEST)

endforeach()

foreach(file_to_test ${TESTCODES_FOR_DEFAULT_UNPARSE_FOLDER})

  string(REGEX MATCH "(test[0-9]+)" TestDir "${file_to_test}")
  string(REPLACE "/" "_" testName "${file_to_test}")

  add_test(
    NAME prepare_${testName}
    COMMAND ${CMAKE_COMMAND} -E remove_directory
    ${CMAKE_CURRENT_BINARY_DIR}/${TestDir}_unparsedHeaders)
  set_tests_properties(prepare_${testName} PROPERTIES LABELS UNPARSEHEADERTEST)

  add_test(
    NAME ${testName}
    COMMAND TestUnparseHeaders -rose:unparseHeaderFiles ${ROSE_FLAGS}
      -c ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})

  set_tests_properties(${testName} PROPERTIES DEPENDS prepare_${testName} LABELS UNPARSEHEADERTEST)

endforeach()
