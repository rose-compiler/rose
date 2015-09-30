include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Build the function extraction translator
noinst_PROGRAMS = incrementalInversionTest

incrementalInversionTest_SOURCES =  incrementalInversionTest.C
incrementalInversionTest_LDADD =	$(top_builddir)/projects/backstroke/libBackstroke.la \
		 	$(top_builddir)/projects/backstroke/restrictedLanguage/libBackstrokeRestrictedLanguage.la \
			$(top_builddir)/projects/backstroke/pluggableReverser/libBackstrokePluggableReverser.la \
			$(top_builddir)/projects/backstroke/normalizations/libBackstrokeNormalizations.la \
			$(top_builddir)/projects/backstroke/utilities/libBackstrokeUtilities.la \
			$(LIBS_WITH_RPATH) $(ROSE_LIBS)


EXTRA_DIST = codes/new1.C

CLEANFILES = 

check-local-disabled:
	cp ${srcdir}/codes/new1.C new1.C
	./incrementalInversionTest --edg:no_warnings -w -I$(top_srcdir)/projects/backstroke/pluggableReverser -c new1.C

clean-local:
	rm -f *.C
	

