
#include "Rose/Traits/Traversal.h"

int main(int argc, char * argv[]) {
  ROSE_INITIALIZE;
  std::vector<std::string> args(argv, argv+argc);

  // Build a project
  SgProject * project = args.size() == 1 ? new SgProject() : frontend(args);

  // A simple depth counting traversal
  size_t max_depth = 0;
  size_t depth = 0;

  //  pre-order functor
  auto pre = [&]<typename NodeT, typename FieldT>(NodeT * node, FieldT pfld) {
    if (depth > max_depth) max_depth = depth;
    depth += 1;
  };

  // post-order functor
  auto post = [&]<typename NodeT, typename FieldT>(NodeT * node, FieldT pfld) {
    depth -= 1;
  };

  Rose::Traits::traverse(project, pre, post);
  std::cout << "max_depth = " << max_depth << std::endl;

  return 0;
}

