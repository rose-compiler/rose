add_library(roseAST OBJECT
  NodeId.C
  utils/edge_ptr_repl.C
  Utility.C Traversal.C)

add_dependencies(roseAST rosetta_generated)

install(FILES
  NodeId.h Traversal.h Utility.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/AST)
