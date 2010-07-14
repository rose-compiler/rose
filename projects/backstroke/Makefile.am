include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = utilities eventDetection reverseComputation restrictedLanguage normalizations pluggableReverser tests

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke

# Make a common backstroke library
# lib_LTLIBRARIES lists all the librarlies that should be built with libtool
lib_LTLIBRARIES =  libBackstroke.la

libBackstroke_ladir = .
libBackstroke_la_DEPENDENCIES = 
libBackstroke_la_SOURCES = 
libBackstroke_la_HEADERS =

# Put the backstroke headers in their own subdirectory
libBackstroke_includedir = $(includedir)/backstroke

# Link all subl
libBackstroke_la_LIBADD = utilities/libBackstrokeUtilities.la \
			normalizations/libBackstrokeNormalizations.la \
			reverseComputation/libBackstrokeReverser.la \
			restrictedLanguage/libBackstrokeRestrictedLanguage.la \
			pluggableReverser/libBackstrokePluggableReverser.la

EXTRA_DIST = 

CLEANFILES = 

include_HEADERS =

check-local:
	@echo "**************************************************************************************"
	@echo "****** ROSE/projects/backstroke: make check rule complete (terminated normally) ******"
	@echo "**************************************************************************************"
