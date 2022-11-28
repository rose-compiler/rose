add_library(roseBinaryAnalysisConcolicM68kSystem OBJECT
  Architecture.C
  ConcreteExecutor.C)

add_dependencies(roseBinaryAnalysisConcolicM68kSystem rosetta_generated)

install(FILES
  Architecture.h
  BasicTypes.h
  ConcreteExecutor.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/Concolic/M68kSystem)
