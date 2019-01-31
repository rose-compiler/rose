add_executable(astCopyReplTest astCopyReplTest.C)
add_dependencies(dotGenerator ROSE_DLL)
target_link_libraries(astCopyReplTest ROSE_DLL ${Boost_LIBRARIES})
install(TARGETS astCopyReplTest DESTINATION bin)
