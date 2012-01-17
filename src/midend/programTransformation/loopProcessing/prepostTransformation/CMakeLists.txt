
include_directories(${CMAKE_SOURCE_DIR}/src/util/support ${CMAKE_SOURCE_DIR}/src/midend/astUtil/astSupport ${CMAKE_SOURCE_DIR}/src/midend/astUtil/symbolicVal ${CMAKE_SOURCE_DIR}/src/midend/astUtil/astInterface ${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/bitvectorDataflow ${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/CFG ${CMAKE_SOURCE_DIR}/src/util/graphs ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


########### install files ###############

install(FILES  BreakupStmt.h  LoopUnroll.h  PrePostTransformation.h PrivatizeScalar.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = -I$(srcdir)/../driver -I$(srcdir)/../depInfo -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/symbolicVal -I$(top_srcdir)/src/midend/astUtil/astInterface -I$(top_srcdir)/src/midend/programAnalysis/bitvectorDataflow -I$(top_srcdir)/src/midend/programAnalysis/CFG -I$(top_srcdir)/src/util/graphs
#
### The grammar generator (ROSETTA) should use its own template repository
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
#libprepostloopSources = BreakupStmt.C  LoopUnroll.C PrivatizeScalar.C
#
## lib_LTLIBRARIES = libprepostloop.a
## libprepostloop_a_SOURCES  = $(libprepostloopSources)
#
#noinst_LTLIBRARIES = libprepostloop.la
#libprepostloop_la_SOURCES = $(libprepostloopSources)
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
#
#distclean-local:
#	rm -rf Templates.DB
#
#include_HEADERS =  BreakupStmt.h  LoopUnroll.h  PrePostTransformation.h PrivatizeScalar.h
#
#
#
#
#
#
#
#
