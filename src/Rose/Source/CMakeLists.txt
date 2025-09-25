
add_subdirectory(Analysis)
add_subdirectory(AST)
add_subdirectory(CodeGen)

add_library(roseSource OBJECT
  Sarif.C)

add_dependencies(roseSource rosetta_generated)

install(FILES
  Analysis.h AST.h CodeGen.h Sarif.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/Source)
