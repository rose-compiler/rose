
include_directories(${CMAKE_SOURCE_DIR}/src/midend/loopProcessing ${CMAKE_SOURCE_DIR}/src/midend/astUtil ${CMAKE_SOURCE_DIR}/src/midend/astUtil/astInterface ${CMAKE_SOURCE_DIR}/src/midend/astUtil/annotation ${CMAKE_SOURCE_DIR}/src/midend/astUtil/symbolicVal ${CMAKE_SOURCE_DIR}/src/midend/astUtil/astSupport ${CMAKE_SOURCE_DIR}/src/util/support ${CMAKE_SOURCE_DIR}/src/util/graphs)


########### next target ###############


########### install files ###############

install(FILES  ArrayAnnot.h ValueAnnot.h ValuePropagate.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
## INCLUDES =  $(ROSE_INCLUDES) -I$(srcdir)/.. -I$(top_srcdir)/src/midend/loopProcessing -I$(top_srcdir)/src/midend/astUtil -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util
#
#INCLUDES =  $(ROSE_INCLUDES) -I$(srcdir)/.. -I$(srcdir)/../dataflowAnalysis -I$(srcdir)/../CFG -I$(top_srcdir)/src/midend/loopProcessing -I$(top_srcdir)/src/midend/astUtil -I$(top_srcdir)/src/midend/astUtil/astInterface -I$(top_srcdir)/src/midend/astUtil/annotation -I$(top_srcdir)/src/midend/astUtil/symbolicVal  -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs
#
### The grammar generator (ROSETTA) should use its own template repository
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
#libvaluePropagationSources = ArrayAnnot.C ValueAnnot.C  ValuePropagate.C
#
## lib_LTLIBRARIES = libvaluePropagation.a
## libvaluePropagation_a_SOURCES  = $(libvaluePropagationSources)
#
#noinst_LTLIBRARIES = libvaluePropagation.la
#libvaluePropagation_la_SOURCES = $(libvaluePropagationSources)
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
#
#distclean-local:
#	rm -rf Templates.DB
#
#include_HEADERS =  ArrayAnnot.h ValueAnnot.h ValuePropagate.h
#
#
#
#
#
#
#
