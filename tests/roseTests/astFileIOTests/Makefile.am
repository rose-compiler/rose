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

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=80 --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
VALGRIND =

# ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0
ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 -rose:skip_unparse_asm_commands

# TEST_TRANSLATOR = $(top_builddir)/tests/roseTests/astFileIOTests/astFileIO $(ROSE_FLAGS)
# READ_TRANSLATOR = $(top_builddir)/tests/roseTests/astFileIOTests/astFileRead $(ROSE_FLAGS)
# TEST_TRANSLATOR = ./astFileIO $(ROSE_FLAGS)
TEST_TRANSLATOR = $(top_builddir)/tests/testAstFileIO $(ROSE_FLAGS)
READ_TRANSLATOR = ./astFileRead $(ROSE_FLAGS)
TESTCODE_INCLUDES =

# DQ (7/12/2004): Modified to run with make -j4 options
# $(TEST_Objects): astFileIO astFileRead $(TESTCODES)
$(TEST_Objects): astFileIO astFileRead
#	@echo "QY:skipping test code using $(TEST_TRANSLATOR) ..."
#	@echo "Compiling test code using $(TEST_TRANSLATOR) ..."
#	$(VALGRIND) $(TEST_TRANSLATOR) $(srcdir)/$(@:.o=.C) -o $(@:.o=)
	$(VALGRIND) $(TEST_TRANSLATOR) -I$(srcdir)/../../CompileTests/Cxx_tests -c $(srcdir)/../../CompileTests/Cxx_tests/$(@:.o=.C) -o $(@:.o=)
#	@echo "Running resulting executable ..."
#	./$(@:.o=)

# EXTRA_DIST = $(TESTCODES) test2001_05.h
EXTRA_DIST = 

# This tests the reading of a collection of binary files (build by running $(MAKE) $(TEST_Objects)
# test-read: test2001_01.binary test2001_02.binary test2001_03.binary
test-read: rose_test2001_01.C.binary rose_test2001_02.C.binary rose_test2001_03.C.binary
	./astFileRead rose_test2001_01.C rose_test2001_02.C rose_test2001_03.C


# This tests the compression of the AST internally, a step that makes it contiguious in 
# memory by copying it to intermediate storage and then rebuilding it from scratch.
# Putting the AST into blocks of contiguious storage is what the binary file I/O mechanis fast.
# This is a great test of all but the the binary file I/O part (which is the simpler part).
testCompression: astCompressionTest
	./astCompressionTest -c $(srcdir)/test2001_01.C

largeFileTest: astFileIO
	./astFileIO -rose:verbose 0 -c $(srcdir)/test2005_36.C -o test2005_36

largestFileTest: astFileIO
	./astFileIO -rose:verbose 2 -I$(top_builddir) $(ROSE_INCLUDES) -c $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C -o Cxx_Grammar

locForRoseFile:
	g++ -E -I$(top_builddir) $(ROSE_INCLUDES) -c $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C | wc -l

check-local:
	$(MAKE) $(TEST_Objects)
	$(MAKE) test-read
	@echo ""
	@echo "*******************************************************************************************"
	@echo "*** ROSE/tests/roseTests/astFileIOTests: make check rule complete (terminated normally) ***"
	@echo "*******************************************************************************************"

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files pass[1-9] pass1[0-9] pass2[0-9] rose_*.C *.ti *.binary *.C.pdf test1 test200?_*
	rm -rf test_int_lexemes  test_int_lexemes_donot_pass  test_simple_int  test_wchars  X Cxx_Grammar
	rm -f *.C_identity inputBug317  inputBug327  inputForLoopLocator  lexPhase2003_01  math  test2010_03  test2010_04  test2010_05  test2010_06  test_CplusplusMacro_Cpp

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
