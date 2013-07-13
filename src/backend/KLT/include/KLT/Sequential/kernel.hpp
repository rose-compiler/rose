
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

class Kernel : public virtual Core::Kernel {
  public:
    struct argument_symbol_maps_t {
      std::map<SgVariableSymbol *, SgVariableSymbol *> param_to_args;
      std::map<SgVariableSymbol *, SgVariableSymbol *> coef_to_args;
      std::map<Core::Data *, SgVariableSymbol *> data_to_args;
    };

    struct coordinate_symbols_t {};

    struct a_sequential_kernel {
      /// Symbol associated to the generated kernel
      SgFunctionSymbol * kernel;

      /// Symbol associated to the "arguments packer", a struct used to be pass the arguments as one void pointer
      SgClassSymbol * arguments_packer;

      /// Symbol for the constructor of the argument packer
      SgMemberFunctionSymbol * arguments_packer_ctor;
    };

  protected:
    /// Set true when the diferent iteration mapping have been produced
    bool p_sequential_iteration_maps_done;
    /// A set of iteration mapping for each loop distribution
    std::map<loop_distribution_t *, std::set<Core::IterationMap<Kernel> *> > p_sequential_iteration_maps;

    /// Set true when the kernel (code/AST) have been produced
    bool p_sequential_kernels_done;
    /// A map between the different iteration mapping and the produced kernel for each loop distribution
    std::map<loop_distribution_t *, std::map<Core::IterationMap<Kernel> *, a_sequential_kernel *> > p_sequential_kernel_map;
    /// All the sequential kernel that have been generated
    std::set<a_sequential_kernel *> p_sequential_kernels;

  public:
    Kernel();
    virtual ~Kernel();

    virtual bool isIterationMapDone() const;
    
    virtual bool areKernelsDone() const;
};

}

}

#endif /* __KLT_SEQUENTIAL_KERNEL_HPP__ */

