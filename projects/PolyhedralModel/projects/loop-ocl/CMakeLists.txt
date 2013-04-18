add_executable(loop_ocl
  loop-ocl.cpp opencl-from-loops.cpp ../utils/rose-utils.cpp)
target_link_libraries(loop_ocl
  PolyModel PolyModelRoseKernel ROSE_DLL ${Boost_LIBRARIES})
