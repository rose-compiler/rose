include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
noinst_PROGRAMS =
EXTRA_DIST =
TEST_TARGETS =

TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

#------------------------------------------------------------------------------------------------------------------------
# runTest

noinst_PROGRAMS  += runTest
runTest_SOURCES = runTest.C
runTest_CPPFLAGS = $(ROSE_INCLUDES)
runTest_LDFLAGS = $(ROSE_RPATHS)
runTest_LDADD = $(ROSE_SEPARATE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# tests

EXTRA_DIST += simpletest.php
TEST_TARGETS += runTest.passed

runTest.passed: simpletest.php runTest
	@$(RTH_RUN) \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/runTest $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# automake boilerplate

check-local: $(TEST_TARGETS)

clean-local:
	rm -f $(TEST_TARGETS) $(TEST_TARGETS:.passed=.failed)
