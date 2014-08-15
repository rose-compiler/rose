/*!
 * 
 * \file DLX/Core/clauses.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __DLX_DIRECTIVES_CLAUSE_HPP__
#define __DLX_DIRECTIVES_CLAUSE_HPP__

#include <string>

class SgLocatedNode;

namespace DLX {

namespace Directives {

/*!
 * \addtogroup grp_dlx_core_directives
 * @{
*/

template <class language_tpl, int kind_tpl>
struct clause_t;

template <class language_tpl>
struct generic_clause_t {
  typedef language_tpl language_t;
  typedef typename language_t::clause_kinds_e clause_kinds_e;

  template <clause_kinds_e kind> struct parameters_t {};

  clause_kinds_e kind;

  generic_clause_t(clause_kinds_e kind_) : kind(kind_) {}

  /// return 'this' cast to the requested type of clause, NULL if kind != 'this->kind'
  template <clause_kinds_e kind>
  clause_t<language_t, kind> * is();

  /// return the parameter stored by the clause, assert kind == 'this->kind'
  template <clause_kinds_e kind>
  parameters_t<kind> & getParameters();
};


// Default template have to be reused if the clause can have parameters
template <class language_tpl, int kind_tpl>
struct clause_t : public generic_clause_t<language_tpl> {
  typedef language_tpl language_t;
  typedef typename language_t::clause_kinds_e clause_kinds_e;

  enum {kind = kind_tpl};

  clause_t() : generic_clause_t<language_t>((clause_kinds_e)kind) {}

  typename generic_clause_t<language_t>::template parameters_t<(clause_kinds_e)kind> parameters;
};

template <class language_tpl>
generic_clause_t<language_tpl> * buildClause(typename language_tpl::clause_kinds_e kind);

template <class language_tpl>
bool parseClauseParameters(std::string & directive_str, SgLocatedNode * directive_node, generic_clause_t<language_tpl> * clause);



template <class language_t>
void addClauseLabel(typename language_t::clause_kinds_e clause_kind, const std::string & label) {
  typename language_t::clause_labels_map_t::iterator it = language_t::s_clause_labels.find(clause_kind);
  if (it == language_t::s_clause_labels.end())
    it = language_t::s_clause_labels.insert(
           std::pair<typename language_t::clause_kinds_e, typename language_t::label_set_t>(clause_kind, typename language_t::label_set_t())
         ).first;

  it->second.push_back(label);
}

/** @} */

}

}

#endif /* __DLX_DIRECTIVES_CLAUSE_HPP__ */

