if(NOT WIN32)
  add_executable(jacobi jacobi.C highresTimer.C)
  target_link_libraries(jacobi ROSE_DLL ${Boost_LIBRARIES} ${RT_LIB})

  add_executable(testPermutations testPermutations.C highresTimer.C)
  target_link_libraries(testPermutations ROSE_DLL ${Boost_LIBRARIES} ${RT_LIB})

  add_executable(hypercube hypercube.C)
  target_link_libraries(hypercube ROSE_DLL ${Boost_LIBRARIES} ${RT_LIB})
endif()
