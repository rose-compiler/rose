
#include "common/placement.hpp"

#include "common/spmd-tree.hpp"
#include "common/compute-system.hpp"

NodePlacement::NodePlacement(System * system_) :
  system(system_)
{}

NodePlacement::~NodePlacement() {
  delete system;
}

void NodePlacement::clear() {}

