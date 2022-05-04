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
	BinaryAnalysis/ByteCode/Jvm.C							\
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
	BinaryAnalysis/InstructionSemantics2/BaseSemantics/BaseSemantics.C		\
	BinaryAnalysis/InstructionSemantics2/BaseSemantics/Dispatcher.C			\
	BinaryAnalysis/InstructionSemantics2/BaseSemantics/Exception.C			\
	BinaryAnalysis/InstructionSemantics2/BaseSemantics/MemoryCell.C			\
	BinaryAnalysis/InstructionSemantics2/BaseSemantics/MemoryCellList.C		\
	BinaryAnalysis/InstructionSemantics2/BaseSemantics/MemoryCellMap.C		\
	BinaryAnalysis/InstructionSemantics2/BaseSemantics/MemoryCellState.C		\
	BinaryAnalysis/InstructionSemantics2/BaseSemantics/MemoryState.C		\
	BinaryAnalysis/InstructionSemantics2/BaseSemantics/Merger.C			\
	BinaryAnalysis/InstructionSemantics2/BaseSemantics/RegisterState.C		\
	BinaryAnalysis/InstructionSemantics2/BaseSemantics/RegisterStateGeneric.C	\
	BinaryAnalysis/InstructionSemantics2/BaseSemantics/RiscOperators.C		\
	BinaryAnalysis/InstructionSemantics2/BaseSemantics/State.C			\
	BinaryAnalysis/InstructionSemantics2/BaseSemantics/SValue.C			\
	BinaryAnalysis/InstructionSemantics2/BaseSemantics/SymbolicMemory.C		\
	BinaryAnalysis/InstructionSemantics2/BaseSemantics/BinaryAnalysis		\
	BinaryAnalysis/InstructionSemantics2/ConcreteSemantics.C			\
	BinaryAnalysis/InstructionSemantics2/DataFlowSemantics.C			\
	BinaryAnalysis/InstructionSemantics2/DispatcherAarch32.C			\
	BinaryAnalysis/InstructionSemantics2/DispatcherAarch64.C			\
	BinaryAnalysis/InstructionSemantics2/DispatcherM68k.C				\
	BinaryAnalysis/InstructionSemantics2/DispatcherPowerpc.C			\
	BinaryAnalysis/InstructionSemantics2/DispatcherX86.C				\
	BinaryAnalysis/InstructionSemantics2/IntervalSemantics.C			\
	BinaryAnalysis/InstructionSemantics2/LlvmSemantics.C				\
	BinaryAnalysis/InstructionSemantics2/MultiSemantics.C				\
	BinaryAnalysis/InstructionSemantics2/NativeSemantics.C				\
	BinaryAnalysis/InstructionSemantics2/NullSemantics.C				\
	BinaryAnalysis/InstructionSemantics2/PartialSymbolicSemantics.C			\
	BinaryAnalysis/InstructionSemantics2/SourceAstSemantics.C			\
	BinaryAnalysis/InstructionSemantics2/StaticSemantics.C				\
	BinaryAnalysis/InstructionSemantics2/SymbolicSemantics.C			\
	BinaryAnalysis/InstructionSemantics2/TraceSemantics.C				\
	BinaryAnalysis/InstructionSemantics2/Util.C					\
	BinaryAnalysis/ModelChecker/BasicBlockUnit.C					\
	BinaryAnalysis/ModelChecker/Engine.C						\
	BinaryAnalysis/ModelChecker/ErrorTag.C						\
	BinaryAnalysis/ModelChecker/ExecutionUnit.C					\
	BinaryAnalysis/ModelChecker/ExternalFunctionUnit.C				\
	BinaryAnalysis/ModelChecker/FailureUnit.C					\
	BinaryAnalysis/ModelChecker/InstructionUnit.C					\
	BinaryAnalysis/ModelChecker/NullDerefTag.C					\
	BinaryAnalysis/ModelChecker/OobTag.C						\
	BinaryAnalysis/ModelChecker/P2Model.C						\
	BinaryAnalysis/ModelChecker/Path.C						\
	BinaryAnalysis/ModelChecker/PathNode.C						\
	BinaryAnalysis/ModelChecker/PathPredicate.C					\
	BinaryAnalysis/ModelChecker/PathPrioritizer.C					\
	BinaryAnalysis/ModelChecker/PathQueue.C						\
	BinaryAnalysis/ModelChecker/Periodic.C						\
	BinaryAnalysis/ModelChecker/Settings.C						\
	BinaryAnalysis/ModelChecker/SemanticCallbacks.C					\
	BinaryAnalysis/ModelChecker/SourceLister.C					\
	BinaryAnalysis/ModelChecker/Tag.C						\
	BinaryAnalysis/ModelChecker/UninitVarTag.C					\
	BinaryAnalysis/ModelChecker/WorkerStatus.C					\
	BinaryAnalysis/Partitioner2/AddressUsageMap.C					\
	BinaryAnalysis/Partitioner2/BasicBlock.C					\
	BinaryAnalysis/Partitioner2/CfgPath.C						\
	BinaryAnalysis/Partitioner2/Config.C						\
	BinaryAnalysis/Partitioner2/ControlFlowGraph.C					\
	BinaryAnalysis/Partitioner2/DataBlock.C						\
	BinaryAnalysis/Partitioner2/DataFlow.C						\
	BinaryAnalysis/Partitioner2/Engine.C						\
	BinaryAnalysis/Partitioner2/JvmEngine.C						\
	BinaryAnalysis/Partitioner2/Exception.C						\
	BinaryAnalysis/Partitioner2/Function.C						\
	BinaryAnalysis/Partitioner2/FunctionCallGraph.C					\
	BinaryAnalysis/Partitioner2/FunctionNoop.C					\
	BinaryAnalysis/Partitioner2/GraphViz.C						\
	BinaryAnalysis/Partitioner2/InstructionProvider.C				\
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
	BinaryAnalysis/Unparser/EdgeArrows.C						\
	BinaryAnalysis/Unparser/Aarch32.C						\
	BinaryAnalysis/Unparser/Aarch64.C						\
	BinaryAnalysis/Unparser/Base.C							\
	BinaryAnalysis/Unparser/Jvm.C							\
	BinaryAnalysis/Unparser/M68k.C							\
	BinaryAnalysis/Unparser/Mips.C							\
	BinaryAnalysis/Unparser/Null.C							\
	BinaryAnalysis/Unparser/Powerpc.C						\
	BinaryAnalysis/Unparser/X86.C							\
	BinaryAnalysis/AbstractLocation.C						\
	BinaryAnalysis/AsmFunctionIndex.C						\
	BinaryAnalysis/AstHash.C							\
	BinaryAnalysis/BestMapAddress.C							\
	BinaryAnalysis/BinaryLoader.C							\
	BinaryAnalysis/BinaryLoaderElf.C						\
	BinaryAnalysis/BinaryLoaderElfObj.C						\
	BinaryAnalysis/BinaryLoaderPe.C							\
	BinaryAnalysis/CallingConvention.C						\
	BinaryAnalysis/CodeInserter.C							\
	BinaryAnalysis/ControlFlow.C							\
	BinaryAnalysis/DataFlow.C							\
	BinaryAnalysis/Debugger.C							\
	BinaryAnalysis/Demangler.C							\
	BinaryAnalysis/Disassembler.C							\
	BinaryAnalysis/DisassemblerAarch32.C						\
	BinaryAnalysis/DisassemblerAarch64.C						\
	BinaryAnalysis/DisassemblerJvm.C						\
	BinaryAnalysis/DisassemblerM68k.C						\
	BinaryAnalysis/DisassemblerMips.C						\
	BinaryAnalysis/DisassemblerNull.C						\
	BinaryAnalysis/DisassemblerPowerpc.C						\
	BinaryAnalysis/DisassemblerX86.C						\
	BinaryAnalysis/FeasiblePath.C							\
	BinaryAnalysis/FunctionCall.C							\
	BinaryAnalysis/FunctionSimilarity.C						\
	BinaryAnalysis/HotPatch.C							\
	BinaryAnalysis/InstructionCache.C						\
	BinaryAnalysis/LibraryIdentification.C						\
	BinaryAnalysis/MagicNumber.C							\
	BinaryAnalysis/MemoryMap.C							\
	BinaryAnalysis/NoOperation.C							\
	BinaryAnalysis/PointerDetection.C						\
	BinaryAnalysis/Reachability.C							\
	BinaryAnalysis/RegisterDescriptor.C						\
	BinaryAnalysis/RegisterParts.C							\
	BinaryAnalysis/Registers.C							\
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
	BinaryAnalysis/SymbolicExprParser.C						\
	BinaryAnalysis/SystemCall.C							\
	BinaryAnalysis/TaintedFlow.C							\
	BinaryAnalysis/ToSource.C							\
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
	StringUtility/Convert.C								\
	StringUtility/Diagnostics.C							\
	StringUtility/Escape.C								\
	StringUtility/NumberToString.C							\
	StringUtility/Predicate.C							\
	StringUtility/SplitJoin.C							\
	StringUtility/StringToNumber.C							\
	FormattedTable.C								\
	Initialize.C									\
	Progress.C									\
	SourceLocation.C								\
	Yaml.C

rose_metaprog_h_includedir = $(prefix)/include/Rose
rose_metaprog_h_include_HEADERS = metaprog.h

# Public header files are installed from parent makefiles.
