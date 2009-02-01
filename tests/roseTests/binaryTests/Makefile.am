include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST = buffer2.bin runTest.C

if USE_BINARY_ANALYSIS_TESTS

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/midend/binaryAnalyses/libraryIdentification

## Don't use the repository in ../src
#CXX_TEMPLATE_REPOSITORY_PATH = .

if ROSE_USE_SQLITE_DATABASE

# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = runTest testLibraryDb

testLibraryDb_SOURCES = testLibraryDb.C
# testCallGraph_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
testLibraryDb_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS) 

test1: testLibraryDb
	./testLibraryDb
	rm test.db
else

bin_PROGRAMS  = runTest

endif
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

if ROSE_USE_SQLITE_DATABASE

check-local: runTest 
	$(MAKE) test1
	./runTest $(srcdir)/buffer2.bin > output
	@echo "*********************************************************************************************************************"
	@echo "*** ROSE/tests/roseTests/programAnalysisTests/binaryTests: make check rule complete (terminated normally)         ***"
	@echo "*********************************************************************************************************************"
else

check-local: runTest 
	./runTest $(srcdir)/buffer2.bin > output
	@echo "*********************************************************************************************************************"
	@echo "*** ROSE/tests/roseTests/programAnalysisTests/binaryTests: make check rule complete (terminated normally)         ***"
	@echo "*********************************************************************************************************************"

endif



clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) test* rose_*.C *.dot Templates.DB ii_files ti_files DATABASE DATABASE.dot

distclean-local:
	rm -rf Templates.DB 

else

check-local:
	@echo "*********************************************************************************************************************"
	@echo "*** ROSE/tests/roseTests/programAnalysisTests/binaryTests: skipped                                                ***"
	@echo "*********************************************************************************************************************"

endif
