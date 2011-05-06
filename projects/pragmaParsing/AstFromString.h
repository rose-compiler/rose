#ifndef __AST_FROM_STRING_H__
#define __AST_FROM_STRING_H__
/**
 * Liao 4/13/2011 
 */

#include "rose.h"
#include <iostream>
#include <vector>
namespace AstFromString
{
  // maximum length for a buffer for a variable, constant, or pragma construct name
  #define OFS_MAX_LEN 256
  // A namespace scope char* to avoid passing and returning target c string for every and each function
  // current characters being scanned, what is pointed is const, the pointer is not const. 
  extern const char* c_char; 
  extern SgNode* c_sgnode; // current anchor SgNode associated with parsing. It will serve as a start point to find enclosing scopes for resolving identifiers/symbols 
  extern SgNode* c_parsed_node; // the generated SgNode from a parsing function.

  // building blocks to parse C-grammar strings to AST nodes
  bool afs_is_digit();
  bool afs_is_identifier_char();
  bool afs_is_letter();
  bool afs_is_lower_letter();
  bool afs_is_upper_letter();
  bool afs_match_char(char c);
  //! Match a given sub c string from the input c string, again skip heading space/tabs if any
  //  checkTrail: Check the immediate following character after the match, it must be one of
  //      whitespace, end of str, newline, tab, (, ), or '!', etc.
  //      Set to true by default, used to ensure the matched substr is a full identifier/keywords.
  //      If try to match operators (+=, etc), please set checkTrail to false!!
  //
  //      But Fortran OpenMP allows blanks/tabs to be ignored between certain pair of keywords:
  //      e.g: end critical == endcritical  , parallel do == paralleldo
  //      to match the 'end' and 'parallel', we have to skip trail checking.
  // return values: 
  //    true: find a match, the current char is pointed to the next char after the substr
  //    false: no match, the current char is intact

  bool afs_match_substr(const char* substr, bool checkTrail = true);
  bool afs_skip_whitespace();
  bool afs_match_identifier();
  bool afs_match_integer_const(int * result);

  // entry point, declarations, types
  // Follow the order of http://www.antlr.org/grammar/1153358328744/C.g
  //-----------------------------------------
  bool afs_match_translation_unit();
  bool afs_match_external_declaration();
  bool afs_match_function_definition();
  bool afs_match_declaration();
  bool afs_match_declaration_specifiers();
  bool afs_match_init_declarator_list();
  bool afs_match_init_declarator();

  bool afs_match_storage_class_specifier();

  bool afs_match_type_specifier();
  // type_id in ANTLR grammar
  //struct_or_union_specifier
  //struct_or_union
  //struct_declaration_list
  //struct_declaration
  bool afs_match_specifier_qualifier_list(std::vector<SgNode*> &);
  //struct_declarator_list
  //struct_declarator
  // enum_specifier
  //enumerator_list
  // enumerator
  bool afs_match_type_qualifier();
  //declarator
  //direct_declarator
  // declarator_suffix
  // pointer
  //parameter_type_list
  // parameter_list
  //parameter_declaration
  // identifier_list
  bool afs_match_type_name(); 
  //abstract_declarator
  //direct_abstract_declarator
  //abstract_declarator_suffix
  // initializer
  // initializer_list


  // expressions 
  //-----------------------------------------
  bool afs_match_argument_expression_list();
  bool afs_match_additive_expression();
  bool afs_match_multiplicative_expression();
  bool afs_match_cast_expression();
  bool afs_match_unary_expression();
  bool afs_match_postfix_expression();
  //unary_operator, included in unary_expression in this implementation
  bool afs_match_primary_expression();
  bool afs_match_constant();
  bool afs_match_expression();
  bool afs_match_constant_expression();
  bool afs_match_assignment_expression();
  bool afs_match_lvalue();
  // assignment_operator, included in assignment_expression in this implementation
  bool afs_match_conditional_expression();

  bool afs_match_logical_or_expression();
  bool afs_match_logical_and_expression();
  bool afs_match_inclusive_or_expression();
  bool afs_match_exclusive_or_expression();
  bool afs_match_and_expression();
  bool afs_match_equality_expression();
  bool afs_match_relational_expression();
  bool afs_match_shift_expression();

  // statements
  //-----------------------------------------
  bool afs_match_statement();
  bool afs_match_labeled_statement();
  bool afs_match_compound_statement();
  bool afs_match_expression_statement();
  bool afs_match_selection_statement();
  bool afs_match_iteration_statement();
  bool afs_match_jump_statement();

}

#endif /* __AST_FROM_STRING_H__ */
