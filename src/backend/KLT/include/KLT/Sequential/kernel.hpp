
#ifndef __KLT_SEQUENTIAL_KERNEL_HPP__
#define __KLT_SEQUENTIAL_KERNEL_HPP__

#include "KLT/Core/kernel.hpp"

class SgClassSymbol;
class SgFunctionSymbol;

namespace KLT {

namespace Sequential {

class Kernel : public virtual Core::Kernel {
  protected:
    /// Symbol associated to the generated kernel
    SgFunctionSymbol * p_kernel_symbol;

    /// Symbol associated to the "arguments packer", a struct used to be pass the arguments as one void pointer
    SgClassSymbol * p_arguments_packer;

  public:
    Kernel();
    virtual ~Kernel();

    /// \return Symbol associated to the generated kernel
    SgFunctionSymbol * getKernelSymbol() const;

    /// \return Symbol associated to the "arguments packer", a struct used to be pass the arguments as one void pointer
    SgClassSymbol * getArgumentsPacker() const;
};

}

}

#endif /* __KLT_SEQUENTIAL_KERNEL_HPP__ */

