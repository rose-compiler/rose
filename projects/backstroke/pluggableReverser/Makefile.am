include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = 

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke

# Build a library of all the reverser code
noinst_LIBRARIES =  libBackstrokePluggableReverser.a

libBackstrokePluggableReverser_a_DEPENDENCIES =
libBackstrokePluggableReverser_a_SOURCES = expressionHandler.C statementHandler.C ifStatementHandler.C eventProcessor.C \
					variableVersionTable.C costModel.C akgulStyleExpressionHandler.C \
					straightlineStatementHandler.C handlerTypes.C returnStatementHandler.C \
					variableDeclarationHandler.C redefineValueRestorer.C extractFromUseValueRestorer.C \
					whileStatementHandler.C stateSavingStatementHandler.C
libBackstrokePluggableReverser_a_includedir = $(includedir)/backstroke
libBackstrokePluggableReverser_a_include_HEADERS = expressionHandler.h statementHandler.h \
					ifStatementHandler.h eventProcessor.h variableVersionTable.h costModel.h \
					handlerTypes.h straightlineStatementHandler.h akgulStyleExpressionHandler.h \
					returnStatementHandler.h variableDeclarationHandler.h redefineValueRestorer.h \
					extractFromUseValueRestorer.h whileStatementHandler.h stateSavingStatementHandler.h \
					expAndStmtHandlers.h

EXTRA_DIST = 

CLEANFILES = 

ALL_TESTCODES =

check-local:
	@echo "*********************************************************************************************************"
	@echo "******* ROSE/projects/backstroke/pluggableReverser: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

