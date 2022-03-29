add_library(RoseAST OBJECT
  cmdline.C graphviz.C checker.C)

add_dependencies(RoseAST rosetta_generated)


########### install files ###############

install(FILES
  cmdline.h graphviz.h checker.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/AST)
