include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


# tps (1Dec2008) : I enabled the mysql path detection and this code fails because no such include files can be found. 
# so I disable this if --with-binarysql is used!
if ROSE_USE_BINARY_SQL
else
if ROSE_USE_MYSQL_DATABASE

INCLUDES = $(ROSE_INCLUDES)

libCallGraphSources =  CallGraph.C ClassHierarchyGraph.C


libCallGraph_la_SOURCES = $(libCallGraphSources)

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

include_HEADERS = CallGraph.h ClassHierarchyGraph.h

endif
endif

noinst_LTLIBRARIES = libCallGraph.la