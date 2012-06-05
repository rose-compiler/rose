
#include "common/compute-system.hpp"

unsigned ComputeSystem::id_cnt = 0;

ComputeSystem::ComputeSystem(ComputeSystem * parent_) :
  id(id_cnt++),
  parent(parent_)
{}

ComputeSystem::ComputeSystem(const ComputeSystem & arg) :
  id(id_cnt++),
  parent(NULL)
{}

ComputeSystem::~ComputeSystem() {}

unsigned ComputeSystem::getID() const { return id; }

ComputeSystem * ComputeSystem::getParent() const { return parent; }
void ComputeSystem::setParent(ComputeSystem * parent_) { parent = parent_; }

unsigned Link::id_cnt = 0;

Link::Link() :
  id(id_cnt++)
{}

Link::~Link() {}

unsigned Link::getID() const { return id; }

