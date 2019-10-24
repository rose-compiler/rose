#ifndef JOVIAL_TO_ROSE_TRANSLATION_H
#define JOVIAL_TO_ROSE_TRANSLATION_H

namespace Jovial_ROSE_Translation
   {

  // TODO - split out the StatementEnums?

  // Enum for different types of expressions (used with untyped IR nodes).
  //
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

          e_literalExpression,
          e_referenceExpression,

       // Expressions for loop control
       // ----------------------------
          e_by_phrase_expr,
          e_then_phrase_expr,
          e_while_phrase_expr,

       // Expressions for table entry words-per-entry type
       // ------------------------------------------------
          e_words_per_entry_w,
          e_words_per_entry_v,

       // Statements
       // ----------

       // For loop control statements
       // ---------------------------
          e_for_while_by_stmt,
          e_for_while_then_stmt,
          e_for_by_while_stmt,
          e_for_then_while_stmt,


       // Declarations
       // ------------

          e_simple_block_type_declaration,
          e_block_type_declaration,
          e_simple_table_type_declaration,
          e_table_type_declaration,
          e_anonymous_declaration,

       // Directives
       // -----------
          e_compool_directive_stmt,
          e_reducible_directive_stmt,
          e_order_directive_stmt,

          e_last
        };

   } // namespace Jovial_ROSE_Translation

#endif
