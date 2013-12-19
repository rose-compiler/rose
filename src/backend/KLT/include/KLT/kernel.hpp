
#ifndef __KLT_KERNEL_HPP__
#define __KLT_KERNEL_HPP__

#include "KLT/loop-trees.hpp"

#include <list>
#include <set>

class SgVariableSymbol;

namespace KLT {

/*!
 * \addtogroup grp_klt_kernel
 * @{
*/

template <class DataAnnotation, class RegionAnnotation, class LoopAnnotation, class Language, class Runtime>
class Kernel {
  public:
    const unsigned long id;

  public:
    struct loop_mapping_t {
      /// the loop nest to be distributed over threads/work-item/processor/...
      std::list<LoopTrees::loop_t *> loop_nest;
      /// remaining statement to be put in the body of the kernel (inside the loop nest)
      std::list<LoopTrees::node_t *> body;
    };

    struct dataflow_t {
      /// All datas
      std::set<Data *> datas;

      /// Read datas
      std::set<Data *> read;
      /// Write datas
      std::set<Data *> write;

      /// Datas flowing in this kernel  (earlier values might be used)
      std::set<Data *> flow_in;
      /// Datas flowing out this kernel (produced values might be consumme later)
      std::set<Data *> flow_out;
      /// Only used by this kernel (still need an allocation)
      std::set<Data *> local;
    };

    struct arguments_t {
      /// Parameters : reference order for call argument
      std::list<SgVariableSymbol *> parameters; 
      /// Coefficients : reference order for call argument
      std::list<SgVariableSymbol *> coefficients;
      /// Datas : reference order for call argument
      std::list<Data *> datas;
    };

  protected:
    /// Root loop-tree
    LoopTrees<>::node_t * p_root;

    /// Set of data sorted accordingly to how they flow through the kernel
    dataflow_t p_data_flow;

    /// ordered symbol lists
    arguments_t p_argument_order;

    /// Set of possible loop distributions
    std::set<loop_mapping_t *> p_loop_mappings;

  protected:
    Kernel(LoopTrees::node_t * root);

  public:
    virtual ~Kernel();

    void setRoot(LoopTrees::node_t * root);
    LoopTrees::node_t * getRoot() const;

    dataflow_t & getDataflow();
    const dataflow_t & getDataflow() const;

    arguments_t & getArguments();
    const arguments_t & getArguments() const;

    std::set<loop_mapping_t *> & getLoopMappings();
    const std::set<loop_mapping_t *> & getLoopMappings() const;

  private:
    static unsigned long id_cnt;
};

void collectReferencedSymbols(Kernel * kernel, std::set<SgVariableSymbol *> & symbols);

/** @} */

}

#endif /* __KLT_KERNEL_HPP__ */

