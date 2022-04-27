
#include "Rose/Traits/grammar/lists.h"
#include "Rose/Traits/describe/fields.h"

#define DO_FIELDS 1
#define DO_EDGES 1
#define DO_TRFLD 1

#define DO_OUTPUT 1

using all = Rose::Traits::list_all_subclasses<SgNode>;

/**
 * Information can be extracted from the AST node hierarchy at compile-time.
 * We show how to use `Rose::mp::List::map_ravel_t` and `Rose::mp::List::filter`.
 *
 * `Rose::mp::List::map_ravel_t` maps a `functor` to the list.
 * This `functor` template must define a subtype `list`.
 * The "list of list of elements" produced is ravelled into a "list of elements".
 *
 * `Rose::mp::List::filter` returns a list of all the elements for which the given `predicate` is true.
 * This `predicate` template must define a boolean `value`.
 *
 */

// Counting the number of fields:
#if DO_FIELD
template <typename NodeT>
struct get_fields_list_t {
  // The functor `get_fields_list_t` returns the list of all fields of a node (*without* its parents' fields)
  // We use `Rose::Traits::generated::describe_node_t` instead of `Rose::Traits::DescribeNode`.
  // It is faster and we do not need the extra information that `Rose::Traits::DescribeNode` provides.
  using list = typename Rose::Traits::generated::describe_node_t<NodeT>::fields_t;
};

using fields = all::template map_ravel_t<get_fields_list_t>;
#endif

// Counting the number of Edges:
#if DO_EDGES
template <typename FieldT>
struct is_edge_t {
  // We "manually" transcribe the field descriptor.
  //     ("transcribe": computing field property that need all descriptor to be defined first)
  // If we used `Rose::Traits::DescribeNode` in `get_traversable_edges_list_t`, `FieldT` would already be transcribed.
  static constexpr bool value = Rose::Traits::transcribe_field_desc<FieldT>::edge;
};

template <typename NodeT>
struct get_edges_list_t {
  using all_fields = typename Rose::Traits::generated::describe_node_t<NodeT>::fields_t;
  // The functor `get_edges_list_t` return the filtered list of all fields
  using list = typename all_fields::template filter<is_edge_t>;
};

using edges = all::template map_ravel_t<get_edges_list_t>;
#endif

// Counting the number of Traversable edges:
#if DO_TRFLD
template <typename FieldT>
struct is_traversable_t {
  static constexpr bool value = FieldT::traverse;
};

template <typename NodeT>
struct get_traversable_edges_list_t {
  using all_fields = typename Rose::Traits::generated::describe_node_t<NodeT>::fields_t;
  using list = typename all_fields::template filter<is_traversable_t>;
};

using traversable_edges = all::template map_ravel_t<get_traversable_edges_list_t>;
#endif

int main(int argc, char * argv[]) {
#if DO_OUTPUT
#if DO_FIELD
  std::cout << "Number of fields:" << std::dec << fields::length << std::endl;
#endif
#if DO_EDGES
  std::cout << "Number of edges:" << std::dec << edges::length << std::endl;
#endif
#if DO_TRFLD
  std::cout << "Number of traversable edges:" << std::dec << traversable_edges::length << std::endl;
#endif
#endif
  return 0;
}

