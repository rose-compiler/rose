include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .

# DQ (8/1/2005): Uncommented to force test code to build, but tests currently fail
# QY 11/9/04 comment out test
# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = astFileIO astFileRead astCompressionTest

astFileIO_SOURCES = astFileIO.C 
astFileIO_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

astCompressionTest_SOURCES = astCompressionTest.C 
astCompressionTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

astFileRead_SOURCES = astFileRead.C
astFileRead_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

# astFileIO_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

TESTCODES = \
          test2001_01.C \
          test2001_02.C \
          test2001_03.C \
          test2001_05.C \
          test2001_06.C \
          test2001_07.C \
          test2001_08.C \
          test2001_09.C \
          test2001_10.C

# Reduce the number of tests here since they don't test many different parts of the File I/O mechanism.
EXTRA_TESTCODES = \
          test2001_14.C \
          test2001_15.C \
          test2001_17.C \
          test2001_18.C \
          test2001_19.C \
          test2001_20.C \
          test2001_25.C \
          test2001_26.C \
          test2001_29.C \
          test2002_01.C \
          test2003_02.C \
          test2003_03.C \
          test2003_04.C \
          test2003_05.C \
          test2003_06.C \
          test2003_08.C \
          test2003_09.C \
          test2003_10.C \
          test2003_11.C \
          test2003_12.C \
          test2003_13.C \
          test2003_14.C \
          test2003_15.C \
          test2003_16.C \
          test2003_17.C \
          test2003_18.C \
          test2003_19.C \
          test2003_20.C \
          test2003_24.C \
          test2003_25.C \
          test2003_26.C \
          test2003_27.C \
          test2003_28.C \
          test2003_29.C \
          test2003_30.C \
          test2003_31.C \
          test2003_32.C \
          test2003_35.C \
          test2003_36.C \
          test2003_37.C \
          test2003_38.C \
          test2004_01.C \
          test2004_02.C \
          test2004_03.C \
          test2004_04.C \
          test2004_05.C \
          test2004_06.C \
          test2004_07.C \
          test2004_08.C \
          test2004_09.C \
          test2004_10.C \
          test2004_11.C \
          test2004_12.C \
          test2004_13.C \
          test2004_14.C \
          test2004_15.C \
          test2004_16.C \
          test2004_17.C \
          test2004_18.C \
          test2004_23.C \
          test2004_26.C \
          test2004_27.C \
          test2004_28.C \
          test2004_29.C \
          test2004_30.C \
          test2004_31.C \
          test2004_32.C \
          test2004_33.C \
          test2004_34.C \
          test2004_35.C \
          test2004_36.C \
          test2004_37.C \
          test2004_38.C \
          test2004_39.C \
          test2004_42.C \
          test2004_43.C \
          test2004_44.C \
          test2004_45.C \
          test2004_47.C \
          test2004_48.C \
          test2004_49.C \
          test2004_50.C \
          test2004_51.C \
          test2004_52.C \
          test2004_53.C \
          test2004_54.C \
          test2004_55.C \
          test2004_56.C \
          test2004_58.C \
          test2004_59.C \
          test2004_60.C \
          test2004_61.C \
          test2004_63.C \
          test2004_64.C \
          test2004_65.C \
          test2004_67.C \
          test2004_68.C \
          test2004_69.C \
          test2004_70.C \
          test2004_71.C \
          test2004_72.C \
          test2004_73.C \
          test2004_74.C \
          test2004_75.C \
          test2004_76.C \
          test2004_77.C \
          test2004_78.C \
          test2004_79.C \
          test2004_80.C \
          test2004_81.C \
          test2004_82.C \
          test2004_84.C \
          test2004_85.C \
          test2004_86.C \
          test2004_87.C \
          test2004_88.C \
          test2004_89.C \
          test2004_90.C \
          test2004_91.C \
          test2004_93.C \
          test2004_94.C \
          test2004_95.C \
          test2004_96.C \
          test2004_97.C \
          test2004_98.C \
          test2004_99.C \
          test2004_100.C \
          test2004_101.C \
          test2004_102.C \
          test2004_103.C \
          test2004_104.C \
          test2004_105.C \
          test2004_106.C \
          test2004_107.C \
          test2004_108.C \
          test2004_109.C \
          test2004_110.C \
          test2004_111.C \
          test2004_112.C \
          test2004_113.C \
          test2004_114.C \
          test2004_115.C \
          test2004_116.C \
          test2004_117.C \
          test2004_118.C \
          test2004_119.C \
          test2004_120.C \
          test2004_121.C \
          test2004_122.C \
          test2004_123.C \
          test2004_124.C \
          test2004_125.C \
          test2004_127.C \
          test2004_128.C \
          test2004_129.C \
          test2004_130.C \
          test2004_131.C \
          test2004_132.C \
          test2004_133.C \
          test2004_134.C \
          test2004_135.C \
          test2004_136.C \
          test2004_137.C \
          test2004_138.C \
          test2004_139.C \
          test2004_140.C \
          test2004_141.C \
          test2004_142.C \
          test2004_143.C \
          test2004_144.C \
          test2004_145.C \
          test2004_146.C \
          test2004_147.C \
          test2004_148.C \
          test2004_149.C \
          test2004_150.C \
          test2004_151.C \
          test2004_153.C \
          test2004_155.C \
          test2004_156.C \
          test2004_157.C \
          test2004_158.C \
          test2004_159.C \
          test2004_160.C \
          test2004_161.C \
          test2004_162.C \
          test2004_163.C \
          test2004_164.C \
          test2005_01.C \
          test2005_02.C \
          test2005_03.C \
          test2005_04.C \
          test2005_05.C \
          test2005_06.C \
          test2005_07.C \
          test2005_08.C \
          test2005_09.C \
          test2005_10.C \
          test2005_11.C \
          test2005_12.C \
          test2005_13.C \
          test2005_14.C \
          test2005_15.C \
          test2005_16.C \
          test2005_17.C \
          test2005_19.C \
          test2005_20.C \
          test2005_21.C \
          test2005_22.C \
          test2005_23.C \
          test2005_24.C \
          test2005_25.C \
          test2005_26.C \
          test2005_27.C \
          test2005_28.C \
          test2005_29.C \
          test2005_30.C \
          test2005_31.C \
          test2005_32.C \
          test2005_33.C \
          test2005_34.C \
          test2005_35.C \
          test2005_36.C \
          test2005_37.C \
          test2005_38.C \
          test2005_39.C \
          test2005_40.C \
          test2005_41.C \
          test2005_42.C \
          test2005_43.C \
          test2005_44.C \
          test2005_45.C \
          test2005_46.C \
          test2005_47.C \
          test2005_48.C \
          test2005_49.C \
          test2005_50.C \
          test2005_51.C \
          test2005_52.C \
          test2005_53.C \
          test2005_55.C \
          test2005_57.C \
          test2005_58.C \
          test2005_59.C \
          test2005_60.C \
          test2005_61.C \
          test2005_62.C \
          test2005_63.C \
          test2005_64.C \
          test2005_65.C \
          test2005_66.C \
          test2005_67.C \
          test2005_68.C \
          test2005_69.C \
          test2005_70.C \
          test2005_71.C \
          test2005_72.C \
          test2005_73.C \
          test2005_74.C \
          test2005_75a.C \
          test2005_75b.C \
          test2005_76.C \
          test2005_77.C \
          test2005_78.C \
          test2005_79.C \
          test2005_80.C \
          test2005_82.C \
          test2005_83.C \
          test2005_84.C \
          test2005_85.C \
          test2005_86.C \
          test2005_87.C \
          test2005_88.C \
          test2005_89.C \
          test2005_90.C \
          test2005_91.C \
          test2005_92.C \
          test2005_93.C \
          test2005_94.C \
          test2005_95.C \
          test2005_96.C \
          test2005_97.C \
          test2005_98.C \
          test2005_99.C \
          test2005_100.C \
          test2005_101.C \
          test2005_102.C \
          test2005_103.C \
          test2005_104.C \
          test2005_105.C \
          test2005_106.C \
          test2005_107.C \
          test2005_108.C \
          test2005_109.C \
          test2005_110.C \
          test2005_111.C \
          test2005_112.C \
          test2005_113.C \
          test2005_114.C \
          test2005_116.C \
          test2005_117.C \
          test2005_118.C \
          test2005_119.C \
          test2005_120.C \
          test2005_121.C \
          test2005_122.C \
          test2005_123.C \
          test2005_124.C \
          test2005_125.C \
          test2005_126.C \
          test2005_127.C \
          test2005_128.C \
          test2005_129.C \
          test2005_130.C \
          test2005_131.C \
          test2005_132.C \
          test2005_133.C \
          test2005_134.C \
          test2005_135.C \
          test2005_136.C \
          test2005_137.C \
          test2005_138.C \
          test2005_139.C \
          test2005_140.C \
          test2005_141.C \
          test2005_142.C \
          test2005_143.C \
          test2005_144.C \
          test2005_145.C \
          test2005_146.C \
          test2005_147.C \
          test2005_148.C \
          test2005_149.C \
          test2005_150.C \
          test2005_151.C \
          test2005_152.C \
          test2005_153.C \
          test2005_154.C \
          test2005_155.C \
          test2005_156.C \
          test2005_157.C \
          test2005_158.C \
          test2005_159.C \
          test2005_160.C 

# These currently fail (will be fixed by Alin later when ROSE works better with template functions)
# template_function.C
# template_class.C

# These codes are DESIGNED to fail in the inlining
# fail1.C fail2.C

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=80 --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
VALGRIND =

ROSE_FLAGS = -rose:verbose 0
# TEST_TRANSLATOR = $(top_builddir)/tests/roseTests/astFileIOTests/astFileIO $(ROSE_FLAGS)
# READ_TRANSLATOR = $(top_builddir)/tests/roseTests/astFileIOTests/astFileRead $(ROSE_FLAGS)
TEST_TRANSLATOR = ./astFileIO $(ROSE_FLAGS)
READ_TRANSLATOR = ./astFileRead $(ROSE_FLAGS)
TESTCODE_INCLUDES =

# DQ (7/12/2004): Modified to run with make -j4 options
$(TEST_Objects): astFileIO astFileRead $(TESTCODES)
#	@echo "QY:skipping test code using $(TEST_TRANSLATOR) ..."
#	@echo "Compiling test code using $(TEST_TRANSLATOR) ..."
	$(VALGRIND) $(TEST_TRANSLATOR) $(srcdir)/$(@:.o=.C) -o $(@:.o=)
#	@echo "Running resulting executable ..."
#	./$(@:.o=)

EXTRA_DIST = $(TESTCODES) test2001_05.h

# This tests the reading of a collection of binary files (build by running $(MAKE) $(TEST_Objects)
test-read: test2001_01.binary test2001_02.binary test2001_03.binary
	./astFileRead test2001_01 test2001_02 test2001_03

# This tests the compression of the AST internally, a step that makes it contiguious in 
# memory by copying it to intermediate storage and then rebuilding it from scratch.
# Putting the AST into blocks of contiguious storage is what the binary file I/O mechanis fast.
# This is a great test of all but the the binary file I/O part (which is the simpler part).
testCompression: astCompressionTest
	./astCompressionTest -c $(srcdir)/test2001_01.C

largeFileTest: astFileIO
	./astFileIO -rose:verbose 0 -c $(srcdir)/test2005_36.C -o test2005_36

largestFileTest: astFileIO
	./astFileIO -rose:verbose 0 -I$(top_builddir) $(ROSE_INCLUDES) -c $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C -o Cxx_Grammar

check-local:
	$(MAKE) $(TEST_Objects)
	$(MAKE) test-read
	@echo ""
	@echo "*******************************************************************************************"
	@echo "*** ROSE/tests/roseTests/astFileIOTests: make check rule complete (terminated normally) ***"
	@echo "*******************************************************************************************"

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files pass[1-9] pass1[0-9] pass2[0-9] rose_*.C *.ti *.binary *.C.pdf test1 test200?_??

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
