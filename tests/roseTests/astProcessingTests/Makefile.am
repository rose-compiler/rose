include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


EXTRA_DIST = input1.C test11.C test12.C test13.C test14.C t1 t2 t3 t4

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .
# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = astTraversalTest processnew3Down4SgIncGraph processnew3Down4 binaryPaths

# Allow development using -lrose -ledg (simpler) or using 
# long list of separate libraries (for faster development)
ROSE_DEVELOPMENT_LIBS = $(ROSE_SEPARATE_LIBS)

astTraversalTest_SOURCES      = astTraversalTest.C
astTraversalTest_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)

processnew3Down4_SOURCES      = processnew3Down4.C
processnew3Down4_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)


processnew3Down4SgIncGraph_SOURCES      = processnew3Down4SgIncGraph.C
processnew3Down4SgIncGraph_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)

binaryPaths_SOURCES = binaryPaths.C
binaryPaths_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)

# source files don't contain anything that would be merged
# TESTCODES = $(srcdir)/performanceTest.C

testTraversals: astTraversalTest
	./astTraversalTest -edg:w -c $(srcdir)/input1.C

testRunExamples: processnew3Down4
	./processnew3Down4 $(srcdir)/test11.C
	./processnew3Down4 $(srcdir)/test12.C
	./processnew3Down4 $(srcdir)/test13.C
	./processnew3Down4 $(srcdir)/test14.C
	rm *.dot
testRunExamples2: processnew3Down4SgIncGraph
	./processnew3Down4SgIncGraph $(srcdir)/test11.C
	./processnew3Down4SgIncGraph $(srcdir)/test12.C
	./processnew3Down4SgIncGraph $(srcdir)/test13.C
	./processnew3Down4SgIncGraph $(srcdir)/test14.C
	rm *.dot
testRunExamplesBin: binaryPaths
	./binaryPaths $(srcdir)/t1
	./binaryPaths $(srcdir)/t2
	./binaryPaths $(srcdir)/t3
	./binaryPaths $(srcdir)/t4
	rm *.dot
check-local:
	@$(MAKE) testTraversals
	@$(MAKE) testRunExamples
	@$(MAKE) testRunExamples2
	@echo "***********************************************************************************************"
	@echo "*** ROSE/tests/roseTests/astProcessingTests: make check rule complete (terminated normally) ***"
	@echo "***********************************************************************************************"

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files rose_*.C

distclean-local:
	rm -rf Templates.DB 


