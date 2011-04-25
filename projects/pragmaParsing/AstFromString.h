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
  bool afs_match_additive_expression();
  bool afs_match_argument_expression_list();
  bool afs_match_assignment_expression();
  bool afs_match_cast_expression();
  bool afs_match_char(char c);
  bool afs_match_conditional_expression();
  bool afs_match_constant();
  bool afs_match_expression();
  bool afs_match_expression();
  bool afs_match_identifier();
  bool afs_match_integer_const(int * result);
  bool afs_match_multiplicative_expression();
  bool afs_match_postfix_expression();
  bool afs_match_primary_expression();
  bool afs_match_specifier_qualifier_list(std::vector<SgNode*> &);
  bool afs_match_substr(const char* substr, bool checkTrail = true);
  bool afs_match_type_name();
  bool afs_match_type_qualifier();
  bool afs_match_type_specifier();
  bool afs_match_unary_expression();
  bool afs_skip_whitespace();

}

#endif /* __AST_FROM_STRING_H__ */
