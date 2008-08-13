include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST = simpletest.php

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

## Don't use the repository in ../src
#CXX_TEMPLATE_REPOSITORY_PATH = .

# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = runTest

runTest_SOURCES = runTest.C

# testCallGraph_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
runTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

#TESTCODES = buffer2.bin

# Build the list of object files
#TEST_Objects = ${TESTCODES:.C=.o}

ROSE_FLAGS = -rose:verbose 0
# ROSE_FLAGS = 

TEST_TRANSLATOR = ./runTest all $(ROSE_FLAGS)
TESTCODE_INCLUDES =

# DQ (7/12/2004): Modified to run with make -j4 options
#$(TEST_Objects): runTest $(TESTCODES) 
#	@echo "Compiling test code using $(TEST_TRANSLATOR) ..."
#	env SRCDIR="$(srcdir)" $(TEST_TRANSLATOR)

CLEANFILES = unparsed.s callgraph.gml output

# DQ (7/25/2005): The current changes to the AST break these test programs:
check-local: runTest 
	./runTest $(srcdir)/simpletest.php > output
	@echo "*********************************************************************************************************************"
	@echo "*** ROSE/tests/roseTests/PHPTests/runTest: make check rule complete (terminated normally) ***"
	@echo "*********************************************************************************************************************"

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) test* rose_*.C *.dot Templates.DB ii_files ti_files DATABASE DATABASE.dot

distclean-local:
	rm -rf Templates.DB 
