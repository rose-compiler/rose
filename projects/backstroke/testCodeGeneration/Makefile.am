include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LINK	 = $(CXXLINK)

# noinst_LIBRARIES lists all the librarlies that should be built, but not installed
noinst_LIBRARIES =  libBackstrokeTestCodeGeneration.a

libBackstrokeTestCodeGeneration_a_DEPENDENCIES = 
libBackstrokeTestCodeGeneration_a_SOURCES = buildTestCode.C testCodeBuilder.C
libBackstrokeTestCodeGeneration_a_include_HEADERS = buildTestCode.h testCodeBuilder.h
libBackstrokeTestCodeGeneration_a_includedir = $(includedir)/backstroke


# EXTRA_DIST are files that are not compiled or installed. These include readme's, internal header files, etc.
EXTRA_DIST = 

CLEANFILES = 


check-local:
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/testCodeGeneration: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

