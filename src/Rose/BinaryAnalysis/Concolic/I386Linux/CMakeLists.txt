add_library(roseBinaryAnalysisConcolicI386Linux OBJECT
  Architecture.C
  ConcreteExecutor.C)

add_dependencies(roseBinaryAnalysisConcolicI386Linux rosetta_generated)

install(FILES
  Architecture.h
  BasicTypes.h
  ConcreteExecutor.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/Concolic/I386Linux)
