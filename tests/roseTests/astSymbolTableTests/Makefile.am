include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

## Don't use the repository in ../src
CXX_TEMPLATE_REPOSITORY_PATH = .

# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = testSymbolTable

# Allow development using -lrose -ledg (simpler) or using 
# long list of separate libraries (for faster development)
ROSE_DEVELOPMENT_LIBS = $(ROSE_SEPARATE_LIBS)

testSymbolTable_SOURCES      = testSymbolTable.C
testSymbolTable_LDADD        = $(LIBS_WITH_RPATH) $(ROSE_DEVELOPMENT_LIBS)

# source files don't contain anything that would be merged
# TESTCODES = $(srcdir)/performanceTest.C

test: testSymbolTable
	./testSymbolTable -c $(srcdir)/input.C

EXTRA_DIST = input.C

check-local:
	@$(MAKE) test
	@echo "************************************************************************************************"
	@echo "*** ROSE/tests/roseTests/astPerformanceTests: make check rule complete (terminated normally) ***"
	@echo "************************************************************************************************"

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files rose_*.C

distclean-local:
	rm -rf Templates.DB 


