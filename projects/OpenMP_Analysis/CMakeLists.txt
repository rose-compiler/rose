
add_executable(ompAnalyzer ompAnalyzer.C)
target_link_libraries(ompAnalyzer ROSE_DLL ${Boost_LIBRARIES})
