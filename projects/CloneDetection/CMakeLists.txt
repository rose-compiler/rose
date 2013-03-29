add_executable(cloneDetection cloneDetection.C)
target_link_libraries(cloneDetection ROSE_DLL ${Boost_LIBRARIES})

add_executable(getSubVariants getSubVariants.C)
target_link_libraries(getSubVariants ROSE_DLL ${Boost_LIBRARIES})
