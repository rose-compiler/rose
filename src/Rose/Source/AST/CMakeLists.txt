add_library(roseSourceAST OBJECT
  graphviz.C
  io/merge.C io/link.C io/delete.C io/share.C io/load.C
  checker/cmdline.C checker/checker.C
  checker/integrity_edges.C checker/integrity_declarations.C checker/integrity_symbols.C checker/integrity_types.C )

add_dependencies(roseSourceAST rosetta_generated)

install(FILES
  cmdline.h IO.h Checker.h Defect.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/Source/AST)
