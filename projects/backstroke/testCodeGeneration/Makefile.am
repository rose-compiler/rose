include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LINK	 = $(CXXLINK)

# noinst_LTLIBRARIES lists all the librarlies that should be built, but not installed
noinst_LTLIBRARIES =  libBackstrokeTestCodeBuilder.la

libBackstrokeTestCodeBuilder_la_DEPENDENCIES = 
libBackstrokeTestCodeBuilder_la_SOURCES = builderTypes.C testCodeBuilder.C testExpressionBuilder.C testStatementBuilder.C
libBackstrokeTestCodeBuilder_la_include_HEADERS = builderTypes.h testCodeBuilder.h testExpressionBuilder.h testStatementBuilder.h

libBackstrokeTestCodeBuilder_la_includedir = $(includedir)/backstroke


# EXTRA_DIST are files that are not compiled or installed. These include readme's, internal header files, etc.
EXTRA_DIST = 

CLEANFILES = 


check-local:
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/testCodeGeneration: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

