
#include "compute-systems/array-system.hpp"

#include <cassert>>

ArraySystem::ArraySystem(
  std::vector<unsigned> dimensions_,
  ComputeSystem * element,
  Memory * shared_,
  Link * interconnect_,
  ComputeSystem * parent
) :
  ComputeSystem(parent),
  dimensions(dimensions_),
  array(NULL),
  shared(shared_),
  interconnect(interconnect_)
{
  unsigned long nbr_elem = 1;
  for (int i = 0; i < dimensions.size(); i++) nbr_elem *= dimensions[i];

  array = new ComputeSystem*[nbr_elem];

  array[0] = element;
  array[0]->setParent(this);
  for (unsigned long i = 1; i < nbr_elem; i++) {
    array[i] = element->copy();
    array[i]->setParent(this);
  }
}

ArraySystem::ArraySystem(const ArraySystem & arg) :
  ComputeSystem(arg.parent),
  dimensions(arg.dimensions),
  array(NULL),
  shared(arg.shared->copy()),
  interconnect(arg.interconnect->copy())
{
  unsigned long nbr_elem = 1;
  for (unsigned i = 0; i < dimensions.size(); i++) nbr_elem *= dimensions[i];

  array = new ComputeSystem*[nbr_elem];

  for (unsigned long i = 0; i < nbr_elem; i++) {
    array[i] = arg.array[i]->copy();
    array[i]->setParent(this); 
  }
}

ArraySystem::~ArraySystem() {
  if (shared != NULL) delete shared;
  if (interconnect != NULL) delete interconnect;
  if (array != NULL) {
    unsigned long nbr_elem = 1;
    for (unsigned i = 0; i < dimensions.size(); i++) nbr_elem *= dimensions[i];

    for (unsigned long i = 0; i < nbr_elem; i++)
      if (array[i] != NULL)
        delete array[i];

    delete array;
  }
}

std::vector<unsigned> ArraySystem::getPosition(ComputeSystem * element) const {
  unsigned long nbr_elem = 1;
  for (unsigned i = 0; i < dimensions.size(); i++) nbr_elem *= dimensions[i];

  unsigned long pos = 0;
  for (pos = 0; pos < nbr_elem; pos++)
    if (array[pos] == element)
      break;
  assert(pos < nbr_elem);

  std::vector<unsigned> res(dimensions.size(), 0);

  assert(false); // TODO

  return res;
}

Link * ArraySystem::getLink(ComputeSystem * cs1, ComputeSystem * cs2) const {
  assert(false); // TODO
  return NULL;
}

Memory * ArraySystem::getSharedMemory() const { return shared; }

Link * ArraySystem::getInterconnect() const { return interconnect; }

