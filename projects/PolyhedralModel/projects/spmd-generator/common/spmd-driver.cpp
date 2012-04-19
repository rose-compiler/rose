
#include "common/spmd-driver.hpp"

#include "common/array-analysis.hpp"
#include "common/placement.hpp"


SPMD_Driver::SPMD_Driver(NodePlacement & placement_, ArrayAnalysis & array_analysis_) :
  placement(placement_),
  array_analysis(array_analysis_)
{}

SPMD_Driver::~SPMD_Driver() {}

SPMD_Root * SPMD_Driver::generateTree(
  SgStatement * first,
  SgStatement * last,
  std::set<ArrayPartition *> & init_comm,
  std::set<ArrayPartition *> & final_comm
) {
  SPMD_Root * original_tree = parse(first, last);

  array_analysis.process(original_tree);
  placement.place(original_tree, array_analysis);

  SPMD_Root * res = transform(original_tree);

  placement.clear();
  array_analysis.clear();

  return res;
}

