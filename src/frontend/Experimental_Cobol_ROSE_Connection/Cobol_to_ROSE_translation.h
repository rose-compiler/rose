#ifndef COBOL_TO_ROSE_TRANSLATION_H
#define COBOL_TO_ROSE_TRANSLATION_H

namespace Cobol_ROSE_Translation
   {

  // Enum for program specifiers
     enum ProgramSpec
        {
          e_unknown,

          e_recursive,
          e_initial,

          e_last
        };


#if 0
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

       // Bit operator
          e_notOperator,

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

       // Dereference operator
          e_derefOperator,

       // Assignment operator
          e_assignOperator,

       // Unary operators
          e_unaryPlusOperator,
          e_unaryMinusOperator,

       // for optional sign (unary operator)
          e_notAnOperator,

       // Expressions
       // -----------

          e_referenceExpression,

          e_last
        };
#endif

   } // namespace Cobol_ROSE_Translation 

#endif
