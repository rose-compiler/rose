add_library(roseBinaryAnalysisPartitioner2 OBJECT
  AddressUsageMap.C BasicBlock.C CfgPath.C Configuration.C
  ControlFlowGraph.C DataBlock.C DataFlow.C Engine.C EngineBinary.C EngineJvm.C
  Exception.C Function.C FunctionCallGraph.C FunctionNoop.C GraphViz.C
  MayReturnAnalysis.C Modules.C ModulesElf.C ModulesJvm.C ModulesLinux.C ModulesM68k.C
  ModulesMips.C ModulesPe.C ModulesPowerpc.C ModulesX86.C Partitioner.C Reference.C
  Semantics.C StackDeltaAnalysis.C Thunk.C Utility.C)

add_dependencies(roseBinaryAnalysisPartitioner2 rosetta_generated)

install(FILES
  AddressUsageMap.h BasicBlock.h BasicTypes.h CfgPath.h
  Configuration.h ControlFlowGraph.h DataBlock.h DataFlow.h Engine.h EngineBinary.h
  EngineJvm.h Exception.h Function.h FunctionCallGraph.h GraphViz.h
  Modules.h ModulesElf.h ModulesJvm.h ModulesLinux.h ModulesM68k.h
  ModulesMips.h ModulesPe.h ModulesPowerpc.h ModulesX86.h Partitioner.h Reference.h
  Semantics.h Thunk.h Utility.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/Partitioner2)
