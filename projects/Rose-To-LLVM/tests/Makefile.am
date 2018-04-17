include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status
OMP_TEST_DIR = $(top_srcdir)/tests/nonsmoke/functional/CompileTests/OpenMP_tests
C_TEST_DIR = $(top_srcdir)/tests/nonsmoke/functional/CompileTests/C_tests

##################################################################################
# These are C testcodes targeting aspects of C that might not be a subset of C++ #
##################################################################################
OMP_TESTCODES_REQUIRED_TO_PASS = \
	array_init.c \
	hello.c \
	3loops.c 

C_TESTCODES_REQUIRED_TO_PASS = \
	test2008_02.c 

OMP_PASSING_TESTCODES_BYTECODES = $(OMP_TESTCODES_REQUIRED_TO_PASS:.c=.bc)
C_PASSING_TESTCODES_BYTECODES = $(C_TESTCODES_REQUIRED_TO_PASS:.c=.bc)

roseToLLVM = $(builddir)/../src/rose2llvm
LLVM_lli = $(llvm_path)/bin/lli

$(OMP_PASSING_TESTCODES_BYTECODES): %.bc: $(OMP_TEST_DIR)/%.c $(roseToLLVM)
	@$(RTH_RUN) \
		TITLE="roseToLLVM $(notdir $<) [$@.passed]" \
		CMD="./$(roseToLLVM)$(EXEEXT) $< && $(LLVM_lli) $@" \
		$(TEST_EXIT_STATUS) $@.passed

$(C_PASSING_TESTCODES_BYTECODES): %.bc: $(C_TEST_DIR)/%.c $(roseToLLVM)
	@$(RTH_RUN) \
		TITLE="roseToLLVM $(notdir $<) [$@.passed]" \
		CMD="./$(roseToLLVM)$(EXEEXT) $< && $(LLVM_lli) $@" \
		$(TEST_EXIT_STATUS) $@.passed


check-local: 
	@echo "Tests for C tests."
	@$(MAKE) $(C_PASSING_TESTCODES_BYTECODES)
	@echo "Tests for OMP tests."
	@$(MAKE) $(OMP_PASSING_TESTCODES_BYTECODES)
	@echo "*********************************************************************************************"
	@echo "****** ROSE/projects/Rose-To-LLVM: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************"

clean-local:
	rm *.o *.bc *.passed *.failed
