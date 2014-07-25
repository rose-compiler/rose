/*!
 *
 * \file DLX/Core/parser.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __DLX_FRONTEND_PARSER_HPP__
#define __DLX_FRONTEND_PARSER_HPP__

#include "DLX/Core/directives.hpp"
#include "DLX/Core/clauses.hpp"
#include "DLX/Core/constructs.hpp"

#include "sage3basic.h"

#include <boost/filesystem.hpp>

#include <vector>
#include <utility>
#include <string>

namespace DLX {

namespace Frontend {

/*!
 * \addtogroup grp_dlx_core_frontend
 * @{
 */

// Basic string manip

void skip_whitespace(std::string & directive_str);
void skip_parenthesis(std::string & directive_str);

bool consume_label(std::string & directive_str, const std::string & label);

// Builtin types for clause's arguments

struct section_t {
  SgExpression * lower_bound;
  SgExpression * size;
  SgExpression * stride;
};

typedef std::vector<section_t> section_list_t;

typedef std::pair<SgVariableSymbol *, section_list_t> data_sections_t;

// Template for parsing of clause's arguments

class Parser {
  static bool s_singleton;

  public:
    Parser(std::string & directive_str, SgLocatedNode * directive_node);
    ~Parser();

    std::string getDirectiveString() const;

    template <typename A>
    bool parse(A &) const;

    template <typename A>
    bool parse_singleton(A & singleton, char start, char stop) const;

    template <typename A, typename B>
    bool parse_pair(std::pair<A, B> & pair, char start, char stop, char sep) const;

    template <typename A>
    bool parse_list(std::vector<A> & vect, char start, char stop, char sep) const;

    void skip_whitespace() const;
    bool consume(const char) const;
    bool consume(const std::string &) const;
};

// Builtin clause's argument types parsing

template <>
bool Parser::parse<size_t>(size_t &) const;

template <>
bool Parser::parse<SgExpression *>(SgExpression * & expr) const;

template <>
bool Parser::parse<SgValueExp *>(SgValueExp * & expr) const;

template <>
bool Parser::parse<std::string>(std::string & str) const;

template <>
bool Parser::parse<std::pair<SgExpression *, SgExpression *> >(std::pair<SgExpression *, SgExpression *> &) const;

template <>
bool Parser::parse<SgVariableSymbol *>(SgVariableSymbol * &) const;

template <>
bool Parser::parse<data_sections_t>(data_sections_t &) const;

template <>
bool Parser::parse<section_list_t>(section_list_t &) const;

template <>
bool Parser::parse<section_t>(section_t &) const;

template <>
bool Parser::parse<boost::filesystem::path>(boost::filesystem::path &) const;

// Template for singleton parsing

template <typename A>
bool Parser::parse_singleton(A & singleton, char start, char stop) const {
  if (!consume(start)) return false;
  if (!parse<A>(singleton)) return false;
  return consume(stop);
}

// Template for pair parsing

template <typename A, typename B>
bool Parser::parse_pair(std::pair<A, B> & pair, char start, char stop, char sep) const {
  if (!consume(start))        return false;
  if (!parse<A>(pair.first))  return false;
  if (!consume(sep))          return false;
  if (!parse<B>(pair.second)) return false;
  return consume(stop);
}

// Template for list parsing

template <typename A>
bool Parser::parse_list(std::vector<A> & vect, char start, char stop, char sep) const {
  skip_whitespace();

  if (!consume(start)) return false;

  skip_whitespace();
  do {
    A a;
    if (!parse<A>(a)) {
      if (sep == '\0') break;
      else return false;
    }
    vect.push_back(a);
    skip_whitespace();
  } while (consume(sep));

  if (!consume(stop)) return false;

  return true;
}

/** @} */

}

}

#endif /* __DLX_FRONTEND_PARSER_HPP__ */

