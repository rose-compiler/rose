add_executable(polygrapher polygrapher.cpp parsetree.cpp polygraph.cpp)
target_link_libraries(polygrapher
  PolyModel.la PolyModelRoseKernel ROSE_DLL ${Boost_LIBRARIES})
