include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
bin_PROGRAMS =
EXTRA_DIST =
TEST_TARGETS =

TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status
AM_CPPFLAGS = $(ROSE_INCLUDES)
SPECIMENS = input1.C
EXTRA_DIST += $(SPECIMENS)

#------------------------------------------------------------------------------------------------------------------------
bin_PROGRAMS += testQuery
testQuery_SOURCES = testQuery.C
testQuery_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

testQuery_TEST_TARGETS = $(addprefix testQuery_, $(addsuffix .passed, $(SPECIMENS)))
TEST_TARGETS += $(testQuery_TEST_TARGETS)
$(testQuery_TEST_TARGETS): testQuery_%.passed: $(srcdir)/% testQuery
	@$(RTH_RUN)						\
		TITLE="testQuery $(notdir $<) [$@]"		\
		USE_SUBDIR=yes					\
		CMD="$$(pwd)/testQuery -c $(abspath $<)"	\
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
bin_PROGRAMS += testQuery2
testQuery2_SOURCES = testQuery2.C
testQuery2_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

testQuery2_TEST_TARGETS = $(addprefix testQuery2_, $(addsuffix .passed, $(SPECIMENS)))
TEST_TARGETS += $(testQuery2_TEST_TARGETS)
$(testQuery2_TEST_TARGETS): testQuery2_%.passed: $(srcdir)/% testQuery2
	@$(RTH_RUN)						\
		TITLE="testQuery2 $(notdir $<) [$@]"		\
		USE_SUBDIR=yes					\
		CMD="$$(pwd)/testQuery2 -c $(abspath $<)"	\
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
bin_PROGRAMS += testQuery3
testQuery3_SOURCES = testQuery3.C
testQuery3_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

testQuery3_TEST_TARGETS = $(addprefix testQuery3_, $(addsuffix .passed, $(SPECIMENS)))
TEST_TARGETS += $(testQuery3_TEST_TARGETS)
$(testQuery3_TEST_TARGETS): testQuery3_%.passed: $(srcdir)/% testQuery3
	@$(RTH_RUN)						\
		TITLE="testQuery3 $(notdir $<) [$@]"		\
		USE_SUBDIR=yes					\
		CMD="$$(pwd)/testQuery3 -c $(abspath $<)"	\
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# These tests were not actually ever executed in the original makefile, so they're marked as disabled.

TEST_TARGETS += test1.passed
EXTRA_DIST += test1.conf
#test1.passed: test1.conf test1A.C test1B.C  #THESE FILES ARE NOT PART OF ROSE'S SOURCE TREE
test1.passed: test1.conf
	@$(RTH_RUN) $< $@

TEST_TARGETS += test2.passed
test2.passed: input1.C testQuery2
	@$(RTH_RUN) \
		DISABLED="This test was not enabled in the original makefile [RPM 2013-09-24]" \
		CMD="./testQuery2 $(ROSE_FLAGS) -rose:verbose 0 -c $<" \
		$(TEST_EXIT_STATUS) $@

TEST_TARGETS += test3.passed
test3.passed: $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C testQuery3
	@$(RTH_RUN) \
		DISABLED="This test was not enabled in the original makefile [RPM 2013-09-24]" \
		CMD="./testQuery3 -rose:verbose 0 -I$(top_builddir) $(ROSE_INCLUDES) -c $<" \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# Automake boilerplate

check-local: $(TEST_TARGETS)

clean-local:
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
