include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = 

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke

# Build a library of all the reverser code
noinst_LTLIBRARIES =  libBackstrokePluggableReverser.la

libBackstrokePluggableReverser_la_DEPENDENCIES =
libBackstrokePluggableReverser_la_SOURCES = expressionHandler.C statementHandler.C ifStatementHandler.C eventProcessor.C \
					variableVersionTable.C costModel.C akgulStyleExpressionHandler.C \
					straightlineStatementHandler.C handlerTypes.C returnStatementHandler.C \
					variableDeclarationHandler.C redefineValueRestorer.C extractFromUseValueRestorer.C \
					whileStatementHandler.C stateSavingStatementHandler.C
libBackstrokePluggableReverser_la_includedir = $(includedir)/backstroke
libBackstrokePluggableReverser_la_include_HEADERS = expressionHandler.h statementHandler.h \
					ifStatementHandler.h eventProcessor.h variableVersionTable.h costModel.h \
					handlerTypes.h straightlineStatementHandler.h akgulStyleExpressionHandler.h \
					returnStatementHandler.h variableDeclarationHandler.h redefineValueRestorer.h \
					extractFromUseValueRestorer.h whileStatementHandler.h stateSavingStatementHandler.h \
					expAndStmtHandlers.h

libBackstrokePluggableReverser_la_LIBADD = 

EXTRA_DIST = backstrokeRuntime.h backstrokeRuntime.C

CLEANFILES = 

ALL_TESTCODES =

check-local:
	@echo "*********************************************************************************************************"
	@echo "******* ROSE/projects/backstroke/pluggableReverser: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

