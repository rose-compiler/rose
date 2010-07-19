include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = eventDetection reverseComputation restrictedLanguage normalizations tests

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke

# Make a common backstroke library
# lib_LTLIBRARIES lists all the librarlies that should be built with libtool
lib_LTLIBRARIES =  libBackstroke.la

libBackstroke_ladir = .
libBackstroke_la_DEPENDENCIES = 
libBackstroke_la_SOURCES = 
libBackstroke_la_HEADERS =
libBackstroke_la_LIBADD = normalizations/libBackstrokeNormalizations.la reverseComputation/libBackstrokeReverser.la \
			restrictedLanguage/libBackstrokeRestrictedLanguage.la

# Build the function extraction translator
bin_PROGRAMS = normalizationTranslator 

normalizationTranslator_SOURCES =  NormalizationTranslator.C 
normalizationTranslator_LDADD =   libBackstroke.la

EXTRA_DIST = 

CLEANFILES = 

include_HEADERS =

check-local:
	@echo "**************************************************************************************"
	@echo "****** ROSE/projects/backstroke: make check rule complete (terminated normally) ******"
	@echo "**************************************************************************************"
