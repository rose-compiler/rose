
add_executable(testMpiDeterminismAnalysis
  MpiDeterminismAnalysis.cpp TestMpiDeterminismAnalysis.cpp
)
target_link_libraries( testMpiDeterminismAnalysis ROSE_DLL ${Boost_LIBRARIES})

