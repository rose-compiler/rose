include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = util ROSETTA frontend midend backend roseSupport \
          3rdPartyLibraries roseIndependentSupport roseExtensions

# This lists a large number of directories.  If "make install"
# is run then we only need a single directory
INCLUDES = $(ROSE_INCLUDES)


# DQ (1/16/2007): The binary release of ROSE requires some libraries to be distributed.
# if ROSE_USE_EDG_SOURCE
# # This is a distribution containing the EDG source code in the source tree (distribution or 
# # development version checked out from CVS).  These libraries are available only as binaries 
# # in the binary version of the ROSE distribution (but are in the compile tree since this 
# # distribution includes the EDG source code).
# libroseEDGlibraries = \
#    $(top_builddir)/src/frontend/CxxFrontend/EDG_3.3/src/libedg33.la \
#    $(top_builddir)/src/frontend/CxxFrontend/EDG_SAGE_Connection/libedgSage.la
# else
# # This is a distribution containing binaries in the source tree (distribution).
# # These versions are available only as binaries and stored in the distribution (in this case).
# libroseEDGlibraries = \
#    $(top_srcdir)/src/frontend/CxxFrontend/EDG_3.3/src/libedg33.la \
#    $(top_builddir)/src/frontend/CxxFrontend/EDG_SAGE_Connection/libedgSage.la
# endif

# DQ (7/23/2008): New handling of EDG builds a single library instead of two.
# Binary distributions of ROSE store the binary for EDG related code in the source tree.
if ROSE_USE_EDG_SOURCE
#  libroseEDGlibraries = $(top_builddir)/src/frontend/CxxFrontend/libroseEDG.la
   libroseEDGlibraries = frontend/CxxFrontend/libroseEDG.la
else
 # Note that this is the wrong location for libroseEDG.la if one mistakenly configures
 # a development version (from svn) of ROSE without specifying "--with-edg_source_code=true"
 # (as is the case when preparing a binary distribution, but then only "make dist" 
 # or "make distcheck" should be run).
   libroseEDGlibraries = $(top_srcdir)/src/frontend/CxxFrontend/libroseEDG.la
 # libroseEDGlibraries = frontend/CxxFrontend/libroseEDG.la
endif

libroseBinaryLibraries = \
   $(top_builddir)/src/frontend/BinaryDisassembly/libbinaryFrontend.la \
   $(top_builddir)/src/frontend/Disassemblers/libroseDisassemblers.la \
   $(top_builddir)/src/frontend/ExecFormats/libroseExecFormats.la \
   $(top_builddir)/src/backend/asmUnparser/libroseAsmUnparser.la 

# TODO: correct list!
libroseLibraries = \
   $(top_builddir)/src/3rdPartyLibraries/coco/cplus2/libcr.la \
   $(top_builddir)/src/3rdPartyLibraries/MSTL/libmstl.la \
   $(top_builddir)/src/backend/unparser/libunparse.la \
   $(top_builddir)/src/backend/unparser/languageIndependenceSupport/libunparseLanguageIndependenceSupport.la \
   $(top_builddir)/src/backend/unparser/CxxCodeGeneration/libunparseCxx.la \
   $(top_builddir)/src/backend/unparser/FortranCodeGeneration/libunparseFortran.la \
   $(top_builddir)/src/backend/unparser/formatSupport/libunparseFormatSupport.la \
   $(top_builddir)/src/frontend/SageIII/libsage3.la \
   $(top_builddir)/src/frontend/OpenFortranParser_SAGE_Connection/libroseFortran.la \
   $(libroseEDGlibraries) \
   $(libroseBinaryLibraries) \
   $(libroseRuntimeTransformation) \
   $(top_builddir)/src/midend/astInlining/libastinlining.la \
   $(top_builddir)/src/midend/astOutlining/libastOutliner.la \
   $(top_builddir)/src/midend/astUtil/astInterface/libastInterface.la \
   $(top_builddir)/src/midend/astUtil/astSupport/libastSupport.la \
   $(top_builddir)/src/midend/astUtil/symbolicVal/libsymbolicVal.la \
   $(top_builddir)/src/midend/astUtil/annotation/libannotation.la \
   $(top_builddir)/src/midend/astQuery/libquery.la \
   $(top_builddir)/src/midend/astProcessing/libastprocessing.la \
   $(top_builddir)/src/midend/astRewriteMechanism/librewrite.la \
   $(top_builddir)/src/midend/astDiagnostics/libastDiagnostics.la \
   $(top_builddir)/src/midend/programAnalysis/CallGraphAnalysis/libCallGraph.la \
   $(top_builddir)/src/midend/programAnalysis/OAWrap/libOAWrap.la \
   $(top_builddir)/src/midend/programAnalysis/CFG/libCFG.la \
   $(top_builddir)/src/midend/programAnalysis/dataflowAnalysis/libdataflowAnalysis.la \
   $(top_builddir)/src/midend/programAnalysis/pointerAnal/libpointerAnal.la \
   $(top_builddir)/src/midend/programAnalysis/valuePropagation/libvaluePropagation.la \
   $(top_builddir)/src/midend/programAnalysis/defUseAnalysis/libDefUseAnalysis.la \
   $(top_builddir)/src/midend/programAnalysis/staticInterproceduralSlicing/libStaticSlicing.la \
   $(top_builddir)/src/midend/programTransformation/partialRedundancyElimination/libpre.la \
   $(top_builddir)/src/midend/programTransformation/finiteDifferencing/libfiniteDifferencing.la \
   $(top_builddir)/src/midend/programTransformation/functionCallNormalization/libfunctionCallNormalization.la \
   $(top_builddir)/src/midend/programTransformation/constantFolding/libconstantFolding.la \
   $(top_builddir)/src/midend/programTransformation/runtimeTransformation/libruntimeTransformation.la \
   $(top_builddir)/src/midend/programTransformation/implicitCodeGeneration/libimplicitCodeGeneration.la \
   $(top_builddir)/src/midend/loopProcessing/prepostTransformation/libprepostloop.la \
   $(top_builddir)/src/midend/loopProcessing/depInfo/libdepInfo.la \
   $(top_builddir)/src/midend/loopProcessing/depGraph/libdepGraph.la \
   $(top_builddir)/src/midend/loopProcessing/computation/libcomputation.la \
   $(top_builddir)/src/midend/loopProcessing/slicing/libslicing.la \
   $(top_builddir)/src/midend/loopProcessing/driver/libdriver.la \
   $(top_builddir)/src/roseSupport/libroseSupport.la \
   $(top_builddir)/src/util/support/libsupport.la \
   $(top_builddir)/src/util/graphs/libgraphs.la \
   $(top_builddir)/src/util/libroseutil.la


if ROSE_USE_EDG_SOURCE

testEdgSourceRule:
	echo "EDG source code will be placed into any ROSE distribution ... "

EXTRA_DIST = extra.docs

dist-hook:
	echo "dist-hook required for symmetry with binary case ..."

else

testEdgSourceRule:
	echo "EDG source code not permitted in any ROSE distribution ... "

# DQ (4/21/2005): We should not have to define this (should be defined by automake as elsewhere in ROSE)
libLIBRARIES_INSTALL = /usr/bin/install -c -m 644

EXTRA_DIST = extra.docs

dist-hook:
	echo "Commented out Copying the libedg.so to $(distdir)"
#	cp -p $(libdir)/libedg.so $(distdir)

# # copy the binary from the distribution (make rule allows for dependences upon libedg.so)
# $(libdir)/libedg.so:
# 	@echo "Copying binary EDG specific library (libedg.so)"
# 	@if ! test -f $(srcdir)/libedg.so; then \
#       echo "Error: libedg.so does not exist, configure using --with-edg_source_code=true so that EDG source code can be used to build libedg.so"; \
#       exit 1; \
#    fi
# 	@echo "Copying $(srcdir)/libedg.so to $(libdir)/libedg.so"
# 	cp -p $(srcdir)/libedg.so $(libdir)/libedg.so

endif

# Header files that need to be included in the distribution
include_HEADERS = \
     rose.h \
     roseInternal.h

# DQ (9/22/2006): Because we now use libtool, the shared library is a reference to the separate
# dynamic libraries in each directory and so testMultiRoseLib is redundant with testSharedRoseLib.
# This is true only if we have configured --with-development-libraries, else we have static
# in each directory and we will link with the collection of all static libraries.
# DQ (9/8/2006): The ROSE/docs/Rose/Makefile references the testSharedRoseLib program, so maybe is should be installed?
# bin_PROGRAMS = testSharedRoseLib # testMultiRoseLib
noinst_PROGRAMS = testSharedRoseLib 

# PC (9/15/2006): force the use of the C++ linker
LINK = $(CXXLINK)

lib_LTLIBRARIES = librose.la

if ROSE_USE_OPEN_FORTRAN_PARSER
   JAVA_JVM_FLAGS = $(JAVA_JVM_LINK)
endif

librose_la_SOURCES = dummyCppFileForLibrose.C
librose_la_LIBADD = $(libroseLibraries) $(LIB_QT) $(JAVA_JVM_FLAGS) -lm $(RT_LIBS) \
                    $(BOOST_LDFLAGS) $(BOOST_DATE_TIME_LIB) \
		    $(BOOST_THREAD_LIB) $(BOOST_FILESYSTEM_LIB) \
		    $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_REGEX_LIB) \
		    $(BOOST_SYSTEM_LIB) $(BOOST_SERIALIZATION_LIB) \
		    $(BOOST_WAVE_LIB) \
		    -L$(top_builddir)/src/3rdPartyLibraries/libharu-2.1.0/src -lhpdf
librose_la_LDFLAGS = -no-undefined
librose_la_DEPENDENCIES = $(libroseLibraries)

# Build list of all required libraries so that they can be built in parallel
# REQUIRED_LIBS = $(libdir)/libedg.la
REQUIRED_LIBS = librose.la 


testSharedRoseLib_SOURCES = testRoseLib.C
testSharedRoseLib_DEPENDENCIES = $(REQUIRED_LIBS)
testSharedRoseLib_LDADD  = $(LIBS_WITH_RPATH) $(ROSE_LIBS) 

print:
	@echo "libroseLibraries = $(libroseLibraries)"

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
	rm -f frontend/CxxFrontend/EDG_3.3/util/edg_prelink.lo
	rm -f frontend/SageIII/Cxx_Grammar.lo
	rm -f roseSupport/templateSupport.lo
	$(MAKE)








