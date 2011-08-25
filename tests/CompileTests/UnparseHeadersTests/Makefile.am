include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

##############################################
# These are Header Files Unparsing testcodes #
##############################################


TESTCODES_REQUIRED_TO_PASS = \
     test1/Simple1.C \
     test2/Simple2.C \
     test3/Simple3.C \
     test4/Simple4.C \
     test5/Simple5.C \
     test6/subdir/Simple6.C \
     test7/Simple7.C \
     test8/Simple8.C \
     test9/Simple9.C \
     test10/Simple10.C \
     test11/Simple11.C \
     test12/Simple12.C \
     test13/Simple13.C

TESTCODES_CURRENTLY_FAILING =

TESTCODES_FOR_DEFAULT_UNPARSE_FOLDER = \
     test0/Simple0.C

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODES_CURRENTLY_FAILING)


PASSING_TEST_Objects = ${TESTCODES:.C=.o}
DEFAULT_UNPARSE_FOLDER_TEST_Objects = ${TESTCODES_FOR_DEFAULT_UNPARSE_FOLDER:.C=.o}
TEST_Objects = ${ALL_TESTCODES:.C=.o}


INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

bin_PROGRAMS = TestUnparseHeaders

TestUnparseHeaders_SOURCES = UnparseHeadersTransformVisitor.C TestUnparseHeaders.C

include_HEADERS = UnparseHeadersTransformVisitor.h

EXTRA_DIST = test0 test1 test2 test3 test4 test5 test6 test7 test8 test9 test10 test11 test12 test13

# Note that although all tests are run with the same -I paths, it is fine as long as we
# ensure that the file names inside the -I paths are unique for each test.
TEST_INCLUDE_PATHS = -I$(srcdir)/test8/subdir

#This gets the root folder name of each test (i.e. test1, test2, etc.), where $(dir $@) gets the containing
#directory of each test, the inner $(patsubst... ) removes 'subdir/' part (which is needed for the test number 6),
#and the outer $(patsubst... ) removes the trailing file path delimiter (i.e. '/').    
TestDir = $(patsubst %/,%,$(patsubst %subdir/,%,$(dir $@)))

ROSE_FLAGS = -rose:unparseHeaderFiles -rose:unparseHeaderFilesRootFolder $(TestDir)_unparsedHeaders --edg:no_warnings --edg:restrict -w -rose:verbose 1

DEFAULT_UNPARSE_FOLDER_ROSE_FLAGS = -rose:unparseHeaderFiles --edg:no_warnings --edg:restrict -w -rose:verbose 1

# Add the dependence upon the source file to trigger recompilation each time the makefile is run!
$(TEST_Objects): TestUnparseHeaders 
	@echo "Calling Test: "
	./TestUnparseHeaders $(TEST_INCLUDE_PATHS) $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.C)

$(DEFAULT_UNPARSE_FOLDER_TEST_Objects): TestUnparseHeaders 
	@echo "Calling Test: "
	./TestUnparseHeaders $(DEFAULT_UNPARSE_FOLDER_ROSE_FLAGS) -c $(srcdir)/$(@:.o=.C)

check-local:
	@echo "Tests for unparsing header files."
#Remove folders, to which modified header files are unparsed, in order to avoid problems on rerunning 
#'make check' due to already existing not empty unparse folders.
	rm -rf test*
	@$(MAKE) $(DEFAULT_UNPARSE_FOLDER_TEST_Objects)
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo $(MY_TESTS)
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/UnparseHeadersTests: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] *.out
	rm -rf test* _rose_unparsed_headers_

#At some point in past I thought I had to add this to avoid a problem with dist-check, which complained about a not removed directory.
#distclean-local:
#	rm -rf test* _rose_unparsed_headers_
