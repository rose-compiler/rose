
include_directories(${CMAKE_SOURCE_DIR}/src/util/support ${CMAKE_SOURCE_DIR}/src/midend/astUtil/astSupport ${CMAKE_SOURCE_DIR}/src/midend/astUtil/astInterface ${CMAKE_SOURCE_DIR}/src/midend/astUtil/symbolicVal ${CMAKE_SOURCE_DIR}/src/midend/loopProcessing/driver ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR})


########### next target ###############


########### install files ###############

install(FILES  DepEDD.h       DepInfo.h     DepInfoUpdate.h  DomainInfo.h                     DepInfoAnal.h  DepInfoSet.h  DepRel.h   DomainInfoUpdate.h                     ReuseAnalysis.h StmtDepAnal.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/astInterface -I$(top_srcdir)/src/midend/astUtil/symbolicVal -I$(top_srcdir)/src/midend/loopProcessing/driver
#
### The grammar generator (ROSETTA) should use its own template repository
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
#libdepInfoSources = \
#   DepEDD.C  DepInfoAnal.C ReuseAnalysis.C DepInfo.C  \
#   DepInfoSet.C  DepRel.C  DomainInfo.C  StmtDepAnal.C
#
## lib_LTLIBRARIES = libdepInfo.a
## libdepInfo_a_SOURCES  = $(libdepInfoSources)
#
#noinst_LTLIBRARIES = libdepInfo.la
#libdepInfo_la_SOURCES = $(libdepInfoSources)
#
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
#
#distclean-local:
#	rm -rf Templates.DB
#
#include_HEADERS =  DepEDD.h       DepInfo.h     DepInfoUpdate.h  DomainInfo.h \
#                   DepInfoAnal.h  DepInfoSet.h  DepRel.h   DomainInfoUpdate.h \
#                   ReuseAnalysis.h StmtDepAnal.h
#
