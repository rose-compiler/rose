# This makefile serves as the canonical example for writing tests.  When creating your own, don't both
# copying these extensive comments -- refer back to this file instead if you want.
#
# See also, tests/smoke/ExamplesForTestWriters/Makefile.am


########################################################################################################################
# Prologue
#
# All makefiles start with these tidbits.

# Definitions for lots of common things.

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# if you are building ROSE translators, remember to set include path and libraries.
#AM_CPPFLAGS = $(ROSE_INCLUDES)
#LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Forward declarations so we can use make's "+=" operator.  We use "noinst_PROGRAMS" instead of "bin_PROGRAMS" because
# we generally don't want checkers used in testing to be installed by "make install".  The "TEST_TARGETS" variable
# will be a list of all the make targets that should be run: one for each test where a "test" is some "checker" run
# with one input "specimen" (or no inputs).

noinst_PROGRAMS =
TEST_TARGETS =



########################################################################################################################
# Input specimens.
#
# Many checkers (programs that test something) process a specimen (input) in order to test (verify) that some feature
# or ROSE works as expected.  If a specimen is expected to be used by many checkers in many directories, then put the
# specimen in a directory that contains only specimens. For non-smoke binary analysis specimens, that directory is
# $ROSE/tests/nonsmoke/specimens/binary.  On the other hand, if you expect a specimen to be useful only to checkers
# in one directory you might as well keep the two together for easier maintenance.
#
# A directory containing only specimens should organize those specimens into a list (or lists) that are stored in
# a "Specimens.inc" file that can be included into a makefile. All variables in a Specimen.inc file should have a
# common prefix so makefiles can include Specimen.inc files from multiple directories.  For instance, the variables
# defined in $ROSE/tests/nonsmoke/specimens/binary/Specimens.inc all start with "nonsmoke_specimens_binary_".  Other
# specimen directories should use a similar naming scheme (i.e., prefix reflects location in the directory hierarchy).
#
# Note about binary secimens in particular: The Specimens.inc file organizes the binary specimens into lists by
# container, file-type, and architecture as in "nonsmoke_specimens_binary_elf_exe_x86". There's also a SpecimenSets.inc
# file generated from Specimens.inc which creates wild-card lists using "any" for one or more of the parts of the
# variable names, as in "nonsmoke_specimens_binary_elf_exe_any" which means any ELF executable regardless of architecture.
# This example includes the latter file, which indirectly includes the former.

include $(top_srcdir)/tests/nonsmoke/specimens/binary/SpecimenSets.inc



########################################################################################################################
# A checker to test that something compiles.
#
# This is the simplest kind of checker -- all we do is make sure it compiles, and we never try to run it.

noinst_PROGRAMS += checkCompilation
checkCompilation_SOURCES = checkCompilation.C



########################################################################################################################
# A checker taking no specimen (i.e., a single test)
#
# The next level of checker is one that we compile and run, but it doesn't need to process any specimens.

noinst_PROGRAMS += noInputs
noInputs_SOURCES = noInputs.C

# In this case, the test is one checker run with no specimens. We'll use the checker name as the basis for the
# make target. The convention is that test targets end with ".passed", and that the list of all targets is saved
# in a make variable named $(TEST_TARGETS).

TEST_TARGETS += noInputs.passed

# The test itself should depend on the checker and all specimens (in this case none). The command should be to run
# the ROSE Test Harness (RTH). It's documentation can be found by running "$ROSE/scripts/rth_run.pl --help". In short,
# it takes variables of the form "VAR=VALUE", the name of some RTH configuration file, and the name of the make target.
# RTH substitutes the variables in the config file, processes the test, and saves the output either in the make target
# or a file created by substituting ".passed" with ".failed".  In this example, the test_exit_status config file needs
# variables for the test title (output by $(RTH_RUN)), the command to run, and whether to run the command in its own
# temporary directory (useful for parallel runs and cleaning up).  The title should always include "[$@]" so a
# user/developer knows how to run this test individually without needing to understand the makefile. $(RTH_RUN) will
# add the word "TEST" to the beginning of your title, so use something that's not awkward.
#
# You might ask, "Why are we hiding the commands behind a complicated test harness layer instead of executing them
# directly from the makefile?"  Here's what $(RTH_RUN) gets us:
#   1. It keeps the make output quiet by emitting standard output/error only if the test fails.
#   2. When it does emit output, each line is prefixed by the test name (important for parallel testing)
#   3. It stores the standard input and output of each test in its own *.passed or *.failed file for later perusal
#   4. It honors the "V=0" and "V=1" verbosity settings of make
#   5. It captures and saves the shell command(s) in the *.passed or *.failed file
#   6. It knows how to compare standard output with a precomputed, known, good answer file using "diff" and filters
#   7. It can run tests is temporary directories, which is useful for parallel testing and cleanup
#   8. A test can be disabled with a single-line explaination in the makefile and will show up in make's output
#   9. It enforces a timeout on the commands, which is important for automated testing in Jenkins and the matrix
#  10. It can optionally skip tests if the machine on which they run has too little memory
#  11. It can organize tests so that they don't need to run again if they already passed (i.e., *.passed exists)
#  12. Tests can share RTH configuration files so commands don't need to be repeated in the makefile.

noInputs.passed: noInputs
	@$(RTH_RUN)							\
		TITLE="run something basic [$@]"			\
		USE_SUBDIR=yes						\
		CMD="$$(pwd)/noInputs"					\
		$(top_srcdir)/scripts/test_exit_status $@



########################################################################################################################
# A checker taking specified specimens (i.e., many tests)
#
# Here's a checker that runs on three specific specimens. It's written using pattern rules so that the specific
# specimens can be easily changed, although in such a small example it might be clearer to list all three tests
# explicitly.

noinst_PROGRAMS += parser
parser_SOURCES = parser.C

# The explicit list of specimens. If you have specimens coming from multiple specimen directories you'll have to
# include the directory name and make special provisions below to prevent conflicts in the test target names.

parser_specdir = $(nonsmoke_specimens_binary_directory)
parser_specimens = i386-fcalls i386-fld i386-lea

# Create test target names by combining the checker name with the specimen name.  There are three specimens, thus
# we'll have three test names corresponding to three make targets. Again, we use the convention that test targets
# always end with ".passed".

parser_targets = $(addprefix parser_, $(addsuffix .passed, $(parser_specimens)))
TEST_TARGETS += $(parser_targets)

# For this example, we expect our checker to produce some output and we want to match that output against a
# known, good answer.  The convention for $(RTH_RUN) is that the answer is stored in the same directory as
# the checker and that they have the extension ".ans".  It's fine (and works) to use some other convention, although
# doing so ends up being slightly more work in the long run.

parser_answers = $(parser_targets:.passed=.ans)

# Declare the make targets for these tests using a pattern rule. Each test target depends on the checker, the
# input specimen, and the expected answer that corresponds with the test. The "$*" in the body expands to whatever
# the "%" matches in the pattern, which in this example is the specimen file name (without directory).  One could
# simplify this rule by making better use of $<, $^, and other make variables. The "USE_SUBDIR=yes" reminds us that
# file names passed to $(RTH_RUN) must be absolute; if the checker doesn't produce any output files then it might be
# easier to just run it in the current working directory (you could do that regardless, but then you're responsible
# for cleaning up after it without indiscriminately removing any additional files that a developer might have put
# there himself -- don't just say things like "rm *.o"!).

$(parser_targets): parser_%.passed: parser $(parser_specdir)/% parser_%.ans
	@$(RTH_RUN)						\
		TITLE="parsing is accurate for $* [$@]"	\
		USE_SUBDIR=yes					\
		CMD="$$(pwd)/parser $(parser_specdir)/$*"	\
		ANS="$(abspath $(srcdir))/parser_$*.ans"	\
		$(top_srcdir)/scripts/test_with_answer $@

# This step is optional, but sometimes it's useful to have additional make targets for running one group of tests,
# such as this checker run on these three specimens.  You can use whatever names you like (this is your makefile), but
# my approach is to use "check-<checkername>"

.PHONY: check-parser
check-parser: $(parser_targets)



########################################################################################################################
# A checker that runs on many specimens (i.e., many tests)
#
# Here's a more complicated example that runs one checker on many specimens with the specimen list coming from some
# other location. As the list is updated we automatically create more tests.
#
# We'll use the same "parser" checker as above, so no need for adding it noinst_PROGRAMS again.  However, we will need
# to use some other prefix for our variables, such as "parser2".

# Our specimens are all ELF executables except those for the MIPS architecture. This wildcard matching is why we've
# included SpecimenSets.inc instead of (or really, in addition to) Specimens.inc. The pattern is
# nonsmoke_specimens_binary_<container>_<filetype>_<architecture>. Another common example is filtering out specific
# specimens that you know are not meant to work (not shown here).

parser2_specdir = $(nonsmoke_specimens_binary_directory)
parser2_specimens = $(filter-out $(nonsmoke_specimens_binary_any_any_mips), $(nonsmoke_specimens_binary_elf_exe_any))

# Create the list of test targets
parser2_targets = $(addprefix parser2_, $(addsuffix .passed, $(parser2_specimens)))
TEST_TARGETS += $(parser2_targets)

# Create the make target rules. This time we don't compare with an answer file because it would mean that if someone
# added a new specimen they'd need to know how to also create a new answer file. Binary analysis often takes this
# approach (using answers) but everyone that would add a new binary specimen also knows how to create and manually
# verify the answers for each checker.

$(parser2_targets): parser2_%.passed: parser $(parser2_specdir)/%
	@$(RTH_RUN)						\
		TITLE="parsing passes (no answer) for $* [$@]"	\
		USE_SUBDIR=yes					\
		CMD="$$(pwd)/parser $(parser2_specdir)/$*"	\
		$(top_srcdir)/scripts/test_exit_status $@

# A rule for running just these tests
.PHONY: check-parser2
check-parser2: $(parser2_targets)


########################################################################################################################
# A smoke checker that needs to run on non-smoke specimens
#
# This time we have a checker on the smoke side (presumably because it needs to be run on some smoke specimens) that
# we also want to run on some non-smoke specimens.  The only change that needs to be made is that instead of compiling
# the checker in this directory, we need to compile it in the smoke directory.  If you have a lot of this situation then
# it's convenient to declare these two variables at the top of this non-smoke makefile:

# Smoke directories corresponding to our non-smoke $(srcdir) and $(blddir) directories
SMOKE_SRCDIR = $(abspath $(top_srcdir))/tests/smoke/ExamplesForTestWriters
SMOKE_BLDDIR = $(abspath $(top_builddir))/tests/smoke/ExamplesForTestWriters

# If the checker's dependencies are simple, here's the easy way to build it.
$(SMOKE_BLDDIR)/runAlgorithm: $(SMOKE_SRCDIR)/runAlgorithm.C
	$(MAKE) -C $(SMOKE_BLDDIR) runAlgorithm

# Get a list of specimens. We'll hard code a list this time.
runAlgorithm_specimens = $(nonsmoke_specimens_binary_elf_exe_amd64)

# Create the test targets
runAlgorithm_targets = $(addprefix runAlgorithm_, $(addsuffix .passed, $(runAlgorithm_specimens)))
TEST_TARGETS += $(runAlgorithm_targets)

# The test rules need to depend on the checker in the smoke build directory
$(runAlgorithm_targets): runAlgorithm_%.passed: $(SMOKE_BLDDIR)/runAlgorithm
	@$(RTH_RUN)						\
		TITLE="algorithm foo on $* [$@]"		\
		USE_SUBDIR=yes					\
		CMD="$< --algorithm=foo $*"			\
		$(top_srcdir)/scripts/test_exit_status $@

# A rule for running just these tests
.PHONY: check-runAlgorithm
check-runAlgorithm: $(runAlgorithm_targets)



########################################################################################################################
# Boilerplate.
#
# All testing makefiles have the following stuff, although it might vary a little.

# We need a target that says how "make check-smoke" and "make check-nonsmoke" should work in this directory. We only
# need one or the other depending on whether we're in the "smoke" or "nonsmoke" side of the testing.  If you're writing
# a makefile for a ROSE *project*, you might have a single makefile with both smoke and non-smoke tests, in which case
# you'd probably have $(SMOKE_TEST_TARGETS) and $(NONSMOKE_TEST_TARGETS), and define both check-smoke-local and
# check-nonsmoke-local rules.

# check-nonsmoke-local: $(TEST_TARGETS)
check-nonsmoke-local:
# DQ (2/19/2017): NEW VERSION OF LOGIC
# DQ (2/18/2017): More specific logic required for portability.
# When using EDG 4.9 these files don't compile on later versions of GNU, CLANG, and Intel compilers 
# but we still want them tests as we transition to EDG 4.12 (where they all work fine independent of 
# the compiler vendor and GNU compiler version).
if ROSE_USE_EDG_VERSION_4_9
	@echo "ROSE_USE_EDG_VERSION_4_9 == true"
if USING_GNU_COMPILER
	@echo "USING_GCC_COMPILER == true"
if ROSE_USING_GCC_VERSION_LATER_4_9
	@echo "ROSE_USING_GCC_VERSION_LATER_4_9 == true"
	@echo "ExamplesForTestWriters tests using EDG 4.9 and GNU version 4.9 backend compiler are disabled."
# Exclude these tests on GNU 4.9 and later compilers).
else  # NOT ROSE_USING_GCC_VERSION_LATER_4_8
# If this is not the CLANG or Intel compilers and an earlier version of GNU than GNU 4.9 compiler, so include these files.
#	@echo "ROSE_USING_GCC_VERSION_LATER_4_8 == false"
	@$(MAKE) $(TEST_TARGETS)
endif # ROSE_USING_GCC_VERSION_LATER_4_8
else  # NOT USING_GNU_COMPILER
#	@echo "USING_GCC_COMPILER == false"
if USING_CLANG_COMPILER
# Exclude this list of files in the case of CLANG compiler.
#	@echo "USING_CLANG_COMPILER == true"
	@echo "ExamplesForTestWriters tests using EDG 4.9: Clang backend compiler are NOT allowed with the EDG 4.9 frontend."
else # NOT USING_CLANG_COMPILER
#	@echo "USING_CLANG_COMPILER == false"
if USING_INTEL_COMPILER
# Exclude this list of files in the case of Intel compiler.
#	@echo "USING_INTEL_COMPILER == true"
	@echo "ExamplesForTestWriters tests using EDG 4.9 Intel backend compiler are allowed."
else # NOT USING_INTEL_COMPILER
# This is an unknown compiler.
#	@echo "USING_INTEL_COMPILER == false"
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER
else  # NOT ROSE_USE_EDG_VERSION_4_9
# Using EDG 4.12 or later (run all tests independent of the compiler).
#	@echo "ROSE_USE_EDG_VERSION_4_9 == false"
if USING_GNU_COMPILER
if ROSE_USING_GCC_VERSION_LATER_4_8
	@echo "ExamplesForTestWriters tests require configuration with either EDG version 4.9 and GNU version 4.8 or backend compiler or EDG 4.12 and 4.8 or any later GNU compiler."
	@echo "ExamplesForTestWriters tests using EDG 4.12 and any supported backend compiler are allowed."
	@$(MAKE) $(TEST_TARGETS)
endif # ROSE_USING_GCC_VERSION_LATER_4_8
else
	@echo "ROSE_USE_EDG_VERSION_4_9 == false"
if USING_CLANG_COMPILER
# Exclude this list of files in the case of CLANG compiler.
	@echo "USING_CLANG_COMPILER == true"
	@echo "ExamplesForTestWriters tests using EDG 4.9: Clang backend compiler using EDG 4.12 frontend."
# DQ (2/28/2017): Skip these files with Clang for now and come back to them later (when we understand the limits of portability better).
#	@$(MAKE) $(TEST_TARGETS)
else # NOT USING_CLANG_COMPILER
	@echo "USING_CLANG_COMPILER == false"
if USING_INTEL_COMPILER
# Exclude this list of files in the case of Intel compiler.
	@echo "USING_INTEL_COMPILER == true"
	@echo "ExamplesForTestWriters tests using EDG 4.12 Intel backend compiler are allowed."
	@$(MAKE) $(TEST_TARGETS)
else # NOT USING_INTEL_COMPILER
# This is an unknown compiler.
	@echo "USING_INTEL_COMPILER == false"
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER
endif # ROSE_USE_EDG_VERSION_4_9
	@echo "*****************************************************************************************************"
	@echo "****** ROSE/tests/smoke/ExamplesForTestWriters: make check rule complete (terminated normally) ******"
	@echo "*****************************************************************************************************"
