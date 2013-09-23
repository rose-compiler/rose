add_executable(pocc_post_processor pocc-post-processor.cpp rose-utils.cpp)
target_link_libraries(pocc_post_processor ROSE_DLL ${Boost_LIBRARIES})
