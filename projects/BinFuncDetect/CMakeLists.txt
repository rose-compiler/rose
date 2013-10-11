
include_directories(${ROSE_BINARY_DIR}/src/roseSupport)

add_executable(analyzer analyzer.C)
target_link_libraries(analyzer ROSE_DLL ${Boost_LIBRARIES})

if(HAVE_SYS_MMAN_H)
  add_executable(feasibility feasibility.C)
  target_link_libraries(feasibility ROSE_DLL ${Boost_LIBRARIES})
endif()
