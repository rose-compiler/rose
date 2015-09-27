include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Build the test language restrictor executable
noinst_PROGRAMS = pluggableReverser
pluggableReverser_SOURCES = pluggableReverserMain.C
pluggableReverser_LDADD = $(top_builddir)/projects/backstroke/libBackstroke.la \
			$(top_builddir)/projects/backstroke/restrictedLanguage/libBackstrokeRestrictedLanguage.la \
			$(top_builddir)/projects/backstroke/normalizations/libBackstrokeNormalizations.la \
			$(top_builddir)/projects/backstroke/pluggableReverser/libBackstrokePluggableReverser.la \
			$(top_builddir)/projects/backstroke/utilities/libBackstrokeUtilities.la \
			$(LIBS_WITH_RPATH) $(ROSE_LIBS)

ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 -rose:detect_dangling_pointers 1

EXTRA_DIST = 

CLEANFILES = 

TESTCODE_AM_CPPFLAGS = -I$(top_srcdir)/projects/backstroke/pluggableReverser -I$(top_srcdir)/projects/backstroke/tests/pluggableReverserTest/testcases

TESTCODES_REQUIRED_TO_PASS = \
test_Basic2.C \
test_Basic3.C

# noted for future work
TESTCODES_KNOWN_TO_FAIL = failingtests

TEST_Objects = ${TESTCODES_REQUIRED_TO_PASS:.C=.o}

$(TEST_Objects):pluggableReverser 
	@cp $(top_srcdir)/projects/backstroke/tests/pluggableReverserTest/testcases/$(@:.o=.C) $(@:.o=.C)
	./pluggableReverser $(ROSE_FLAGS) $(TESTCODE_AM_CPPFLAGS) -c $(@:.o=.C)    

check-local:
	@$(MAKE) $(TEST_Objects)
	@echo "*********************************************************************************************************"
	@echo "* ROSE/projects/backstroke/tests/pluggableReverserTest: make check rule complete (terminated normally) *"
	@echo "*********************************************************************************************************"

