include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .

# DQ (8/1/2005): Uncommented to force test code to build, but tests currently fail
# QY 11/9/04 comment out test
# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = astFileIO astFileRead astCompressionTest parallelMerge

astFileIO_SOURCES = astFileIO.C 
astFileIO_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

astCompressionTest_SOURCES = astCompressionTest.C 
astCompressionTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

astFileRead_SOURCES = astFileRead.C
astFileRead_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

parallelMerge_SOURCES = parallelMerge.C
parallelMerge_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

# astFileIO_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

include $(srcdir)/../../CompileTests/Cxx_tests/Makefile-pass.inc

# These currently fail (will be fixed by Alin later when ROSE works better with template functions)
# template_function.C
# template_class.C

# These codes are DESIGNED to fail in the inlining
# fail1.C fail2.C

# DQ (2/13/2010): Optionally make this a longer test.
# TESTCODES += $(EXTRA_TESTCODES)

# DQ (2/20/2010): Use the standard list of test codes (if possible)
TESTCODES = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS)

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

# VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=80 --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
VALGRIND_OPTIONS = --tool=memcheck -v --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/bin/valgrind $(VALGRIND_OPTIONS)
VALGRIND =

# ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0
ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 -rose:skip_unparse_asm_commands

# TEST_TRANSLATOR = $(top_builddir)/tests/roseTests/astFileIOTests/astFileIO $(ROSE_FLAGS)
# READ_TRANSLATOR = $(top_builddir)/tests/roseTests/astFileIOTests/astFileRead $(ROSE_FLAGS)
# TEST_TRANSLATOR = ./astFileIO $(ROSE_FLAGS)
TEST_TRANSLATOR = $(top_builddir)/tests/testAstFileIO $(ROSE_FLAGS)

# DQ (2/23/2010): This is the new AST Reader (merges function type tables, etc.)
# READ_TRANSLATOR = ./astFileRead $(ROSE_FLAGS)
READ_TRANSLATOR = $(VALGRIND) $(top_builddir)/tests/testAstFileRead
# READ_TRANSLATOR = $(top_builddir)/tests/parallelASTMerge

TESTCODE_INCLUDES =

../../testAstFileIO:
	make -C ../.. testAstFileIO
../../testAstFileRead:
	make -C ../.. testAstFileRead
# DQ (7/12/2004): Modified to run with make -j4 options
# $(TEST_Objects): astFileIO astFileRead $(TESTCODES)
$(TEST_Objects): astFileIO astFileRead ../../testAstFileIO
#	@echo "QY:skipping test code using $(TEST_TRANSLATOR) ..."
#	@echo "Compiling test code using $(TEST_TRANSLATOR) ..."
#	$(VALGRIND) $(TEST_TRANSLATOR) $(srcdir)/$(@:.o=.C) -o $(@:.o=)
#	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir)/../../CompileTests/Cxx_tests -c $(srcdir)/../../CompileTests/Cxx_tests/$(@:.o=.C) -o $(@:.o=)
	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir)/../../CompileTests/Cxx_tests -c $(srcdir)/../../CompileTests/Cxx_tests/$(@:.o=.C)
#	@echo "Running resulting executable ..."
#	./$(@:.o=)

# EXTRA_DIST = $(TESTCODES) test2001_05.h
# EXTRA_DIST = input_tiny_01.C input_tiny_02.C
EXTRA_DIST = input_tiny_01a.C  input_tiny_01b.C  input_tiny_02a.C  input_tiny_02b.C  input_tiny_03a.C  input_tiny_03b.C

test2001_01.C.binary: test2001_01.o

test2001_02.C.binary: test2001_02.o

test2001_03.C.binary: test2001_03.o

# This tests the reading of a collection of binary files (build by running $(MAKE) $(TEST_Objects)
# test-read: test2001_01.binary test2001_02.binary test2001_03.binary
test-read: test2001_01.C.binary test2001_02.C.binary test2001_03.C.binary ../../testAstFileRead
#	./astFileRead rose_test2001_01.C rose_test2001_02.C rose_test2001_03.C
#	$(READ_TRANSLATOR) test2001_01.C test2001_02.C test2001_03.C
	$(READ_TRANSLATOR) test2001_01.C test2001_02.C temp_output.C

#TEST_Binaries_short = test2003_*.C.binary
#TEST_Binaries_shortTrunk = $(TEST_Binaries_short:.binary=)
#test-read-medium: $(TEST_Binaries_short)
##	$(READ_TRANSLATOR) $(TEST_Binaries_short:.binary=)
#	$(READ_TRANSLATOR) `$(TEST_Binaries_shortTrunk)`

TEST_TINY_LIST = input_tiny_01.C input_tiny_02.C
# TEST_TINY_LIST_BINARIES = $(TEST_TINY_LIST:.C=.o)

# test-read-tiny: ../../testAstFileIO ../../testAstFileRead
#	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir) -c $(srcdir)/input_tiny_01.C
#	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir) -c $(srcdir)/input_tiny_02.C
#	$(READ_TRANSLATOR) $(TEST_TINY_LIST) temp_output_tiny.C

# test-read-tiny2: ../../testAstFileIO ../../testAstFileRead
#	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir) -c $(srcdir)/input_tiny2_01.C
#	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir) -c $(srcdir)/input_tiny2_02.C
##	$(READ_TRANSLATOR) input_tiny2_01.C input_tiny2_02.C temp_output_tiny2.C
#	$(READ_TRANSLATOR) input_tiny2_01.C input_tiny2_02.C temp_output_tiny2.C 

TEST_SHORT_LIST = test2003_01.C test2003_03.C test2003_05.C test2003_08.C test2003_10.C test2003_12.C test2003_14.C
# TEST_SHORT_LIST_BINARIES = $(TEST_SHORT_LIST:.C=.C.binary)
TEST_SHORT_LIST_BINARIES = $(TEST_SHORT_LIST:.C=.o)

test-read-short: $(TEST_SHORT_LIST_BINARIES) ../../testAstFileRead
	$(READ_TRANSLATOR) $(TEST_SHORT_LIST) temp_output_short.C

# 34
ORIGINAL_TEST_MEDIUM_LIST = test2003_01.C test2003_03.C test2003_05.C test2003_08.C test2003_10.C test2003_12.C test2003_14.C \
   test2003_16.C test2003_18.C test2003_20.C test2003_23.C test2003_25.C test2003_27.C test2003_29.C test2003_31.C \
   test2003_35.C test2003_37.C test2003_02.C test2003_04.C test2003_06.C test2003_09.C test2003_11.C test2003_13.C \
   test2003_15.C test2003_17.C test2003_19.C test2003_21.C test2003_24.C test2003_26.C test2003_28.C test2003_30.C \
   test2003_32.C test2003_36.C test2003_38.C

# DQ (6/23/2010): Debug using a smaller set of files.
# TEST_MEDIUM_LIST = test2003_01.C test2003_03.C test2003_05.C test2003_08.C test2003_10.C test2003_12.C test2003_14.C test2003_35.C test2003_37.C 
# TEST_MEDIUM_LIST = test2003_01.C test2003_37.C 
# TEST_MEDIUM_LIST = test2003_15.C test2003_17.C test2003_19.C test2003_21.C
# TEST_MEDIUM_LIST = test2003_19.C test2003_21.C
# TEST_MEDIUM_LIST = test2003_19.C test2003_21.C
# TEST_MEDIUM_LIST = test2003_21.C
# TEST_MEDIUM_LIST = test2003_01.C test2003_03.C test2003_05.C test2003_08.C test2003_10.C test2003_12.C test2003_14.C test2003_35.C test2003_37.C 
TEST_MEDIUM_LIST = $(ORIGINAL_TEST_MEDIUM_LIST)
# TEST_MEDIUM_LIST = test2003_02.C test2004_52.C

# 72 files appear to merge and write out an AST, but 73 files always fail!
TEST_MEDIUM_LIST_EXTRA = \
test2004_01.C   test2004_106.C  test2004_11.C   test2004_134.C  test2004_148.C  test2004_30.C  test2004_47.C test2004_63.C  test2004_79.C  test2004_95.C \
test2004_02.C   test2004_107.C  test2004_120.C  test2004_162.C  test2004_31.C  test2004_48.C  test2004_64.C  test2004_80.C  test2004_96.C \
test2004_03.C   test2004_108.C  test2004_121.C  test2004_14.C   test2004_163.C  test2004_32.C  test2004_49.C  test2004_65.C  test2004_81.C  test2004_97.C \
test2004_04.C   test2004_109.C  test2004_122.C  test2004_137.C  test2004_150.C  test2004_33.C  test2004_67.C  test2004_82.C  test2004_98.C test2004_164.C

# Problem files:
# test2004_161.C 
# test2004_136.C
TEST_MEDIUM_PROBLEM_FILE_LIST = test2004_136.C test2004_161.C

# /home/dquinlan/ROSE/git-dq-main-rc/src/ROSETTA/Grammar/grammarAST_FileIoSource.code:363: static long unsigned int AST_FILE_IO::getGlobalIndexFromSgClassPointer(SgNode*): Assertion `globalIndex < AST_FILE_IO::getAccumulatedPoolSizeOfNewAst(pointer->variantT()+1)' failed.
# test2004_50.C 

# One of these (large) files is a problem
# test2004_135.C  test2004_149.C  

ALT2_TEST_MEDIUM_LIST_EXTRA = test2004_06.C   test2004_110.C  test2004_124.C  test2004_139.C  test2004_152.C  test2004_17.C   test2004_35.C  test2004_52.C \
test2004_04.C   test2004_109.C  test2004_122.C  test2004_137.C  test2004_150.C  test2004_164.C  test2004_33.C  test2004_50.C  test2004_67.C  test2004_82.C  test2004_98.C \
test2004_69.C   test2004_84.C test2004_07.C   test2004_111.C  test2004_125.C  test2004_13.C test2004_153.C  test2004_18.C   test2004_36.C  test2004_53.C  test2004_70.C  \
test2004_05.C   test2004_10.C   test2004_123.C  test2004_138.C  test2004_151.C  test2004_16.C   test2004_34.C  test2004_51.C  test2004_68.C  test2004_83.C  test2004_99.C \
test2004_08.C   test2004_112.C  test2004_140.C  test2004_154.C test2004_19.C   test2004_37.C  test2004_54.C  test2004_71.C  test2004_86.C \
test2004_09.C   test2004_113.C  test2004_128.C  test2004_141.C  test2004_155.C  test2004_23.C   test2004_38.C  test2004_55.C  test2004_72.C  test2004_87.C 

# These files are a problem for the "medium" size test.
# test2004_85.C 
# test2004_127.C  
TEST_MEDIUM_PROBLEM_FILE_LIST += test2004_85.C test2004_127.C

ALT3_TEST_MEDIUM_LIST_EXTRA = \
test2004_100.C  test2004_114.C  test2004_129.C  test2004_142.C  test2004_156.C  test2004_24.C   test2004_39.C  test2004_56.C  test2004_73.C  test2004_88.C \
test2004_40.C  test2004_58.C  test2004_74.C  test2004_89.C \
test2004_102.C  test2004_116.C  test2004_144.C  test2004_158.C  test2004_26.C   test2004_42.C  test2004_59.C  test2004_90.C 

# Problem files:
# test2004_101.C  
# test2004_115.C  
# test2004_12.C   
# test2004_143.C 
# test2004_157.C
# test2004_130.C  
# test2004_75.C 
# test2004_103.C  
# test2004_117.C  
# test2004_145.C 
# test2004_159.C 
# test2004_27.C  
# test2004_43.C  
# test2004_60.C  
TEST_MEDIUM_PROBLEM_FILE_LIST += test2004_101.C test2004_115.C test2004_12.C test2004_143.C test2004_157.C test2004_130.C test2004_75.C test2004_103.C test2004_117.C test2004_145.C test2004_159.C test2004_27.C test2004_43.C test2004_60.C  

ALT4_TEST_MEDIUM_LIST_EXTRA = \
test2004_131.C  test2004_76.C test2004_91.C \
test2004_104.C  test2004_118.C  test2004_132.C  test2004_146.C  test2004_15.C  test2004_28.C   test2004_44.C  test2004_61.C  test2004_77.C

# Problem files:
# test2004_93.C 
# test2004_119.C 
# test2004_133.C  
# test2004_147.C 
# test2004_160.C 
# test2004_29.C 
# test2004_45.C 
# test2004_62.C 
# test2004_78.C
# test2004_94.C

# src/frontend/SageIII/sageInterface/generateUniqueName.C:494: std::string SageInterface::generateUniqueName(SgNode*, bool): Assertion `key.empty() == false' failed.
# test2004_105.C 

TEST_MEDIUM_PROBLEM_FILE_LIST += test2004_93.C test2004_119.C test2004_133.C test2004_147.C test2004_160.C test2004_29.C test2004_45.C test2004_62.C test2004_78.C test2004_94.C

# DQ (7/10/2010): Deal with this later!
# test2004_105.C


ALT_TEST_MEDIUM_LIST_EXTRA = \
test2004_01.C   test2004_106.C  test2004_11.C   test2004_134.C  test2004_148.C  test2004_161.C  test2004_30.C  test2004_47.C  test2004_63.C  test2004_79.C  test2004_95.C \
test2004_02.C   test2004_107.C  test2004_120.C  test2004_135.C  test2004_149.C  test2004_162.C  test2004_31.C  test2004_48.C  test2004_64.C  test2004_80.C  test2004_96.C \
test2004_03.C   test2004_108.C  test2004_121.C  test2004_136.C  test2004_14.C   test2004_163.C  test2004_32.C  test2004_49.C  test2004_65.C  test2004_81.C  test2004_97.C \
test2004_04.C   test2004_109.C  test2004_122.C  test2004_137.C  test2004_150.C  test2004_164.C  test2004_33.C  test2004_50.C  test2004_67.C  test2004_82.C  test2004_98.C \
test2004_05.C   test2004_10.C   test2004_123.C  test2004_138.C  test2004_151.C  test2004_16.C   test2004_34.C  test2004_51.C  test2004_68.C  test2004_83.C  test2004_99.C \
test2004_06.C   test2004_110.C  test2004_124.C  test2004_139.C  test2004_152.C  test2004_17.C   test2004_35.C  test2004_52.C  test2004_69.C  test2004_84.C \
test2004_07.C   test2004_111.C  test2004_125.C  test2004_13.C   test2004_153.C  test2004_18.C   test2004_36.C  test2004_53.C  test2004_70.C  test2004_85.C \
test2004_08.C   test2004_112.C  test2004_127.C  test2004_140.C  test2004_154.C  test2004_19.C   test2004_37.C  test2004_54.C  test2004_71.C  test2004_86.C \
test2004_09.C   test2004_113.C  test2004_128.C  test2004_141.C  test2004_155.C  test2004_23.C   test2004_38.C  test2004_55.C  test2004_72.C  test2004_87.C \
test2004_100.C  test2004_114.C  test2004_129.C  test2004_142.C  test2004_156.C  test2004_24.C   test2004_39.C  test2004_56.C  test2004_73.C  test2004_88.C \
test2004_101.C  test2004_115.C  test2004_12.C   test2004_143.C  test2004_157.C  test2004_40.C  test2004_58.C  test2004_74.C  test2004_89.C \
test2004_102.C  test2004_116.C  test2004_130.C  test2004_144.C  test2004_158.C  test2004_26.C   test2004_42.C  test2004_59.C  test2004_75.C  test2004_90.C \
test2004_103.C  test2004_117.C  test2004_131.C  test2004_145.C  test2004_159.C  test2004_27.C   test2004_43.C  test2004_60.C  test2004_76.C  test2004_91.C \
test2004_104.C  test2004_118.C  test2004_132.C  test2004_146.C  test2004_15.C   test2004_28.C   test2004_44.C  test2004_61.C  test2004_77.C  test2004_93.C \
test2004_105.C  test2004_119.C  test2004_133.C  test2004_147.C  test2004_160.C  test2004_29.C   test2004_45.C  test2004_62.C  test2004_78.C  test2004_94.C

# This make this a much larger test.
# TEST_MEDIUM_LIST += $(TEST_MEDIUM_LIST_EXTRA) $(ALT2_TEST_MEDIUM_LIST_EXTRA)
# TEST_MEDIUM_LIST += $(TEST_MEDIUM_LIST_EXTRA)
# TEST_MEDIUM_LIST += $(TEST_MEDIUM_LIST_EXTRA) $(ALT2_TEST_MEDIUM_LIST_EXTRA)
# TEST_MEDIUM_LIST = $(ALT3_TEST_MEDIUM_LIST_EXTRA)
# TEST_MEDIUM_LIST = $(ALT4_TEST_MEDIUM_LIST_EXTRA)
# TEST_MEDIUM_LIST += $(ALT4_TEST_MEDIUM_LIST_EXTRA)
# TEST_MEDIUM_LIST += $(TEST_MEDIUM_LIST_EXTRA) $(ALT2_TEST_MEDIUM_LIST_EXTRA)
TEST_MEDIUM_LIST = $(ALT_TEST_MEDIUM_LIST_EXTRA)

# Error (only works as seperate sets)
# TEST_MEDIUM_LIST += $(TEST_MEDIUM_LIST_EXTRA) $(ALT2_TEST_MEDIUM_LIST_EXTRA) $(ALT4_TEST_MEDIUM_LIST_EXTRA)

# Error these will not merge (though they merge separately).
# TEST_MEDIUM_LIST += $(ALT3_TEST_MEDIUM_LIST_EXTRA)

# Error these will not merge (though they merge separately).
# TEST_MEDIUM_LIST = $(ALT3_TEST_MEDIUM_LIST_EXTRA) $(ALT4_TEST_MEDIUM_LIST_EXTRA)

# Error these will not merge (though they merge separately).
# TEST_MEDIUM_LIST = $(ALT3_TEST_MEDIUM_LIST_EXTRA) test2004_12.C

# Changing hte order causes an error 12,75,27 works but 12,27,75 fails
# TEST_MEDIUM_LIST = test2004_12.C test2004_75.C test2004_27.C 
# TEST_MEDIUM_LIST = test2004_12.C test2004_27.C test2004_75.C 




TEST_MEDIUM_LIST_BINARIES = $(TEST_MEDIUM_LIST:.C=.o)

test-read-medium: $(TEST_MEDIUM_LIST_BINARIES) ../../testAstFileRead
	$(READ_TRANSLATOR) $(TEST_MEDIUM_LIST) temp_output_medium.C

# FAILING_TEST_MEDIUM_LIST = $(TEST_MEDIUM_LIST) $(TEST_MEDIUM_PROBLEM_FILE_LIST)
# FAILING_TEST_MEDIUM_LIST = $(TEST_MEDIUM_PROBLEM_FILE_LIST)
FAILING_TEST_MEDIUM_LIST = test2004_105.C
FAILING_TEST_MEDIUM_LIST_BINARIES = $(FAILING_TEST_MEDIUM_LIST:.C=.o)

failing-test-read-medium: $(FAILING_TEST_MEDIUM_LIST_BINARIES) ../../testAstFileRead
	$(READ_TRANSLATOR) $(FAILING_TEST_MEDIUM_LIST) temp_output_medium.C


TEST_LARGE_LIST_BINARIES = $(TESTCODES:.C=.o)
TEST_Binaries = $(TESTCODES:.C=.C.binary)
# test-read-large: test2001_01.C.binary test2001_02.C.binary test2001_03.C.binary
test-read-large: $(TEST_LARGE_LIST_BINARIES) ../../testAstFileRead
	ls -s $(TEST_Binaries)
#	@echo "$(READ_TRANSLATOR) $(TEST_Binaries:.binary=)"
	$(READ_TRANSLATOR) $(TEST_Binaries:.C.binary=.C) temp_output_large.C

test-read-huge: ../../testAstFileRead
	$(READ_TRANSLATOR) temp_output_large.C temp_output_large.C temp_output_large.C temp_output_large.C temp_output_huge.C

test-read-really-huge: ../../testAstFileRead
	$(READ_TRANSLATOR) temp_output_huge.C temp_output_huge.C temp_output_huge.C temp_output_huge.C temp_output_really_huge.C


FailingTests = test2004_106.C test2004_01.C test2003_38.C test2003_36.C test2003_32.C test2003_28.C test2003_24.C test2003_21.C test2003_19.C test2003_17.C test2003_13.C
FailingTests_Binaries = $(FailingTests:.C=.o)
failingTest: $(FailingTests_Binaries)
#	../../../tests/testAstFileRead test2004_106.C test2004_01.C test2003_38.C test2003_36.C test2003_32.C test2003_28.C test2003_24.C test2003_21.C test2003_19.C test2003_17.C test2003_13.C temp_output.C
	../../../tests/testAstFileRead test2004_106.C test2004_01.C test2003_38.C test2003_36.C test2003_32.C test2003_28.C test2003_24.C

# test2003_21.C 
# test2003_19.C 
# test2003_17.C test2003_13.C temp_output.C

TEST_PARALLEL_SHORT_LIST = test2003_01.C test2003_03.C test2003_05.C test2003_10.C test2003_12.C test2003_14.C
TEST_PARALLEL_SHORT_LIST_BINARIES = $(TEST_PARALLEL_SHORT_LIST:.C=.o)
testParallelMerge-short: parallelMerge $(TEST_PARALLEL_SHORT_LIST_BINARIES)
	rm -rf data temp
	mkdir data
	mkdir temp
	./parallelMerge $(TEST_PARALLEL_SHORT_LIST) temp_output_parallel_short.C

# TEST_PARALLEL_MEDIUM_LIST = $(ORIGINAL_TEST_MEDIUM_LIST)
# TEST_PARALLEL_MEDIUM_LIST = test2003_01.C test2003_03.C test2003_05.C test2003_08.C test2003_10.C test2003_12.C test2003_14.C 
TEST_PARALLEL_MEDIUM_LIST = test2003_01.C test2003_03.C test2003_05.C test2003_10.C test2003_12.C test2003_14.C \
test2003_18.C test2003_20.C test2003_23.C test2003_25.C test2003_27.C test2003_35.C test2003_02.C test2003_06.C test2003_09.C test2003_11.C \
test2003_15.C test2003_17.C test2003_21.C test2003_24.C test2003_26.C test2003_28.C test2003_30.C test2003_32.C test2003_36.C 

# This longer list fails when run in parallel, need to debug this.
# TEST_PARALLEL_MEDIUM_LIST = $(TEST_MEDIUM_LIST)

# Problem test for parallel tests:
# test2003_08.C 
# test2003_16.C 
# test2003_29.C
# test2003_31.C 
# test2003_37.C 
# test2003_04.C
# test2003_13.C 
# test2003_19.C 
# test2003_38.C

TEST_PARALLEL_MEDIUM_LIST_BINARIES = $(TEST_PARALLEL_MEDIUM_LIST:.C=.o)
testParallelMerge-medium: parallelMerge $(TEST_PARALLEL_MEDIUM_LIST_BINARIES)
	rm -rf data temp
	mkdir data
	mkdir temp
	./parallelMerge $(TEST_PARALLEL_MEDIUM_LIST) temp_output_parallel_medium.C

# This is a test of the same files used with parallel test but run in serial.
# This is different and usually faster for small problems than the singled threaded parallel merge.
testParellelMerge-medium-serial: $(TEST_PARALLEL_MEDIUM_LIST_BINARIES)
	$(READ_TRANSLATOR) $(TEST_PARALLEL_MEDIUM_LIST) temp_output_parallel_medium_serial.C

TEST_PARALLEL_LARGE_LIST = $(TESTCODES)
TEST_PARALLEL_LARGE_LIST_BINARIES = $(TEST_PARALLEL_LARGE_LIST:.C=.o)
testParallelMerge-large: parallelMerge $(TEST_PARALLEL_LARGE_LIST_BINARIES)
	rm -rf data temp
	mkdir data
	mkdir temp
	./parallelMerge $(TEST_PARALLEL_LARGE_LIST) temp_output_parallel_large.C

# DQ (3/1/2010): This is the sart of a general command to process all the
# binaries in a directory structure.
ls_generatedBinaries:
#	ls -l `find $(top_builddir)/src -name "*.C.binary" -print`
#	cd $(top_builddir)/src; ls -l `find . -name "*.C.binary" -print`
#	cd $(top_builddir); du -a * | grep "C.binary"
	find $(top_builddir) -name "*.C.binary" -print

ls_binaries:
	ls $(TEST_Binaries)

# TEST_Binaries_short = test2003_*.binary
ls_binaries_short:
	ls -l $(TEST_Binaries_shortTrunk)
#	ls -l $(TEST_Binaries_short:.binary=)


# This tests the compression of the AST internally, a step that makes it contiguious in 
# memory by copying it to intermediate storage and then rebuilding it from scratch.
# Putting the AST into blocks of contiguious storage is what makes the binary file I/O mechanism fast.
# This is a great test of all but the the binary file I/O part (which is the simpler part).
testCompression: astCompressionTest
	./astCompressionTest -c $(srcdir)/test2001_01.C

largeFileTest: astFileIO
	./astFileIO -rose:verbose 0 -c $(srcdir)/test2005_36.C -o test2005_36

largestFileTest: astFileIO
	./astFileIO -rose:verbose 2 -I$(top_builddir) $(ROSE_INCLUDES) -c $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C -o Cxx_Grammar

largestRoseFileTest: astFileIO ../../testAstFileIO
#	./astFileIO -rose:verbose 2 -I$(top_builddir) $(ROSE_INCLUDES) -c $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C -o Cxx_Grammar
	$(TEST_TRANSLATOR) -I$(top_builddir) $(ROSE_INCLUDES) -c $(srcdir)/testRose_testFile.C

locForRoseFile:
	g++ -E -I$(top_builddir) $(ROSE_INCLUDES) -c $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C | wc -l

test-read-tiny_01:
	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir) -c $(srcdir)/input_tiny_01a.C
	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir) -c $(srcdir)/input_tiny_01b.C
	$(READ_TRANSLATOR) input_tiny_01a.C input_tiny_01b.C temp_output_tiny_01.C 

test-read-tiny_02:
	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir) -c $(srcdir)/input_tiny_02a.C
	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir) -c $(srcdir)/input_tiny_02b.C
	$(READ_TRANSLATOR) input_tiny_02a.C input_tiny_02b.C temp_output_tiny_02.C 

test-read-tiny_03:
	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir) -c $(srcdir)/input_tiny_03a.C
	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir) -c $(srcdir)/input_tiny_03b.C
	$(READ_TRANSLATOR) input_tiny_03a.C input_tiny_03b.C temp_output_tiny_03.C 

check-local:
	$(MAKE) $(TEST_Objects)
	$(MAKE) test-read
	$(MAKE) test-read-tiny_01
	$(MAKE) test-read-tiny_02
	$(MAKE) test-read-tiny_03
	$(MAKE) test-read-short
#	DQ (6/25/2010): Commented out testParallelMerge-short for now
#	Test the parallel merge mechanism using different numbers of threads.
	$(MAKE) testParallelMerge-short
	$(MAKE) testParallelMerge-medium
	$(MAKE) testParellelMerge-medium-serial
if ROSE_USE_LONG_MAKE_CHECK_RULE
#	DQ (7/4/2010): Make this only if running longer tests.
	$(MAKE) test-read-medium
#	This is a bit too large of a test to run on most people's machines so skip it for now.
#	$(MAKE) test-read-large
endif
	@echo ""
	@echo "*******************************************************************************************"
	@echo "*** ROSE/tests/roseTests/astFileIOTests: make check rule complete (terminated normally) ***"
	@echo "*******************************************************************************************"

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files pass[1-9] pass1[0-9] pass2[0-9] rose_*.C *.ti *.binary *.C.pdf test1 test200?_*
	rm -rf test_int_lexemes  test_int_lexemes_donot_pass  test_simple_int  test_wchars  X Cxx_Grammar
	rm -f *.dot *.C_identity inputBug317  inputBug327  inputForLoopLocator  lexPhase2003_01  math  test2010_03  test2010_04  test2010_05  test2010_06  test_CplusplusMacro_Cpp
#	Remove some generated files by the parallel merge.
	rm -f *.txt
	rm -rf tmp? data temp_output_*

distclean-local:
	rm -rf Templates.DB 

test1: test1A.C test1B.C
	@echo "Compiling test code using $(TEST_TRANSLATOR) ..."
#	DQ(7/13/2004): This is an error since it specifies the output file twice in the vendor compile line
#	$(TEST_TRANSLATOR) -rose:verbose -o test1A.o -c $(srcdir)/test1A.C 
	$(TEST_TRANSLATOR) -rose:verbose -c $(srcdir)/test1A.C 
	$(TEST_TRANSLATOR) -rose:verbose -c $(srcdir)/test1B.C
	g++ -o test1 rose_test1A.o rose_test1B.o
	@echo "Running resulting executable ..."
	./test1

# DQ (2/5/2009): This is a demonstration that the AST generated 
# from a binary executable fails when being written to disk.
# Ideas about how to simplify this problem:
#  1) Skip the dwarf information to make the problem smaller
#
# Example from Thomas.
test-binary: astFileIO
	./astFileIO -rose:verbose 0 $(srcdir)/buffer2.bin
