
#include "single-gpu/single-gpu.hpp"
#include "compute-systems/memory.hpp"

#include <cassert>

PCIe::PCIe() :
  Accelerator_Interconnect()
{}

PCIe::PCIe(const PCIe & arg) :
  Accelerator_Interconnect(arg)
{}

PCIe::~PCIe() {}

PCIe * PCIe::copy() const {
  return new PCIe(*this);
}

DummyGPU::DummyGPU(ComputeSystem * parent) :
  GPU(parent, 8, new GPU_simd(this, 16, new BasicMemory(128*1024, 4, 5)), new MemoryHierarchy())
{
  MemoryHierarchy * mem_hierarchy = dynamic_cast<MemoryHierarchy *>(shared);
  assert(mem_hierarchy != NULL);
  mem_hierarchy->add(new BasicMemory(4*1024*1024*1024lu, 1024, 200));
}

DummyGPU::DummyGPU(const DummyGPU & arg) :
  GPU(arg)
{}

DummyGPU::~DummyGPU() {}

DummyGPU * DummyGPU::copy() const {
  return new DummyGPU(*this);
}

SingleGPU_SingleCPU::SingleGPU_SingleCPU() :
  ComputeNode(new Core(this, new MemoryHierarchy())) 
{
  Core * core = dynamic_cast<Core *>(cpu);
  assert(core != NULL);
  core->getMemory()->add(new       Cache(64*1024,              16,   2));                // L1: 64KB /  16B /  2 cycles
  core->getMemory()->add(new       Cache(1024*1024,            64,  20));                // L2:  1MB /  64B / 20 cycles
  core->getMemory()->add(new       Cache(4*1024*1024,         256,  50, 4, Cache::LRU)); // L3:  4MB / 256B / 50 cycles / 4 ways LRU
  core->getMemory()->add(new BasicMemory(4*1024*1024*1024lu, 1024, 200));                // MEM: 4 GB / 1KB / 200 cycles

  addAccelerator(new DummyGPU(this), new PCIe());
}

SingleGPU_SingleCPU::SingleGPU_SingleCPU(const SingleGPU_SingleCPU & arg) :
  ComputeNode(arg)
{}

SingleGPU_SingleCPU::~SingleGPU_SingleCPU() {}

SingleGPU_SingleCPU * SingleGPU_SingleCPU::copy() const {
  return new SingleGPU_SingleCPU(*this);
}

