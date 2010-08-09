include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke

# Build the test language restrictor executable
bin_PROGRAMS = eventChecker
eventChecker_SOURCES = EventChecker.C
eventChecker_LDADD = $(top_builddir)/projects/backstroke/restrictedLanguage/libBackstrokeRestrictedLanguage.la \
			$(top_builddir)/projects/backstroke/utilities/libBackstrokeUtilities.la \
			$(LIBS_WITH_RPATH) $(ROSE_LIBS)

TESTCODES_REQUIRED_TO_PASS = \
   testcode_01.C \
   testcode_02.C

TESTCODE_CURRENTLY_FAILING =

TESTCODES = $(TESTCODES_REQUIRED_TO_PASS) 

ALL_TESTCODES = $(TESTCODES_REQUIRED_TO_PASS) $(TESTCODE_CURRENTLY_FAILING)

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

ROSE_FLAGS =


$(TEST_Objects): eventChecker
	$(VALGRIND) ./eventChecker $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)


EXTRA_DIST = $(TESTCODES)

CLEANFILES = 

check-local:
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "*********************************************************************************************************"
	@echo "* ROSE/projects/backstroke/tests//restrictedLanguageTest: make check rule complete (terminated normally) *"
	@echo "*********************************************************************************************************"

clean-local:
	rm -rf rose_testcode_??.C

