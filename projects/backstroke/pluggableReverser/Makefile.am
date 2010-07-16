include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = 

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Build a library of all the reverser code
noinst_LTLIBRARIES =  libBackstrokePluggableReverser.la

libBackstrokePluggableReverser_ladir = .
libBackstrokePluggableReverser_la_DEPENDENCIES =
libBackstrokePluggableReverser_la_SOURCES = expressionHandler.C statementHandler.C storage.C
libBackstrokePluggableReverser_includedir = $(includedir)/backstroke
libBackstrokePluggableReverser_include_HEADERS = expressionHandler.h statementHandler.h processorPool.h singleton.h storage.h
libBackstrokePluggableReverser_la_LIBADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = 

CLEANFILES = 

ALL_TESTCODES =

check-local:
	@echo "*********************************************************************************************************"
	@echo "******* ROSE/projects/backstroke/pluggableReverser: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

