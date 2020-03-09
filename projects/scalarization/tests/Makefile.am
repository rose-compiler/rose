include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

DIFF=diff
REFERENCE_PATH=$(srcdir)/tests/references

FORTRAN_TESTCODES_REQUIRED_TO_PASS = \
	Ftest1.f90 \
	Ftest2.f90 \
	Ftest3.f90 \
	Ftest4.f90 \
	Ftest5.f90 

C_TESTCODES_REQUIRED_TO_PASS = \
	Ctest1.c \
	Ctest2.c 


COMPARE_CODES = \
	rose_Ftest1.f90 \
	rose_Ftest2.f90 \
	rose_Ftest3.f90 \
	rose_Ftest4.f90 \
	rose_Ftest5.f90 \
	rose_Ctest1.c \
	rose_Ctest2.c 

FORTRAN_TEST_OUTPUTS = $(FORTRAN_TESTCODES_REQUIRED_TO_PASS:.f90=)
C_TEST_OUTPUTS = $(C_TESTCODES_REQUIRED_TO_PASS:.c=)
FORTRAN_COMPARE_OUTPUTS = $(COMPARE_FORTRAN_CODES:.f90=)
C_COMPARE_OUTPUTS = $(COMPARE_C_CODES:.c=)

../src/rose-scalarizer:
	$(MAKE) -C $(top_builddir)/projects/scalarization/src

.PHONY: $(FORTRAN_TEST_OUTPUTS) $(C_TEST_OUTPUTS)

$(FORTRAN_TEST_OUTPUTS): ../src/rose-scalarizer
	../src/rose-scalarizer $(srcdir)/$(@:=.f90)

$(C_TEST_OUTPUTS): ../src/rose-scalarizer
	../src/rose-scalarizer $(srcdir)/$(@:=.c)

$(FORTRAN_COMPARE_OUTPUTS): 
	if $(DIFF) $(srcdir)/references/$(@:=.f90) $(@:=.f90) > $@; then echo "$@ Test Passed" ; else echo "Files differ; test falied"; cat $@; rm -rf $@; exit 1; fi

$(C_COMPARE_OUTPUTS): 
	if $(DIFF) $(srcdir)/references/$(@:=.c) $(@:=.c) > $@; then echo "$@ Test Passed" ; else echo "Files differ; test falied"; cat $@; rm -rf $@; exit 1; fi

check-local:
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
	@$(MAKE) $(FORTRAN_TEST_OUTPUTS)
	@$(MAKE) $(FORTRAN_COMPARE_OUTPUTS)
else
	@echo "Skipping Fortran tests"
endif
if ROSE_BUILD_C_LANGUAGE_SUPPORT
	@$(MAKE) $(C_TEST_OUTPUTS)
	@$(MAKE) $(C_COMPARE_OUTPUTS)
else
	@echo "Skipping C tests"
endif
	@echo "***********************************************************************************************"
	@echo "*** ROSE/projects/scalarization/tests: make check rule complete (terminated normally)"
	@echo "***********************************************************************************************"

clean-local:
	rm -f *.o rose_* *.dot *.out *.mod *.rmod *postprocessed.* 
EXTRA_DIST = $(FORTRAN_TESTCODES_REQUIRED_TO_PASS) $(COMPARE_CODES)
