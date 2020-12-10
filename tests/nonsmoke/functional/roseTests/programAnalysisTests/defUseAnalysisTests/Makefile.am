include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)

noinst_PROGRAMS  = runTest
runTest_SOURCES = runTest.C
runTest_LDADD = $(ROSE_SEPARATE_LIBS)

# Tests are numbered in runTest.C, and each test uses a hard-coded specimen.  Rather than duplicate the specimen-selecting
# logic of runTest.C in this makefile, we'll just make sure that each test depends on all the available specimens.
SPECIMEN_NUMBERS = $(shell seq 1 25)
SPECIMEN_NAMES = $(shell ls $(srcdir)/tests/test*.C)
TEST_CONFIG=$(srcdir)/runTest.conf
EXTRA_DIST = tests $(TEST_CONFIG)

TEST_TARGETS = $(addprefix runTest_, $(addsuffix .passed, $(SPECIMEN_NUMBERS)))
$(TEST_TARGETS): runTest_%.passed: runTest $(SPECIMEN_NAMES) $(TEST_CONFIG)
	@tnum="$@"; tnum="$${tnum%.passed}"; tnum="$${tnum#runTest_}"; $(RTH_RUN) TESTNUM=$$tnum $(TEST_CONFIG) $@

check-local: $(TEST_TARGETS)
	@echo "***************************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/roseTests/programAnalysisTests/defUseAnalysisTests: make check rule complete (terminated normally) ******"
	@echo "***************************************************************************************************************************"

clean-local:
	rm -rf $(MOSTLYCLEANFILES)
	rm -rf dfa.dot cfg.dot
	rm -rf $(TEST_TARGETS) $(TEST_TARGETS:.passed=.failed)
