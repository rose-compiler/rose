add_subdirectory(BaseSemantics)

add_library(roseBinaryAnalysisInstructionSemantics2 OBJECT
  ConcreteSemantics.C
  DataFlowSemantics.C
  DispatcherAarch32.C
  DispatcherAarch64.C
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
  TraceSemantics.C
  Util.C)

add_dependencies(roseBinaryAnalysisInstructionSemantics2 rosetta_generated)

install(FILES
  BaseSemantics.h
  BaseSemanticsDispatcher.h
  BaseSemanticsException.h
  BaseSemanticsFormatter.h
  BaseSemanticsMemoryState.h
  BaseSemanticsMerger.h
  BaseSemanticsRegisterState.h
  BaseSemanticsRiscOperators.h
  BaseSemanticsState.h
  BaseSemanticsSValue.h
  BaseSemanticsTypes.h
  ConcreteSemantics.h
  DataFlowSemantics.h
  DispatcherAarch32.h
  DispatcherAarch64.h
  DispatcherM68k.h
  DispatcherPowerpc.h
  DispatcherX86.h
  IntervalSemantics.h
  MemoryCell.h
  MemoryCellList.h
  MemoryCellMap.h
  MemoryCellState.h
  MultiSemantics.h
  NativeSemantics.h
  NullSemantics.h
  PartialSymbolicSemantics.h
  RegisterStateGeneric.h
  SourceAstSemantics.h
  StaticSemantics.h
  SymbolicMemory.h
  SymbolicSemantics.h
  TestSemantics.h
  TraceSemantics.h
  Util.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/InstructionSemantics2)
