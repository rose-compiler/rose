
#ifndef __ARRAY_ANALYSIS_HPP__
#define __ARRAY_ANALYSIS_HPP__

#include "rose/Variable.hpp"

#include <utility>
#include <vector>
#include <map>

class SPMD_Tree;
class SPMD_Root;

class CommDescriptor;
class SyncDescriptor;
class NodePlacement;

class SgType;

// Base type for Array analysis
// it doesn't consider actual partition
// it does however consider the aliases for each "ComputeSystem"
class ArrayPartition {
  protected:
    RoseVariable original_array;
    std::vector<unsigned> dimensions;
    SgType * type;

  public:
    ArrayPartition(RoseVariable & array_);
    ArrayPartition(RoseVariable & array_, std::vector<unsigned> & dimensions_, SgType * type_);
    virtual ~ArrayPartition();

  static ArrayPartition * merge(ArrayPartition * p1, ArrayPartition * p2);
};

class ArrayAnalysis {
  protected:
    std::map<SPMD_Tree *, std::pair<std::vector<ArrayPartition *>, std::vector<ArrayPartition *> > > accesses_map;

  public:
    ArrayAnalysis();
    virtual ~ArrayAnalysis();

    virtual void process(SPMD_Root * tree) = 0;

    virtual std::vector<CommDescriptor *> genComm(SPMD_Tree * t1, SPMD_Tree * t2, NodePlacement & placement) = 0;
    virtual std::vector<SyncDescriptor *> genSync(SPMD_Tree * t1, SPMD_Tree * t2, NodePlacement & placement) = 0;

    const std::pair<std::vector<ArrayPartition *>, std::vector<ArrayPartition *> > & get(SPMD_Tree * tree) const;

    virtual void clear();
};

#endif /* __ARRAY_ANALYSIS_HPP__ */

