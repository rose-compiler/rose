include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
AUTOMAKE_OPTIONS = nostdinc
SUBDIRS = Concolic InstructionSemantics2 ModelChecker Partitioner2 Unparser
AM_CPPFLAGS = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = librose_partial.la
librose_partial_la_SOURCES =			\
    AbstractLocation.C				\
    AsmFunctionIndex.C				\
    AstHash.C					\
    BestMapAddress.C				\
    BinaryLoader.C				\
    BinaryLoaderElf.C				\
    BinaryLoaderElfObj.C			\
    BinaryLoaderPe.C				\
    CallingConvention.C				\
    CodeInserter.C				\
    ControlFlow.C				\
    DataFlow.C					\
    Debugger.C					\
    Demangler.C					\
    Disassembler.C				\
    DisassemblerAarch32.C			\
    DisassemblerAarch64.C			\
    DisassemblerM68k.C				\
    DisassemblerMips.C				\
    DisassemblerNull.C				\
    DisassemblerPowerpc.C			\
    DisassemblerX86.C				\
    FeasiblePath.C				\
    FunctionCall.C				\
    FunctionSimilarity.C			\
    HotPatch.C					\
    InstructionCache.C				\
    MagicNumber.C				\
    MemoryMap.C					\
    NoOperation.C				\
    PointerDetection.C				\
    Reachability.C				\
    RegisterDescriptor.C			\
    RegisterParts.C				\
    Registers.C					\
    ReturnValueUsed.C				\
    SerialIo.C					\
    SmtCommandLine.C				\
    SmtlibSolver.C				\
    SmtSolver.C					\
    SourceLocations.C				\
    SRecord.C					\
    StackDelta.C				\
    String.C					\
    SymbolicExpr.C				\
    SymbolicExprParser.C			\
    SystemCall.C				\
    TaintedFlow.C				\
    ToSource.C					\
    Variables.C					\
    Z3Solver.C

# For header installation, see parent makefiles
