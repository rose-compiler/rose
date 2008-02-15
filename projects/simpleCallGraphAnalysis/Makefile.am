include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

# -I$(top_srcdir)/src/util/graphs -I$(top_srcdir)/src/midend/astUtil/astInterface/

bin_PROGRAMS = callGraph writeCallGraph

writeCallGraph_SOURCES = writeCallGraph.C
callGraph_SOURCES      = callGraph.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)


test: callGraph $(srcdir)/callGraphInputTest.C
	callGraph $(srcdir)/callGraphInputTest.C

