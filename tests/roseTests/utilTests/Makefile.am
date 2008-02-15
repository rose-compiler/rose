include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .

# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = testStrings filenameSupport
testStrings_SOURCES = testStrings.C
testStrings_LDADD   = $(top_builddir)/src/util/stringSupport/libRoseStringSupport.la

filenameSupport_SOURCES = filenameSupport.C
filenameSupport_LDADD   = $(top_builddir)/src/util/stringSupport/libRoseStringSupport.la


testFilenameSupport: filenameSupport
	./filenameSupport

# EXTRA_DIST = $(TESTCODES)

check-local: $(TEST_Objects)
	@echo "Executing ROSE string function tests ..."
	./testStrings
	./filenameSupport
	@echo "********************************************************************************************"
	@echo "****** ROSE/tests/roseTests/utilTests: make check rule complete (terminated normally) ******"
	@echo "********************************************************************************************"

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files

distclean-local:
	rm -rf Templates.DB








