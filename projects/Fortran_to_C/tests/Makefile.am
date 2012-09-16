include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TESTCODE_INCLUDES = 

F2C_FLAGS = -rose:C_output_language 

TESTCODES_REQUIRED_TO_PASS = \
							   doloop1.F \
							   doloop2.F \
							   doloop3.F \
							   implicitVariable1.F \
							   implicitVariable2.F \
							   array1.F \
							   array2.F \
							   array3.F \
							   array4.F \
							   declarationList.F \
							   dimension1.F \
							   dimension2.F \
							   equivalence1.F \
							   equivalence2.F \
							   equivalence3.F \
							   equivalence4.F \
							   equivalence5.F \
							   equivalence6.F \
							   equivalence7.F \
							   program1.F \
							   subroutine1.F \
							   implicitFunctionCall1.F \
							   implicitFunctionCall2.F \
							   functionArgs1.F \
							   functionArgs2.F \
							   function1.F


ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) 

PASSING_FORTRAN_TEST_OBJECTS = $(TESTCODES_REQUIRED_TO_PASS:.F=.o)

# if ROSE_USE_OPEN_FORTRAN_PARSER
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT

Fortran_to_C:
	$(MAKE) -C ../src Fortran_to_C
	

$(PASSING_FORTRAN_TEST_OBJECTS): %.o: $(srcdir)/%.F Fortran_to_C 
	../src/Fortran_to_C $(F2C_FLAGS) -c $<


check-local:
	@echo "Test for Fortran-to-C translator."	
	@$(MAKE) $(PASSING_FORTRAN_TEST_OBJECTS)
	@echo "======== make check rule for Fortran-to-C terminated normally ========"

else
check-local:
	@echo "\nROSE Fortran development tests using the LANL POP code NOT run.  To run these reconfigure with:"
	@echo "*********************************************************************************************"
	@echo "******************* make check not run here: configure --with-fortran ***********************"
	@echo "*********************************************************************************************"
endif

clean-local:	
	rm -f $(PASSING_FORTRAN_TEST_OBJECTS) *.c *.f *.dot

EXTRA_DIST = $(ALL_TESTCODES) 

