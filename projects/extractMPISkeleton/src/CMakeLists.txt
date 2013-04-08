
include_directories(
  ${ROSE_SOURCE_DIR}/src/midend/programAnalysis/CallGraphAnalysis
  ${ROSE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing
  ${ROSE_SOURCE_DIR}/src/midend/programAnalysis/staticSingleAssignment
)

add_executable(extractMPISkeleton
  APIDepFinder.cc APIReader.cc APISpec.cc Outline.cc Utils.cc
  extractMPISkeleton.cc ssexpr.cc processPragmas.cc annotatePragmas.cc
)
target_link_libraries(extractMPISkeleton ROSE_DLL ${Boost_LIBRARIES})

