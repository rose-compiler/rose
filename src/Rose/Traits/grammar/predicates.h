#ifndef Rose_Traits_grammar_predicates_h
#define Rose_Traits_grammar_predicates_h

#include "Rose/Traits/grammar/traversal.h"

namespace Rose {
namespace Traits {

// typedef
//   is_subclass_of<SgXXX, SgYYY>
//   is_concrete_subclass_of<SgXXX, SgYYY>

// function
//   is_subclass_of<SgXXX>(V_SgYYY)
//   is_concrete_subclass_of<SgXXX>(V_SgYYY)
//   is_base_class_of<SgXXX>(V_SgYYY)

template <typename P, typename C>
struct is_subclass_of_t {
  template <typename P_>
  struct is_subsubclass_of_t : is_subclass_of_t<P_,C> {};

  using is_same  = std::is_same<P,C>;

  using subclasses = typename generated::describe_node_t<P>::subclasses_t;
  using is_child = typename subclasses::template map_t<is_subsubclass_of_t>::template reduce_t<mp::or_t>;

  using type = mp::or_t<is_same, is_child>;
  static constexpr bool value = type::value;
};

template <typename P, typename C>
constexpr bool is_subclass_of = is_subclass_of_t<P, C>::value;

template <typename P, typename C>
using enable_subclass_of = std::enable_if_t<is_subclass_of<P, C>>;

} }

#endif /* Rose_Traits_grammar_predicates_h */
