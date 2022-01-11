include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
AUTOMAKE_OPTIONS = nostdinc
AM_CPPFLAGS = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = librose_partial.la

librose_partial_la_SOURCES =			\
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

# Header files are installed from parent makefiles.
