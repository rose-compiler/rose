include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke

# Build a library for the language restrictor
noinst_LTLIBRARIES = libBackstrokeRestrictedLanguage.la
libBackstrokeRestrictedLanguage_ladir = .
libBackstrokeRestrictedLanguage_la_DEPENDENCIES =
libBackstrokeRestrictedLanguage_la_SOURCES = LanguageRestrictions.C
libBackstrokeRestrictedLanguage_la_includedir = $(includedir)/backstroke
libBackstrokeRestrictedLanguage_la_include_HEADERS = LanguageRestrictions.h

CLEANFILES = 

check-local:
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/restrictedLanguage: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

clean-local:
	

