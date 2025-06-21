add_subdirectory(BaseSemantics)

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
  LlvmSemantics.h
  MultiSemantics.h
  NativeSemantics.h
  NullSemantics.h
  PartialSymbolicSemantics.h
  SourceAstSemantics.h
  StaticSemantics.h
  SymbolicSemantics.h
  TestSemantics.h
  TraceSemantics.h
  Util.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/InstructionSemantics2)
