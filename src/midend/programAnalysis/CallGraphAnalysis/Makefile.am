include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

libCallGraphSources =  CallGraph.C ClassHierarchyGraph.C

if DEV_LIBS
lib_LTLIBRARIES = libCallGraph.la
else
noinst_LTLIBRARIES = libCallGraph.la
endif
libCallGraph_la_SOURCES = $(libCallGraphSources)

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

include_HEADERS = CallGraph.h ClassHierarchyGraph.h CallGraphTemplate.C
