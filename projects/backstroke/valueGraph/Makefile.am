include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LINK	 = $(CXXLINK)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)


# noinst_LIBRARIES lists all the librarlies that should be built, but not installed
#noinst_LIBRARIES =  libBackstrokeValueGraph.a
noinst_PROGRAMS = valueGraph

valueGraph_SOURCES = valueGraphNode.C valueGraph.C eventReverser.C pathNumGenerator.C \
		CFGFilter.C test.C
valueGraph_LDADD = $(top_builddir)/projects/backstroke/utilities/libBackstrokeUtilities.a \
		$(top_builddir)/projects/backstroke/ssa/libSSA.a \
		$(LIBS_WITH_RPATH) $(ROSE_LIBS)
valueGraph_include_HEADERS = valueGraph.h valueGraphNode.h pathNumGenerator.h CFGFilter.h
valueGraph_includedir = $(includedir)/backstroke


# EXTRA_DIST are files that are not compiled or installed. These include readme's, internal header files, etc.
EXTRA_DIST = 

CLEANFILES = 


check-local:
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/valueGraph: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

test:
	./valueGraph -c temp.C
