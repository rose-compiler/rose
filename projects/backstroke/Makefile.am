include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = utilities eventDetection reverseComputation restrictedLanguage \
	normalizations slicing ssa valueGraph pluggableReverser testCodeGeneration . tests

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke

# Make a common backstroke library
# lib_LTLIBRARIES lists all the librarlies that should be built with libtool
lib_LIBRARIES =  libBackstroke.a

libBackstroke_a_DEPENDENCIES = 
libBackstroke_a_SOURCES = backstroke.C
libBackstroke_a_include_HEADERS = backstroke.h
libBackstroke_a_includedir = $(includedir)/backstroke

# Link all sublibs
libBackstroke_a_LIBADD = utilities/libBackstrokeUtilities.a \
			normalizations/libBackstrokeNormalizations.a \
			reverseComputation/libBackstrokeReverser.a \
			restrictedLanguage/libBackstrokeRestrictedLanguage.a \
			pluggableReverser/libBackstrokePluggableReverser.a \
			testCodeGeneration/libBackstrokeTestCodeBuilder.a \
			ssa/libSSA.a

EXTRA_DIST = 

CLEANFILES = 

include_HEADERS =

check-local:
	@echo "**************************************************************************************"
	@echo "****** ROSE/projects/backstroke: make check rule complete (terminated normally) ******"
	@echo "**************************************************************************************"
