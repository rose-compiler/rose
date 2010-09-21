include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = 

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke

# Build a library of all the reverser code
noinst_LIBRARIES =  libBackstrokePluggableReverser.a

libBackstrokePluggableReverser_a_DEPENDENCIES =
libBackstrokePluggableReverser_a_SOURCES = expressionProcessor.C statementProcessor.C ifStatementProcessor.C eventProcessor.C \
					variableVersionTable.C costModel.C akgulStyleExpressionProcessor.C \
					straightlineStatementProcessor.C handlerTypes.C returnStatementHandler.C \
					variableDeclarationHandler.C redefineValueRestorer.C extractFromUseValueRestorer.C \
					whileStatementProcessor.C
libBackstrokePluggableReverser_a_includedir = $(includedir)/backstroke
libBackstrokePluggableReverser_a_include_HEADERS = expressionProcessor.h statementProcessor.h \
					ifStatementProcessor.h eventProcessor.h variableVersionTable.h costModel.h \
					handlerTypes.h straightlineStatementProcessor.h akgulStyleExpressionProcessor.h \
					returnStatementHandler.h variableDeclarationHandler.h redefineValueRestorer.h \
					extractFromUseValueRestorer.h whileStatementProcessor.h

EXTRA_DIST = 

CLEANFILES = 

ALL_TESTCODES =

check-local:
	@echo "*********************************************************************************************************"
	@echo "******* ROSE/projects/backstroke/pluggableReverser: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

