include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

##############################################
# These are Header Files Unparsing testcodes #
##############################################


TESTCODES_REQUIRED_TO_PASS = \
     test1/Simple.C
     #test2/Simple.C \
     #test3/Simple.C \
     #test4/Simple.C \
     #test5/Simple.C \
     #test6/subdir/Simple.C \
     #test7/Simple.C \
     #test8/Simple.C

TESTCODES_CURRENTLY_FAILING =

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODES_CURRENTLY_FAILING)

ROSE_FLAGS = -rose:unparseHeaderFiles --edg:no_warnings --edg:restrict -w -rose:verbose 1

PASSING_TEST_Objects = ${TESTCODES:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}


INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

bin_PROGRAMS = TestUnparseHeaders

TestUnparseHeaders_SOURCES = UnparseHeadersTransformVisitor.C TestUnparseHeaders.C

include_HEADERS = UnparseHeadersTransformVisitor.h

EXTRA_DIST = test1
#TODO: Use a wildcard instead of specifying each folder separately.
#EXTRA_DIST = test1 test2 test3 test4 test5 test6 test7 test8

# Note that although all tests are run with the same -I paths, it is fine as long as we
# ensure that the file names inside the -I paths are unique for each test.
TEST_INCLUDE_PATHS =
#TEST_INCLUDE_PATHS = -I$(srcdir)/test8/subdir

# Add the dependence upon the source file to trigger recompilation each time the makefile is run!
$(TEST_Objects): TestUnparseHeaders 
	@echo "Calling Test: "
	./TestUnparseHeaders $(TEST_INCLUDE_PATHS) $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.C)


check-local:
	@echo "Tests for unparsing header files."
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo $(MY_TESTS)
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/UnparseHeadersTests: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] *.out
	rm -rf QMTest
