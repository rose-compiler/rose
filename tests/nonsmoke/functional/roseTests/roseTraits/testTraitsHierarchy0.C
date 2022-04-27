
#include "Rose/Traits/Describe.h"

using concrete_named_types = Rose::Traits::list_concrete_subclasses<SgNamedType>;

int main(int argc, char * argv[]) {
  /*
   * Iterate over Rose AST classes and fields using nested lambdas
   *   - uses the `apply` static method of Rose::metaprog::List (Rose::mp::List)
   *
   * Canonical example:
   *   a_node_list::apply([]<typename NodeT>() {
   *     // for each Rose AST class
   *     Rose::Traits::DescribeNode<NodeT>::fields_t::apply([]<typename FldT>() {
   *       // for each field of that class
   *     });
   *   });
   *
   * Arguments can be forwarded by `apply` to the lambda (but lambda-capture is preferable when possible: most of the time).
   * Below, it forwards the standart-output to the lambdas which print the fields of all concrete types.
   */
  std::cout << "Concrete subclasses of SgNamedType:" << std::endl;
  concrete_named_types::apply([]<typename NodeT>(std::ostream & out) {
      // `DescribeNode` is built from descriptor generated when building Rose's AST (Rosetta).

      // `DescribeNode` provides:
      //  * generated properties: (`Rose::Traits::generated::describe_node_t`)
      //    * using node;                               // the node's `SgXXX`  
      //    * using base;                               // the parent `SgXXX` in the hierarchy
      //    * static constexpr char const * const name; // string representation of the class' name: "XXX"
      //    * static constexpr unsigned long variant;   // numerical variant of the node
      //    * static constexpr bool concrete;           // whether it is a concrete grammar node
      //    * using subclasses_t;                       // `Rose::mp::List` of `SgXXX` that descend from this node
      //  * computed properties:
      //    * using fields_t;                           // `Rose::mp::List` of `Rose::Traits::DescribeField` instances (from list of `Rose::Traits::generated::describe_field_t`)

      // `DescribeField` provides:
      //  * generated properties: (`Rose::Traits::generated::describe_field_t`)
      //    * using parent;                                // the `SgXXX` class this field is part of
      //    * using field_type;                            // the type of the field
      //    * static constexpr size_t position;            // fields position in the class
      //    * static constexpr char const * const name;    // string representation of the field's name
      //    * static constexpr char const * const typestr; // string representation of the field's type
      //    * static constexpr bool traverse;              // whether this edge is traversed
      //    * static constexpr auto mbr_ptr;               // pointer to the class-member
      //  * computed properties:
      //    * static constexpr bool iterable;              // if the field is stl-like iterable
      //    * using type;                                  // type of the field or, value_type of iterable
      //    * static constexpr bool edge;                  // whether `type` is a pointer to SgXXX
      //    * static constexpr unsigned long variant;      // variant of the target if it is an edge
      //    * size_t const index;                          // only if iterable, used by traversal to provide the index in the iterable
      using desc_t = Rose::Traits::DescribeNode<NodeT>;
      out << desc_t::name << " : " << desc_t::variant << std::endl;
      desc_t::fields_t::apply([]<typename FldT>(std::ostream & out) {
          out << "  " << FldT::name << " : " << FldT::typestr << " : " << FldT::traverse << " : " << FldT::edge << " : " << FldT::iterable << std::endl;
      }, out);
  }, std::cout);
  std::cout << std::endl;
}

