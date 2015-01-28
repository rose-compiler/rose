
#ifndef __DLX_LOGGER_LANGUAGE_HPP__
#define __DLX_LOGGER_LANGUAGE_HPP__

#include "DLX/Core/constructs.hpp"
#include "DLX/Core/clauses.hpp"
#include "DLX/Core/frontend.hpp"
#include "DLX/Core/parser.hpp"
#include "DLX/Core/compiler.hpp"

#include <vector>
#include <map>
#include <string>

class SgLocatedNode;
class SgStatement;
class SgScopeStatement;
class SgExpression;
class SgValueExp;

namespace DLX {

namespace Logger {

/// This structure defines the language of the Logger's pragma
struct language_t {
  /// Name of the laguage
  static std::string language_label;

  /// Enumeration of the construct of the language. Only one for Logger: "log"
  enum construct_kinds_e {
    e_logger_construct_log,
    e_construct_last
  };

  typedef std::map<construct_kinds_e, std::string> construct_label_map_t;
  /// Associates a label to each construct kind
  static construct_label_map_t s_construct_labels;
  /// To handle blank construct [not used for Logger]
  static construct_kinds_e s_blank_construct;

  /// Enumeration of the clauses of the language. Logger: "where", "message", "cond", "params"
  enum clause_kinds_e {
    e_logger_clause_where,
    e_logger_clause_message,
    e_logger_clause_conds,
    e_logger_clause_params,
    e_clause_last
  };
  typedef std::vector<std::string> label_set_t;
  typedef std::map<clause_kinds_e, label_set_t> clause_labels_map_t;
  /// Associates a set of labels to each clause kind
  static clause_labels_map_t s_clause_labels;

  /// Kinds of relations that can exist between directives
  enum directives_relation_e {
    e_relation_last
  };
  typedef std::map<directives_relation_e, std::string> directives_relation_label_map_t;
  /// Used to output the directive graph (GraphViz format)
  static directives_relation_label_map_t s_directives_relation_labels;

  static void init();
};

}

namespace Directives {

/// AST nodes associated with a directive using the "log" construct
template <>
template <>
struct generic_construct_t<Logger::language_t>::assoc_nodes_t<Logger::language_t::e_logger_construct_log> {
  SgPragmaDeclaration * pragma_decl;
  SgScopeStatement * parent_scope;
  SgStatement * logged_region;
};

/// The "where" clause can have one of "before", "after", "both" as argument. We represent it using an enum
template <>
template <>
struct generic_clause_t<Logger::language_t>::parameters_t<Logger::language_t::e_logger_clause_where> {
  enum log_position_e {
    e_before, e_after, e_both
  } position;
};

/// The "message" clause takes a string as arguments
template <>
template <>
struct generic_clause_t<Logger::language_t>::parameters_t<Logger::language_t::e_logger_clause_message> {
  std::string message;
};

/// The "cond" clause takes an expression and a constant value as arguments
template <>
template <>
struct generic_clause_t<Logger::language_t>::parameters_t<Logger::language_t::e_logger_clause_conds> {
  SgExpression * expr;
  SgValueExp * value;
};

/// The "params" clause takes a list of expression as arguments
template <>
template <>
struct generic_clause_t<Logger::language_t>::parameters_t<Logger::language_t::e_logger_clause_params> {
  std::vector<SgExpression *> params;
};

/// Boiler plate function: cannot call "new construct_t<Logger::language_t, kind>();" if kind is not known at compile time.
template <>
generic_construct_t<Logger::language_t> * buildConstruct<Logger::language_t>(Logger::language_t::construct_kinds_e kind);

/// Boiler plate function: cannot call "new clause_t<Logger::language_t, kind>();" if kind is not known at compile time.
template <>
generic_clause_t<Logger::language_t> * buildClause<Logger::language_t>(Logger::language_t::clause_kinds_e kind);

/// Boiler plate function: Calls the version of "parseClauseParameters" that correspond to "clause->kind". 
template <>
bool parseClauseParameters<Logger::language_t>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  generic_clause_t<Logger::language_t> * clause
);

/// Boiler plate function: Calls the version of "findAssociatedNodes" that correspond to "clause->kind". 
template <>
bool findAssociatedNodes<Logger::language_t>(
  SgLocatedNode * directive_node,
  generic_construct_t<Logger::language_t> * construct,
  const std::map<SgLocatedNode *, directive_t<Logger::language_t> *> & translation_map
);

}

}

#endif /* __DLX_LOGGER_LANGUAGE_HPP__ */

