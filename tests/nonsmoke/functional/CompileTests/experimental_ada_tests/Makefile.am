include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (8/23/2017): Use command line option to alternatively test the experimental fortran frontend.
# ROSE_FLAGS = -rose:verbose 0 -rose:experimental_csharp_frontend -I$(srcdir)
ROSE_FLAGS    ?= -rose:skipfinalCompileStep

# testTranslator runs the AST consistency checks
../../testTranslator:
	cd ../..; $(MAKE) testTranslator


# ROSE_COMPILER ?= $(top_builddir)/src/rose-compiler
ROSE_COMPILER ?= ../../testTranslator

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


PASSING_TESTS :=                       \
  procedure_call.adb                   \
  task_with_body.adb                   \
  recursive_call.adb                   \
  raise_statement.adb                  \
  type_conversion.adb                  \
  unit_1.adb                           \
  loop_statement.adb                   \
  minimal.adb                          \
  if_statement.adb                     \
  fib.adb                              \
  while_loop_statement.adb             \
  increment.adb	                       \
  pkgrename.ads                        \
  variable_declaration.ads             \
  constant_declaration.ads             \
  derived_record.ads                   \
  empty_package.ads                    \
  named.ads                            \
  opaque.ads                           \
  ordinary_type_declaration.ads        \
  parent-child.ads                     \
  selected_component.ads               \
  simple_expression_range.ads          \
  tagged_definition.ads                \
  task_type_declaration.ads            \
  task_with_body.ads                   \
  EndOfList

# pass asis, frontend, backend, but representation is incomplete
PASSING_INCOMPLETE_TESTS :=            \
  component_declaration.ads            \
  real_typedecl.ads                    \
  subtypes.ads                         \
  EndOfList

# fail in backend
PASSING_FRONTEND :=                    \
  constants.ads                        \
  loop_exit.adb                        \
  operator.adb                         \
  EndOfList

# fail in frontend
PASSING_ASIS := \
  hello_world.adb                      \
  choice_parameter_specification.adb   \
  EndOfList

# incomplete Asis or Dot file
NOT_HANDLED := \
  accept_statement.adb                 \
  all_modes.ads                        \
  case_function.ads                    \
  case_function.adb                    \
  entry_declaration.ads                \
  entry_call.adb                       \
  exception_declaration.ads            \
  for_loop.adb                         \
  function_call.adb                    \
  function_declaration.ads             \
  function_declaration.adb             \
  gototest.ads                         \
  gototest.adb                         \
  iterate_range.adb                    \
  mpiuse.ads                           \
  nested_package.ads                   \
  parameter_declaration.ads            \
  package_with_body.ads                \
  package_with_body.adb                \
  procedure_declaration.ads            \
  procedure_declaration.adb            \
  sender.ads                           \
  short_circuit.adb                    \
  some_modes.ads                       \
  test.ads                             \
  test.adb                             \
  test_unit.ads                        \
  test_unit.adb                        \
  unit_2.ads                           \
  unit_2.adb                           \
  unit_3.ads                           \
  EndOfList

NOT_SEPARATELY_CHECKED_DEPENDENCIES := \
  parent.ads                           \
  comm.ads                             \
  mpi.ads                              \
  EndOfList

#
# define test suite
TEST_TARGETS := $(PASSING_TESTS) $(PASSING_INCOMPLETE_TESTS)
ALL_TEST_TARGETS := $(PASSING_TESTS) $(PASSING_INCOMPLETE_TESTS) $(PASSING_FRONTEND) $(PASSING_ASIS) $(NOT_HANDLED)

TESTCODES_REQUIRED_TO_PASS := $(TEST_TARGETS)

#
# targets
.PHONY: all
all: passing

.PHONY: passing
passing: $(TEST_TARGETS)

.PHONY: complete
complete: $(ALL_TEST_TARGETS)

.PHONY: passing-frontend
passing-frontend: $(PASSING_FRONTEND)

.PHONY: passing-asis
passing-asis: $(PASSING_ASIS)

.PHONY: EndOfList
EndOfList:

.PHONY: print
print:
	@echo "Output Makefile variables:"
	@echo "PASSING_TEST_Objects = $(PASSING_TEST_Objects)"
	@echo "TEST_Source_passed   = $(TEST_Source_passed)"
	@echo "TEST_Objects_passed  = $(TEST_Objects_passed)"

$(top_builddir)/src/rose-compiler:
	$(MAKE) -C $(top_builddir)/src rose-compiler

%.ads:$(abs_srcdir)/tests/%.ads $(ROSE_COMPILER)
	$(ROSE_COMPILER) $(ROSE_FLAGS) $<

%.adb:$(abs_srcdir)/tests/%.adb $(ROSE_COMPILER)
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

