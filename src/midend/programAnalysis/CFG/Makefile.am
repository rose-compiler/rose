include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES =  $(ROSE_INCLUDES) -I$(srcdir)/.. -I$(srcdir)/../OAWrap -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/astInterface

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .


# DQ (2/10/2010): ROSE currently fails to generate code for this file (it does however compile for analysis only).
# libCFGSources =  CFG_ROSE.C CFG_OA.C 
libCFGSources =  CFG_ROSE.C
if !ROSE_USING_ROSE_CODE_GENERATION
   libCFGSources += CFG_OA.C 
endif

noinst_LTLIBRARIES = libCFG.la
libCFG_la_SOURCES = $(libCFGSources)

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

include_HEADERS =  CFG.h CFGImpl.h

EXTRA_DIST = CMakeLists.txt
