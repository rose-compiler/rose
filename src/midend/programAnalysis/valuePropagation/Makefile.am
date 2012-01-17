include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# INCLUDES =  $(ROSE_INCLUDES) -I$(srcdir)/.. -I$(top_srcdir)/src/midend/loopProcessing -I$(top_srcdir)/src/midend/astUtil -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util

INCLUDES =  $(ROSE_INCLUDES) -I$(srcdir)/.. -I$(srcdir)/../bitvectorDataflow -I$(srcdir)/../CFG -I$(top_srcdir)/src/midend/loopProcessing -I$(top_srcdir)/src/midend/astUtil -I$(top_srcdir)/src/midend/astUtil/astInterface -I$(top_srcdir)/src/midend/astUtil/annotation -I$(top_srcdir)/src/midend/astUtil/symbolicVal  -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

libvaluePropagationSources = ValueAnnot.C  ValuePropagate.C

# lib_LTLIBRARIES = libvaluePropagation.a
# libvaluePropagation_a_SOURCES  = $(libvaluePropagationSources)

noinst_LTLIBRARIES = libvaluePropagation.la
libvaluePropagation_la_SOURCES = $(libvaluePropagationSources)

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

include_HEADERS =  ValueAnnot.h ValuePropagate.h








EXTRA_DIST = CMakeLists.txt
