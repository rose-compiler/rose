
#ifndef __DEPTH_PLACEMENT_HPP__
#define __DEPTH_PLACEMENT_HPP__

#include "common/placement.hpp"
#include "compute-systems/compute-node.hpp"

class DepthPlacement : public NodePlacement {
  public:
    DepthPlacement(ComputeNode * node) :
      NodePlacement(node)
    {}

    virtual void place(SPMD_Root * root, ArrayAnalysis & array_analysis, std::map<ComputeSystem *, std::set<ArrayPartition *> > & to_be_aliased) {
      to_be_aliased.insert(std::pair<ComputeSystem *, std::set<ArrayPartition *> >(NULL, std::set<ArrayPartition *>()));
    }
    virtual Domain * onSameComputeSystem(SPMD_Tree * t1, SPMD_Tree * t2) const { return NULL; }
    virtual std::vector<std::pair<ComputeSystem *, Domain *> > * assigned(SPMD_Tree * tree) const {
      std::vector<std::pair<ComputeSystem *, Domain *> > * res = new std::vector<std::pair<ComputeSystem *, Domain *> >();
      res->push_back(std::pair<ComputeSystem *, Domain *>(NULL, NULL));
      return res;
    }
};

#endif /* __DEPTH_PLACEMENT_HPP__ */

