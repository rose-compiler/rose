include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

# -I$(top_srcdir)/src/util/graphs -I$(top_srcdir)/src/midend/astUtil/astInterface/
# Liao 5/7/2010. we don't install them since we have better call graph program from tutorial
noinst_PROGRAMS = callGraph writeCallGraph

writeCallGraph_SOURCES = writeCallGraph.C
callGraph_SOURCES      = callGraph.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)


test: callGraph $(srcdir)/callGraphInputTest.C
	callGraph $(srcdir)/callGraphInputTest.C

