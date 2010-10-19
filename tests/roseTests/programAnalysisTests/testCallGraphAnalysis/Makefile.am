include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)
# INCLUDES =

## Don't use the repository in ../src
# CXX_TEMPLATE_REPOSITORY_PATH = .

# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = \
   testCallGraph \
   testCG \
   classHierarchyTestGen\
   virtualFctsTester

testCallGraph_SOURCES = testCallGraph.C

testCG_SOURCES = CG.C

classHierarchyTestGen_SOURCES = ClassHierarchyTestGen.cpp
classHierarchyTestGen_LDADD = $(ROSE_LIBS)

# DQ (2/10/2010): ROSE currently fails to generate code for this file (it does however compile for analysis only).
# virtualFctsTester_SOURCES = VirtualFctsTester.cpp
virtualFctsTester_SOURCES =
if !ROSE_USING_ROSE_CODE_GENERATION
   virtualFctsTester_SOURCES += VirtualFctsTester.cpp
endif

virtualFctsTester_LDADD   = $(ROSE_LIBS)

# testCallGraph_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
testCallGraph_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
testCG_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

# this caused a segfault once  
test01: testCG
	rm -f ./test01.db
	./testCG -I. -I$(srcdir) $(srcdir)/main.C $(srcdir)/f1.C $(srcdir)/f2.C -o Callgraph -db:name test01.db
	rm -f ./test01.db Callgraph.cg.dmp Callgraph.dot 


# Test that the call graph generates all the edges it should
test02: testCG stressTestFactory
	echo " $(srcdir)/stressTestFactory 10"
	$(srcdir)/stressTestFactory 10
	rm -f ./sT10.db
	./testCG sT10.C -o sT10 -compare:graph sT10.cg.dmp -db:name sT10.db
	diff -U5 sT10.cmp.dmp sT10.cg.dmp
	rm -f ./sT10.db ./sT10.cg.dmp ./sT10.cmp.dmp ./sT10.dot ./sT10.C
	$(srcdir)/stressTestFactory 100
	rm -f ./sT100.db
	./testCG sT100.C -o sT100 -compare:graph sT100.cg.dmp -db:name sT100.db
	diff -U5 sT100.cmp.dmp sT100.cg.dmp
	rm -f ./sT100.db ./sT100.cg.dmp ./sT100.cmp.dmp ./sT100.dot ./sT100.C


testMultipleFiles: testCG stressTestFactoryMultipleFiles.pl
	$(srcdir)/testMultipleFiles.sh $(srcdir) . 20 4

# DQ (8/15/2009): Added simple case of multiple file test...
testSimpleMultipleFiles: testCG $(srcdir)/input_simpleMultipleFileTest_01.C $(srcdir)/input_simpleMultipleFileTest_02.C 
	echo "Testing simple case of multiple file call graph generation..."
	./testCG $(srcdir)/input_simpleMultipleFileTest_01.C -o input_simpleMultipleFileTest_01 -db:name input_simpleMultipleFileTest.db
	./testCG $(srcdir)/input_simpleMultipleFileTest_02.C -o input_simpleMultipleFileTest_02 -db:name input_simpleMultipleFileTest.db
	echo "The final call graph is in input_simpleMultipleFileTest_02.dot (latest file called)"

# DQ (8/15/2009): Added simple case of multiple file test for files in separate directories...
testSimpleMultipleFilesInSeperateDirectories: testCG $(srcdir)/input_simpleMultipleFileTest_01.C $(srcdir)/input_simpleMultipleFileTest_02.C
	echo "Testing simple case of multiple file call graph generation..."
	rm -f simpleMultipleFileTestInSeperateDirectories.db
	rm -rf seperateDirectory_1 seperateDirectory_2
	mkdir seperateDirectory_1; cp $(srcdir)/input_simpleMultipleFileTest_01.C seperateDirectory_1/input_simpleMultipleFileTestInSeperateDirectories_01.C
	cd seperateDirectory_1; ../testCG -I$(srcdir) -c input_simpleMultipleFileTestInSeperateDirectories_01.C -o input_simpleMultipleFileTestInSeperateDirectories_01 -db:name ../simpleMultipleFileTestInSeperateDirectories.db
	mkdir seperateDirectory_2; cp $(srcdir)/input_simpleMultipleFileTest_02.C seperateDirectory_2/input_simpleMultipleFileTestInSeperateDirectories_02.C
	cd seperateDirectory_2; ../testCG -I$(srcdir) -c input_simpleMultipleFileTestInSeperateDirectories_02.C -o input_simpleMultipleFileTestInSeperateDirectories_02 -db:name ../simpleMultipleFileTestInSeperateDirectories.db
	echo "The final call graph is in input_simpleMultipleFileTestInSeperateDirectories_02.dot (latest file called)"


# Import list of C++ test cases that should pass:
#   EXAMPLE_TESTCODES_REQUIRED_TO_PASS
#   EXAMPLE_TESTCODES_REQUIRED_TO_PASS_SHORT
#
include $(srcdir)/Makefile-pass.inc
# include $(srcdir)/Makefile-pass.inc

# Import list of C++ test cases that are known to fail:
#   TESTCODE_CURRENTLY_FAILING
#
include $(top_srcdir)/tests/CompileTests/Cxx_tests/Makefile-fail.inc
# include $(srcdir)/Makefile-fail.inc

EXTRA_TESTCODES_REQUIRED_TO_PASS = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS)

EXTRA_ALL_TESTCODES = $(EXTRA_TESTCODES_REQUIRED_TO_PASS) $(TESTS_STILL_TOO_HARD)

# DQ (8/1/2009): This should be uncommented to run all the proper tests.
# EXTRA_TEST_Objects = ${EXTRA_ALL_TESTCODES:.C=.C.dot}
EXTRA_TEST_Objects = ${EXTRA_TESTCODES_REQUIRED_TO_PASS:.C=.C.dot}

## Include the path to A++ and the transformation specification
TESTCODE_INCLUDES = -I$(top_srcdir)/tests/CompileTests/A++Code

$(EXTRA_TEST_Objects): testCG
#	@echo "Copy the file to the local build tree so that we can change the file name suffix (to be a C language file) ..." # Not true anymore
#	$(TEST_TRANSLATOR) $(INCLUDES) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.o=.C) -o $(@:.o=) -db:name $(@:.o=.db)
	$(VALGRIND) ./testCG $(ROSE_FLAGS) -I$(top_srcdir)/tests/CompileTests/Cxx_tests $(TESTCODE_INCLUDES) -db:name $(@:.dot=.db) -c $(top_srcdir)/tests/CompileTests/Cxx_tests/$(@:.dot=) -o $(@:.C.dot=.o) -DTEST_STRING_MACRO="\"test\""
#	cp  $(@:.C.dot=.o.cg.dmp)  $(srcdir)/tests/$(@:.dot=.cg.dmp)
	@if diff -U5 $(srcdir)/tests/$(@:.dot=.cg.dmp) $(@:.C.dot=.o.cg.dmp); then :; else echo "Files differ; byte order test failed"; exit 1; fi
	rm -f  $(@:.dot=.db) $(@:.C.dot=.o.dot) $(@:.C.dot=.o.cg.dmp)



TESTCODES = test1.C test2.C  test3.C  test4.C  test5.C test7.C test6.C 

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

ROSE_FLAGS = -rose:verbose 0
# ROSE_FLAGS = 

TEST_TRANSLATOR = ./testCG $(ROSE_FLAGS)
# TESTCODE_INCLUDES =

# DQ (7/12/2004): Modified to run with make -j4 options
$(TEST_Objects): testCG $(TESTCODES) 
	@echo "Compiling test code using $(TEST_TRANSLATOR) ..."
	rm -f  $(@:.o=.db)
	$(TEST_TRANSLATOR) $(INCLUDES) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.o=.C) -o $(@:.o=) -db:name $(@:.o=.db)
	@if diff -U5 $(srcdir)/tests/$(@:.o=.C.cg.dmp) $(@:.o=.cg.dmp); then :; else echo "Files differ; byte order test failed"; exit 1; fi
	rm -f  $(@:.o=.db) $(@:.o=.cg.dmp)  $(@:.o=.cmp.dmp) $(@:.o=.dot)



EXTRA_DIST = $(TESTCODES) functions.h main.C f1.C f2.C stressTestFactory $(srcdir)/tests input_simpleMultipleFileTest_01.C input_simpleMultipleFileTest_02.C

# DQ (7/25/2005): The current changes to the AST break these test programs:
check-local:
# DQ (9/10/2009): Skip tests on Debian system to test ROSE (and focus first on where it works!)
if !OS_VENDOR_DEBIAN
if !OS_MACOSX
# DQ (9/13/2009): This controls use of testing in ROSE where it is dependent 
# upon the backend compiler of rht version of the backend compiler.
if BACKEND_COMPILER_VERSION_OK_FOR_ROSE_TESTING
	@$(MAKE) $(TEST_Objects)
	@$(MAKE) test01 test02

# DQ (9/13/2009): Commented out these tests; test test2004_153.C fails even with just g++ 4.2.2.
#	@$(MAKE) $(EXTRA_TEST_Objects)
endif
endif
	@echo "***********************************************************************************************************************"
	@echo "*** ROSE/tests/roseTests/programAnalysisTests/testCallGraphAnalysis: make check rule complete (terminated normally) ***"
	@echo "***********************************************************************************************************************"
else
	@echo "***** SKIPPING TESTS IN AST CALLGRAPH TESTS *****"
endif

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) test0? testCG testCallGraph *.o rose_*.C *.dot Templates.DB ii_files ti_files DATABASE DATABASE.dot *.dmp sT*.C
	rm -rf seperateDirectory_1 seperateDirectory_2 Callgraph.cg.dmp Callgraph.dot

