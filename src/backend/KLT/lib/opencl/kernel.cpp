
#include "KLT/OpenCL/kernel.hpp"

namespace KLT {

namespace OpenCL {

Kernel::Kernel() :
  Core::Kernel(),
  p_kernels()
{}

Kernel::~Kernel() {}

const std::set<Kernel::a_kernel *> & Kernel::getKernels() const { return p_kernels; }

void Kernel::addKernel(Kernel::a_kernel * kernel) { p_kernels.insert(kernel); }

}

}
