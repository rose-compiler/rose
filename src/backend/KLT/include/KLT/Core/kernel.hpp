
#ifndef __KLT_CORE_KERNEL_HPP__
#define __KLT_CORE_KERNEL_HPP__

#include "KLT/Core/loop-trees.hpp"

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
    ///
    bool p_dataflow_done;
    /// Datas flowing in this kernel/task
    std::set<Data *> p_data_in;
    /// Datas flowing out this kernel/task
    std::set<Data *> p_data_out;

    ///
    bool p_arguments_done;
    /// List of the datas (union of flow-in and flow-out and local) in the order they need to be provided to the kernel
    std::list<Data *> p_datas_argument_order;
    /// List of the parameters in the order they need to be provided to the kernel (uses original symbol)
    std::list<SgVariableSymbol *> p_parameters_argument_order;
    /// List of the coefficients in the order they need to be provided to the kernel (uses original symbol)
    std::list<SgVariableSymbol *> p_coefficients_argument_order;

    ///
    bool p_content_done;
    /// the list of perfectly nested loops, it will become the outter most iteration domain of the kernel (fully parallel) [internal]
    std::list<LoopTrees::loop_t *> p_perfectly_nested_loops;
    /// the list of statements composing the body of the perfectly nested loop.
    std::list<LoopTrees::node_t *> p_body_branches;

  protected:
    Kernel();

  public:
    virtual ~Kernel();

    /// Set the result of the Data Flow Analysis (called only once), set flag p_dataflow_done
    void setDataflow(const std::set<Data *> & data_in, const std::set<Data *> & data_out);

    /// Set the result of the Argument Analysis (called only once), set flag p_arguments_done
    void setArgument(const std::list<Data *> &datas_argument_order, const std::list<SgVariableSymbol *> & parameters_argument_order, const std::list<SgVariableSymbol *> & coefficients_argument_order);

    /// Set the result of the Content Analysis (called only once), set flag p_content_done
    void setContent(const std::list<LoopTrees::loop_t *> & perfectly_nested_loops, const std::list<LoopTrees::node_t *> & body_branches);

    /// \return true if the dataflow analysis results are available
    bool isDataflowDone() const;

    /// \return Datas flowing in this kernel/task
    const std::set<Data *> & getFlowingIn() const;

    /// \return Datas flowing out of this kernel/task
    const std::set<Data *> & getFlowingOut() const;

    /// \return true if the argument have been processed
    bool isArgumentDone() const;

    /// \return  List of the datas (union of flow-in and flow-out) in the order they need to be provided to the kernel
    const std::list<Data *> & getDatasArguments() const;

    /// \return List of the parameters in the order they need to be provided to the kernel
    const std::list<SgVariableSymbol *> & getParametersArguments() const;

    /// \return true if the associated "perfecly nested loops" and "body" have been set
    bool isContentDone() const;

  private:
    static unsigned long id_cnt;

  friend class Generator;
};

}

}

#endif /* __KLT_CORE_KERNEL_HPP__ */

