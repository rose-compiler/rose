
#include "common/compute-system.hpp"

unsigned ComputeSystem::id_cnt = 0;

ComputeSystem::ComputeSystem() :
  id(id_cnt++)
{}

ComputeSystem::~ComputeSystem() {}

unsigned ComputeSystem::getID() const { return id; }

unsigned Link::id_cnt = 0;

Link::Link() :
  id(id_cnt++)
{}

Link::~Link() {}

unsigned Link::getID() const { return id; }

System::System() :
  ComputeSystem(),
  elements()
{}

System::~System() {
  std::vector<ComputeSystem *>::iterator it;
  for (it = elements.begin(); it != elements.end(); it++)
    delete *it;
}

