include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status
AM_CPPFLAGS = $(ROSE_INCLUDES)

bindir = ${exec_prefix}/bin/testsuite
bin_PROGRAMS  = typeTraitExerciseWithRetCode typeTraitExerciseWithoutRetCode

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
	@$(RTH_RUN) CMD="./typeTraitExerciseWithRetCode $(SWITCHES) -c $<" $(TEST_EXIT_STATUS)  $@

ROSE_HRD_TEST: typeTraitExerciseWithoutRetCode $(srcdir)/tests/roseHeader.C
	./typeTraitExerciseWithoutRetCode -c $(ROSE_FLAGS) -rose:verbose 2 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/tests/roseHeader.C

check-local:
# type traits is a not fully supported in older gcc versions. We enable tests only when rose is compiled with gcc 4.2 or higher and the backend compiler used is also gcc 4.2 or higher.
	if  test "$(GCC_VERSION)" -ge "4" && test "$(GCC_MINOR_VERSION)" -ge "4" && \
            test "$(BACKEND_COMPILER_VERSION_MAJOR_NUMBER)" -ge "4" && test "$(BACKEND_COMPILER_VERSION_MINOR_NUMBER)" -ge "4"; then \
		$(MAKE) $(TEST_TARGETS) ROSE_HRD_TEST || exit 1 ; \
		echo "**********************************************************************************************************************"; \
		echo "****** ROSE/tests/roseTests/programAnalysisTests/typeTraitTests: make check rule complete (terminated normally) ******"; \
		echo "**********************************************************************************************************************"; \
	else \
		echo "***********************************************************************************************"; \
		echo "****** ROSE/tests/roseTests/programAnalysisTests/typeTraitTests: make check skipping tests ****"; \
		echo "****** Minimum GNU version needed for type traits is 4.2.x or higher **************************"; \
		echo "***********************************************************************************************"; \
	fi

clean-local:
	rm -rf $(MOSTLYCLEANFILES)
	rm -rf dfa.dot cfg.dot
	rm -rf $(TEST_TARGETS) $(TEST_TARGETS:.passed=.failed)
