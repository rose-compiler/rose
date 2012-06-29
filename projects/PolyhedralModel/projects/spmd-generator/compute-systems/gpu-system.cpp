
#include "compute-systems/gpu-system.hpp"

#include <cassert>

GPU_ALU::GPU_ALU(GPU_simd * parent) :
  ComputeSystem(parent)
{}

GPU_ALU::GPU_ALU(const GPU_ALU & arg) :
  ComputeSystem(arg)
{}

GPU_ALU::~GPU_ALU() {}

GPU_ALU * GPU_ALU::copy() const {
  return new GPU_ALU(*this);
}

Link * GPU_ALU::getLink(ComputeSystem * cs1, ComputeSystem * cs2) const {
  assert(false); // TODO
  return NULL;
}

GPU_simd::GPU_simd(GPU * parent, unsigned nbr_unit, Memory * shared_) : 
  ArraySystem(std::vector<unsigned>(1, nbr_unit), new GPU_ALU(), shared_, NULL, parent)
{}

GPU_simd::GPU_simd(GPU * parent, unsigned nbr_unit, Memory * shared_, GPU_ALU * specific_alu) :
  ArraySystem(std::vector<unsigned>(1, nbr_unit), specific_alu, shared_, NULL, parent)
{}

GPU_simd::GPU_simd(const GPU_simd & arg) :
  ArraySystem(arg)
{}

GPU_simd::~GPU_simd() {}

GPU_simd * GPU_simd::copy() const {
  return new GPU_simd(*this);
}

GPU::GPU(ComputeSystem * parent, unsigned nbr_unit, GPU_simd * element, MemoryHierarchy * shared_) :
  ArraySystem(std::vector<unsigned>(1, nbr_unit), element, shared_, NULL, parent)
{}

GPU::GPU(const GPU & arg) :
  ArraySystem(arg)
{}

GPU::~GPU() {}

