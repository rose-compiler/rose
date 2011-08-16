
include_directories(${CMAKE_SOURCE_DIR}/src/util ${CMAKE_SOURCE_DIR}/src/util/support ${CMAKE_SOURCE_DIR}/src/util/graphs ${CMAKE_SOURCE_DIR}/src/midend/astUtil ${CMAKE_SOURCE_DIR}/src/midend/astUtil/astSupport ${CMAKE_SOURCE_DIR}/src/midend/astUtil/astInterface ${CMAKE_SOURCE_DIR}/src/util/support ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


########### install files ###############

install(FILES  DataFlowAnalysis.h DefUseChain.h ReachingDefinition.h ReachingDefinitionFacade.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES =  $(ROSE_INCLUDES) -I$(srcdir)/.. -I$(srcdir)/../CFG -I$(top_srcdir)/src/util -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs -I$(top_srcdir)/src/midend/astUtil -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/astInterface -I$(top_srcdir)/src/util/support 
#
### The grammar generator (ROSETTA) should use its own template repository
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
#libdataflowAnalysisSources = DataFlowAnalysis.C  DefUseChain.C  ReachingDefinition.C
##libdataflowAnalysisSources = DataFlowAnalysis.C  ReachingDefinition.C
#
## lib_LTLIBRARIES = libdataflowAnalysis.a
## libdataflowAnalysis_a_SOURCES  = $(libdataflowAnalysisSources)
#
#noinst_LTLIBRARIES = libdataflowAnalysis.la
#libdataflowAnalysis_la_SOURCES = $(libdataflowAnalysisSources)
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
#
#distclean-local:
#	rm -rf Templates.DB
#
#include_HEADERS = DataFlowAnalysis.h DefUseChain.h ReachingDefinition.h
#
#
#
#
#
#
#
#
