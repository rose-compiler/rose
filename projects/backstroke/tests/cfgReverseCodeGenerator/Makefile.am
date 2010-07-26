include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Build the function extraction translator
noinst_PROGRAMS = reverserTest

reverserTest_SOURCES =  CFGReverserMain.C
reverserTest_LDADD = 	$(top_builddir)/projects/backstroke/restrictedLanguage/libBackstrokeRestrictedLanguage.la \
			$(top_builddir)/projects/backstroke/reverseComputation/libBackstrokeReverser.la \
			$(top_builddir)/projects/backstroke/utilities/libBackstrokeUtilities.la \
			$(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = 

CLEANFILES = 

check-local:


clean-local:
	

