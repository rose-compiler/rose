#ifndef Rose_Traits_describe_predicates_h
#define Rose_Traits_describe_predicates_h

#include "Rose/Traits/describe/declarations.h"

namespace Rose {
namespace Traits {

//! True if the template type parameter is a Rose AST node.
template <typename T> constexpr bool isAstNode = !std::is_same<typename generated::describe_node_t<T>::node, void>::value;

//! True if the template type parameter is a pointer to a Rose AST node.
template <typename T> constexpr bool isAstNodePtr = std::is_pointer<T>::value && isAstNode<std::remove_pointer_t<T>>;

namespace details {
  template <typename T, bool=mp::is_iterable<T> >
  struct check_iterable_t {
    static constexpr bool value = false;
    using type = void;
  };

  template <typename T>
  struct check_iterable_t<T, true> {
    static constexpr bool value = true;
    using type = typename T::value_type;
  };

  template <typename T, bool = mp::is_iterable<T>>
  struct inspect_field_t;

  template <typename T>
  struct inspect_field_t<T, false> {
    static constexpr bool iterable = false;
    using type = T;
    static constexpr bool edge = isAstNodePtr<type>;
    size_t const index{0};
    inspect_field_t(size_t idx) : index(idx) {}
  };

  template <typename T>
  struct inspect_field_t<T, true> {
    static constexpr bool iterable = true;
    using type = typename T::value_type;
    static constexpr bool edge = isAstNodePtr<type>;
    size_t const index{0};
    inspect_field_t(size_t idx) : index(idx) {}
  };
}

template <typename T>
using inspect_field = details::inspect_field_t<T>;

} }

#endif /* Rose_Traits_describe_predicates_h */
