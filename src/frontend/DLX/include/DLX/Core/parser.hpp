
#ifndef __DLX_FRONTEND_PARSER_HPP__
#define __DLX_FRONTEND_PARSER_HPP__

#include <vector>
#include <utility>
#include <string>

#include "DLX/Core/directives.hpp"
#include "DLX/Core/clauses.hpp"
#include "DLX/Core/constructs.hpp"

#include "sage3basic.h"

namespace DLX {

namespace Frontend {

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

  protected:
    void skip_whitespace() const;
    bool consume(const char) const;

  public:
    Parser(std::string & directive_str, SgLocatedNode * directive_node);
    ~Parser();

    std::string getDirectiveString() const;

    template <typename A>
    bool parse(A &) const;

    template <typename A, typename B>
    bool parse_pair(std::pair<A, B> & pair, char start, char stop, char sep) const;

    template <typename A>
    bool parse_list(std::vector<A> & vect, char start, char stop, char sep) const;
};

// Builtin clause's argument types parsing

template <>
bool Parser::parse<SgExpression *>(SgExpression * &) const;

template <>
bool Parser::parse<SgVariableSymbol *>(SgVariableSymbol * &) const;

template <>
bool Parser::parse<data_sections_t>(data_sections_t &) const;

template <>
bool Parser::parse<section_list_t>(section_list_t &) const;

template <>
bool Parser::parse<section_t>(section_t &) const;

// Template for pair parsing

template <typename A, typename B>
bool Parser::parse_pair(std::pair<A, B> & pair, char start, char stop, char sep) const {
  bool result = consume(start);
  if (!result) return false;
  result = parse<A>(pair.first);
  if (!result) return false;
  result = consume(sep);
  if (!result) return false;
  result = parse<B>(pair.second);
  if (!result) return false;
  result = consume(stop);
  return result;
}

// Template for list parsing

template <typename A>
bool Parser::parse_list(std::vector<A> & vect, char start, char stop, char sep) const {
  A a;

  skip_whitespace();

  if (!consume(start)) return false;

  skip_whitespace();
  do {
    if (!parse<A>(a))
      if (sep == '\0') break;
      else return false;
    vect.push_back(a);
    skip_whitespace();
  } while (consume(sep));

  if (!consume(stop)) return false;

  return true;
}

}

}

#endif /* __DLX_FRONTEND_PARSER_HPP__ */

