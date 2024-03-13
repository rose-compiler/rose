#ifndef Rose_Traits_describe_declarations_h
#define Rose_Traits_describe_declarations_h
#include <RoseFirst.h>

#include "Rose/metaprog.h"

namespace Rose {
namespace Traits {
namespace generated {

template <typename NodeT>
struct describe_node_t {
  using node = void;
  using base = void;

  static constexpr char const * const name{""};
  static constexpr unsigned long variant{0};
  static constexpr bool concrete{false};

  using subclasses = mp::List<>;
  using fields = mp::List<>;
};

template <typename NodeT, typename FieldT, FieldT NodeT::* fld_ptr>
struct describe_field_t {
  using parent = NodeT;
  using field_type = FieldT;
  static constexpr size_t position{0};

  static constexpr char const * const name{""};
  static constexpr char const * const typestr{"void"};
  static constexpr bool traverse{true};

  static constexpr auto mbr_ptr{fld_ptr};
};

template <unsigned long variantT>
struct node_from_variant_t {
  using type = void;
};

} } }

#include "Rose/Traits/generated.h"

#endif /* Rose_Traits_describe_declarations_h */
