
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

    struct loop_distribution_t {
      /// the loop nest to be distributed over threads/work-item/processor/...
      std::list<LoopTrees::loop_t *> loop_nest;
      /// remaining statement to be put in the body of the kernel (inside the loop nest)
      std::list<LoopTrees::node_t *> body;
    };

    struct descriptor_t {
      /// Parameters : reference order for call argument
      const std::list<SgVariableSymbol *> & parameters_argument_order; 
      /// Coefficients : reference order for call argument
      const std::list<SgVariableSymbol *> & coefficients_argument_order;
      /// Datas : reference order for call argument
      const std::list<Data *> & datas_argument_order;

      /// Loop Distribution
      const loop_distribution_t & loop_distribution;

      descriptor_t(const Kernel & kernel_, const loop_distribution_t & loop_distribution_);
    };

  protected:
    /// Set true when the data flow analysis have been performed
    bool p_dataflow_done;
    /// Datas flowing in this kernel/task
    std::set<Data *> p_data_in;
    /// Datas flowing out this kernel/task
    std::set<Data *> p_data_out;

    /// Set true when the ordered symbol list have been produced. 
    bool p_arguments_done;
    /// Parameters : reference order for call argument
    std::list<SgVariableSymbol *> p_parameters_argument_order;
    /// Coefficients : reference order for call argument
    std::list<SgVariableSymbol *> p_coefficients_argument_order;
    /// Datas : reference order for call argument
    std::list<Data *> p_datas_argument_order;

    ///
    bool p_loop_distribution_done;
    /// Set of possible loop distributions
    std::set<loop_distribution_t *> p_loop_distributions;

  protected:
    Kernel();

  public:
    virtual ~Kernel();

    /// Set the result of the Data Flow Analysis (called only once), set flag p_dataflow_done
    void setDataflow(const std::set<Data *> & data_in, const std::set<Data *> & data_out);
    /// \return true if the dataflow analysis results are available
    bool isDataflowDone() const;
    /// \return Datas flowing in this kernel/task
    const std::set<Data *> & getFlowingIn() const;
    /// \return Datas flowing out of this kernel/task
    const std::set<Data *> & getFlowingOut() const;

    /// Set the result of the Argument Analysis (called only once), set flag p_arguments_done
    void setArgument(
      const std::list<SgVariableSymbol *> & parameters_argument_order,
      const std::list<SgVariableSymbol *> & coefficients_argument_order,
      const std::list<Data *> & datas_argument_order
    );
    /// \return true if the reference argument order have been generated
    bool isArgumentDone() const;
    /// \return Parameters reference order
    const std::list<SgVariableSymbol *> & getParametersArguments() const;
    /// \return Coefficients reference oreder
    const std::list<SgVariableSymbol *> & getCoefficientsArguments() const;
    /// \return Datas reference order
    const std::list<Data *> & getDatasArguments() const;

    /// set the different loop distributions
    void setLoopDistributions(const std::set<loop_distribution_t *> & loop_distributions);
    /// \return true if the loop distribution have been done
    bool isLoopDistributionDone() const;
    /// \return OAthe different loop distributions
    const std::set<loop_distribution_t *> & getLoopDistributions() const;

    // \return true if the Iteration Maps have been produced
    virtual bool isIterationMapDone() const = 0;

    // \return true if the Kernels have been produced
    virtual bool areKernelsDone() const = 0;

  private:
    static unsigned long id_cnt;

  friend class Generator;
};

}

}

#endif /* __KLT_CORE_KERNEL_HPP__ */

