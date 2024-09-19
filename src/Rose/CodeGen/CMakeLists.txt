add_library(roseCodegen OBJECT Driver.C API.C Factory.C)

add_dependencies(roseCodegen rosetta_generated)

########### install files ###############

install(FILES
  Driver.h Object.h API.h API.txx Factory.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/CodeGen)

install(FILES
  factory/namespaces.txx factory/classes.txx factory/typedefs.txx factory/variables.txx factory/functions.txx
  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/CodeGen/factory)

install(FILES ../CodeGen.h DESTINATION ${INCLUDE_INSTALL_DIR}/Rose)
