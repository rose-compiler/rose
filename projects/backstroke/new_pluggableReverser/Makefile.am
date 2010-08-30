include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = 

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke

# Build a library of all the reverser code
noinst_LIBRARIES =  libBackstrokenew_PluggableReverser.a

libBackstrokenew_PluggableReverser_a_DEPENDENCIES =
libBackstrokenew_PluggableReverser_a_SOURCES = expressionProcessor.C statementProcessor.C ifStatementProcessor.C eventProcessor.C variableVersionTable.C 
libBackstrokenew_PluggableReverser_a_includedir = $(includedir)/backstroke
libBackstrokenew_PluggableReverser_a_include_HEADERS = expressionProcessor.h statementProcessor.h ifStatementProcessor.h eventProcessor.h variableVersionTable.h costModel.h

EXTRA_DIST = 

CLEANFILES = 

ALL_TESTCODES =

check-local:
	@echo "*********************************************************************************************************"
	@echo "******* ROSE/projects/backstroke/pluggableReverser: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

