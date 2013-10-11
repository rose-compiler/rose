
add_executable(ompChecker ompChecker.cpp ompCheckInterface.cpp)
target_link_libraries(ompChecker ROSE_DLL ${Boost_LIBRARIES})
