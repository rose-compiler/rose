# Build the single statement to block normalization translator

include_directories(${CMAKE_SOURCE_DIR}/src/midend/programTransformation/extractFunctionArgumentsNormalization)
add_executable(normalizationTranslator NormalizationTranslator.C)
target_link_libraries(normalizationTranslator
  ROSE_DLL EDG ${link_with_libraries})

# This populates the list ROSE__CXX_TESTS
include(${CMAKE_SOURCE_DIR}/tests/nonsmoke/functional/CompileTests/Cxx_tests/Cxx_Testcodes.cmake)

set(TESTCODE_DIR ${CMAKE_SOURCE_DIR}/tests/nonsmoke/functional/CompileTests/Cxx_tests)

set(normalizationTranslator_SWITCHES
	--edg:no_warnings -w -rose:verbose 0 -I${TESTCODE_DIR}
  -I${CMAKE_SOURCE_DIR}/tests/nonsmoke/functional/CompileTests/A++Code)

foreach(file_to_test ${ROSE__CXX_TESTS})
  add_test(
    NAME normalizationTranslator_${file_to_test}
    COMMAND normalizationTranslator ${normalizationTranslator_SWITCHES}
      -c ${TESTCODE_DIR}/${file_to_test})
endforeach()
