
#ifndef __KLT_SEQUENTIAL_KERNEL_HPP__
#define __KLT_SEQUENTIAL_KERNEL_HPP__

#include "KLT/Core/kernel.hpp"

class SgClassSymbol;
class SgFunctionSymbol;
class SgMemberFunctionSymbol;

namespace KLT {

namespace Core {
template <typename Kernel> class IterationMap;
}

namespace Sequential {

/*!
 * \addtogroup grp_klt_seq
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

    struct coordinate_symbols_t {};

    struct dimensions_t {};

    struct a_kernel {
      /// Symbol associated to the generated kernel
      SgFunctionSymbol * kernel;

      /// Symbol associated to the "arguments packer", a struct used to be pass the arguments as one void pointer
      SgClassSymbol * arguments_packer;

      /// Symbol for the constructor of the argument packer
      SgMemberFunctionSymbol * arguments_packer_ctor;
      
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

#endif /* __KLT_SEQUENTIAL_KERNEL_HPP__ */

