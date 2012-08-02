
#include "compute-systems/compute-node.hpp"

#include <map>
#include <cassert>

CPU_Interconnect::CPU_Interconnect() :
  Link()
{}

CPU_Interconnect::~CPU_Interconnect() {}

Accelerator_Interconnect::Accelerator_Interconnect() :
  Link()
{}

Accelerator_Interconnect::~Accelerator_Interconnect() {}

Internal_CPU_Interconnect::Internal_CPU_Interconnect() :
  Link()
{}

Internal_CPU_Interconnect::~Internal_CPU_Interconnect() {}

ComputeNode::ComputeNode(ComputeSystem * cpu_, ComputeSystem * parent_) :
  ComputeSystem(parent_),
  cpu(cpu_),
  accelerators(),
  internal_links()
{}

ComputeNode::ComputeNode(const ComputeNode & arg) :
  ComputeSystem(arg),
  cpu(arg.cpu == NULL ? NULL : arg.cpu->copy()),
  accelerators(),
  internal_links()
{
  std::map<Accelerator_Interconnect *, Accelerator_Interconnect *> map;
  std::set<Accelerator_Interconnect *>::const_iterator it_link;
  for (it_link = arg.internal_links.begin(); it_link != arg.internal_links.end(); it_link++) {
    Accelerator_Interconnect * copy = *it_link == NULL ? NULL : (*it_link)->copy();
    internal_links.insert(copy);
    map.insert(std::pair<Accelerator_Interconnect *, Accelerator_Interconnect *>(*it_link, copy));
  }

  std::vector<std::pair<ComputeSystem *, Accelerator_Interconnect *> >::const_iterator it_acc;
  for (it_acc = arg.accelerators.begin(); it_acc != arg.accelerators.end(); it_acc++) {
    ComputeSystem * copy = it_acc->first == NULL ? NULL : it_acc->first->copy();
    std::map<Accelerator_Interconnect *, Accelerator_Interconnect *>::iterator it_map = map.find(it_acc->second);
    assert(it_map != map.end());
    accelerators.push_back(std::pair<ComputeSystem *, Accelerator_Interconnect *>(copy, it_map->second));
  }
}

ComputeNode::~ComputeNode() {
  std::vector<std::pair<ComputeSystem *, Accelerator_Interconnect *> >::iterator it_acc;
  for (it_acc = accelerators.begin(); it_acc != accelerators.end(); it_acc++) if (it_acc->first != NULL) delete it_acc->first;
  std::set<Accelerator_Interconnect *>::iterator it_link;
  for (it_link = internal_links.begin(); it_link != internal_links.end(); it_link++) if (*it_link != NULL) delete *it_link;
}

void ComputeNode::addAccelerator(ComputeSystem * acc, Accelerator_Interconnect * link) {
  accelerators.push_back(std::pair<ComputeSystem *, Accelerator_Interconnect *>(acc, link));
  internal_links.insert(link);
}

unsigned ComputeNode::getAcceleratorID(ComputeSystem * acc) const {
  for (unsigned i = 0; i < accelerators.size(); i++)
    if (accelerators[i].first == acc)
      return i;
  assert(false);
}

ComputeSystem * ComputeNode::getAcceleratorByID(unsigned id) const {
  if (id >= accelerators.size()) return NULL;
  else return accelerators[id].first;
}

Accelerator_Interconnect * ComputeNode::getLinkByAccID(unsigned id) const {
  if (id >= accelerators.size()) return NULL;
  else return accelerators[id].second;
}

ComputeSystem * ComputeNode::getCPU() const {
  return cpu;
}

Link * ComputeNode::getLink(ComputeSystem * cs1, ComputeSystem * cs2) const {
  assert(false); // TODO
  return NULL;
}

Core::Core(ComputeSystem * parent, MemoryHierarchy * memory_, unsigned nbr_threads_) :
  ComputeSystem(parent),
  memory(memory_),
  nbr_threads(nbr_threads_)
{}

Core::Core(const Core & arg) :
  ComputeSystem(arg),
  memory(arg.memory == NULL ? NULL : arg.memory->copy()),
  nbr_threads(arg.nbr_threads)
{}

Core::~Core() {
  if (memory != NULL) delete memory;
}

MemoryHierarchy * Core::getMemory() const {
  return memory;
}

Link * Core::getLink(ComputeSystem * cs1, ComputeSystem * cs2) const {
  assert(false); // TODO
  return NULL;
}

Core * Core::copy() const {
  return new Core(*this);
}

MultiCore::MultiCore(ComputeSystem * parent, unsigned nbr_unit, Core * element, MemoryHierarchy * shared_, Internal_CPU_Interconnect * link) :
  ArraySystem(std::vector<unsigned>(1, nbr_unit), element, shared_, link, parent)
{}

MultiCore::MultiCore(const MultiCore & arg) :
  ArraySystem(arg)
{}

MultiCore::~MultiCore() {}

MultiCore * MultiCore::copy() const {
  return new MultiCore(*this);
}

