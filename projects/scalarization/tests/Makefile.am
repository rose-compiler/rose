include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

DIFF=diff

FORTRAN_TESTCODES_REQUIRED_TO_PASS = \
	Ftest1.F90 \
	Ftest2.F90 \
	Ftest3.F90 \
	Ftest4.F90 

C_TESTCODES_REQUIRED_TO_PASS = \
	Ctest1.c \
	Ctest2.c 


COMPARE_CODES = \
	rose_test1.F

FORTRAN_TEST_OUTPUTS = $(FORTRAN_TESTCODES_REQUIRED_TO_PASS:.F90=)
C_TEST_OUTPUTS = $(C_TESTCODES_REQUIRED_TO_PASS:.c=)
FORTRAN_COMPARE_OUTPUTS = $(COMPARE_CODES:.F=)

../src/scalarization:
	$(MAKE) -C $(top_builddir)/projects/scalarization/src

.PHONY: $(FORTRAN_TEST_OUTPUTS) $(C_TEST_OUTPUTS)

$(FORTRAN_TEST_OUTPUTS): ../src/scalarization
	../src/scalarization $(srcdir)/$(@:=.F90)

$(C_TEST_OUTPUTS): ../src/scalarization
	../src/scalarization $(srcdir)/$(@:=.c)

$(FORTRAN_COMPARE_OUTPUTS): 
	if $(DIFF) $(top_srcdir)/projects/scalarization/tests/$(@:=.F90) $(@:=.F90); then \
		echo "scalarization: diff translated ouput: " $@;   \
	else    \
		echo "Files differ; test falied"; \
		exit 1; \
	fi

check-local:
if ROSE_BUILD_C_LANGUAGE_SUPPORT
	@$(MAKE) $(FORTRAN_TEST_OUTPUTS)
	@$(MAKE) $(C_TEST_OUTPUTS)
#	@$(MAKE) $(FORTRAN_COMPARE_OUTPUTS)
else
	@echo "Skipping tests"
endif
	@echo "***********************************************************************************************"
	@echo "*** ROSE/projects/scalarization/tests: make check rule complete (terminated normally)"
	@echo "***********************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] rose_*.F90 *.dot *.out *postprocessed.* 
EXTRA_DIST = $(FORTRAN_TESTCODES_REQUIRED_TO_PASS) $(COMPARE_CODES)
