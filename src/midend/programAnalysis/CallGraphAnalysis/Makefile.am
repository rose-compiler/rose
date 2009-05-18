include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs



INCLUDES = $(ROSE_INCLUDES) $(BOOST_CPPFLAGS) 
libCallGraphSources =  CallGraph.C ClassHierarchyGraph.C

noinst_LTLIBRARIES = libCallGraph.la
libCallGraph_la_SOURCES = $(libCallGraphSources)

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

include_HEADERS = CallGraph.h ClassHierarchyGraph.h


