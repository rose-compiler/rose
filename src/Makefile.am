include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
bin_PROGRAMS =
bin_SCRIPTS =
pkgdata_DATA =
MOSTLYCLEANFILES =

if ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
SUBDIRS = util ROSETTA roseSupport Rose frontend midend backend \
          3rdPartyLibraries roseIndependentSupport
else
SUBDIRS = util ROSETTA roseSupport Rose frontend midend backend \
          3rdPartyLibraries roseIndependentSupport roseExtensions
endif

# This lists a large number of directories.  If "make install"
# is run then we only need a single directory
AM_CPPFLAGS = -Winvalid-pch $(ROSE_INCLUDES)


# DQ (7/23/2008): New handling of EDG builds a single library instead of two.
# Binary distributions of ROSE store the binary for EDG related code in the source tree.
# JJW (8/3/2008): See src/frontend/CxxFrontend/Makefile.am.forBinaryEDG for
# more information on this process.
# TV (08/2012): changed 'libroseEDGlibraries' to 'libroseCxxlibraries' while adding Clang support to ROSE
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
if ROSE_USE_CLANG_FRONTEND
   libroseCxxlibraries = frontend/CxxFrontend/Clang/libroseClangFrontend.la
else
   libroseCxxlibraries = frontend/CxxFrontend/EDG/libroseEDG.la
endif
else
   libroseCxxlibraries =
endif

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
   libroseOpenFortranParserlibraries = frontend/OpenFortranParser_SAGE_Connection/libroseFortran.la
   pkgdata_DATA += $(top_srcdir)/src/3rdPartyLibraries/fortran-parser/iso_c_binding.rmod
   pkgdata_DATA += $(top_srcdir)/src/3rdPartyLibraries/fortran-parser/openacc.rmod
else
   libroseOpenFortranParserlibraries =
endif

# Added support for Fortran front-end development using the flang (F18) compiler [Rasmussen 8/12/2019]
if ROSE_EXPERIMENTAL_FLANG_ROSE_CONNECTION
   libroseExperimentalFlangParserlibraries = \
        frontend/Experimental_General_Language_Support/libexperimentalRoseGeneralLanguage.la \
        frontend/Experimental_Flang_ROSE_Connection/libexperimentalRoseFlang.la \
        3rdPartyLibraries/flang-parser/libFlangParser.la
else
   libroseExperimentalFlangParserlibraries =
endif

# Rasmussen (01/21/2018): Adding general support for experimental languages
# Rasmussen (05/29/2018): Moved parse tables for installation in $(prefix)/share/rose
if ROSE_BUILD_ATERM_SUPPORT
   libroseExperimentalGeneralLanguagelibraries = \
        frontend/Experimental_General_Language_Support/libexperimentalRoseGeneralLanguage.la \
        frontend/Experimental_General_Language_Support/ATerm/libexperimentalRoseATerm.la
   pkgdata_DATA += $(top_srcdir)/src/3rdPartyLibraries/experimental-fortran-parser/share/rose/Fortran.tbl
   pkgdata_DATA += $(top_srcdir)/src/3rdPartyLibraries/experimental-fortran-parser/share/rose/CUDA_Fortran.tbl
   pkgdata_DATA += $(top_srcdir)/src/3rdPartyLibraries/experimental-jovial-parser/share/rose/Jovial.tbl
else
   libroseExperimentalGeneralLanguagelibraries =
endif

# Rasmussen (01/29/2018): Removed building of ATerm traversal in 3rdPartyLibraries. Replaced with C++ traversals of ATerms.
if ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION
   libroseExperimentalOpenFortranParserlibraries = frontend/Experimental_OpenFortranParser_ROSE_Connection/libexperimentalRoseFortran.la
else
   libroseExperimentalOpenFortranParserlibraries =
endif

# DQ (8/15/2011): Revert back to what we had before to fix Hudson java only test.
# DQ (8/14/2011): Need this change to match Makefile changes in ECJ_ROSE_Connection/Makefile.am
if ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
libroseJavaParserlibraries = frontend/ECJ_ROSE_Connection/libroseJava.la
else
libroseJavaParserlibraries =
endif

if ROSE_BUILD_PHP_LANGUAGE_SUPPORT
   librosePHPlibraries = frontend/PHPFrontend/librosePHPFrontend.la
else
   librosePHPlibraries =
endif

if ROSE_BUILD_PYTHON_LANGUAGE_SUPPORT
   librosePythonlibraries = frontend/PythonFrontend/librosePythonFrontend.la
else
   librosePythonlibraries =
endif

# DQ (28/8/2017): Adding more general language support.
if ROSE_EXPERIMENTAL_CSHARP_ROSE_CONNECTION
   libroseCsharplibraries = frontend/Experimental_Csharp_ROSE_Connection/libexperimentalRoseCsharp.la
else
   libroseCsharplibraries =
endif

# DQ (28/8/2017): Adding more general language support.
if ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION
   libroseAdalibraries = frontend/Experimental_Ada_ROSE_Connection/libexperimentalRoseAda.la

# DQ (31/8/2017): This library needs to be provided at configure time.
#   DOT_ASIS_LIB_DIR_ ?= /home/quinlan1/ROSE/ADA/dot_asis/dot_asis_library/lib
#   DOT_ASIS_LIB_DIR_ = $(abs_top_builddir)/src/frontend/Experimental_Ada_ROSE_Connection/dot_asis/dot_asis_library/lib
#   dot_asis_lib_dir := $(DOT_ASIS_LIB_DIR_)

# DQ (31/8/2017): At least the Asis library needs to be provided built in to GNAT (atypical installation of GNAT).
#  asis_lib_dir = @gnat_home@/lib/asis/asislib
#  gnat_lib_dir = @gnat_home@/lib/gcc/x86_64-pc-linux-gnu/6.3.1/adalib

# DQ (31/8/2017): Some of these need to be built in ROSE as libtool constructed libraries (maybe also as dynamic libraries).
# Charles is working on this and it should be ready 4/9/2017.
# libroseAdalibraries += ${dot_asis_lib_dir}/libdot_asis.a ${asis_lib_dir}/libasis.a ${gnat_lib_dir}/libgnarl.a ${gnat_lib_dir}/libgnat.a
#  libroseAdalibraries += ${dot_asis_lib_dir}/libdot_asis.so
#  libroseAdalibraries += -Wl,-rpath -Wl,${dot_asis_lib_dir}/libdot_asis.so
#   libroseAdalibraries += ${dot_asis_lib_dir}/libdot_asis.so
else
# Ada is not enabled
   libroseAdalibraries =
endif

# DQ (28/8/2017): Adding more general language support.
if ROSE_EXPERIMENTAL_JOVIAL_ROSE_CONNECTION
   libroseJoviallibraries = frontend/Experimental_Jovial_ROSE_Connection/libexperimentalRoseJovial.la
else
   libroseJoviallibraries =
endif

# DQ (28/8/2017): Adding more general language support.
if ROSE_EXPERIMENTAL_COBOL_ROSE_CONNECTION
   libroseCobollibraries = frontend/Experimental_Cobol_ROSE_Connection/libexperimentalRoseCobol.la
else
   libroseCobollibraries =
endif

# Rasmussen (11/19/2017): Adding language support for Matlab
if ROSE_EXPERIMENTAL_MATLAB_ROSE_CONNECTION
   libroseMatlablibraries = frontend/Experimental_Matlab_ROSE_Connection/libexperimentalRoseMatlab.la
else
   libroseMatlablibraries =
endif

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
   libroseBinaryLibraries =							\
	$(top_builddir)/src/frontend/BinaryFormats/libroseBinaryFormats.la	\
	$(top_builddir)/src/frontend/Disassemblers/libroseDisassemblers.la	\
	$(top_builddir)/src/midend/BinaryAnalysis/libbinaryMidend.la
else
   libroseBinaryLibraries =							\
	$(top_builddir)/src/frontend/BinaryFormats/libroseBinaryFormats.la	\
	$(top_builddir)/src/midend/BinaryAnalysis/libbinaryMidend.la		\
	$(top_builddir)/src/frontend/Disassemblers/libroseDisassemblers.la
endif

libroseX10Libraries = frontend/X10_ROSE_Connection/libroseX10.la

if ROSE_USE_MYSQL
  OPT_MYSQL_LDFLAGS = $(MYSQL_LDFLAGS)
else
  OPT_MYSQL_LDFLAGS =
endif

if ROSE_USE_SQLITE_DATABASE
   libroseSqlite3xConnection = \
		$(top_builddir)/src/roseExtensions/sqlite3x/libRoseSQLite3xDatabase.la
   OPT_SQLITE3_LDFLAGS       = $(SQLITE3_LDFLAGS)

else
   libroseSqlite3xConnection =
   OPT_SQLITE3_LDFLAGS =
endif

if ROSE_WITH_ROSEQT
   libroseQtExtension = $(top_builddir)/src/roseExtensions/qtWidgets/libRoseQtWidgets.la \
                        $(top_builddir)/src/3rdPartyLibraries/qrose/QRoseLib/libqrose.la
else
   libroseQtExtension =
endif

# TOO (11/22/2011): flattening midend Makefiles => TODO: move conditional compilation to Makefile.
# TODO: correct list!
if ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
libroseLibraries = \
   $(top_builddir)/src/3rdPartyLibraries/MSTL/libmstl.la \
   $(top_builddir)/src/backend/libbackend.la \
   $(top_builddir)/src/frontend/SageIII/libsage3.la \
   $(libroseCxxlibraries) \
   $(top_builddir)/src/midend/libmidend.la \
   $(top_builddir)/src/roseSupport/libroseSupport.la \
   $(top_builddir)/src/util/support/libsupport.la \
   $(top_builddir)/src/util/graphs/libgraphs.la \
   $(top_builddir)/src/util/libroseutil.la \
   $(top_builddir)/src/util/libroseSaywer.la \
   $(libroseBinaryLibraries)

# DQ (12/18/2016): Moved below to make POET optional.
#  $(top_builddir)/src/3rdPartyLibraries/POET/libpoet.la
else
libroseLibraries = \
   $(top_builddir)/src/3rdPartyLibraries/MSTL/libmstl.la \
   $(top_builddir)/src/backend/libbackend.la \
   $(top_builddir)/src/frontend/SageIII/libsage3.la \
   $(libroseCxxlibraries) \
   $(libroseOpenFortranParserlibraries) \
   $(libroseExperimentalFlangParserlibraries) \
   $(libroseExperimentalGeneralLanguagelibraries) \
   $(libroseExperimentalOpenFortranParserlibraries) \
   $(libroseJavaParserlibraries) \
   $(librosePHPlibraries) \
   $(librosePythonlibraries) \
   $(libroseBinaryLibraries) \
   $(libroseX10Libraries) \
   $(libroseCsharplibraries) \
   $(libroseAdalibraries) \
   $(libroseJoviallibraries) \
   $(libroseCobollibraries) \
   $(top_builddir)/src/midend/programAnalysis/libprogramAnalysis.la \
   $(top_builddir)/src/midend/programTransformation/loopProcessing/libLoopProcessing.la \
   $(top_builddir)/src/midend/programTransformation/extractFunctionArgumentsNormalization/libExtractFunctionArgumentsNormalization.la \
   $(top_builddir)/src/midend/programTransformation/singleStatementToBlockNormalization/libSingleStatementToBlockNormalization.la \
   $(top_builddir)/src/midend/libmidend.la \
   $(top_builddir)/src/roseSupport/libroseSupport.la \
   $(top_builddir)/src/util/support/libsupport.la \
   $(top_builddir)/src/util/graphs/libgraphs.la \
   $(top_builddir)/src/util/libroseutil.la \
   $(top_builddir)/src/util/Sawyer/libroseSawyer.la \
   $(top_builddir)/src/roseExtensions/failSafe/libfailsafe.la \
   $(top_builddir)/src/Rose/librose_partial.la \
   $(top_builddir)/src/Rose/BinaryAnalysis/librose_partial.la \
   $(top_builddir)/src/Rose/BinaryAnalysis/Concolic/librose_partial.la \
   $(top_builddir)/src/Rose/BinaryAnalysis/InstructionSemantics2/librose_partial.la \
   $(top_builddir)/src/Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/librose_partial.la \
   $(top_builddir)/src/Rose/BinaryAnalysis/ModelChecker/librose_partial.la \
   $(top_builddir)/src/Rose/BinaryAnalysis/Partitioner2/librose_partial.la \
   $(top_builddir)/src/Rose/BinaryAnalysis/Unparser/librose_partial.la \
   $(top_builddir)/src/Rose/Color/librose_partial.la \
   $(top_builddir)/src/Rose/CommandLine/librose_partial.la \
   $(top_builddir)/src/Rose/Diagnostics/librose_partial.la \
   $(top_builddir)/src/Rose/FileSystem/librose_partial.la \
   $(top_builddir)/src/Rose/StringUtility/librose_partial.la \
   $(libroseSqlite3xConnection) \
   $(libroseQtExtension)

if ROSE_WITH_LIBHARU
libroseLibraries += \
   $(top_builddir)/src/3rdPartyLibraries/libharu-2.1.0/src/libhpdf.la
endif

# DQ (6/10/2017): Added libhpdf.la library and removed it from LIBADD rule.
# This is being done to fix the "make install" rule (which failes for me).
# Added: $(top_builddir)/src/3rdPartyLibraries/libharu-2.1.0/src/libhpdf.la above.

# DQ (12/18/2016): Moved below to make POET optional.
#  $(top_builddir)/src/3rdPartyLibraries/POET/libpoet.la
endif

# DQ (1/14/2017): Remove POET uniformally until it can be added to provide greater portability (working on Ubuntu systems).
# if !OS_VENDOR_UBUNTU
# if !OS_VENDOR_DEBIAN
# # DQ (12/18/2016): make POET optional (skip on Debian and Ubuntu systems until the
# # build is better and can avoid linking issues for these operating systems.
#    libroseLibraries += $(top_builddir)/src/3rdPartyLibraries/POET/libpoet.la
# endif
# endif

# DQ (4/17/2010): Added support to permit minimal ROSE configurations (e.g. --enable-only-fortran).
# used $(libroseOpenFortranParserlibraries) to replace $(top_builddir)/src/frontend/OpenFortranParser_SAGE_Connection/libroseFortran.la
# used $(librosePHPlibraries) to replace $(top_builddir)/src/frontend/PHPFrontend/librosePHPFrontend.la


EXTRA_DIST = CMakeLists.txt extra.docs

# Header files that need to be included in the distribution
pkginclude_HEADERS = \
     featureTests.h \
     rose.h \
     roseInternal.h \
     rose_msvc.h \
     msvc_stdint.h \
     roseSupport/utility_functionsImpl.C \
     midend/astProcessing/AstDOTGeneration.h \
     midend/astProcessing/AstDOTGenerationImpl.C \
     frontend/SageIII/sage3basic.h \
     frontend/SageIII/sage3basic.hhh \
     midend/astDiagnostics/AstConsistencyTests.h \
     backend/asmUnparser/AsmUnparser_compat.h

# Header files organized by namespace. If the directory part of the name doesn't match
# a C++ namespace, then DO NOT put the header file in this list.
nobase_pkginclude_HEADERS=								\
	Rose/BinaryAnalysis.h								\
	Rose/BinaryAnalysis/AbstractLocation.h						\
	Rose/BinaryAnalysis/AsmFunctionIndex.h						\
	Rose/BinaryAnalysis/AstHash.h							\
	Rose/BinaryAnalysis/BestMapAddress.h						\
	Rose/BinaryAnalysis/BinaryLoader.h						\
	Rose/BinaryAnalysis/BinaryLoaderElf.h						\
	Rose/BinaryAnalysis/BinaryLoaderElfObj.h					\
	Rose/BinaryAnalysis/BinaryLoaderPe.h						\
	Rose/BinaryAnalysis/CallingConvention.h						\
	Rose/BinaryAnalysis/CodeInserter.h						\
	Rose/BinaryAnalysis/Concolic.h							\
	Rose/BinaryAnalysis/Concolic/Architecture.h					\
	Rose/BinaryAnalysis/Concolic/BasicTypes.h					\
	Rose/BinaryAnalysis/Concolic/ConcolicExecutor.h					\
	Rose/BinaryAnalysis/Concolic/ConcreteExecutor.h					\
	Rose/BinaryAnalysis/Concolic/Database.h						\
	Rose/BinaryAnalysis/Concolic/ExecutionEvent.h					\
	Rose/BinaryAnalysis/Concolic/ExecutionLocation.h				\
	Rose/BinaryAnalysis/Concolic/ExecutionManager.h					\
	Rose/BinaryAnalysis/Concolic/InputVariables.h					\
	Rose/BinaryAnalysis/Concolic/LinuxExecutor.h					\
	Rose/BinaryAnalysis/Concolic/LinuxExitStatus.h					\
	Rose/BinaryAnalysis/Concolic/LinuxI386.h					\
	Rose/BinaryAnalysis/Concolic/LinuxTraceExecutor.h				\
	Rose/BinaryAnalysis/Concolic/SharedMemory.h					\
	Rose/BinaryAnalysis/Concolic/Specimen.h						\
	Rose/BinaryAnalysis/Concolic/SystemCall.h					\
	Rose/BinaryAnalysis/Concolic/TestCase.h						\
	Rose/BinaryAnalysis/Concolic/TestSuite.h					\
	Rose/BinaryAnalysis/Concolic/io-utility.h					\
	Rose/BinaryAnalysis/ControlFlow.h						\
	Rose/BinaryAnalysis/DataFlow.h							\
	Rose/BinaryAnalysis/Debugger.h							\
	Rose/BinaryAnalysis/Demangler.h							\
	Rose/BinaryAnalysis/Disassembler.h						\
	Rose/BinaryAnalysis/DisassemblerAarch32.h					\
	Rose/BinaryAnalysis/DisassemblerAarch64.h					\
	Rose/BinaryAnalysis/DisassemblerM68k.h						\
	Rose/BinaryAnalysis/DisassemblerMips.h						\
	Rose/BinaryAnalysis/DisassemblerNull.h						\
	Rose/BinaryAnalysis/DisassemblerPowerpc.h					\
	Rose/BinaryAnalysis/DisassemblerX86.h						\
	Rose/BinaryAnalysis/FeasiblePath.h						\
	Rose/BinaryAnalysis/FunctionCall.h						\
	Rose/BinaryAnalysis/FunctionSimilarity.h					\
	Rose/BinaryAnalysis/HotPatch.h							\
	Rose/BinaryAnalysis/InstructionCache.h						\
	Rose/BinaryAnalysis/InstructionEnumsAarch32.h					\
	Rose/BinaryAnalysis/InstructionEnumsAarch64.h					\
	Rose/BinaryAnalysis/InstructionEnumsM68k.h					\
	Rose/BinaryAnalysis/InstructionEnumsMips.h					\
	Rose/BinaryAnalysis/InstructionEnumsPowerpc.h					\
	Rose/BinaryAnalysis/InstructionEnumsX86.h					\
	Rose/BinaryAnalysis/InstructionSemantics2.h					\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/Dispatcher.h		\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/Exception.h		\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/Formatter.h		\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/MemoryCell.h		\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/MemoryCellList.h	\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/MemoryCellMap.h		\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/MemoryCellState.h	\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/MemoryState.h		\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/Merger.h		\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/RegisterState.h		\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/RegisterStateGeneric.h	\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/RiscOperators.h		\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/State.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/SValue.h		\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/SymbolicMemory.h	\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/Types.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemanticsDispatcher.h		\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemanticsException.h		\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemanticsFormatter.h		\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemanticsMemoryState.h		\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemanticsMerger.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemanticsRegisterState.h		\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemanticsRiscOperators.h		\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemanticsState.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemanticsSValue.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/BaseSemanticsTypes.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/ConcreteSemantics.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/DataFlowSemantics.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/DispatcherAarch32.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/DispatcherAarch64.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/DispatcherM68k.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/DispatcherPowerpc.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/DispatcherX86.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/IntervalSemantics.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/LlvmSemantics.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/MemoryCell.h				\
	Rose/BinaryAnalysis/InstructionSemantics2/MemoryCellList.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/MemoryCellMap.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/MemoryCellState.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/MultiSemantics.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/NativeSemantics.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/NullSemantics.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/PartialSymbolicSemantics.h		\
	Rose/BinaryAnalysis/InstructionSemantics2/RegisterStateGeneric.h		\
	Rose/BinaryAnalysis/InstructionSemantics2/SourceAstSemantics.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/StaticSemantics.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/SymbolicMemory.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/SymbolicSemantics.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/TestSemantics.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/TraceSemantics.h			\
	Rose/BinaryAnalysis/InstructionSemantics2/Util.h				\
	Rose/BinaryAnalysis/MagicNumber.h						\
	Rose/BinaryAnalysis/Matrix.h							\
	Rose/BinaryAnalysis/MemoryMap.h							\
	Rose/BinaryAnalysis/ModelChecker.h						\
	Rose/BinaryAnalysis/ModelChecker/BasicBlockUnit.h				\
	Rose/BinaryAnalysis/ModelChecker/Engine.h					\
	Rose/BinaryAnalysis/ModelChecker/ErrorTag.h					\
	Rose/BinaryAnalysis/ModelChecker/Exception.h					\
	Rose/BinaryAnalysis/ModelChecker/ExecutionUnit.h				\
	Rose/BinaryAnalysis/ModelChecker/ExternalFunctionUnit.h				\
	Rose/BinaryAnalysis/ModelChecker/FailureUnit.h					\
	Rose/BinaryAnalysis/ModelChecker/InstructionUnit.h				\
	Rose/BinaryAnalysis/ModelChecker/NullDerefTag.h					\
	Rose/BinaryAnalysis/ModelChecker/OobTag.h					\
	Rose/BinaryAnalysis/ModelChecker/P2Model.h					\
	Rose/BinaryAnalysis/ModelChecker/Path.h						\
	Rose/BinaryAnalysis/ModelChecker/PathNode.h					\
	Rose/BinaryAnalysis/ModelChecker/PathPredicate.h				\
	Rose/BinaryAnalysis/ModelChecker/PathPrioritizer.h				\
	Rose/BinaryAnalysis/ModelChecker/PathQueue.h					\
	Rose/BinaryAnalysis/ModelChecker/Periodic.h					\
	Rose/BinaryAnalysis/ModelChecker/SemanticCallbacks.h				\
	Rose/BinaryAnalysis/ModelChecker/Settings.h					\
	Rose/BinaryAnalysis/ModelChecker/SourceLister.h					\
	Rose/BinaryAnalysis/ModelChecker/Tag.h						\
	Rose/BinaryAnalysis/ModelChecker/Types.h					\
	Rose/BinaryAnalysis/NoOperation.h						\
	Rose/BinaryAnalysis/Partitioner2.h						\
	Rose/BinaryAnalysis/Partitioner2/AddressUsageMap.h				\
	Rose/BinaryAnalysis/Partitioner2/BasicBlock.h					\
	Rose/BinaryAnalysis/Partitioner2/BasicTypes.h					\
	Rose/BinaryAnalysis/Partitioner2/CfgPath.h					\
	Rose/BinaryAnalysis/Partitioner2/Config.h					\
	Rose/BinaryAnalysis/Partitioner2/ControlFlowGraph.h				\
	Rose/BinaryAnalysis/Partitioner2/DataBlock.h					\
	Rose/BinaryAnalysis/Partitioner2/DataFlow.h					\
	Rose/BinaryAnalysis/Partitioner2/Engine.h					\
	Rose/BinaryAnalysis/Partitioner2/Exception.h					\
	Rose/BinaryAnalysis/Partitioner2/Function.h					\
	Rose/BinaryAnalysis/Partitioner2/FunctionCallGraph.h				\
	Rose/BinaryAnalysis/Partitioner2/GraphViz.h					\
	Rose/BinaryAnalysis/Partitioner2/InstructionProvider.h				\
	Rose/BinaryAnalysis/Partitioner2/Modules.h					\
	Rose/BinaryAnalysis/Partitioner2/ModulesElf.h					\
	Rose/BinaryAnalysis/Partitioner2/ModulesLinux.h					\
	Rose/BinaryAnalysis/Partitioner2/ModulesM68k.h					\
	Rose/BinaryAnalysis/Partitioner2/ModulesMips.h					\
	Rose/BinaryAnalysis/Partitioner2/ModulesPe.h					\
	Rose/BinaryAnalysis/Partitioner2/ModulesPowerpc.h				\
	Rose/BinaryAnalysis/Partitioner2/ModulesX86.h					\
	Rose/BinaryAnalysis/Partitioner2/ParallelPartitioner.h				\
	Rose/BinaryAnalysis/Partitioner2/Partitioner.h					\
	Rose/BinaryAnalysis/Partitioner2/Reference.h					\
	Rose/BinaryAnalysis/Partitioner2/Semantics.h					\
	Rose/BinaryAnalysis/Partitioner2/Thunk.h					\
	Rose/BinaryAnalysis/Partitioner2/Utility.h					\
	Rose/BinaryAnalysis/PointerDetection.h						\
	Rose/BinaryAnalysis/Reachability.h						\
	Rose/BinaryAnalysis/RegisterDescriptor.h					\
	Rose/BinaryAnalysis/RegisterParts.h						\
	Rose/BinaryAnalysis/Registers.h							\
	Rose/BinaryAnalysis/ReturnValueUsed.h						\
	Rose/BinaryAnalysis/SerialIo.h							\
	Rose/BinaryAnalysis/SmtCommandLine.h						\
	Rose/BinaryAnalysis/SmtlibSolver.h						\
	Rose/BinaryAnalysis/SmtSolver.h							\
	Rose/BinaryAnalysis/SourceLocations.h						\
	Rose/BinaryAnalysis/SRecord.h							\
	Rose/BinaryAnalysis/StackDelta.h						\
	Rose/BinaryAnalysis/String.h							\
	Rose/BinaryAnalysis/SymbolicExpr.h						\
	Rose/BinaryAnalysis/SymbolicExprParser.h					\
	Rose/BinaryAnalysis/SystemCall.h						\
	Rose/BinaryAnalysis/TaintedFlow.h						\
	Rose/BinaryAnalysis/ToSource.h							\
	Rose/BinaryAnalysis/Unparser.h							\
	Rose/BinaryAnalysis/Unparser/EdgeArrows.h					\
	Rose/BinaryAnalysis/Unparser/Aarch32.h						\
	Rose/BinaryAnalysis/Unparser/Aarch64.h						\
	Rose/BinaryAnalysis/Unparser/Base.h						\
	Rose/BinaryAnalysis/Unparser/M68k.h						\
	Rose/BinaryAnalysis/Unparser/Mips.h						\
	Rose/BinaryAnalysis/Unparser/Null.h						\
	Rose/BinaryAnalysis/Unparser/Powerpc.h						\
	Rose/BinaryAnalysis/Unparser/Settings.h						\
	Rose/BinaryAnalysis/Unparser/X86.h						\
	Rose/BinaryAnalysis/Utils.h							\
	Rose/BinaryAnalysis/Variables.h							\
	Rose/BinaryAnalysis/Z3Solver.h							\
	Rose/BitFlags.h									\
	Rose/BitOps.h									\
	Rose/Color.h									\
	Rose/CommandLine.h								\
	Rose/CommandLine/IntervalParser.h						\
	Rose/CommandLine/SuffixMultiplierParser.h					\
	Rose/Constants.h								\
	Rose/Diagnostics.h								\
	Rose/Exception.h								\
	Rose/FileSystem.h								\
	Rose/FormatRestorer.h								\
	Rose/FormattedTable.h								\
	Rose/GraphUtility.h								\
	Rose/Initialize.h								\
	Rose/Progress.h									\
	Rose/RecursionCounter.h								\
	Rose/SourceLocation.h								\
	Rose/StringUtility.h								\
	Rose/StringUtility/Convert.h							\
	Rose/StringUtility/Diagnostics.h						\
	Rose/StringUtility/Escape.h							\
	Rose/StringUtility/NumberToString.h						\
	Rose/StringUtility/Predicate.h							\
	Rose/StringUtility/SplitJoin.h							\
	Rose/StringUtility/StringToNumber.h


########################################################################################################################

# DQ (9/22/2006): Because we now use libtool, the shared library is a reference to the separate
# dynamic libraries in each directory and so testMultiRoseLib is redundant with testSharedRoseLib.
# This is true only if we have configured --with-development-libraries, else we have static
# in each directory and we will link with the collection of all static libraries.

all-local:

# PC (9/15/2006): force the use of the C++ linker
LINK = $(CXXLINK)

lib_LTLIBRARIES = librose.la

# if ROSE_USE_OPEN_FORTRAN_PARSER
if ROSE_USE_INTERNAL_JAVA_SUPPORT
   JAVA_JVM_FLAGS = $(JAVA_JVM_LINK)

# OpenFortranParser-${OFP_VERSION}.jar:
#	cd ${top_builddir}/src/3rdPartyLibraries/fortran-parser; $(MAKE) get_jar_file
endif


librose_la_SOURCES = dummyCppFileForLibrose.C

if ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
librose_la_LIBADD = $(BOOST_LDFLAGS) $(libroseLibraries) $(JAVA_JVM_FLAGS) $(LIB_QT) -lm $(RT_LIBS) \
          $(BOOST_DATE_TIME_LIB) \
          $(BOOST_THREAD_LIB) $(BOOST_FILESYSTEM_LIB) \
          $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_REGEX_LIB) \
          $(BOOST_SYSTEM_LIB) $(BOOST_SERIALIZATION_LIB) \
          $(BOOST_WAVE_LIB) $(BOOST_IOSTREAMS_LIB)
else
librose_la_LIBADD = $(BOOST_LDFLAGS) $(libroseLibraries) $(JAVA_JVM_FLAGS) $(LIB_QT) -lm $(RT_LIBS) \
          $(BOOST_DATE_TIME_LIB) \
          $(BOOST_THREAD_LIB) $(BOOST_FILESYSTEM_LIB) \
          $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_REGEX_LIB) \
          $(BOOST_SYSTEM_LIB) $(BOOST_SERIALIZATION_LIB) \
          $(BOOST_WAVE_LIB) $(BOOST_IOSTREAMS_LIB) \
          $(OPT_SQLITE3_LDFLAGS) \
          $(OPT_MYSQL_LDFLAGS) $(ROSE_ATERM_LIBS) $(ROSE_FLANG_LIBS) \
          $(ROSE_COBOL_PT_LIBS_WITH_PATH)

# DQ (6/9/2017): Removed because it failes for "make install" rule on my machine.
# This was located after the BOOST_WAVE_LIB line in the librose_la_LIBADD rule above.
#          -L$(top_builddir)/src/3rdPartyLibraries/libharu-2.1.0/src -lhpdf
# Added this library to the libroseLibraries rule farther above in this file.
endif

# libelf is a prerequisite for using libdwarf
if ROSE_HAVE_LIBELF
   librose_la_LIBADD += $(ROSE_ELF_LIBS_WITH_PATH)
endif

# DQ (6/5/2010): Suggested change by Peter to support dwarf that would simplify use with Haskell
if ROSE_HAVE_LIBDWARF
   librose_la_LIBADD += $(ROSE_DWARF_LIBS_WITH_PATH)
endif

if ROSE_USE_GCC_OMP
   librose_la_LIBADD += lgomp
endif

if ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION
  librose_la_LIBADD += -L${DOT_ASIS_LIB_DIR_} -lasis_adapter
endif
# librose_la_LDFLAGS = -no-undefined --rpath $(INTEL_PIN_PATH)/intel64/lib-ext
librose_la_LDFLAGS = -no-undefined $(ROSE_RPATHS)

#librose_la_LDFLAGS = $(ROSE_DWARF_LIBS_WITH_PATH)
#if ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION
#  librose_la_LDFLAGS += -R ${DOT_ASIS_LIB_DIR_} 
#endif

# DQ (4/12/2016): This is required for use with clang (not clear how to compile ROSE with out it).
if USING_CLANG_COMPILER
# librose_la_LIBADD += -lstdc++
# librose_la_LIBADD += -lstdc++ -fno-rtti
librose_la_LIBADD += -lstdc++
librose_la_LDFLAGS += -fno-rtti
endif
librose_la_LDFLAGS += -fno-rtti

librose_la_DEPENDENCIES = $(libroseLibraries)

# if ROSE_USE_OPEN_FORTRAN_PARSER
# librose_la_DEPENDENCIES += OpenFortranParser-${OFP_VERSION}.jar
# endif

# Build list of all required libraries so that they can be built in parallel
# REQUIRED_LIBS = $(libdir)/libedg.la
REQUIRED_LIBS = librose.la


# DQ (3/14/2009): In this file we have to overwrite existing values in LDFLAGS (to build librose.so).
# DQ (3/14/2009): We have to clear the LDFLAGS variable...  This is not so great.
LDFLAGS =
if OS_MACOSX
if ROSE_HAVE_LIBGCRYPT
PORT_PATH = $(shell which port)
PORT_HOME = $(shell dirname $(PORT_PATH))/..
LDFLAGS += -L$(PORT_HOME)/lib -lgcrypt
else
LDFLAGS +=
endif
endif


########################################################################################################################
# This tool spits out information that tells users how to compile and link programs that use ROSE. Therefore, we have to
# build a configuration file and compile its installed name into the rose-config tool. It's also especially important
# that this tool uses rpaths to find its dynamically-linked libraries, otherwise the user will be tempted to use
# LD_LIBRARY_PATH and complain when the tool segfaults, not realizing that the fault is because they told the tool to
# use a library that is incompatible with the one it was compiled to use.
bin_PROGRAMS += rose-config
rose_config_SOURCES = rose-config.C
rose_config_LDFLAGS = $(ROSE_RPATHS)
rose_config_DEPENDENCIES = $(REQUIRED_LIBS)
rose_config_CPPFLAGS = -DLIBDIR='"$(libdir)"'
rose_config_LDADD = $(ROSE_LIBS)
if OS_MACOSX
  rose_config_LDFLAGS += -Xlinker -rpath -Xlinker $(JAVA_JVM_PATH)
endif

# DQ (4/12/2016): This is required for use with clang (not clear how to compile ROSE with out it).
if USING_CLANG_COMPILER
rose_config_LDADD += -lstdc++
rose_config_LDFLAGS += -fno-rtti
endif
rose_config_LDFLAGS += -fno-rtti

rose_config_installed_cppflags =		\
	-I@includedir@/rose			\
	$(ROSE_QT_INCLUDE)			\
	$(ROSE_SQLITE_DATABASE_INCLUDE)		\
	$(ROSE_PHP_INCLUDES)			\
	$(ROSE_YICES_INCLUDES)			\
	$(ROSE_Z3_INCLUDES)			\
	$(ROSE_YAML_INCLUDES)			\
	$(ROSE_LIBMAGIC_INCLUDES)		\
	$(ROSE_ELF_INCLUDES)			\
	$(ROSE_DWARF_INCLUDES)			\
	$(ROSE_WINE_INCLUDES)			\
	$(ROSE_PCH_INCLUDE)			\
	$(INTEL_PIN_INCLUDE_PATHS)		\
	$(ROSE_ETHER_INCLUDE)			\
	$(ROSE_ATERM_INCLUDE)			\
	$(ROSE_DLIB_INCLUDES)			\
	$(ROSE_GCRYPT_INCLUDES)			\
	$(ROSE_CAPSTONE_INCLUDES)		\
	$(BOOST_CPPFLAGS)

rose_config_installed_ldflags =			\
	-L@libdir@ -lrose			\
	$(BOOST_LDFLAGS)			\
	$(BOOST_DATE_TIME_LIB)			\
	$(BOOST_THREAD_LIB)			\
	$(BOOST_FILESYSTEM_LIB)			\
	$(BOOST_PROGRAM_OPTIONS_LIB)		\
	$(BOOST_REGEX_LIB)			\
	$(BOOST_SYSTEM_LIB)			\
	$(BOOST_SERIALIZATION_LIB)		\
	$(BOOST_WAVE_LIB)			\
	$(BOOST_IOSTREAMS_LIB)			\
	$(BOOST_CHRONO_LIB)			\
	$(JAVA_JVM_LINK)			\
	$(SQLITE_DATABASE_LIBS)			\
	$(RT_LIBS)				\
	$(ROSE_YICES_LIBS_WITH_PATH)		\
	$(ROSE_Z3_LIBS_WITH_PATH)		\
	$(ROSE_PHP_LIBS_WITH_PATH)		\
	$(ROSE_ELF_LIBS_WITH_PATH)		\
	$(ROSE_DWARF_LIBS_WITH_PATH)		\
	$(ROSE_QT_LIBS_WITH_PATH)		\
	$(ROSE_SSL_LIBS)			\
	$(ROSE_ETHER_LIBS)			\
	$(ROSE_INTEL_COMPILER_MATH_LIBS)	\
	$(ROSE_ATERM_LIBS)			\
	$(ROSE_FLANG_LIBS)			\
	$(ROSE_COBOL_PT_LIBS_WITH_PATH)         \
	$(ROSE_YAML_LIBS_WITH_PATH)		\
	$(ROSE_LIBMAGIC_LIBS_WITH_PATH)		\
	$(ROSE_DLIB_LIBS_WITH_PATH)		\
	$(ROSE_GCRYPT_LIBS_WITH_PATH)		\
	$(ROSE_CAPSTONE_LIBS_WITH_PATH)		\
	-lm
if ROSE_USE_EDG_QUAD_FLOAT
  rose_config_installed_ldflags += -lquadmath
endif
if ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION
  rose_config_installed_ldflags += -L${DOT_ASIS_LIB_DIR_} -lasis_adapter
endif
rose_config_cfgdir = $(libdir)
rose_config_cfg_DATA = rose-config.cfg

# Rasmussen (3/23/2018): Added ROSE_RPATHS and ROSE_LINK_RPATHS to enable
# linking on Mac OSX from a ROSE installations.
rose-config.cfg:
	$(AM_V_GEN) (								\
		echo "# Things that the 'rose-config' tool can report";		\
		echo "ROSE_CC       = $(CC)";					\
		echo "ROSE_CXX      = $(CXX)";					\
		echo "ROSE_CPPFLAGS = $(rose_config_installed_cppflags)";	\
		echo "ROSE_CFLAGS   = $(CFLAGS)";				\
		echo "ROSE_CXXFLAGS = $(CXXFLAGS)";				\
		echo "ROSE_LDFLAGS  = $(rose_config_installed_ldflags)";	\
		echo "ROSE_LIBDIRS  = $(filter-out -R, $(ROSE_RPATHS))";	\
		echo "ROSE_PREFIX   = $(prefix)";				\
		echo "# Additional items useful to include in Makefiles";	\
		echo "ROSE_RPATHS       = $(ROSE_RPATHS)";			\
		echo "ROSE_LINK_RPATHS  = $(ROSE_LINK_RPATHS)";			\
	) >$@

MOSTLYCLEANFILES += rose-config.cfg

bin_PROGRAMS += rose-compiler
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
  bin_PROGRAMS += rose-cc rose-c++
endif
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
  bin_PROGRAMS += rose-fortran
endif
# The -std= option in ROSE only support c, c++, fortran
#if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#if ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
#if ROSE_BUILD_PYTHON_LANGUAGE_SUPPORT
#if ROSE_BUILD_PHP_LANGUAGE_SUPPORT
#if ROSE_EXPERIMENTAL_CSHARP_ROSE_CONNECTION
#if ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION
#if ROSE_EXPERIMENTAL_JOVIAL_ROSE_CONNECTION
#if ROSE_EXPERIMENTAL_COBOL_ROSE_CONNECTION
#if ROSE_EXPERIMENTAL_MATLAB_ROSE_CONNECTION

ROSEBIN_LDADD        = $(ROSE_LIBS)
ROSEBIN_LDFLAGS      = $(ROSE_RPATHS)
ROSEBIN_DEPENDENCIES = $(REQUIRED_LIBS)
if USING_CLANG_COMPILER
  ROSEBIN_LDADD   += -lstdc++
  ROSEBIN_LDFLAGS += -fno-rtti
endif
  ROSEBIN_LDFLAGS += -fno-rtti
if OS_MACOSX
  ROSEBIN_LDFLAGS += -Xlinker -rpath -Xlinker $(JAVA_JVM_PATH)
endif

rose_compiler_SOURCES      = rose-compiler.C
rose_compiler_LDFLAGS      = $(ROSEBIN_LDFLAGS)
rose_compiler_DEPENDENCIES = $(REQUIRED_LIBS)
rose_compiler_CPPFLAGS     =
rose_compiler_LDADD        = $(ROSEBIN_LDADD)

rose_cc_SOURCES      = rose-compiler.C
rose_cc_LDFLAGS      = $(ROSEBIN_LDFLAGS)
rose_cc_DEPENDENCIES = $(REQUIRED_LIBS)
rose_cc_CPPFLAGS     = -DROSE_COMPILER_FOR_LANGUAGE="\"c\""
rose_cc_LDADD        = $(ROSEBIN_LDADD)

rose_c___SOURCES      = rose-compiler.C
rose_c___LDFLAGS      = $(ROSEBIN_LDFLAGS)
rose_c___DEPENDENCIES = $(REQUIRED_LIBS)
rose_c___CPPFLAGS     = -DROSE_COMPILER_FOR_LANGUAGE="\"c++\""
rose_c___LDADD        = $(ROSEBIN_LDADD)

rose_fortran_SOURCES      = rose-compiler.C
rose_fortran_LDFLAGS      = $(ROSEBIN_LDFLAGS)
rose_fortran_DEPENDENCIES = $(REQUIRED_LIBS)
rose_fortran_CPPFLAGS     = -DROSE_COMPILER_FOR_LANGUAGE="\"fortran\""
rose_fortran_LDADD        = $(ROSEBIN_LDADD)

########################################################################################################################
# Test that a simple ROSE program can run, and as a side effect, print some version numbers.

check-boost:
	@echo "  TEST     boost shared linkage [rose-compiler]"
	${AM_V_at}$(top_srcdir)/scripts/check_boost_linkage.sh .libs/rose-compiler >&2

check-dynload:
	@echo "  TEST     whether a trivial ROSE program runs [rose-compiler]"
	${AM_V_at}if ! ./rose-compiler --version; then                                           \
	  echo "error: You cannot run a program linked with ROSE. Please be sure that your" >&2; \
	  echo "error: LD_LIBRARY_PATH (Linux) or DYLD_LIBRARY_PATH (Mac OSX) contains the" >&2; \
	  echo "error: correct libraries (JVM, Boost, librose.so, etc.)" >&2;                    \
	  exit 1;                                                                                \
	fi

check-config:
	@echo "  TEST     ROSE configuration tool [rose-config]"
	./rose-config --version

if ROSE_BUILD_C_LANGUAGE_SUPPORT
if OS_MACOSX
check-hello-c: $(eval SHELL:=/bin/bash)
	@echo "  TEST     ROSE C compiler [rose-cc]"
	${AM_V_at}echo "#include <stdio.h>"                     >  hello-world.c
	${AM_V_at}echo "int main() {"                           >> hello-world.c
	${AM_V_at}echo "   printf(\"Hello World from C\\n\");" >> hello-world.c
	${AM_V_at}echo "   return 0;"                           >> hello-world.c
	${AM_V_at}echo "}"                                      >> hello-world.c
	${AM_V_at}./rose-cc -rose:ast:write hello-world-c.ast hello-world.c -o hello-world-c
	${AM_V_at}./hello-world-c
	${AM_V_at}./rose-compiler -rose:ast:read hello-world-c.ast
else
check-hello-c:
	@echo "  TEST     ROSE C compiler [rose-cc]"
	${AM_V_at}echo "#include <stdio.h>"                     >  hello-world.c
	${AM_V_at}echo "int main() {"                           >> hello-world.c
	${AM_V_at}echo "   printf(\"Hello World from C\\n\");" >> hello-world.c
	${AM_V_at}echo "   return 0;"                           >> hello-world.c
	${AM_V_at}echo "}"                                      >> hello-world.c
	${AM_V_at}./rose-cc -rose:ast:write hello-world-c.ast hello-world.c -o hello-world-c
	${AM_V_at}./hello-world-c
	${AM_V_at}./rose-compiler -rose:ast:read hello-world-c.ast
endif
endif
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
check-hello-cxx:
	@echo "  TEST     ROSE C++ compiler [rose-c++]"
	${AM_V_at}echo "#include <iostream>"                                      >  hello-world.cpp
	${AM_V_at}echo "int main() {"                                             >> hello-world.cpp
	${AM_V_at}echo "    std::cout << \"Hello World from C++!\" << std::endl;" >> hello-world.cpp
	${AM_V_at}echo "    return 0;"                                            >> hello-world.cpp
	${AM_V_at}echo "}"                                                        >> hello-world.cpp
	${AM_V_at}./rose-c++ -rose:ast:write hello-world-cxx.ast hello-world.cpp -o hello-world-cpp
	${AM_V_at}./hello-world-cpp
	${AM_V_at}./rose-compiler -rose:ast:read hello-world-cxx.ast
endif
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
check-hello-fortran:
	@echo "  TEST     ROSE Fortran compiler [rose-fortran]"
	${AM_V_at}echo "program hello"                            >  hello-world.f90
	${AM_V_at}echo "  print *, \"Hello World from Fortran!\"" >> hello-world.f90
	${AM_V_at}echo "end program hello"                        >> hello-world.f90
	${AM_V_at}./rose-fortran -rose:ast:write hello-world-f90.ast hello-world.f90 -o hello-world-f90
	${AM_V_at}./hello-world-f90
#	${AM_V_at}./rose-compiler -rose:ast:read hello-world-f90.ast
endif

check-local: $(bin_PROGRAMS)
	${AM_V_at}$(MAKE) check-boost
	${AM_V_at}$(MAKE) check-dynload
	${AM_V_at}$(MAKE) check-config
if ROSE_BUILD_C_LANGUAGE_SUPPORT
	${AM_V_at}$(MAKE) check-hello-c
endif
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
	${AM_V_at}$(MAKE) check-hello-cxx
endif
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
	${AM_V_at}$(MAKE) check-hello-fortran
endif

########################################################################################################################

# Rasmussen (3/23/2018): Added installing libtool to the bin directory to support
# linking an application using ROSE from a ROSE installations.
bin_SCRIPTS += ../libtool


# DQ (1/10/2004): remove the librose.a and libedg.a files so that distclean will work cleanly
# Do not remove libraries indiscriminately with wildcards [Matzke, 2017-06-19]
clean-local:
	rm -rf librose.a librose.so testSharedRoseLib testMultiRoseLib $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files
	rm -rf $(MOSTLYCLEANFILES)

distclean-local:
	rm -rf librose.a librose.so testSharedRoseLib testMultiRoseLib Templates.DB

cleanTest:
	rm -rf *.lo testSharedRoseLib testMultiRoseLib

# DQ (9/14/2006): These are required!
# Note that uninstall-hook does not work, but uninstall-local does.
uninstall-local:
	rm -f $(DESTDIR)$(libdir)/libedg.so
	rm -f $(DESTDIR)$(libdir)/librose.so
#	rm -f $(libdir)/libedg.so
#	rm -f $(libdir)/librose.so
	@echo "************************************************************"
	@echo "**** make uninstall rule complete (terminated normally) ****"
	@echo "************************************************************"


rebuildPrelinker:
#	DQ (12/21/2009): The locaion of the EDG directory has been changed now that it is a submodule in our git repository.
#	rm -f frontend/CxxFrontend/EDG_3.3/util/edg_prelink.lo
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
	rm -f frontend/CxxFrontend/EDG/EDG_3.3/util/edg_prelink.lo
endif
	rm -f frontend/SageIII/Cxx_Grammar.lo
	rm -f roseSupport/templateSupport.lo
	$(MAKE)

