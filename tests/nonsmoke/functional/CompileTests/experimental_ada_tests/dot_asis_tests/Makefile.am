include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

ADA_MAIN=$(top_builddir)/src/frontend/Experimental_Ada_ROSE_Connection/dot_asis/ada_main

$(ADA_MAIN)/obj/run_asis_tool_2:
	cd $(ADA_MAIN); $(MAKE) run_asis_tool_2

RUN_ASIS_TOOL=$(ADA_MAIN)/obj/run_asis_tool_2
GNAT_HOME=$(shell dirname $(shell dirname $(shell which gprbuild)))

ASIS_TESTS_DIR = $(srcdir)/test_units
# ASIS_SPEC_TESTS := $(shell find $(ASIS_TESTS_DIR) -name '*.ads')

ASIS_SPEC_FAILED_TESTS := \
	all_modes.ads 

ASIS_SPEC_PASS_TESTS := \
	component_declaration.ads \
	constant_declaration.ads \
	derived_record.ads \
	empty_package.ads \
	entry_declaration.ads \
	exception_declaration.ads \
	function_declaration.ads \
	ordinary_type_declaration.ads \
	package_with_body.ads \
	parameter_declaration.ads \
	procedure_declaration.ads \
	selected_component.ads \
	simple_expression_range.ads \
	tagged_definition.ads \
	task_type_declaration.ads \
	task_with_body.ads \
	test_unit.ads \
	variable_declaration.ads \
	unit_2.ads 

ASIS_SPEC_TESTS = $(ASIS_SPEC_PASS_TESTS) $(ASIS_SPEC_FAILED_TESTS)


#ASIS_BODY_TESTS := $(shell find $(ASIS_TESTS_DIR) -name '*.adb')

ASIS_BODY_FAILED_TESTS := \
	asm_machine_code.adb \
	for_loop_statement.adb \
	requeue_statements.adb \
        pragma.adb 

ASIS_BODY_PASS_TESTS := \
	accept_statement.adb \
	block_statement.adb \
	case_statement.adb \
	choice_parameter_specification.adb \
	delay_relative.adb \
	delay_until.adb \
	entry_call.adb \
	exit_statement.adb \
	goto_loop.adb \
	function_call.adb \
	function_declaration.adb \
	hello_world.adb \
	if_statement.adb \
	loop_statement.adb \
	minimal.adb \
	operator.adb \
	procedure_call.adb \
	package_with_body.adb \
	procedure_declaration.adb \
	raise_statement.adb \
	select_accept.adb \
	task_with_abort.adb \
	task_with_body.adb \
	task_with_body.adb \
	test_unit.adb \
	type_conversion.adb \
	unit_2.adb \
	while_loop_statement.adb \
        array_declare.adb \
        variable_variant_record.adb \
        discriminant.adb 

ASIS_BODY_TESTS = $(ASIS_BODY_PASS_TESTS) $(ASIS_BODY_FAILED_TESTS) 

ASIS_TESTS = $(ASIS_SPEC_TESTS) $(ASIS_BODY_TESTS)

#DOT_FILES = $(addsuffix .dot,$(ASIS_TESTS))
DOT_SPEC_PASS_FILES = ${ASIS_SPEC_PASS_TESTS:.ads=.ads.dot}
DOT_BODY_PASS_FILES = ${ASIS_BODY_PASS_TESTS:.adb=.adb.dot}

DOT_FILES = $(DOT_SPEC_PASS_FILES) $(DOT_BODY_PASS_FILES)

$(DOT_SPEC_PASS_FILES): %.ads.dot: $(ASIS_TESTS_DIR)/%.ads  $(RUN_ASIS_TOOL)
	@echo "Testing spec file:" $<
	$(RUN_ASIS_TOOL) --file=$< --gnat_home=$(GNAT_HOME) --output_dir=$(builddir) 2> /dev/null | (! grep -i "ASIS Error") || (echo "$< failed $?"; exit 1)

$(DOT_BODY_PASS_FILES): %.adb.dot: $(ASIS_TESTS_DIR)/%.adb  $(RUN_ASIS_TOOL)
	@echo "Testing body file:" $<
	$(RUN_ASIS_TOOL) --file=$< --gnat_home=$(GNAT_HOME) --output_dir=$(builddir) 2> /dev/null | (! grep -i "ASIS Error") || (echo "$< failed $?"; exit 1)

PASSING_DOT_DIFF=$(addsuffix .diff, $(DOT_FILES))
REFERENCE_PATH=$(top_srcdir)/tests/nonsmoke/functional/CompileTests/experimental_ada_tests/dot_asis_tests/referecing_dot_output

$(PASSING_DOT_DIFF): %.dot.diff: %.dot
	@echo "Comparing dot file:" $<
	if diff $(@:.dot.diff=.dot) $(REFERENCE_PATH)/$(@:.dot.diff=.dot) > $@ ; then echo "$< passed" ; else echo "$< differ; test falied"; rm -rf $@; exit 1; fi

print:
	@echo "Output Makefile variables:"
	@echo "ASIS_SPEC_TESTS = $(ASIS_SPEC_TESTS)"
	@echo "ASIS_BODY_TESTS = $(ASIS_BODY_TESTS)"
	@echo "DOT_SPEC_PASS_FILES = $(DOT_SPEC_PASS_FILES)"
	@echo "DOT_BODY_PASS_FILES = $(DOT_BODY_PASS_FILES)"

check-local: 
	@echo "Tests ASIS."
	@$(MAKE) $(DOT_FILES)
	@echo "Compare Dot files."
	@$(MAKE) $(PASSING_DOT_DIFF)

.PHONY: clean-local
clean-local:
	rm -rf obj *.dot *.dot.diff

