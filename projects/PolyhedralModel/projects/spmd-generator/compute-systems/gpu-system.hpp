
#ifndef __GPU_SYSTEM_HPP__
#define __GPU_SYSTEM_HPP__

#include "compute-systems/array-system.hpp"
#include "compute-systems/memory.hpp"

class GPU_simd;
class GPU;

class GPU_ALU : public ComputeSystem {
  public:
    GPU_ALU(GPU_simd * parent = NULL);
    GPU_ALU(const GPU_ALU & arg);
    virtual ~GPU_ALU();

    virtual Link * getLink(ComputeSystem * cs1, ComputeSystem * cs2) const;

    virtual GPU_ALU * copy() const;
};

class GPU_simd : public ArraySystem {
  public:
    GPU_simd(GPU * parent, unsigned nbr_unit, Memory * shared_);
    GPU_simd(GPU * parent, unsigned nbr_unit, Memory * shared_, GPU_ALU * specific_alu);
    GPU_simd(const GPU_simd & arg);
    virtual ~GPU_simd();

    virtual GPU_simd * copy() const;
};

class GPU : public ArraySystem {
  public:
    GPU(ComputeSystem * parent, unsigned nbr_unit, GPU_simd * element, MemoryHierarchy * shared_);
    GPU(const GPU & arg);
    virtual ~GPU();

    virtual GPU * copy() const = 0;
};

#endif /* __GPU_SYSTEM_HPP__ */

