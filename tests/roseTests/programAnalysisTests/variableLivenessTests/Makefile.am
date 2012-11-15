include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
INCLUDES = $(ROSE_INCLUDES)

noinst_PROGRAMS = runTest
runTest_SOURCES = runTest.C
runTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

# Tests are numbered in runTest.C, and each test uses a hard-coded specimen.  Rather than duplicate the specimen-selecting
# logic of runTest.C in this makefile, we'll just make sure that each test depends on all the available specimens.
SPECIMEN_NUMBERS = $(shell seq 1 11) 13 14 15 $(shell seq 18 25)
SPECIMEN_NAMES = $(shell ls $(srcdir)/tests/test*.C)
TEST_CONFIG=$(srcdir)/runTest.conf
EXTRA_DIST = tests $(TEST_CONFIG)

TEST_TARGETS = $(addprefix runTest_, $(addsuffix .passed, $(SPECIMEN_NUMBERS)))
$(TEST_TARGETS): runTest_%.passed: runTest $(SPECIMEN_NAMES) $(TEST_CONFIG)
	@tnum="$@"; tnum="$${tnum%.passed}"; tnum="$${tnum#runTest_}"; $(RTH_RUN) TESTNUM=$$tnum $(TEST_CONFIG) $@

check-local: $(TEST_TARGETS)

clean-local:
	rm -rf $(MOSTLYCLEANFILES)
	rm -rf dfa.dot cfg.dot var.dot varFix.dot
	rm -rf $(TEST_TARGETS) $(TEST_TARGETS:.passed=.failed)
	rm -rf $(TEST_TARGETS:.passed=.err) $(TEST_TARGETS:.passed=.out)
