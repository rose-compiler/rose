include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .

# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = testStrings filenameSupport testFileNameClassifier directorySupport

testFileNameClassifier_SOURCES = testFileNameClassifier.C
#testFileNameClassifier_LDADD   = $(top_builddir)/src/util/stringSupport/libRoseStringSupport.la
testFileNameClassifier_LDADD   = $(LIBS_WITH_RPATH) $(ROSE_LIBS)


testStrings_SOURCES = testStrings.C
testStrings_LDADD   = $(top_builddir)/src/util/stringSupport/libRoseStringSupport.la

filenameSupport_SOURCES = filenameSupport.C
filenameSupport_LDADD   = $(top_builddir)/src/util/stringSupport/libRoseStringSupport.la

# Added test for ROSE directory support (SgDirectory IR node)
directorySupport_SOURCES = directorySupport.C
directorySupport_LDADD   = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

testFilenameSupport: filenameSupport
	./filenameSupport

testDirectorySupport: directorySupport $(srcdir)/input_directorySupport.C
	./directorySupport -c -rose:verbose 0 $(srcdir)/input_directorySupport.C

# EXTRA_DIST = $(TESTCODES)
EXTRA_DIST = input_directorySupport.C

# DQ (3/21/2009): testFileNameClassifier fails to run under Cygwin.
if ROSE_BUILD_OS_IS_CYGWIN
check-local: $(TEST_Objects)
	@echo "Executing ROSE string function tests ..."
	./testStrings
#	./testFileNameClassifier
	./filenameSupport
	@echo "********************************************************************************************"
	@echo "****** ROSE/tests/roseTests/utilTests: make check rule complete (terminated normally) ******"
	@echo "********************************************************************************************"
else
check-local: $(TEST_Objects)
	@echo "Executing ROSE string function tests ..."
	./testStrings
	./testFileNameClassifier
	./filenameSupport
	$(MAKE) testDirectorySupport
	@echo "********************************************************************************************"
	@echo "****** ROSE/tests/roseTests/utilTests: make check rule complete (terminated normally) ******"
	@echo "********************************************************************************************"
endif

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files
	rm -f rose_*.C *.dot
	rm -rf xxx_new_directory_node_support

distclean-local:
	rm -rf Templates.DB








