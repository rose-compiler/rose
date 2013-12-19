
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

/*!
 * \addtogroup grp_klt_ocl
 * @{
*/

class Kernel : public virtual Core::Kernel {
  public:
    struct local_symbol_maps_t {
      std::map<SgVariableSymbol *, SgVariableSymbol *> parameters;
      std::map<SgVariableSymbol *, SgVariableSymbol *> coefficients;
      std::map<Core::Data *, SgVariableSymbol *> datas;
      std::map<SgVariableSymbol *, SgVariableSymbol *> iterators;
    };

    struct coordinate_symbols_t {
      std::vector<SgVariableSymbol *> global_ids;
      std::vector<SgVariableSymbol *> local_ids;
    };

    struct dimensions_t {
      /// Number of dimensions of the grid of threads
      unsigned int number_dims;

      /// Global size of each dimension of the grid of thread (kind of SPMD)
      std::vector<SgExpression *> global_work_size;

      bool have_local_work_size;

      /// Local size of each dimension of the grid of thread (kind of SIMD)
      std::vector<SgExpression *> local_work_size;
    };

    struct a_kernel {
      /// Name of the OpenCL kernel
      std::string kernel_name;
      
      dimensions_t dimensions;
    };

  protected:
    /// A set of iteration mapping for each loop distribution
    std::map<loop_mapping_t *, std::set<Core::IterationMap<Kernel> *> > p_iteration_maps;

    /// A map between the different iteration mapping and the produced kernel for each loop distribution
    std::map<loop_mapping_t *, std::map<Core::IterationMap<Kernel> *, a_kernel *> > p_kernel_map;

  public:
    Kernel(Core::LoopTrees::node_t * root);
    virtual ~Kernel();

    std::set<Core::IterationMap<Kernel> *> & getIterationMaps(loop_mapping_t * loop_mapping);
    const std::set<Core::IterationMap<Kernel> *> & getIterationMaps(loop_mapping_t * loop_mapping) const;

    void setKernel(loop_mapping_t * loop_mapping, Core::IterationMap<Kernel> * iteration_map, a_kernel * kernel);
    a_kernel * getKernel(loop_mapping_t * loop_mapping, Core::IterationMap<Kernel> * iteration_map) const;
};

/** @} */

}

}

#endif /* __KLT_OPENCL_KERNEL_HPP__ */

