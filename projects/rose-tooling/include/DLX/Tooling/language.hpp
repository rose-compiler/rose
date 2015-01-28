
#ifndef __DLX_TOOLING_LANGUAGE_HPP__
#define __DLX_TOOLING_LANGUAGE_HPP__

#include "DLX/Core/constructs.hpp"
#include "DLX/Core/clauses.hpp"
#include "DLX/Core/frontend.hpp"
#include "DLX/Core/parser.hpp"
#include "DLX/Core/compiler.hpp"

#include <boost/filesystem.hpp>

#include <vector>
#include <map>
#include <string>

class SgLocatedNode;
class SgStatement;
class SgScopeStatement;
class SgExpression;
class SgValueExp;

namespace DLX {

namespace Tooling {

/// This structure defines the language of the Tooling's pragma
struct language_t {
  /// Name of the laguage
  static std::string language_label;

  /// Enumeration of the construct of the language.
  enum construct_kinds_e {
    e_tooling_construct_set,       /// Propagate a const value to replace a scalar variable. Applied in the scope of the target variable.
    e_tooling_construct_init,      /// Takes a file (CSV, SQLITE) and generates initializer. 
    e_tooling_construct_callgraph, /// Marks functions we want to appear in the call graph. Edges between functions with the same tag are emphasized.
    e_tooling_construct_outline,   /// Outline the following statement in its own function. If 'file' is provided the outlined function is *appended* to the file.
    e_tooling_construct_replay,    /// Replay first outline the following statement. Then it provides runtime support to save the outlined function inputs and outputs. It enables to optimize a kernel in isolation.
    e_tooling_construct_grapher,   /// Generates GraphViz representation of a sub-AST
    e_construct_last
  };

  typedef std::map<construct_kinds_e, std::string> construct_label_map_t;
  /// Associates a label to each construct kind
  static construct_label_map_t s_construct_labels;
  /// To handle blank construct
  static construct_kinds_e s_blank_construct;

  /// Enumeration of the clauses of the language.
  enum clause_kinds_e {
    e_tooling_clause_var,    /// For 'set'
    e_tooling_clause_value,  /// For 'set'
    e_tooling_clause_file,   /// to provide a filename. Used by: 'init', 'callgraph', 'outline', 'grapher'
    e_tooling_clause_format, /// input format for 'init'
    e_tooling_clause_tag,    /// associate numeric tags to a directive (one per clause). Used by: 'callgraph'
    e_tooling_clause_pack,   /// for 'outliner' and 'replay': outline normally place one variable per argument of outlined function, 'pack' takes a list of symbols and pack them in one structure.
    e_clause_last
  };
  typedef std::vector<std::string> label_set_t;
  typedef std::map<clause_kinds_e, label_set_t> clause_labels_map_t;
  /// Associates a set of labels to each clause kind
  static clause_labels_map_t s_clause_labels;

  /// Kinds of relations that can exist between directives
  enum directives_relation_e {
    e_depends_on,
    e_relation_last
  };
  typedef std::map<directives_relation_e, std::string> directives_relation_label_map_t;
  /// Used to output the directive graph (GraphViz format)
  static directives_relation_label_map_t s_directives_relation_labels;

  /// Specific to ROSE's tooling
  typedef std::pair<construct_kinds_e, construct_kinds_e> construct_kind_pair_t;
  typedef std::set<construct_kind_pair_t> constructs_dependencies_t;
  static constructs_dependencies_t s_constructs_dependencies;

  static void init();
};

}

namespace Directives {

template <>
template <>
struct generic_construct_t<Tooling::language_t>::assoc_nodes_t<Tooling::language_t::e_tooling_construct_set> {
  SgPragmaDeclaration * pragma_decl;
  SgScopeStatement * parent_scope;
};

template <>
template <>
struct generic_construct_t<Tooling::language_t>::assoc_nodes_t<Tooling::language_t::e_tooling_construct_init> {
  SgPragmaDeclaration * pragma_decl;
  SgScopeStatement * parent_scope;
  SgInitializedName * var_to_init;
};

template <>
template <>
struct generic_construct_t<Tooling::language_t>::assoc_nodes_t<Tooling::language_t::e_tooling_construct_callgraph> {
  SgPragmaDeclaration * pragma_decl;
  SgScopeStatement * parent_scope;
  SgFunctionDeclaration * function_decl;
};

template <>
template <>
struct generic_construct_t<Tooling::language_t>::assoc_nodes_t<Tooling::language_t::e_tooling_construct_outline> {
  SgPragmaDeclaration * pragma_decl;
  SgScopeStatement * parent_scope;
  SgStatement * stmt;
};

template <>
template <>
struct generic_construct_t<Tooling::language_t>::assoc_nodes_t<Tooling::language_t::e_tooling_construct_replay> {
  SgPragmaDeclaration * pragma_decl;
  SgScopeStatement * parent_scope;
  SgStatement * stmt;
};

template <>
template <>
struct generic_construct_t<Tooling::language_t>::assoc_nodes_t<Tooling::language_t::e_tooling_construct_grapher> {
  SgPragmaDeclaration * pragma_decl;
  SgScopeStatement * parent_scope;
  SgStatement * stmt;
};

template <>
template <>
struct generic_clause_t<Tooling::language_t>::parameters_t<Tooling::language_t::e_tooling_clause_var> {
  SgVariableSymbol * var_sym;
};

template <>
template <>
struct generic_clause_t<Tooling::language_t>::parameters_t<Tooling::language_t::e_tooling_clause_value> {
  SgExpression * value_exp;
};

template <>
template <>
struct generic_clause_t<Tooling::language_t>::parameters_t<Tooling::language_t::e_tooling_clause_file> {
  boost::filesystem::path file;
};

template <>
template <>
struct generic_clause_t<Tooling::language_t>::parameters_t<Tooling::language_t::e_tooling_clause_format> {
  enum datafile_format_t {
    e_csv,
    e_sqlite3,
    e_unknown
  } format;
};

template <>
template <>
struct generic_clause_t<Tooling::language_t>::parameters_t<Tooling::language_t::e_tooling_clause_tag> {
  int tag;
};

template <>
template <>
struct generic_clause_t<Tooling::language_t>::parameters_t<Tooling::language_t::e_tooling_clause_pack> {
  std::vector<SgVariableSymbol *> symbols;
};

/// Boiler plate function: cannot call "new construct_t<Tooling::language_t, kind>();" if kind is not known at compile time.
template <>
generic_construct_t<Tooling::language_t> * buildConstruct<Tooling::language_t>(Tooling::language_t::construct_kinds_e kind);

/// Boiler plate function: cannot call "new clause_t<Tooling::language_t, kind>();" if kind is not known at compile time.
template <>
generic_clause_t<Tooling::language_t> * buildClause<Tooling::language_t>(Tooling::language_t::clause_kinds_e kind);

/// Boiler plate function: Calls the version of "parseClauseParameters" that correspond to "clause->kind". 
template <>
bool parseClauseParameters<Tooling::language_t>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  generic_clause_t<Tooling::language_t> * clause
);

/// Boiler plate function: Calls the version of "findAssociatedNodes" that correspond to "clause->kind". 
template <>
bool findAssociatedNodes<Tooling::language_t>(
  SgLocatedNode * directive_node,
  generic_construct_t<Tooling::language_t> * construct,
  const std::map<SgLocatedNode *, directive_t<Tooling::language_t> *> & translation_map
);

}

}

#endif /* __DLX_TOOLING_LANGUAGE_HPP__ */

