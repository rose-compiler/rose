include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = utilities eventDetection restrictedLanguage \
	normalizations slicing ssa valueGraph pluggableReverser testCodeGeneration . tests sdg

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke

# Make a common backstroke library
# lib_LTLIBRARIES lists all the libraries that should be built with libtool
lib_LTLIBRARIES =  libBackstroke.la

libBackstroke_la_DEPENDENCIES = 
libBackstroke_la_SOURCES = backstroke.C
libBackstroke_la_include_HEADERS = backstroke.h
libBackstroke_la_includedir = $(includedir)/backstroke

# Link all sublibs
libBackstroke_la_LIBADD = utilities/libBackstrokeUtilities.la \
			normalizations/libBackstrokeNormalizations.la \
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
