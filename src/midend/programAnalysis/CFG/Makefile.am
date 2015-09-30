include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS =  $(ROSE_INCLUDES) -I$(srcdir)/.. -I$(srcdir)/../OAWrap -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/astInterface

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

# DQ (2/11/2010): ROSE currently fails to generate code for this CFG_OA.C file (it does however compile for analysis only).
# Also it does not appear that ROSE requires this CFG_OA.C file, so I have removed it.
# libCFGSources =  CFG_ROSE.C CFG_OA.C 
libCFGSources =  CFG_ROSE.C

noinst_LTLIBRARIES = libCFG.la
libCFG_la_SOURCES = $(libCFGSources)

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

pkginclude_HEADERS =  CFG.h CFGImpl.h

EXTRA_DIST = CMakeLists.txt
