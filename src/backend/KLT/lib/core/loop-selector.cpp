
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
    LoopTrees::node_t * node = it_kernel->second;

    std::set<Kernel::loop_distribution_t *> loop_distributions;
    {
      Kernel::loop_distribution_t * loop_distribution = new Kernel::loop_distribution_t();
        loop_distributions.insert(loop_distribution);

      LoopTrees::loop_t * loop = dynamic_cast<LoopTrees::loop_t *>(node);
      assert(node != NULL);

      while (loop != NULL && loop->parallel_pattern == LoopTrees::loop_t::parfor) {
        loop_distribution->loop_nest.push_back(loop); // cond: loop != NULL && loop->parallel_pattern == LoopTrees::parfor
        if (loop->children.size() == 1) {
          node = loop->children.front();               // cond: 
          loop = dynamic_cast<LoopTrees::loop_t *>(node);
        }
        else break;
      }

      if (loop == NULL)
        loop_distribution->body.push_back(node);
      else if (loop->parallel_pattern != LoopTrees::loop_t::parfor)
        loop_distribution->body.push_back(loop);
      else {
        assert(loop != NULL && loop->parallel_pattern == LoopTrees::loop_t::parfor);
        loop_distribution->body.insert(loop_distribution->body.end(), loop->children.begin(), loop->children.end());
      }

      assert(!loop_distribution->body.empty());
    }

    kernel->setLoopDistributions(loop_distributions);
  }
}

}

}

