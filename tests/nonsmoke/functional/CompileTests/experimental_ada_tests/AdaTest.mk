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
  access_to_object.ads                 \
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
  enum_derived_type.adb                \
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
  largeenum.adb                        \
  linkedlists.ads                      \
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
  ordinary_type_declaration_range_constraint.ads    \
  overload.ads                         \
  overload.adb                         \
  package_with_body.ads                \
  parameter_declaration.ads            \
  parent-child.ads                     \
  private_enum.ads                     \
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
  simple_expression_range.ads          \
  some_modes.ads                       \
  standard_subtypes.ads                \
  subtypes.ads                         \
  sub_derived_types.ads                \
  switches.ads                         \
  tagged_definition.ads                \
  tagged_incomplete_type_declaration.ads \
  type_conversion.adb                  \
  unconstrained_array_definition.ads   \
  unit_1.adb                           \
  unit_3.ads                           \
  variable_declaration.ads             \
  week_string_literal.ads              \
  while_loop_statement.adb

ADA_TRANSLATOR_PASSING_INCOMPLETE_TESTS :=       \
  access_definition.ads                \
  access_type_test.adb                 \
  access_variable.adb                  \
  alignment_component_clause.adb       \
  all_modes.ads                        \
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
  derived_private_record.adb           \
  enumeration.adb                      \
  enumeration_with_text_io.adb         \
  exception_declaration.ads            \
  exception_renamingrenaming.adb       \
  exhandler.adb                        \
  extension_aggregate.adb              \
  explicit_deref.adb                   \
  floating_point_definition.ads        \
  formal_procedure_declaration.ads     \
  formal_subprogram_declaration.ads    \
  function_body_stub.adb               \
  function_renaming.adb                \
  function_instantiation.adb           \
  generic_function_declaration.ads     \
  generic_package_declaration.ads      \
  goto_loop_v2.adb                     \
  hardwarestore.adb                    \
  hello_world.adb                      \
  identity.ads                         \
  ifexpr.adb                           \
  io.ads                               \
  io_test.adb                          \
  io_test2.adb                         \
  multidimensional_array.adb           \
  multidimensional_array.ads           \
  myarray.adb                          \
  nullptr.adb                          \
  object_renaming_declaration.adb      \
  operator.adb                         \
  package_instantiation.adb            \
  package_pragma.ads                   \
  package_with_body.adb                \
  parent.ads                           \
  parenthesis_matters.adb              \
  pragmas.adb                          \
  private_access_type.adb              \
  prefixcalls.adb                      \
  procedure_call.adb                   \
  procedure_instantiation.adb          \
  procedure_instantiation2.adb         \
  procedure_body_stub.adb              \
  procedure_pragma.adb                 \
  raise_statement.adb                  \
  real_typedecl.ads                    \
  representation_clause.ads            \
  representation_clause_2.ads          \
  scopequal.adb                        \
  simpleada-io.ads                     \
  simple_array_expressions.adb         \
  simple_hello_world.adb               \
  simple_hello_world_with_renamed.adb  \
  simple_hello_world_with_use.adb      \
  default_function_argument.adb        \
  qualified_expression.adb             \
  type_in_local_pkg.adb                \
  very_large_value.adb                 

ADA_TRANSLATOR_FAILING_TESTS :=        \
  ada_packages.adb                     \
  accept_statement.adb                 \
  accept_statement_task_decl.adb       \
  access_to_subprogram.ads             \
  array_declare.adb                    \
  array_declare_2.ads                  \
  ascii_user.adb                       \
  asm_machine_code.adb                 \
  based_numbers.ads                    \
  box.adb                              \
  choice_parameter_specification.adb   \
  complex.ads                          \
  complex.adb                          \
  deref_test.adb                                    \
  discriminant.adb                     \
  discriminant_constraint.ads          \
  discriminated_record.adb             \
  discriminant_specification.ads       \
  dynamic_array.adb                    \
  entry_call.adb                       \
  entry_declaration.ads                \
  entry_body_declaration.adb           \
  entry_index_specification.adb        \
  exception_rename.ads                 \
  fixed_point_definition.ads           \
  generic_package_renaming_declaration.adb          \
  genex.adb                            \
  gentask.ads                          \
  gentask.adb                          \
  gentask-share.adb                    \
  generic_package_declaration.adb      \
  generic_procedure_renaming_declaration.ads  \
  generic_function_renaming_declaration.ads   \
  inherited_primitive_operation.adb    \
  iterate_range.adb                    \
  digital.ads                          \
  imageloader.adb                      \
  m_types.ads                          \
  m_types_test.ads                     \
  m_types_use.ads                      \
  namequal_with_renamed_1.adb          \
  namequal_with_renamed_2.adb          \
  nested_package.ads                   \
  opaque.ads                           \
  package_renaming_declaration.ads     \
  parent-child.adb                     \
  pkgrename.ads                        \
  private_derived_formal_type.adb      \
  procedure_renaming.adb               \
  protected_object.adb                 \
  protected_body_declaration.adb       \
  protected_body_declaration.ads       \
  protected_type_declaration.ads       \
  record_extends_formal_type.ads       \
  renamings.adb                        \
  renaming_as_body.adb                 \
  requeue_statement_2.adb              \
  requeue_statements.adb               \
  rep_database.ads                     \
  rep_database-mgr.ads                 \
  rep_common_types-data.ads            \
  rep_common_types-menu.ads            \
  rep_common_types.ads                 \
  select_accept.adb                    \
  simple_select_accept.adb             \
  simple_select_accept_guarded.adb     \
  simple_use_type.ads                  \
  single_protected_declaration.ads     \
  subobj_renaming_declaration.adb      \
  std_ascii_user.adb                   \
  taskarray.adb                        \
  task_type_declaration.ads            \
  task_with_abort.adb                  \
  task_with_abort_v2.adb               \
  task_with_body.adb                   \
  task_with_body.ads                   \
  test.ads                             \
  test_unit.ads                        \
  test.adb                             \
  test_unit.adb                        \
  usegenericinstance.adb               \
  unit_2.adb                           \
  unit_2.ads                           \
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


# ROSE_PASSING_IMCOMPLETE_ADA_TESTS :=                

ROSE_FAILING_ADA_TESTS :=                           \
  aspect_spec.ads                                   \
  blockreference.adb                                \
  sq_pkg_fun_overload.adb                           \
  delay_until.adb                                   \
  mangle_disctypes.adb                 \
  mpiuse.ads

# defines tests that are supported by ASIS
ASIS_PASSING_TESTS :=                               \
  $(ROSE_PASSING_ADA_TESTS)                         \
  $(ROSE_PASSING_IMCOMPLETE_ADA_TESTS)              \
  $(ROSE_FAILING_ADA_TESTS)

# the failing tests may actually not fail but just take a very long time
ASIS_FAILING_TESTS :=                               \
  mpi.ads

#defining all Ada test files
ADA_TESTS :=                                        \
  $(ASIS_PASSING_TESTS)                             \
  $(ASIS_FAILING_TESTS)
