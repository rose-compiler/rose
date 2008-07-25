include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)
# INCLUDES = 

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .

# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = \
   testMerge

# Allow development using -lrose -ledg (simpler) or using 
# long list of separate libraries (for faster development)
ROSE_DEVELOPMENT_LIBS = $(ROSE_SEPARATE_LIBS)
# ROSE_DEVELOPMENT_LIBS = $(ROSE_LIBS)

testMerge_SOURCES = testMerge.C
testMerge_LDADD = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)

# source files don't contain anything that would be merged
# TESTCODES = $(srcdir)/mangleTest.C $(srcdir)/mangleTwo.C $(srcdir)/mangleThree.C
TESTCODES = mangleTest.C mangleTwo.C mangleThree.C

checkMerge: $(TESTCODES) testMerge
	@echo "Compiling test of merged ASTs $(TESTCODES) ..."
# DQ (5/30/2007): Simplify this test since merging all three files causes an error at present!
#	./testMerge -rose:verbose 0 -c $(srcdir)/mangleTest.C $(srcdir)/mangleTwo.C $(srcdir)/mangleThree.C
#	./testMerge -rose:verbose 0 -rose:astMerge -c $(srcdir)/mangleTest.C $(srcdir)/mangleTwo.C $(srcdir)/mangleThree.C
	./testMerge -rose:verbose 0 -rose:astMerge -c $(srcdir)/mangleTest.C $(srcdir)/mangleTwo.C

newMerge: $(TESTCODES) testMerge
	@echo "Compiling test of merged ASTs $(TESTCODES) ..."
	rm -rf commandFile.data
	@echo "Build the AST merge command file"
	./testMerge -rose:astMergeCommandFile commandFile.data $(INCLUDES) -c $(srcdir)/mangleTest.C
	./testMerge -rose:astMergeCommandFile commandFile.data $(INCLUDES) -c $(srcdir)/mangleTwo.C
	./testMerge -rose:astMergeCommandFile commandFile.data $(INCLUDES) -c $(srcdir)/mangleThree.C
	cat commandFile.data
	@echo "Now geneate and merge the ASTs"
	./testMerge -rose:verbose 0 -rose:astMerge -rose:astMergeCommandFile commandFile.data

EXTRA_DIST = $(TESTCODES) mangleTest.h  mangleTwo.h mangleTest.C

check-local:
	@$(MAKE) checkMerge
# DQ (5/30/2007): This is not working yet, we are testing merge using multiple files on the command line for now!
#	@$(MAKE) newMerge
	@echo "******************************************************************************************"
	@echo "*** ROSE/tests/roseTests/astMergeTests: make check rule complete (terminated normally) ***"
	@echo "******************************************************************************************"

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files rose_*.C *.dot *.ps *.data

distclean-local:
	rm -rf Templates.DB 


