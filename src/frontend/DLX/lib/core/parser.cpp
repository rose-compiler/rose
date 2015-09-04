/*!
 * 
 * \file lib/core/parser.cpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "sage3basic.h"

#include "DLX/Core/parser.hpp"
#include <cstring>

namespace DLX {

namespace Frontend {

/*!
 * \addtogroup grp_dlx_core_frontend
 * @{
 */

inline bool whitespace(const char & c) {
  return (c == ' ');
}

void skip_whitespace(std::string & directive_str) {
  while (whitespace(directive_str[0])) directive_str = directive_str.substr(1);
}

void skip_parenthesis(std::string & directive_str) {
  assert(directive_str[0] == '(');
  size_t count = 1;
  size_t depth = 1;
  while (depth > 0) {
    if (directive_str[count] == '(') depth++;
    else if (directive_str[count] == ')') depth--;
    count++;
    assert(count <= directive_str.size());
  }
  directive_str = directive_str.substr(count);
}

bool consume_label(std::string & directive_str, const std::string & label) {
  skip_whitespace(directive_str);

  if (directive_str.find(label) != 0) return false;

  if ((directive_str.size() > label.size()) && !whitespace(directive_str[label.size()]) && (directive_str[label.size()] != '(') && (directive_str[label.size()] != '[')) return false;

  directive_str = directive_str.substr(label.size());

  skip_whitespace(directive_str);

  return true;
}

bool Parser::s_singleton = true;

Parser::Parser(std::string & directive_str, SgLocatedNode * directive_node) {
  assert(s_singleton == true);
  s_singleton = false;

  AstFromString::c_sgnode = directive_node;
  AstFromString::c_char = directive_str.c_str();
}

Parser::~Parser() {
  assert(s_singleton == false);
  s_singleton = true;
}


void Parser::skip_whitespace() const {
  AstFromString::afs_skip_whitespace();
}

bool Parser::consume(const char c) const {
  if (c == '\0') return true;
  if (AstFromString::c_char[0] == c) {
    AstFromString::c_char++;
    return true;
  }
  return false;
}

bool Parser::consume(const std::string & label) const {
  std::string str(AstFromString::c_char);
  if (str.find(label) == 0) {
    AstFromString::c_char += label.length();
    return true;
  }
  else return false;
}

std::string Parser::getDirectiveString() const {
  return std::string(AstFromString::c_char);
}

template <>
bool Parser::parse<size_t>(size_t & val) const {
  skip_whitespace();
  int val_;
  if (!AstFromString::afs_match_integer_const(&val_)) 
    return false;
  skip_whitespace();
  val = val_;

  return true;
}

template <>
bool Parser::parse<SgExpression *>(SgExpression * & expr) const {
  skip_whitespace();
  if (!AstFromString::afs_match_assignment_expression()) // AstFromString::afs_match_expression would match expression list too
     return false;
  skip_whitespace();

  expr = isSgExpression(AstFromString::c_parsed_node);
  return expr != NULL;
}

template <>
bool Parser::parse<SgValueExp *>(SgValueExp * & expr) const {
  skip_whitespace();
  if (!AstFromString::afs_match_constant())
     return false;
  skip_whitespace();

  expr = isSgValueExp(AstFromString::c_parsed_node);
  return expr != NULL;
}

template <>
bool Parser::parse<std::string>(std::string & str) const {
  if (!consume('"')) return false;

  const char * old_c_char = AstFromString::c_char;

  while (AstFromString::c_char[0] != '\0' && !consume('"')) {
    str += AstFromString::c_char[0];
    AstFromString::c_char++;
  }

  if (AstFromString::c_char[0] == '\0') {
    AstFromString::c_char = old_c_char;
    return false;
  }

  return true;
}

template <>
bool Parser::parse<boost::filesystem::path>(boost::filesystem::path & file) const {

  if (!consume('"')) return false;

  const char * old_c_char = AstFromString::c_char;
  while (AstFromString::c_char[0] != '\0' && !consume('"')) {
    AstFromString::c_char++;
  }

  if (AstFromString::c_char[0] == '\0') {
    AstFromString::c_char = old_c_char;
    return false;
  }

  file = boost::filesystem::path(old_c_char, AstFromString::c_char);

  return true;
}

template <>
bool Parser::parse<std::pair<SgExpression *, SgExpression *> >(std::pair<SgExpression *, SgExpression *> & pair) const {
  skip_whitespace();
  if (!parse<SgExpression *>(pair.first)) return false;
  skip_whitespace();
  if (!consume(',')) return false;
  skip_whitespace();
  if (!parse<SgExpression *>(pair.second)) return false;
  return true;
}
 
template <>
bool Parser::parse<SgVariableSymbol *>(SgVariableSymbol * & var_sym) const {
  skip_whitespace();
  if (!AstFromString::afs_match_identifier())
    return false;
  skip_whitespace();
 
  SgVarRefExp * var_ref = isSgVarRefExp(AstFromString::c_parsed_node);
  if (var_ref == NULL)
    return false;
  var_sym = var_ref->get_symbol();

  return var_sym != NULL;
}

template <>
bool Parser::parse<int>(int & val) const {
  skip_whitespace();
  if (!AstFromString::afs_match_integer_const(&val))
    return false;
  skip_whitespace();
 
  return true;
}

template <>
bool Parser::parse<data_sections_t>(data_sections_t & data_sections) const {
  return parse_pair<SgVariableSymbol *, section_list_t>(data_sections, '\0', '\0', '\0');
}
 
template <>
bool Parser::parse<section_list_t>(section_list_t & section_list) const {
  return parse_list<section_t>(section_list, '\0', '\0', '\0');
}
 
template <>
bool Parser::parse<section_t>(section_t & section) const {
  SgExpression * expr_1 = NULL;
  SgExpression * expr_2 = NULL;
  SgExpression * expr_3 = NULL;

  if (!consume('[')) return false;

  if (!parse<SgExpression *>(expr_1)) return false;

  if (!consume(':')) {
    if (!consume(']')) return false;

    section.lower_bound = NULL;
    section.size = expr_1;
    section.stride = NULL;

    return true;
  }

  if (!parse<SgExpression *>(expr_2)) return false;

  if (!consume(':')) {
    if (!consume(']')) return false;

    section.lower_bound = expr_1;
    section.size = expr_2;
    section.stride = NULL;

    return true;
  }

  if (!parse<SgExpression *>(expr_3)) return false;

  if (!consume(']')) return false;

  section.lower_bound = expr_1;
  section.size = expr_2;
  section.stride = expr_3;

  return true;
}

/** @} */

}

}

