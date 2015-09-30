include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = 

AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke

# Build a library of all the reverser code
noinst_LTLIBRARIES =  libBackstrokePluggableReverser.la

libBackstrokePluggableReverser_la_DEPENDENCIES =
libBackstrokePluggableReverser_la_SOURCES = expressionHandler.C SgNullStatement_Handler.C SgIfStmt_Handler.C eventProcessor.C \
					variableVersionTable.C costModel.C akgulStyleExpressionHandler.C \
					SgBasicBlock_Handler.C handlerTypes.C SgReturnStmt_Handler.C \
					SgVariableDeclaration_Handler.C redefineValueRestorer.C extractFromUseValueRestorer.C \
					SgWhileStmt_Handler.C stateSavingStatementHandler.C SgNewExp_Handler.C
libBackstrokePluggableReverser_la_includedir = $(includedir)/backstroke
libBackstrokePluggableReverser_la_include_HEADERS = expressionHandler.h SgNullStatement_Handler.h \
					SgIfStmt_Handler.h eventProcessor.h variableVersionTable.h costModel.h \
					handlerTypes.h SgBasicBlock_Handler.h akgulStyleExpressionHandler.h \
					SgReturnStmt_Handler.h SgVariableDeclaration_Handler.h redefineValueRestorer.h \
					extractFromUseValueRestorer.h SgWhileStmt_Handler.h stateSavingStatementHandler.h \
					expAndStmtHandlers.h SgNewExp_Handler.h

libBackstrokePluggableReverser_la_LIBADD = 

EXTRA_DIST = backstrokeRuntime.h backstrokeRuntime.C

CLEANFILES = 

ALL_TESTCODES =

check-local:
	@echo "*********************************************************************************************************"
	@echo "******* ROSE/projects/backstroke/pluggableReverser: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

