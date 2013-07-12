
#ifndef __KLT_CORE_LOOP_SELECTOR_HPP__
#define __KLT_CORE_LOOP_SELECTOR_HPP__

#include "KLT/Core/loop-trees.hpp"

#include <list>
#include <map>

namespace KLT {

namespace Core {

class Generator;
class Kernel;

class LoopSelector {
  public:
    virtual void createKernels(
      const LoopTrees & loop_trees,
      std::list<Kernel *> & kernels,
      std::map<Kernel *, LoopTrees::node_t *> & kernels_map,
      Generator * generator
    ) const = 0;

    virtual void extractNestedLoops(
      const std::map<Kernel *, LoopTrees::node_t *> & kernels_map
    ) const = 0;
};

class Dummy_LoopSelector : public LoopSelector {
  public:
    virtual void createKernels(
      const LoopTrees & loop_trees,
      std::list<Kernel *> & kernels,
      std::map<Kernel *, LoopTrees::node_t *> & kernels_map,
      Generator * generator
    ) const;

    virtual void extractNestedLoops(
      const std::map<Kernel *, LoopTrees::node_t *> & kernels_map
    ) const;
};

}

}

#endif /* __KLT_CORE_LOOP_SELECTOR_HPP__ */

