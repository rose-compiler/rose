
#ifndef __POLYHEDRAL_ARRAY_ANALYSIS_HPP__
#define __POLYHEDRAL_ARRAY_ANALYSIS_HPP__

#include "common/array-analysis.hpp"

class PolyArrayAnalysis : public ArrayAnalysis {
  protected:
    

  public:
    PolyArrayAnalysis();
    virtual ~PolyArrayAnalysis();

    virtual void process(SPMD_Root * tree);

    virtual std::set<CommDescriptor *> genComm(SPMD_Tree * t1, SPMD_Tree * t2, NodePlacement & placement);
    virtual std::set<SyncDescriptor *> genSync(SPMD_Tree * t1, SPMD_Tree * t2, NodePlacement & placement);
};

#endif /* __POLYHEDRAL_ARRAY_ANALYSIS_HPP__ */ 

