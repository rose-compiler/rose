
#ifndef __DEPTH_PLACEMENT_HPP__
#define __DEPTH_PLACEMENT_HPP__

#include "polydriver/polyhedral-driver.hpp"
#include "polydriver/polyhedral-placement.hpp"
#include "compute-systems/compute-node.hpp"
#include "compute-systems/gpu-system.hpp"
#include "common/spmd-tree.hpp"

#include <map>
#include <vector>

class DepthPlacement : public PolyPlacement {
  protected:
    unsigned out;
    unsigned used;

    std::vector<GPU *> gpus;
    std::vector<Core *> cores;

    std::map<SPMD_Tree *, ComputeSystem *> placement;

  protected:
    void generateKernels(
      SPMD_Tree * parent,
      std::vector<SPMD_Tree *> & branch,
      unsigned length,
      std::vector<SPMD_KernelCall *> & res,
      ArrayAnalysis & array_analysis
    );
    void preplace(SPMD_Tree * tree, unsigned depth);
    void makeKernel(
        SPMD_Loop * first_loop,
        ArrayAnalysis & array_analysis
    );

    virtual bool isParallel(SPMD_Loop * loop);

  public:
    DepthPlacement(ComputeNode * node, unsigned out_, unsigned used_);
    virtual ~DepthPlacement();

    virtual void place(
        SPMD_Root * root,
        ArrayAnalysis & array_analysis,
        std::map<ComputeSystem *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > > & to_be_aliased
    );
    virtual Domain * onSameComputeSystem(SPMD_Tree * t1, SPMD_Tree * t2) const;
    virtual ComputeSystem * assigned(SPMD_Tree * tree) const;
};

#endif /* __DEPTH_PLACEMENT_HPP__ */

