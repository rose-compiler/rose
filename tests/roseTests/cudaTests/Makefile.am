include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST = ex.c cuda_traversal.hpp

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

# This test program does not require the rest of ROSE so it can be handled locally
bin_PROGRAMS  = runTest

runTest_SOURCES = runTest.C

runTest_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

ROSE_FLAGS = -rose:verbose 0

TEST_TRANSLATOR = ./runTest all $(ROSE_FLAGS)
TESTCODE_INCLUDES =

CLEANFILES = rose_*.C rose_*.c output

# DQ (7/25/2005): The current changes to the AST break these test programs:
check-local: runTest 
	./runTest $(srcdir)/ex.c > output
	@echo "*********************************************************************************************************************"
	@echo "*** ROSE/tests/roseTests/cudaTests/runTest: make check rule complete (terminated normally) ***"
	@echo "*********************************************************************************************************************"

clean-local:
	rm -rf test* rose_*.C rose_*.c

distclean-local:
	rm -rf rose_*.C rose_*.c
