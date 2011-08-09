include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# INCLUDES =  $(ROSE_INCLUDES) -I$(srcdir)/.. -I$(srcdir)/../dataflowAnalysis -I$(srcdir)/../CFG -I$(top_srcdir)/src/midend/loopProcessing -I$(top_srcdir)/src/midend/astUtil -I$(top_srcdir)/src/midend/astUtil/astInterface -I$(top_srcdir)/src/midend/astUtil/annotation -I$(top_srcdir)/src/midend/astUtil/symbolicVal  -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs -I$(srcdir)/../dominatorTreesAndDominanceFrontiers
INCLUDES =  $(ROSE_INCLUDES) -I$(srcdir)/.. -I$(srcdir)/../dataflowAnalysis -I$(srcdir)/../CFG -I$(top_srcdir)/src/midend/loopProcessing -I$(top_srcdir)/src/midend/astUtil -I$(top_srcdir)/src/midend/astUtil/astInterface -I$(top_srcdir)/src/midend/astUtil/annotation -I$(top_srcdir)/src/midend/astUtil/symbolicVal  -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs

noinst_LTLIBRARIES = libStaticSlicing.la

# DQ (1/2/2007): Copied the ControlFlowGraph.C and ControlFlowGraph.h from ../dominatorTreesAndDominanceFrontiers
# since that directory has been deprecated.

libStaticSlicing_la_SOURCES = \
   $(srcdir)/ControlDependenceGraph.C  $(srcdir)/DataDependenceGraph.C  \
   $(srcdir)/MergedDependenceGraph.C $(srcdir)/CreateSlice.C $(srcdir)/SlicingInfo.C $(srcdir)/CreateSliceSet.C \
   $(srcdir)/DependenceGraph.C $(srcdir)/FunctionDependenceGraph.C \
   $(srcdir)/SystemDependenceGraph.C $(srcdir)/DefUseExtension.C $(srcdir)/EDefUse.C \
   ControlFlowGraph.C

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

include_HEADERS = DebugTool.h  DefUseExtension.h  DependenceGraph.h  InterproceduralInfo.h  SimpleDirectedGraph.h  SlicingInfo.h EDefUse.h SDGLibraryExtender.h CreateSlice.h CreateSliceSet.h ControlFlowGraph.h

# New files (not yet used) and representing bug fixes from University of Wisc not yet passing
# "make check" rule in tests/roseTests/programAnalysisTests/staticInterproceduralSlicingTests
# This is an intermediate step to getting this into place.
NEW_SDG = NEW_ControlDependenceGraph.C  NEW_CreateSliceSet.C  NEW_CreateSliceSet.h  NEW_DataDependenceGraph.C  NEW_DependenceGraph.C  NEW_DependenceGraph.h  NEW_EDefUse.C  NEW_EDefUse.h  NEW_MergedDependenceGraph.C  NEW_SystemDependenceGraph.C

EXTRA_DIST = CMakeLists.txt $(NEW_SDG)
