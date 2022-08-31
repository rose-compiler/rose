add_subdirectory(BaseSemantics)

add_library(roseBinaryAnalysisInstructionSemantics OBJECT
  ConcreteSemantics.C
  DataFlowSemantics.C
  DispatcherAarch32.C
  DispatcherAarch64.C
  DispatcherCil.C
  DispatcherM68k.C
  DispatcherPowerpc.C
  DispatcherX86.C
  IntervalSemantics.C
  LlvmSemantics.C
  MultiSemantics.C
  NativeSemantics.C
  NullSemantics.C
  PartialSymbolicSemantics.C
  SourceAstSemantics.C
  StaticSemantics.C
  SymbolicSemantics.C
  TaintSemantics.C
  TraceSemantics.C
  Util.C)

add_dependencies(roseBinaryAnalysisInstructionSemantics rosetta_generated)

install(FILES
  BaseSemantics.h
  ConcreteSemantics.h
  DataFlowSemantics.h
  DispatcherAarch32.h
  DispatcherAarch64.h
  DispatcherCil.h
  DispatcherM68k.h
  DispatcherPowerpc.h
  DispatcherX86.h
  IntervalSemantics.h
  MultiSemantics.h
  NativeSemantics.h
  NullSemantics.h
  PartialSymbolicSemantics.h
  SourceAstSemantics.h
  StaticSemantics.h
  SymbolicSemantics.h
  TestSemantics.h
  TaintSemantics.h
  TraceSemantics.h
  Util.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/InstructionSemantics)
