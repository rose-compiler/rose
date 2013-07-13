
#include "KLT/Sequential/kernel.hpp"

#include <cassert>

namespace KLT {

namespace Sequential {

Kernel::Kernel() :
  Core::Kernel(),
  p_sequential_iteration_maps_done(false),
  p_sequential_iteration_maps(),
  p_sequential_kernels_done(false),
  p_sequential_kernel_map(),
  p_sequential_kernels()
{}

Kernel::~Kernel() {
  // TODO std::set<a_sequential_kernel *> p_sequential_kernels
}

bool Kernel::isIterationMapDone() const { return p_sequential_iteration_maps_done; }

bool Kernel::areKernelsDone() const { return p_sequential_kernels_done; }

}

}

