
#ifndef __KLT_OPENCL_KERNEL_HPP__
#define __KLT_OPENCL_KERNEL_HPP__

#include "KLT/Core/kernel.hpp"

#include <string>
#include <vector>

class SgExpression;

namespace KLT {

namespace OpenCL {

class Kernel : public virtual Core::Kernel {
  public:
    struct a_kernel {
      /// Name of the OpenCL kernel
      std::string kernel_name;

      /// Number of dimensions of the grid of threads
      unsigned int number_dims;

      /// Global size of each dimension of the grid of thread (kind of SPMD)
      std::vector<SgExpression *> global_work_size;

      /// Local size of each dimension of the grid of thread (kind of SIMD)
      std::vector<SgExpression *> local_work_size;

      bool have_local_work_size;
    };

  protected:
    std::set<a_kernel *> p_kernels;

  public:
    Kernel();
    virtual ~Kernel();

    void addKernel(a_kernel * kernel);
    const std::set<a_kernel *> & getKernels() const;
};

}

}

#endif /* __KLT_OPENCL_KERNEL_HPP__ */

