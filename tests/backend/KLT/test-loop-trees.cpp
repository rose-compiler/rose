
#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/data.hpp"

#include <cassert>


int main(int argc, char ** argv) {
  assert(argc == 3);

  KLT::Core::LoopTrees loop_trees;

  std::list<SgVariableSymbol *> parameter_order;
  std::pair<std::list<KLT::Core::Data *>, std::list<KLT::Core::Data *> > inout_data_order;

  loop_trees.read(argv[1], parameter_order, inout_data_order);

  loop_trees.toText(argv[2]);

  return 0;
}

