include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Build the function extraction translator
noinst_PROGRAMS = normalizationTranslator

normalizationTranslator_SOURCES =  NormalizationTranslator.C
normalizationTranslator_LDADD = $(top_builddir)/projects/backstroke/normalizations/libBackstrokeNormalizations.la \
			$(top_builddir)/projects/backstroke/utilities/libBackstrokeUtilities.la

EXTRA_DIST = test_basic.C test_failingTests.C test_forLoops.C test_Swap.C test_whileLoops.C

CLEANFILES = 

check-local:


clean-local:
	

