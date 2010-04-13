include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = lang
lang_SOURCES = lang.C

INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

TESTCODES_REQUIRED_TO_PASS = \
   testcode_01.C \
   testcode_02.C 

TESTCODE_CURRENTLY_FAILING =

TESTCODES = $(TESTCODES_REQUIRED_TO_PASS) 

ALL_TESTCODES = $(TESTCODES_REQUIRED_TO_PASS) $(TESTCODE_CURRENTLY_FAILING)

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

ROSE_FLAGS =


$(TEST_Objects): lang
	$(VALGRIND) ./lang $(ROSE_FLAGS) -I$(srcdir) -c $(srcdir)/$(@:.o=.C)


EXTRA_DIST = $(TESTCODES)

CLEANFILES = 

include_HEADERS =

check-local:
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/restrictedLanguage: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

clean-local:
	rm rose_testcode_??.C

