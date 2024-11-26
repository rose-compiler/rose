add_library(roseBinaryAnalysisInstructionSemanticsBaseSemantics OBJECT
  AddressSpace.C								
  BaseSemantics.C
  Dispatcher.C
  Exception.C
  Formatter.C
  MemoryCell.C
  MemoryCellList.C
  MemoryCellMap.C
  MemoryCellState.C
  MemoryState.C
  Merger.C
  RegisterState.C
  RegisterStateGeneric.C
  RiscOperators.C
  State.C
  SValue.C
  SymbolicMemory.C
  )

add_dependencies(roseBinaryAnalysisInstructionSemanticsBaseSemantics rosetta_generated)

install(FILES
  AddressSpace.h
  BasicTypes.h
  Dispatcher.h
  Exception.h
  Formatter.h
  MemoryCell.h
  MemoryCellList.h
  MemoryCellMap.h
  MemoryCellState.h
  MemoryState.h
  Merger.h
  RegisterState.h
  RegisterStateGeneric.h
  RiscOperators.h
  State.h
  SValue.h
  SymbolicMemory.h
    
  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics)
