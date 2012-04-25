
#include "polydriver/polyhedral-array-analysis.hpp"

PolyArrayAnalysis::PolyArrayAnalysis() :
  ArrayAnalysis()
{}

PolyArrayAnalysis::~PolyArrayAnalysis() {}

void PolyArrayAnalysis::process(SPMD_Root * tree) {
  // TODO recursively fill std::map<SPMD_Tree *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > > 'accesses_map'
}

std::set<CommDescriptor *> PolyArrayAnalysis::genComm(SPMD_Tree * t1, SPMD_Tree * t2, NodePlacement & placement) {
  // TODO
}

std::set<SyncDescriptor *> PolyArrayAnalysis::genSync(SPMD_Tree * t1, SPMD_Tree * t2, NodePlacement & placement) {
  // TODO
}

