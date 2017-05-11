include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -Winvalid-pch $(ROSE_INCLUDES) -I$(srcdir)/graph -I$(srcdir)/libraryIdentification 

noinst_LTLIBRARIES = libbinaryMidend.la
libbinaryMidend_la_DEPENDENCIES =

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
libbinaryMidend_la_SOURCES =					\
    AbstractLocation.C						\
    BinaryBestMapAddress.C					\
    BinaryCallingConvention.C					\
    BinaryControlFlow.C						\
    BinaryDataFlow.C						\
    BinaryDemangler.C						\
    BinaryDominance.C						\
    BinaryFeasiblePath.C					\
    BinaryFunctionCall.C					\
    BinaryFunctionSimilarity.C					\
    BinaryMagic.C						\
    BinaryNoOperation.C						\
    BinaryPointerDetection.C					\
    BinaryReachability.C					\
    BinaryReturnValueUsed.C					\
    BinaryStackDelta.C						\
    BinaryString.C						\
    BinarySymbolicExpr.C					\
    BinarySymbolicExprParser.C					\
    BinaryTaintedFlow.C						\
    BinaryToSource.C						\
    DwarfLineMapper.C						\
    instructionSemantics/BaseSemantics2.C			\
    instructionSemantics/ConcreteSemantics2.C			\
    instructionSemantics/DataFlowSemantics2.C			\
    instructionSemantics/DispatcherM68k.C			\
    instructionSemantics/DispatcherPowerpc.C			\
    instructionSemantics/DispatcherX86.C			\
    instructionSemantics/IntervalSemantics2.C			\
    instructionSemantics/LlvmSemantics2.C			\
    instructionSemantics/MemoryCell.C				\
    instructionSemantics/MemoryCellList.C			\
    instructionSemantics/MemoryCellMap.C			\
    instructionSemantics/MemoryCellState.C			\
    instructionSemantics/MultiSemantics2.C			\
    instructionSemantics/NullSemantics2.C			\
    instructionSemantics/PartialSymbolicSemantics2.C		\
    instructionSemantics/RegisterStateGeneric.C			\
    instructionSemantics/SMTSolver.C				\
    instructionSemantics/SourceAstSemantics2.C			\
    instructionSemantics/StaticSemantics2.C			\
    instructionSemantics/SymbolicMemory2.C			\
    instructionSemantics/SymbolicSemantics2.C			\
    instructionSemantics/TraceSemantics2.C			\
    instructionSemantics/YicesSolver.C
else
libbinaryMidend_la_SOURCES = dummyBinaryMidend.C
endif

if ROSE_USE_SQLITE_DATABASE
libbinaryMidend_la_SOURCES +=					\
    libraryIdentification/libraryIdentification_reader.C	\
    libraryIdentification/libraryIdentification_writer.C	\
    libraryIdentification/functionIdentification.C 
endif

pkginclude_HEADERS =					\
    AbstractLocation.h					\
    BinaryAnalysisUtils.h				\
    BinaryBestMapAddress.h				\
    BinaryCallingConvention.h				\
    BinaryControlFlow.h					\
    BinaryDataFlow.h					\
    BinaryDemangler.h					\
    BinaryDominance.h					\
    BinaryFeasiblePath.h				\
    BinaryFunctionCall.h				\
    BinaryFunctionSimilarity.h				\
    BinaryMagic.h					\
    BinaryNoOperation.h					\
    BinaryPointerDetection.h				\
    BinaryReachability.h				\
    BinaryReturnValueUsed.h				\
    BinaryStackDelta.h					\
    BinaryStackVariable.h				\
    BinaryString.h					\
    BinarySymbolicExpr.h				\
    BinarySymbolicExprParser.h				\
    BinaryTaintedFlow.h					\
    BinaryToSource.h					\
    DwarfLineMapper.h					\
    ether.h						\
    instructionSemantics/BaseSemantics2.h		\
    instructionSemantics/ConcreteSemantics2.h		\
    instructionSemantics/DataFlowSemantics2.h		\
    instructionSemantics/DispatcherM68k.h		\
    instructionSemantics/DispatcherPowerpc.h		\
    instructionSemantics/DispatcherX86.h		\
    instructionSemantics/IntervalSemantics2.h		\
    instructionSemantics/LlvmSemantics2.h		\
    instructionSemantics/MemoryCell.h			\
    instructionSemantics/MemoryCellList.h		\
    instructionSemantics/MemoryCellMap.h		\
    instructionSemantics/MemoryCellState.h		\
    instructionSemantics/MultiSemantics2.h		\
    instructionSemantics/NullSemantics2.h		\
    instructionSemantics/PartialSymbolicSemantics2.h	\
    instructionSemantics/RegisterStateGeneric.h		\
    instructionSemantics/SMTSolver.h			\
    instructionSemantics/SourceAstSemantics2.h		\
    instructionSemantics/StaticSemantics2.h		\
    instructionSemantics/SymbolicMemory2.h		\
    instructionSemantics/SymbolicSemantics2.h		\
    instructionSemantics/TestSemantics2.h		\
    instructionSemantics/TraceSemantics2.h		\
    instructionSemantics/YicesSolver.h			\
    libraryIdentification/functionIdentification.h	\
    libraryIdentification/libraryIdentification.h


EXTRA_DIST = CMakeLists.txt dummyBinaryMidend.C
