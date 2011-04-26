include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

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

testCallGraph_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
testCG_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

test01: testCG
	./testCG --edg:no_warnings -I. -I$(srcdir) $(srcdir)/main.C $(srcdir)/f1.C $(srcdir)/f2.C -o Callgraph
	rm -f Callgraph.cg.dmp Callgraph.dot 


# Test that the call graph generates all the edges it should
test02: testCG stressTestFactory
	echo " $(srcdir)/stressTestFactory 10"
	$(srcdir)/stressTestFactory 10
	rm -f ./sT10.db
	./testCG --edg:no_warnings sT10.C -o sT10 -compare:graph sT10.cg.dmp
	diff -U5 sT10.cmp.dmp sT10.cg.dmp
	rm -f ./sT10.db ./sT10.cg.dmp ./sT10.cmp.dmp ./sT10.dot ./sT10.C
	$(srcdir)/stressTestFactory 100
	./testCG --edg:no_warnings sT100.C -o sT100 -compare:graph sT100.cg.dmp
	diff -U5 sT100.cmp.dmp sT100.cg.dmp
	rm -f ./sT100.cg.dmp ./sT100.cmp.dmp ./sT100.dot ./sT100.C


testMultipleFiles: testCG stressTestFactoryMultipleFiles.pl
	$(srcdir)/testMultipleFiles.sh $(srcdir) . 20 4

# DQ (8/15/2009): Added simple case of multiple file test...
testSimpleMultipleFiles: testCG $(srcdir)/input_simpleMultipleFileTest_01.C $(srcdir)/input_simpleMultipleFileTest_02.C 
	echo "Testing simple case of multiple file call graph generation..."
	./testCG $(srcdir)/input_simpleMultipleFileTest_01.C -o input_simpleMultipleFileTest_01
	./testCG $(srcdir)/input_simpleMultipleFileTest_02.C -o input_simpleMultipleFileTest_02
	echo "The final call graph is in input_simpleMultipleFileTest_02.dot (latest file called)"

# DQ (8/15/2009): Added simple case of multiple file test for files in separate directories...
testSimpleMultipleFilesInSeperateDirectories: testCG $(srcdir)/input_simpleMultipleFileTest_01.C $(srcdir)/input_simpleMultipleFileTest_02.C
	echo "Testing simple case of multiple file call graph generation..."
	rm -f simpleMultipleFileTestInSeperateDirectories.db
	rm -rf seperateDirectory_1 seperateDirectory_2
	mkdir seperateDirectory_1; cp $(srcdir)/input_simpleMultipleFileTest_01.C seperateDirectory_1/input_simpleMultipleFileTestInSeperateDirectories_01.C
	cd seperateDirectory_1; ../testCG -I$(srcdir) -c input_simpleMultipleFileTestInSeperateDirectories_01.C -o input_simpleMultipleFileTestInSeperateDirectories_01
	mkdir seperateDirectory_2; cp $(srcdir)/input_simpleMultipleFileTest_02.C seperateDirectory_2/input_simpleMultipleFileTestInSeperateDirectories_02.C
	cd seperateDirectory_2; ../testCG -I$(srcdir) -c input_simpleMultipleFileTestInSeperateDirectories_02.C -o input_simpleMultipleFileTestInSeperateDirectories_02
	echo "The final call graph is in input_simpleMultipleFileTestInSeperateDirectories_02.dot (latest file called)"


# Import list of C++ test cases that should pass:
#   EXAMPLE_TESTCODES_REQUIRED_TO_PASS
#   EXAMPLE_TESTCODES_REQUIRED_TO_PASS_SHORT
#
include $(srcdir)/Makefile-pass.inc

# Import list of C++ test cases that are known to fail:
#   TESTCODE_CURRENTLY_FAILING
#
include $(top_srcdir)/tests/CompileTests/Cxx_tests/Makefile-fail.inc


EXTRA_TEST_Objects = $(EXAMPLE_TESTCODES_REQUIRED_TO_PASS) $(TESTS_STILL_TOO_HARD)

## Include the path to A++ and the transformation specification
TESTCODE_INCLUDES = -I$(top_srcdir)/tests/CompileTests/A++Code

TESTCODES = test1.C test2.C  test3.C  test4.C  test5.C test7.C test6.C 

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

TEST_TRANSLATOR = ./testCG $(ROSE_FLAGS) -rose:verbose 0 --edg:no_warnings

# This target compiles the ROSE test files with the testCG Translator. Then, it compares the output of the translator to previous dumps.
$(TEST_Objects): testCG $(TESTCODES) 
	@echo "Compiling test code using $(TEST_TRANSLATOR) ..."
	$(TEST_TRANSLATOR) $(INCLUDES) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.o=.C) -o $(@:.o=)
	@if diff -U5 $(srcdir)/tests/$(@:.o=.C.cg.dmp) $(@:.o=.cg.dmp); then :; else echo "Files differ; byte order test failed"; exit 1; fi
	rm -f  $(@:.o=.cg.dmp)  $(@:.o=.cmp.dmp) $(@:.o=.dot)

$(EXTRA_TEST_Objects): testCG
	$(VALGRIND) $(TEST_TRANSLATOR) -I$(top_srcdir)/tests/CompileTests/Cxx_tests $(TESTCODE_INCLUDES) -c $(top_srcdir)/tests/CompileTests/Cxx_tests/$(@) -o $(@:.C=.C) -DTEST_STRING_MACRO="\"test\""
#	@if diff -U5 $(srcdir)/tests/$(@:.C=.C.cg.dmp) $(@:.C=.o.cg.dmp); then :; else echo "Files differ; byte order test failed"; exit 1; fi
#	rm -f  $(@:.C=.o.cg.dmp)

EXTRA_DIST = $(TESTCODES) functions.h main.C f1.C f2.C stressTestFactory $(srcdir)/tests input_simpleMultipleFileTest_01.C input_simpleMultipleFileTest_02.C


check-local:
	@$(MAKE) $(TEST_Objects)
	@$(MAKE) test01 test02
	@$(MAKE) $(EXTRA_TEST_Objects)

	@echo "***********************************************************************************************************************"
	@echo "*** ROSE/tests/roseTests/programAnalysisTests/testCallGraphAnalysis: make check rule complete (terminated normally) ***"
	@echo "***********************************************************************************************************************"

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) test0? testCG testCallGraph *.o rose_*.C *.dot Templates.DB ii_files ti_files DATABASE DATABASE.dot *.dmp sT*.C
	rm -rf seperateDirectory_1 seperateDirectory_2 Callgraph.cg.dmp Callgraph.dot

