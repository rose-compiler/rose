include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LINK	 = $(CXXLINK)

# noinst_LIBRARIES lists all the librarlies that should be built, but not installed
noinst_LIBRARIES =  libBackstrokeNormalizations.a

libBackstrokeNormalizations_a_DEPENDENCIES = 
libBackstrokeNormalizations_a_SOURCES = ExtractFunctionArguments.C expNormalization.C
libBackstrokeNormalizations_a_include_HEADERS = ExtractFunctionArguments.h expNormalization.h
libBackstrokeNormalizations_a_includedir = $(includedir)/backstroke


# EXTRA_DIST are files that are not compiled or installed. These include readme's, internal header files, etc.
EXTRA_DIST = LIMITATIONS.txt

CLEANFILES = 


check-local:
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/normalizations: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"
