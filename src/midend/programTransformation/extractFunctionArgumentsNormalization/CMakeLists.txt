set(EXTRACT_FUNCTION_ARGUMENTS_NORMALIZATION_HEADERS ExtractFunctionArguments.h functionEvaluationOrderTraversal.h)
set(EXTRACT_FUNCTION_ARGUMENTS_NORMALIZATION_SOURCES ExtractFunctionArguments.C functionEvaluationOrderTraversal.C) 

add_library(ExtractFunctionArgumentsNormalization OBJECT ${EXTRACT_FUNCTION_ARGUMENTS_NORMALIZATION_SOURCES})
add_dependencies(ExtractFunctionArgumentsNormalization rosetta_generated)
target_include_directories(ExtractFunctionArgumentsNormalization PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/midend/programTransformation/singleStatementToBlockNormalization>
  $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>
)

target_link_libraries(ExtractFunctionArgumentsNormalization PRIVATE SingleStatementToBlockNormalization)

install(FILES ${EXTRACT_FUNCTION_ARGUMENTS_NORMALIZATION_HEADERS}
        DESTINATION ${INCLUDE_INSTALL_DIR}
)
