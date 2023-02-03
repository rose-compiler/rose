add_subdirectory(Callback)
add_subdirectory(I386Linux)
add_subdirectory(M68kSystem)

add_library(roseBinaryAnalysisConcolic OBJECT
  Architecture.C
  ConcolicExecutor.C
  ConcreteExecutor.C
  ConcreteResult.C
  Database.C
  Emulation.C
  ExecutionEvent.C
  ExecutionLocation.C
  ExecutionManager.C
  InputVariables.C
  Settings.C
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
  Callback.h
  ConcolicExecutor.h
  ConcreteExecutor.h
  ConcreteResult.h
  Database.h
  Emulation.h
  ExecutionEvent.h
  ExecutionLocation.h
  ExecutionManager.h
  I386Linux.h
  InputVariables.h
  M68kSystem.h
  Settings.h
  SharedMemory.h
  Specimen.h
  SystemCall.h
  TestCase.h
  TestSuite.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/Concolic)
