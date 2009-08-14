include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)
# INCLUDES =

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .

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

virtualFctsTester_SOURCES = VirtualFctsTester.cpp
virtualFctsTester_LDADD   = $(ROSE_LIBS)

# testCallGraph_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
testCallGraph_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
testCG_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

# this caused a segfault once  
test01: testCG
	rm -f ./test01.db
	./testCG -I. -I$(srcdir) $(srcdir)/main.C $(srcdir)/f1.C $(srcdir)/f2.C -o Callgraph -db:name test01.db
	rm -f ./test01.db


# Test that the call graph generates all the edges it should
test02: testCG stressTestFactory
	echo " $(srcdir)/stressTestFactory 10"
	$(srcdir)/stressTestFactory 10
	rm -f ./sT10.db
	./testCG sT10.C -o sT10 -compare:graph sT10.cg.dmp -db:name sT10.db
	rm -f ./sT10.db
	diff -U5 sT10.cmp.dmp sT10.cg.dmp
	$(srcdir)/stressTestFactory 100
	rm -f ./sT100.db
	./testCG sT100.C -o sT100 -compare:graph sT100.cg.dmp -db:name sT100.db
	rm -f ./sT100.db
	diff -U5 sT100.cmp.dmp sT100.cg.dmp


testMultipleFiles: testCG stressTestFactoryMultipleFiles.pl
	$(srcdir)/testMultipleFiles.sh $(srcdir) . 20 4

# Import list of C++ test cases that should pass:
#   EXAMPLE_TESTCODES_REQUIRED_TO_PASS
#   EXAMPLE_TESTCODES_REQUIRED_TO_PASS_SHORT
#
include $(top_srcdir)/tests/CompileTests/Cxx_tests/Makefile-pass.inc
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
	$(VALGRIND) ./testCG $(ROSE_FLAGS) -I$(top_srcdir)/tests/CompileTests/Cxx_tests $(TESTCODE_INCLUDES) -db:name $(@:.dot=.database) -c $(top_srcdir)/tests/CompileTests/Cxx_tests/$(@:.dot=) -o $(@:.C.dot=.o) -DTEST_STRING_MACRO="\"test\""
#	cp  $(@:.C.dot=.o.cg.dmp)  $(srcdir)/$(@:.dot=.cg.dmp)
#	@if diff -U5 $(srcdir)/tests/$(@:.dot=.cg.dmp) $(@:.C.dot=.o.cg.dmp); then :; else echo "Files differ; byte order test failed"; exit 1; fi
	rm -f  $(@:.o=.db)


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
#	@if diff -U5 $(srcdir)/tests/$(@:.o=.C.cg.dmp) $(@:.o=.cg.dmp); then :; else echo "Files differ; byte order test failed"; exit 1; fi
	rm -f  $(@:.o=.db)


EXTRA_DIST = $(TESTCODES) functions.h main.C f1.C f2.C stressTestFactory $(srcdir)/tests

# DQ (7/25/2005): The current changes to the AST break these test programs:
check-local:
	@$(MAKE) $(TEST_Objects)
	@$(MAKE) test01 test02
	@$(MAKE) $(EXTRA_TEST_Objects)
	@echo "***********************************************************************************************************************"
	@echo "*** ROSE/tests/roseTests/programAnalysisTests/testCallGraphAnalysis: make check rule complete (terminated normally) ***"
	@echo "***********************************************************************************************************************"

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) test0? testCG testCallGraph *.o rose_*.C *.dot Templates.DB ii_files ti_files DATABASE DATABASE.dot *.dmp sT*.C

