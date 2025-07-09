
add_library(roseSourceCodeCodegen OBJECT
  Driver.C
  API.C
  Factory.C
  SymbolScanner.C)

add_dependencies(roseSourceCodeCodegen rosetta_generated)

install(FILES
  Driver.h Object.h API.h API.txx Factory.h SymbolScanner.h SymbolScanner.txx
  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/SourceCode/CodeGen)

install(FILES
  factory/factory.txx factory/namespaces.txx factory/classes.txx factory/typedefs.txx factory/variables.txx factory/functions.txx
  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/SourceCode/CodeGen/factory)
