include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TESTCODE_INCLUDES = 

F2C_FLAGS = -rose:C_output_language 

#####
# The source files used for testing were all Fortran *.F sources
# and had been removed previously by someone. It appeared that these
# files were translanted to C and then the tools in this project run
# on them. Since the Fortran_to_C translator is not maintained
# and has been removed from the ROSE distribution project files,
# the test files have been removed [Rasmussen, 2022.10.1].
#
TESTCODES_REQUIRED_TO_PASS =

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

