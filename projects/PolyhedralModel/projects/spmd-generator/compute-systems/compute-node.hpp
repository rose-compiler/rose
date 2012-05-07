
#ifndef __COMPUTE_NODE_HPP__
#define __COMPUTE_NODE_HPP__ 

#include <set>
#include <vector>
#include <utility>

#include "common/compute-system.hpp"
#include "compute-systems/array-system.hpp"
#include "compute-systems/memory.hpp"

// as QPI
class CPU_Interconnect : public Link {
  public:
    CPU_Interconnect();
    virtual ~CPU_Interconnect();

    virtual CPU_Interconnect * copy() const = 0;
};

// as PCIe
class Accelerator_Interconnect : public Link {
  public:
   Accelerator_Interconnect();
   virtual ~Accelerator_Interconnect();

   virtual Accelerator_Interconnect * copy() const = 0;
};

// as internal CPU crossbar
class Internal_CPU_Interconnect : public Link {
  public:
    Internal_CPU_Interconnect();
    virtual ~Internal_CPU_Interconnect();

    virtual Internal_CPU_Interconnect * copy() const = 0;
};

class ComputeNode : public ComputeSystem {
  protected:
    ComputeSystem * cpu;
    std::vector<std::pair<ComputeSystem *, Accelerator_Interconnect *> > accelerators;

    std::set<Accelerator_Interconnect *> internal_links;

  public:
    ComputeNode(ComputeSystem * cpu_, ComputeSystem * parent_ = NULL);
    ComputeNode(const ComputeNode & arg);
    virtual ~ComputeNode();

    void addAccelerator(ComputeSystem * acc, Accelerator_Interconnect * link);

    unsigned getAcceleratorID(ComputeSystem * acc) const;

    ComputeSystem * getAcceleratorByID(unsigned id) const;
    Accelerator_Interconnect * getLinkByAccID(unsigned id) const;

    ComputeSystem * getCPU() const;

    virtual Link * getLink(ComputeSystem * cs1, ComputeSystem * cs2) const;

    virtual ComputeNode * copy() const = 0;
};

class Core : public ComputeSystem {
  protected:
    MemoryHierarchy * memory;
    unsigned nbr_threads;

  public:
    Core(ComputeSystem * parent, MemoryHierarchy * memory_, unsigned nbr_threads_ = 1);
    Core(const Core & arg);
    virtual ~Core();

    MemoryHierarchy * getMemory() const;

    virtual Link * getLink(ComputeSystem * cs1, ComputeSystem * cs2) const;

    virtual Core * copy() const;
}; 

class MultiCore : public ArraySystem {
  public:
    MultiCore(ComputeSystem * parent, unsigned nbr_unit, Core * element, MemoryHierarchy * shared_, Internal_CPU_Interconnect * link);
    MultiCore(const MultiCore & arg);
    virtual ~MultiCore();

    virtual MultiCore * copy() const;
};

#endif /* __COMPUTE_NODE_HPP__ */

