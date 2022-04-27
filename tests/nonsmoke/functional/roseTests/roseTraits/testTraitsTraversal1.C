
#include "Rose/Traits/Traversal.h"

#define DO_OUTPUT 1

struct pre_functor_t {
  template <typename NodeT>
  using desc_node_t = Rose::Traits::generated::describe_node_t<NodeT>;

//template <typename NodeT, typename FieldT> static constexpr bool case_expression = Rose::Traits::is_subclass_of<SgExpression, NodeT>;
  template <typename NodeT, typename FieldT> static constexpr bool case_expression = std::is_same<SgIntVal, NodeT>::value;
  template <typename NodeT, typename FieldT> static constexpr bool otherwise = !( case_expression<NodeT, FieldT> );
  

  size_t & max_depth;
  size_t & depth;

  pre_functor_t(size_t & max_depth_, size_t & depth_) : max_depth(max_depth_), depth(depth_) {}

  void update() {
    if (depth > max_depth) max_depth = depth;
    depth += 1;
  }

  template <typename NodeT, typename FieldT, std::enable_if_t<case_expression<NodeT, FieldT>> * = nullptr>
  void operator() (NodeT * node, FieldT pfld) {
    using n_desc_t = desc_node_t<NodeT>;
    using p_desc_t = desc_node_t<typename FieldT::parent>;
    std::cout << "[" << std::dec << depth << "] " << n_desc_t::name << " = " << std::hex << node << " from " << p_desc_t::name << "[" << pfld.index << "]" << std::endl;
    update();
  }

  template <typename NodeT, typename FieldT, std::enable_if_t<otherwise<NodeT, FieldT>> * = nullptr>
  void operator() (NodeT * node, FieldT pfld) {
   update();
  }
};

int main(int argc, char * argv[]) {
  ROSE_INITIALIZE;
  std::vector<std::string> args(argv, argv+argc);

  // Build a project
  SgProject * project = args.size() == 1 ? new SgProject() : frontend(args);

  // A simple depth counting traversal
  size_t max_depth = 0;
  size_t depth = 0;

  //  pre-order functor
  pre_functor_t pre(max_depth, depth);

  // post-order functor
  auto post = [&]<typename NodeT, typename FieldT>(NodeT * node, FieldT pfld) {
    depth -= 1;
  };

  Rose::Traits::traverse(project, pre, post);
  std::cout << "max_depth = " << max_depth << std::endl;

  return 0;
}

