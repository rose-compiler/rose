
include_directories(${CMAKE_SOURCE_DIR}/src/util/support ${CMAKE_SOURCE_DIR}/src/util/graphs ${CMAKE_SOURCE_DIR}/src/midend/astUtil/astSupport ${CMAKE_SOURCE_DIR}/src/midend/astUtil/astInterface ${CMAKE_SOURCE_DIR}/src/midend/astUtil/symbolicVal)


########### next target ###############


########### install files ###############

install(FILES  DepGraphBuild.h  DepGraph.h  DepGraphTransform.h  LoopAnalysis.h  TransDepGraph.h TransDepGraphImpl.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = -I$(srcdir)/../driver -I$(srcdir)/../depInfo  -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs  -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/astInterface -I$(top_srcdir)/src/midend/astUtil/symbolicVal 
#
### The grammar generator (ROSETTA) should use its own template repository
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
#libdepGraphSources = DepGraphBuild.C  LoopAnalysis.C  TransDepGraph.C
#
## lib_LTLIBRARIES = libdepGraph.a
## libdepGraph_a_SOURCES  = $(libdepGraphSources)
#
#noinst_LTLIBRARIES = libdepGraph.la
#libdepGraph_la_SOURCES = $(libdepGraphSources)
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
#
#distclean-local:
#	rm -rf Templates.DB
#
#include_HEADERS =  DepGraphBuild.h  DepGraph.h  DepGraphTransform.h  LoopAnalysis.h  TransDepGraph.h TransDepGraphImpl.h
#
