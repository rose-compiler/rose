include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LINK	 = $(CXXLINK)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)


# noinst_LIBRARIES lists all the librarlies that should be built, but not installed
noinst_PROGRAMS = unparseHeader

unparseHeader_SOURCES = headerUnparser.C
unparseHeader_LDADD = $(top_builddir)/projects/backstroke/utilities/libBackstrokeUtilities.a \
		$(top_builddir)/projects/backstroke/normalizations/libBackstrokeNormalizations.a \
		$(top_builddir)/projects/backstroke/ssa/libSSA.a \
		$(top_builddir)/projects/backstroke/valueGraph/libBackstrokeFunctionReverser.a \
		$(LIBS_WITH_RPATH) $(ROSE_LIBS)
unparseHeader_include_HEADERS = 
unparseHeader_includedir = $(includedir)/backstroke


# EXTRA_DIST are files that are not compiled or installed. These include readme's, internal header files, etc.
EXTRA_DIST = 

CLEANFILES = 


check-local:
	@echo "*********************************************************************************************************"
	@echo "*** ROSE/projects/backstroke/valueGraph/unparseHeader: make check rule complete (terminated normally) ***"
	@echo "*********************************************************************************************************"

test2:
	./valueGraph -c temp.C -I/home/hou1/opt/boost_1_36_0//include/boost-1_36

