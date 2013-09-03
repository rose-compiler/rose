include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (6/8/2013): Use command line option to alternatively test the experimental fortran frontend.
ROSE_FLAGS = -rose:verbose 0 -rose:experimental_fortran_frontend -I$(srcdir)

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
VALGRIND =

TEST_F90_Objects = main_program_2008_01.f90

PASSING_TEST_Objects = $(TEST_F90_Objects)

$(TEST_F90_Objects): ../../../testTranslator
	$(VALGRIND) ../../../testTranslator $(ROSE_FLAGS) -rose:f90 -c $(srcdir)/$(@:.o=.f90)

check-local:
	@echo "Tests for experimental Fortran frontend."
if ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "*******************************************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/Fortran_tests/experimental_frontend_tests: make check rule complete (terminated normally) ******"
	@echo "*******************************************************************************************************************************"
else
	@echo "*******************************************************************************************"
	@echo "***** Skipping tests of experimental Fortran frontend (must be configured explicitly) *****"
	@echo "*******************************************************************************************"
endif

