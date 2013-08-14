include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status
INCLUDES = $(ROSE_INCLUDES)

noinst_PROGRAMS  = typeTraitExerciseWithRetCode typeTraitExerciseWithoutRetCode

typeTraitExerciseWithRetCode_SOURCES = typeTraitExerciseWithRetCode.C
typeTraitExerciseWithRetCode_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

typeTraitExerciseWithoutRetCode_SOURCES = typeTraitExerciseWithoutRetCode.C
typeTraitExerciseWithoutRetCode_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

SPECIMEN_NUMBERS = $(shell seq 1 16)
SPECIMEN_NAMES = $(addprefix test, $(addsuffix .C, $(SPECIMEN_NUMBERS)))
TESTDIR = $(srcdir)/tests/

TEST_TARGETS = $(addprefix nt_, $(addsuffix .passed, $(SPECIMEN_NAMES)))

SWITCHES = \
        --edg:no_warnings -w 

$(TEST_TARGETS): nt_%.passed: $(TESTDIR)/% typeTraitExerciseWithRetCode $(TEST_EXIT_STATUS)
	echo '@$(RTH_RUN) CMD="./typeTraitExerciseWithRetCode $(SWITCHES) -c $<" $(TEST_EXIT_STATUS) $@'
	@$(RTH_RUN) CMD="./typeTraitExerciseWithRetCode $(SWITCHES) -c $<" $(TEST_EXIT_STATUS)  $@

ROSE_HRD_TEST: typeTraitExerciseWithoutRetCode $(srcdir)/tests/roseHeader.C
	./typeTraitExerciseWithoutRetCode -c $(ROSE_FLAGS) -rose:verbose 2 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/tests/roseHeader.C

check-local: $(TEST_TARGETS) ROSE_HRD_TEST

clean-local:
	rm -rf $(MOSTLYCLEANFILES)
	rm -rf dfa.dot cfg.dot
	rm -rf $(TEST_TARGETS) $(TEST_TARGETS:.passed=.failed)
	rm -rf $(TEST_TARGETS:.passed=.err) $(TEST_TARGETS:.passed=.out)
