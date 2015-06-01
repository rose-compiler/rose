include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
bin_PROGRAMS =

if ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
SUBDIRS = util ROSETTA roseSupport frontend midend backend \
          3rdPartyLibraries roseIndependentSupport
else
SUBDIRS = util ROSETTA roseSupport frontend midend backend \
          3rdPartyLibraries roseIndependentSupport roseExtensions
endif

# This lists a large number of directories.  If "make install"
# is run then we only need a single directory
INCLUDES = -Winvalid-pch $(ROSE_INCLUDES) 


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
else
   libroseOpenFortranParserlibraries =
endif

if ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION
   libroseExperimentalOpenFortranParserlibraries = frontend/Experimental_OpenFortranParser_ROSE_Connection/libexperimentalRoseFortran.la
#   libroseExperimentalOpenFortranParserlibraries =
#   ROSE_EXPERIMENTAL_OFP_INCLUDE = -I$(_PREFIX)/include
#   ROSE_EXPERIMENTAL_OFP_LIBS    = -L$(_PREFIX)/lib -lxenctrl
else
   libroseExperimentalOpenFortranParserlibraries =
endif

# DQ (8/15/2011): Revert back to what we had before to fix Hudson java only test.
# DQ (8/14/2011): Need this change to match Makefile changes in ECJ_ROSE_Connection/Makefile.am
libroseJavaParserlibraries = frontend/ECJ_ROSE_Connection/libroseJava.la

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

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
   libroseBinaryLibraries =							\
	$(top_builddir)/src/frontend/BinaryDisassembly/libbinaryFrontend.la	\
	$(top_builddir)/src/frontend/BinaryFormats/libroseBinaryFormats.la	\
	$(top_builddir)/src/frontend/BinaryLoader/libroseBinaryLoader.la	\
	$(top_builddir)/src/frontend/Disassemblers/libroseDisassemblers.la	\
	$(top_builddir)/src/frontend/Partitioner2/libPartitioner.la		\
	$(top_builddir)/src/midend/binaryAnalyses/libbinaryMidend.la
else
   libroseBinaryLibraries =							\
	$(top_builddir)/src/frontend/BinaryFormats/libroseBinaryFormats.la	\
	$(top_builddir)/src/frontend/BinaryLoader/libroseBinaryLoader.la	\
	$(top_builddir)/src/midend/binaryAnalyses/libbinaryMidend.la		\
	$(top_builddir)/src/frontend/Disassemblers/libroseDisassemblers.la	\
	$(top_builddir)/src/frontend/Partitioner2/libPartitioner.la
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
   $(top_builddir)/src/3rdPartyLibraries/POET/libpoet.la \
   $(libroseBinaryLibraries)
else
libroseLibraries = \
   $(top_builddir)/src/3rdPartyLibraries/MSTL/libmstl.la \
   $(top_builddir)/src/backend/libbackend.la \
   $(top_builddir)/src/frontend/SageIII/libsage3.la \
   $(libroseCxxlibraries) \
   $(libroseOpenFortranParserlibraries) \
   $(libroseExperimentalOpenFortranParserlibraries) \
   $(libroseJavaParserlibraries) \
   $(librosePHPlibraries) \
   $(librosePythonlibraries) \
   $(libroseBinaryLibraries) \
   $(libroseX10Libraries) \
   $(top_builddir)/src/midend/programAnalysis/libprogramAnalysis.la \
   $(top_builddir)/src/midend/programTransformation/loopProcessing/libLoopProcessing.la \
   $(top_builddir)/src/midend/programTransformation/extractFunctionArgumentsNormalization/libExtractFunctionArgumentsNormalization.la \
   $(top_builddir)/src/midend/programTransformation/singleStatementToBlockNormalization/libSingleStatementToBlockNormalization.la \
   $(top_builddir)/src/3rdPartyLibraries/POET/libpoet.la \
   $(top_builddir)/src/midend/libmidend.la \
   $(top_builddir)/src/roseSupport/libroseSupport.la \
   $(top_builddir)/src/util/support/libsupport.la \
   $(top_builddir)/src/util/graphs/libgraphs.la \
   $(top_builddir)/src/util/libroseutil.la \
   $(top_builddir)/src/util/libroseSawyer.la \
   $(top_builddir)/src/roseExtensions/failSafe/libfailsafe.la \
   $(libroseSqlite3xConnection) \
   $(libroseQtExtension)
endif

# DQ (4/17/2010): Added support to permit minimal ROSE configurations (e.g. --enable-only-fortran).
# used $(libroseOpenFortranParserlibraries) to replace $(top_builddir)/src/frontend/OpenFortranParser_SAGE_Connection/libroseFortran.la
# used $(librosePHPlibraries) to replace $(top_builddir)/src/frontend/PHPFrontend/librosePHPFrontend.la

EXTRA_DIST = CMakeLists.txt extra.docs

# Header files that need to be included in the distribution
pkginclude_HEADERS = \
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
	@$(top_srcdir)/scripts/check_boost_linkage.sh .libs/testSharedRoseLib >&2
	@echo "[INFO] Testing whether a simple program linked with ROSE can be run (a warning will be issued because the translator is being run without an input file):" >&2
	@./testSharedRoseLib$(EXEEXT) || (echo "[ERROR] You cannot run a program linked with ROSE. Please be sure that your LD_LIBRARY_PATH (Linux) or DYLD_LIBRARY_PATH (Mac OSX) contains the correct libraries (JVM, Boost, librose.so, etc.)." >&2; exit 1)
	@echo "[INFO] Success!"

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
          $(BOOST_WAVE_LIB)
else
librose_la_LIBADD = $(BOOST_LDFLAGS) $(libroseLibraries) $(JAVA_JVM_FLAGS) $(LIB_QT) -lm $(RT_LIBS) \
          $(BOOST_DATE_TIME_LIB) \
          $(BOOST_THREAD_LIB) $(BOOST_FILESYSTEM_LIB) \
          $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_REGEX_LIB) \
          $(BOOST_SYSTEM_LIB) $(BOOST_SERIALIZATION_LIB) \
          $(BOOST_WAVE_LIB) \
          -L$(top_builddir)/src/3rdPartyLibraries/libharu-2.1.0/src -lhpdf \
          $(OPT_SQLITE3_LDFLAGS) \
          $(OPT_MYSQL_LDFLAGS) $(ROSE_ATERM_LIBS)
endif

# DQ (6/5/2010): Suggested change by Peter to support dwarf that would simplify use with Haskell
if ROSE_USE_DWARF
   librose_la_LIBADD += $(ROSE_DWARF_LIBS_WITH_PATH)
endif

if ROSE_USE_GCC_OMP
   librose_la_LIBADD += lgomp
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
if OS_MACOSX
if HAS_LIBRARY_GCRYPT
PORT_PATH = $(shell which port)
PORT_HOME = $(shell dirname $(PORT_PATH))/..
LDFLAGS += -L$(PORT_HOME)/lib -lgcrypt -lgcc_eh
else
LDFLAGS += -lgcc_eh
endif
else
LDFLAGS =
endif

testSharedRoseLib_SOURCES = testRoseLib.C
testSharedRoseLib_DEPENDENCIES = $(REQUIRED_LIBS)
testSharedRoseLib_LDADD  = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
# testSharedRoseLib_LDFLAGS  = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(ROSE_DWARF_LIBS_WITH_PATH)
# testSharedRoseLib_LDFLAGS  = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
# testSharedRoseLib_LDFLAGS  = $(ROSE_DWARF_LIBS_WITH_PATH)

# AM_LDFLAGS = $(ROSE_LIBS) $(ROSE_DWARF_LIBS_WITH_PATH)
# AM_LDFLAGS = $(ROSE_DWARF_LIBS_WITH_PATH)
# AM_LDFLAGS =

# This tool spits out information that tells users how to compile and link programs that use ROSE. Therefore, we have to pass
# that information to the program when we compile it, and we do so with the "-DCXX", etc compiler switches.
bin_PROGRAMS += rose-config
rose_config_SOURCES = rose-config.C
rose_config_DEPENDENCIES = $(REQUIRED_LIBS)

rose_config_installed_cppflags =		\
	-I@includedir@/rose			\
	$(ROSE_QT_INCLUDE)			\
	$(ROSE_SQLITE_DATABASE_INCLUDE)		\
	$(ROSE_PHP_INCLUDES)			\
	$(ROSE_YICES_INCLUDES)			\
	$(ROSE_YAML_INCLUDES)			\
	$(ROSE_LIBMAGIC_INCLUDES)		\
	$(ROSE_DWARF_INCLUDES)			\
	$(ROSE_WINE_INCLUDES)			\
	$(ROSE_PCH_INCLUDE)			\
	$(INTEL_PIN_INCLUDE_PATHS)		\
	$(ROSE_ETHER_INCLUDE)			\
	$(ROSE_ATERM_INCLUDE)			\
	$(BOOST_CPPFLAGS)
rose_config_build_cppflags = $(ROSE_INCLUDES)

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
	$(JAVA_JVM_LINK)			\
	$(SQLITE_DATABASE_LIBS)			\
	$(RT_LIBS)				\
	$(ROSE_YICES_LIBS_WITH_PATH)		\
	$(ROSE_PHP_LIBS_WITH_PATH)		\
	$(ROSE_DWARF_LIBS_WITH_PATH)		\
	$(ROSE_QT_LIBS_WITH_PATH)		\
	$(ROSE_SSL_LIBS)			\
	$(ROSE_ETHER_LIBS)			\
	$(ROSE_INTEL_COMPILER_MATH_LIBS)	\
	$(ROSE_ATERM_LIBS)			\
	$(ROSE_YAML_LIBS_WITH_PATH)		\
	$(ROSE_LIBMAGIC_LIBS_WITH_PATH)		\
	-lm
rose_config_build_ldflags =			\
	$(LIBS_WITH_RPATH)			\
	$(ROSE_LIBS)

rose_config_CPPFLAGS =									\
	-DCXX='"$(subst ",\",$(CXX))"'							\
	-DINSTALLED_CPPFLAGS='"$(subst ",\",$(rose_config_installed_cppflags))"'	\
	-DBUILD_CPPFLAGS='"$(subst ",\",$(rose_config_build_cppflags))"'		\
	-DCFLAGS='"$(subst ",\",$(CFLAGS))"'						\
	-DINSTALLED_LDFLAGS='"$(subst ",\",$(rose_config_installed_ldflags))"'		\
	-DBUILD_LDFLAGS='"$(subst ",\",$(rose_config_build_ldflags))"'

rose_config_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

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








