#ifndef __AST_FROM_STRING_H__
#define __AST_FROM_STRING_H__
/**
 * Liao 4/13/2011 
 */

#include "sage3basic.h"
#include <iostream>
#include <vector>

/*! 
 * Support creating simple recursive descent parsers of source code annotations
 * 
 * The namespace, AstFromString, collects a set of helper functions (or parser building blocks) that operate on an input string to create simple recursive descent parsers for 
 * source code annotations(C/C++ pragmas or Fortran comments). 
 */
namespace AstFromString
{

 //--------------------------------------------------------------
 //@{
 /*! @name Namespace scope variables
   \brief  Semi-global variables
 */

  //! maximum length for a buffer for a variable, constant, or pragma construct name
  #define OFS_MAX_LEN 256

  //! A namespace scope char* to avoid passing and returning a target c string for every and each function
  //! current characters being scanned, what is pointed is const, the pointer itself is not const. 
  ROSE_DLL_API extern const char* c_char; 

  //! current anchor SgNode associated with parsing. It will serve as a start point to find enclosing scopes for resolving identifiers/symbols 
  ROSE_DLL_API extern SgNode* c_sgnode; 

  //! Store the AST substree (expression, statement) generated from a helper function.
  ROSE_DLL_API extern SgNode* c_parsed_node; 

  //@}

 
  //@{
 /*! @name Character check , without side effect on the current position
   \brief  utility functions for checking characters, without changing the current position of the input string being parsed.
 */

  //! Check if the current character is a digit. The current position of the input string remains unchanged either way.
  bool afs_is_digit();

  //! Check if the current character is a legal identifier character, including letters, digits, '_' and '$' (For fortran). No side effect on the current position.
  bool afs_is_identifier_char();

  //! Check if the current character is a letter. No side effect on the current position of the input string. 
  bool afs_is_letter();

  //! Check if the current character is a lower case letter
  bool afs_is_lower_letter();

  //! Check if the current character is a upper case letter
  bool afs_is_upper_letter();

  //@}

//@{
 /*! @name Match with side effects (set c_parsed_node and advance the current position of character) if successful. Return false and have no side effects if the match fails.
   \brief  utility functions for matching string, whitespace, identifiers, const etc. Successful match will advance the current position.
          entry point, declarations, types
         Follow the order of http://www.antlr.org/grammar/1153358328744/C.g
 */

  //! match a char, advance one position if successful.
  ROSE_DLL_API bool afs_match_char(char c);

  /*! \brief Match a sub string: a given sub c string from the input c string, again skip heading space/tabs if any
   * 
   * checkTrail: Check the immediate following character after the match, it must be one of  whitespace, end of str, newline, tab, (, ), or '!', etc.
   *       It is set to true by default, used to ensure the matched substr is a full identifier/keywords.
   *  Note: If try to match non-identifier, such as operators +=, etc), please set checkTrail to false!!
   *       But Fortran OpenMP allows blanks/tabs to be ignored between certain pair of keywords:
   *       e.g: end critical == endcritical  , parallel do == paralleldo
   *       to match the 'end' and 'parallel', we have to skip trail checking.
   *  return values: 
   *   -  true: find a match, the current char is pointed to the next char after the substr
   *   - false: no match, the current char is intact
   */   
  ROSE_DLL_API bool afs_match_substr(const char* substr, bool checkTrail = true);

  //! Match and skip whitespace
  ROSE_DLL_API bool afs_skip_whitespace();

 
  //! Match identifier, move to the next character if successful. The identifier could be a name of a type, function, variable, or label. The resolved identifier (type or variable/function/label name) is stored into c_parsed_node.
  ROSE_DLL_API bool afs_match_identifier();

  //! Match an integer constant, store it into result
  ROSE_DLL_API bool afs_match_integer_const(int * result);

  //! Match a translation unit. Not yet implemented.
  ROSE_DLL_API bool afs_match_translation_unit();

  //! Match an external declaration. Not yet implemented.
  ROSE_DLL_API bool afs_match_external_declaration();

  //! Match a function definition. Not yet implemented.
  ROSE_DLL_API bool afs_match_function_definition();

  //! Match a declaration. Not yet implemented. 
  ROSE_DLL_API bool afs_match_declaration();

  //!Match declaration specifiers. Not yet implemented.
  ROSE_DLL_API bool afs_match_declaration_specifiers();

  //!Match an init declarator list. Not yet implemented.
  ROSE_DLL_API bool afs_match_init_declarator_list();

  //! Match init declarator. Not yet implemented.
  ROSE_DLL_API bool afs_match_init_declarator();

  //! Match a storage class specifier. Not yet implemented.
  ROSE_DLL_API bool afs_match_storage_class_specifier();

  //! Match a type specifier : 'void' | 'char' | 'short' | 'int' | 'long' | 'float' | 'double' | 'signed' | 'unsigned' | struct_or_union_specifier | enum_specifier | type_id. The recognized type is stored in c_parsed_node.
  ROSE_DLL_API bool afs_match_type_specifier();
  // type_id in ANTLR grammar
  //struct_or_union_specifier
  //struct_or_union
  //struct_declaration_list
  //struct_declaration
  //! Match a list of specifiers and qualifiers : : ( type_qualifier | type_specifier )+
  ROSE_DLL_API bool afs_match_specifier_qualifier_list(std::vector<SgNode*> &);
  //struct_declarator_list
  //struct_declarator
  // enum_specifier
  //enumerator_list
  // enumerator
  //! Match a type qualifier : 'const' | 'volatile'
  ROSE_DLL_API bool afs_match_type_qualifier();
  //declarator
  //direct_declarator
  // declarator_suffix
  // pointer
  //parameter_type_list
  // parameter_list
  //parameter_declaration
  // identifier_list
  //! Match a type name : specifier_qualifier_list abstract_declarator? 
  ROSE_DLL_API bool afs_match_type_name(); 
  //abstract_declarator
  //direct_abstract_declarator
  //abstract_declarator_suffix
  // initializer
  // initializer_list


//@{
 /*! @name Expression
   \brief  utility functions for parsing expressions and generate AST pieces for them. Successful match will advance the current position. The function names and grammars largely follow the conventions used in http://www.antlr.org/grammar/1153358328744/C.g. For example in the grammar shown,  | means or, * means 0 or more repetition of the previous term , ? means 0 or 1 occurrence of the previous term. + means 1 or more occurrence. 
 */
  //-----------------------------------------
  //!  Grammar:  argument_expression_list : assignment_expression (',' assignment_expression)*  one assignment_expression, followed by optionally arbitrary numbers of (',' assignment_expression)
  ROSE_DLL_API bool afs_match_argument_expression_list();

  //!  Grammar:  additive_expression : (multiplicative_expression) ('+' multiplicative_expression | '-' multiplicative_expression)*
  ROSE_DLL_API bool afs_match_additive_expression();

  //!  multiplicative_expression : (cast_expression) ('*' cast_expression | '/' cast_expression | '%' cast_expression)*
  ROSE_DLL_API bool afs_match_multiplicative_expression();

  //! cast_expression  : '(' type_name ')' cast_expression   | unary_expression 
  ROSE_DLL_API bool afs_match_cast_expression();

  //!      unary_expression : postfix_expression | INC_OP unary_expression  | DEC_OP unary_expression   | unary_operator cast_expression  | SIZEOF unary_expression  | SIZEOF '(' type_name ')'
  ROSE_DLL_API bool afs_match_unary_expression();

 //! postfix_expression :   primary_expression ( '[' expression ']' | '(' ')' | '(' argument_expression_list ')' | '.' IDENTIFIER | '->' IDENTIFIER | '++' | '--' )*
  ROSE_DLL_API bool afs_match_postfix_expression();

  //! primary_expression : IDENTIFIER | constant | '(' expression ')'
  ROSE_DLL_API bool afs_match_primary_expression();

  //! Only integer constant is supported for now. Full grammar is constant :   HEX_LITERAL |   OCTAL_LITERAL  |   DECIMAL_LITERAL    |   CHARACTER_LITERAL   |   STRING_LITERAL  |   FLOATING_POINT_LITERAL
  ROSE_DLL_API bool afs_match_constant(); 

  //! expression : assignment_expression (',' assignment_expression)*.  Match one or more assignment_expression
  ROSE_DLL_API bool afs_match_expression();

  //! constant_expression : conditional_expression
  ROSE_DLL_API bool afs_match_constant_expression();

  //! assignment_expression : lvalue assignment_operator assignment_expression | conditional_expression
  ROSE_DLL_API bool afs_match_assignment_expression();
  
  //! lvalue: unary_expression
  ROSE_DLL_API bool afs_match_lvalue();
  // assignment_operator, included in assignment_expression in this implementation
  //! conditional_expression : logical_or_expression ('?' expression ':' conditional_expression)?  '?' means 0 or 1 occurrence 
  ROSE_DLL_API bool afs_match_conditional_expression();

  //! logical_or_expression : logical_and_expression ('||' logical_and_expression)*
  ROSE_DLL_API bool afs_match_logical_or_expression();

  //! logical_and_expression : inclusive_or_expression ('&&' inclusive_or_expression)*
  ROSE_DLL_API bool afs_match_logical_and_expression();

  //! inclusive_or_expression : exclusive_or_expression ('|' exclusive_or_expression)*
  ROSE_DLL_API bool afs_match_inclusive_or_expression();

  //! exclusive_or_expression : and_expression ('^' and_expression)*
  ROSE_DLL_API bool afs_match_exclusive_or_expression();

  //! and_expression : equality_expression ('&' equality_expression)*
  ROSE_DLL_API bool afs_match_and_expression();

  //! equality_expression : relational_expression (('=='|'!=') relational_expression)*
  ROSE_DLL_API bool afs_match_equality_expression();

  //! relational_expression : shift_expression (('<'|'>'|'<='|'>=') shift_expression)*
  ROSE_DLL_API bool afs_match_relational_expression();

  //! shift_expression : additive_expression (('<<'|'>>') additive_expression)*
  ROSE_DLL_API bool afs_match_shift_expression();
 //@}

  //-----------------------------------------
//@{
 /*! @name Statement
   \brief  utility functions to parse a few statements. Successful match will advance the current position. INCOMPLETE now, Don't use them yet!
 */
  //! match any statement, not complete yet. Don't use it yet . : labeled_statement  | compound_statement | expression_statement | selection_statement | iteration_statement | jump_statement
  ROSE_DLL_API bool afs_match_statement();

  //! Match labeled statement : IDENTIFIER ':' statement | 'case' constant_expression ':' statement | 'default' ':' statement
  ROSE_DLL_API bool afs_match_labeled_statement();
  //! Not yet implemented. Match compound statement: '{' declaration* statement_list? '}'
  ROSE_DLL_API bool afs_match_compound_statement();
  //! Match expression statement: : ';'  | expression ';'
  ROSE_DLL_API bool afs_match_expression_statement();
  //! Match selection statement: IF '(' expression ')' statement  | IF '(' expression ')' statement ELSE statement  | SWITCH '(' expression ')' statement
  ROSE_DLL_API bool afs_match_selection_statement();
  //! Match an iteration statement: : 'while' '(' expression ')' statement   | 'do' statement 'while' '(' expression ')' ';' | 'for' '(' expression_statement expression_statement expression? ')' statement
  ROSE_DLL_API bool afs_match_iteration_statement();
  //! Match a jump statement : 'goto' IDENTIFIER ';' | 'continue' ';'  | 'break' ';' | 'return' ';' | 'return' expression ';'
  ROSE_DLL_API bool afs_match_jump_statement();

  //@}
}

#endif /* __AST_FROM_STRING_H__ */
