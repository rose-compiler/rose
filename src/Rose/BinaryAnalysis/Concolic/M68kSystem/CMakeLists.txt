add_library(roseBinaryAnalysisConcolicM68kSystem OBJECT
  Architecture.C
  TracingExecutor.C
  TracingResult.C)

add_dependencies(roseBinaryAnalysisConcolicM68kSystem rosetta_generated)

install(FILES
  Architecture.h
  BasicTypes.h
  TracingExecutor.h
  TracingResult.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/Concolic/M68kSystem)
