include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

ROSE_FLAGS = -rose:verbose 0 -I$(srcdir)

AM_CPPFLAGS = $(ROSE_INCLUDES) 
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD    = $(ROSE_LIBS)

FORTRAN_TESTCODES = \
  tiny.f90

TEST_FORTRAN_Objects = ${FORTRAN_TESTCODES:.f90=.o}
TEST_FORTRAN_Sources = ${FORTRAN_TESTCODES:.f90=.f90.passed}

PASSING_TEST_Objects = $(TEST_FORTRAN_Objects)

../../testTranslagor:
	cd ../..; $(MAKE) testTranslator

$(TEST_FORTRAN_Objects): ../../testTranslator
	../../testTranslator -rose:experimental_flang_frontend -c $(srcdir)/$(@:.o=.f90)
#    ## make sure output file is not empty
	@test -n "$$(find rose_$(@:.o=.f90) -size +0c)"

check-fortran:
	@echo "TEST_FORTRAN_Objects:"
	@echo $(TEST_FORTRAN_Objects)
	@echo "PASSING_FORTRAN_Objects:"
	@echo $(PASSING_TEST_Objects)

clean-local:
	rm -f rose_*.*

check-local:
	@echo "Tests for experimental Fortran frontend."
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "***********************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/experimental_fortran_tests: make check completed successfully ******"
	@echo "***********************************************************************************************************************"
	@echo "*************************************************************************************************"
	@echo "***** Skipping tests of experimental Fortran clang frontend (must be configured explicitly) *****"
	@echo "*************************************************************************************************"
