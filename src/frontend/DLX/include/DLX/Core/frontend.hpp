/*!
 *
 * \file DLX/Core/frontend.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __DLX_FRONTEND_HPP__
#define __DLX_FRONTEND_HPP__

#include <vector>
#include <string>

#include "DLX/Core/directives.hpp"
#include "DLX/Core/clauses.hpp"
#include "DLX/Core/constructs.hpp"

#include "sage3basic.h"

namespace DLX {

namespace Frontend {

/*!
 * \addtogroup grp_dlx_core_frontend
 * @{
 */

template <class language_tpl>
class Frontend {
  public:
    typedef language_tpl language_t;

    typedef Directives::directive_t<language_t> directive_t;
    typedef std::vector<directive_t *> directives_ptr_set_t;

    typedef Directives::generic_construct_t<language_t> generic_construct_t;
    typedef Directives::generic_clause_t<language_t> generic_clause_t;

  public:
    directives_ptr_set_t directives;
    directives_ptr_set_t graph_entry;
    directives_ptr_set_t graph_final;

  protected:
    static generic_construct_t * parseConstruct(std::string & directive_str);
    static generic_clause_t    * parseClause(std::string & directive_str);
    static directive_t         * parse(std::string & directive_str, SgLocatedNode * directive_node);

/*!
 * \addtogroup grp_dlx_core_specific
 * @{
 */

    template <typename language_t::construct_kinds_e kind>
    static bool findAssociatedNodes(SgLocatedNode * directive_node, Directives::construct_t<language_t, kind> * construct);

    template <typename language_t::clause_kinds_e kind>
    static bool parseClauseParameters(std::string & directive_str, SgLocatedNode * directive_node, Directives::clause_t<language_t, kind> * clause);

    bool build_graph();

/*! @} */

  public:
    bool parseDirectives(SgNode *);

  friend bool Directives::findAssociatedNodes<language_t>(SgLocatedNode *, Directives::generic_construct_t<language_t> *); 
  friend bool Directives::parseClauseParameters<language_t>(std::string &, SgLocatedNode *, Directives::generic_clause_t<language_t> *); 
};

template <class language_tpl>
typename Frontend<language_tpl>::generic_construct_t * Frontend<language_tpl>::parseConstruct(std::string & directive_str) {
  assert(language_t::s_construct_labels.size() > 0);

  std::cout << "> parseConstruct in \"" << directive_str << "\"" << std::endl;

  typename language_t::construct_label_map_t::const_iterator it_construct;
  for (it_construct = language_t::s_construct_labels.begin(); it_construct != language_t::s_construct_labels.end(); it_construct++)
    if (consume_label(directive_str, it_construct->second))
      return Directives::buildConstruct<language_tpl>(it_construct->first); // new Directives::construct_t<language_t, construct_kind>();
  return Directives::buildConstruct<language_tpl>(language_tpl::s_blank_construct);
}

template <class language_tpl>
typename Frontend<language_tpl>::generic_clause_t * Frontend<language_tpl>::parseClause(std::string & directive_str) {
  assert(language_t::s_clause_labels.size() > 0);

  std::cout << "> parseClause    in \"" << directive_str << "\"" << std::endl;

  typename language_t::clause_labels_map_t::const_iterator it_clause;
  for (it_clause = language_t::s_clause_labels.begin(); it_clause != language_t::s_clause_labels.end(); it_clause++) {
    const typename language_t::label_set_t & labels = it_clause->second;
    typename language_t::label_set_t::const_iterator it_label;
    for (it_label = labels.begin(); it_label != labels.end(); it_label++)
      if (consume_label(directive_str, *it_label))
        return Directives::buildClause<language_tpl>(it_clause->first);
  }
 
  return NULL;
}

template <class language_tpl>
typename Frontend<language_tpl>::directive_t * Frontend<language_tpl>::parse(std::string & directive_str, SgLocatedNode * directive_node) {
  if (!consume_label(directive_str, language_t::language_label)) return NULL;

  directive_t * directive = new directive_t();

  std::cout << "(1)     directive_str = " << directive_str << std::endl;

  directive->construct = parseConstruct(directive_str);
  assert(directive->construct != NULL);

  std::cout << "(2)     directive_str = " << directive_str << std::endl;

  assert(Directives::findAssociatedNodes(directive_node, directive->construct));

  generic_clause_t * clause = NULL;
  while ((clause = parseClause(directive_str)) != NULL) {
    std::cout << "(3) (a) directive_str = " << directive_str << std::endl;
    assert(Directives::parseClauseParameters(directive_str, directive_node, clause));
    std::cout << "(3) (b) directive_str = " << directive_str << std::endl;
    directive->clause_list.push_back(clause);
  }

  std::cout << "(4)     directive_str = " << directive_str << std::endl;

  return directive;
}

template <class language_tpl>
bool Frontend<language_tpl>::parseDirectives(SgNode * node) {
  // FIXME C/C++ only
  std::vector<SgPragmaDeclaration *> pragma_decls = SageInterface::querySubTree<SgPragmaDeclaration>(node);
  std::vector<SgPragmaDeclaration *>::iterator it_pragma_decl;
  for (it_pragma_decl = pragma_decls.begin(); it_pragma_decl != pragma_decls.end(); it_pragma_decl++) {
    SgPragmaDeclaration * pragma_decl = *it_pragma_decl;
    assert(pragma_decl != NULL);
    std::string directive_string = pragma_decl->get_pragma()->get_pragma(); 
    directives.push_back(parse(directive_string, pragma_decl));
  }
 
  assert(build_graph());
 
  return true;
}

/** @} */

}

}

#endif /* __DLX_FRONTEND_HPP__ */
