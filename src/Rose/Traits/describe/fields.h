#ifndef Rose_Traits_describe_fields_h
#define Rose_Traits_describe_fields_h

#include "Rose/Traits/describe/predicates.h"

namespace Rose {
namespace Traits {

/**
 * Describes a field of an AST node
 *
 * \tparam NodeT the AST node class
 * \tparam FieldPtrT the type of the pointer-to-member
 * \tparam fld_ptr the the pointer-to-member
 *
 * \todo c++17 with auto non-type template parameter:
 *        -> remove the need for FieldPtrT
 *        -> remove the need for partial spec (c++17 has "get base-type of field-pointer-type")
 *        -> remove the need for describe_field_t::bind which is here because c++14 don't want to initialize
 *              a pointer-to-member template argument from a constexpr variable. (it requires `&X::mbr` form)
 *              It must be: `using type = TPL<&X::mbr>;` not `constexpr auto Xmbr = &X::mbr; using type = TPL<Xmbr>;`.
 * \note using decltype(nullptr) instead of nullptr_t because of ICC 19
 *
 * @{ */
template <typename NodeT, typename FieldPtrT=decltype(nullptr), FieldPtrT fld_ptr = nullptr> struct DescribeField;

template <typename NodeT, typename FieldTypeT, FieldTypeT NodeT::* fld_ptr>
struct DescribeField<NodeT, FieldTypeT NodeT::*, fld_ptr> : generated::describe_field_t<NodeT, FieldTypeT, fld_ptr>, inspect_field<FieldTypeT> {
  using details::inspect_field_t<FieldTypeT>::inspect_field_t;
};
/** @} */

/**
 * For documentation purpose.
 */
template <>
struct DescribeField<void, decltype(nullptr), nullptr> {
  using parent = SgNode;                               //!< the `SgXXX` class this field is part of
  using field_type = int;                              //!< the type of the field
  static constexpr size_t position{0};                 //!< fields position in the class
  static constexpr char const * const name{""};        //!< string representation of the field's name
  static constexpr char const * const typestr{"int"};  //!< string representation of the field's type
  static constexpr bool traverse{false};               //!< whether this edge is traversed
  static constexpr auto mbr_ptr{nullptr};              //!< pointer to the class-member

  static constexpr bool iterable{false};               //!< if the field is stl-like iterable
  using type = void;                                   //!< type of the field or, value_type of iterable
  static constexpr bool edge{false};                   //!< whether `type` is a pointer to SgXXX
  size_t const index{0};                               //!< used by traversal to provide the index in the container if iterable else it is equal to the field position. Given that Rose nodes either have multiple traversable fields or a single traversable container, index is always a valid child index w.r.t the traversal.
};

namespace details {
  template <typename FieldDescT>
  struct transcribe_field_desc_t {
    using type = typename FieldDescT::template bind<DescribeField>;
  };
}

template <typename FieldDescT>
using transcribe_field_desc = typename details::transcribe_field_desc_t<FieldDescT>::type;

} }

#endif /* Rose_Traits_describe_fields_h */
