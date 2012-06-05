
#ifndef __ARRAY_SYSTEM_HPP__
#define __ARRAY_SYSTEM_HPP__

#include "common/compute-system.hpp"
#include "compute-systems/memory.hpp"

class ArraySystem : public ComputeSystem {
  protected:
    std::vector<unsigned> dimensions;
    ComputeSystem ** array;

    Memory * shared;
    Link * interconnect;

  public:
    ArraySystem(std::vector<unsigned> dimensions_, ComputeSystem * element, Memory * shared_, Link * interconnect_, ComputeSystem * parent = NULL);
    ArraySystem(const ArraySystem & arg);
    virtual ~ArraySystem();

    std::vector<unsigned> getPosition(ComputeSystem * element) const;

    virtual Link * getLink(ComputeSystem * cs1, ComputeSystem * cs2) const;

    Memory * getSharedMemory() const;
    Link * getInterconnect() const;

    virtual ArraySystem * copy() const = 0;
};

#endif /* __ARRAY_SYSTEM_HPP__ */

