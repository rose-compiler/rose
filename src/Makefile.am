include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = util ROSETTA frontend midend backend roseSupport \
          3rdPartyLibraries roseIndependentSupport roseExtensions

# This lists a large number of directories.  If "make install"
# is run then we only need a single directory
INCLUDES = -Winvalid-pch $(ROSE_INCLUDES) 

# DQ (7/23/2008): New handling of EDG builds a single library instead of two.
# Binary distributions of ROSE store the binary for EDG related code in the source tree.
# JJW (8/3/2008): See src/frontend/CxxFrontend/Makefile.am.forBinaryEDG for
# more information on this process.
# libroseEDGlibraries = frontend/CxxFrontend/EDG/libroseEDG.la
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
   libroseEDGlibraries = frontend/CxxFrontend/EDG/libroseEDG.la
else
   libroseEDGlibraries =
endif

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
   libroseOpenFortranParserlibraries = frontend/OpenFortranParser_SAGE_Connection/libroseFortran.la
   libroseJavaParserlibraries = frontend/ECJ_ROSE_Connection/libroseJava.la
else
   libroseOpenFortranParserlibraries =
   libroseJavaParserlibraries =
endif

if ROSE_BUILD_PHP_LANGUAGE_SUPPORT
   librosePHPlibraries = frontend/PHPFrontend/librosePHPFrontend.la
else
   librosePHPlibraries =
endif

# if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
   libroseBinaryLibraries = \
      $(top_builddir)/src/frontend/BinaryDisassembly/libbinaryFrontend.la \
      $(top_builddir)/src/midend/binaryAnalyses/libbinaryMidend.la \
      $(top_builddir)/src/frontend/Disassemblers/libroseDisassemblers.la \
      $(top_builddir)/src/backend/asmUnparser/libroseAsmUnparser.la \
      $(top_builddir)/src/frontend/BinaryFormats/libroseBinaryFormats.la \
      $(top_builddir)/src/frontend/BinaryLoader/libroseBinaryLoader.la 
# else
#   libroseBinaryLibraries =
# endif

if ROSE_USE_MYSQL
  OPT_MYSQL_LDFLAGS = $(MYSQL_LDFLAGS)
else
  OPT_MYSQL_LDFLAGS =
endif

if ROSE_USE_SQLITE_DATABASE
   libroseSqlite3xConnection = \
		$(top_builddir)/src/roseExtensions/sqlite3x/libRoseSQLite3xDatabase.la
   OPT_SQLITE3_LDFLAGS       = $(SQLITE3_LDFLAGS)

# side effect requires sqlite3
   libsideEffect_LIBS = \
			$(top_builddir)/src/midend/programAnalysis/sideEffectAnalysis/libsideEffect.la
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

# TODO: correct list!
libroseLibraries = \
   $(top_builddir)/src/3rdPartyLibraries/MSTL/libmstl.la \
   $(top_builddir)/src/backend/unparser/libunparse.la \
   $(top_builddir)/src/backend/unparser/languageIndependenceSupport/libunparseLanguageIndependenceSupport.la \
   $(top_builddir)/src/backend/unparser/CxxCodeGeneration/libunparseCxx.la \
   $(top_builddir)/src/backend/unparser/FortranCodeGeneration/libunparseFortran.la \
   $(top_builddir)/src/backend/unparser/PHPCodeGeneration/libunparsePHP.la \
   $(top_builddir)/src/backend/unparser/formatSupport/libunparseFormatSupport.la \
   $(top_builddir)/src/frontend/SageIII/libsage3.la \
   $(libroseOpenFortranParserlibraries) \
   $(libroseJavaParserlibraries) \
   $(librosePHPlibraries) \
   $(libroseEDGlibraries) \
   $(libroseBinaryLibraries) \
   $(top_builddir)/src/midend/abstractHandle/libabstractHandle.la \
   $(top_builddir)/src/midend/astDiagnostics/libastDiagnostics.la \
   $(top_builddir)/src/midend/programTransformation/astInlining/libastinlining.la \
   $(top_builddir)/src/midend/programTransformation/astOutlining/libastOutliner.la \
   $(top_builddir)/src/midend/astProcessing/libastprocessing.la \
   $(top_builddir)/src/midend/astQuery/libquery.la \
   $(top_builddir)/src/midend/astRewriteMechanism/librewrite.la \
   $(top_builddir)/src/midend/astUtil/annotation/libannotation.la \
   $(top_builddir)/src/midend/astUtil/astInterface/libastInterface.la \
   $(top_builddir)/src/midend/astUtil/astSupport/libastSupport.la \
   $(top_builddir)/src/midend/astUtil/symbolicVal/libsymbolicVal.la \
   $(top_builddir)/src/midend/programTransformation/loopProcessing/computation/libcomputation.la \
   $(top_builddir)/src/midend/programTransformation/loopProcessing/depGraph/libdepGraph.la \
   $(top_builddir)/src/midend/programTransformation/loopProcessing/depInfo/libdepInfo.la \
   $(top_builddir)/src/midend/programTransformation/loopProcessing/driver/libdriver.la \
   $(top_builddir)/src/midend/programTransformation/loopProcessing/outsideInterface/libarrayOptimization.la \
   $(top_builddir)/src/midend/programTransformation/loopProcessing/prepostTransformation/libprepostloop.la \
   $(top_builddir)/src/midend/programTransformation/loopProcessing/slicing/libslicing.la \
   $(top_builddir)/src/midend/programTransformation/ompLowering/libompLowering.la \
   $(top_builddir)/src/midend/programAnalysis/CFG/libCFG.la \
   $(top_builddir)/src/midend/programAnalysis/CallGraphAnalysis/libCallGraph.la \
   $(top_builddir)/src/midend/programAnalysis/OAWrap/libOAWrap.la \
   $(top_builddir)/src/midend/programAnalysis/dataflowAnalysis/libdataflowAnalysis.la \
   $(top_builddir)/src/midend/programAnalysis/defUseAnalysis/libDefUseAnalysis.la \
   $(top_builddir)/src/midend/programAnalysis/graphAnalysis/libSB_GraphAnalysis.la \
   $(top_builddir)/src/midend/programAnalysis/pointerAnal/libpointerAnal.la \
   $(top_builddir)/src/midend/programAnalysis/staticInterproceduralSlicing/libStaticSlicing.la \
   $(top_builddir)/src/midend/programAnalysis/valuePropagation/libvaluePropagation.la \
   $(top_builddir)/src/midend/programAnalysis/variableRenaming/libVariableRenaming.la \
   $(top_builddir)/src/midend/programTransformation/constantFolding/libconstantFolding.la \
   $(top_builddir)/src/midend/programTransformation/finiteDifferencing/libfiniteDifferencing.la \
   $(top_builddir)/src/midend/programTransformation/functionCallNormalization/libfunctionCallNormalization.la \
   $(top_builddir)/src/midend/programTransformation/implicitCodeGeneration/libimplicitCodeGeneration.la \
   $(top_builddir)/src/midend/programTransformation/partialRedundancyElimination/libpre.la \
   $(top_builddir)/src/roseSupport/libroseSupport.la \
   $(top_builddir)/src/util/support/libsupport.la \
   $(top_builddir)/src/util/graphs/libgraphs.la \
   $(top_builddir)/src/util/libroseutil.la \
   $(libroseSqlite3xConnection) \
   $(libsideEffect_LIBS) \
   $(libroseQtExtension)

# DQ (4/17/2010): Added support to permit minimal ROSE configurations (e.g. --enable-only-fortran).
# used $(libroseOpenFortranParserlibraries) to replace $(top_builddir)/src/frontend/OpenFortranParser_SAGE_Connection/libroseFortran.la
# used $(librosePHPlibraries) to replace $(top_builddir)/src/frontend/PHPFrontend/librosePHPFrontend.la

EXTRA_DIST = CMakeLists.txt extra.docs

# Header files that need to be included in the distribution
include_HEADERS = \
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

# DQ (9/22/2006): Because we now use libtool, the shared library is a reference to the separate
# dynamic libraries in each directory and so testMultiRoseLib is redundant with testSharedRoseLib.
# This is true only if we have configured --with-development-libraries, else we have static
# in each directory and we will link with the collection of all static libraries.
# DQ (9/8/2006): The ROSE/docs/Rose/Makefile references the testSharedRoseLib program, so maybe is should be installed?
# bin_PROGRAMS = testSharedRoseLib # testMultiRoseLib
noinst_PROGRAMS = testSharedRoseLib 
all-local: testSharedRoseLib$(EXEEXT) 
# Test if programs using ROSE can be run
	@echo "Testing whether a simple program linked against ROSE can be run:" >&2
	@echo "The translator will be run without any input files, and will print a warning about that; the warning is normal." >&2
	@./testSharedRoseLib$(EXEEXT) || (echo "You cannot run a program linked against ROSE.  Please be sure that your LD_LIBRARY_PATH (Linux) or DYLD_LIBRARY_PATH (Mac OSX) contains the correct libraries (JVM, Boost, librose.so, etc.)." >&2; exit 1)

# PC (9/15/2006): force the use of the C++ linker
LINK = $(CXXLINK)

lib_LTLIBRARIES = librose.la

if ROSE_USE_OPEN_FORTRAN_PARSER
   JAVA_JVM_FLAGS = $(JAVA_JVM_LINK)

# OpenFortranParser-${OFP_VERSION}.jar:
#	cd ${top_builddir}/src/3rdPartyLibraries/fortran-parser; $(MAKE) get_jar_file
endif

librose_la_SOURCES = dummyCppFileForLibrose.C
librose_la_LIBADD = $(libroseLibraries) $(JAVA_JVM_FLAGS) $(LIB_QT) -lm $(RT_LIBS) \
                    $(BOOST_LDFLAGS) $(BOOST_DATE_TIME_LIB) \
		    $(BOOST_THREAD_LIB) $(BOOST_FILESYSTEM_LIB) \
		    $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_REGEX_LIB) \
		    $(BOOST_SYSTEM_LIB) $(BOOST_SERIALIZATION_LIB) \
		    $(BOOST_WAVE_LIB) \
		    -L$(top_builddir)/src/3rdPartyLibraries/libharu-2.1.0/src -lhpdf \
		    $(OPT_SQLITE3_LDFLAGS) \
        $(OPT_MYSQL_LDFLAGS)

# DQ (6/5/2010): Suggested change by Peter to support dwarf that would simplify use with Haskell
if ROSE_USE_DWARF
   librose_la_LIBADD += $(ROSE_DWARF_LIBS_WITH_PATH)
endif

# librose_la_LDFLAGS = -no-undefined --rpath $(INTEL_PIN_PATH)/intel64/lib-ext
librose_la_LDFLAGS = -no-undefined

#librose_la_LDFLAGS = $(ROSE_DWARF_LIBS_WITH_PATH)

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

testSharedRoseLib_SOURCES = testRoseLib.C
testSharedRoseLib_DEPENDENCIES = $(REQUIRED_LIBS)
testSharedRoseLib_LDADD  = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
# testSharedRoseLib_LDFLAGS  = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(ROSE_DWARF_LIBS_WITH_PATH)
# testSharedRoseLib_LDFLAGS  = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
# testSharedRoseLib_LDFLAGS  = $(ROSE_DWARF_LIBS_WITH_PATH)

# AM_LDFLAGS = $(ROSE_LIBS) $(ROSE_DWARF_LIBS_WITH_PATH)
# AM_LDFLAGS = $(ROSE_DWARF_LIBS_WITH_PATH)
# AM_LDFLAGS =

print:
	@echo "libroseLibraries = $(libroseLibraries)"
	@echo "LIBS_WITH_RPATH = $(LIBS_WITH_RPATH)"
	@echo "ROSE_LIBS = $(ROSE_LIBS)"
	@echo "ROSE_DWARF_LIBS_WITH_PATH = $(ROSE_DWARF_LIBS_WITH_PATH)"
	@echo "AM_LDFLAGS = $(AM_LDFLAGS)"

# DQ (1/10/2004): remove the librose.a and libedg.a files so that distclean will work cleanly
clean-local:
	rm -rf *.a *.so testSharedRoseLib testMultiRoseLib $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files 


distclean-local:
	rm -rf *.a *.so testSharedRoseLib testMultiRoseLib Templates.DB 

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








