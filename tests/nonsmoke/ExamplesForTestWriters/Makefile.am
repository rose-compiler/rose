# This makefile serves as the canonical example for writing tests.  When creating your own, don't both
# copying these extensive comments -- refer back to this file instead if you want.


########################################################################################################################
# Prologue
#
# All makefiles start with these tidbits.

# Definitions for lots of common things.

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

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

noinst_PROGRAMS += checkNoInputs
checkNoInputs_SOURCES = checkNoInputs.C

# In this case, the test is one checker run with no specimens. We'll use the checker name as the basis for the
# make target. The convention is that test targets end with ".passed", and that the list of all targets is saved
# in a make variable named $(TEST_TARGETS).

TEST_TARGETS += checkNoInputs.passed

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

checkNoInputs.passed: checkNoInputs
	@$(RTH_RUN)							\
		TITLE="run something basic [$@]"			\
		CMD="$$(pwd)/checkNoInputs"				\
		USE_SUBDIR=yes						\
		$(abspath $(top_srcdir))/scripts/test_exit_status $@



########################################################################################################################
# A checker taking specified specimens (i.e., many tests)
#
# Here's a checker that runs on three specific specimens. It's written using pattern rules so that the specific
# specimens can be easily changed, although in such a small example it might be clearer to list all three tests
# explicitly.

noinst_PROGRAMS += checkWithInput
checkWithInput_SOURCES = checkWithInput.C

# The explicit list of specimens. If you have specimens coming from multiple specimen directories you'll have to
# include the directory name and make special provisions below to prevent conflicts in the test target names.

checkWithInput_specdir = $(nonsmoke_specimens_binary_directory)
checkWithInput_specimens = i386-fcalls i386-fld i386-lea

# Create test target names by combining the checker name with the specimen name.  There are three specimens, thus
# we'll have three test names corresponding to three make targets. Again, we use the convention that test targets
# always end with ".passed".

checkWithInput_targets = $(addprefix checkWithInput_, $(addsuffix .passed, $(checkWithInput_specimens)))
TEST_TARGETS += $(checkWithInput_targets)

# For this example, we expect our checker to produce some output and we want to match that output against a
# known, good answer.  The convention for $(RTH_RUN) is that the answer is stored in the same directory as
# the checker and that they have the extension ".ans".  It's fine (and works) to use some other convention, although
# doing so ends up being slightly more work in the long run.

checkWithInput_answers = $(checkWithInput_targets:.passed=.ans)

# Declare the make targets for these tests using a pattern rule. Each test target depends on the checker, the
# input specimen, and the expected answer that corresponds with the test. The "$*" in the body expands to whatever
# the "%" matches in the pattern, which in this example is the specimen file name (without directory).  One could
# simplify this rule by making better use of $<, $^, and other make variables. The "USE_SUBDIR=yes" reminds us that
# file names passed to $(RTH_RUN) must be absolute; if the checker doesn't produce any output files then it might be
# easier to just run it in the current working directory (you could do that regardless, but then you're responsible
# for cleaning up after it without indiscriminately removing any additional files that a developer might have put
# there himself -- don't just say things like "rm *.o"!).

$(checkWithInput_targets): checkWithInput_%.passed: checkWithInput $(checkWithInput_specdir)/% checkWithInput_%.ans
	@$(RTH_RUN)								\
		TITLE="parsing is accurate for $* [$@]"			\
		CMD="$$(pwd)/checkWithInput $(checkWithInput_specdir)/$*"	\
		ANS="$(abspath $(srcdir))/checkWithInput_$*.ans"		\
		USE_SUBDIR=yes							\
		$(abspath $(top_srcdir))/scripts/test_with_answer $@

# This step is optional, but sometimes it's useful to have additional make targets for running one group of tests,
# such as this checker run on these three specimens.  You can use whatever names you like (this is your makefile), but
# my approach is to use "check-<checkername>" (which in this example with its generic checker name is a little awkward).

.PHONY: check-checkWithInput
check-checkWithInput: $(checkWithInput_targets)



########################################################################################################################
# A checker that runs on many specimens (i.e., many tests)
#
# Here's a more complicated example that runs one checker on many specimens with the specimen list coming from some
# other location. As the list is updated we automatically create more tests.
#
# We'll use the same checkWithInput as above, so no need for adding it noinst_PROGRAMS again.  However, we will need
# to use some other prefix for our variables, such as "cwi2" (for checkWithInput #2).

# Our specimens are all ELF executables except those for the MIPS architecture. This wildcard matching is why we've
# included SpecimenSets.inc instead of (or really, in addition to) Specimens.inc. The pattern is
# nonsmoke_specimens_binary_<container>_<filetype>_<architecture>. Another common example is filtering out specific
# specimens that you know are not meant to work (not shown here).

cwi2_specdir = $(nonsmoke_specimens_binary_directory)
cwi2_specimens = $(filter-out $(nonsmoke_specimens_binary_any_any_mips), $(nonsmoke_specimens_binary_elf_exe_any))

# Create the list of test targets
cwi2_targets = $(addprefix cwi2_, $(addsuffix .passed, $(cwi2_specimens)))
TEST_TARGETS += $(cwi2_targets)

# Create the make target rules. This time we don't compare with an answer file because it would mean that if someone
# added a new specimen they'd need to know how to also create a new answer file. Binary analysis often takes this
# approach (using answers) but everyone that would add a new binary specimen also knows how to create and manually
# verify the answers for each checker.

$(cwi2_targets): cwi2_%.passed: checkWithInput $(cwi2_specdir)/%
	@$(RTH_RUN)								\
		TITLE="parsing passes (no answer) for $* [$@]"			\
		CMD="$$(pwd)/checkWithInput $(checkWithInput_specdir)/$*"	\
		USE_SUBDIR=eys							\
		$(abspath $(top_srcdir))/scripts/test_exit_status $@

# A rule for running just these tests
.PHONY: check-cwi2
check-cwi2: $(cwi2_targets)



########################################################################################################################
# Boilerplate.
#
# All testing makefiles have the following stuff, although it might vary a little.

# We need a target that says how "make check-smoke" and "make check-nonsmoke" should work in this directory. We only
# need one or the other depending on whether we're in the "smoke" or "nonsmoke" side of the testing.  If you're writing
# a makefile for a ROSE *project*, you might have a single makefile with both smoke and non-smoke tests, in which case
# you'd probably have $(SMOKE_TEST_TARGETS) and $(NONSMOKE_TEST_TARGETS), and define both check-smoke-local and
# check-nonsmoke-local rules.

check-nonsmoke-local: $(TEST_TARGETS)

# We also need targets that define what "make check" and "make check-all" do.  These will be the same in every
# makefile but we have to list them each time because some old testing makefiles and some projects do special things
# for these targets.
#
# TEMPORARY: As a temporary workaround for the fact that Jenkins and the matrix testing only run "make check", the
# check-local should run all tests, not just smoke tests. Eventaully this line should read:
#        check-local: check-smoke-local

check-local: check-smoke-local check-nonsmoke-local
check-all-local: check-smoke-local check-nonsmoke-local

# Rule for cleaning up. In particular, don't use wildcards here! Some developers, especially those doing binary analysis,
# will have various object files, executables, libraries, core dumps, etc. that they might be working with and having
# "make clean" delete things it didn't create is just impolite. Sometimes automake/autoconf generate indiscriminate
# rules like this and there's nothing you can do about it.
#
# We could have done this all in a single "rm" command, but there are some makefiles where these lists are so long
# that the commands exceed some OS-specific limit.  For really, really long lists you might even need to use make's
# "foreach" function to split them further.

clean-local:
	rm -f $(MOSTLYCLEANFILES)
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
	rm -f $(TEST_TARGETS:.passed=.out)
	rm -f $(TEST_TARGETS:.passed=.err)
