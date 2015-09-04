/*!
 * \file DLX/Core/directives.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __DLX_DIRECTIVES_HPP__
#define __DLX_DIRECTIVES_HPP__

#include <utility>
#include <vector>
#include <string>

namespace DLX {

namespace Directives {

template <class language_tpl> struct generic_construct_t;
template <class language_tpl> struct generic_clause_t;

template <class language_tpl>
struct directive_t {
  typedef language_tpl language_t;

  typedef generic_construct_t<language_t> construct_t;
  typedef generic_clause_t<language_t> clause_t;

  typedef typename language_t::directives_relation_e relation_e;

  typedef std::pair<relation_e, directive_t<language_t> *> predecessor_t;
  typedef std::pair<relation_e, directive_t<language_t> *> successor_t;

  typedef std::vector<clause_t *> clause_list_t;

  typedef std::vector<predecessor_t> predecessor_list_t;
  typedef std::vector<successor_t> successor_list_t;

  construct_t * construct;
  clause_list_t clause_list;

  predecessor_list_t predecessor_list;
  successor_list_t successor_list;

  void add_predecessor(relation_e rel, directive_t<language_t> * directive) {
    predecessor_list.push_back(std::pair<relation_e, directive_t<language_t> *>(rel, directive));
  }

  void add_successor(relation_e rel, directive_t<language_t> * directive) {
    successor_list.push_back(std::pair<relation_e, directive_t<language_t> *>(rel, directive));
  }
};

}

}

#endif /* __DLX_DIRECTIVES_HPP__ */

