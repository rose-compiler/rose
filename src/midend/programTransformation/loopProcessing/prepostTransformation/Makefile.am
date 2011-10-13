include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = -I$(srcdir)/../driver -I$(srcdir)/../computation -I$(srcdir)/../depInfo -I$(srcdir)/../outsideInterface -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/symbolicVal -I$(top_srcdir)/src/midend/astUtil/astInterface -I$(top_srcdir)/src/midend/programAnalysis/bitvectorDataflow -I$(top_srcdir)/src/midend/programAnalysis/CFG -I$(top_srcdir)/src/util/graphs

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

libprepostloopSources = BreakupStmt.C  LoopUnroll.C PrivatizeScalar.C

# lib_LTLIBRARIES = libprepostloop.a
# libprepostloop_a_SOURCES  = $(libprepostloopSources)

noinst_LTLIBRARIES = libprepostloop.la
libprepostloop_la_SOURCES = $(libprepostloopSources)

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

include_HEADERS =  BreakupStmt.h  LoopUnroll.h  PrePostTransformation.h PrivatizeScalar.h









EXTRA_DIST = CMakeLists.txt
