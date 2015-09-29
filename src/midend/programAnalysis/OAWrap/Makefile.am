include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS =  $(ROSE_INCLUDES) -I$(top_srcdir)/src/midend/programAnalysis/OAWrap -I$(top_srcdir)/src/midend/programAnalysis

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

# DQ (2/10/2010): ROSE currently fails to generate code for this file (it does however compile for analysis only).
libOAWrapSources = OAWrap.C SAGE2OA.C 
#libOAWrapSources = SAGE2OA.C 
#if !ROSE_USING_ROSE_CODE_GENERATION
#   libOAWrapSources += OAWrap.C
#endif

noinst_LTLIBRARIES = libOAWrap.la
libOAWrap_la_SOURCES = $(libOAWrapSources)

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

pkginclude_HEADERS =  OAWrap.h SAGE2OA.h OAConfig.h








EXTRA_DIST = CMakeLists.txt
