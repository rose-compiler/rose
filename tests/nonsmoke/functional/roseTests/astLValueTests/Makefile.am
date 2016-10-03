include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
noinst_PROGRAMS =
EXTRA_DIST =
TEST_TARGETS =

TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

#------------------------------------------------------------------------------------------------------------------------
# testLValues

noinst_PROGRAMS += testLValues
testLValues_SOURCES = runTest.C
testLValues_CPPFLAGS = $(ROSE_INCLUDES)
testLValues_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

TEST_TARGETS += testLValues_1.passed
EXTRA_DIST += test_astLValues.C
testLValues_1.passed: test_astLValues.C testLValues
	@$(RTH_RUN) \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/testLValues $(abspath $<)" \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# automake boilerplate

check-local: $(TEST_TARGETS)

clean-local:
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
