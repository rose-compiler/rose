
add_library(roseSourceCodeAnalysis OBJECT
  Analyzer.C
  Visitor.C)

add_dependencies(roseSourceCodeAnalysis rosetta_generated)

install(FILES
  Analyzer.h Visitor.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/SourceCode/Analysis)
