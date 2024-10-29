set(tools_classMemberVariablesInLambdas_exe_SOURCES
    classMemberVariablesInLambdas.C)

add_executable(tools_classMemberVariablesInLambdas_exe
               ${tools_classMemberVariablesInLambdas_exe_SOURCES})

target_link_libraries(tools_classMemberVariablesInLambdas_exe PRIVATE ROSE_DLL)

# TODO: When the refactor goes live, link libraries instead of doing this
# include hack.
target_include_directories(
  tools_classMemberVariablesInLambdas_exe
  PRIVATE ${CMAKE_BINARY_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}
          ${CMAKE_PREFIX_INSTALL}/include/rose
          ${ROSE_INCLUDES}
)

set_target_properties(tools_classMemberVariablesInLambdas_exe PROPERTIES OUTPUT_NAME "classMemberVariablesInLambdas")
install(TARGETS tools_classMemberVariablesInLambdas_exe DESTINATION bin)

add_dependencies(tools tools_classMemberVariablesInLambdas_exe)
