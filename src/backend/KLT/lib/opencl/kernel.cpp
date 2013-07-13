
#include "KLT/OpenCL/kernel.hpp"

namespace KLT {

namespace OpenCL {

Kernel::Kernel() :
  Core::Kernel(),
  p_opencl_iteration_maps_done(false),
  p_opencl_iteration_maps(),
  p_kernels_done(false),
  p_opencl_kernel_map(),
  p_opencl_kernels()
{}

Kernel::~Kernel() {
  // TODO std::set<a_opencl_kernel *> p_opencl_kernels
}

}

}
