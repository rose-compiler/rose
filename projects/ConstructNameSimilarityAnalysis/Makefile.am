include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

bin_PROGRAMS = nameTests

# This transltor output source position info
nameTests_SOURCES = nameTests.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

# This is a shorted list that tests more quickly.
TESTCODES_REQUIRED_TO_PASS = \
test2001_01.C 

# DQ (8/28/2006): Verified non-working code
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
TESTCODE_INCLUDES = -I$(top_srcdir)/tests/CompileTests/A++Code

ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0

# A number of tests require the path to the A++ include directory 
# and a number of other tests require a path to the source directory.
# $(TEST_Objects): preprocessor $(srcdir)/$(@:.o=.C)
$(TEST_Objects): nameTests
#	$(VALGRIND) ./statements $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir)/../Cxx_tests -c $(srcdir)/../Cxx_tests/$(@:.o=.C)
	echo "Copy the file to the local build tree so that the tview program can access it with the generated .highlight file ..."
	cp $(srcdir)/../Cxx_tests/$(@:.o=.C) $(@:.o=.C)
	$(VALGRIND) ./nameTests $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir)/../Cxx_tests -c $(@:.o=.C)

# This was a bug specific to the normalization of source file names to absolute paths
testNames: nameTests
	./nameTests -rose:verbose 0 -c $(srcdir)/input_nameTests.C

check-local:
	@$(MAKE) testNames
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/projects/ConstructNameSimilarityAnalysis: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

distclean-local:
	rm -rf Makefile

clean-local:
	rm -rf *.o *.ps *.dot rose_*.C rose_*.c core *.int.c a.out *.highlight test*_*.C

cleandata:
	rm -f *.ti *.ii rose_*.C rose_*.c *.dot test*.o rose_*.o



EXTRA_DIST = CMakeLists.txt
