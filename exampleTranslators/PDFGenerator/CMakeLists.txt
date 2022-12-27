add_executable(jsonGenerator jsonGenerator.C)
add_dependencies(jsonGenerator ROSE_DLL)
target_link_libraries(jsonGenerator ROSE_DLL ${Boost_LIBRARIES})
install(TARGETS jsonGenerator DESTINATION bin)
