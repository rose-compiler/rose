include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

ROSE_FLAGS = -rose:verbose 0 -I$(srcdir)

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
VALGRIND =

AM_CPPFLAGS = $(ROSE_INCLUDES) 
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD    = $(ROSE_LIBS)

TEST_JOVIAL_Objects = ${JOVIAL_TESTCODES:.jovial=.o}
TEST_F90_Sources = ${JOVIAL_TESTCODES:.jovial=.jovial.passed}

PASSING_TEST_Objects = $(TEST_JOVIAL_Objects)

# Go back and build the translator we are using to test Fortran (in case make check 
# is run directly in this directory before make has been run from the root of the 
# ROSE directory structure).  This does not appear to work!
../../testTranslator:
	cd ../..; $(MAKE) testTranslator

testTranslator: ../../testTranslator

$(TEST_JOVIAL_Objects): ../../testTranslator
if ROSE_EXPERIMENTAL_JOVIAL_ROSE_CONNECTION
	$(VALGRIND) ../../../testTranslator $(ROSE_FLAGS) -rose:jovial -c $(srcdir)/$(@:.o=.jovial)
endif

test: $(srcdir)/tiny.jovial
	../../testTranslator -rose:verbose 0 -rose:jovial $(srcdir)/tiny.jovial

clean-local:
	rm -f rose_*.* *.o *.exe

check-local:
	@echo "Tests for experimental Jovial frontend."
if ROSE_EXPERIMENTAL_JOVIAL_ROSE_CONNECTION
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "***********************************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/experimental_jovial_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************************************"
else
	@echo "******************************************************************************************"
	@echo "***** Skipping tests of experimental Jovial frontend (must be configured explicitly) *****"
	@echo "******************************************************************************************"
endif

