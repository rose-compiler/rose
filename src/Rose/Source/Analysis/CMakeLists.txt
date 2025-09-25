
add_library(roseSourceAnalysis OBJECT
  Analyzer.C
  Visitor.C)

add_dependencies(roseSourceAnalysis rosetta_generated)

install(FILES
  Analyzer.h Visitor.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/Source/Analysis)
