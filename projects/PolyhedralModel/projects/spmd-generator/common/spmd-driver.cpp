
#include "common/spmd-driver.hpp"

#include "common/array-analysis.hpp"
#include "common/placement.hpp"


SPMD_Driver::SPMD_Driver(NodePlacement * placement_, ArrayAnalysis * array_analysis_) :
  placement(placement_),
  array_analysis(array_analysis_)
{}

SPMD_Driver::~SPMD_Driver() {}

SPMD_Root * SPMD_Driver::generateTree(
  SgStatement * first,
  SgStatement * last,
  std::map<ComputeSystem *, std::set<ArrayPartition *> > & to_be_aliased
) {
  SPMD_Root * original_tree = parse(first, last);

  array_analysis->process(original_tree);
  placement->place(original_tree, *array_analysis, to_be_aliased);

  SPMD_Root * res = transform(original_tree);

  placement->clear();
  array_analysis->clear();

  return res;
}

const NodePlacement & SPMD_Driver::getPlacement() const { return *placement; }
const ArrayAnalysis & SPMD_Driver::getArrayAnalysis() const {return *array_analysis; }

bool SPMD_Driver::hasPlacement() const { return placement != NULL; }
bool SPMD_Driver::hasArrayAnalysis() const { return array_analysis != NULL; }

