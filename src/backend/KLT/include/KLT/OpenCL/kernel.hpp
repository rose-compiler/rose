
#ifndef __KLT_OPENCL_KERNEL_HPP__
#define __KLT_OPENCL_KERNEL_HPP__

#include "KLT/Core/kernel.hpp"

#include <string>
#include <vector>

class SgExpression;

namespace KLT {

namespace OpenCL {

class Kernel : public virtual Core::Kernel {
  protected:
    /// Name of the OpenCL kernel
    std::string p_kernel_name;

    /// Number of dimensions of the grid of threads
    unsigned int p_ndims;

    /// Global size of each dimension of the grid of thread (kind of SPMD)
    std::vector<SgExpression *> p_global_work_size;

    /// Local size of each dimension of the grid of thread (kind of SIMD)
    std::vector<SgExpression *> p_local_work_size;

  public:
    Kernel();
    virtual ~Kernel();

    /// \return Name of the OpenCL kernel
    const std::string & getKernelName() const;

    /// \return Number of dimensions of the grid of threads
    unsigned int getNumberDimensions() const;

    /// \return Global size of each dimension of the grid of thread (kind of SPMD)
    const std::vector<SgExpression *> & getGlobalWorkSize() const;

    /// \return Local size of each dimension of the grid of thread (kind of SIMD)
    const std::vector<SgExpression *> & getLocalWorkSize() const;
};

}

}

#endif /* __KLT_OPENCL_KERNEL_HPP__ */

