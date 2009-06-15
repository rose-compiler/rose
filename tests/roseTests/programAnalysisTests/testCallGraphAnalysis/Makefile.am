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
   classHierarchyTestGen   

testCallGraph_SOURCES = testCallGraph.C

testCG_SOURCES = CG.C

classHierarchyTestGen_SOURCES = ClassHierarchyTestGen.cpp
classHierarchyTestGen_LDADD = $(ROSE_LIBS)

# testCallGraph_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
testCallGraph_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)
testCG_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)



 
# this caused a segfault once  
test01: testCG
	rm -f ./DATABASE
	./testCG -I. -I$(srcdir) $(srcdir)/main.C $(srcdir)/f1.C $(srcdir)/f2.C -o Callgraph

# Test that the call graph generates all the edges it should
test02: testCG stressTestFactory
	echo " $(srcdir)/stressTestFactory 10"
	$(srcdir)/stressTestFactory 10
	rm -f ./DATABASE
	./testCG sT10.C -o sT10
	diff -U5 sT10.cmp.dmp sT10.cg.dmp
	$(srcdir)/stressTestFactory 100
	rm -f ./DATABASE
	./testCG sT100.C -o sT100
	diff -U5 sT100.cmp.dmp sT100.cg.dmp


testMultipleFiles: testCG stressTestFactoryMultipleFiles.pl
	$(srcdir)/testMultipleFiles.sh $(srcdir) . 20 4
			
TESTCODES = test1.C test2.C  test3.C  test4.C  test5.C  test6.C test7.C

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

ROSE_FLAGS = -rose:verbose 0
# ROSE_FLAGS = 

TEST_TRANSLATOR = ./testCallGraph $(ROSE_FLAGS)
TESTCODE_INCLUDES =

# DQ (7/12/2004): Modified to run with make -j4 options
$(TEST_Objects): testCallGraph $(TESTCODES) 
	@echo "Compiling test code using $(TEST_TRANSLATOR) ..."
	$(TEST_TRANSLATOR) $(INCLUDES) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.o=.C) -o $(@:.o=)

EXTRA_DIST = $(TESTCODES) functions.h main.C f1.C f2.C stressTestFactory

# DQ (7/25/2005): The current changes to the AST break these test programs:
check-local:
	@$(MAKE) $(TEST_Objects)
	@$(MAKE) test01 test02
	@echo "***********************************************************************************************************************"
	@echo "*** ROSE/tests/roseTests/programAnalysisTests/testCallGraphAnalysis: make check rule complete (terminated normally) ***"
	@echo "***********************************************************************************************************************"

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) test0? testCG testCallGraph *.o rose_*.C *.dot Templates.DB ii_files ti_files DATABASE DATABASE.dot *.dmp sT*.C

