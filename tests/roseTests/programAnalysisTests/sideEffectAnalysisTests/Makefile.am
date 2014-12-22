include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# The testing of the side effect analysis depends upon the side effect analysis,
# which depends upon the database support within ROSE, which depends upon the use
# of sqlite, which is an external (and optional) piece of the ROSE puzzle.

if ROSE_USE_SQLITE_DATABASE

INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS = testSideEffect
testSideEffect_SOURCES = testSideEffect.C
testSideEffect_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# NOTE: testPtr2.C was not tested for some undocumented reason in the original makefile, so it not in this list either.
TEST_SPECIMENS = testGlobal1.C testGlobal2.C testRef.C testPtr1.C testClasses1.C testClasses2.C testClasses3.C testLoop.C
TEST_ANSWERS = $(addsuffix .ans, $(TEST_SPECIMENS))
TEST_TARGETS = $(addsuffix .passed, $(TEST_SPECIMENS))

EXTRA_DIST = $(TEST_SPECIMENS) $(TEST_ANSWERS)

# NOTE: foo.outx is a funny name for test answers; the usual name is foo.C.ans
$(TEST_TARGETS): %.passed: testSideEffect $(srcdir)/% $(srcdir)/%.ans $(top_srcdir)/scripts/test_with_answer
	@$(RTH_RUN)									\
		TITLE="testSideEffect $* [$@]"						\
		USE_SUBDIR=yes								\
		CMD="$$(pwd)/testSideEffect $(srcdir)/$* |grep -i 'side effect'"	\
		ANS="$(srcdir)/$*.ans"							\
		$(top_srcdir)/scripts/test_with_answer $@

clean-local:
	rm -f $(TEST_TARGETS) $(TEST_TARGETS:.passed=.failed)

check-local: $(TEST_TARGETS)

endif
