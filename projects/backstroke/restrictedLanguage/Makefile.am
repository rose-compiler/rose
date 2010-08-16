include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke

# Build a library for the language restrictor
noinst_LIBRARIES = libBackstrokeRestrictedLanguage.a
libBackstrokeRestrictedLanguage_a_DEPENDENCIES =
libBackstrokeRestrictedLanguage_a_SOURCES = LanguageRestrictions.C
libBackstrokeRestrictedLanguage_a_includedir = $(includedir)/backstroke
libBackstrokeRestrictedLanguage_a_include_HEADERS = LanguageRestrictions.h

CLEANFILES = 

check-local:
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/restrictedLanguage: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

clean-local:
	

