
#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/data.hpp"

#include <cassert>


int main(int argc, char ** argv) {
  assert(argc == 3);

  KLT::Core::LoopTrees loop_trees;
  
  loop_trees.read(argv[1]);

  loop_trees.toText(argv[2]);

  return 0;
}

