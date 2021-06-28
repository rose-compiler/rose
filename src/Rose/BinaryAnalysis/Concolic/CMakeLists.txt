add_library(roseBinaryAnalysisConcolic OBJECT
  Architecture.C
  ConcolicExecutor.C
  ConcreteExecutor.C
  Database.C
  ExecutionEvent.C
  ExecutionManager.C
  InputVariables.C
  LinuxExecutor.C
  LinuxExitStatus.C
  LinuxI386.C
  LinuxTraceExecutor.C
  Specimen.C
  SystemCall.C
  TestCase.C
  TestSuite.C
  Utility.C)

add_dependencies(roseBinaryAnalysisConcolic rosetta_generated)

install(FILES
  Architecture.h
  BasicTypes.h
  ConcolicExecutor.h
  ConcreteExecutor.h
  Database.h
  ExecutionEvent.h
  ExecutionManager.h
  InputVariables.h
  LinuxExecutor.h
  LinuxExitStatus.h
  LinuxI386.h
  LinuxTraceExecutor.h
  Specimen.h
  SystemCall.h
  TestCase.h
  TestSuite.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/Concolic)
