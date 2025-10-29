
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR})


########### next target ###############


########### install files ###############

install(FILES  expressionTreeEqual.h pre.h preControlFlowGraph.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES) -I${top_srcdir}/src/midend/programAnalysis/CFG 
#
#libpreSources = \
#   expressionTreeEqual.C pre.C preControlFlowGraph.C
#
#noinst_LTLIBRARIES = libpre.la
#libpre_la_SOURCES = $(libpreSources)
#
#include_HEADERS = expressionTreeEqual.h pre.h preControlFlowGraph.h
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files core
