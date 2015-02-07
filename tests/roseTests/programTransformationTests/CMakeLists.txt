#add_subdirectory(extractFunctionArgumentsTest)
#add_subdirectory(singleStatementToBlockNormalization)

#-------------------------------------------------------------------------------
# preTest

add_executable(preTest preTest.C)
target_link_libraries(preTest ROSE_DLL EDG ${link_with_libraries})

set(files_to_test pass1.C pass2.C pass3.C)
foreach(file_to_test ${files_to_test})
  add_test(
    NAME pre_${file_to_test}
    COMMAND preTest ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test}
  )
  add_test(
    NAME pre_${file_to_test}_verify
    COMMAND ${CMAKE_COMMAND} -E compare_files
      ${CMAKE_CURRENT_SOURCE_DIR}/rose_${file_to_test}.save
      ${CMAKE_CURRENT_BINARY_DIR}/rose_${file_to_test}
  )
  set_tests_properties(pre_${file_to_test}_verify
    PROPERTIES DEPENDS pre_${file_to_test})
endforeach()

#-------------------------------------------------------------------------------
# patternRewriteTest

add_executable(patternRewriteTest patternRewriteTest.C)
target_link_libraries(patternRewriteTest ROSE_DLL EDG ${link_with_libraries})

add_test(
  NAME pr_rewrite_test1.C
  COMMAND patternRewriteTest ${CMAKE_CURRENT_SOURCE_DIR}/rewrite_test1.C
)
add_test(
  NAME pr_rewrite_test1.C_verify
  COMMAND ${CMAKE_COMMAND} -E compare_files
    ${CMAKE_CURRENT_SOURCE_DIR}/rose_rewrite_test1.C.save
    ${CMAKE_CURRENT_BINARY_DIR}/rose_rewrite_test1.C
)
set_tests_properties(pr_rewrite_test1.C_verify
  PROPERTIES DEPENDS pr_rewrite_test1.C)

#-------------------------------------------------------------------------------
# finiteDifferencingTest

add_executable(finiteDifferencingTest finiteDifferencingTest.C)
target_link_libraries(finiteDifferencingTest
  ROSE_DLL EDG ${link_with_libraries})

set(files_to_test
  finitediff_test1.C finitediff_test2.C finitediff_test3.C)
foreach(file_to_test ${files_to_test})
  add_test(
    NAME fd_${file_to_test}
    COMMAND finiteDifferencingTest ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test}
  )
  add_test(
    NAME fd_${file_to_test}_verify
    COMMAND ${CMAKE_COMMAND} -E compare_files
      ${CMAKE_CURRENT_SOURCE_DIR}/rose_${file_to_test}.save
      ${CMAKE_CURRENT_BINARY_DIR}/rose_${file_to_test}
  )
  set_tests_properties(fd_${file_to_test}_verify
    PROPERTIES DEPENDS fd_${file_to_test})
endforeach()

#-------------------------------------------------------------------------------
# constantFoldingTest

add_executable(constantFoldingTest constantFoldingTest.C)
target_link_libraries(constantFoldingTest ROSE_DLL EDG ${link_with_libraries})

add_executable(constantFoldingTest2 constantFoldingTest2.C)
target_link_libraries(constantFoldingTest2 ROSE_DLL EDG ${link_with_libraries})

add_test(
  NAME constantFoldingTest_1
  COMMAND constantFoldingTest
    -c ${CMAKE_CURRENT_SOURCE_DIR}/inputCode_constantFoldingTransformation.C
)

add_test(
  NAME constantFoldingTest_2
  COMMAND constantFoldingTest2
    -c ${CMAKE_CURRENT_SOURCE_DIR}/inputCode_constantFoldingTransformation.C
	  -rose:output rose_inputCode_constantFoldingTransformation_1.C
)

#FIXME: Test is failing
#add_test(
#  NAME constantFoldingTest_3
#  COMMAND constantFoldingTest2
#    -c ${CMAKE_CURRENT_SOURCE_DIR}/inputCode_constantFoldingTransformation.C
#)

#-------------------------------------------------------------------------------
# implicitCodeGenerationTest

add_executable(implicitCodeGenerationTest implicitCodeGenerationTest.C)
target_link_libraries(implicitCodeGenerationTest
  ROSE_DLL EDG ${link_with_libraries})

set(icg_SWITCHES
  -icg:defaultFunctionGenerator -icg:shortCircuitingTransformation
  -icg:destructorCallAnnotator)

add_test(
  NAME icg_1
  COMMAND implicitCodeGenerationTest ${icg_SWITCHES}
    -c ${CMAKE_SOURCE_DIR}/tests/CompileTests/Cxx_tests/test2004_116.C
)

add_test(
  NAME icg_2
  COMMAND implicitCodeGenerationTest ${icg_SWITCHES}
    -c ${CMAKE_CURRENT_SOURCE_DIR}/inputCode_implicitCodeGenerationTest.C
)
