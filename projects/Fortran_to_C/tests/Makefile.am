include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TESTCODE_INCLUDES = 

F2C_FLAGS = -rose:C_output_language 

TESTCODES_REQUIRED_TO_PASS = \
							   program1.F \
							   subroutine1.F \
							   function1.F


ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) 

PASSING_FORTRAN_TEST_OBJECTS = $(TESTCODES_REQUIRED_TO_PASS:.F=.o)

Fortran_to_C:
	$(MAKE) -C ../src Fortran_to_C
	

$(PASSING_FORTRAN_TEST_OBJECTS): %.o: $(srcdir)/%.F Fortran_to_C 
	../src/Fortran_to_C $(F2C_FLAGS) -c $<

check-local:
	@echo "Test for Fortran-to-C translator."	
	@$(MAKE) $(PASSING_FORTRAN_TEST_OBJECTS)
	@echo "======== make check rule for Fortran-to-C terminated normally ========"

clean-local:	
	rm -f $(PASSING_FORTRAN_TEST_OBJECTS) *.C *.f *.dot

EXTRA_DIST = $(ALL_TESTCODES) README

