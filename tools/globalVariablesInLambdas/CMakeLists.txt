set(tools_globalVariablesInLambdas_exe_SOURCES globalVariablesInLambdas.C)

add_executable(tools_globalVariablesInLambdas_exe
               ${tools_globalVariablesInLambdas_exe_SOURCES})

target_link_libraries(tools_globalVariablesInLambdas_exe PRIVATE ROSE_DLL)

# TODO: When the refactor goes live, link libraries instead of doing this
# include hack.
target_include_directories(
  tools_globalVariablesInLambdas_exe
  PRIVATE ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
          ${CMAKE_PREFIX_INSTALL}/include/rose ${ROSE_INCLUDES})

set_target_properties(tools_globalVariablesInLambdas_exe
                      PROPERTIES OUTPUT_NAME "globalVariablesInLambdas")

install(TARGETS tools_globalVariablesInLambdas_exe DESTINATION bin)

add_dependencies(tools tools_globalVariablesInLambdas_exe)
