add_subdirectory(M68kSystem)
add_subdirectory(I386Linux)

add_library(roseBinaryAnalysisConcolic OBJECT
  Architecture.C
  ConcolicExecutor.C
  ConcreteExecutor.C
  ConcreteExecutorResult.C
  Database.C
  ExecutionEvent.C
  ExecutionLocation.C
  ExecutionManager.C
  InputVariables.C
  LinuxExitStatus.C
  LinuxTraceConcrete.C
  LinuxTraceConcreteResult.C
  SharedMemory.C
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
  ConcreteExecutorResult.h
  Database.h
  ExecutionEvent.h
  ExecutionLocation.h
  ExecutionManager.h
  I386Linux.h
  InputVariables.h
  LinuxExitStatus.h
  LinuxTraceConcrete.h
  LinuxTraceConcreteResult.h
  M68kSystem.h
  SharedMemory.h
  Specimen.h
  SystemCall.h
  TestCase.h
  TestSuite.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/Concolic)
