
include_directories(
  ${ROSE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing
  ${ROSE_SOURCE_DIR}/src/midend/programAnalysis/CallGraphAnalysis
)

add_executable(mpiCodeMotion
  CallGraphQuery.C MotionInformation.C MPINonBlockingCodeMotion.C
  testMPINonBlockingCodeMotion.C
)
target_link_libraries(mpiCodeMotion ROSE_DLL ${Boost_LIBRARIES})
