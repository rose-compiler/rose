
#include "KLT/Core/loop-selector.hpp"

#include "KLT/Core/generator.hpp"
#include "KLT/Core/kernel.hpp"

namespace KLT {

namespace Core {

void LoopSelector::createKernels(const LoopTrees & loop_trees, std::list<Kernel *> & kernels, std::map<Kernel *, LoopTrees::node_t *> & kernels_map, Generator * generator) const {
  // TODO
}

void LoopSelector::extractNestedLoops(const std::map<Kernel *, LoopTrees::node_t *> & kernels_map) const {
  // TODO
}

}

}

