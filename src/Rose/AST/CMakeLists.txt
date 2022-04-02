add_library(RoseAST OBJECT
  cmdline.C graphviz.C checker.C fixupTraversal.C merge.C link.C delete.C share.C load.C consistency.C)

add_dependencies(RoseAST rosetta_generated)


########### install files ###############

install(FILES
  cmdline.h graphviz.h checker.h merge.h fixupTraversal.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/AST)
