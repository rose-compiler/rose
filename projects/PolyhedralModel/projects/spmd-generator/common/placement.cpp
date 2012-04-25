
#include "common/placement.hpp"

#include "common/spmd-tree.hpp"
#include "common/compute-system.hpp"

NodePlacement::NodePlacement(ComputeSystem * system_) :
  system(system_)
{}

NodePlacement::~NodePlacement() {
  if (system != NULL)
    delete system;
}

void NodePlacement::clear() {}

