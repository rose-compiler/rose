include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Build a library for the language restrictor
noinst_LTLIBRARIES = libBackstrokeRestrictedLanguage.la
libBackstrokeRestrictedLanguage_ladir = .
libBackstrokeRestrictedLanguage_la_DEPENDENCIES =
libBackstrokeRestrictedLanguage_la_SOURCES = LanguageRestrictions.C
include_HEADERS = LanguageRestrictions.h
libBackstrokeRestrictedLanguage_la_LIBADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Build the test language restrictor executable
bin_PROGRAMS = eventChecker
eventChecker_SOURCES = EventChecker.C
eventChecker_LDADD = libBackstrokeRestrictedLanguage.la


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
	@echo "****** ROSE/projects/backstroke/restrictedLanguage: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

clean-local:
	rm -rf rose_testcode_??.C

