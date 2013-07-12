
#include "KLT/Core/loop-selector.hpp"

#include "KLT/Core/generator.hpp"
#include "KLT/Core/kernel.hpp"
#include "KLT/Core/loop-trees.hpp"

#include <cassert>

namespace KLT {

namespace Core {

void Dummy_LoopSelector::createKernels(
  const LoopTrees & loop_trees,
  std::list<Kernel *> & kernels,
  std::map<Kernel *, LoopTrees::node_t *> & kernels_map,
  Generator * generator
) const {
  const std::list< LoopTrees::node_t * > & trees = loop_trees.getTrees();

  assert(trees.size() > 0);

  // assert(trees.size() == 1); // FIXME Minimal handling

  std::list< LoopTrees::node_t * >::const_iterator it_tree;
  for (it_tree = trees.begin(); it_tree != trees.end(); it_tree++) {
    LoopTrees::node_t * tree = *it_tree;
    Kernel * kernel = generator->makeKernel();
    assert(kernel != NULL);

    kernels.push_back(kernel);
    kernels_map.insert(std::pair<Kernel *, LoopTrees::node_t *>(kernel, tree));
  }
}

void Dummy_LoopSelector::extractNestedLoops(const std::map<Kernel *, LoopTrees::node_t *> & kernels_map) const {
  std::map<Kernel *, LoopTrees::node_t *>::const_iterator it_kernel;
  for (it_kernel = kernels_map.begin() ; it_kernel != kernels_map.end(); it_kernel++) {
    Kernel * kernel = it_kernel->first;

    std::list<LoopTrees::loop_t *> perfectly_nested_loops;

    LoopTrees::node_t * node = it_kernel->second;
    LoopTrees::loop_t * loop = dynamic_cast<LoopTrees::loop_t *>(node);
    assert(node != NULL);

    while (loop != NULL && loop->parallel_pattern == LoopTrees::loop_t::parfor) {
      perfectly_nested_loops.push_back(loop); // cond: loop != NULL && loop->parallel_pattern == LoopTrees::parfor
      if (loop->children.size() == 1) {
        node = loop->children.front();               // cond: 
        loop = dynamic_cast<LoopTrees::loop_t *>(node);
      }
      else break;
    }

    std::list<LoopTrees::node_t *> body_branches;

    if (loop == NULL)
      body_branches.push_back(node);
    else if (loop->parallel_pattern != LoopTrees::loop_t::parfor)
      body_branches.push_back(loop);
    else {
      assert(loop != NULL && loop->parallel_pattern == LoopTrees::loop_t::parfor);
      body_branches.insert(body_branches.end(), loop->children.begin(), loop->children.end());
    }

    assert(!body_branches.empty());

    kernel->setContent(perfectly_nested_loops, body_branches);
  }
}

}

}

