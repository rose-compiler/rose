add_executable(dotGenerator dotGenerator.C)
target_link_libraries(dotGenerator ROSE_DLL ${Boost_LIBRARIES})

add_executable(dotGeneratorWholeASTGraph dotGeneratorWholeASTGraph.C)
target_link_libraries(dotGeneratorWholeASTGraph ROSE_DLL ${Boost_LIBRARIES})
