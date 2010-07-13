include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Build the test language restrictor executable
noinst_PROGRAMS = pluggableReverser
pluggableReverser_SOURCES = pluggableReverserMain.C
pluggableReverser_LDADD = $(top_builddir)/projects/backstroke/restrictedLanguage/libBackstrokeRestrictedLanguage.la \
			$(top_builddir)/projects/backstroke/normalizations/libBackstrokeNormalizations.la \
			$(top_builddir)/projects/backstroke/pluggableReverser/libBackstrokePluggableReverser.la \
			$(top_builddir)/projects/backstroke/reverseComputation/libBackstrokeReverser.la \
			$(top_builddir)/projects/backstroke/utilities/libBackstrokeUtilities.la

EXTRA_DIST = 

CLEANFILES = 

check-local:
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "*********************************************************************************************************"
	@echo "* ROSE/projects/backstroke/tests/pluggableReverserTest: make check rule complete (terminated normally) *"
	@echo "*********************************************************************************************************"


