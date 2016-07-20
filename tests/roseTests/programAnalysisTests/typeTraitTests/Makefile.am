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

# The original comment was:
#     type traits is a not fully supported in older gcc versions. We enable tests only when rose is compiled with gcc 4.2 or
#     higher and the backend compiler used is also gcc 4.2 or higher.
#
# But the logic to implement that was really messed up:
#     (1) It didn't check that we're using GCC instead of some other compiler to compile ROSE
#     (2) It didn't check that we're using GCC instead of some other compiler for the backend
#     (3) It tested minor version "4" instead of "2" for the compiler used to compile ROSE
#     (4) It tested minor version "4" instead of "2" for the backend compiler
#     (5) It assumed versions "5.0", "5.1", "5.2" and "5.3" are older than "4.4" (same for major > 5)
#
# I'm keeping the original logic about gcc vs. other compilers since that's what is being actually tested in Jenkins and
# passing, but I'm implementing a black-list with proper version number comparisons instead of using the original messed
# up white-list. [Matzke, 2016-03-29]
#
# The main purpose for doing things this way instead of using recursive make is that we want parallelism to work
# properly. The recursive method adds "-j1" on some systems.  We should try to avoid rules like this:
#     |some-rule:
#     |    test some-condition && $(MAKE) test_targets
# in favor of
#     |some-rule: $(conditional_test_targets)
# because the latter works correctly in parallel

# Black-listed compilers. ROSE doesn't support gcc <4.0, therefore no need to list them here.
type_traits_test_disabled_gcc_4_0 = not supported for gcc version 4.0
type_traits_test_disabled_gcc_4_1 = not supported for gcc version 4.1
type_traits_test_disabled_gcc_4_2 = not supported for gcc version 4.2
type_traits_test_disabled_gcc_4_3 = not supported for gcc version 4.3

type_traits_test_disabled = \
    $(strip $(or \
	 $(type_traits_test_disabled_gcc_$(GCC_VERSION)_$(GCC_MINOR_VERSION)), \
         $(type_traits_test_disabled_gcc_$(BACKEND_COMPILER_VERSION_MAJOR_NUMBER)_$(BACKEND_COMPILER_VERSION_MINOR_NUMBER))))

# Run all tests, but conditionally disable some
$(TEST_TARGETS): nt_%.passed: $(TESTDIR)/% typeTraitExerciseWithRetCode $(TEST_EXIT_STATUS)
	@$(RTH_RUN) \
		TITLE="type trait exercise with ret code [$@]" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/typeTraitExerciseWithRetCode $(SWITCHES) -c $<" \
		DISABLED="$(type_traits_test_disabled)" \
		$(TEST_EXIT_STATUS)  $@

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
	@if [ "$(type_traits_test_disabled)" = "" ]; then					\
		echo "  TEST   hard test [$@] (disabled: quadratic algorithm is too expensive)";\
		#(set -x;									\
		#       ./typeTraitExerciseWithoutRetCode					\
		#		-c $(ROSE_FLAGS) -rose:verbose 2 -I$(top_builddir)		\
		#		$(ROSE_INCLUDES) -c $(srcdir)/tests/roseHeader.C		\
		#);										\
	else											\
		echo "  TEST   hard test [$@] (disabled: $(type_traits_test_disabled))";	\
	fi

check-local: $(TEST_TARGETS) ROSE_HRD_TEST

clean-local:
	rm -rf $(MOSTLYCLEANFILES)
	rm -rf dfa.dot cfg.dot
	rm -rf $(TEST_TARGETS) $(TEST_TARGETS:.passed=.failed)
