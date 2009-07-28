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

TESTS_STILL_TOO_HARD = \
test2004_35.C \
test2004_43.C \
test2004_45.C \
test2004_77.C \
test2005_06.C \
test2005_82.C \
test2005_98.C \
test2007_56.C \
test2007_90.C 

TESTS_STILL_TOO_HARD_USING_DATABASE_AND_FILTER = \
test2004_11.C \
test2004_18.C \
test2004_48.C \
test2004_85.C \
test2004_120.C \
test2004_127.C \
test2004_129.C \
test2004_130.C \
test2004_141.C \
test2004_142.C \
test2004_143.C \
test2004_145.C \
test2004_149.C \
test2004_157.C \
test2005_10.C \
test2005_12.C \
test2005_17.C \
test2005_28.C \
test2005_34.C \
test2005_35.C \
test2005_39.C \
test2005_42.C \
test2005_46.C \
test2005_63.C \
test2005_71.C \
test2005_76.C \
test2005_77.C \
test2005_93.C \
test2005_103.C \
test2005_128.C \
test2005_129.C \
test2005_130.C \
test2005_131.C \
test2005_132.C \
test2005_133.C \
test2005_136.C \
test2005_138.C \
test2005_139.C \
test2005_142.C \
test2005_146.C \
test2005_152.C \
test2005_156.C \
test2005_161.C \
test2005_175.C \
test2005_181.C \
test2006_04.C \
test2006_20.C \
test2006_54.C \
test2006_78.C \
test2006_79.C \
test2006_84.C \
test2006_107.C \
test2006_124.C \
test2006_141.C \
test2007_81.C \
test2007_82.C \
test2007_141.C


EXTRA_ALL_TESTCODES = $(EXTRA_TESTCODES_REQUIRED_TO_PASS) $(TESTS_STILL_TOO_HARD_USING_DATABASE_AND_FILTER)

EXTRA_TEST_Objects = ${EXTRA_ALL_TESTCODES:.C=.C.dot}

$(EXTRA_TEST_Objects): testCallGraph
#	@echo "Copy the file to the local build tree so that we can change the file name suffix (to be a C language file) ..." # Not true anymore
	$(VALGRIND) ./testCallGraph $(ROSE_FLAGS) -I$(srcdir)/../Cxx_tests $(TESTCODE_INCLUDES) -db:name $(@:.dot=.database) -c $(top_srcdir)/developersScratchSpace/Dan/Cxx_tests/$(@:.dot=)


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
	rm -f  $(@:.o=.db)
	$(TEST_TRANSLATOR) $(INCLUDES) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.o=.C) -o $(@:.o=) -db:name $(@:.o=.db)
	rm -f  $(@:.o=.db)


EXTRA_DIST = $(TESTCODES) functions.h main.C f1.C f2.C stressTestFactory

# DQ (7/25/2005): The current changes to the AST break these test programs:
check-local:
	@$(MAKE) $(EXTRA_TEST_Objects)
	@$(MAKE) $(TEST_Objects)
	@$(MAKE) test01 test02
	@echo "***********************************************************************************************************************"
	@echo "*** ROSE/tests/roseTests/programAnalysisTests/testCallGraphAnalysis: make check rule complete (terminated normally) ***"
	@echo "***********************************************************************************************************************"

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) test0? testCG testCallGraph *.o rose_*.C *.dot Templates.DB ii_files ti_files DATABASE DATABASE.dot *.dmp sT*.C

