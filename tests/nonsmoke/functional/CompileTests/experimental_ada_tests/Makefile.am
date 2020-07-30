include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (8/23/2017): Use command line option to alternatively test the experimental fortran frontend.
# ROSE_FLAGS = -rose:verbose 0 -rose:experimental_csharp_frontend -I$(srcdir)
ROSE_FLAGS    ?= -rose:skipfinalCompileStep
ROSE_COMPILER ?= $(top_builddir)/src/rose-compiler


#VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
#VALGRIND =


# Go back and build the translator we are using to test Fortran (in case make check
# is run directly in this directory before make has been run from the root of the
# ROSE directory structure).  This does not appear to work!


# $(TEST_CSHARP_Objects): ../../testTranslator
# if ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION
#	$(VALGRIND) ../../../testTranslator $(ROSE_FLAGS) -rose:cs -c $(srcdir)/$(@:.o=.cs)
# endif


# TESTCODES_REQUIRED_TO_PASS = minimal.adb empty_package.ads ordinary_type_declaration.ads task_type_declaration.ads variable_declaration.ads \
#   constant_declaration.ads component_declaration.ads procedure_declaration.ads function_declaration.ads parameter_declaration.ads \
#   all_modes.ads procedure_declaration.adb function_declaration.adb package_with_body.adb task_with_body.adb entry_declaration.ads \
#   exception_declaration.ads choice_parameter_specification.adb tagged_definition.ads derived_record.ads simple_expression_range.ads \
#   function_call.adb operator.adb selected_component.ads type_conversion.adb if_statement.adb accept_statement.adb entry_call.adb \
#   loop_statement.adb procedure_call.adb raise_statement.adb

# TESTCODES_REQUIRED_TO_PASS = empty_package.ads

ORIGINAL_FILE_LIST := minimal.adb empty_package.ads ordinary_type_declaration.ads task_type_declaration.ads variable_declaration.ads \
   constant_declaration.ads component_declaration.ads procedure_declaration.ads function_declaration.ads parameter_declaration.ads \
   all_modes.ads procedure_declaration.adb function_declaration.adb package_with_body.adb task_with_body.adb entry_declaration.ads \
   exception_declaration.ads choice_parameter_specification.adb tagged_definition.ads derived_record.ads simple_expression_range.ads \
   function_call.adb operator.adb selected_component.ads type_conversion.adb if_statement.adb accept_statement.adb entry_call.adb \
   loop_statement.adb procedure_call.adb raise_statement.adb

PASSING_TESTS :=                       \
  pkgrename.ads                        \
  accept_statement.adb                 \
  all_modes.ads                        \
  case_function.adb                    \
  case_function.ads                    \
  choice_parameter_specification.adb   \
  component_declaration.ads            \
  constant_declaration.ads             \
  constants.ads                        \
  derived_record.ads                   \
  empty_package.ads                    \
  entry_call.adb                       \
  entry_declaration.ads                \
  exception_declaration.ads            \
  fib.adb                              \
  for_loop.adb                         \
  function_call.adb                    \
  function_declaration.adb             \
  function_declaration.ads             \
  gototest.adb                         \
  gototest.ads                         \
  hello_world.adb                      \
  if_statement.adb                     \
  iterateRange.adb                     \
  loop_exit.adb                        \
  loop_statement.adb                   \
  minimal.adb                          \
  nested_package.ads                   \
  operator.adb                         \
  ordinary_type_declaration.ads        \
  package_with_body.adb                \
  package_with_body.ads                \
  parameter_declaration.ads            \
  procedure_call.adb                   \
  procedure_declaration.adb            \
  procedure_declaration.ads            \
  raise_statement.adb                  \
  real_typedecl.ads                    \
  recursive_call.adb                   \
  selected_component.ads               \
  short_circuit.adb                    \
  simple_expression_range.ads          \
  some_modes.ads                       \
  subtypes.ads                         \
  tagged_definition.ads                \
  task_type_declaration.ads            \
  task_with_body.adb                   \
  task_with_body.ads                   \
  test.adb                             \
  test.ads                             \
  test_unit.adb                        \
  test_unit.ads                        \
  type_conversion.adb                  \
  unit_1.adb                           \
  unit_2.adb                           \
  unit_2.ads                           \
  unit_3.ads                           \
  variable_declaration.ads             \
  while_loop_statement.adb             
  
# PASSING_TESTS   

INCOMPLETE_FRONTEND := 

# INCOMPLETE_FRONTEND

INCOMPLETE_ASIS := 

# INCOMPLETE_ASIS

#
# define test suite
TEST_TARGETS := $(PASSING_TESTS)
ALL_TEST_TARGETS := $(PASSING_TESTS) $(INCOMPLETE_FRONTEND) $(INCOMPLETE_ASIS)

#
# targets

.PHONY: all
all: $(ALL_TEST_TARGETS)

.PHONY: passing
passing: $(TEST_TARGETS)

.PHONY: print
print:
	@echo "Output Makefile variables:"
	@echo "PASSING_TEST_Objects = $(PASSING_TEST_Objects)"
	@echo "TEST_Source_passed   = $(TEST_Source_passed)"
	@echo "TEST_Objects_passed  = $(TEST_Objects_passed)"

$(top_builddir)/src/rose-compiler:
	$(MAKE) -C $(top_builddir)/src rose-compiler
	
%.ads:$(abs_srcdir)/tests/%.ads
	$(ROSE_COMPILER) $(ROSE_FLAGS) $<
	
%.adb:$(abs_srcdir)/tests/%.adb
	$(ROSE_COMPILER) $(ROSE_FLAGS) $<

.PHONY: clean-local
clean-local:
	rm -f *.adb *.ads *.ali tests/obj/* *.dot

.PHONY: check-local
check-local:
	@echo "Tests for experimental Ada frontend."
if ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION
	@$(MAKE) $(PASSING_TEST_Objects_ads)
	@$(MAKE) $(PASSING_TEST_Objects_adb)
	@echo "***********************************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/experimental_ada_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************************************"
else
	@echo "******************************************************************************************"
	@echo "***** Skipping tests of experimental Ada frontend (must be configured explicitly) *****"
	@echo "******************************************************************************************"
endif

