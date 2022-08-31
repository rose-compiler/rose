
#include "Rose/Traits/Describe.h"
#include "Rose/Traits/grammar/lists.h"

#define DO_NODES 0
#if DO_NODES == 0
using node_list = Rose::Traits::list_all_subclasses<SgNode>;
#elif DO_NODES == 1
using node_list = Rose::Traits::list_all_subclasses<SgNamedType>;
#else
using node_list = Rose::mp::List<>;
#endif

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
  node_list::apply([]<typename NodeT>(std::ostream & out) {
      using desc_t = Rose::Traits::DescribeNode<NodeT>;
      out << desc_t::name << " : " << desc_t::variant << std::endl;
      desc_t::fields_t::apply([]<typename FldT>(std::ostream & out) {
          out << "  " << FldT::name << " : " << FldT::typestr << " : " << FldT::traverse << " : " << FldT::edge << " : " << FldT::iterable << std::endl;
      }, out);
  }, std::cout);
  std::cout << std::endl;
  return 0;
}

