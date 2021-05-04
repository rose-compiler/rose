########################################################################################3#
#                              STRUCTURE OF ADA TESTS
#
#                                                               ADA_TESTS
#                                                               /      \
#                                             ASIS_PASSING_TESTS        ASIS_FAILING_TESTS
#                                             /       |        \
#                                            /        |         \
#                       ROSE_PASSING_ADA_TESTS        |         ROSE_FAILING_ADA_TESTS
#                      /           |          \	     ROSE_PASSING_IMCOMPLETE_ADA_TESTS
#                     /            |           \
#   ADA_TRANSLATOR_PASSING_TESTS   |     ADA_TRANSLATOR_FAILING_TESTS
#                  ADA_TRANSLATOR_PASSING_INCOMPLETE_TESTS
########################################################################################3#

ADA_TRANSLATOR_PASSING_TESTS :=        \
  block_statement.adb                  \
  case_statement.adb                   \
  case_function.ads                    \
  comm.ads                             \
  component_declaration.ads            \
  constant_declaration.ads             \
  constants.ads                        \
  delay_relative.adb                   \
  derived_record.ads                   \
  empty_package.ads                    \
  enum_colors.ads                      \
  exit_statement.adb                   \
  fib.adb                              \
  for_loop.adb                         \
  for_loop_statement.adb               \
  function_body_stub.ads               \
  function_call.adb                    \
  function_declaration.adb             \
  function_declaration.ads             \
  goto_loop.adb                        \
  gototest.adb                         \
  gototest.ads                         \
  if_statement.adb                     \
  increment.adb                        \
  loop_exit.adb                        \
  loop_parameter_specification.adb     \
  loop_statement.adb                   \
  minimal.adb                          \
  named.ads                            \
  ordinary_type_declaration.ads        \
  operator.adb                         \
  overload.ads                         \
  overload.adb                         \
  package_with_body.ads                \
  parameter_declaration.ads            \
  procedure_body_stub.ads              \
  procedure_declaration.adb            \
  procedure_declaration.ads            \
  recursive_call.adb                   \
  selected_component.ads               \
  sender.ads                           \
  short_circuit.adb                    \
  short_circuit_v2.adb                 \
  simpleada-io.ads                     \
  simpleada.ads                        \
  simple_hello_world.adb               \
  simple_hello_world_with_use.adb      \
  standard_subtypes.ads                \
  unit_1.adb                           \
  unit_3.ads                           \
  variable_declaration.ads             \
  while_loop_statement.adb

ADA_TRANSLATOR_PASSING_INCOMPLETE_TESTS :=       \
  case_function.adb                    \
  constrained_array_definition.ads     \
  defining_character_literal.adb       \
  defining_enumeration_literal.adb     \
  defining_operator_symbol.ads         \
  enumeration.adb                      \
  exception_declaration.ads            \
  exhandler.adb                        \
  function_renaming.adb                \
  myarray.ads                          \
  myarray.adb                          \
  nullrecord.ads                       \
  package_pragma.ads                   \
  parent.ads                           \
  parent-child.ads                     \
  pragmas.adb                          \
  private_extension_declaration.ads    \
  private_type_definition.ads          \
  procedure_call.adb                   \
  procedure_pragma.adb                 \
  raise_statement.adb                  \
  simple_array_expressions.adb         \
  some_modes.ads                       \
  subtypes.ads                         \
  tagged_definition.ads                \
  type_conversion.adb                  \
  unconstrained_array_definition.ads   \
  week_string_literal.ads

ADA_TRANSLATOR_FAILING_TESTS :=        \
  allocators.adb                       \
  accept_statement.adb                 \
  alignment_component_clause.adb       \
  array_declare.adb                    \
  array_declare_2.ads                  \
  array_expressions.adb                \
  array_pointer_type.ads               \
  asm_machine_code.adb                 \
  async_transfer_select.adb            \
  based_numbers.ads                    \
  choice_parameter_specification.adb   \
  complex.ads                          \
  empty_private.ads                    \
  entry_call.adb                       \
  entry_declaration.ads                \
  enumeration_with_text_io.adb         \
  exception_rename.ads                 \
  exception_renamingrenaming.adb       \
  explicit_deref.adb                   \
  floating_point_definition.ads        \
  goto_loop_v2.adb                     \
  hello_world.adb                      \
  io_test.adb                          \
  linkedlists.ads                      \
  modular_type_definition.ads          \
  multidimensional_array.adb           \
  multidimensional_array.ads           \
  nested_package.ads                   \
  opaque.ads                           \
  package_renaming_declaration.ads     \
  package_with_body.adb                \
  pkgrename.ads                        \
  procedure_renaming.adb               \
  ranges.adb                           \
  real_typedecl.ads                    \
  representation_clause.ads            \
  representation_clause_2.ads          \
  scopequal.adb                        \
  select_accept.adb                    \
  simple_expression_range.ads          \
  simple_select_accept.adb             \
  simple_select_accept_guarded.adb     \
  simple_use_type.ads                  \
  std_ascii_user.adb                   \
  sub_derived_types.ads                \
  task_type_declaration.ads            \
  task_with_abort.adb                  \
  task_with_abort_v2.adb               \
  task_with_body.adb                   \
  task_with_body.ads                   \
  test.ads                             \
  test_unit.ads                        \
  type_in_local_pkg.adb

ADA_TRANSLATOR_PENDING_TESTS :=

# defines tests that are fully supported by ROSE
ROSE_PASSING_ADA_TESTS :=                           \
  $(ADA_TRANSLATOR_PASSING_TESTS)                   \
  $(ADA_TRANSLATOR_PASSING_INCOMPLETE_TESTS)        \
  $(ADA_TRANSLATOR_PENDING_TESTS)                   \
  $(ADA_TRANSLATOR_FAILING_TESTS)


ROSE_PASSING_IMCOMPLETE_ADA_TESTS :=                \
  access_to_object.ads                              \
  access_to_subprogram.ads                          \
  discriminant.adb                                  \
  discriminant_constraint.ads                       \
  discriminant_specification.ads                    \
  dynamic_array.adb                                 \
  extension_aggregate.adb                           \
  fixed_point_definition.ads                        \
  function_instantiation.adb                        \
  generic_function_declaration.ads                  \
  generic_package_declaration.ads                   \
  ordinary_type_declaration_range_constraint.ads    \
  package_instantiation.adb                         \
  procedure_instantiation.adb                       \
  protected_type_declaration.ads                    \
  qualified_expression.adb                          \
  signed_integer_type_definition.ads                \
  single_protected_declaration.ads                  \
  test.adb                                          \
  object_renaming_declaration.adb                   \
  entry_body_declaration.adb                        \
  function_body_stub.adb                            \
  generic_function_renaming_declaration.ads         \
  generic_package_renaming_declaration.ads          \
  generic_procedure_renaming_declaration.ads        \
  protected_body_declaration.adb                    \
  protected_body_declaration.ads                    \
  procedure_body_stub.adb                           \
  unknown_discriminant.ads                          \
  variable_variant_record.adb                       \
  variant_record.ads

ROSE_FAILING_ADA_TESTS :=                           \
  all_modes.ads                                     \
  complex.adb                                       \
  delay_until.adb                                   \
  iterate_range.adb                                 \
  mpiuse.ads                                        \
  requeue_statement_2.adb                           \
  requeue_statements.adb                            \
  test_unit.adb                                     \
  unit_2.adb                                        \
  unit_2.ads

# defines tests that are supported by ASIS
ASIS_PASSING_TESTS :=                               \
  $(ROSE_PASSING_ADA_TESTS)                         \
  $(ROSE_PASSING_IMCOMPLETE_ADA_TESTS)              \
  $(ROSE_FAILING_ADA_TESTS)

ASIS_FAILING_TESTS :=                               \
  entry_index_specification.adb                     \
  mpi.ads

#defining all Ada test files
ADA_TESTS :=                                        \
  $(ASIS_PASSING_TESTS)                             \
  $(ASIS_FAILING_TESTS)
