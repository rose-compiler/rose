#ifndef GENERAL_LANGUAGE_TRANSLATION_H
#define GENERAL_LANGUAGE_TRANSLATION_H

namespace General_Language_Translation
   {

  // Forward declarations
     struct FormalParameter;
     struct LocationSpecifier;
     struct StructureSpecifier;

     enum SubroutineAttribute
        {
           e_subroutine_attr_none = 0,

        // Jovial
           e_subroutine_attr_rent,
           e_subroutine_attr_rec,

           e_subroutine_attr_last
        };

  // Enum for different types of expressions (used with untyped IR nodes).
  //
     enum ExpressionKind
        {
          e_none = 0,
          e_unknown = 1,

       // Access modifiers
       // --------------
          e_access_modifier_public,
          e_access_modifier_private,

       // Storage modifiers
       // --------------
          e_storage_modifier_contiguous,
          e_storage_modifier_external,
          e_storage_modifier_static,
          e_storage_modifier_location,
          e_storage_modifier_jovial_def,     /* SimpleDef or CompoundDef */
          e_storage_modifier_jovial_ref,     /* SimpleRef or CompoundRef */

       // Type modifiers
       // --------------
          e_type_modifier_list,
          e_type_modifier_allocatable,
          e_type_modifier_asynchronous,
          e_type_modifier_const,
          e_type_modifier_intent_in,
          e_type_modifier_intent_out,
          e_type_modifier_intent_inout,
          e_type_modifier_intrinsic,
          e_type_modifier_optional,
          e_type_modifier_packing_d,
          e_type_modifier_packing_m,
          e_type_modifier_packing_n,
          e_type_modifier_pointer,
          e_type_modifier_protected,
          e_type_modifier_round,
          e_type_modifier_save,
          e_type_modifier_target,
          e_type_modifier_truncate,
          e_type_modifier_volatile,
          e_type_modifier_z,

       // Function modifiers
       // ------------------
          e_function_modifier_list,
          e_function_modifier_none,
          e_function_modifier_elemental,
          e_function_modifier_impure,
          e_function_modifier_module,
          e_function_modifier_pure,
          e_function_modifier_recursive,
          e_function_modifier_reentrant,

       // Function formal parameters
       // --------------------------
          e_param_binding_value,
          e_param_binding_reference,
          e_param_binding_result,

       // Structure modifiers
          e_struct_modifier_list,
          e_struct_item_modifier_list,

       // Operators
       // ---------

       // Assignment operator
          e_operator_assign,

       // Arithmetic operators
          e_operator_exponentiate,
          e_operator_concatenate,
          e_operator_add,
          e_operator_subtract,
          e_operator_mod,
          e_operator_multiply,
          e_operator_divide,

       // Logical operators
          e_operator_and,
          e_operator_or,
          e_operator_xor,
          e_operator_equiv,

       // Relational operators
          e_operator_less_than,
          e_operator_greater_than,
          e_operator_less_than_or_equal,
          e_operator_greater_than_or_equal,
          e_operator_equality,
          e_operator_not_equal,

       // Unary operators
          e_operator_unary_plus,
          e_operator_unary_minus,
          e_operator_unary_not,

       // for optional sign
          e_operator_unity,

       // Expressions
       // -----------

          e_literalExpression,

          e_function_reference,
          e_procedure_call,
          e_argument_list,
          e_argument_keyword,

          e_variable_reference,
          e_array_reference,

          e_case_range,
          e_case_selector,

       // Special expressions denoting array declaration type
          e_array_shape,
          e_explicit_shape,
          e_assumed_or_implied_shape,
          e_assumed_shape,
          e_assumed_size,

       // Expressions for array indexing
          e_array_subscripts,
          e_array_index_triplet,
          e_section_subscripts,

       // Explicitly for Jovial but try to reuse for Fortran
          e_explicit_dimension,
          e_star_dimension,

          e_star_expression,

       // Expressions for initialization (preset in Jovial)
          e_initializer,
          e_struct_initializer,

       // Statements
       // ----------

       // General
          e_switch_stmt,
          e_end_switch_stmt,
          e_case_option_stmt,
          e_case_default_option_stmt,

          e_end_proc_ref_stmt,
          e_end_proc_def_stmt,

          e_define_directive_stmt,

       // Fortran specific
          e_fortran_dimension_stmt,
          e_fortran_import_stmt,

          e_fortran_if_stmt,
          e_fortran_if_then_stmt,
          e_fortran_else_if_stmt,
          e_fortran_else_stmt,
          e_fortran_end_do_stmt,

          e_fortran_do_concurrent_stmt,
          e_fortran_concurrent_header,
          e_fortran_concurrent_control,
          e_fortran_concurrent_locality,

          e_fortran_forall_stmt,
          e_fortran_end_forall_stmt,

          e_fortran_sync_all_stmt,
          e_fortran_sync_images_stmt,
          e_fortran_sync_memory_stmt,
          e_fortran_sync_team_stmt,
          e_fortran_lock_stmt,
          e_fortran_unlock_stmt,

          e_fortran_sync_stat_list,
          e_fortran_sync_stat_stat,
          e_fortran_sync_stat_errmsg,
          e_fortran_stat_acquired_lock,

       // Jovial specific
          e_jovial_compool_stmt,

       // CUDA Attributes
       // ---------------
          e_cuda_host,
          e_cuda_device,
          e_cuda_global_function,
          e_cuda_grid_global, /* all threads within a thread group guaranteed to be coresident */

          e_cuda_global,
          e_cuda_device_memory,
          e_cuda_managed,
          e_cuda_constant,
          e_cuda_shared,
          e_cuda_pinned,
          e_cuda_texture,

          e_last
        };

      struct FormalParameter
        {
           FormalParameter()
             : name(std::string("")), binding(General_Language_Translation::e_none) {} // want to use C++11 = delete
           FormalParameter(const std::string &param_name, const ExpressionKind &param_binding)
             : name(param_name), binding(param_binding) {}
           std::string name;
           ExpressionKind binding;
        };

      struct LocationSpecifier {
         LocationSpecifier() : start_bit(nullptr), start_word(nullptr) {}
         LocationSpecifier(SgExpression* sbit, SgExpression* sword) : start_bit(sbit), start_word(sword) {}
         SgExpression* start_bit;
         SgExpression* start_word;
      };

      struct StructureSpecifier {
         StructureSpecifier()                  : bits_per_entry(nullptr), is_parallel(false) {}
         StructureSpecifier(SgExpression* bpe) : bits_per_entry(bpe),     is_parallel(false) {}
         StructureSpecifier(bool isp)          : bits_per_entry(nullptr), is_parallel(isp)   {}
         SgExpression* bits_per_entry;
         bool is_parallel;
      };

   } // namespace General_Language_Translation 

#endif
