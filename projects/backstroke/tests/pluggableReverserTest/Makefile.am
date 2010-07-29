include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Build the test language restrictor executable
noinst_PROGRAMS = pluggableReverser mytest
pluggableReverser_SOURCES = pluggableReverserMain.C
pluggableReverser_LDADD = $(top_builddir)/projects/backstroke/restrictedLanguage/libBackstrokeRestrictedLanguage.la \
			$(top_builddir)/projects/backstroke/normalizations/libBackstrokeNormalizations.la \
			$(top_builddir)/projects/backstroke/pluggableReverser/libBackstrokePluggableReverser.la \
			$(top_builddir)/projects/backstroke/reverseComputation/libBackstrokeReverser.la \
			$(top_builddir)/projects/backstroke/utilities/libBackstrokeUtilities.la \
			$(LIBS_WITH_RPATH) $(ROSE_LIBS)

mytest_SOURCES = main.C
mytest_LDADD = $(top_builddir)/projects/backstroke/restrictedLanguage/libBackstrokeRestrictedLanguage.la \
			$(top_builddir)/projects/backstroke/normalizations/libBackstrokeNormalizations.la \
			$(top_builddir)/projects/backstroke/pluggableReverser/libBackstrokePluggableReverser.la \
			$(top_builddir)/projects/backstroke/reverseComputation/libBackstrokeReverser.la \
			$(top_builddir)/projects/backstroke/utilities/libBackstrokeUtilities.la \
			$(LIBS_WITH_RPATH) $(ROSE_LIBS)


EXTRA_DIST = 

CLEANFILES = 

#ALL_TESTCODES = test0.C

#TEST_Objects = ${ALL_TESTCODES:.C=.o}

TESTCODE_INCLUDES = -I$(top_srcdir)/projects/backstroke/tests/pluggableReverserTest/test

$(TEST_Objects):pluggableReverser 
	@cp $(top_srcdir)/projects/backstroke/tests/pluggableReverserTest/test/$(@:.o=.C) $(@:.o=.C)
	./pluggableReverser $(TESTCODE_INCLUDES) -c $(@:.o=.C)    

check-local:
#@$(MAKE) $(TEST_Objects)
	@echo "*********************************************************************************************************"
	@echo "* ROSE/projects/backstroke/tests/pluggableReverserTest: make check rule complete (terminated normally) *"
	@echo "*********************************************************************************************************"

test: FORCE
	@cp $(top_srcdir)/projects/backstroke/tests/pluggableReverserTest/test/test0.C test0.C
	./pluggableReverser $(TESTCODE_INCLUDES) -c test0.C

FORCE:
