add_library(roseBinaryAnalysisConcolic OBJECT
  ConcolicExecutor.C
  ConcreteExecutor.C
  Database.C
  ExecutionManager.C
  LinuxExecutor.C
  LinuxExitStatus.C
  LinuxTraceExecutor.C
  Specimen.C
  SystemCall.C
  TestCase.C
  TestSuite.C
  Utility.C)

add_dependencies(roseBinaryAnalysisConcolic rosetta_generated)

install(FILES
  BasicTypes.h
  ConcolicExecutor.h
  ConcreteExecutor.h
  Database.h
  ExecutionManager.h
  LinuxExecutor.h
  LinuxExitStatus.h
  LinuxTraceExecutor.h
  Specimen.h
  SystemCall.h
  TestCase.h
  TestSuite.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/Concolic)
