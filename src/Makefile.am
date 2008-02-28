include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (8/21/2005):
# Note: When the vendor compiler name is changed use:
#      grep -r "/usr/apps/gcc/3.4.3/bin/g++" *
# to find locations of which libraries to remove and 
# force to be recompiled.

# DQ: Note that 3rdPartyLibraries/MSTL and roseSupport require 
#     frontend/SageIII to be built
# SUBDIRS = util ROSETTA frontend midend backend roseSupport \
#           3rdPartyLibraries roseIndependentSupport roseExtensions

# DQ (9/3/2005): Try to move roseIndependentSupport up in the order 
# while we debug makefile system for QRose.
SUBDIRS = util ROSETTA frontend midend backend roseSupport \
          3rdPartyLibraries roseIndependentSupport roseExtensions

# This lists a large number of directories.  If "make install"
# is run then we only need a single directory
INCLUDES = $(ROSE_INCLUDES)

if ROSE_USE_EDG_SOURCE

# DQ (1/13/2005): Added prelink object file to EDG library to remove 
# installation problem in locating EDG executables from ROSE generated 
# translators.  The should simplify use of ROSE!

# Note: Both EDG and the EDG-to-SageIII code are placed into 
# the same library since at some point this library will be
# distributed as a binary file (to avoid distribution of the
# EDG source code as required by the EDG license).
libedgObjects = \
   $(top_builddir)/src/frontend/EDG/EDG_3.3/util/decode.lo \
   $(top_builddir)/src/frontend/EDG/EDG_3.3/util/edg_prelink.lo \
   $(top_builddir)/src/frontend/EDG/EDG_3.3/src/*.lo \
   $(top_builddir)/src/frontend/EDG_SAGE_Connection/*.lo

#   $(top_builddir)/src/frontend/EDG/EDG_3.3/util/edg_decode.o 

endif

# Conditional support for Gabriel's QRose GUI Library 
if ROSE_USE_QROSE
# ROSE GUI objects
ROSE_GUI_OBJS = \
   $(top_builddir)/src/roseIndependentSupport/graphicalUserInterface/src/QRGui/*.o \
   $(top_builddir)/src/roseIndependentSupport/graphicalUserInterface/src/QRCodeBox/*.o \
   $(top_builddir)/src/roseIndependentSupport/graphicalUserInterface/src/QRTree/*.o \
   $(top_builddir)/src/roseIndependentSupport/graphicalUserInterface/src/QRQueryBox/*.o
endif

if ROSE_USE_BINARY_ANALYSIS_SUPPORT
ROSE_BINARY_DISASSEMBLER_OBJS = \
   $(top_builddir)/src/frontend/BinaryDisassembly/RoseBin_support.o \
   $(top_builddir)/src/frontend/BinaryDisassembly/RoseBin_unparse.o \
   $(top_builddir)/src/frontend/BinaryDisassembly/RoseBin_unparse_visitor.o \
   $(top_builddir)/src/frontend/BinaryDisassembly/analyses/RoseBin_CompareAnalysis.o \
   $(top_builddir)/src/frontend/BinaryDisassembly/analyses/RoseBin_ControlFlowAnalysis.o \
   $(top_builddir)/src/frontend/BinaryDisassembly/analyses/RoseBin_printInstructions.o \
   $(top_builddir)/src/frontend/BinaryDisassembly/analyses/RoseBin_DataFlowAnalysis.o \
   $(top_builddir)/src/frontend/BinaryDisassembly/graph/RoseBin_DotGraph.o \
   $(top_builddir)/src/frontend/BinaryDisassembly/graph/RoseBin_Graph.o \
   $(top_builddir)/src/frontend/BinaryDisassembly/graph/RoseBin_GmlGraph.o \
   $(top_builddir)/src/frontend/BinaryDisassembly/analyses/RoseBin_CallGraphAnalysis.o \
   $(top_builddir)/src/frontend/BinaryDisassembly/analyses/RoseBin_FlowAnalysis.o \
   $(top_builddir)/src/frontend/BinaryDisassembly/analyses/dataflowanalyses/RoseBin_DefUseAnalysis.o \
   $(top_builddir)/src/frontend/BinaryDisassembly/analyses/dataflowanalyses/RoseBin_VariableAnalysis.o \
   $(top_builddir)/src/frontend/BinaryDisassembly/analyses/dataflowanalyses/RoseBin_Emulate.o \
   $(top_builddir)/src/frontend/BinaryDisassembly/analyses/dataflowanalyses/RoseBin_DataFlowAbstract.o \
   $(top_builddir)/src/frontend/ObjdumpDisassembler/*.o

libroseBinaryLibraries = \
   $(top_builddir)/src/frontend/BinaryDisassembly/libbinaryFrontend.la \
   $(top_builddir)/src/frontend/ObjdumpDisassembler/libroseObjdumpDisassembler.la 

endif

# Note: ROSE/src/*.o are not included since they are only
# related to the test program that is compiled to test the
# ROSE libraries (librose.a librose.so)
#
# RV (8/11/2005): Removed reference to PDFLibrary
#   $(top_builddir)/src/3rdPartyLibraries/PDFLibrary/*.o
#
libroseObjects = \
   $(top_builddir)/src/util/support/*.o \
   $(top_builddir)/src/util/graphs/*.o \
   $(top_builddir)/src/util/stringSupport/*.o \
   $(top_builddir)/src/util/commandlineProcessing/*.o \
   $(top_builddir)/src/3rdPartyLibraries/coco/cplus2/*.o \
   $(top_builddir)/src/3rdPartyLibraries/MSTL/*.o \
   $(top_builddir)/src/3rdPartyLibraries/$(PDFLIB_BASE)/libs/pdflib/*.o \
   $(top_builddir)/src/3rdPartyLibraries/$(PDFLIB_BASE)/libs/pdcore/*.o \
   $(top_builddir)/src/3rdPartyLibraries/$(PDFLIB_BASE)/libs/png/*.o \
   $(top_builddir)/src/3rdPartyLibraries/$(PDFLIB_BASE)/libs/tiff/*.o \
   $(top_builddir)/src/3rdPartyLibraries/$(PDFLIB_BASE)/libs/flate/*.o \
   $(top_builddir)/src/3rdPartyLibraries/$(PDFLIB_BASE)/libs/jpeg/*.o \
   $(top_builddir)/src/frontend/SageIII/*.o \
   $(top_builddir)/src/frontend/SageIII/astFixup/*.o \
   $(top_builddir)/src/frontend/SageIII/astPostProcessing/*.o \
   $(top_builddir)/src/frontend/SageIII/astMerge/*.o \
   $(top_builddir)/src/frontend/SageIII/astFileIO/*.o \
   $(top_builddir)/src/frontend/SageIII/sageInterface/*.o \
   $(top_builddir)/src/frontend/SageIII/virtualCFG/*.o \
   $(top_builddir)/src/frontend/OpenFortranParser_SAGE_Connection/*.o \
   $(ROSE_BINARY_DISASSEMBLER_OBJS) \
   $(top_builddir)/src/midend/astUtil/astInterface/*.o \
   $(top_builddir)/src/midend/astUtil/astSupport/*.o \
   $(top_builddir)/src/midend/astUtil/symbolicVal/*.o \
   $(top_builddir)/src/midend/astUtil/annotation/*.o \
   $(top_builddir)/src/midend/astInlining/*.o \
   $(top_builddir)/src/midend/astOutlining/*.o \
   $(top_builddir)/src/midend/programTransformation/partialRedundancyElimination/*.o \
   $(top_builddir)/src/midend/programTransformation/finiteDifferencing/*.o \
   $(top_builddir)/src/midend/programTransformation/functionCallNormalization/*.o \
   $(top_builddir)/src/midend/programTransformation/constantFolding/*.o \
   $(top_builddir)/src/midend/programTransformation/implicitCodeGeneration/*.o \
   $(top_builddir)/src/midend/astQuery/*.o \
   $(top_builddir)/src/midend/astDiagnostics/*.o \
   $(top_builddir)/src/midend/astProcessing/*.o \
   $(top_builddir)/src/midend/astRewriteMechanism/*.o \
   $(top_builddir)/src/midend/programAnalysis/CallGraphAnalysis/*.o \
   $(top_builddir)/src/midend/programAnalysis/OAWrap/*.o \
   $(top_builddir)/src/midend/programAnalysis/CFG/*.o \
   $(top_builddir)/src/midend/programAnalysis/dataflowAnalysis/*.o \
   $(top_builddir)/src/midend/programAnalysis/pointerAnal/*.o \
   $(top_builddir)/src/midend/programAnalysis/valuePropagation/*.o \
   $(top_builddir)/src/midend/programAnalysis/defUseAnalysis/*.o \
   $(top_builddir)/src/midend/programAnalysis/staticInterproceduralSlicing/*.o \
   $(ROSE_SIDEEFFECTS_OBJS) \
   $(top_builddir)/src/midend/loopProcessing/depInfo/*.o \
   $(top_builddir)/src/midend/loopProcessing/depGraph/*.o \
   $(top_builddir)/src/midend/loopProcessing/computation/*.o \
   $(top_builddir)/src/midend/loopProcessing/slicing/*.o \
   $(top_builddir)/src/midend/loopProcessing/prepostTransformation/*.o \
   $(top_builddir)/src/midend/loopProcessing/driver/*.o \
   $(top_builddir)/src/midend/loopProcessing/*.o \
   $(top_builddir)/src/backend/unparser/*.o \
   $(top_builddir)/src/backend/unparser/languageIndependenceSupport/*.o \
   $(top_builddir)/src/backend/unparser/CxxCodeGeneration/*.o \
   $(top_builddir)/src/backend/unparser/FortranCodeGeneration/*.o \
   $(top_builddir)/src/backend/unparser/formatSupport/*.o \
   $(top_builddir)/src/roseSupport/*.o \
   $(ROSE_DATABASE_OBJS) \
   $(ROSE_SQLITE_DATABASE_OBJS) \
   $(ROSE_GUI_OBJS)

# DQ (1/16/2007): The binary release of ROSE requires some libraries to be distributed.
if ROSE_USE_EDG_SOURCE
# This is a distribution containing the EDG soruce code in the source tree (distribution or 
# development version checked out from CVS).  These libraries are available only as binaries 
# in the binary version of the ROSE distribution (but are in the compile tree since this 
# distribution includes the EDG source code).
libroseEDGlibraries = \
   $(top_builddir)/src/frontend/EDG/EDG_3.3/src/libedg33.la \
   $(top_builddir)/src/frontend/EDG/EDG_3.3/util/libedgutil.la \
   $(top_builddir)/src/frontend/EDG_SAGE_Connection/libedgSage.la
else
# This is a distribution containing binaries in the source tree (distribution).
# These versions are available only as binaries and stored in the distribution (in this case).
libroseEDGlibraries = \
   $(top_srcdir)/src/frontend/EDG/EDG_3.3/src/libedg33.la \
   $(top_srcdir)/src/frontend/EDG/EDG_3.3/util/libedgutil.la \
   $(top_builddir)/src/frontend/EDG_SAGE_Connection/libedgSage.la
endif

# TODO: correct list!
libroseLibraries = \
   $(top_builddir)/src/3rdPartyLibraries/coco/cplus2/libcr.la \
   $(top_builddir)/src/3rdPartyLibraries/MSTL/libmstl.la \
   $(top_builddir)/src/3rdPartyLibraries/PDFlib-Lite-6.0.2rv/libs/pdflib/libpdf.la \
   $(top_builddir)/src/backend/unparser/libunparse.la \
   $(top_builddir)/src/backend/unparser/languageIndependenceSupport/libunparseLanguageIndependenceSupport.la \
   $(top_builddir)/src/backend/unparser/CxxCodeGeneration/libunparseCxx.la \
   $(top_builddir)/src/backend/unparser/FortranCodeGeneration/libunparseFortran.la \
   $(top_builddir)/src/backend/unparser/formatSupport/libunparseFormatSupport.la \
   $(top_builddir)/src/frontend/SageIII/libsage3.la \
   $(top_builddir)/src/frontend/OpenFortranParser_SAGE_Connection/libroseFortran.la \
   $(libroseEDGlibraries) \
   $(libroseBinaryLibraries) \
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


# DQ (1/24/2008): moved files to consolidate this directory (too many small subdirectories).
#   $(top_builddir)/src/midend/astOutlining/src/libliaOutliner.la 

# DQ (5/3/2007): Removed older work
#   $(top_builddir)/src/midend/programAnalysis/dominatorTreesAndDominanceFrontiers/libdominance.la 

#  QY: removed astOutlining temporarity because it doesnt compile. Alin will fix it later
#   $(top_builddir)/src/midend/astOutlining/*.o \
# DQ (10/22/2004): Removed since I could not make it work easily with "make distcheck" rule
# $(top_builddir)/src/midend/programAnalysis/annotationLanguageParser/*.o

# This should be in ROSE/src before we include it into ROSE/src/librose.so
#  $(ROSE_DATABASE_OBJS)

# Old way of specifying libraries (test it to make sure it still works)
# RV (8/11/2005): Removed reference to PDFLibrary
#   $(top_builddir)/src/3rdPartyLibraries/PDFLibrary/libpdf.a
#OLD_MULTI_ROSE_LIBS = \
#   $(top_builddir)/src/frontend/SageIII/libsage3.a \
#   $(top_builddir)/src/midend/astRewriteMechanism/librewrite.a \
#   $(top_builddir)/src/midend/astQuery/libquery.a \
#   $(EDG_LIBS) \
#   $(top_builddir)/src/3rdPartyLibraries/$(PDFLIB_BASE)/libpdf.a \
#   $(top_builddir)/src/util/libroseutil.a \
#   $(top_builddir)/src/roseSupport/libroseSupport.a \
#   $(top_builddir)/src/3rdPartyLibraries/MSTL/libmstl.a \
#   $(top_builddir)/src/3rdPartyLibraries/coco/cplus2/libcr.a \
#   $(top_builddir)/src/midend/astProcessing/libastDiagnostics.a \
#   $(top_builddir)/src/midend/astProcessing/libastprocessing.a \
#   $(top_builddir)/src/midend/astRewriteMechanism/librewrite.a \
#   $(top_builddir)/src/frontend/SageIII/libsage3.a \
#   $(top_builddir)/src/util/libroseutil.a \
#   $(top_builddir)/src/roseSupport/libroseSupport.a \
#   $(top_builddir)/src/frontend/SageIII/libsage3.a \
#   $(top_builddir)/src/frontend/OpenFortranParser_SAGE_Connection/libroseFortran.a \
#   $(EDG_LIBS) \
#   $(top_builddir)/src/util/libroseutil.a \
#   $(top_builddir)/src/3rdPartyLibraries/PDFLibrary/libpdf.a \
#   $(top_builddir)/src/3rdPartyLibraries/coco/cplus2/libcr.a \
#   $(top_builddir)/src/midend/astProcessing/libastprocessing.a \
#   $(top_builddir)/src/midend/astQuery/libquery.a \
#   $(top_builddir)/src/backend/unparser/libunparse.a \
#   $(top_builddir)/src/backend/unparser/languageIndependenceSupport/libunparseLanguageIndependenceSupport.a \
#   $(top_builddir)/src/backend/unparser/CxxCodeGeneration/libunparseCxx.a \
#   $(top_builddir)/src/backend/unparser/FortranCodeGeneration/libunparseFortran.a \
#   $(top_builddir)/src/backend/unparser/formatSupport/libunparseFormatSupport.a \
#   $(EDG_LIBS) \
#   $(top_builddir)/src/util/libroseutil.a \
#   $(top_builddir)/src/roseSupport/libroseSupport.a \
#   -lm $(LEXLIB)

# These are optional and dependent upon the database support being configured with ROSE 
# (see configure --help, for details). (Note: make variables are empty if not defined!)
# if ROSE_USE_DATABASE
#   ROSE_SIDEEFFECT_LIBS = lib$(ROSE_SIDEEFFECTS_LIB_NAME).so
#   ROSE_DATABASE_LIBS   = lib$(ROSE_DATABASE_LIB_NAME).so
# endif

# DQ (7/19/2004)
# This rule builds links from the dynamic library in the specific subdirectory into
# the main ROSE/src level directory.  This permits the simplified use of ROSE during
# development by inserting the smaller library (e.g. -lunparse) ahead of -lrose
# and avoiding the complete relinking of the dynamic librose.so library (which takes
# about 30 seconds and is long enough to be annoying).
# MULTI_ROSE_SO_LIBS = \
#   $(libdir)/libRoseStringSupport.so $(libdir)/libRoseCommandlineUtil.so $(libdir)/libsupport.so $(libdir)/libgraphs.so $(libdir)/libroseutil.so \
#   $(libdir)/libcr.so $(libdir)/libmstl.so $(libdir)/libpdf.so $(libdir)/libsage3.so $(libdir)/libedgutil.so $(libdir)/libedg33.so $(libdir)/libedgSage.so $(libdir)/libastinlining.so \
#   $(libdir)/libfiniteDifferencing.so $(libdir)/libpre.so $(libdir)/libconstantFolding.so $(libdir)/libimplicitCodeGeneration.so \
#   $(libdir)/libastoutlining.so $(libdir)/libastInterface.so $(libdir)/libastSupport.so $(libdir)/libsymbolicVal.so  $(libdir)/libannotation.so \
#   $(libdir)/libquery.so $(libdir)/libastprocessing.so $(libdir)/librewrite.so $(libdir)/libOAWrap.so $(libdir)/libCFG.so $(libdir)/libdataflowAnalysis.so \
#   $(libdir)/libvaluePropagation.so $(libdir)/libdepInfo.so $(libdir)/libdepGraph.so $(libdir)/libfunctionCallNormalization.so \
#   $(libdir)/libcomputation.so $(libdir)/libslicing.so $(libdir)/libdriver.so $(libdir)/libprepostloop.so $(libdir)/libdriver.so \
#   $(libdir)/libunparse.so $(libdir)/libroseSupport.so $(libdir)/libastDiagnostics.so \
#   $(libdir)/libCallGraph.so $(ROSE_SQLITE_DATABASE_LIB_FILE) \
#   $(ROSE_SIDEEFFECTS_LIB_FILE) $(ROSE_DATABASE_LIB_FILE)

# QY (11/2/04): removed libastoutlining.so from MULTI_ROSE_SO_LIBS

# DQ (10/22/2004): Removed since I could not make it work easily with "make distcheck" rule
# libannotationLanguageParser.so 

# Define in the common ROSE makefile (in ROSE/config)
MULTI_ROSE_LIBS = $(ROSE_SEPARATE_LIBS)

# DQ (4/23/2005): If ROSE is configured with out "--with-edg_source_code=true"
# then the following are an error.  We detect the lack of each library and
# output a message to use the correct option with configure so that the EDG
# source files can be used in development!  since the binaries are not checked
# into CVS (they should not be).
# These rules build links from the location of the library to a location in ROSE/src
# this allows the specification of a single "-LROSE/src -Wl,-rpath ROSE/src" instead
# of one for each directory.
$(libdir)/libRoseStringSupport.so: $(top_builddir)/src/util/stringSupport/libRoseStringSupport.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libslibRoseStringSupport.so   || ln -s $(top_builddir)/src/util/stringSupport/libRoseStringSupport.so $(libdir)/libRoseStringSupport.so
#	test -f $(libdir)/libslibRoseStringSupport.so   || cp $(top_builddir)/src/util/stringSupport/libRoseStringSupport.so $(libdir)/libRoseStringSupport.so
	cp $(top_builddir)/src/util/stringSupport/libRoseStringSupport.so $(libdir)/libRoseStringSupport.so

$(libdir)/libRoseCommandlineUtil.so: $(top_builddir)/src/util/commandlineProcessing/libRoseCommandlineUtil.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libRoseCommandlineUtil.so || ln -s $(top_builddir)/src/util/commandlineProcessing/libRoseCommandlineUtil.so $(libdir)/libRoseCommandlineUtil.so
#	test -f $(libdir)/libRoseCommandlineUtil.so || cp $(top_builddir)/src/util/commandlineProcessing/libRoseCommandlineUtil.so $(libdir)/libRoseCommandlineUtil.so
	cp $(top_builddir)/src/util/commandlineProcessing/libRoseCommandlineUtil.so $(libdir)/libRoseCommandlineUtil.so

$(libdir)/libsupport.so: $(top_builddir)/src/util/support/libsupport.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libsupport.so             || ln -s $(top_builddir)/src/util/support/libsupport.so $(libdir)/libsupport.so
#	test -f $(libdir)/libsupport.so             || cp $(top_builddir)/src/util/support/libsupport.so $(libdir)/libsupport.so
	cp $(top_builddir)/src/util/support/libsupport.so $(libdir)/libsupport.so

$(libdir)/libgraphs.so: $(top_builddir)/src/util/graphs/libgraphs.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libgraphs.so              || ln -s $(top_builddir)/src/util/graphs/libgraphs.so $(libdir)/libgraphs.so
#	test -f $(libdir)/libgraphs.so              || cp $(top_builddir)/src/util/graphs/libgraphs.so $(libdir)/libgraphs.so
	cp $(top_builddir)/src/util/graphs/libgraphs.so $(libdir)/libgraphs.so

$(libdir)/libroseutil.so: $(top_builddir)/src/util/libroseutil.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libroseutil.so            || ln -s $(top_builddir)/src/util/libroseutil.so $(libdir)/libroseutil.so
#	test -f $(libdir)/libroseutil.so            || cp $(top_builddir)/src/util/libroseutil.so $(libdir)/libroseutil.so
	cp $(top_builddir)/src/util/libroseutil.so $(libdir)/libroseutil.so

$(libdir)/libcr.so: $(top_builddir)/src/3rdPartyLibraries/coco/cplus2/libcr.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libcr.so                  || ln -s $(top_builddir)/src/3rdPartyLibraries/coco/cplus2/libcr.so $(libdir)/libcr.so
#	test -f $(libdir)/libcr.so                  || cp $(top_builddir)/src/3rdPartyLibraries/coco/cplus2/libcr.so $(libdir)/libcr.so
	cp $(top_builddir)/src/3rdPartyLibraries/coco/cplus2/libcr.so $(libdir)/libcr.so

$(libdir)/libmstl.so: $(top_builddir)/src/3rdPartyLibraries/MSTL/libmstl.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libmstl.so                || ln -s $(top_builddir)/src/3rdPartyLibraries/MSTL/libmstl.so $(libdir)/libmstl.so
#	test -f $(libdir)/libmstl.so                || cp $(top_builddir)/src/3rdPartyLibraries/MSTL/libmstl.so $(libdir)/libmstl.so
	cp $(top_builddir)/src/3rdPartyLibraries/MSTL/libmstl.so $(libdir)/libmstl.so

$(libdir)/libpdf.so:
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
# RV (8/11/2005): Removed reference to PDFLibrary
#  test -f libpdf.so                 || ln -s $(top_builddir)/src/3rdPartyLibraries/PDFLibrary/libpdf.so libpdf.so
	pdfdir=$(top_builddir)/src/3rdPartyLibraries/$(PDFLIB_BASE)/libs/pdflib/.libs ; \
	for f in $$pdfdir/libpdf*.so* ; do \
		bf=`basename $$f` ; \
		cp $$pdfdir/$$bf $(libdir)/$$bf ; \
	done

#		test -f $(libdir)/$$bf || ln -s $$pdfdir/$$bf $(libdir)/$$bf ; 
#		test -f $(libdir)/$$bf || cp $$pdfdir/$$bf $(libdir)/$$bf ;

$(libdir)/libsage3.so: $(top_builddir)/src/frontend/SageIII/libsage3.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libsage3.so               || ln -s $(top_builddir)/src/frontend/SageIII/libsage3.so $(libdir)/libsage3.so
#	test -f $(libdir)/libsage3.so               || cp $(top_builddir)/src/frontend/SageIII/libsage3.so $(libdir)/libsage3.so
	cp $(top_builddir)/src/frontend/SageIII/libsage3.so $(libdir)/libsage3.so

# DQ (8/27/2005): this is now included in libsage3.[a,so]
#libastMerge.so:
#	@echo "Adding links to separate dynamic libraries to simplify development ..."
#	test -f libastMerge.so            || ln -s $(top_builddir)/src/frontend/SageIII/astMerge/libastMerge.so libastMerge.so

$(libdir)/libedg33.so: $(top_builddir)/src/frontend/EDG/EDG_3.3/src/libedg33.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to libedg33.so)"
	if test -f $(top_builddir)/src/frontend/EDG/EDG_3.3/src/libedg33.so; then \
      echo "Error: no $(top_builddir)/src/frontend/EDG/EDG_3.3/src/libedg33.so exists, configure using --with-edg_source_code=true so that EDG source code can be used to build libedg33.so"; \
   fi
#	test -f $(libdir)/libedg33.so               || ln -s $(top_builddir)/src/frontend/EDG/EDG_3.3/src/libedg33.so $(libdir)/libedg33.so
#	test -f $(libdir)/libedg33.so               || cp $(top_builddir)/src/frontend/EDG/EDG_3.3/src/libedg33.so $(libdir)/libedg33.so
	cp $(top_builddir)/src/frontend/EDG/EDG_3.3/src/libedg33.so $(libdir)/libedg33.so

$(libdir)/libedgutil.so: $(top_builddir)/src/frontend/EDG/EDG_3.3/util/libedgutil.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libedgutil.so             || ln -s $(top_builddir)/src/frontend/EDG/EDG_3.3/util/libedgutil.so $(libdir)/libedgutil.so
#	test -f $(libdir)/libedgutil.so             || cp $(top_builddir)/src/frontend/EDG/EDG_3.3/util/libedgutil.so $(libdir)/libedgutil.so
	cp $(top_builddir)/src/frontend/EDG/EDG_3.3/util/libedgutil.so $(libdir)/libedgutil.so

$(libdir)/libedgSage.so: $(top_builddir)/src/frontend/EDG_SAGE_Connection/libedgSage.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libedgSage.so             || ln -s $(top_builddir)/src/frontend/EDG_SAGE_Connection/libedgSage.so $(libdir)/libedgSage.so
#	test -f $(libdir)/libedgSage.so             || cp $(top_builddir)/src/frontend/EDG_SAGE_Connection/libedgSage.so $(libdir)/libedgSage.so
	cp $(top_builddir)/src/frontend/EDG_SAGE_Connection/libedgSage.so $(libdir)/libedgSage.so

$(libdir)/libastinlining.so: $(top_builddir)/src/midend/astInlining/libastinlining.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libastinlining.so         || ln -s $(top_builddir)/src/midend/astInlining/libastinlining.so $(libdir)/libastinlining.so
#	test -f $(libdir)/libastinlining.so         || cp $(top_builddir)/src/midend/astInlining/libastinlining.so $(libdir)/libastinlining.so
	cp $(top_builddir)/src/midend/astInlining/libastinlining.so $(libdir)/libastinlining.so

# DQ (8/2/2005): Added outlining library
$(libdir)/libastoutlining.so: $(top_builddir)/src/midend/astOutlining/libastoutlining.so
	@echo "Adding links to separate dynamic libraries to simplify development ..."
#	test -f $(libdir)/libastoutlining.so        || ln -s $(top_builddir)/src/midend/astOutlining/libastoutlining.so $(libdir)/libastoutlining.so
#	test -f $(libdir)/libastoutlining.so        || cp $(top_builddir)/src/midend/astOutlining/libastoutlining.so $(libdir)/libastoutlining.so
	cp $(top_builddir)/src/midend/astOutlining/libastoutlining.so $(libdir)/libastoutlining.so

$(libdir)/libfiniteDifferencing.so: $(top_builddir)/src/midend/programTransformation/finiteDifferencing/libfiniteDifferencing.so
	@echo "Adding links to separate dynamic libraries to simplify development ..."
#	test -f $(libdir)/libfiniteDifferencing.so  || ln -s $(top_builddir)/src/midend/programTransformation/finiteDifferencing/libfiniteDifferencing.so $(libdir)/libfiniteDifferencing.so
#	test -f $(libdir)/libfiniteDifferencing.so  || cp $(top_builddir)/src/midend/programTransformation/finiteDifferencing/libfiniteDifferencing.so $(libdir)/libfiniteDifferencing.so
	cp $(top_builddir)/src/midend/programTransformation/finiteDifferencing/libfiniteDifferencing.so $(libdir)/libfiniteDifferencing.so

$(libdir)/libpre.so: $(top_builddir)/src/midend/programTransformation/partialRedundancyElimination/libpre.so
	@echo "Adding links to separate dynamic libraries to simplify development ..."
#	test -f $(libdir)/libpre.so                 || ln -s $(top_builddir)/src/midend/programTransformation/partialRedundancyElimination/libpre.so $(libdir)/libpre.so
#	test -f $(libdir)/libpre.so                 || cp $(top_builddir)/src/midend/programTransformation/partialRedundancyElimination/libpre.so $(libdir)/libpre.so
	cp $(top_builddir)/src/midend/programTransformation/partialRedundancyElimination/libpre.so $(libdir)/libpre.so

$(libdir)/libconstantFolding.so: $(top_builddir)/src/midend/programTransformation/constantFolding/libconstantFolding.so
	@echo "Adding links to separate dynamic libraries to simplify development ..."
#	test -f $(libdir)/libconstantFolding.so     || ln -s $(top_builddir)/src/midend/programTransformation/constantFolding/libconstantFolding.so $(libdir)/libconstantFolding.so
#	test -f $(libdir)/libconstantFolding.so     || cp $(top_builddir)/src/midend/programTransformation/constantFolding/libconstantFolding.so $(libdir)/libconstantFolding.so
	cp $(top_builddir)/src/midend/programTransformation/constantFolding/libconstantFolding.so $(libdir)/libconstantFolding.so

$(libdir)/libimplicitCodeGeneration.so: $(top_builddir)/src/midend/programTransformation/implicitCodeGeneration/libimplicitCodeGeneration.so
	@echo "Adding links to separate dynamic libraries to simplify development ..."
#	test -f $(libdir)/libimplicitCodeGeneration.so || ln -s $(top_builddir)/src/midend/programTransformation/implicitCodeGeneration/libimplicitCodeGeneration.so $(libdir)/libimplicitCodeGeneration.so
#	test -f $(libdir)/libimplicitCodeGeneration.so || cp $(top_builddir)/src/midend/programTransformation/implicitCodeGeneration/libimplicitCodeGeneration.so $(libdir)/libimplicitCodeGeneration.so
	cp $(top_builddir)/src/midend/programTransformation/implicitCodeGeneration/libimplicitCodeGeneration.so $(libdir)/libimplicitCodeGeneration.so

$(libdir)/libastInterface.so: $(top_builddir)/src/midend/astUtil/astInterface/libastInterface.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libastInterface.so        || ln -s $(top_builddir)/src/midend/astUtil/astInterface/libastInterface.so $(libdir)/libastInterface.so
#	test -f $(libdir)/libastInterface.so        || cp $(top_builddir)/src/midend/astUtil/astInterface/libastInterface.so $(libdir)/libastInterface.so
	cp $(top_builddir)/src/midend/astUtil/astInterface/libastInterface.so $(libdir)/libastInterface.so

$(libdir)/libastSupport.so: $(top_builddir)/src/midend/astUtil/astSupport/libastSupport.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libastSupport.so          || ln -s $(top_builddir)/src/midend/astUtil/astSupport/libastSupport.so $(libdir)/libastSupport.so
#	test -f $(libdir)/libastSupport.so          || cp $(top_builddir)/src/midend/astUtil/astSupport/libastSupport.so $(libdir)/libastSupport.so
	cp $(top_builddir)/src/midend/astUtil/astSupport/libastSupport.so $(libdir)/libastSupport.so

$(libdir)/libsymbolicVal.so: $(top_builddir)/src/midend/astUtil/symbolicVal/libsymbolicVal.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libsymbolicVal.so         || ln -s $(top_builddir)/src/midend/astUtil/symbolicVal/libsymbolicVal.so $(libdir)/libsymbolicVal.so
#	test -f $(libdir)/libsymbolicVal.so         || cp $(top_builddir)/src/midend/astUtil/symbolicVal/libsymbolicVal.so $(libdir)/libsymbolicVal.so
	cp $(top_builddir)/src/midend/astUtil/symbolicVal/libsymbolicVal.so $(libdir)/libsymbolicVal.so

$(libdir)/libannotation.so: $(top_builddir)/src/midend/astUtil/annotation/libannotation.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libannotation.so          || ln -s $(top_builddir)/src/midend/astUtil/annotation/libannotation.so $(libdir)/libannotation.so
#	test -f $(libdir)/libannotation.so          || cp $(top_builddir)/src/midend/astUtil/annotation/libannotation.so $(libdir)/libannotation.so
	cp $(top_builddir)/src/midend/astUtil/annotation/libannotation.so $(libdir)/libannotation.so

$(libdir)/libquery.so: $(top_builddir)/src/midend/astQuery/libquery.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libquery.so               || ln -s $(top_builddir)/src/midend/astQuery/libquery.so $(libdir)/libquery.so
#	test -f $(libdir)/libquery.so               || cp $(top_builddir)/src/midend/astQuery/libquery.so $(libdir)/libquery.so
	cp $(top_builddir)/src/midend/astQuery/libquery.so $(libdir)/libquery.so

$(libdir)/libastprocessing.so: $(top_builddir)/src/midend/astProcessing/libastprocessing.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libastprocessing.so       || ln -s $(top_builddir)/src/midend/astProcessing/libastprocessing.so $(libdir)/libastprocessing.so
#	test -f $(libdir)/libastprocessing.so       || cp $(top_builddir)/src/midend/astProcessing/libastprocessing.so $(libdir)/libastprocessing.so
	cp $(top_builddir)/src/midend/astProcessing/libastprocessing.so $(libdir)/libastprocessing.so

$(libdir)/libastDiagnostics.so: $(top_builddir)/src/midend/astDiagnostics/libastDiagnostics.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libastDiagnostics.so       || ln -s $(top_builddir)/src/midend/astDiagnostics/libastDiagnostics.so $(libdir)/libastDiagnostics.so
#	test -f $(libdir)/libastDiagnostics.so       || cp $(top_builddir)/src/midend/astDiagnostics/libastDiagnostics.so $(libdir)/libastDiagnostics.so
	cp $(top_builddir)/src/midend/astDiagnostics/libastDiagnostics.so $(libdir)/libastDiagnostics.so

$(libdir)/librewrite.so: $(top_builddir)/src/midend/astRewriteMechanism/librewrite.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/librewrite.so             || ln -s $(top_builddir)/src/midend/astRewriteMechanism/librewrite.so $(libdir)/librewrite.so
#	test -f $(libdir)/librewrite.so             || cp $(top_builddir)/src/midend/astRewriteMechanism/librewrite.so $(libdir)/librewrite.so
	cp $(top_builddir)/src/midend/astRewriteMechanism/librewrite.so $(libdir)/librewrite.so

$(libdir)/libOAWrap.so: $(top_builddir)/src/midend/programAnalysis/OAWrap/libOAWrap.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libOAWrap.so              || ln -s $(top_builddir)/src/midend/programAnalysis/OAWrap/libOAWrap.so $(libdir)/libOAWrap.so
#	test -f $(libdir)/libOAWrap.so              || cp $(top_builddir)/src/midend/programAnalysis/OAWrap/libOAWrap.so $(libdir)/libOAWrap.so
	cp $(top_builddir)/src/midend/programAnalysis/OAWrap/libOAWrap.so $(libdir)/libOAWrap.so

$(libdir)/libCFG.so: $(top_builddir)/src/midend/programAnalysis/CFG/libCFG.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libCFG.so                 || ln -s $(top_builddir)/src/midend/programAnalysis/CFG/libCFG.so $(libdir)/libCFG.so
#	test -f $(libdir)/libCFG.so                 || cp $(top_builddir)/src/midend/programAnalysis/CFG/libCFG.so $(libdir)/libCFG.so
	cp $(top_builddir)/src/midend/programAnalysis/CFG/libCFG.so $(libdir)/libCFG.so

$(libdir)/libpointerAnal.so: $(top_builddir)/src/midend/programAnalysis/pointerAnal/libpointerAnal.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libpointerAnal.so    || ln -s $(top_builddir)/src/midend/programAnalysis/pointerAnal/libpointerAnal.so $(libdir)/libpointerAnal.so
#	test -f $(libdir)/libpointerAnal.so    || cp $(top_builddir)/src/midend/programAnalysis/pointerAnal/libpointerAnal.so $(libdir)/libpointerAnal.so
	cp $(top_builddir)/src/midend/programAnalysis/pointerAnal/libpointerAnal.so $(libdir)/libpointerAnal.so

$(libdir)/libdataflowAnalysis.so: $(top_builddir)/src/midend/programAnalysis/dataflowAnalysis/libdataflowAnalysis.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libdataflowAnalysis.so    || ln -s $(top_builddir)/src/midend/programAnalysis/dataflowAnalysis/libdataflowAnalysis.so $(libdir)/libdataflowAnalysis.so
#	test -f $(libdir)/libdataflowAnalysis.so    || cp $(top_builddir)/src/midend/programAnalysis/dataflowAnalysis/libdataflowAnalysis.so $(libdir)/libdataflowAnalysis.so
	cp $(top_builddir)/src/midend/programAnalysis/dataflowAnalysis/libdataflowAnalysis.so $(libdir)/libdataflowAnalysis.so

$(libdir)/libvaluePropagation.so: $(top_builddir)/src/midend/programAnalysis/valuePropagation/libvaluePropagation.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libvaluePropagation.so    || ln -s $(top_builddir)/src/midend/programAnalysis/valuePropagation/libvaluePropagation.so $(libdir)/libvaluePropagation.so
#	test -f $(libdir)/libvaluePropagation.so    || cp $(top_builddir)/src/midend/programAnalysis/valuePropagation/libvaluePropagation.so $(libdir)/libvaluePropagation.so
	cp $(top_builddir)/src/midend/programAnalysis/valuePropagation/libvaluePropagation.so $(libdir)/libvaluePropagation.so

$(libdir)/libannotationLanguageParser.so: $(top_builddir)/src/midend/programAnalysis/annotationLanguageParser/libannotationLanguageParser.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libannotationLanguageParser.so || ln -s $(top_builddir)/src/midend/programAnalysis/annotationLanguageParser/libannotationLanguageParser.so $(libdir)/libannotationLanguageParser.so
#	test -f $(libdir)/libannotationLanguageParser.so || cp $(top_builddir)/src/midend/programAnalysis/annotationLanguageParser/libannotationLanguageParser.so $(libdir)/libannotationLanguageParser.so
	cp $(top_builddir)/src/midend/programAnalysis/annotationLanguageParser/libannotationLanguageParser.so $(libdir)/libannotationLanguageParser.so

$(libdir)/libdepInfo.so: $(top_builddir)/src/midend/loopProcessing/depInfo/libdepInfo.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libdepInfo.so             || ln -s $(top_builddir)/src/midend/loopProcessing/depInfo/libdepInfo.so $(libdir)/libdepInfo.so
#	test -f $(libdir)/libdepInfo.so             || cp $(top_builddir)/src/midend/loopProcessing/depInfo/libdepInfo.so $(libdir)/libdepInfo.so
	cp $(top_builddir)/src/midend/loopProcessing/depInfo/libdepInfo.so $(libdir)/libdepInfo.so

$(libdir)/libdepGraph.so: $(top_builddir)/src/midend/loopProcessing/depGraph/libdepGraph.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libdepGraph.so            || ln -s $(top_builddir)/src/midend/loopProcessing/depGraph/libdepGraph.so $(libdir)/libdepGraph.so
#	test -f $(libdir)/libdepGraph.so            || cp $(top_builddir)/src/midend/loopProcessing/depGraph/libdepGraph.so $(libdir)/libdepGraph.so
	cp $(top_builddir)/src/midend/loopProcessing/depGraph/libdepGraph.so $(libdir)/libdepGraph.so

$(libdir)/libcomputation.so: $(top_builddir)/src/midend/loopProcessing/computation/libcomputation.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libcomputation.so         || ln -s $(top_builddir)/src/midend/loopProcessing/computation/libcomputation.so $(libdir)/libcomputation.so
#	test -f $(libdir)/libcomputation.so         || cp $(top_builddir)/src/midend/loopProcessing/computation/libcomputation.so $(libdir)/libcomputation.so
	cp $(top_builddir)/src/midend/loopProcessing/computation/libcomputation.so $(libdir)/libcomputation.so

$(libdir)/libslicing.so: $(top_builddir)/src/midend/loopProcessing/slicing/libslicing.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libslicing.so             || ln -s $(top_builddir)/src/midend/loopProcessing/slicing/libslicing.so $(libdir)/libslicing.so
#	test -f $(libdir)/libslicing.so             || cp $(top_builddir)/src/midend/loopProcessing/slicing/libslicing.so $(libdir)/libslicing.so
	cp $(top_builddir)/src/midend/loopProcessing/slicing/libslicing.so $(libdir)/libslicing.so

$(libdir)/libprepostloop.so: $(top_builddir)/src/midend/loopProcessing/prepostTransformation/libprepostloop.so
	@echo "Adding links to separate dynamic libraries to simplify development ..."
#	test -f $(libdir)/libprepostloop.so             || ln -s $(top_builddir)/src/midend/loopProcessing/prepostTransformation/libprepostloop.so $(libdir)/libprepostloop.so
#	test -f $(libdir)/libprepostloop.so             || cp $(top_builddir)/src/midend/loopProcessing/prepostTransformation/libprepostloop.so $(libdir)/libprepostloop.so
	cp $(top_builddir)/src/midend/loopProcessing/prepostTransformation/libprepostloop.so $(libdir)/libprepostloop.so

$(libdir)/libdriver.so: $(top_builddir)/src/midend/loopProcessing/driver/libdriver.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libdriver.so              || ln -s $(top_builddir)/src/midend/loopProcessing/driver/libdriver.so $(libdir)/libdriver.so
#	test -f $(libdir)/libdriver.so              || cp $(top_builddir)/src/midend/loopProcessing/driver/libdriver.so $(libdir)/libdriver.so
	cp $(top_builddir)/src/midend/loopProcessing/driver/libdriver.so $(libdir)/libdriver.so


$(libdir)/libunparse.so: $(top_builddir)/src/backend/unparser/libunparse.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libunparse.so             || ln -s $(top_builddir)/src/backend/unparser/libunparse.so $(libdir)/libunparse.so
#	test -f $(libdir)/libunparse.so             || cp $(top_builddir)/src/backend/unparser/libunparse.so $(libdir)/libunparse.so
	cp $(top_builddir)/src/backend/unparser/libunparse.so $(libdir)/libunparse.so

$(libdir)/libroseSupport.so: $(top_builddir)/src/roseSupport/libroseSupport.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libroseSupport.so         || ln -s $(top_builddir)/src/roseSupport/libroseSupport.so $(libdir)/libroseSupport.so
#	test -f $(libdir)/libroseSupport.so         || cp $(top_builddir)/src/roseSupport/libroseSupport.so $(libdir)/libroseSupport.so
	cp $(top_builddir)/src/roseSupport/libroseSupport.so $(libdir)/libroseSupport.so

$(libdir)/libCallGraph.so: $(top_builddir)/src/midend/programAnalysis/CallGraphAnalysis/libCallGraph.so
	@echo "Adding links to separate dynamic libraries to simplify development ..."
#	test -f libCallGraph.so           || ln -s $(top_builddir)/src/midend/programAnalysis/CallGraphAnalysis/libCallGraph.so libCallGraph.so
#	test -f $(libdir)/libCallGraph.so           || cp $(top_builddir)/src/midend/programAnalysis/CallGraphAnalysis/libCallGraph.so $(libdir)/libCallGraph.so
	cp $(top_builddir)/src/midend/programAnalysis/CallGraphAnalysis/libCallGraph.so $(libdir)/libCallGraph.so

$(libdir)/libDefUseAnalysis.so: $(top_builddir)/src/midend/programAnalysis/defUseAnalysis/libDefUseAnalysis.so
	@echo "Adding links to separate dynamic libraries to simplify development ..."
	cp $(top_builddir)/src/midend/programAnalysis/defUseAnalysis/libDefUseAnalysis.so $(libdir)/libDefUseAnalysis.so

$(libdir)/libStaticSlicing.so: $(top_builddir)/src/midend/programAnalysis/staticInterproceduralSlicing/libStaticSlicing.so
	@echo "Adding links to separate dynamic libraries to simplify development ..."
	cp $(top_builddir)/src/midend/programAnalysis/staticInterproceduralSlicing/libStaticSlicing.so $(libdir)/libStaticSlicing.so

$(libdir)/libfunctionCallNormalization.so: $(top_builddir)/src/midend/programTransformation/functionCallNormalization/libfunctionCallNormalization.so
	@echo "Adding links to separate dynamic libraries to simplify development ..."
#	test -f $(libdir)/libfunctionCallNormalization.so || ln -s $(top_builddir)/src/midend/programTransformation/functionCallNormalization/libfunctionCallNormalization.so $(libdir)/libfunctionCallNormalization.so
#	test -f $(libdir)/libfunctionCallNormalization.so || cp $(top_builddir)/src/midend/programTransformation/functionCallNormalization/libfunctionCallNormalization.so $(libdir)/libfunctionCallNormalization.so
	cp $(top_builddir)/src/midend/programTransformation/functionCallNormalization/libfunctionCallNormalization.so $(libdir)/libfunctionCallNormalization.so

# DQ (8/24/2004): This does not belong in ROSE (not even as a 3rd party library, I think)
#libckpt.so:
#	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
##	test -f libckpt.so                || ln -s $(top_srcdir)/src/3rdPartyLibraries/checkPointLibrary/libckpt.so libckpt.so
#	test -f libckpt.so                || ln -s $(top_srcdir)/src/3rdPartyLibraries/ckpt/libckpt.so libckpt.so

if ROSE_USE_DATABASE
# The connection to MySQL (by Nils Thuerey) is optional and should not be required 
# to use ROSE (still somewhat experimental).  It's use is controled by an option on 
# the configuration command line (see configure --help for details).
$(libdir)/librosedatabase.so: $(top_builddir)/src/roseExtensions/databaseConnection/librosedatabase.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/librosedatabase.so        || ln -s $(top_builddir)/src/roseExtensions/databaseConnection/librosedatabase.so $(libdir)/librosedatabase.so
#	test -f $(libdir)/librosedatabase.so        || cp $(top_builddir)/src/roseExtensions/databaseConnection/librosedatabase.so $(libdir)/librosedatabase.so
	cp $(top_builddir)/src/roseExtensions/databaseConnection/librosedatabase.so $(libdir)/librosedatabase.so

# This is Brian White's side effect analysis, it depends upon the MySQL support within ROSE
$(libdir)/libsideEffect.so: $(top_builddir)/src/midend/programAnalysis/sideEffectAnalysis/libsideEffect.so
	@echo "Adding links to separate dynamic libraries to simplify development ... (link to .so)"
#	test -f $(libdir)/libsideEffect.so          || ln -s $(top_builddir)/src/midend/programAnalysis/sideEffectAnalysis/libsideEffect.so $(libdir)/libsideEffect.so
#	test -f $(libdir)/libsideEffect.so          || cp $(top_builddir)/src/midend/programAnalysis/sideEffectAnalysis/libsideEffect.so $(libdir)/libsideEffect.so
	cp $(top_builddir)/src/midend/programAnalysis/sideEffectAnalysis/libsideEffect.so $(libdir)/libsideEffect.so
endif

if ROSE_USE_SQLITE_DATABASE
# Use the SQLite database
$(libdir)/libRoseSQLiteDatabase.so: $(top_builddir)/src/roseExtensions/SQLiteConnection/libRoseSQLiteDatabase.so
	@echo "Adding links to separate dynamic libraries to simplify development ..."
#	test -f $(libdir)/libRoseSQLiteDatabase.so        || ln -s $(top_builddir)/src/roseExtensions/SQLiteConnection/libRoseSQLiteDatabase.so $(libdir)/libRoseSQLiteDatabase.so
#	test -f $(libdir)/libRoseSQLiteDatabase.so        || cp $(top_builddir)/src/roseExtensions/SQLiteConnection/libRoseSQLiteDatabase.so $(libdir)/libRoseSQLiteDatabase.so
	cp $(top_builddir)/src/roseExtensions/SQLiteConnection/libRoseSQLiteDatabase.so $(libdir)/libRoseSQLiteDatabase.so
endif

# We have to explicily build this rule since we want to avoid the use of "ar cru" 
# (need to use the C++ compiler so that templates will be built)
$(libdir)/librose.a: $(libroseObjects)
	-rm -f $@
	$(CXX_STATIC_LIB_UPDATE) $@ $(libroseObjects)
#	$(RANLIB) librose.a
#	cp librose.a $(libdir)/librose.a
	test -f $@ || exit 1

$(libdir)/librose.so: $(libroseObjects)
	-rm -f $@
	$(CXX_SHARED_LIB_UPDATE) $@ $(libroseObjects)
#	cp librose.so $(libdir)/librose.so
	test -f $@ || exit 1

if ROSE_USE_EDG_SOURCE

# DQ (9/8/2006): Make sure after it is built that a copy is saved into the compile 
# tree in a location independent of libdir (use the current build directory).
$(libdir)/libedg.a: $(libedgObjects)
	-rm -f $@
	$(CXX_STATIC_LIB_UPDATE) $@ $(libedgObjects)
#	$(RANLIB) libedg.a
#	cp libedg.a $(libdir)/libedg.a
#	cp $(libdir)/libedg.a libedg.a
	test -f $@ || exit 1

# DQ (9/8/2006): Make sure after it is built that a copy is saved into the compile 
# tree in a location independent of libdir (use the current build directory).
$(libdir)/libedg.so: $(libedgObjects)
	-rm -f $@
	$(CXX_SHARED_LIB_UPDATE) $@ $(libedgObjects)
#	cp libedg.so $(libdir)/libedg.so
#	cp $(libdir)/libedg.so libedg.so
	test -f $@ || exit 1

testEdgSourceRule:
	echo "EDG source code will be placed into any ROSE distribution ... "

EXTRA_DIST = extra.docs

dist-hook:
	echo "dist-hook required for symetry with binary case ..."

else

# DQ (4/21/2005): librose.so should be built normally but libedg.so 
# should be copied from the distribution (by configure).

testEdgSourceRule:
	echo "EDG source code not permitted in any ROSE distribution ... "

# DQ (4/21/2005): We should not have to define this (should be defined by automake as elsewhere in ROSE)
libLIBRARIES_INSTALL = /usr/bin/install -c -m 644

# DQ (9/14/2006): This rule should not be run since the installation already has the library.
# The build proces requires the installation to have the libs in the install path.
# This might change in the near future since we would prefer to not have the build 
# process write to the install tree. This is part of future work.
# This whole rule, since ti does nothing, can likely disappear.
# DQ (1/22/2005): Use local rule to install libedg33.so in with the other libraries
install-data-local:
	@echo "Install shared library using install-data-local rule ... install $(libdir)/libedg.so to $(DESTDIR)/libedg.so"
#	$(libLIBRARIES_INSTALL) libedg.so $(DESTDIR)$(libdir)/libedg.so
#	$(libLIBRARIES_INSTALL) $(libdir)/libedg.so $(DESTDIR)/libedg.so

# These should be present in the compile tree (after compiling ROSE)
# EXTRA_DIST = libedg.so extra.docs
EXTRA_DIST = extra.docs

dist-hook:
	echo "Commented out Copying the libedg.so to $(distdir)"
#	cp -p $(libdir)/libedg.so $(distdir)

# copy the binary from the distribution (make rule allows for dependences upon libedg.so)
$(libdir)/libedg.so:
	@echo "Copying binary EDG specific library (libedg.so)"
	@if ! test -f $(srcdir)/libedg.so; then \
      echo "Error: libedg.so does not exist, configure using --with-edg_source_code=true so that EDG source code can be used to build libedg.so"; \
      exit 1; \
   fi
	@echo "Copying $(srcdir)/libedg.so to $(libdir)/libedg.so"
	cp -p $(srcdir)/libedg.so $(libdir)/libedg.so

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

# noinst_PROGRAMS = testSharedRoseLib testMultiRoseLib

# Force the building of the checkpoint library even thoughit is not included in librose
# lib_LIBRARIES = libckpt.so 

# PC (9/15/2006): force the use of the C++ linker
LINK = $(CXXLINK)

lib_LTLIBRARIES = librose.la

if ROSE_USE_OPEN_FORTRAN_PARSER
JAVA_JVM_FLAGS = -L$(JAVA_JVM_PATH) -ljvm
endif

librose_la_SOURCES =
librose_la_LIBADD = $(libroseLibraries) $(LIB_QT) $(JAVA_JVM_FLAGS) -lm -lrt
librose_la_LDFLAGS = -no-undefined

# DQ: Skip this to speed up development
# testStaticRoseLib 

# Build list of all required libraries so that they can be built in parallel
# REQUIRED_LIBS = librose.so libedg.so librose.a libedg.a 
# lib_REQUIRED_LIBS = librose.so libedg.so
# lib_REQUIRED_LIBS = librose.so libedg.so
REQUIRED_LIBS = librose.la # $(libdir)/libedg.la

# DQ (7/21/2004): Ignore the generation and use of static libraries
# testStaticRoseLib_SOURCES = testRoseLib.C
# testStaticRoseLib_DEPENDENCIES = $(REQUIRED_LIBS)
# testStaticRoseLib_LDADD = librose.a libedg.a -lm -lfl


testSharedRoseLib_SOURCES = testRoseLib.C
testSharedRoseLib_DEPENDENCIES = $(REQUIRED_LIBS)
testSharedRoseLib_LDADD  = $(LIBS_WITH_RPATH) $(ROSE_LIBS) 

#testMultiRoseLib_SOURCES = testRoseLib.C
## testMultiRoseLib_DEPENDENCIES = $(REQUIRED_LIBS) $(libList)
#testMultiRoseLib_DEPENDENCIES = $(MULTI_ROSE_SO_LIBS)
#testMultiRoseLib_LDADD = $(LIBS_WITH_RPATH) $(MULTI_ROSE_LIBS) 

print:
	@echo "MULTI_ROSE_SO_LIBS = $(MULTI_ROSE_SO_LIBS)"
	@echo "libroseObjects = $(libroseObjects)"

# DQ (1/10/2004): remove the librose.a and libedg.a files so that distclean will work cleanly
clean-local:
	rm -rf *.a *.so testSharedRoseLib testMultiRoseLib $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files

distclean-local:
	rm -rf *.a *.so testSharedRoseLib testMultiRoseLib Templates.DB

cleanTest:
	rm -rf *.o testSharedRoseLib testMultiRoseLib

# DQ (9/7/2006): The libraries are now copied directly into $(libdir) without 
# ever being present in the current build directory.
# DQ (7/22/2004): Added support to copy libraries into final install lib directory
# install-exec-hook:
#	@list='$(REQUIRED_LIBS)'; \
#	for p in $$list; do \
#	  if test -f $$p; then \
#	    echo " cp $$p $(DESTDIR)$(libdir)/$$p"; \
#	    cp $$p $(DESTDIR)$(libdir)/$$p; \
#	  else :; fi; \
#	done

# DQ (9/14/2006): These are required!
# DQ (9/14/2006): I don't know if we really need to remove the libs
# I think we might because there is nothing obvious in the generated 
# Makefile to remove the libraries.
# DQ (7/22/2004): Need to be able to remove the REQUIRED_LIBS also!
# Note that uninstall-hook does not work, but uninstall-local does.
uninstall-local:
#	rm -f $(DESTDIR)$(libdir)/libedg.so
#	rm -f $(DESTDIR)$(libdir)/librose.so
	rm -f $(libdir)/libedg.so
	rm -f $(libdir)/librose.so
	@echo "************************************************************"
	@echo "**** make uninstall rule complete (terminated normally) ****"
	@echo "************************************************************"


rebuildPrelinker:
	rm -f frontend/EDG/EDG_3.3/util/edg_prelink.o
	rm -f frontend/SageIII/Cxx_Grammar.o
	rm -f roseSupport/templateSupport.o
	$(MAKE)








