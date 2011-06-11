#ifdef __cplusplus
extern "C" {
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ActionEnums.h"
#include "token.h"
typedef int ofp_bool;

void c_action_name(Token_t *carg_0);

void c_action_substring(ofp_bool carg_0);

void c_action_format();

void c_action_rename(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4, Token_t *carg_5);

void c_action_prefix(int carg_0);

void c_action_block();

void c_action_suffix(Token_t *carg_0, ofp_bool carg_1);

void c_action_keyword();

void c_action_expr();

void c_action_generic_name_list__begin();

void c_action_generic_name_list(int carg_0);

void c_action_generic_name_list_part(Token_t *carg_0);

void c_action_specification_part(int carg_0, int carg_1, int carg_2, int carg_3);

void c_action_declaration_construct();

void c_action_execution_part();

void c_action_execution_part_construct();

void c_action_internal_subprogram_part(int carg_0);

void c_action_internal_subprogram();

void c_action_specification_stmt();

void c_action_executable_construct();

void c_action_action_stmt();

void c_action_constant(Token_t *carg_0);

void c_action_scalar_constant();

void c_action_literal_constant();

void c_action_int_constant(Token_t *carg_0);

void c_action_char_constant(Token_t *carg_0);

void c_action_intrinsic_operator();

void c_action_defined_operator(Token_t *carg_0, ofp_bool carg_1);

void c_action_extended_intrinsic_op();

void c_action_label(Token_t *carg_0);

void c_action_label_list__begin();

void c_action_label_list(int carg_0);

void c_action_type_spec();

void c_action_type_param_value(ofp_bool carg_0, ofp_bool carg_1, ofp_bool carg_2);

void c_action_intrinsic_type_spec(Token_t *carg_0, Token_t *carg_1, int carg_2, ofp_bool carg_3);

void c_action_kind_selector(Token_t *carg_0, Token_t *carg_1, ofp_bool carg_2);

void c_action_signed_int_literal_constant(Token_t *carg_0);

void c_action_int_literal_constant(Token_t *carg_0, Token_t *carg_1);

void c_action_kind_param(Token_t *carg_0);

void c_action_boz_literal_constant(Token_t *carg_0);

void c_action_signed_real_literal_constant(Token_t *carg_0);

void c_action_real_literal_constant(Token_t *carg_0, Token_t *carg_1);

void c_action_complex_literal_constant();

void c_action_real_part(ofp_bool carg_0, ofp_bool carg_1, Token_t *carg_2);

void c_action_imag_part(ofp_bool carg_0, ofp_bool carg_1, Token_t *carg_2);

void c_action_char_selector(Token_t *carg_0, Token_t *carg_1, int carg_2, int carg_3, ofp_bool carg_4);

void c_action_length_selector(Token_t *carg_0, int carg_1, ofp_bool carg_2);

void c_action_char_length(ofp_bool carg_0);

void c_action_scalar_int_literal_constant();

void c_action_char_literal_constant(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_logical_literal_constant(Token_t *carg_0, ofp_bool carg_1, Token_t *carg_2);

void c_action_derived_type_def();

void c_action_type_param_or_comp_def_stmt(Token_t *carg_0, int carg_1);

void c_action_type_param_or_comp_def_stmt_list();

void c_action_derived_type_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, ofp_bool carg_4, ofp_bool carg_5);

void c_action_type_attr_spec(Token_t *carg_0, Token_t *carg_1, int carg_2);

void c_action_type_attr_spec_list__begin();

void c_action_type_attr_spec_list(int carg_0);

void c_action_private_or_sequence();

void c_action_end_type_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_sequence_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_type_param_decl(Token_t *carg_0, ofp_bool carg_1);

void c_action_type_param_decl_list__begin();

void c_action_type_param_decl_list(int carg_0);

void c_action_type_param_attr_spec(Token_t *carg_0);

void c_action_component_def_stmt(int carg_0);

void c_action_data_component_def_stmt(Token_t *carg_0, Token_t *carg_1, ofp_bool carg_2);

void c_action_component_attr_spec(Token_t *carg_0, int carg_1);

void c_action_component_attr_spec_list__begin();

void c_action_component_attr_spec_list(int carg_0);

void c_action_component_decl(Token_t *carg_0, ofp_bool carg_1, ofp_bool carg_2, ofp_bool carg_3, ofp_bool carg_4);

void c_action_component_decl_list__begin();

void c_action_component_decl_list(int carg_0);

void c_action_component_array_spec(ofp_bool carg_0);

void c_action_deferred_shape_spec_list__begin();

void c_action_deferred_shape_spec_list(int carg_0);

void c_action_component_initialization();

void c_action_proc_component_def_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, ofp_bool carg_3);

void c_action_proc_component_attr_spec(Token_t *carg_0, Token_t *carg_1, int carg_2);

void c_action_proc_component_attr_spec_list__begin();

void c_action_proc_component_attr_spec_list(int carg_0);

void c_action_private_components_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_type_bound_procedure_part(int carg_0, ofp_bool carg_1);

void c_action_binding_private_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_proc_binding_stmt(Token_t *carg_0, int carg_1, Token_t *carg_2);

void c_action_specific_binding(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, ofp_bool carg_4);

void c_action_generic_binding(Token_t *carg_0, ofp_bool carg_1);

void c_action_binding_attr(Token_t *carg_0, int carg_1, Token_t *carg_2);

void c_action_binding_attr_list__begin();

void c_action_binding_attr_list(int carg_0);

void c_action_final_binding(Token_t *carg_0);

void c_action_derived_type_spec(Token_t *carg_0, ofp_bool carg_1);

void c_action_type_param_spec(Token_t *carg_0);

void c_action_type_param_spec_list__begin();

void c_action_type_param_spec_list(int carg_0);

void c_action_structure_constructor(Token_t *carg_0);

void c_action_component_spec(Token_t *carg_0);

void c_action_component_spec_list__begin();

void c_action_component_spec_list(int carg_0);

void c_action_component_data_source();

void c_action_enum_def(int carg_0);

void c_action_enum_def_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_enumerator_def_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_enumerator(Token_t *carg_0, ofp_bool carg_1);

void c_action_enumerator_list__begin();

void c_action_enumerator_list(int carg_0);

void c_action_end_enum_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3);

void c_action_array_constructor();

void c_action_ac_spec();

void c_action_ac_value();

void c_action_ac_value_list__begin();

void c_action_ac_value_list(int carg_0);

void c_action_ac_implied_do();

void c_action_ac_implied_do_control(ofp_bool carg_0);

void c_action_scalar_int_variable();

void c_action_type_declaration_stmt(Token_t *carg_0, int carg_1, Token_t *carg_2);

void c_action_declaration_type_spec(Token_t *carg_0, int carg_1);

void c_action_attr_spec(Token_t *carg_0, int carg_1);

void c_action_entity_decl(Token_t *carg_0, ofp_bool carg_1, ofp_bool carg_2, ofp_bool carg_3, ofp_bool carg_4);

void c_action_entity_decl_list__begin();

void c_action_entity_decl_list(int carg_0);

void c_action_initialization(ofp_bool carg_0, ofp_bool carg_1);

void c_action_null_init(Token_t *carg_0);

void c_action_access_spec(Token_t *carg_0, int carg_1);

void c_action_language_binding_spec(Token_t *carg_0, Token_t *carg_1, ofp_bool carg_2);

void c_action_coarray_spec(int carg_0);

void c_action_array_spec(int carg_0);

void c_action_array_spec_element(int carg_0);

void c_action_explicit_shape_spec(ofp_bool carg_0);

void c_action_explicit_shape_spec_list__begin();

void c_action_explicit_shape_spec_list(int carg_0);

void c_action_intent_spec(Token_t *carg_0, Token_t *carg_1, int carg_2);

void c_action_access_stmt(Token_t *carg_0, Token_t *carg_1, ofp_bool carg_2);

void c_action_access_id();

void c_action_access_id_list__begin();

void c_action_access_id_list(int carg_0);

void c_action_allocatable_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_allocatable_decl(Token_t *carg_0, ofp_bool carg_1, ofp_bool carg_2);

void c_action_allocatable_decl_list__begin();

void c_action_allocatable_decl_list(int carg_0);

void c_action_asynchronous_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_bind_stmt(Token_t *carg_0, Token_t *carg_1);

void c_action_bind_entity(Token_t *carg_0, ofp_bool carg_1);

void c_action_bind_entity_list__begin();

void c_action_bind_entity_list(int carg_0);

void c_action_codimension_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_codimension_decl(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_codimension_decl_list__begin();

void c_action_codimension_decl_list(int carg_0);

void c_action_data_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, int carg_3);

void c_action_data_stmt_set();

void c_action_data_stmt_object();

void c_action_data_stmt_object_list__begin();

void c_action_data_stmt_object_list(int carg_0);

void c_action_data_implied_do(Token_t *carg_0, ofp_bool carg_1);

void c_action_data_i_do_object();

void c_action_data_i_do_object_list__begin();

void c_action_data_i_do_object_list(int carg_0);

void c_action_data_stmt_value(Token_t *carg_0);

void c_action_data_stmt_value_list__begin();

void c_action_data_stmt_value_list(int carg_0);

void c_action_scalar_int_constant();

void c_action_hollerith_constant(Token_t *carg_0);

void c_action_data_stmt_constant();

void c_action_dimension_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, int carg_3);

void c_action_dimension_decl(Token_t *carg_0);

void c_action_intent_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_optional_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_parameter_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_named_constant_def_list__begin();

void c_action_named_constant_def_list(int carg_0);

void c_action_named_constant_def(Token_t *carg_0);

void c_action_pointer_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_pointer_decl_list__begin();

void c_action_pointer_decl_list(int carg_0);

void c_action_pointer_decl(Token_t *carg_0, ofp_bool carg_1);

void c_action_cray_pointer_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_cray_pointer_assoc_list__begin();

void c_action_cray_pointer_assoc_list(int carg_0);

void c_action_cray_pointer_assoc(Token_t *carg_0, Token_t *carg_1);

void c_action_protected_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_save_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, ofp_bool carg_3);

void c_action_saved_entity_list__begin();

void c_action_saved_entity_list(int carg_0);

void c_action_saved_entity(Token_t *carg_0, ofp_bool carg_1);

void c_action_target_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_target_decl(Token_t *carg_0, ofp_bool carg_1, ofp_bool carg_2);

void c_action_target_decl_list__begin();

void c_action_target_decl_list(int carg_0);

void c_action_value_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_volatile_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_implicit_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, ofp_bool carg_4);

void c_action_implicit_spec();

void c_action_implicit_spec_list__begin();

void c_action_implicit_spec_list(int carg_0);

void c_action_letter_spec(Token_t *carg_0, Token_t *carg_1);

void c_action_letter_spec_list__begin();

void c_action_letter_spec_list(int carg_0);

void c_action_namelist_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, int carg_3);

void c_action_namelist_group_name(Token_t *carg_0);

void c_action_namelist_group_object(Token_t *carg_0);

void c_action_namelist_group_object_list__begin();

void c_action_namelist_group_object_list(int carg_0);

void c_action_equivalence_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_equivalence_set();

void c_action_equivalence_set_list__begin();

void c_action_equivalence_set_list(int carg_0);

void c_action_equivalence_object();

void c_action_equivalence_object_list__begin();

void c_action_equivalence_object_list(int carg_0);

void c_action_common_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, int carg_3);

void c_action_common_block_name(Token_t *carg_0);

void c_action_common_block_object_list__begin();

void c_action_common_block_object_list(int carg_0);

void c_action_common_block_object(Token_t *carg_0, ofp_bool carg_1);

void c_action_variable();

void c_action_designator(ofp_bool carg_0);

void c_action_designator_or_func_ref();

void c_action_substring_range_or_arg_list();

void c_action_substr_range_or_arg_list_suffix();

void c_action_logical_variable();

void c_action_default_logical_variable();

void c_action_scalar_default_logical_variable();

void c_action_char_variable();

void c_action_default_char_variable();

void c_action_scalar_default_char_variable();

void c_action_int_variable();

void c_action_substring_range(ofp_bool carg_0, ofp_bool carg_1);

void c_action_data_ref(int carg_0);

void c_action_part_ref(Token_t *carg_0, ofp_bool carg_1, ofp_bool carg_2);

void c_action_section_subscript(ofp_bool carg_0, ofp_bool carg_1, ofp_bool carg_2, ofp_bool carg_3);

void c_action_section_subscript_list__begin();

void c_action_section_subscript_list(int carg_0);

void c_action_vector_subscript();

void c_action_allocate_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, ofp_bool carg_3, ofp_bool carg_4);

void c_action_image_selector(Token_t *carg_0, Token_t *carg_1);

void c_action_alloc_opt(Token_t *carg_0);

void c_action_alloc_opt_list__begin();

void c_action_alloc_opt_list(int carg_0);

void c_action_cosubscript_list__begin();

void c_action_cosubscript_list(int carg_0, Token_t *carg_1);

void c_action_allocation(ofp_bool carg_0, ofp_bool carg_1);

void c_action_allocation_list__begin();

void c_action_allocation_list(int carg_0);

void c_action_allocate_object();

void c_action_allocate_object_list__begin();

void c_action_allocate_object_list(int carg_0);

void c_action_allocate_shape_spec(ofp_bool carg_0, ofp_bool carg_1);

void c_action_allocate_shape_spec_list__begin();

void c_action_allocate_shape_spec_list(int carg_0);

void c_action_nullify_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_pointer_object();

void c_action_pointer_object_list__begin();

void c_action_pointer_object_list(int carg_0);

void c_action_deallocate_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, ofp_bool carg_3);

void c_action_dealloc_opt(Token_t *carg_0);

void c_action_dealloc_opt_list__begin();

void c_action_dealloc_opt_list(int carg_0);

void c_action_allocate_coarray_spec();

void c_action_allocate_coshape_spec(ofp_bool carg_0);

void c_action_allocate_coshape_spec_list__begin();

void c_action_allocate_coshape_spec_list(int carg_0);

void c_action_primary();

void c_action_level_1_expr(Token_t *carg_0);

void c_action_defined_unary_op(Token_t *carg_0);

void c_action_power_operand(ofp_bool carg_0);

void c_action_power_operand__power_op(Token_t *carg_0);

void c_action_mult_operand(int carg_0);

void c_action_mult_operand__mult_op(Token_t *carg_0);

void c_action_signed_operand(Token_t *carg_0);

void c_action_add_operand(int carg_0);

void c_action_add_operand__add_op(Token_t *carg_0);

void c_action_level_2_expr(int carg_0);

void c_action_power_op(Token_t *carg_0);

void c_action_mult_op(Token_t *carg_0);

void c_action_add_op(Token_t *carg_0);

void c_action_level_3_expr(Token_t *carg_0);

void c_action_concat_op(Token_t *carg_0);

void c_action_rel_op(Token_t *carg_0);

void c_action_and_operand(ofp_bool carg_0, int carg_1);

void c_action_and_operand__not_op(ofp_bool carg_0);

void c_action_or_operand(int carg_0);

void c_action_equiv_operand(int carg_0);

void c_action_equiv_operand__equiv_op(Token_t *carg_0);

void c_action_level_5_expr(int carg_0);

void c_action_level_5_expr__defined_binary_op(Token_t *carg_0);

void c_action_not_op(Token_t *carg_0);

void c_action_and_op(Token_t *carg_0);

void c_action_or_op(Token_t *carg_0);

void c_action_equiv_op(Token_t *carg_0);

void c_action_defined_binary_op(Token_t *carg_0);

void c_action_assignment_stmt(Token_t *carg_0, Token_t *carg_1);

void c_action_pointer_assignment_stmt(Token_t *carg_0, Token_t *carg_1, ofp_bool carg_2, ofp_bool carg_3);

void c_action_data_pointer_object();

void c_action_bounds_spec();

void c_action_bounds_spec_list__begin();

void c_action_bounds_spec_list(int carg_0);

void c_action_bounds_remapping();

void c_action_bounds_remapping_list__begin();

void c_action_bounds_remapping_list(int carg_0);

void c_action_proc_pointer_object();

void c_action_where_stmt__begin();

void c_action_where_stmt(Token_t *carg_0, Token_t *carg_1);

void c_action_where_construct(int carg_0, ofp_bool carg_1, ofp_bool carg_2);

void c_action_where_construct_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_where_body_construct();

void c_action_masked_elsewhere_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_masked_elsewhere_stmt__end(int carg_0);

void c_action_elsewhere_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_elsewhere_stmt__end(int carg_0);

void c_action_end_where_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_forall_construct();

void c_action_forall_construct_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3);

void c_action_forall_header();

void c_action_forall_triplet_spec(Token_t *carg_0, ofp_bool carg_1);

void c_action_forall_triplet_spec_list__begin();

void c_action_forall_triplet_spec_list(int carg_0);

void c_action_forall_body_construct();

void c_action_forall_assignment_stmt(ofp_bool carg_0);

void c_action_end_forall_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_forall_stmt__begin();

void c_action_forall_stmt(Token_t *carg_0, Token_t *carg_1);

void c_action_if_construct();

void c_action_if_then_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_else_if_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4, Token_t *carg_5);

void c_action_else_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3);

void c_action_end_if_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_if_stmt__begin();

void c_action_if_stmt(Token_t *carg_0, Token_t *carg_1);

void c_action_block_construct();

void c_action_specification_part_and_block(int carg_0, int carg_1, int carg_2);

void c_action_block_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3);

void c_action_end_block_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_critical_construct();

void c_action_critical_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3);

void c_action_end_critical_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_case_construct();

void c_action_select_case_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_case_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3);

void c_action_end_select_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_case_selector(Token_t *carg_0);

void c_action_case_value_range();

void c_action_case_value_range_list__begin();

void c_action_case_value_range_list(int carg_0);

void c_action_case_value_range_suffix();

void c_action_case_value();

void c_action_associate_construct();

void c_action_associate_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3);

void c_action_association_list__begin();

void c_action_association_list(int carg_0);

void c_action_association(Token_t *carg_0);

void c_action_selector();

void c_action_end_associate_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_select_type_construct();

void c_action_select_type_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3);

void c_action_select_type(Token_t *carg_0, Token_t *carg_1);

void c_action_type_guard_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_end_select_type_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_do_construct();

void c_action_block_do_construct();

void c_action_do_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4, ofp_bool carg_5);

void c_action_label_do_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4, ofp_bool carg_5);

void c_action_loop_control(Token_t *carg_0, int carg_1, ofp_bool carg_2);

void c_action_do_variable(Token_t *carg_0);

void c_action_end_do();

void c_action_end_do_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_do_term_action_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_cycle_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3);

void c_action_exit_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3);

void c_action_goto_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3);

void c_action_computed_goto_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3);

void c_action_assign_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4, Token_t *carg_5);

void c_action_assigned_goto_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_stmt_label_list();

void c_action_pause_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3);

void c_action_arithmetic_if_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4, Token_t *carg_5);

void c_action_continue_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_stop_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, ofp_bool carg_3);

void c_action_stop_code(Token_t *carg_0);

void c_action_errorstop_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, ofp_bool carg_4);

void c_action_sync_all_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, ofp_bool carg_4);

void c_action_sync_stat(Token_t *carg_0);

void c_action_sync_stat_list__begin();

void c_action_sync_stat_list(int carg_0);

void c_action_sync_images_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, ofp_bool carg_4);

void c_action_image_set(Token_t *carg_0, ofp_bool carg_1);

void c_action_sync_memory_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, ofp_bool carg_4);

void c_action_lock_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, ofp_bool carg_3);

void c_action_lock_stat(Token_t *carg_0);

void c_action_lock_stat_list__begin();

void c_action_lock_stat_list(int carg_0);

void c_action_unlock_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, ofp_bool carg_3);

void c_action_lock_variable();

void c_action_scalar_char_constant();

void c_action_io_unit();

void c_action_file_unit_number();

void c_action_open_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_connect_spec(Token_t *carg_0);

void c_action_connect_spec_list__begin();

void c_action_connect_spec_list(int carg_0);

void c_action_close_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_close_spec(Token_t *carg_0);

void c_action_close_spec_list__begin();

void c_action_close_spec_list(int carg_0);

void c_action_read_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, ofp_bool carg_3);

void c_action_write_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, ofp_bool carg_3);

void c_action_print_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, ofp_bool carg_3);

void c_action_io_control_spec(ofp_bool carg_0, Token_t *carg_1, ofp_bool carg_2);

void c_action_io_control_spec_list__begin();

void c_action_io_control_spec_list(int carg_0);

void c_action_input_item();

void c_action_input_item_list__begin();

void c_action_input_item_list(int carg_0);

void c_action_output_item();

void c_action_output_item_list__begin();

void c_action_output_item_list(int carg_0);

void c_action_io_implied_do();

void c_action_io_implied_do_object();

void c_action_io_implied_do_control(ofp_bool carg_0);

void c_action_dtv_type_spec(Token_t *carg_0);

void c_action_wait_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_wait_spec(Token_t *carg_0);

void c_action_wait_spec_list__begin();

void c_action_wait_spec_list(int carg_0);

void c_action_backspace_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, ofp_bool carg_3);

void c_action_endfile_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, ofp_bool carg_4);

void c_action_rewind_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, ofp_bool carg_3);

void c_action_position_spec(Token_t *carg_0);

void c_action_position_spec_list__begin();

void c_action_position_spec_list(int carg_0);

void c_action_flush_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, ofp_bool carg_3);

void c_action_flush_spec(Token_t *carg_0);

void c_action_flush_spec_list__begin();

void c_action_flush_spec_list(int carg_0);

void c_action_inquire_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, ofp_bool carg_4);

void c_action_inquire_spec(Token_t *carg_0);

void c_action_inquire_spec_list__begin();

void c_action_inquire_spec_list(int carg_0);

void c_action_format_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_format_specification(ofp_bool carg_0);

void c_action_format_item(Token_t *carg_0, ofp_bool carg_1);

void c_action_format_item_list__begin();

void c_action_format_item_list(int carg_0);

void c_action_v_list_part(Token_t *carg_0, Token_t *carg_1);

void c_action_v_list__begin();

void c_action_v_list(int carg_0);

void c_action_main_program__begin();

void c_action_main_program(ofp_bool carg_0, ofp_bool carg_1, ofp_bool carg_2);

void c_action_ext_function_subprogram(ofp_bool carg_0);

void c_action_program_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3);

void c_action_end_program_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_module();

void c_action_module_stmt__begin();

void c_action_module_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3);

void c_action_end_module_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_module_subprogram_part(int carg_0);

void c_action_module_subprogram(ofp_bool carg_0);

void c_action_use_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4, ofp_bool carg_5, ofp_bool carg_6, ofp_bool carg_7);

void c_action_module_nature(Token_t *carg_0);

void c_action_rename_list__begin();

void c_action_rename_list(int carg_0);

void c_action_only(ofp_bool carg_0, ofp_bool carg_1, ofp_bool carg_2);

void c_action_only_list__begin();

void c_action_only_list(int carg_0);

void c_action_submodule(ofp_bool carg_0);

void c_action_submodule_stmt__begin();

void c_action_submodule_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3);

void c_action_parent_identifier(Token_t *carg_0, Token_t *carg_1);

void c_action_end_submodule_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_block_data();

void c_action_block_data_stmt__begin();

void c_action_block_data_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_end_block_data_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4, Token_t *carg_5);

void c_action_interface_block();

void c_action_interface_specification();

void c_action_interface_stmt__begin();

void c_action_interface_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, ofp_bool carg_4);

void c_action_end_interface_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, ofp_bool carg_4);

void c_action_interface_body(ofp_bool carg_0);

void c_action_procedure_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3);

void c_action_generic_spec(Token_t *carg_0, Token_t *carg_1, int carg_2);

void c_action_dtio_generic_spec(Token_t *carg_0, Token_t *carg_1, int carg_2);

void c_action_import_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, ofp_bool carg_3);

void c_action_external_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_procedure_declaration_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, ofp_bool carg_3, int carg_4);

void c_action_proc_interface(Token_t *carg_0);

void c_action_proc_attr_spec(Token_t *carg_0, Token_t *carg_1, int carg_2);

void c_action_proc_decl(Token_t *carg_0, ofp_bool carg_1);

void c_action_proc_decl_list__begin();

void c_action_proc_decl_list(int carg_0);

void c_action_intrinsic_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_function_reference(ofp_bool carg_0);

void c_action_call_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, ofp_bool carg_3);

void c_action_procedure_designator();

void c_action_actual_arg_spec(Token_t *carg_0);

void c_action_actual_arg_spec_list__begin();

void c_action_actual_arg_spec_list(int carg_0);

void c_action_actual_arg(ofp_bool carg_0, Token_t *carg_1);

void c_action_function_subprogram(ofp_bool carg_0, ofp_bool carg_1);

void c_action_function_stmt__begin();

void c_action_function_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, ofp_bool carg_4, ofp_bool carg_5);

void c_action_proc_language_binding_spec();

void c_action_t_prefix(int carg_0);

void c_action_prefix_spec(ofp_bool carg_0);

void c_action_t_prefix_spec(Token_t *carg_0);

void c_action_result_name();

void c_action_end_function_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_subroutine_stmt__begin();

void c_action_subroutine_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, ofp_bool carg_4, ofp_bool carg_5, ofp_bool carg_6, ofp_bool carg_7);

void c_action_dummy_arg(Token_t *carg_0);

void c_action_dummy_arg_list__begin();

void c_action_dummy_arg_list(int carg_0);

void c_action_end_subroutine_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_entry_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, ofp_bool carg_4, ofp_bool carg_5);

void c_action_return_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, ofp_bool carg_3);

void c_action_contains_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_separate_module_subprogram(ofp_bool carg_0, ofp_bool carg_1);

void c_action_separate_module_subprogram__begin();

void c_action_mp_subprogram_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_end_mp_subprogram_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, Token_t *carg_3, Token_t *carg_4);

void c_action_stmt_function_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, ofp_bool carg_3);

void c_action_end_of_stmt(Token_t *carg_0);

void c_action_start_of_file(const char *carg_0);

void c_action_end_of_file(const char *carg_0);

void c_action_cleanUp();

void c_action_rice_image_selector(Token_t *carg_0);

void c_action_rice_allocate_coarray_spec(int carg_0, Token_t *carg_1);

void c_action_rice_co_with_team_stmt(Token_t *carg_0, Token_t *carg_1);

void c_action_rice_end_with_team_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_rice_finish_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2);

void c_action_rice_end_finish_stmt(Token_t *carg_0, Token_t *carg_1);

void c_action_rice_spawn_stmt(Token_t *carg_0, Token_t *carg_1, Token_t *carg_2, ofp_bool carg_3);

void c_action_next_token(Token_t *carg_0);

#ifdef __cplusplus
}
#endif
