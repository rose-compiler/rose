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

# DQ (3/26/2016): WARNING: This typeTrait testing represents a quadratic test on the size of the AST
# and as such is inapropriate for testing using large files.  This test takes 90 minutes on our fastest
# desktop machines and thus contributes an unacceptabily large burden to our ROSE testing infrastructure.
# As a result it is being disabled as a test.  All other tests are run as usual.
# A solution to improve the performance would be to have the GetAllMemberFunctionsInClassHierarchy() 
# function save computed results, since it appears to be an expensive operation that is repreated many 
# times for the same classes.
# Note that if we wanted to we could still make this a release level test, but it should not be a 
# development level test.
ROSE_HRD_TEST: typeTraitExerciseWithoutRetCode $(srcdir)/tests/roseHeader.C
	@echo "Skipping large file quadratic test of type traits on ROSE headers (too expensive for testing)."
#	./typeTraitExerciseWithoutRetCode $(SWITCHES) -c $(ROSE_FLAGS) -rose:verbose 0 -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/tests/roseHeader.C

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
