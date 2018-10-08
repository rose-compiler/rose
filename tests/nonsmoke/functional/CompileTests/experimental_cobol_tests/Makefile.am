include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (8/23/2017): Use command line option to alternatively test the experimental fortran frontend.
ROSE_FLAGS = -rose:verbose 0 -I$(srcdir)

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
VALGRIND =

AM_CPPFLAGS = $(ROSE_INCLUDES) 
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD    = $(ROSE_LIBS)

TEST_COBOL_Objects = ${COBOL_TESTCODES:.cob=.o}
TEST_F90_Sources = ${COBOL_TESTCODES:.cob=.cob.passed}

PASSING_TEST_Objects = $(TEST_COBOL_Objects)

# Go back and build the translator we are using to test Fortran (in case make check 
# is run directly in this directory before make has been run from the root of the 
# ROSE directory structure).  This does not appear to work!
../../testTranslator:
	cd ../..; $(MAKE) testTranslator

testTranslator: ../../testTranslator

$(TEST_COBOL_Objects): ../../testTranslator
if ROSE_EXPERIMENTAL_COBOL_ROSE_CONNECTION
	$(VALGRIND) ../../../testTranslator $(ROSE_FLAGS) -rose:cobol -c $(srcdir)/$(@:.o=.cobol)
endif

test: $(srcdir)/display_reals.cob
	../../testTranslator -rose:verbose 0 -rose:cobol $(srcdir)/display_reals.cob

clean-local:
	rm -f rose_*.* *.o *.exe

check-local:
	@echo "Tests for experimental Cobol frontend."
if ROSE_EXPERIMENTAL_COBOL_ROSE_CONNECTION
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "***********************************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/experimental_cobol_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************************************"
else
	@echo "******************************************************************************************"
	@echo "***** Skipping tests of experimental cobol frontend (must be configured explicitly) *****"
	@echo "******************************************************************************************"
endif

