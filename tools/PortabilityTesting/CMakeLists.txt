#
# matrix_attachments
#
set(tools_matrix_attachments_exe_SOURCES ./src/matrix-attachments.C)

add_executable(tools_matrix_attachments_exe
               ${tools_matrix_attachments_exe_SOURCES})

target_link_libraries(tools_matrix_attachments_exe
                      PRIVATE ROSE_DLL tools_matrixTools_obj roseNamespace)

# TODO: When the refactor goes live, link libraries instead of doing this
# include hack.
target_include_directories(
  tools_matrix_attachments_exe
  PRIVATE ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_PREFIX_INSTALL}/include/rose
          ${ROSE_INCLUDES})
set_target_properties(tools_matrix_attachments_exe
                      PROPERTIES OUTPUT_NAME "matrix_attachments")

install(TARGETS tools_matrix_attachments_exe DESTINATION bin)

#
# matrix_dependencies
#
set(tools_matrix_dependencies_exe_SOURCES ./src/matrix-dependencies.C)

add_executable(tools_matrix_dependencies_exe
               ${tools_matrix_dependencies_exe_SOURCES})

target_link_libraries(tools_matrix_dependencies_exe
                      PRIVATE ROSE_DLL tools_matrixTools_obj)

# TODO: When the refactor goes live, link libraries instead of doing this
# include hack.
target_include_directories(
  tools_matrix_dependencies_exe
  PRIVATE ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_PREFIX_INSTALL}/include/rose
          ${ROSE_INCLUDES})
set_target_properties(tools_matrix_dependencies_exe
                      PROPERTIES OUTPUT_NAME "matrix_dependencies")

install(TARGETS tools_matrix_dependencies_exe DESTINATION bin)

#
# matrix_errors
#
set(tools_matrix_errors_exe_SOURCES ./src/matrix-errors.C)

add_executable(tools_matrix_errors_exe ${tools_matrix_errors_exe_SOURCES})

target_link_libraries(tools_matrix_errors_exe PRIVATE ROSE_DLL
                                                      tools_matrixTools_obj)

# TODO: When the refactor goes live, link libraries instead of doing this
# include hack.
target_include_directories(
  tools_matrix_errors_exe
  PRIVATE ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_PREFIX_INSTALL}/include/rose
          ${ROSE_INCLUDES})
set_target_properties(tools_matrix_errors_exe PROPERTIES OUTPUT_NAME
                                                         "matrix_errors")

install(TARGETS tools_matrix_errors_exe DESTINATION bin)

#
# matrix_next_test
#
set(tools_matrix_next_test_exe_SOURCES ./src/matrix-next-test.C)

add_executable(tools_matrix_next_test_exe ${tools_matrix_next_test_exe_SOURCES})

target_link_libraries(tools_matrix_next_test_exe PRIVATE ROSE_DLL
                                                         tools_matrixTools_obj)

# TODO: When the refactor goes live, link libraries instead of doing this
# include hack.
target_include_directories(
  tools_matrix_next_test_exe
  PRIVATE ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_PREFIX_INSTALL}/include/rose
          ${ROSE_INCLUDES})
set_target_properties(tools_matrix_next_test_exe PROPERTIES OUTPUT_NAME
                                                            "matrix_next_test")

install(TARGETS tools_matrix_next_test_exe DESTINATION bin)

#
# matrix-phases
#
set(tools_matrix-phases_exe_SOURCES ./src/matrix-phases.C)

add_executable(tools_matrix-phases_exe ${tools_matrix-phases_exe_SOURCES})

target_link_libraries(tools_matrix-phases_exe PRIVATE ROSE_DLL
                                                      tools_matrixTools_obj)

# TODO: When the refactor goes live, link libraries instead of doing this
# include hack.
target_include_directories(
  tools_matrix-phases_exe
  PRIVATE ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_PREFIX_INSTALL}/include/rose
          ${ROSE_INCLUDES})
set_target_properties(tools_matrix-phases_exe PROPERTIES OUTPUT_NAME
                                                         "matrix-phases")

install(TARGETS tools_matrix-phases_exe DESTINATION bin)

#
# matrix-query
#
set(tools_matrix-query_exe_SOURCES ./src/matrix-query.C)

add_executable(tools_matrix-query_exe ${tools_matrix-query_exe_SOURCES})

target_link_libraries(tools_matrix-query_exe PRIVATE ROSE_DLL
                                                     tools_matrixTools_obj)

# TODO: When the refactor goes live, link libraries instead of doing this
# include hack.
target_include_directories(
  tools_matrix-query_exe
  PRIVATE ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_PREFIX_INSTALL}/include/rose
          ${ROSE_INCLUDES})
set_target_properties(tools_matrix-query_exe PROPERTIES OUTPUT_NAME
                                                        "matrix-query")

install(TARGETS tools_matrix-query_exe DESTINATION bin)

#
# matrix-slave-config
#
set(tools_matrix-slave-config_exe_SOURCES ./src/matrix-slave-config.C)

add_executable(tools_matrix-slave-config_exe
               ${tools_matrix-slave-config_exe_SOURCES})

target_link_libraries(tools_matrix-slave-config_exe
                      PRIVATE ROSE_DLL tools_matrixTools_obj roseNamespace)

# TODO: When the refactor goes live, link libraries instead of doing this
# include hack.
target_include_directories(
  tools_matrix-slave-config_exe
  PRIVATE ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_PREFIX_INSTALL}/include/rose
          ${ROSE_INCLUDES})
set_target_properties(tools_matrix-slave-config_exe
                      PROPERTIES OUTPUT_NAME "matrix-slave-config")

install(TARGETS tools_matrix-slave-config_exe DESTINATION bin)

#
# matrix-slave-health
#
set(tools_matrix-slave-health_exe_SOURCES ./src/matrix-slave-health.C)

add_executable(tools_matrix-slave-health_exe
               ${tools_matrix-slave-health_exe_SOURCES})

target_link_libraries(tools_matrix-slave-health_exe
                      PRIVATE ROSE_DLL tools_matrixTools_obj)

# TODO: When the refactor goes live, link libraries instead of doing this
# include hack.
target_include_directories(
  tools_matrix-slave-health_exe
  PRIVATE ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_PREFIX_INSTALL}/include/rose
          ${ROSE_INCLUDES})
set_target_properties(tools_matrix-slave-health_exe
                      PROPERTIES OUTPUT_NAME "matrix-slave-health")

install(TARGETS tools_matrix-slave-health_exe DESTINATION bin)

#
# matrix-test-result
#
set(tools_matrix-test-result_exe_SOURCES ./src/matrix-test-result.C)

add_executable(tools_matrix-test-result_exe
               ${tools_matrix-test-result_exe_SOURCES})

target_link_libraries(tools_matrix-test-result_exe
                      PRIVATE ROSE_DLL tools_matrixTools_obj)

# TODO: When the refactor goes live, link libraries instead of doing this
# include hack.
target_include_directories(
  tools_matrix-test-result_exe
  PRIVATE ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_PREFIX_INSTALL}/include/rose
          ${ROSE_INCLUDES})
set_target_properties(tools_matrix-test-result_exe
                      PROPERTIES OUTPUT_NAME "matrix-test-result")

install(TARGETS tools_matrix-test-result_exe DESTINATION bin)

#
# matrixTools
#
set(tools_matrixTools_obj_HEADERS ./src/matrixTools.h)

set(tools_matrixTools_obj_SOURCES ./src/matrixTools.C)

add_library(tools_matrixTools_obj OBJECT)

target_sources(
  tools_matrixTools_obj
  PRIVATE ${tools_matrixTools_obj_SOURCES}
  PUBLIC ${tools_matrixTools_obj_HEADERS})

target_link_libraries(tools_matrixTools_obj PRIVATE ROSE_DLL util_sawyer)

# TODO: When the refactor goes live, link libraries instead of doing this
# include hack.
target_include_directories(
  tools_matrixTools_obj
  PRIVATE ${CMAKE_BINARY_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/src
          ${CMAKE_PREFIX_INSTALL}/include/rose
          ${CMAKE_PREFIX_INSTALL}/include/rose/Sawyer
          ${ROSE_INCLUDES}
          )


# Add the various tools to the custom build target.
add_dependencies(tools
  tools_matrix_attachments_exe
  tools_matrix_dependencies_exe
  tools_matrix_errors_exe
  tools_matrix_next_test_exe
  tools_matrix-phases_exe
  tools_matrix-query_exe
  tools_matrix-slave-config_exe
  tools_matrix-slave-health_exe
  tools_matrix-test-result_exe
  tools_matrixTools_obj
)
