/*!
 * 
 * \file DLX/Core/constructs.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __DLX_DIRECTIVES_CONSTRUCT_HPP__
#define __DLX_DIRECTIVES_CONSTRUCT_HPP__

#include <string>
#include <map>

class SgLocatedNode;

namespace DLX {

namespace Directives {

/*!
 * \addtogroup grp_dlx_core_directives
 * @{
 */

template <class language_tpl>
struct directive_t;

template <class language_tpl, int kind_tpl>
struct construct_t;

template <class language_tpl>
struct generic_construct_t {
  typedef language_tpl language_t;
  typedef typename language_t::construct_kinds_e construct_kinds_e;

  template <int kind_tpl> struct assoc_nodes_t {};

  construct_kinds_e kind;

  generic_construct_t(construct_kinds_e kind_) : kind(kind_) {}

  /// return 'this' cast to the requested type of construct, NULL if kind != 'this->kind'
  template <construct_kinds_e kind>
  construct_t<language_t, kind> * is();

  /// return the nodes associated to the construct, assert kind == 'this->kind'
  template <construct_kinds_e kind>
  assoc_nodes_t<kind> & getAssociatedNodes();
};

template <class language_tpl, int kind_tpl>
struct construct_t : public generic_construct_t<language_tpl> {
  typedef language_tpl language_t;
  typedef typename language_t::construct_kinds_e construct_kinds_e;

  enum {kind = kind_tpl};

  construct_t() : generic_construct_t<language_t>((construct_kinds_e)kind) {}

  typename generic_construct_t<language_t>::template assoc_nodes_t<kind> assoc_nodes;
};



template <class language_tpl>
generic_construct_t<language_tpl> * buildConstruct(typename language_tpl::construct_kinds_e kind);

template <class language_tpl>
bool findAssociatedNodes(
  SgLocatedNode * directive_node,
  generic_construct_t<language_tpl> * construct,
  const std::map<SgLocatedNode *, Directives::directive_t<language_tpl> *> & translation_map
);



template <class language_t>
void setConstructLabel(typename language_t::construct_kinds_e construct_kind, const std::string & label) {
  typename language_t::construct_label_map_t::iterator it = language_t::s_construct_labels.find(construct_kind);
  if (it == language_t::s_construct_labels.end())
    language_t::s_construct_labels.insert(std::pair<typename language_t::construct_kinds_e, std::string>(construct_kind, label));
  else
    it->second = label;
}

/** @} */

}

}

#endif /* __DLX_DIRECTIVES_CONSTRUCT_HPP__ */

