include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I${top_srcdir}/src/midend/programAnalysis/CFG 

libpreSources = \
   expressionTreeEqual.C pre.C controlFlowGraph.C

if DEV_LIBS
lib_LTLIBRARIES = libpre.la
else
noinst_LTLIBRARIES = libpre.la
endif
libpre_la_SOURCES = $(libpreSources)

include_HEADERS = expressionTreeEqual.h pre.h controlFlowGraph.h

clean-local:
	rm -rf Templates.DB ii_files ti_files core
