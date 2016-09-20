include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
TEST_TARGETS =
EXTRA_TEST_TARGETS =
MOSTLYCLEANFILES =

TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status
EXTRA_DIST = README.txt

AM_CPPFLAGS = $(ROSE_INCLUDES)

#------------------------------------------------------------------------------------------------------------------------
# It makes no sense to install these since some (at least parallelMerge) have hard-coded paths to other executables.
noinst_PROGRAMS  = astFileIO astFileRead astCompressionTest parallelMerge

astFileIO_SOURCES = astFileIO.C 
astFileIO_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

astCompressionTest_SOURCES = astCompressionTest.C 
astCompressionTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

astFileRead_SOURCES = astFileRead.C
astFileRead_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

parallelMerge_SOURCES = parallelMerge.C
parallelMerge_CPPFLAGS = -DTEST_AST_FILE_READ='"$(abspath $(top_builddir)/tests/testAstFileRead)"' $(ROSE_INCLUDES)
parallelMerge_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

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
Cxx_directory = $(abspath $(top_srcdir)/tests/CompileTests/Cxx_tests)
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

parallelMerge_short.passed: parallelMerge $(parallelMerge_short_binaries)
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

parallelMerge_medium.passed: parallelMerge $(parallelMerge_medium_binaries)
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
# This section of the original makefile is a true mess and has been entirely commented out.  All of these targets are
# either handled above, or were not executed by any of the standard automake stuff like "make", "make check", "make
# install", etc.

##  TEST_TINY_LIST = input_tiny_01.C input_tiny_02.C
##  TEST_SHORT_LIST_BINARIES = $(TEST_SHORT_LIST:.C=.o)
##  
##  ORIGINAL_TEST_MEDIUM_LIST = test2003_01.C test2003_03.C test2003_05.C test2003_08.C test2003_10.C test2003_12.C test2003_14.C \
##     test2003_16.C test2003_18.C test2003_20.C test2003_23.C test2003_25.C test2003_27.C test2003_29.C test2003_31.C \
##     test2003_35.C test2003_37.C test2003_02.C test2003_04.C test2003_06.C test2003_09.C test2003_11.C test2003_13.C \
##     test2003_15.C test2003_17.C test2003_19.C test2003_21.C test2003_24.C test2003_26.C test2003_28.C test2003_30.C \
##     test2003_32.C test2003_36.C test2003_38.C
##  
##  # DQ (6/23/2010): Debug using a smaller set of files.
##  # TEST_MEDIUM_LIST = test2003_01.C test2003_03.C test2003_05.C test2003_08.C test2003_10.C test2003_12.C test2003_14.C test2003_35.C test2003_37.C 
##  # TEST_MEDIUM_LIST = test2003_01.C test2003_37.C 
##  # TEST_MEDIUM_LIST = test2003_15.C test2003_17.C test2003_19.C test2003_21.C
##  # TEST_MEDIUM_LIST = test2003_19.C test2003_21.C
##  # TEST_MEDIUM_LIST = test2003_19.C test2003_21.C
##  # TEST_MEDIUM_LIST = test2003_21.C
##  # TEST_MEDIUM_LIST = test2003_01.C test2003_03.C test2003_05.C test2003_08.C test2003_10.C test2003_12.C test2003_14.C test2003_35.C test2003_37.C 
##  # TEST_MEDIUM_LIST = $(ORIGINAL_TEST_MEDIUM_LIST)
##  # TEST_MEDIUM_LIST = test2003_02.C test2004_52.C
##  
##  # 72 files appear to merge and write out an AST, but 73 files always fail!
##  # TV (06/19/2013): removed file which does not pass the regression...
##  # TEST_MEDIUM_LIST_EXTRA = \
##  # test2004_01.C   test2004_106.C  test2004_11.C   test2004_134.C  test2004_148.C  test2004_30.C  test2004_47.C test2004_63.C  test2004_79.C  test2004_95.C \
##  # test2004_02.C   test2004_107.C  test2004_120.C  test2004_162.C  test2004_31.C  test2004_48.C  test2004_64.C  test2004_80.C  test2004_96.C \
##  # test2004_03.C   test2004_108.C  test2004_121.C  test2004_14.C   test2004_163.C  test2004_32.C  test2004_49.C  test2004_65.C  test2004_81.C \
##  # test2004_04.C   test2004_109.C  test2004_122.C  test2004_137.C  test2004_150.C  test2004_33.C  test2004_67.C  test2004_82.C  test2004_98.C test2004_164.C
##  TEST_MEDIUM_LIST_EXTRA = \
##  test2004_01.C   test2004_106.C  test2004_11.C   test2004_134.C  test2004_148.C  test2004_30.C  test2004_47.C test2004_63.C  test2004_79.C  test2004_95.C \
##  test2004_02.C   test2004_107.C                  test2004_162.C  test2004_31.C  test2004_48.C  test2004_64.C  test2004_80.C  test2004_96.C \
##  test2004_03.C   test2004_108.C  test2004_121.C  test2004_14.C   test2004_163.C  test2004_32.C  test2004_49.C  test2004_65.C  test2004_81.C \
##  test2004_04.C   test2004_109.C  test2004_122.C  test2004_137.C  test2004_150.C  test2004_33.C  test2004_67.C                 test2004_98.C test2004_164.C
##  
##  # Problem files:
##  # test2004_161.C 
##  # test2004_136.C
##  
##  # DQ (9/24/2011): These files are a problem for the new original expression tree support (replacing constant values with the original expression trees).
##  # test2004_97.C
##  
##  TEST_MEDIUM_PROBLEM_FILE_LIST = test2004_136.C test2004_161.C
##  
##  # /home/dquinlan/ROSE/git-dq-main-rc/src/ROSETTA/Grammar/grammarAST_FileIoSource.code:363: static long unsigned int AST_FILE_IO::getGlobalIndexFromSgClassPointer(SgNode*): Assertion `globalIndex < AST_FILE_IO::getAccumulatedPoolSizeOfNewAst(pointer->variantT()+1)' failed.
##  # test2004_50.C 
##  
##  # One of these (large) files is a problem
##  # test2004_135.C  test2004_149.C  
##  
##  # ALT2_TEST_MEDIUM_LIST_EXTRA = test2004_06.C   test2004_110.C  test2004_124.C  test2004_139.C  test2004_152.C  test2004_17.C   test2004_35.C  test2004_52.C \
##  # test2004_04.C   test2004_109.C  test2004_122.C  test2004_137.C  test2004_150.C  test2004_164.C  test2004_33.C  test2004_50.C  test2004_67.C  test2004_82.C  test2004_98.C \
##  # test2004_69.C   test2004_84.C test2004_07.C   test2004_111.C  test2004_125.C  test2004_13.C test2004_153.C  test2004_18.C   test2004_36.C  test2004_53.C  test2004_70.C  \
##  # test2004_05.C   test2004_10.C   test2004_123.C  test2004_138.C  test2004_151.C  test2004_16.C   test2004_34.C  test2004_51.C  test2004_68.C  test2004_83.C  test2004_99.C \
##  # test2004_08.C   test2004_112.C  test2004_140.C  test2004_154.C test2004_19.C   test2004_37.C  test2004_54.C  test2004_71.C  test2004_86.C \
##  # test2004_09.C   test2004_113.C  test2004_128.C  test2004_141.C  test2004_155.C  test2004_23.C   test2004_38.C  test2004_55.C  test2004_72.C  test2004_87.C 
##  ALT2_TEST_MEDIUM_LIST_EXTRA = test2004_06.C   test2004_110.C  test2004_124.C  test2004_139.C  test2004_152.C  test2004_17.C   test2004_35.C  test2004_52.C \
##  test2004_04.C   test2004_109.C  test2004_122.C  test2004_137.C  test2004_150.C  test2004_164.C  test2004_33.C  test2004_50.C  test2004_67.C                 test2004_98.C \
##  test2004_69.C   test2004_84.C test2004_07.C   test2004_111.C  test2004_125.C  test2004_13.C test2004_153.C  test2004_18.C   test2004_36.C  test2004_53.C  test2004_70.C  \
##  test2004_05.C   test2004_10.C   test2004_123.C  test2004_138.C  test2004_151.C  test2004_16.C   test2004_34.C  test2004_51.C  test2004_68.C  test2004_83.C  test2004_99.C \
##  test2004_08.C   test2004_112.C  test2004_140.C  test2004_154.C test2004_19.C   test2004_37.C  test2004_54.C  test2004_71.C  test2004_86.C \
##  test2004_09.C   test2004_113.C  test2004_128.C  test2004_141.C  test2004_155.C  test2004_23.C   test2004_38.C  test2004_55.C  test2004_72.C  test2004_87.C
##  
##  # These files are a problem for the "medium" size test.
##  # test2004_85.C 
##  # test2004_127.C  
##  # TV (06/19/2013) : Problem EDG 4.7
##  # TEST_MEDIUM_PROBLEM_FILE_LIST += test2004_85.C test2004_127.C
##  
##  # TV (06/19/2013) : Problem EDG 4.7
##  # ALT3_TEST_MEDIUM_LIST_EXTRA = \
##  # test2004_100.C  test2004_114.C  test2004_129.C  test2004_142.C  test2004_156.C  test2004_24.C   test2004_39.C  test2004_56.C  test2004_73.C  test2004_88.C \
##  # test2004_40.C  test2004_58.C  test2004_74.C  test2004_89.C \
##  # test2004_102.C  test2004_116.C  test2004_144.C  test2004_158.C  test2004_26.C   test2004_42.C  test2004_59.C  test2004_90.C 
##  ALT3_TEST_MEDIUM_LIST_EXTRA = \
##  test2004_100.C  test2004_114.C                  test2004_142.C  test2004_156.C  test2004_24.C   test2004_39.C  test2004_56.C  test2004_73.C  test2004_88.C \
##  test2004_40.C  test2004_58.C  test2004_74.C  test2004_89.C \
##  test2004_102.C  test2004_116.C  test2004_144.C  test2004_158.C  test2004_26.C   test2004_42.C  test2004_59.C  test2004_90.C
##  
##  # Problem files:
##  # test2004_101.C  
##  # test2004_115.C  
##  # test2004_12.C   
##  # test2004_143.C 
##  # test2004_157.C
##  # test2004_130.C  
##  # test2004_75.C 
##  # test2004_103.C  
##  # test2004_117.C  
##  # test2004_145.C 
##  # test2004_159.C 
##  # test2004_27.C  
##  # test2004_43.C  
##  # test2004_60.C 
##  
##  # TV (06/19/2013) : Problem EDG 4.7
##  # TEST_MEDIUM_PROBLEM_FILE_LIST += test2004_101.C test2004_115.C test2004_12.C test2004_143.C test2004_157.C test2004_130.C test2004_75.C test2004_103.C test2004_117.C test2004_145.C test2004_159.C test2004_27.C test2004_43.C test2004_60.C  
##  TEST_MEDIUM_PROBLEM_FILE_LIST += test2004_101.C test2004_115.C test2004_12.C test2004_143.C test2004_157.C                test2004_75.C test2004_103.C test2004_117.C test2004_145.C test2004_159.C test2004_27.C test2004_43.C test2004_60.C  
##  
##  ALT4_TEST_MEDIUM_LIST_EXTRA = \
##  test2004_131.C  test2004_76.C test2004_91.C \
##  test2004_104.C  test2004_118.C  test2004_132.C  test2004_146.C  test2004_15.C  test2004_28.C   test2004_44.C  test2004_61.C  test2004_77.C
##  
##  # Problem files:
##  # test2004_93.C 
##  # test2004_119.C 
##  # test2004_133.C  
##  # test2004_147.C 
##  # test2004_160.C 
##  # test2004_29.C 
##  # test2004_45.C 
##  # test2004_62.C 
##  # test2004_78.C
##  # test2004_94.C
##  
##  # src/frontend/SageIII/sageInterface/generateUniqueName.C:494: std::string SageInterface::generateUniqueName(SgNode*, bool): Assertion `key.empty() == false' failed.
##  # test2004_105.C 
##  
##  TEST_MEDIUM_PROBLEM_FILE_LIST += test2004_93.C test2004_119.C test2004_133.C test2004_147.C test2004_160.C test2004_29.C test2004_45.C test2004_62.C test2004_78.C test2004_94.C
##  
##  # DQ (7/10/2010): Deal with this later!
##  # test2004_105.C
##  
##  
##  # TV (06/19/2013): removed file which does not pass the regression...
##  # ALT_TEST_MEDIUM_LIST_EXTRA = \
##  # test2004_01.C   test2004_106.C  test2004_11.C   test2004_134.C  test2004_148.C  test2004_161.C  test2004_30.C  test2004_47.C  test2004_63.C  test2004_79.C  test2004_95.C \
##  # test2004_02.C   test2004_107.C  test2004_120.C  test2004_135.C  test2004_149.C  test2004_162.C  test2004_31.C  test2004_48.C  test2004_64.C  test2004_80.C  test2004_96.C \
##  # test2004_03.C   test2004_108.C  test2004_121.C  test2004_136.C  test2004_14.C   test2004_163.C  test2004_32.C  test2004_49.C  test2004_65.C  test2004_81.C   \
##  # test2004_04.C   test2004_109.C  test2004_122.C  test2004_137.C  test2004_150.C  test2004_164.C  test2004_33.C  test2004_50.C  test2004_67.C  test2004_82.C  test2004_98.C \
##  # test2004_05.C   test2004_10.C   test2004_123.C  test2004_138.C  test2004_151.C  test2004_16.C   test2004_34.C  test2004_51.C  test2004_68.C  test2004_83.C  test2004_99.C \
##  # test2004_06.C   test2004_110.C  test2004_124.C  test2004_139.C  test2004_152.C  test2004_17.C   test2004_35.C  test2004_52.C  test2004_69.C  test2004_84.C \
##  # test2004_07.C   test2004_111.C  test2004_125.C  test2004_13.C   test2004_153.C  test2004_18.C   test2004_36.C  test2004_53.C  test2004_70.C  test2004_85.C \
##  # test2004_08.C   test2004_112.C  test2004_127.C  test2004_140.C  test2004_154.C  test2004_19.C   test2004_37.C  test2004_54.C  test2004_71.C  test2004_86.C \
##  # test2004_09.C   test2004_113.C  test2004_128.C  test2004_141.C  test2004_155.C  test2004_23.C   test2004_38.C  test2004_55.C  test2004_72.C  test2004_87.C \
##  # test2004_100.C  test2004_114.C  test2004_129.C  test2004_142.C  test2004_156.C  test2004_24.C   test2004_39.C  test2004_56.C  test2004_73.C  test2004_88.C \
##  # test2004_101.C  test2004_115.C  test2004_12.C   test2004_143.C  test2004_157.C  test2004_40.C  test2004_58.C  test2004_74.C  test2004_89.C \
##  # test2004_102.C  test2004_116.C  test2004_130.C  test2004_144.C  test2004_158.C  test2004_26.C   test2004_42.C  test2004_59.C  test2004_75.C  test2004_90.C \
##  # test2004_103.C  test2004_117.C  test2004_131.C  test2004_145.C  test2004_159.C  test2004_27.C   test2004_43.C  test2004_60.C  test2004_76.C  test2004_91.C \
##  # test2004_104.C  test2004_118.C  test2004_132.C  test2004_146.C  test2004_15.C   test2004_28.C   test2004_44.C  test2004_61.C  test2004_77.C  test2004_93.C \
##  # test2004_105.C  test2004_119.C  test2004_133.C  test2004_147.C  test2004_160.C  test2004_29.C   test2004_45.C  test2004_62.C  test2004_78.C  test2004_94.C
##  ALT_TEST_MEDIUM_LIST_EXTRA = \
##  test2004_01.C   test2004_106.C  test2004_11.C   test2004_134.C  test2004_148.C  test2004_161.C  test2004_30.C  test2004_47.C  test2004_63.C  test2004_79.C  test2004_95.C \
##  test2004_02.C   test2004_107.C                  test2004_135.C  test2004_149.C  test2004_162.C  test2004_31.C  test2004_48.C  test2004_64.C  test2004_80.C  test2004_96.C \
##  test2004_03.C   test2004_108.C  test2004_121.C  test2004_136.C  test2004_14.C   test2004_163.C  test2004_32.C  test2004_49.C  test2004_65.C  test2004_81.C   \
##  test2004_04.C   test2004_109.C  test2004_122.C  test2004_137.C  test2004_150.C  test2004_164.C  test2004_33.C  test2004_50.C  test2004_67.C                 test2004_98.C \
##  test2004_05.C   test2004_10.C   test2004_123.C  test2004_138.C  test2004_151.C  test2004_16.C   test2004_34.C  test2004_51.C  test2004_68.C  test2004_83.C  test2004_99.C \
##  test2004_06.C   test2004_110.C  test2004_124.C  test2004_139.C  test2004_152.C  test2004_17.C   test2004_35.C  test2004_52.C  test2004_69.C  test2004_84.C \
##  test2004_07.C   test2004_111.C  test2004_125.C  test2004_13.C   test2004_153.C  test2004_18.C   test2004_36.C  test2004_53.C  test2004_70.C                \
##  test2004_08.C   test2004_112.C                  test2004_140.C  test2004_154.C  test2004_19.C   test2004_37.C  test2004_54.C  test2004_71.C  test2004_86.C \
##  test2004_09.C   test2004_113.C  test2004_128.C  test2004_141.C  test2004_155.C  test2004_23.C   test2004_38.C  test2004_55.C  test2004_72.C  test2004_87.C \
##  test2004_100.C  test2004_114.C                  test2004_142.C  test2004_156.C  test2004_24.C   test2004_39.C  test2004_56.C  test2004_73.C  test2004_88.C \
##  test2004_101.C  test2004_115.C  test2004_12.C   test2004_143.C  test2004_157.C  test2004_40.C  test2004_58.C  test2004_74.C  test2004_89.C \
##  test2004_102.C  test2004_116.C                  test2004_144.C  test2004_158.C  test2004_26.C   test2004_42.C  test2004_59.C  test2004_75.C  test2004_90.C \
##  test2004_103.C  test2004_117.C  test2004_131.C  test2004_145.C  test2004_159.C  test2004_27.C   test2004_43.C  test2004_60.C  test2004_76.C  test2004_91.C \
##  test2004_104.C  test2004_118.C  test2004_132.C  test2004_146.C  test2004_15.C   test2004_28.C   test2004_44.C  test2004_61.C  test2004_77.C  test2004_93.C \
##  test2004_105.C  test2004_119.C  test2004_133.C  test2004_147.C  test2004_160.C  test2004_29.C   test2004_45.C  test2004_62.C  test2004_78.C  test2004_94.C
##  
##  # DQ (9/24/2011): These files are a problem for the new original expression tree support (replacing constant values with the original expression trees).
##  # test2004_97.C
##  
##  
##  # This make this a much larger test.
##  # TEST_MEDIUM_LIST += $(TEST_MEDIUM_LIST_EXTRA) $(ALT2_TEST_MEDIUM_LIST_EXTRA)
##  # TEST_MEDIUM_LIST += $(TEST_MEDIUM_LIST_EXTRA)
##  # TEST_MEDIUM_LIST += $(TEST_MEDIUM_LIST_EXTRA) $(ALT2_TEST_MEDIUM_LIST_EXTRA)
##  # TEST_MEDIUM_LIST = $(ALT3_TEST_MEDIUM_LIST_EXTRA)
##  # TEST_MEDIUM_LIST = $(ALT4_TEST_MEDIUM_LIST_EXTRA)
##  # TEST_MEDIUM_LIST += $(ALT4_TEST_MEDIUM_LIST_EXTRA)
##  # TEST_MEDIUM_LIST += $(TEST_MEDIUM_LIST_EXTRA) $(ALT2_TEST_MEDIUM_LIST_EXTRA)
##  TEST_MEDIUM_LIST = $(ALT_TEST_MEDIUM_LIST_EXTRA)
##  
##  # Error (only works as seperate sets)
##  # TEST_MEDIUM_LIST += $(TEST_MEDIUM_LIST_EXTRA) $(ALT2_TEST_MEDIUM_LIST_EXTRA) $(ALT4_TEST_MEDIUM_LIST_EXTRA)
##  
##  # Error these will not merge (though they merge separately).
##  # TEST_MEDIUM_LIST += $(ALT3_TEST_MEDIUM_LIST_EXTRA)
##  
##  # Error these will not merge (though they merge separately).
##  # TEST_MEDIUM_LIST = $(ALT3_TEST_MEDIUM_LIST_EXTRA) $(ALT4_TEST_MEDIUM_LIST_EXTRA)
##  
##  # Error these will not merge (though they merge separately).
##  # TEST_MEDIUM_LIST = $(ALT3_TEST_MEDIUM_LIST_EXTRA) test2004_12.C
##  
##  # Changing hte order causes an error 12,75,27 works but 12,27,75 fails
##  # TEST_MEDIUM_LIST = test2004_12.C test2004_75.C test2004_27.C 
##  # TEST_MEDIUM_LIST = test2004_12.C test2004_27.C test2004_75.C 
##  
##  
##  
##  
##  TEST_MEDIUM_LIST_BINARIES = $(TEST_MEDIUM_LIST:.C=.o)
##  
##  # FAILING_TEST_MEDIUM_LIST = $(TEST_MEDIUM_LIST) $(TEST_MEDIUM_PROBLEM_FILE_LIST)
##  # FAILING_TEST_MEDIUM_LIST = $(TEST_MEDIUM_PROBLEM_FILE_LIST)
##  FAILING_TEST_MEDIUM_LIST = test2004_105.C
##  FAILING_TEST_MEDIUM_LIST_BINARIES = $(FAILING_TEST_MEDIUM_LIST:.C=.o)
##  
##  failing-test-read-medium: $(FAILING_TEST_MEDIUM_LIST_BINARIES) ../../testAstFileRead
##  	$(READ_TRANSLATOR) $(FAILING_TEST_MEDIUM_LIST) temp_output_medium.C
##  
##  
##  TEST_LARGE_LIST_BINARIES = $(TESTCODES:.C=.o)
##  TEST_Binaries = $(TESTCODES:.C=.C.binary)
##  # test-read-large: test2001_01.C.binary test2001_02.C.binary test2001_03.C.binary
##  
##  
##  FailingTests = test2004_106.C test2004_01.C test2003_38.C test2003_36.C test2003_32.C test2003_28.C test2003_24.C test2003_21.C test2003_19.C test2003_17.C test2003_13.C
##  FailingTests_Binaries = $(FailingTests:.C=.o)
##  failingTest: $(FailingTests_Binaries)
##  #	../../../tests/testAstFileRead test2004_106.C test2004_01.C test2003_38.C test2003_36.C test2003_32.C test2003_28.C test2003_24.C test2003_21.C test2003_19.C test2003_17.C test2003_13.C temp_output.C
##  	../../../tests/testAstFileRead test2004_106.C test2004_01.C test2003_38.C test2003_36.C test2003_32.C test2003_28.C test2003_24.C
##  
##  # test2003_21.C 
##  # test2003_19.C 
##  # test2003_17.C test2003_13.C temp_output.C
##  
##  
##  # TEST_PARALLEL_MEDIUM_LIST = $(ORIGINAL_TEST_MEDIUM_LIST)
##  # TEST_PARALLEL_MEDIUM_LIST = test2003_01.C test2003_03.C test2003_05.C test2003_08.C test2003_10.C test2003_12.C test2003_14.C 
##  
##  # This longer list fails when run in parallel, need to debug this.
##  # TEST_PARALLEL_MEDIUM_LIST = $(TEST_MEDIUM_LIST)
##  
##  # Problem test for parallel tests:
##  # test2003_08.C 
##  # test2003_16.C 
##  # test2003_29.C
##  # test2003_31.C 
##  # test2003_37.C 
##  # test2003_04.C
##  # test2003_13.C 
##  # test2003_19.C 
##  # test2003_38.C
##  
##  
##  TEST_PARALLEL_LARGE_LIST = $(TESTCODES)
##  TEST_PARALLEL_LARGE_LIST_BINARIES = $(TEST_PARALLEL_LARGE_LIST:.C=.o)
##  testParallelMerge-large: parallelMerge $(TEST_PARALLEL_LARGE_LIST_BINARIES)
##  	rm -rf data temp
##  	mkdir data
##  	mkdir temp
##  	./parallelMerge $(TEST_PARALLEL_LARGE_LIST) temp_output_parallel_large.C
##  
##  # DQ (3/1/2010): This is the sart of a general command to process all the
##  # binaries in a directory structure.
##  ls_generatedBinaries:
##  #	ls -l `find $(top_builddir)/src -name "*.C.binary" -print`
##  #	cd $(top_builddir)/src; ls -l `find . -name "*.C.binary" -print`
##  #	cd $(top_builddir); du -a * | grep "C.binary"
##  	find $(top_builddir) -name "*.C.binary" -print
##  
##  ls_binaries:
##  	ls $(TEST_Binaries)
##  
##  # TEST_Binaries_short = test2003_*.binary
##  ls_binaries_short:
##  	ls -l $(TEST_Binaries_shortTrunk)
##  #	ls -l $(TEST_Binaries_short:.binary=)
##  
##  
##  # This tests the compression of the AST internally, a step that makes it contiguious in 
##  # memory by copying it to intermediate storage and then rebuilding it from scratch.
##  # Putting the AST into blocks of contiguious storage is what makes the binary file I/O mechanism fast.
##  # This is a great test of all but the the binary file I/O part (which is the simpler part).
##  testCompression: astCompressionTest
##  	./astCompressionTest -c $(srcdir)/test2001_01.C
##  
##  largeFileTest: astFileIO
##  	./astFileIO -rose:verbose 0 -c $(srcdir)/test2005_36.C -o test2005_36
##  
##  largestFileTest: astFileIO
##  	./astFileIO -rose:verbose 0 -I$(top_builddir) $(ROSE_INCLUDES) -c $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C -o Cxx_Grammar
##  
##  largestRoseFileTest: astFileIO ../../testAstFileIO
##  #	./astFileIO -rose:verbose 0 -I$(top_builddir) $(ROSE_INCLUDES) -c $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C -o Cxx_Grammar
##  	$(TEST_TRANSLATOR) -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/testRose_testFile.C
##  
##  locForRoseFile:
##  	g++ -E -I$(top_builddir) $(ROSE_INCLUDES) -c $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C | wc -l
##  
##  
##  testFileGeneration:
##  	$(MAKE) $(TEST_Objects)
##  test1: test1A.C test1B.C
##  	@echo "Compiling test code using $(TEST_TRANSLATOR) ..."
##  #	DQ(7/13/2004): This is an error since it specifies the output file twice in the vendor compile line
##  #	$(TEST_TRANSLATOR) -rose:verbose 0 -o test1A.o -c $(srcdir)/test1A.C 
##  	$(TEST_TRANSLATOR) -rose:verbose 0 -c $(srcdir)/test1A.C 
##  	$(TEST_TRANSLATOR) -rose:verbose 0 -c $(srcdir)/test1B.C
##  	g++ -o test1 rose_test1A.o rose_test1B.o
##  	@echo "Running resulting executable ..."
##  	./test1
##  
##  # DQ (2/5/2009): This is a demonstration that the AST generated 
##  # from a binary executable fails when being written to disk.
##  # Ideas about how to simplify this problem:
##  #  1) Skip the dwarf information to make the problem smaller
##  #
##  # Example from Thomas.
##  test-binary: astFileIO
##  	./astFileIO -rose:verbose 0 $(srcdir)/buffer2.bin



#------------------------------------------------------------------------------------------------------------------------
# automake boilerplate

check-local: $(Cxx_binaries) $(local_binaries) $(TEST_TARGETS)

clean-local:
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
	rm -f $(EXTRA_TEST_TARGETS)
	rm -f $(EXTRA_TEST_TARGETS:.passed=.failed)
	rm -f $(EXTRA_TEST_TARGETS:.passed=)

