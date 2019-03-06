include_directories(
  ${ROSE_SOURCE_DIR}/src/midend/programAnalysis/CallGraphAnalysis
  ${ROSE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing
  ${ROSE_SOURCE_DIR}/src/midend/programAnalysis/staticSingleAssignment
)

add_executable(extractMPISkeleton
  extractMPISkeleton.cc APIReader.cc APIReader.h APISpec.cc APISpec.h
  GenericDepAttrib.cc GenericDepAttrib.h APIDepAttrib.h APIDepFinder.cc
  APIDepFinder.h SignatureReader.cc SignatureReader.h ssexpr.cc ssexpr.h
  DangerousOperationFinder.h Utils.cc Utils.h APIDepChecker.h Outline.cc
  Outline.h processPragmas.cc processPragmas.h annotatePragmas.cc
  annotatePragmas.h)
target_link_libraries(extractMPISkeleton ROSE_DLL ${Boost_LIBRARIES})


add_executable(generateSignatures
  generateSignatures.cc APISpec.cc APISpec.h GenericDepAttrib.cc
  GenericDepAttrib.h APIDepAttrib.h APIReader.cc APIReader.h APIDepFinder.cc
  APIDepFinder.h SignatureReader.cc SignatureReader.h ssexpr.cc ssexpr.h
  DangerousOperationFinder.h Utils.cc Utils.h APIDepChecker.h processPragmas.cc
  processPragmas.h annotatePragmas.cc annotatePragmas.h)
target_link_libraries(generateSignatures ROSE_DLL ${Boost_LIBRARIES})

add_executable(summarizeSignatures
  summarizeSignatures.cc ssexpr.cc ssexpr.h APISpec.cc APISpec.h
  GenericDepAttrib.cc GenericDepAttrib.h APIDepAttrib.h APIReader.cc
  APIReader.h SignatureReader.cc SignatureReader.h)
target_link_libraries(summarizeSignatures ROSE_DLL ${Boost_LIBRARIES})
