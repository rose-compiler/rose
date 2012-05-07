
#include "compute-systems/memory.hpp"

#include <cassert>

Memory::Memory() {}

Memory::Memory(const Memory & arg) {}

Memory::~Memory() {}

BasicMemory::BasicMemory(unsigned long size_, unsigned blocking_, unsigned distance_) :
  Memory(),
  size(size_),
  blocking(blocking_),
  distance(distance_)
{}

BasicMemory::BasicMemory(const BasicMemory & arg) :
  Memory(arg),
  size(arg.size),
  blocking(arg.blocking),
  distance(arg.distance)
{}

BasicMemory::~BasicMemory() {}
 
BasicMemory * BasicMemory::copy() const {
  return new BasicMemory(*this);
}

MemoryHierarchy::MemoryHierarchy() :
  Memory(),
  levels()
{}

MemoryHierarchy::MemoryHierarchy(const MemoryHierarchy & arg) :
  Memory(arg),
  levels()
{
  for (unsigned i = 0; i < arg.levels.size(); i++) {
    assert(arg.levels[i] != NULL);
    levels.push_back(arg.levels[i]->copy());
  }
}

MemoryHierarchy::~MemoryHierarchy() {
  for (unsigned i = 0; i < levels.size(); i++)
    delete levels[i];
}

void MemoryHierarchy::add(Memory * mem) {
  levels.push_back(mem);
}

unsigned MemoryHierarchy::size() const {
  return levels.size();
}

// 1 <= i <= nbr_lvl
Memory * MemoryHierarchy::get(unsigned i) const {
  assert(levels.size() > i-1);
  return levels[i-1];
}

MemoryHierarchy * MemoryHierarchy::copy() const {
  return new MemoryHierarchy(*this);
}

Cache::Cache(unsigned long size_, unsigned block_, unsigned distance_, unsigned way_, CachePolicy policy_) :
  BasicMemory(size_, block_, distance_),
  way(way_),
  policy(policy_)
{}

Cache::Cache(const Cache & arg) :
  BasicMemory(arg),
  way(arg.way),
  policy(arg.policy)
{}

Cache::~Cache() {}

Cache * Cache::copy() const {
  return new Cache(*this);
}

