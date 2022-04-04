add_library(RoseAST OBJECT
  graphviz.C fixupTraversal.C io/merge.C io/link.C io/delete.C io/share.C io/load.C checker/consistency.C checker/checker.C)

add_dependencies(RoseAST rosetta_generated)


########### install files ###############

install(FILES
  cmdline.h io.h checker.h fixupTraversal.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/AST)
