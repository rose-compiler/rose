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
  // Structure of the language

    // Kind of constructs
    enum construct_kinds_e {
      e_construct_kernel,
      e_construct_loop,
      e_construct_last
    };

    // Kind of clauses
    enum clause_kinds_e {
      e_clause_data,
      e_clause_tile,
      e_clause_last
    };

    // Kind of relation between directives
    enum directives_relation_e {
      e_child_scope,
      e_parent_scope,
      e_relation_last
    };

  // Static fields and initializer

    static std::string language_label;

    typedef std::map<construct_kinds_e, std::string> construct_label_map_t;
    static construct_label_map_t s_construct_labels;
    static construct_kinds_e s_blank_construct;

    typedef std::vector<std::string> label_set_t;
    typedef std::map<clause_kinds_e, label_set_t> clause_labels_map_t;
    static clause_labels_map_t s_clause_labels;

    typedef std::map<directives_relation_e, std::string> directives_relation_label_map_t;
    static directives_relation_label_map_t s_directives_relation_labels;

    static void init();

  // KLT Interface

    typedef Directives::directive_t<language_t> directive_t;
    typedef Directives::generic_construct_t<language_t> construct_t;
    typedef Directives::generic_clause_t<language_t> clause_t;

    // Kernel support

      typedef Directives::construct_t<language_t, e_construct_kernel> kernel_construct_t;
      static kernel_construct_t * isKernelConstruct(construct_t * construct);
      static SgStatement * getKernelRegion(kernel_construct_t * kernel_construct);

    // Loop support

      typedef Directives::construct_t<language_t, e_construct_loop> loop_construct_t;
      static loop_construct_t * isLoopConstruct(construct_t * construct);
      static SgForStatement * getLoopStatement(loop_construct_t * loop_construct);

    // Data support

      typedef Directives::clause_t<language_t, e_clause_data> data_clause_t;
      static data_clause_t * isDataClause(clause_t * clause);
      static const std::vector<DLX::Frontend::data_sections_t> & getDataSections(data_clause_t * data_clause);
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

