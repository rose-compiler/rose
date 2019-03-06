include_directories(${ROSE_SOURCE_DIR}/src/roseExtensions/sqlite3x)
add_executable(compilerFlagsGui compilerFlagsMainGui.C compilerFlagsGui.C)
target_link_libraries(compilerFlagsGui ROSE_DLL ${Boost_LIBRARIES} qrose)
