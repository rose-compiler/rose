
add_subdirectory(Analysis)
add_subdirectory(AST)
add_subdirectory(Generation)

add_library(roseSource OBJECT
  Sarif.C)

add_dependencies(roseSource rosetta_generated)

install(FILES
  Analysis.h AST.h Generation.h Sarif.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/Source)
