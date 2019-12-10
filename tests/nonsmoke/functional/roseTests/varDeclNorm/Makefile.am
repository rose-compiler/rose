include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TESTCODES_REQUIRED_TO_PASS = \
   int_global_scope.C \
   int_function_scope.C \
   int_struct_scope.C \
   mix_global_scope.C \
   nonreal_type_template_class_scope.C

TESTCODE_CURRENTLY_FAILING = 

TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) 

ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING) 

TEST_Sources = $(addsuffix .passed, $(ALL_TESTCODES))

PASSING_TEST_Sources = $(addsuffix .passed, $(TESTCODES))


TEST_TRANSLATOR = ../../testTranslator

../../testTranslator:
	make -C ../.. testTranslator

$(TEST_Sources): %.passed: $(srcdir)/% $(TEST_TRANSLATOR) $(top_srcdir)/scripts/test_exit_status
if !ROSE_USE_INSURE
	@$(RTH_RUN) \
		CMD="$(TEST_TRANSLATOR) -rose:suppress_variable_declaration_normalization -rose:skipfinalCompileStep $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $<" \
		TITLE="varDeclNorm [$(@:.passed=)]" \
		$(top_srcdir)/scripts/test_exit_status $@
else
	$(VALGRIND) $(TEST_TRANSLATOR) -rose:suppress_variable_declaration_normalization -rose:skipfinalCompileStep $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -I$(srcdir) -c $(@:.passed=)
endif

check-local:
	@$(MAKE) $(PASSING_TEST_Sources)
	@echo "*******************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/roseTests/varDeclNorm: make check rule complete (terminated normally) ******"
	@echo "*******************************************************************************************************************"

clean-local:
	rm -f a.out *.o rose_*.[cC] roseTestProgram.C *.dot *.ti *.pdf
	rm -f *.passed *.failed

