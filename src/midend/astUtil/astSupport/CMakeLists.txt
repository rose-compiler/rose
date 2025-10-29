include_directories(${CMAKE_SOURCE_DIR}/src/util ${CMAKE_SOURCE_DIR}/src/util/support ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR})

########### header files to be installed ###############
set(ASTSUPPORT_HEADERS AnalysisInterface.h ProcessAstTree.h ProcessAstTree.C StmtInfoCollect.h LoopInfoInterface.h) 

# ProcessAstTree.C fails on windows, so we remove it in that case 
if(WIN32)
  list(REMOVE_ITEM ${ASTSUPPORT_HEADERS} ProcessAstTree.C)
endif() 

install(FILES ${ASTSUPPORT_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR})

#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES) -I$(srcdir)/../astInterface -I$(srcdir)/../symbolicVal -I$(top_srcdir)/src/util -I$(top_srcdir)/src/util/support
#
### The grammar generator (ROSETTA) should use its own template repository
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
#libastSupportSources = ProcessAstTree.C  StmtInfoCollect.C
#
## lib_LTLIBRARIES = libastSupport.a
## libastSupport_a_SOURCES  = $(libastSupportSources)
#
#noinst_LTLIBRARIES = libastSupport.la
#libastSupport_la_SOURCES = $(libastSupportSources)
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
#
#distclean-local:
#	rm -rf Templates.DB
#
#include_HEADERS =  AnalysisInterface.h  ProcessAstTree.h  StmtInfoCollect.h LoopInfoInterface.h
