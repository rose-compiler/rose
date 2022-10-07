include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

ROSE_FLAGS = -rose:verbose 0 -I$(srcdir)

AM_CPPFLAGS = $(ROSE_INCLUDES) 
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD    = $(ROSE_LIBS)

FORTRAN_TESTCODES =               \
  tiny.f90


FORTRAN_TESTCODES_TODO =          \
  binary_expressions.f90          \
  case_1.f90                      \
  common_1.f90                    \
  common_2.f90                    \
  dowhile.f90                     \
  function_2.f90                  \
  function_3.f90                  \
  function_4.f90                  \
  ifstmt1.f90                     \
  ifstmt2.f90                     \
  ifstmt3.f90                     \
  ifstmt4.f90                     \
  ifstmt5.f90                     \
  implicit_none.f90               \
  implicit_none_external.f90      \
  implicit_none_external_type.f90 \
  implicit_none_type.f90          \
  negative_values.f90             \
  print1.f90                      \
  subroutine_1.f90                \
  type_decls_1.f90                \
  type_decls_2.f90                \
  type_decls_3.f90                \
  type_def_1.f90


TEST_FORTRAN_Objects = ${FORTRAN_TESTCODES:.f90=.o}
TEST_FORTRAN_Sources = ${FORTRAN_TESTCODES:.f90=.f90.passed}

PASSING_TEST_Objects = $(TEST_FORTRAN_Objects)

../../testTranslator:
	cd ../..; $(MAKE) testTranslator

$(TEST_FORTRAN_Objects): ../../testTranslator
	../../testTranslator -rose:experimental_flang_frontend -c $(srcdir)/$(@:.o=.f90)
#    ## make sure output file is not empty
	@test -n "$$(find rose_$(@:.o=.f90) -size +0c)"

check-fortran:
	@echo "TEST_FORTRAN_Sources:"
	@echo $(TEST_FORTRAN_Sources)
	@echo "PASSING_FORTRAN_Objects:"
	@echo $(PASSING_TEST_Objects)

clean-local:
	rm -f rose_*.* *.o

check-local:
	@echo "Tests for experimental Fortran frontend."
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "***********************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/experimental_fortran_tests: make check completed successfully ******"
	@echo "***********************************************************************************************************************"
