add_library(roseBinaryAnalysisInstructionSemantics2BaseSemantics OBJECT
  BaseSemantics.C
  Dispatcher.C
  Exception.C
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

add_dependencies(roseBinaryAnalysisInstructionSemantics2BaseSemantics rosetta_generated)

install(FILES
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
  Types.h
    
  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics)
