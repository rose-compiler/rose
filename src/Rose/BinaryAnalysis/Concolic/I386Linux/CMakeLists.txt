add_library(roseBinaryAnalysisConcolicI386Linux OBJECT
  Architecture.C
  ExitStatusExecutor.C
  ExitStatusResult.C
  TracingExecutor.C
  TracingResult.C)

add_dependencies(roseBinaryAnalysisConcolicI386Linux rosetta_generated)

install(FILES
  Architecture.h
  BasicTypes.h
  ExitStatusExecutor.h
  ExitStatusResult.h
  TracingExecutor.h
  TracingResult.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/Concolic/I386Linux)
