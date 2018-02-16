#ifndef FORTRAN_TO_ROSE_TRANSLATION_H
#define FORTRAN_TO_ROSE_TRANSLATION_H

namespace Fortran_ROSE_Translation
   {

  // Enum for different types of expressions.  These are used with the SgUntypedExpression IR nodes.
     enum ExpressionKind
        {
          e_unknown,

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
          e_array_range,
          e_array_shape,
          e_explicit_shape,
          e_assumed_or_implied_shape_array,
          e_assumed_shape_array,
          e_assumed_size_array,

          e_last
        };

   } // namespace Jovial_ROSE_Translation 

#endif
