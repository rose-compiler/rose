
#ifndef __POLYHEDRAL_ARRAY_ANALYSIS_HPP__
#define __POLYHEDRAL_ARRAY_ANALYSIS_HPP__

#include "common/array-analysis.hpp"

class PolyDriver;

class PolyArrayAnalysis : public ArrayAnalysis {
  protected:
    PolyDriver * polydriver;
    std::map<RoseVariable, ArrayPartition *> whole_array_partition;

  protected:
    void collectAccess(SPMD_Tree * tree);

  public:
    PolyArrayAnalysis(PolyDriver * polydriver_);
    virtual ~PolyArrayAnalysis();

    virtual void process(SPMD_Root * tree);

    virtual std::set<CommDescriptor *> genComm(SPMD_Tree * t1, SPMD_Tree * t2, NodePlacement & placement);
    virtual std::set<SyncDescriptor *> genSync(SPMD_Tree * t1, SPMD_Tree * t2, NodePlacement & placement);
};

#endif /* __POLYHEDRAL_ARRAY_ANALYSIS_HPP__ */ 

