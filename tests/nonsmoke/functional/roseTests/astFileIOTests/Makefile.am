include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
TEST_TARGETS =
EXTRA_TEST_TARGETS =
MOSTLYCLEANFILES =

TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status
EXTRA_DIST = README.txt

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)

#------------------------------------------------------------------------------------------------------------------------
# It makes no sense to install these since some (at least parallelMerge) have hard-coded paths to other executables.
noinst_PROGRAMS  = astFileIO astFileRead astCompressionTest parallelMerge

astFileIO_SOURCES = astFileIO.C 
astFileIO_LDADD = $(ROSE_SEPARATE_LIBS)

astCompressionTest_SOURCES = astCompressionTest.C 
astCompressionTest_LDADD = $(ROSE_SEPARATE_LIBS)

astFileRead_SOURCES = astFileRead.C
astFileRead_LDADD = $(ROSE_SEPARATE_LIBS)

parallelMerge_SOURCES = parallelMerge.C
parallelMerge_CPPFLAGS = -DTEST_AST_FILE_READ='"$(abspath $(top_builddir)/tests/nonsmoke/functional/testAstFileRead)"' $(ROSE_INCLUDES)
parallelMerge_LDADD = $(ROSE_SEPARATE_LIBS)

#------------------------------------------------------------------------------------------------------------------------
# This makefile uses ../../testAstFileIO and ../../testAstFileRead, and must therefore make sure they're built.

../../testAstFileIO:
	$(MAKE) -C ../.. testAstFileIO

../../testAstFileRead:
	$(MAKE) -C ../.. testAstFileRead

#------------------------------------------------------------------------------------------------------------------------
# Creates *.binary files which are used as inputs for other tests.  The creation of the *.binary file is itself a test.

EXTRA_DIST += testAstFileIO.conf

include $(srcdir)/Makefile-pass-IO.inc
Cxx_specimens = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS)
Cxx_directory = $(abspath $(top_srcdir)/tests/nonsmoke/functional/CompileTests/Cxx_tests)
Cxx_binaries = $(addsuffix .binary, $(Cxx_specimens))
$(Cxx_binaries): %.binary: $(Cxx_directory)/% ../../testAstFileIO
	@$(RTH_RUN) \
		TITLE="../../testAstFileIO $(notdir $<) [$@]" \
		FLAGS="$(ROSE_FLAGS) -I$(Cxx_directory)" \
		INPUT=$(abspath $<) \
		OUTPUT=$@ \
		$(srcdir)/testAstFileIO.conf $@.passed
EXTRA_TEST_TARGETS += $(addsuffix .passed, $(Cxx_binaries))

local_specimens = input_tiny_01a.C input_tiny_01b.C input_tiny_02a.C input_tiny_02b.C input_tiny_03a.C input_tiny_03b.C
local_binaries = $(addsuffix .binary, $(local_specimens))
EXTRA_DIST += $(local_specimens)
$(local_binaries): %.binary: % ../../testAstFileIO
	@$(RTH_RUN) \
		TITLE="../../testAstFileIO $(notdir $<) [$@]" \
		FLAGS="$(ROSE_FLAGS) -I." \
		INPUT=$(abspath $<) \
		OUTPUT=$@ \
		$(srcdir)/testAstFileIO.conf $@.passed
EXTRA_TEST_TARGETS += $(addsuffix .passed, $(local_binaries))

# use this target if you just want to check creating all the binaries
check_testAstFileIO: $(Cxx_binaries) $(local_binaries)

#------------------------------------------------------------------------------------------------------------------------
# Test ../../testAstFileRead on a coupld of specific inputs.  The testAstFileRead has the annoying feature that when
# you tell it to read "foo" it actually tries to read "foo.binary", so we have to jump through some hoops in order to
# pass the correct file names.  We also run the test in a temporary subdirectory since it drops other files that are
# hard to accurately clean up in the makefile (we want to avoid wildcards since that wipes out important files that the
# developer might have sitting around).
TEST_TARGETS += test_read.passed
test_read_specimens = test2001_01.C test2001_02.C
test_read_directory = $(Cxx_directory)
test_read_binaries = $(addsuffix .binary, $(test_read_specimens))
test_read.passed: ../../testAstFileRead $(test_read_binaries)
	@$(RTH_RUN) \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/../../testAstFileRead $(addprefix $$(pwd)/, $(test_read_specimens)) output.C" \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# Some tests of ../../testAstFileIO on particular *.binary files generated from files in this source directory
# Same difficulties as for the test_read.passed target.

TEST_TARGETS += test_read_tiny_01.passed
test_read_tiny_01_specimens = input_tiny_01a.C input_tiny_01b.C
test_read_tiny_01_binaries = $(addsuffix .binary, $(test_read_tiny_01_specimens))
EXTRA_DIST += $(test_read_tiny_01_specimens)
test_read_tiny_01.passed: ../../testAstFileRead $(test_read_tiny_01_binaries)
	@$(RTH_RUN) \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/../../testAstFileRead $(addprefix $$(pwd)/, $(test_read_tiny_01_specimens)) output.C" \
		$(TEST_EXIT_STATUS) $@

TEST_TARGETS += test_read_tiny_02.passed
test_read_tiny_02_specimens = input_tiny_02a.C input_tiny_02b.C
test_read_tiny_02_binaries = $(addsuffix .binary, $(test_read_tiny_02_specimens))
EXTRA_DIST += $(test_read_tiny_02_specimens)
test_read_tiny_02.passed: ../../testAstFileRead $(test_read_tiny_02_binaries)
	@$(RTH_RUN) \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/../../testAstFileRead $(addprefix $$(pwd)/, $(test_read_tiny_02_specimens)) output.C " \
		$(TEST_EXIT_STATUS) $@

TEST_TARGETS += test_read_tiny_03.passed
test_read_tiny_03_specimens = input_tiny_03a.C input_tiny_03b.C
test_read_tiny_03_binaries = $(addsuffix .binary, $(test_read_tiny_03_specimens))
EXTRA_DIST += $(test_read_tiny_03_specimens)
test_read_tiny_03.passed: ../../testAstFileRead $(test_read_tiny_03_binaries)
	@$(RTH_RUN) \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/../../testAstFileRead $(addprefix $$(pwd)/, $(test_read_tiny_03_specimens)) output.C" \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# Tests ../../testAstFileRead on a short list of inputs. Same difficulties as for test_read.passed

TEST_TARGETS += test_read_short.passed
test_read_short_directory = $(Cxx_directory)
test_read_short_specimens = test2003_01.C test2003_03.C test2003_05.C test2003_08.C test2003_10.C test2003_12.C test2003_14.C
test_read_short_binaries = $(addsuffix .binary, $(test_read_short_specimens))

test_read_short.passed: ../../testAstFileRead $(test_read_short_binaries)
	@$(RTH_RUN) \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/../../testAstFileRead $(addprefix $$(pwd)/, $(test_read_short_specimens)) output.C" \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# Tests parallelMerge on a short list of inputs from the Cxx_tests directory.
# The parallelMerge executable takes "foo" as an argument, but actually reads "foo.binary"; hence we need to jump through
# some hoops: parallelMerge_short_arguments are the absolute names of the foo.binary files, but without the ".binary" part.

parallelMerge_short_specimens = test2003_01.C test2003_03.C test2003_05.C test2003_10.C test2003_12.C test2003_14.C
parallelMerge_short_directory = $(Cxx_directory)
parallelMerge_short_binaries = $(addsuffix .binary, $(parallelMerge_short_specimens))
parallelMerge_short_arguments = $(addprefix $$(pwd)/, $(parallelMerge_short_specimens))

# Liao 2/9/2011. boost thread_group may have bug on Mac OS X 10.6
if !OS_MACOSX	
TEST_TARGETS += parallelMerge_short.passed
endif

parallelMerge_short.passed: ../../testAstFileRead parallelMerge $(parallelMerge_short_binaries)
	@$(RTH_RUN) \
		USE_SUBDIR=yes \
		CMD="mkdir data temp; $$(pwd)/parallelMerge $(parallelMerge_short_arguments) output.C" \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# Similar to parallelMerge_short but with a longer list of specimens.
# Must run in a subdir to avoid conflicts with other parallelMerge tests running concurrently.

parallelMerge_medium_specimens =											\
	test2003_01.C test2003_03.C test2003_05.C test2003_10.C test2003_12.C test2003_14.C test2003_18.C test2003_20.C	\
	test2003_23.C test2003_25.C test2003_27.C test2003_35.C test2003_02.C test2003_06.C test2003_09.C test2003_11.C	\
	test2003_15.C test2003_17.C test2003_21.C test2003_24.C test2003_26.C test2003_28.C test2003_30.C test2003_32.C	\
	test2003_36.C 
parallelMerge_medium_directory = $(Cxx_directory)
parallelMerge_medium_binaries = $(addsuffix .binary, $(parallelMerge_medium_specimens))
parallelMerge_medium_arguments = $(addprefix $$(pwd)/, $(parallelMerge_medium_specimens))

# Liao 2/9/2011. boost thread_group may have bug on Mac OS X 10.6
if !OS_MACOSX	
TEST_TARGETS += parallelMerge_medium.passed
endif

parallelMerge_medium.passed: ../../testAstFileRead parallelMerge $(parallelMerge_medium_binaries)
	@$(RTH_RUN) \
		USE_SUBDIR=yes \
		CMD="mkdir data temp; $$(pwd)/parallelMerge $(parallelMerge_short_arguments) output.C" \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# This is a test of the same files used with parallelMerge_medium.passed but run in serial. This is different and usually
# faster for small problems than the singled threaded parallel merge.  Same difficulties apply as for test_read_short.passed.

test_read_medium_specimens = $(parallelMerge_medium_specimens)
test_read_medium_directory = $(parallelMerge_medium_directory)
test_read_medium_binaries = $(addsuffix .binary, $(test_read_medium_specimens))
test_read_medium_arguments = $(addprefix $$(pwd)/, $(test_read_medium_specimens))

# The output from the following tests build on each other, each reading the output from the previous test to produce
# a new output.  Therefore, we make the tests depend on the output file rather than the usual .passed file (although
# the .passed and .failed files will still be created.
#
# Beware of the strange arguments for testAstFileRead: it takes "foo" as an argument, but actually reads "foo.binary".
# Same thing applies to the output file name (the last argument).
MOSTLYCLEANFILES += medium_output.C.binary $(addprefix medium_output, .passed .failed)
medium_output.C.binary: ../../testAstFileRead $(test_read_medium_binaries)
	@$(RTH_RUN) \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/../../testAstFileRead $(test_read_medium_arguments) $$(pwd)/$(@:.binary=)" \
		$(TEST_EXIT_STATUS) medium_output.passed

MOSTLYCLEANFILES += large_output.C.binary $(addprefix large_output, .passed .failed)
large_output.C.binary: medium_output.C.binary
	@$(RTH_RUN) \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/../../testAstFileRead $(foreach I, 1 2 3 4, $$(pwd)/$(<:.binary=)) $$(pwd)/$(@:.binary=)" \
		$(TEST_EXIT_STATUS) large_output.passed

MOSTLYCLEANFILES += huge_output.C.binary $(addprefix huge_output, .passed .failed)
huge_output.C.binary: large_output.C.binary
	@$(RTH_RUN) \
		DISABLED="too large for most systems" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/../../testAstFileRead $(foreach I, 1 2 3 4, $$(pwd)/$(<:.binary=)) $$(pwd)/$(@:.binary=)" \
		$(TEST_EXIT_STATUS) huge_output.passed

MOSTLYCLEANFILES += reallyhuge_output.C.binary $(addprefix reallyhuge_output, .passed .failed)
reallyhuge_output.C.binary: huge_output.C.binary
	@$(RTH_RUN) \
		DISABLED="too large for most systems" \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/../../testAstFileRead $(foreach I, 1 2 3 4, $$(pwd)/$(<:.binary=)) $$(pwd)/$(@:.binary=)" \
		$(TEST_EXIT_STATUS) reallyhuge_output.passed

# Liao 2/9/2011. boost thread_group may have bug on Mac OS X 10.6
if !OS_MACOSX	
TEST_TARGETS += medium_output.C.binary
if ROSE_USE_LONG_MAKE_CHECK_RULE
TEST_TARGETS += large_output.C.binary huge_output.C.binary reallyhuge_output.C.binary
endif
endif

#------------------------------------------------------------------------------------------------------------------------
# automake boilerplate

check-local: $(Cxx_binaries) $(local_binaries) $(TEST_TARGETS)
	@echo "*********************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/roseTests/astFileIOTests: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************************"

clean-local:
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
	rm -f $(EXTRA_TEST_TARGETS)
	rm -f $(EXTRA_TEST_TARGETS:.passed=.failed)
	rm -f $(EXTRA_TEST_TARGETS:.passed=)

