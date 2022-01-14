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
  empty_private.ads                    \
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
  modular_type_definition.ads          \
  myarray.ads                          \
  named.ads                            \
  named_block.adb                      \
  nullrecord.ads                       \
  ordinary_type_declaration.ads        \
  operator.adb                         \
  ordinary_type_declaration_range_constraint.ads    \
  overload.ads                         \
  overload.adb                         \
  package_with_body.ads                \
  parameter_declaration.ads            \
  private_extension_declaration.ads    \
  private_type_definition.ads          \
  procedure_body_stub.ads              \
  procedure_declaration.adb            \
  procedure_declaration.ads            \
  ranges.adb                           \
  recursive_call.adb                   \
  selected_component.ads               \
  sender.ads                           \
  short_circuit.adb                    \
  short_circuit_v2.adb                 \
  signed_integer_type_definition.ads   \
  simpleada.ads                        \
  some_modes.ads                       \
  standard_subtypes.ads                \
  subtypes.ads                         \
  switches.ads                         \
  tagged_definition.ads                \
  unit_1.adb                           \
  unit_3.ads                           \
  variable_declaration.ads             \
  very_large_value.adb                 \
  week_string_literal.ads              \
  while_loop_statement.adb

ADA_TRANSLATOR_PASSING_INCOMPLETE_TESTS :=       \
  access_variable.adb                  \
  alignment_component_clause.adb       \
  allocators.adb                       \
  array_expressions.adb                \
  array_pointer_type.ads               \
  async_transfer_select.adb            \
  case_function.adb                    \
  character_type.adb                   \
  constrained_array_definition.ads     \
  defining_character_literal.adb       \
  defining_enumeration_literal.adb     \
  defining_operator_symbol.ads         \
  enum_derived_type.adb                \
  enumeration.adb                      \
  enumeration_with_text_io.adb         \
  exception_declaration.ads            \
  exception_renamingrenaming.adb       \
  exhandler.adb                        \
  explicit_deref.adb                   \
  floating_point_definition.ads        \
  function_renaming.adb                \
  function_instantiation.adb           \
  goto_loop_v2.adb                     \
  hardwarestore.adb                    \
  hello_world.adb                      \
  io_test.adb                          \
  io_test2.adb                         \
  linkedlists.ads                      \
  multidimensional_array.adb           \
  multidimensional_array.ads           \
  myarray.adb                          \
  nullptr.adb                          \
  package_pragma.ads                   \
  package_instantiation.adb            \
  package_with_body.adb                \
  parent.ads                           \
  parent-child.ads                     \
  parenthesis_matters.adb              \
  pragmas.adb                          \
  private_access_type.adb              \
  private_enum.ads                     \
  procedure_call.adb                   \
  procedure_instantiation.adb          \
  procedure_instantiation2.adb         \
  procedure_pragma.adb                 \
  raise_statement.adb                  \
  real_typedecl.ads                    \
  scopequal.adb                        \
  simpleada-io.ads                     \
  simple_array_expressions.adb         \
  simple_expression_range.ads          \
  sub_derived_types.ads                \
  type_conversion.adb                  \
  unconstrained_array_definition.ads   \
  default_function_argument.adb        \
  qualified_expression.adb             \
  type_in_local_pkg.adb                

ADA_TRANSLATOR_FAILING_TESTS :=        \
  ada_packages.adb                     \
  accept_statement.adb                 \
  accept_statement_task_decl.adb       \
  array_declare.adb                    \
  array_declare_2.ads                  \
  asm_machine_code.adb                 \
  based_numbers.ads                    \
  choice_parameter_specification.adb   \
  complex.ads                          \
  complex.adb                          \
  discriminant.adb                     \
  discriminant_constraint.ads          \
  discriminated_record.adb             \
  discriminant_specification.ads       \
  dynamic_array.adb                    \
  entry_call.adb                       \
  entry_declaration.ads                \
  exception_rename.ads                 \
  formal_procedure_declaration.ads     \
  generic_function_declaration.ads     \
  generic_package_declaration.ads      \
  inherited_primitive_operation.adb    \
  io.ads                               \
  iterate_range.adb                    \
  nested_package.ads                   \
  opaque.ads                           \
  package_renaming_declaration.ads     \
  parent-child.adb                     \
  pkgrename.ads                        \
  procedure_renaming.adb               \
  protected_object.adb                 \
  protected_body_declaration.adb       \
  protected_body_declaration.ads       \
  representation_clause.ads            \
  representation_clause_2.ads          \
  select_accept.adb                    \
  simple_select_accept.adb             \
  simple_select_accept_guarded.adb     \
  simple_use_type.ads                  \
  simple_hello_world.adb               \
  simple_hello_world_with_renamed.adb  \
  simple_hello_world_with_use.adb      \
  single_protected_declaration.ads     \
  std_ascii_user.adb                   \
  taskarray.adb                        \
  task_type_declaration.ads            \
  task_with_abort.adb                  \
  task_with_abort_v2.adb               \
  task_with_body.adb                   \
  task_with_body.ads                   \
  test.ads                             \
  test_unit.ads                        \
  usegenericinstance.adb               \
  unknown_discriminant.ads             \
  uppercase.adb                        \
  variant_record.ads                   \
  variable_variant_record.adb

ADA_TRANSLATOR_PENDING_TESTS :=       

# defines tests that are fully supported by ROSE
ROSE_PASSING_ADA_TESTS :=                           \
  $(ADA_TRANSLATOR_PASSING_TESTS)                   \
  $(ADA_TRANSLATOR_PASSING_INCOMPLETE_TESTS)        \
  $(ADA_TRANSLATOR_PENDING_TESTS)                   \
  $(ADA_TRANSLATOR_FAILING_TESTS)


ROSE_PASSING_IMCOMPLETE_ADA_TESTS :=                \
  access_to_subprogram.ads                          \
  access_to_object.ads                              \
  fixed_point_definition.ads

ROSE_FAILING_ADA_TESTS :=                           \
  access_definition.ads                             \
  all_modes.ads                                     \
  aspect_spec.ads                                   \
  deref_test.adb                                    \
  delay_until.adb                                   \
  entry_body_declaration.adb                        \
  extension_aggregate.adb                           \
  function_body_stub.adb                            \
  generic_function_renaming_declaration.ads         \
  generic_package_renaming_declaration.ads          \
  generic_procedure_renaming_declaration.ads        \
  mpiuse.ads                                        \
  object_renaming_declaration.adb                   \
  procedure_body_stub.adb                           \
  protected_type_declaration.ads                    \
  requeue_statement_2.adb                           \
  requeue_statements.adb                            \
  tagged_incomplete_type_declaration.ads            \
  test.adb                                          \
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
