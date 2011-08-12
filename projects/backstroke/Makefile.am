include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = utilities eventDetection reverseComputation restrictedLanguage \
	normalizations slicing ssa valueGraph pluggableReverser testCodeGeneration . tests

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke

# Make a common backstroke library
# lib_LTLIBRARIES lists all the librarlies that should be built with libtool
lib_LTLIBRARIES =  libBackstroke.la

libBackstroke_la_DEPENDENCIES = 
libBackstroke_la_SOURCES = backstroke.C
libBackstroke_la_include_HEADERS = backstroke.h
libBackstroke_la_includedir = $(includedir)/backstroke

# Link all sublibs
libBackstroke_a_LIBADD = utilities/libBackstrokeUtilities.la \
			normalizations/libBackstrokeNormalizations.la \
			reverseComputation/libBackstrokeReverser.la \
			restrictedLanguage/libBackstrokeRestrictedLanguage.la \
			pluggableReverser/libBackstrokePluggableReverser.la \
			testCodeGeneration/libBackstrokeTestCodeBuilder.la
			
EXTRA_DIST = 

CLEANFILES = 

include_HEADERS =

check-local:
	@echo "**************************************************************************************"
	@echo "****** ROSE/projects/backstroke: make check rule complete (terminated normally) ******"
	@echo "**************************************************************************************"
