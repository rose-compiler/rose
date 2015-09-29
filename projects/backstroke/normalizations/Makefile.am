include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LINK	 = $(CXXLINK)

# noinst_LTLIBRARIES lists all the libraries that should be built, but not installed
noinst_LTLIBRARIES =  libBackstrokeNormalizations.la

libBackstrokeNormalizations_la_DEPENDENCIES = 
libBackstrokeNormalizations_la_SOURCES = expNormalization.C
libBackstrokeNormalizations_la_include_HEADERS = expNormalization.h
libBackstrokeNormalizations_la_includedir = $(includedir)/backstroke


# EXTRA_DIST are files that are not compiled or installed. These include readme's, internal header files, etc.
EXTRA_DIST = 

CLEANFILES = 


check-local:

