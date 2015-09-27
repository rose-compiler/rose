include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TESTCODE_AM_CPPFLAGS = 

F2C_FLAGS = -rose:C_output_language 
F2C_SENTINEL_FLAGS = -f2c:sentinel

SENTINEL_TESTCODES_REQUIRED_TO_PASS = \
							   sentinel.f
BASIC_TESTCODES_REQUIRED_TO_PASS = \
							   doloop1.F \
							   doloop2.F \
							   doloop3.F \
							   implicitVariable1.F \
							   implicitVariable2.F \
							   array1.F \
							   array2.F \
							   array3.F \
							   array4.F \
							   array5.F \
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
							   equivalence8.F \
							   program1.F \
							   subroutine1.F \
							   implicitFunctionCall1.F \
							   implicitFunctionCall2.F \
							   functionArgs1.F \
							   functionArgs2.F \
							   functionArgs3.F \
							   functionArgs4.F \
							   functionArgs5.F \
							   label.F \
							   loopWithLabel.F \
							   parameter1.F \
							   parameter2.F \
							   parameter3.F \
							   type.F \
							   doubleValue.F \
							   function1.F \
							   functionCall1.F \
							   functionCall2.F \
							   implicitNone.F 


ALL_TESTCODES = \
$(BASIC_TESTCODES_REQUIRED_TO_PASS) \
$(SENTINEL_TESTCODES_REQUIRED_TO_PASS) 

PASSING_BASIC_FORTRAN_TEST_OBJECTS = \
$(BASIC_TESTCODES_REQUIRED_TO_PASS:.F=.o) 

PASSING_SENTINEL_FORTRAN_TEST_OBJECTS = \
$(SENTINEL_TESTCODES_REQUIRED_TO_PASS:.f=.o) 


# if ROSE_USE_OPEN_FORTRAN_PARSER
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT

Fortran_to_C:
	$(MAKE) -C ../src Fortran_to_C
	

$(PASSING_BASIC_FORTRAN_TEST_OBJECTS): %.o: $(srcdir)/%.F Fortran_to_C 
	../src/Fortran_to_C $(F2C_FLAGS) -c $<

$(PASSING_SENTINEL_FORTRAN_TEST_OBJECTS): %.o: $(srcdir)/%.f Fortran_to_C 
	../src/Fortran_to_C $(F2C_FLAGS) $(F2C_SENTINEL_FLAGS) -c $<



check-local:
	@echo "Test for Fortran-to-C translator."	
	@$(MAKE) $(PASSING_BASIC_FORTRAN_TEST_OBJECTS)
	@$(MAKE) $(PASSING_SENTINEL_FORTRAN_TEST_OBJECTS)
	@echo "======== make check rule for Fortran-to-C terminated normally ========"

else
check-local:
	@echo "\nROSE Fortran development tests using the LANL POP code NOT run.  To run these reconfigure with:"
	@echo "*********************************************************************************************"
	@echo "******************* make check not run here: configure --with-fortran ***********************"
	@echo "*********************************************************************************************"
endif

clean-local:	
	rm -f $(PASSING_BASIC_FORTRAN_TEST_OBJECTS) $(PASSING_SENTINEL_FORTRAN_TEST_OBJECTS) *.c *.f *.dot

EXTRA_DIST = $(ALL_TESTCODES) 

