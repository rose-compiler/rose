# This Makefile is only for things in the ::Rose namespace. If
# you're adding something that's not in ::Rose you're doing it wrong!

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
AUTOMAKE_OPTIONS = nostdinc subdir-objects

SUBDIRS=AST CodeGen Traits

# LicenseString.h is included by CommandLine/License.C but is not installed.
#
CommandLine/LicenseString.h: $(top_srcdir)/LicenseInformation/ROSE_BSD_License.txt
	mkdir -p CommandLine
	perl $(top_srcdir)/scripts/to-c-string.pl < $< > $@
BUILT_SOURCES = CommandLine/LicenseString.h

# .C files for the ::Rose namespace (see top comment)
AM_CPPFLAGS = $(ROSE_INCLUDES)
noinst_LTLIBRARIES = librose_partial.la
librose_partial_la_SOURCES =								\
	BinaryAnalysis/AbstractLocation.C						\
	BinaryAnalysis/AsmFunctionIndex.C						\
	BinaryAnalysis/AstHash.C							\
	BinaryAnalysis/BestMapAddress.C							\
	BinaryAnalysis/BinaryLoader.C							\
	BinaryAnalysis/BinaryLoaderElf.C						\
	BinaryAnalysis/BinaryLoaderElfObj.C						\
	BinaryAnalysis/BinaryLoaderPe.C							\
	BinaryAnalysis/ByteCode/Analysis.C						\
	BinaryAnalysis/ByteCode/Jvm.C							\
	BinaryAnalysis/CallingConvention.C						\
	BinaryAnalysis/CodeInserter.C							\
	BinaryAnalysis/Concolic/Architecture.C						\
	BinaryAnalysis/Concolic/ConcolicExecutor.C					\
	BinaryAnalysis/Concolic/ConcreteExecutor.C					\
	BinaryAnalysis/Concolic/Database.C						\
	BinaryAnalysis/Concolic/ExecutionEvent.C					\
	BinaryAnalysis/Concolic/ExecutionLocation.C					\
	BinaryAnalysis/Concolic/ExecutionManager.C					\
	BinaryAnalysis/Concolic/InputVariables.C					\
	BinaryAnalysis/Concolic/LinuxExecutor.C						\
	BinaryAnalysis/Concolic/LinuxExitStatus.C					\
	BinaryAnalysis/Concolic/LinuxI386.C						\
	BinaryAnalysis/Concolic/LinuxTraceExecutor.C					\
	BinaryAnalysis/Concolic/SharedMemory.C						\
	BinaryAnalysis/Concolic/Specimen.C						\
	BinaryAnalysis/Concolic/SystemCall.C						\
	BinaryAnalysis/Concolic/TestCase.C						\
	BinaryAnalysis/Concolic/TestSuite.C						\
	BinaryAnalysis/Concolic/Utility.C						\
	BinaryAnalysis/ConcreteLocation.C						\
	BinaryAnalysis/ControlFlow.C							\
	BinaryAnalysis/DataFlow.C							\
	BinaryAnalysis/Debugger.C							\
	BinaryAnalysis/Demangler.C							\
	BinaryAnalysis/Disassembler/Aarch32.C						\
	BinaryAnalysis/Disassembler/Aarch64.C						\
	BinaryAnalysis/Disassembler/Base.C						\
	BinaryAnalysis/Disassembler/BasicTypes.C					\
	BinaryAnalysis/DisassemblerCil.C						\
	BinaryAnalysis/Disassembler/Exception.C						\
	BinaryAnalysis/Disassembler/Jvm.C						\
	BinaryAnalysis/Disassembler/M68k.C						\
	BinaryAnalysis/Disassembler/Mips.C						\
	BinaryAnalysis/Disassembler/Null.C						\
	BinaryAnalysis/Disassembler/Powerpc.C						\
	BinaryAnalysis/Disassembler/X86.C						\
	BinaryAnalysis/FeasiblePath.C							\
	BinaryAnalysis/FunctionCall.C							\
	BinaryAnalysis/FunctionSimilarity.C						\
	BinaryAnalysis/HotPatch.C							\
	BinaryAnalysis/InstructionCache.C						\
	BinaryAnalysis/InstructionSemantics/BaseSemantics/BaseSemantics.C		\
	BinaryAnalysis/InstructionSemantics/BaseSemantics/BinaryAnalysis		\
	BinaryAnalysis/InstructionSemantics/BaseSemantics/Dispatcher.C			\
	BinaryAnalysis/InstructionSemantics/BaseSemantics/Exception.C			\
	BinaryAnalysis/InstructionSemantics/BaseSemantics/Formatter.C			\
	BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCell.C			\
	BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCellList.C		\
	BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCellMap.C		\
	BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCellState.C		\
	BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryState.C			\
	BinaryAnalysis/InstructionSemantics/BaseSemantics/Merger.C			\
	BinaryAnalysis/InstructionSemantics/BaseSemantics/RegisterState.C		\
	BinaryAnalysis/InstructionSemantics/BaseSemantics/RegisterStateGeneric.C	\
	BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.C		\
	BinaryAnalysis/InstructionSemantics/BaseSemantics/State.C			\
	BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.C			\
	BinaryAnalysis/InstructionSemantics/BaseSemantics/SymbolicMemory.C		\
	BinaryAnalysis/InstructionSemantics/ConcreteSemantics.C				\
	BinaryAnalysis/InstructionSemantics/DataFlowSemantics.C				\
	BinaryAnalysis/InstructionSemantics/DispatcherAarch32.C				\
	BinaryAnalysis/InstructionSemantics/DispatcherAarch64.C				\
	BinaryAnalysis/InstructionSemantics/DispatcherCil.C				\
	BinaryAnalysis/InstructionSemantics/DispatcherM68k.C				\
	BinaryAnalysis/InstructionSemantics/DispatcherPowerpc.C				\
	BinaryAnalysis/InstructionSemantics/DispatcherX86.C				\
	BinaryAnalysis/InstructionSemantics/IntervalSemantics.C				\
	BinaryAnalysis/InstructionSemantics/LlvmSemantics.C				\
	BinaryAnalysis/InstructionSemantics/MultiSemantics.C				\
	BinaryAnalysis/InstructionSemantics/NativeSemantics.C				\
	BinaryAnalysis/InstructionSemantics/NullSemantics.C				\
	BinaryAnalysis/InstructionSemantics/PartialSymbolicSemantics.C			\
	BinaryAnalysis/InstructionSemantics/SourceAstSemantics.C			\
	BinaryAnalysis/InstructionSemantics/StaticSemantics.C				\
	BinaryAnalysis/InstructionSemantics/SymbolicSemantics.C				\
	BinaryAnalysis/InstructionSemantics/TaintSemantics.C				\
	BinaryAnalysis/InstructionSemantics/TraceSemantics.C				\
	BinaryAnalysis/InstructionSemantics/Utility.C					\
	BinaryAnalysis/LibraryIdentification.C						\
	BinaryAnalysis/MagicNumber.C							\
	BinaryAnalysis/MemoryMap.C							\
	BinaryAnalysis/ModelChecker/BasicBlockUnit.C					\
	BinaryAnalysis/ModelChecker/Engine.C						\
	BinaryAnalysis/ModelChecker/ErrorTag.C						\
	BinaryAnalysis/ModelChecker/ExecutionUnit.C					\
	BinaryAnalysis/ModelChecker/ExternalFunctionUnit.C				\
	BinaryAnalysis/ModelChecker/FailureUnit.C					\
	BinaryAnalysis/ModelChecker/InstructionUnit.C					\
	BinaryAnalysis/ModelChecker/NullDereferenceTag.C				\
	BinaryAnalysis/ModelChecker/OutOfBoundsTag.C					\
	BinaryAnalysis/ModelChecker/PartitionerModel.C					\
	BinaryAnalysis/ModelChecker/Path.C						\
	BinaryAnalysis/ModelChecker/PathNode.C						\
	BinaryAnalysis/ModelChecker/PathPredicate.C					\
	BinaryAnalysis/ModelChecker/PathPrioritizer.C					\
	BinaryAnalysis/ModelChecker/PathQueue.C						\
	BinaryAnalysis/ModelChecker/Periodic.C						\
	BinaryAnalysis/ModelChecker/SemanticCallbacks.C					\
	BinaryAnalysis/ModelChecker/Settings.C						\
	BinaryAnalysis/ModelChecker/SourceLister.C					\
	BinaryAnalysis/ModelChecker/Tag.C						\
	BinaryAnalysis/ModelChecker/UninitializedVariableTag.C				\
	BinaryAnalysis/ModelChecker/WorkerStatus.C					\
	BinaryAnalysis/NoOperation.C							\
	BinaryAnalysis/Partitioner2/AddressUsageMap.C					\
	BinaryAnalysis/Partitioner2/BasicBlock.C					\
	BinaryAnalysis/Partitioner2/CfgPath.C						\
	BinaryAnalysis/Partitioner2/Configuration.C					\
	BinaryAnalysis/Partitioner2/ControlFlowGraph.C					\
	BinaryAnalysis/Partitioner2/DataBlock.C						\
	BinaryAnalysis/Partitioner2/DataFlow.C						\
	BinaryAnalysis/Partitioner2/Engine.C						\
	BinaryAnalysis/Partitioner2/Exception.C						\
	BinaryAnalysis/Partitioner2/Function.C						\
	BinaryAnalysis/Partitioner2/FunctionCallGraph.C					\
	BinaryAnalysis/Partitioner2/FunctionNoop.C					\
	BinaryAnalysis/Partitioner2/GraphViz.C						\
	BinaryAnalysis/Partitioner2/InstructionProvider.C				\
	BinaryAnalysis/Partitioner2/JvmEngine.C						\
	BinaryAnalysis/Partitioner2/MayReturnAnalysis.C					\
	BinaryAnalysis/Partitioner2/Modules.C						\
	BinaryAnalysis/Partitioner2/ModulesElf.C					\
	BinaryAnalysis/Partitioner2/ModulesLinux.C					\
	BinaryAnalysis/Partitioner2/ModulesM68k.C					\
	BinaryAnalysis/Partitioner2/ModulesMips.C					\
	BinaryAnalysis/Partitioner2/ModulesPe.C						\
	BinaryAnalysis/Partitioner2/ModulesPowerpc.C					\
	BinaryAnalysis/Partitioner2/ModulesX86.C					\
	BinaryAnalysis/Partitioner2/ParallelPartitioner.C				\
	BinaryAnalysis/Partitioner2/Partitioner.C					\
	BinaryAnalysis/Partitioner2/Reference.C						\
	BinaryAnalysis/Partitioner2/Semantics.C						\
	BinaryAnalysis/Partitioner2/StackDeltaAnalysis.C				\
	BinaryAnalysis/Partitioner2/Thunk.C						\
	BinaryAnalysis/Partitioner2/Utility.C						\
	BinaryAnalysis/PointerDetection.C						\
	BinaryAnalysis/Reachability.C							\
	BinaryAnalysis/RegisterDescriptor.C						\
	BinaryAnalysis/RegisterDictionary.C						\
	BinaryAnalysis/RegisterNames.C							\
	BinaryAnalysis/RegisterParts.C							\
	BinaryAnalysis/ReturnValueUsed.C						\
	BinaryAnalysis/SerialIo.C							\
	BinaryAnalysis/SmtCommandLine.C							\
	BinaryAnalysis/SmtlibSolver.C							\
	BinaryAnalysis/SmtSolver.C							\
	BinaryAnalysis/SourceLocations.C						\
	BinaryAnalysis/SRecord.C							\
	BinaryAnalysis/StackDelta.C							\
	BinaryAnalysis/String.C								\
	BinaryAnalysis/SymbolicExpr.C							\
	BinaryAnalysis/SymbolicExpressionParser.C					\
	BinaryAnalysis/SystemCall.C							\
	BinaryAnalysis/TaintedFlow.C							\
	BinaryAnalysis/ToSource.C							\
	BinaryAnalysis/Unparser/Aarch32.C						\
	BinaryAnalysis/Unparser/Aarch64.C						\
	BinaryAnalysis/Unparser/Base.C							\
	BinaryAnalysis/Unparser/Cil.C							\
	BinaryAnalysis/Unparser/EdgeArrows.C						\
	BinaryAnalysis/Unparser/Jvm.C							\
	BinaryAnalysis/Unparser/M68k.C							\
	BinaryAnalysis/Unparser/Mips.C							\
	BinaryAnalysis/Unparser/Null.C							\
	BinaryAnalysis/Unparser/Powerpc.C						\
	BinaryAnalysis/Unparser/X86.C							\
	BinaryAnalysis/Variables.C							\
	BinaryAnalysis/Z3Solver.C							\
	Color/Color.C									\
	CommandLine/DurationParser.C							\
	CommandLine/License.C								\
	CommandLine/Parser.C								\
	CommandLine/SelfTest.C								\
	CommandLine/Version.C								\
	Diagnostics/Diagnostics.C							\
	FileSystem/FileSystem.C								\
	FormattedTable.C								\
	Initialize.C									\
	Progress.C									\
	SourceLocation.C								\
	StringUtility/Convert.C								\
	StringUtility/Diagnostics.C							\
	StringUtility/Escape.C								\
	StringUtility/NumberToString.C							\
	StringUtility/Predicate.C							\
	StringUtility/SplitJoin.C							\
	StringUtility/StringToNumber.C							\
	Yaml.C

rose_metaprog_h_includedir = $(prefix)/include/Rose
rose_metaprog_h_include_HEADERS = metaprog.h

# Public header files are installed from parent makefiles.
