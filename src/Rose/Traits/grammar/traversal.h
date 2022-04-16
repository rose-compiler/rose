#ifndef Rose_Traits_grammar_traversal_h
#define Rose_Traits_grammar_traversal_h

#include "Rose/Traits/describe/declarations.h"

namespace Rose {
namespace Traits {

namespace grammar_traversal_details {
  //! In general we extract the node tag for any matching node
  template <typename T> struct identity_t { using type = T; };

  //! predicate to match any grammar nodes
  template <typename Node> struct any_t { static constexpr bool value = true; };

  //! predicate to match concrete grammar nodes
  template <typename Node> struct concrete_node_t { static constexpr bool value = generated::describe_node_t<Node>::concrete; };

  //! How to traverse the grammar hierarchy
  template <typename Node> struct children_t { using list = typename generated::describe_node_t<Node>::subclasses_t; };

  /**
   * \brief Factory for traversals of the grammar
   *
   * \tparam Predicate
   * \tparam Extractor
   */
  template <template<typename> class Predicate, template<typename> class Extractor=identity_t>
  using factory_t = typename mp::tree_traversal_factory_t<Predicate, Extractor, children_t>;

}

/**
 * Pre-order breath-first traversal of Rose Grammar returning a list of Node classes matching the predicate
 *
 * \tparam Root an AST class SgXXX
 * \tparam Predicate template that takes one AST class as argument and define the boolean "value"
 */
template <template<typename> class Predicate, typename Root>
using podf_selector_t = typename grammar_traversal_details::factory_t<Predicate, grammar_traversal_details::identity_t>::template preorder_depthfirst_t<Root>::list;

} }

#endif /* Rose_Traits_grammar_traversal_h */
