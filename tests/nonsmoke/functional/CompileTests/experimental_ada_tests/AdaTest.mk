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
  component_declaration.ads            \
  constant_declaration.ads             \
  constants.ads                        \
  delay_relative.adb                   \
  empty_package.ads                    \
  exit_statement.adb                   \
  fib.adb                              \
  for_loop.adb                         \
  for_loop_statement.adb               \
  function_body_stub.ads               \
  function_declaration.adb             \
  function_declaration.ads             \
  goto_loop.adb                        \
  gototest.adb                         \
  gototest.ads                         \
  if_statement.adb                     \
  increment.adb                        \
  loop_exit.adb                        \
  loop_statement.adb                   \
  minimal.adb                          \
  named.ads                            \
  operator.adb                         \
  overload.ads                         \  
  overload.adb                         \
  package_with_body.adb                \
  package_with_body.ads                \
  parameter_declaration.ads            \
  procedure_body_stub.ads              \
  procedure_declaration.adb            \
  procedure_declaration.ads            \
  recursive_call.adb                   \
  selected_component.ads               \
  short_circuit.adb                    \
  short_circuit_v2.adb                 \
  unit_1.adb                           \
  unit_3.ads                           \
  variable_declaration.ads             \
  while_loop_statement.adb 

ADA_TRANSLATOR_PASSING_INCOMPLETE_TESTS :=       \
  derived_record.ads                   \
  myarray.ads                          \
  myarray.adb                          \
  nullrecord.ads                       \
  package_pragma.ads                   \
  private_extension_declaration.ads    \
  procedure_pragma.adb                 \
  some_modes.ads                       \
  subtypes.ads                         \
  tagged_definition.ads                \
  unconstrained_array_definition.ads   

ADA_TRANSLATOR_FAILING_TESTS :=        \
  accept_statement.adb                 \
  alignment_component_clause.adb       \
  array_declare_2.ads                  \
  async_transfer_select.adb            \
  based_numbers.ads                    \
  choice_parameter_specification.adb   \
  complex.ads                          \
  defining_character_literal.adb       \
  defining_operator_symbol.ads         \
  defining_enumeration_literal.adb     \
  entry_call.adb                       \
  entry_declaration.ads                \
  exception_declaration.ads            \
  exception_rename.ads                 \
  floating_point_definition.ads        \
  multidimensional_array.adb           \
  multidimensional_array.ads           \
  opaque.ads                           \
  ordinary_type_declaration.ads        \
  package_renaming_declaration.ads     \
  parent.ads                           \
  parent-child.ads                     \
  pragmas.adb                          \
  procedure_call.adb                   \
  raise_statement.adb                  \
  scopequal.adb                        \
  representation_clause.ads            \
  representation_clause_2.ads          \
  simple_array_expressions.adb         \
  simple_expression_range.ads          \
  simple_hello_world.adb               \
  simple_hello_world_with_use.adb      \
  simple_select_accept.adb             \
  simple_select_accept_guarded.adb     \
  simple_use_type.ads                  \
  simpleada.ads                        \
  simpleada-io.ads                     \
  sub_derived_types.ads                \
  task_type_declaration.ads            \
  task_with_abort.adb                  \
  task_with_abort_v2.adb               \
  task_with_body.adb                   \
  task_with_body.ads                   \
  type_conversion.adb                  \
  week_string_literal.ads              

ADA_TRANSLATOR_PENDING_TESTS :=        \
  array_expressions.adb                \
  asm_machine_code.adb                 \
  case_function.adb                    \
  case_function.ads                    \
  comm.ads                             \
  constrained_array_definition.ads     \
  enum_colors.ads                      \
  enumeration.adb                      \
  enumeration_with_text_io.adb         \
  function_call.adb                    \
  function_renaming.adb                \
  goto_loop_v2.adb                     \
  hello_world.adb                      \
  io_test.adb                          \
  loop_parameter_specification.adb     \
  modular_type_definition.ads          \
  pkgrename.ads                        \
  procedure_renaming.adb               \
  private_type_definition.ads          \
  real_typedecl.ads                    \
  select_accept.adb                    \
  sender.ads                           \
  standard_subtypes.ads                \
  test.ads                             \
  test_unit.ads                                     

# defines tests that are fully supported by ROSE
ROSE_PASSING_ADA_TESTS :=                           \
  $(ADA_TRANSLATOR_PASSING_TESTS)                   \
  $(ADA_TRANSLATOR_PASSING_INCOMPLETE_TESTS)        \
  $(ADA_TRANSLATOR_PENDING_TESTS)                   \
  $(ADA_TRANSLATOR_FAILING_TESTS)                   


ROSE_PASSING_IMCOMPLETE_ADA_TESTS :=                \
  access_to_object.ads                              \
  access_to_subprogram.ads                          \
  allocators.adb                                    \
  array_declare.adb                                 \
  discriminant.adb                                  \
  discriminant_constraint.ads                       \
  discriminant_specification.ads                    \
  explicit_deref.adb                                \
  extension_aggregate.adb                           \
  fixed_point_definition.ads                        \
  ordinary_type_declaration_range_constraint.ads    \
  protected_type_declaration.ads                    \
  qualified_expression.adb                          \
  signed_integer_type_definition.ads                \
  single_protected_declaration.ads                  \
  test.adb                                          \
  object_renaming_declaration.adb                   \
  entry_body_declaration.adb                        \
  function_body_stub.adb                            \
  generic_package_renaming_declaration.ads          \
  protected_body_declaration.adb                    \
  protected_body_declaration.ads                    \
  procedure_body_stub.adb                           \
  unknown_discriminant.ads                          \
  variable_variant_record.adb                       \
  variant_record.ads                                

ROSE_FAILING_ADA_TESTS :=                           \
  delay_until.adb                                   \
  entry_index_specification.adb                     \
  iterate_range.adb                                 \
  mpi.ads                                           \
  mpiuse.ads                                        \
  nested_package.ads                                \
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
  all_modes.ads                                     \
  complex.adb                                       

#defining all Ada test files                         
ADA_TESTS :=                                        \
  $(ASIS_PASSING_TESTS)                             \
  $(ASIS_FAILING_TESTS)                             
