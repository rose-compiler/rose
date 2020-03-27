include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
noinst_PROGRAMS =
TEST_TARGETS =
EXTRA_DIST =

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)
TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

test_input_sources = mangleTest.C mangleTwo.C mangleThree.C
test_input_headers = mangleTest.h mangleTwo.h
test_input_files = $(test_input_sources) $(test_input_headers)
EXTRA_DIST += $(test_input_files)

#------------------------------------------------------------------------------------------------------------------------
# testMerge executable
noinst_PROGRAMS += testMerge
testMerge_SOURCES = testMerge.C
testMerge_LDADD = $(ROSE_SEPARATE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# tests of the testMerge executable
testMerge_CMD = ./testMerge -rose:verbose 0
testMerge_TESTS = testMerge_test1.passed testMerge_test2.passed testMerge_test3.passed
TEST_TARGETS += $(testMerge_TESTS)

.PHONY: check_testMerge
check_testMerge: $(testMerge_TESTS)

testMerge_test1.passed: testMerge $(test_input_files)
	@$(RTH_RUN) \
		CMD="$(testMerge_CMD) -rose:ast:merge -c $(srcdir)/mangleTest.C $(srcdir)/mangleTwo.C" \
		$(TEST_EXIT_STATUS) $@

testMerge_test2.passed: testMerge $(test_input_files)
	@$(RTH_RUN) \
		CMD="$(testMerge_CMD) -c $(srcdir)/mangleTest.C $(srcdir)/mangleTwo.C $(srcdir)/mangleThree.C" \
		DISABLED="Simplify this test since merging all three files causes an error at present [DQ 5/30/2007]" \
		$(TEST_EXIT_STATUS) $@

testMerge_test3.passed: testMerge $(test_input_files)
	@$(RTH_RUN) \
		CMD="$(testMerge_CMD) -rose:ast:merge -c $(srcdir)/mangleTest.C $(srcdir)/mangleTwo.C $(srcdir)/mangleThree.C" \
		DISABLED="Simplify this test since merging all three files causes an error at present [DQ 5/30/2007]" \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# automake boilerplate

check-local: $(TEST_TARGETS)

clean-local:
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
	rm -f mangleTest--mangleTwo.C.dot
