add_library(roseBinaryAnalysisConcolic OBJECT
  ConcolicExecutor.C
  ConcreteExecutor.C
  Database.C
  ExecutionEvent.C
  ExecutionManager.C
  LinuxExecutor.C
  LinuxExitStatus.C
  LinuxI386Executor.C
  LinuxTraceExecutor.C
  Specimen.C
  TestCase.C
  TestSuite.C
  Utility.C)

add_dependencies(roseBinaryAnalysisConcolic rosetta_generated)

install(FILES
  BasicTypes.h
  ConcolicExecutor.h
  ConcreteExecutor.h
  Database.h
  ExecutionEvent.h
  ExecutionManager.h
  LinuxExecutor.h
  LinuxExitStatus.h
  LinuxI386Executor.h
  LinuxTraceExecutor.h
  Specimen.h
  TestCase.h
  TestSuite.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/Concolic)
