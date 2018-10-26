#ifndef FORTRAN_TO_ROSE_TRANSLATION_H
#define FORTRAN_TO_ROSE_TRANSLATION_H

namespace Fortran_ROSE_Translation
   {

  // Enum for different types of expressions.  These are used with the SgUntypedExpression IR nodes.
     enum ExpressionKind
        {
          e_unknown,

       // Attributes
       // ----------
          e_public,
          e_private,
          e_allocatable,
          e_asynchronous,
          e_contiguous,
          e_external,
          e_intent_in,
          e_intent_out,
          e_intent_inout,
          e_intrinsic,
          e_optional,
          e_parameter,
          e_pointer,
          e_protected,
          e_save,
          e_target,
          e_value,
          e_volatile,

          e_elemental,
          e_impure,
          e_module,
          e_pure,
          e_recursive,

       // Operators
       // ---------

       // Arithmetic operators
          e_exponentiateOperator,
          e_plusOperator,
          e_minusOperator,
          e_modOperator,
          e_multiplyOperator,
          e_divideOperator,

       // Logical operators
          e_andOperator,
          e_orOperator,
          e_xorOperator,
          e_equivOperator,

       // Relational operators
          e_lessThanOperator,
          e_greaterThanOperator,
          e_lessThanOrEqualOperator,
          e_greaterThanOrEqualOperator,
          e_equalOperator,
          e_notEqualOperator,

       // Unary operators
          e_unaryPlusOperator,
          e_unaryMinusOperator,

       // for optional sign (unary operator)
          e_notAnOperator,

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

       // Statements
       // ----------
          e_do_stmt,

       // CUDA Attributes
       // ---------------
          e_cuda_device,
          e_cuda_host,
          e_cuda_kernel,

          e_cuda_global,
          e_cuda_grid_global,
          e_cuda_managed,
          e_cuda_constant,
          e_cuda_shared,
          e_cuda_pinned,
          e_cuda_texture,

          e_last
        };

   } // namespace Fortran_ROSE_Translation

#endif
