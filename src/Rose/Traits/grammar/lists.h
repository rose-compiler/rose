#ifndef Rose_Traits_grammar_lists_h
#define Rose_Traits_grammar_lists_h

#include "Rose/Traits/grammar/traversal.h"

namespace Rose {
namespace Traits {

//! Traverse all subclasses and returns concrete one (includes root)
template <typename Root> using list_concrete_subclasses = podf_selector_t<grammar_traversal_details::concrete_node_t, Root>;

//! Traverse all subclasses and returns them (includes root)
template <typename Root> using list_all_subclasses = podf_selector_t<grammar_traversal_details::any_t, Root>;

namespace list_base_classes_details {
  template <typename Node>
  struct list_base_classes_t {
    using head = typename list_base_classes_t<typename generated::describe_node_t<Node>::base>::list;
    using list = typename head::template append<Node>;
  };

  template <>
  struct list_base_classes_t<void> {
    using list = mp::List<>;
  };
}

template <typename Node>
using list_base_classes = typename list_base_classes_details::list_base_classes_t<Node>::list;

} }

#endif /* Rose_Traits_grammar_lists_h */
