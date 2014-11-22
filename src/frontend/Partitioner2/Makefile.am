include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
INCLUDES = $(ROSE_INCLUDES)

# Binary partitioner -- partitions instructions into basic blocks and
# functions.  Header files must be installed under the Partitioner2
# subdirectory and included as <Partitioner2/*.h> to avoid conflicts
# with other headers, thus the header file list is in the parent
# makefile.
noinst_LTLIBRARIES = libPartitioner.la

libPartitioner_la_SOURCES =			\
	AddressUsageMap.C			\
	Attribute.C				\
	BasicBlock.C				\
	ControlFlowGraph.C			\
	DataBlock.C				\
	Engine.C				\
	Exception.C				\
	Function.C				\
	FunctionCallGraph.C			\
	InstructionProvider.C			\
	MayReturnAnalysis.C			\
	Modules.C				\
	ModulesElf.C				\
	ModulesM68k.C				\
	ModulesPe.C				\
	ModulesX86.C				\
	OwnedDataBlock.C			\
	Partitioner.C				\
	Semantics.C				\
	StackDeltaAnalysis.C			\
	Utility.C
