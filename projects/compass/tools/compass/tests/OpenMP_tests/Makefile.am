include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

# bin_PROGRAMS = statements

# This transltor output source position info
# statements_SOURCES = statements.C
# statements_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)

# DQ (8/14/2004) Developer's Note:
# This permits the use of the pair of libraries -lrose -ledg OR the longer 
# list of libraries (the later option is much faster for development work).
# LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
# LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

# Turn on GNU Coverage Tool (gcov)
# CXX += -fprofile-arcs -ftest-coverage

# Configure-time automake conditional
if ROSE_USE_LONG_MAKE_CHECK_RULE

TESTCODES_REQUIRED_TO_PASS = \
   ompPrivateLockTest1.C

else

# Shorten the list of code to compile to simplify checks using "make distcheck"
# Since these file reference file in the ../Cxx_tests direcotry we have to 
# shorten this file list but not more than it is done there, else "make distcheck"
# will fail.
TESTCODES_REQUIRED_TO_PASS = \
   ompPrivateLockTest1.C

endif


TESTCODE_CURRENTLY_FAILING = 

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = $(TESTCODES_REQUIRED_TO_PASS) 

# QMTest allows both passing and failing tests.
ALL_TESTCODES = $(TESTCODES_REQUIRED_TO_PASS) $(TESTCODE_CURRENTLY_FAILING)

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
# VALGRIND = /usr/apps/valgrind/new/bin/valgrind $(VALGRIND_OPTIONS)
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
VALGRIND =

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}

## Include the path to A++ and the transformation specification
TESTCODE_INCLUDES = -I$(top_srcdir)/tests/CompileTests/A++Code -I$(top_srcdir)/src/frontend/SageIII

ROSE_FLAGS = -rose:openmp -rose:openmp:ast_only

# A number of tests require the path to the A++ include directory 
# and a number of other tests require a path to the source directory.
# JJW: Add touch command so tests will not be run more than once
# $(TEST_Objects): preprocessor $(srcdir)/$(@:.o=.C)
$(TEST_Objects): ../../compassMain ../../compass_parameters
#	$(VALGRIND) ./statements $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir)/../Cxx_tests -c $(srcdir)/../Cxx_tests/$(@:.o=.C)
#	echo "Copy the file to the local build tree so that the we can process it with compass (not required by compass) ..."
#	cp $(top_srcdir)/tests/CompileTests/Cxx_tests/$(@:.o=.C) $(@:.o=.c)
	env COMPASS_PARAMETERS=../../compass_parameters $(VALGRIND) ../../compassMain --compass:silent $(ROSE_FLAGS) -I$(top_builddir) -I$(top_srcdir)/tests/CompileTests/Cxx_tests $(TESTCODE_INCLUDES) -rose:includeFile $(@:.o=)  -c $(srcdir)/$(@:.o=.C) && touch $@

../../compass_parameters:
	cd ../..; $(MAKE) compass_parameters

../../compassMain:
	cd ../..; $(MAKE) compassMain
EXTRA_DIST = $(ALL_TESTCODES)

check-local:
# DQ (9/11/2009): Skip tests on Debian system to test ROSE (and focus first on where it works!)
if !OS_VENDOR_DEBIAN
	@echo "Tests Compass (and its checkers) using a cross-section of the ROSE OpenMP test codes..."
	@$(MAKE) $(PASSING_TEST_Objects)
endif
	@echo "*************************************************************************************************"
	@echo "****** ROSE/projects/compass/tests/OpenMP_tests: make check rule complete (terminated normally) ******"
	@echo "*************************************************************************************************"

distclean-local:
	rm -rf Makefile 

clean-local:
	rm -rf *.o *.ps *.dot *.c rose_*.C
