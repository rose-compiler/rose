include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

DIFF=diff


C_TESTCODES_REQUIRED_TO_PASS = \
	zforpBitstream.c 


COMPARE_CODES = \
	rose_test1.F

C_TEST_OUTPUTS = $(C_TESTCODES_REQUIRED_TO_PASS:.c=)

../src/ZFPTool:
	$(MAKE) -C $(top_builddir)/projects/ZFPTool/src

.PHONY: $(FORTRAN_TEST_OUTPUTS) $(C_TEST_OUTPUTS)


$(C_TEST_OUTPUTS): ../src/ZFPTool
	../src/ZFPTool $(srcdir)/$(@:=.c)


check-local:
if ROSE_BUILD_C_LANGUAGE_SUPPORT
	@$(MAKE) $(C_TEST_OUTPUTS)
else
	@echo "Skipping tests"
endif
	@echo "***********************************************************************************************"
	@echo "*** ROSE/projects/ZFPTool/tests: make check rule complete (terminated normally)"
	@echo "***********************************************************************************************"

clean-local:
	rm -f *.o rose_*.[cC] *.dot *.out *postprocessed.* 
EXTRA_DIST = $(FORTRAN_TESTCODES_REQUIRED_TO_PASS) $(COMPARE_CODES)
