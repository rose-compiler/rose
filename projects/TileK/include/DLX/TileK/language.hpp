/*!
 * 
 * \file DLX/TileK/language.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __DLX_TILEK_LANGUAGE_HPP__
#define __DLX_TILEK_LANGUAGE_HPP__

#include "DLX/Core/constructs.hpp"
#include "DLX/Core/clauses.hpp"
#include "DLX/Core/frontend.hpp"
#include "DLX/Core/parser.hpp"
#include "DLX/Core/compiler.hpp"

#include <vector>
#include <map>
#include <string>

class SgScopeStatement;
class SgForStatement;
class SgExpression;

namespace DLX {

namespace TileK {

struct language_t {
  static std::string language_label;

  typedef std::vector<std::string> label_set_t;

  enum construct_kinds_e {
    e_construct_kernel,
    e_construct_loop,
    e_construct_last
  };
  typedef std::map<construct_kinds_e, std::string> construct_label_map_t;
  static construct_label_map_t s_construct_labels;
  static construct_kinds_e s_blank_construct;

  enum clause_kinds_e {
    e_clause_data,
    e_clause_tile,
    e_clause_last
  };
  typedef std::map<clause_kinds_e, label_set_t> clause_labels_map_t;
  static clause_labels_map_t s_clause_labels;

  enum directives_relation_e {
    e_child_scope,
    e_parent_scope,
    e_relation_last
  };
  typedef std::map<directives_relation_e, std::string> directives_relation_label_map_t;
  static directives_relation_label_map_t s_directives_relation_labels;

  static void init();
};

}

namespace Directives {

template <>
generic_construct_t<TileK::language_t> * buildConstruct<TileK::language_t>(TileK::language_t::construct_kinds_e kind);

template <>
generic_clause_t<TileK::language_t> * buildClause<TileK::language_t>(TileK::language_t::clause_kinds_e kind);
 
template <>
bool parseClauseParameters<TileK::language_t>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  generic_clause_t<TileK::language_t> * clause
);

//////

template <>
template <>
struct generic_construct_t<TileK::language_t>::assoc_nodes_t<TileK::language_t::e_construct_kernel> {
  SgScopeStatement * parent_scope;
  SgStatement * kernel_region;
};

template <>
template <>
struct generic_construct_t<TileK::language_t>::assoc_nodes_t<TileK::language_t::e_construct_loop> {
  SgScopeStatement * parent_scope;
  SgForStatement   * for_loop;
};

//////

template <>
template <>
struct generic_clause_t<TileK::language_t>::parameters_t<TileK::language_t::e_clause_data> {
  std::vector<Frontend::data_sections_t> data_sections;
};

template <>
template <>
struct generic_clause_t<TileK::language_t>::parameters_t<TileK::language_t::e_clause_tile> {
  size_t order;
  enum kind_e {
    e_static_tile,
    e_dynamic_tile,
  } kind;
  size_t nbr_it;
};


}

}

#endif /* __DLX_TILEK_LANGUAGE_HPP__ */

