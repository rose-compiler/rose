
include_directories(${CMAKE_SOURCE_DIR}/src/midend/loopProcessing ${CMAKE_SOURCE_DIR}/src/midend/astUtil ${CMAKE_SOURCE_DIR}/src/midend/astUtil/astInterface ${CMAKE_SOURCE_DIR}/src/midend/astUtil/annotation ${CMAKE_SOURCE_DIR}/src/midend/astUtil/symbolicVal ${CMAKE_SOURCE_DIR}/src/midend/astUtil/astSupport ${CMAKE_SOURCE_DIR}/src/util/support ${CMAKE_SOURCE_DIR}/src/util/graphs)


########### next target ###############


########### install files ###############

install(FILES  DebugTool.h  DefUseExtension.h  DependenceGraph.h  InterproceduralInfo.h  SimpleDirectedGraph.h  SlicingInfo.h EDefUse.h SDGLibraryExtender.h CreateSlice.h CreateSliceSet.h ControlFlowGraph.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
## INCLUDES =  $(ROSE_INCLUDES) -I$(srcdir)/.. -I$(srcdir)/../dataflowAnalysis -I$(srcdir)/../CFG -I$(top_srcdir)/src/midend/loopProcessing -I$(top_srcdir)/src/midend/astUtil -I$(top_srcdir)/src/midend/astUtil/astInterface -I$(top_srcdir)/src/midend/astUtil/annotation -I$(top_srcdir)/src/midend/astUtil/symbolicVal  -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs -I$(srcdir)/../dominatorTreesAndDominanceFrontiers
#INCLUDES =  $(ROSE_INCLUDES) -I$(srcdir)/.. -I$(srcdir)/../dataflowAnalysis -I$(srcdir)/../CFG -I$(top_srcdir)/src/midend/loopProcessing -I$(top_srcdir)/src/midend/astUtil -I$(top_srcdir)/src/midend/astUtil/astInterface -I$(top_srcdir)/src/midend/astUtil/annotation -I$(top_srcdir)/src/midend/astUtil/symbolicVal  -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs
#
#noinst_LTLIBRARIES = libStaticSlicing.la
#
## DQ (1/2/2007): Copied the ControlFlowGraph.C and ControlFlowGraph.h from ../dominatorTreesAndDominanceFrontiers
## since that directory has been deprecated.
#
#libStaticSlicing_la_SOURCES = \
#   $(srcdir)/ControlDependenceGraph.C  $(srcdir)/DataDependenceGraph.C  \
#   $(srcdir)/MergedDependenceGraph.C $(srcdir)/CreateSlice.C $(srcdir)/SlicingInfo.C $(srcdir)/CreateSliceSet.C \
#   $(srcdir)/DependenceGraph.C $(srcdir)/FunctionDependenceGraph.C \
#   $(srcdir)/SystemDependenceGraph.C $(srcdir)/DefUseExtension.C $(srcdir)/EDefUse.C \
#   ControlFlowGraph.C
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
#
#distclean-local:
#	rm -rf Templates.DB
#
#include_HEADERS = DebugTool.h  DefUseExtension.h  DependenceGraph.h  InterproceduralInfo.h  SimpleDirectedGraph.h  SlicingInfo.h EDefUse.h SDGLibraryExtender.h CreateSlice.h CreateSliceSet.h ControlFlowGraph.h
