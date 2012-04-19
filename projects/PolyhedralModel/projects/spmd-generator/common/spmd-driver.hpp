
#ifndef __SPMD_DRIVER_HPP__
#define __SPMD_DRIVER_HPP__

#include <set>

class SgStatement;

class SPMD_Root;

class ArrayPartition;
class ArrayAnalysis;
class NodePlacement;

class SPMD_Driver {
  protected:
    NodePlacement & placement;
    ArrayAnalysis & array_analysis;

  public:
    SPMD_Driver(NodePlacement & placement_, ArrayAnalysis & array_analysis_);
    virtual ~SPMD_Driver();

    SPMD_Root * generateTree(
      SgStatement * first,
      SgStatement * last,
      std::set<ArrayPartition *> & init_comm,
      std::set<ArrayPartition *> & final_comm
    );

    virtual SPMD_Root * parse(SgStatement * first, SgStatement * last) = 0;
    virtual SPMD_Root * transform(SPMD_Root * tree) = 0;
};

#endif /* __SPMD_DRIVER_HPP__ */

