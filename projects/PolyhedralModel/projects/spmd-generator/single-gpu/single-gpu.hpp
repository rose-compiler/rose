
#ifndef __SINGLE_GPU_HPP__
#define __SINGLE_GPU_HPP__

#include "compute-systems/compute-node.hpp"
#include "compute-systems/gpu-system.hpp"

class PCIe : public Accelerator_Interconnect {
  public:
    PCIe();
    PCIe(const PCIe & arg);
    virtual ~PCIe();

    virtual PCIe * copy() const;
};

class DummyGPU : public GPU {
  public:
    DummyGPU(ComputeSystem * parent);
    DummyGPU(const DummyGPU & arg);
    virtual ~DummyGPU();

    virtual DummyGPU * copy() const;
};

class SingleGPU_SingleCPU : public ComputeNode {
  public:
    SingleGPU_SingleCPU();
    SingleGPU_SingleCPU(const SingleGPU_SingleCPU & arg);
    virtual ~SingleGPU_SingleCPU();

    virtual SingleGPU_SingleCPU * copy() const;
};

#endif /* __SINGLE_GPU_HPP__ */

