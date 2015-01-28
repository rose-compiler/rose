add_library(rosePartitioner2
	OBJECT 	AddressUsageMap.C Attribute.C BasicBlock.C ControlFlowGraph.C
		DataBlock.C DataFlow.C Engine.C Exception.C Function.C
		FunctionCallGraph.C GraphViz.C InstructionProvider.C
		MayReturnAnalysis.C Modules.C ModulesElf.C
		ModulesM68k.C ModulesPe.C ModulesX86.C
		OwnedDataBlock.C Partitioner.C Semantics.C
		StackDeltaAnalysis.C Utility.C)

add_dependencies(rosePartitioner2 rosetta_generated generate_stringify)

install(FILES AddressUsageMap.h Attribute.h BasicBlock.h BasicTypes.h
	      ControlFlowGraph.h DataBlock.h DataFlow.h Engine.h
	      Exception.h Function.h FunctionCallGraph.h GraphViz.h
	      InstructionProvider.h Modules.h ModulesElf.h
	      ModulesM68k.h ModulesPe.h ModulesX86.h OwnedDataBlock.h
	      Partitioner.h Semantics.h Utility.h
 
  DESTINATION ${INCLUDE_INSTALL_DIR}/Partitioner2)
