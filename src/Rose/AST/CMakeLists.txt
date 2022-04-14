add_library(RoseAST OBJECT
  graphviz.C
  io/merge.C io/link.C io/delete.C io/share.C io/load.C
  checker/cmdline.C checker/checker.C
  checker/integrity_edges.C checker/integrity_declarations.C checker/integrity_symbols.C checker/integrity_types.C
  utils/edge_ptr_repl.C)

add_dependencies(RoseAST rosetta_generated)


########### install files ###############

install(FILES ../AST.h DESTINATION ${INCLUDE_INSTALL_DIR}/Rose)

install(FILES
  cmdline.h IO.h Checker.h Defect.h Utils.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/AST)
