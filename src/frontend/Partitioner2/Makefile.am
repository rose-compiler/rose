include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
AM_CPPFLAGS = $(ROSE_INCLUDES)

# Binary partitioner -- partitions instructions into basic blocks and
# functions.  Header files must be installed under the Partitioner2
# subdirectory and included as <Partitioner2/*.h> to avoid conflicts
# with other headers, thus the header file list is in the parent
# makefile.
noinst_LTLIBRARIES = libPartitioner.la

libPartitioner_la_SOURCES =			\
	AddressUsageMap.C			\
	BasicBlock.C				\
	CfgPath.C				\
	Config.C				\
	ControlFlowGraph.C			\
	DataBlock.C				\
	DataFlow.C				\
	Engine.C				\
	Exception.C				\
	Function.C				\
	FunctionCallGraph.C			\
	FunctionNoop.C				\
	GraphViz.C				\
	InstructionProvider.C			\
	MayReturnAnalysis.C			\
	Modules.C				\
	ModulesElf.C				\
	ModulesLinux.C				\
	ModulesM68k.C				\
	ModulesMips.C				\
	ModulesPe.C				\
	ModulesPowerpc.C			\
	ModulesX86.C				\
	ParallelPartitioner.C			\
	Partitioner.C				\
	Reference.C				\
	Semantics.C				\
	StackDeltaAnalysis.C			\
	Thunk.C					\
	Utility.C
