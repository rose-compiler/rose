set(tools_featureVector_exe_SOURCES featureVector.cpp)

add_executable(tools_featureVector_exe ${tools_featureVector_exe_SOURCES})

target_link_libraries(tools_featureVector_exe PRIVATE ROSE_DLL)

# TODO: When the refactor goes live, link libraries instead of doing this
# include hack.
target_include_directories(
  tools_featureVector_exe
  PRIVATE ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
          ${CMAKE_PREFIX_INSTALL}/include/rose ${ROSE_INCLUDES})

set_target_properties(tools_featureVector_exe PROPERTIES OUTPUT_NAME
                                                         "featureVector")

install(TARGETS tools_featureVector_exe DESTINATION bin)

add_dependencies(tools tools_featureVector_exe)
