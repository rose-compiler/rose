########################################################################################3#
#                              STRUCTURE OF ADA TESTS 
#                  
#                                                               ADA_TESTS
#                                                               /      \
#                                             ASIS_PASSING_TESTS        ASIS_FAILING_TESTS
#                                             /       |        \
#                                            /        |         \
#                       ROSE_PASSING_ADA_TESTS        |         ROSE_FAILING_ADA_TESTS 
#                      /                      \	     ROSE_PASSING_IMCOMPLETE_ADA_TESTS
#                     /                        \
#   ADA_TRANSLATOR_PASSING_TESTS         ADA_TRANSLATOR_FAILING_TESTS 
#                  
########################################################################################3#

ADA_TRANSLATOR_PASSING_TESTS :=        \
  block_statement.adb                  \
  case_statement.adb                   \
  component_declaration.ads            \
  constant_declaration.ads             \
  constants.ads                        \
  delay_relative.adb                   \
  empty_package.ads                    \
  exit_statement.adb                   \
  fib.adb                              \
  for_loop.adb                         \
  for_loop_statement.adb               \
  function_declaration.adb             \
  function_declaration.ads             \
  goto_loop.adb                        \
  if_statement.adb                     \
  increment.adb                        \
  loop_exit.adb                        \
  loop_statement.adb                   \
  minimal.adb                          \
  named.ads                            \
  operator.adb                         \
  package_with_body.adb                \
  package_with_body.ads                \
  parameter_declaration.ads            \
  procedure_declaration.adb            \
  recursive_call.adb                   \
  selected_component.ads               \
  simpleada.ads                        \
  unit_1.adb                           \
  unit_3.ads                           \
  variable_declaration.ads             \
  while_loop_statement.adb 

ADA_TRANSLATOR_FAILING_TESTS :=        \
  accept_statement.adb                 \
  array_declare_2.ads                  \
  async_transfer_select.adb            \
  based_numbers.ads                    \
  complex.ads                          \
  defining_character_literal.adb       \
  defining_enumeration_literal.adb     \
  defining_operator_symbol.ads         \
  derived_record.ads                   \
  entry_call.adb                       \
  entry_declaration.ads                \
  exception_declaration.ads            \
  exception_rename.ads                 \
  floating_point_definition.ads        \
  function_body_stub.ads               \
  gototest.adb                         \
  gototest.ads                         \
  multidimensional_array.adb           \
  multidimensional_array.ads           \
  myarray.ads                          \
  myarray.adb                          \
  nullrecord.ads                       \
  opaque.ads                           \
  ordinary_type_declaration.ads        \
  package_pragma.ads                   \
  package_renaming_declaration.ads     \
  parent-child.ads                     \
  parent.ads                           \
  pragmas.adb                          \
  private_extension_declaration.ads    \
  procedure_body_stub.ads              \
  procedure_call.adb                   \
  procedure_declaration.ads            \
  procedure_pragma.adb                 \
  raise_statement.adb                  \
  representation_clause.ads            \
  representation_clause_2.ads          \
  scopequal.adb                        \
  short_circuit.adb                    \
  short_circuit_v2.adb                 \
  simple_array_expressions.adb         \
  simple_expression_range.ads          \
  simple_hello_world.adb               \
  simple_hello_world_with_use.adb      \
  simple_select_accept.adb             \
  simple_select_accept_guarded.adb     \
  simple_use_type.ads                  \
  simpleada-io.ads                     \
  some_modes.ads                       \
  subtypes.ads                         \
  tagged_definition.ads                \
  task_type_declaration.ads            \
  task_with_abort.adb                  \
  task_with_abort_v2.adb               \
  task_with_body.adb                   \
  task_with_body.ads                   \
  type_conversion.adb                  \
  unconstrained_array_definition.ads   \
  week_string_literal.ads              

# defines tests that are fully supported by ROSE
ROSE_PASSING_ADA_TESTS :=              \
  $(ADA_TRANSLATOR_PASSING_TESTS)      \
  $(ADA_TRANSLATOR_FAILING_TESTS)      

ROSE_PASSING_IMCOMPLETE_ADA_TESTS :=                \
  protected_type_declaration.ads                    \
  single_protected_declaration.ads                  \
  object_renaming_declaration.adb                   \
  entry_body_declaration.adb                        \
  function_body_stub.adb                            \
  generic_package_renaming_declaration.ads          \
  protected_body_declaration.adb                    \
  protected_body_declaration.ads                    \
  procedure_body_stub.adb                           

ROSE_FAILING_ADA_TESTS :=                           \
  access_to_object.ads                              \
  access_to_subprogram.ads                          \
  alignment_component_clause.adb                    \
  allocators.adb                                    \
  array_declare.adb                                 \
  array_expressions.adb                             \
  asm_machine_code.adb                              \
  case_function.adb                                 \
  case_function.ads                                 \
  choice_parameter_specification.adb                \
  comm.ads                                          \
  constrained_array_definition.ads                  \
  delay_until.adb                                   \
  discriminant.adb                                  \
  discriminant_constraint.ads                       \
  discriminant_specification.ads                    \
  entry_index_specification.adb                     \
  enumeration.adb                                   \
  enum_colors.ads                                   \
  enumeration_with_text_io.adb                      \
  explicit_deref.adb                                \
  extension_aggregate.adb                           \
  fixed_point_definition.ads                        \
  function_call.adb                                 \
  function_renaming.adb                             \
  goto_loop_v2.adb                                  \
  hello_world.adb                                   \
  io_test.adb                                       \
  iterate_range.adb                                 \
  loop_parameter_specification.adb                  \
  modular_type_definition.ads                       \
  mpi.ads                                           \
  mpiuse.ads                                        \
  nested_package.ads                                \
  ordinary_type_declaration_range_constraint.ads    \
  pkgrename.ads                                     \
  private_type_definition.ads                       \
  procedure_renaming.adb                            \
  qualified_expression.adb                          \
  real_typedecl.ads                                 \
  requeue_statement_2.adb                           \
  requeue_statements.adb                            \
  select_accept.adb                                 \
  sender.ads                                        \
  signed_integer_type_definition.ads                \
  test.adb                                          \
  test.ads                                          \
  test_unit.adb                                     \
  test_unit.ads                                     \
  unit_2.adb                                        \
  unit_2.ads                                        \
  unknown_discriminant.ads                          \
  variable_variant_record.adb                       \
  variant_record.ads                                

# defines tests that are supported by ASIS
ASIS_PASSING_TESTS :=                               \
  $(ROSE_PASSING_ADA_TESTS)                         \
  $(ROSE_PASSING_IMCOMPLETE_ADA_TESTS)              \
  $(ROSE_FAILING_ADA_TESTS)                         

ASIS_FAILING_TESTS :=                               \
  all_modes.ads                                     \
  complex.adb                                       

#defining all Ada test files                         
ADA_TESTS :=                                        \
  $(ASIS_PASSING_TESTS)                             \
  $(ASIS_FAILING_TESTS)                             
