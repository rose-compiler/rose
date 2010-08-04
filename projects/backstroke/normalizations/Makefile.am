include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
LINK	 = $(CXXLINK)

# noinst_LTLIBRARIES lists all the librarlies that should be built with libtool, but not installed
noinst_LTLIBRARIES =  libBackstrokeNormalizations.la

libBackstrokeNormalizations_ladir = .
libBackstrokeNormalizations_la_DEPENDENCIES = 
libBackstrokeNormalizations_la_SOURCES = ExtractFunctionArguments.C expNormalization.C
libBackstrokeNormalizations_la_include_HEADERS = ExtractFunctionArguments.h expNormalization.h
libBackstrokeNormalizations_la_includedir = $(includedir)/backstroke


# EXTRA_DIST are files that are not compiled or installed. These include readme's, internal header files, etc.
EXTRA_DIST = LIMITATIONS.txt

CLEANFILES = 


check-local:
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/normalizations: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"
