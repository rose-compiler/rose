add_library(astDiagnostics OBJECT
  AstConsistencyTests.C AstWarnings.C AstStatistics.C AstPerformance.C)
add_dependencies(astDiagnostics rosetta_generated generate_stringify)

########### install files ###############

install(FILES
  AstDiagnostics.h AstConsistencyTests.h AstWarnings.h AstStatistics.h
  AstPerformance.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
