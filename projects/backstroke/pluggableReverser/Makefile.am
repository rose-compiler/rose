include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = 

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Build a library of all the reverser code
noinst_LTLIBRARIES =  libBackstrokePluggableReverser.la

libBackstrokePluggableReverser_ladir = .
libBackstrokePluggableReverser_la_DEPENDENCIES =
libBackstrokePluggableReverser_la_SOURCES = expressionProcessor.C statementProcessor.C eventProcessor.C variableVersionTable.C akgulStyleExpressionProcessor.C \
						straightlineStatementProcessor.C
libBackstrokePluggableReverser_includedir = $(includedir)/backstroke
libBackstrokePluggableReverser_include_HEADERS = expressionProcessor.h statementProcessor.h eventProcessor.h variableVersionTable.h akgulStyleExpressionProcessor.h \
						straightlineStatementProcessor.h costModel.h

EXTRA_DIST = 

CLEANFILES = 

ALL_TESTCODES =

check-local:
	@echo "*********************************************************************************************************"
	@echo "******* ROSE/projects/backstroke/pluggableReverser: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

