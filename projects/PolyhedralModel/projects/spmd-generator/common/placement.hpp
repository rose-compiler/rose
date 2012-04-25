
#ifndef __PLACEMENT_HPP_
#define __PLACEMENT_HPP_

#include <map>
#include <set>
#include <vector>
#include <utility>

class SPMD_Tree;
class SPMD_Root;

class ComputeSystem;
class System;
class Domain;

class ArrayAnalysis;
class ArrayPartition;;

class NodePlacement {
  protected:
    ComputeSystem * system;

  public:
    NodePlacement(ComputeSystem * system_);
    virtual ~NodePlacement();

    virtual void place(SPMD_Root * root, ArrayAnalysis & array_analysis, std::map<ComputeSystem *, std::set<ArrayPartition *> > & to_be_aliased) = 0;
    virtual Domain * onSameComputeSystem(SPMD_Tree * t1, SPMD_Tree * t2) const = 0;
    virtual std::vector<std::pair<ComputeSystem *, Domain *> > * assigned(SPMD_Tree * tree) const = 0;

    virtual void clear();
};

#endif /* __PLACEMENT_HPP_ */

