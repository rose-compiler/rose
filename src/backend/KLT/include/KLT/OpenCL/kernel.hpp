
#ifndef __KLT_OPENCL_KERNEL_HPP__
#define __KLT_OPENCL_KERNEL_HPP__

#include "KLT/Core/kernel.hpp"

#include <string>
#include <vector>
#include <map>

class SgExpression;

namespace KLT {

namespace Core {
template <typename Kernel> class IterationMap;
}

namespace OpenCL {

class Kernel : public virtual Core::Kernel {
  public:
    struct argument_symbol_maps_t {
      std::map<SgVariableSymbol *, SgVariableSymbol *> param_to_args;
      std::map<SgVariableSymbol *, SgVariableSymbol *> coef_to_args;
      std::map<Core::Data *, SgVariableSymbol *> data_to_args;
    };

    struct coordinate_symbols_t {
      std::list<SgVariableSymbol *> global_ids;
      std::list<SgVariableSymbol *> local_ids;
    };

    struct an_opencl_kernel {
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
    /// Set true when the diferent iteration mapping have been produced
    bool p_opencl_iteration_maps_done;
    /// A set of iteration mapping for each loop distribution
    std::map<loop_distribution_t *, std::set<Core::IterationMap<Kernel> *> > p_opencl_iteration_maps;

    /// Set true when the kernel (code/AST) have been produced
    bool p_kernels_done;
    /// A map between the different iteration mapping and the produced kernel for each loop distribution
    std::map<loop_distribution_t *, std::map<Core::IterationMap<Kernel> *, an_opencl_kernel *> > p_opencl_kernel_map;
    /// All the OpenCL kernel that have been generated
    std::set<an_opencl_kernel *> p_opencl_kernels;

  public:
    Kernel();
    virtual ~Kernel();
};

}

}

#endif /* __KLT_OPENCL_KERNEL_HPP__ */

