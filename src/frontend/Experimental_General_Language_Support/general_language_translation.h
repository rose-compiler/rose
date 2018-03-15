#ifndef GENERAL_LANGUAGE_TRANSLATION_H
#define GENERAL_LANGUAGE_TRANSLATION_H

namespace General_Language_Translation
   {

  // Enum for different types of expressions.  These are used with the SgUntypedExpression IR nodes.
     enum ExpressionKind
        {
          e_unknown,

       // Access modifiers
       // --------------
          e_access_modifier_public,
          e_access_modifier_private,

       // Storage modifiers
       // --------------
          e_storage_modifier_contiguous,
          e_storage_modifier_external,

       // Type modifiers
       // --------------
          e_type_modifier_allocatable,
          e_type_modifier_asynchronous,
          e_type_modifier_const,
          e_type_modifier_intent_in,
          e_type_modifier_intent_out,
          e_type_modifier_intent_inout,
          e_type_modifier_intrinsic,
          e_type_modifier_optional,
          e_type_modifier_pointer,
          e_type_modifier_protected,
          e_type_modifier_save,
          e_type_modifier_target,
          e_type_modifier_value,
          e_type_modifier_volatile,

       // Function modifiers
       // ------------------
          e_function_modifier_elemental,
          e_function_modifier_impure,
          e_function_modifier_module,
          e_function_modifier_pure,
          e_function_modifier_recursive,

       // Operators
       // ---------

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
          e_referenceExpression,

       // Special expressions denoting array declaration type
          e_array_shape,
          e_explicit_shape,
          e_assumed_or_implied_shape,
          e_assumed_shape,
          e_assumed_size,

       // Explicitly for Jovial but try to reuse for Fortran
          e_explicit_dimension,
          e_star_dimension,

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

   } // namespace General_Language_Translation 

#endif
