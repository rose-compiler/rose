add_executable(dotGenerator dotGenerator.C)
add_dependencies(dotGenerator ROSE_DLL)
target_link_libraries(dotGenerator ROSE_DLL ${Boost_LIBRARIES})
install(TARGETS dotGenerator DESTINATION bin)

add_executable(dotGeneratorWholeASTGraph dotGeneratorWholeASTGraph.C)
add_dependencies(dotGeneratorWholeASTGraph ROSE_DLL)
target_link_libraries(dotGeneratorWholeASTGraph ROSE_DLL ${Boost_LIBRARIES})
install(TARGETS dotGeneratorWholeASTGraph DESTINATION bin)
