
add_executable(graphColoringMain
  graphColoringMain.cpp interferenceGraph.cpp interferenceGraph.h
)

target_link_libraries(graphColoringMain ROSE_DLL ${Boost_LIBRARIES})
