
#ifndef __KLT_CORE_KERNEL_HPP__
#define __KLT_CORE_KERNEL_HPP__

#include <list>
#include <set>

class SgVariableSymbol;

namespace KLT {

namespace Core {

class Data;

class Kernel {
  public:
    const unsigned long id;

  protected:
    /// Datas flowing in this kernel/task
    std::set<Data *> p_data_in;
    /// Datas flowing out this kernel/task
    std::set<Data *> p_data_out;

    /// List of the datas (union of flow-in and flow-out and local) in the order they need to be provided to the kernel
    std::list<Data *> p_datas_argument_order;
    /// List of the parameters in the order they need to be provided to the kernel
    std::list<SgVariableSymbol *> p_parameters_argument_order;

  protected:
    Kernel();

  public:
    virtual ~Kernel();

    /// \return Datas flowing in this kernel/task
    const std::set<Data *> & getFlowingIn() const;

    /// \return Datas flowing out of this kernel/task
    const std::set<Data *> & getFlowingOut() const;

    /// \return  List of the datas (union of flow-in and flow-out) in the order they need to be provided to the kernel
    const std::list<Data *> & getDatasArguments() const;

    /// \return List of the parameters in the order they need to be provided to the kernel
    const std::list<SgVariableSymbol *> & getParametersArguments() const;

  private:
    static unsigned long id_cnt;
};

}

}

#endif /* __KLT_CORE_KERNEL_HPP__ */

