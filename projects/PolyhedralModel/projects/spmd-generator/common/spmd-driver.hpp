
#ifndef __SPMD_DRIVER_HPP__
#define __SPMD_DRIVER_HPP__

#include <set>
#include <map>

class SgStatement;

class SPMD_Root;

class ArrayPartition;
class ArrayAnalysis;
class NodePlacement;

class ComputeSystem;

class SPMD_Driver {
  protected:
    NodePlacement * placement;
    ArrayAnalysis * array_analysis;

  public:
    SPMD_Driver(NodePlacement * placement_, ArrayAnalysis * array_analysis_);
    virtual ~SPMD_Driver();

    SPMD_Root * generateTree(
      SgStatement * first,
      SgStatement * last,
      std::map<ComputeSystem *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > > & to_be_aliased
    );

    const NodePlacement & getPlacement() const;
    const ArrayAnalysis & getArrayAnalysis() const;

    bool hasPlacement() const;
    bool hasArrayAnalysis() const;

    virtual SPMD_Root * parse(SgStatement * first, SgStatement * last) = 0;
};

#endif /* __SPMD_DRIVER_HPP__ */

