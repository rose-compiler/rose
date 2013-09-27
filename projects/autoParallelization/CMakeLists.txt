
add_executable(autoPar autoPar.C autoParSupport.C)
target_link_libraries(autoPar ROSE_DLL ${Boost_LIBRARIES})

