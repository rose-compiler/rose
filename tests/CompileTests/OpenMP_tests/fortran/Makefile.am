include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

F77_TESTCODES_TO_PASS = \
    helloworld.f \
    helloworld2.f \
    jacobi.f \
    default.f \
    exampleA11f.f \
    schedule.f

F90_TESTCODES_TO_PASS = \
  paralledo.f90 \
  exampleA134f.f90 \
  collapse.f90 \
  critical.f90

# see bug 350, wrong position for C$OMP END PARALLEL
# case insensitive in symobl lookup for Fortran
# variables used in OpenMP directives before they are used in loop body
TESTCODES_CURRENTLY_FAILING = \
    helloworld0.f \
    exampleA281f.f \
    exampleA102f.f90 \
    exampleA161f.f90

ROSE_FLAGS = -rose:openmp 

TESTCODE_INCLUDES = -I$(top_srcdir)/src/frontend/SageIII

PASSING_F77_TEST_OBJECTS = $(F77_TESTCODES_TO_PASS:.f=.o)
PASSING_F90_TEST_OBJECTS = $(F90_TESTCODES_TO_PASS:.f90=.o)

$(PASSING_F77_TEST_OBJECTS): %.o:$(srcdir)/%.f ../parseOmp
	../parseOmp $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $<

$(PASSING_F90_TEST_OBJECTS): %.o:$(srcdir)/%.f90 ../parseOmp
	../parseOmp $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $<

../parseOmp:
	$(MAKE) -C ../. parseOmp
ALL_TESTCODES =\
  $(F77_TESTCODES_TO_PASS) \
  $(F90_TESTCODES_TO_PASS) \
  $(TESTCODES_CURRENTLY_FAILING) 

check-local:
	@echo "Fortran tests for ROSE OpenMP parser"
	@$(MAKE) $(PASSING_F77_TEST_OBJECTS)
	@$(MAKE) $(PASSING_F90_TEST_OBJECTS)
	@echo "****** ROSE/tests/CompilTests/OpenMP_tests/fortran: make check rule completes (terminated normally)" 
	@echo "***********************************************************************************************************"        	
EXTRA_DIST = $(ALL_TESTCODES)
clean-local:
	rm -f *.o rose_*.* *.out
	rm -rf QMTest
