
add_executable(taintcheck taintcheck.C)
target_link_libraries(taintcheck ROSE_DLL ${Boost_LIBRARIES})

