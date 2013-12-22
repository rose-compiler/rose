
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

template <class Annotation, class Language, class Runtime>
class Kernel {
  public:
    const unsigned long id;

  public:
    struct loop_mapping_t {
      /// the loop nest to be distributed over threads/work-item/processor/...
      std::list<typename LoopTrees<Annotation>::loop_t *> loop_nest;
      /// remaining statement to be put in the body of the kernel (inside the loop nest)
      std::list<typename LoopTrees<Annotation>::node_t *> body;
    };

    struct dataflow_t {
      /// All datas
      std::set<Data<Annotation> *> datas;

      /// Read datas
      std::set<Data<Annotation> *> read;
      /// Write datas
      std::set<Data<Annotation> *> write;

      /// Datas flowing in this kernel  (earlier values might be used)
      std::set<Data<Annotation> *> flow_in;
      /// Datas flowing out this kernel (produced values might be consumme later)
      std::set<Data<Annotation> *> flow_out;
      /// Only used by this kernel (still need an allocation)
      std::set<Data<Annotation> *> local;
    };

    struct arguments_t {
      /// Parameters : reference order for call argument
      std::list<SgVariableSymbol *> parameters; 
      /// Coefficients : reference order for call argument
      std::list<SgVariableSymbol *> coefficients;
      /// Datas : reference order for call argument
      std::list<Data<Annotation> *> datas;
    };

    struct local_symbol_maps_t {
      std::map<SgVariableSymbol *, SgVariableSymbol *> scalars;
      std::map<Data<Annotation> *, SgVariableSymbol *> datas;
      std::map<SgVariableSymbol *, SgVariableSymbol *> iterators;
      SgVariableSymbol * context;
    };

    struct a_kernel {
      /// \todo
    };

  protected:
    /// Root loop-tree
    typename LoopTrees<Annotation>::node_t * p_root;

    /// Set of data sorted accordingly to how they flow through the kernel
    dataflow_t p_data_flow;

    /// ordered symbol lists
    arguments_t p_argument_order;

    /// Set of possible loop distributions
    std::set<loop_mapping_t *> p_loop_mappings;

    /// A set of iteration mapping for each loop distribution
    std::map<loop_mapping_t *, std::set<IterationMap<Annotation, Language, Runtime> *> > p_iteration_maps;

    /// A map between the different iteration mapping and the produced kernel for each loop distribution
    std::map<loop_mapping_t *, std::map<IterationMap<Annotation, Language, Runtime> *, a_kernel *> > p_kernel_map;

  public:
    Kernel(typename LoopTrees<Annotation>::node_t * root);
    virtual ~Kernel();

    void setRoot(typename LoopTrees<Annotation>::node_t * root);
    typename LoopTrees<Annotation>::node_t * getRoot() const;

    dataflow_t & getDataflow();
    const dataflow_t & getDataflow() const;

    arguments_t & getArguments();
    const arguments_t & getArguments() const;

    std::set<loop_mapping_t *> & getLoopMappings();
    const std::set<loop_mapping_t *> & getLoopMappings() const;

    std::set<IterationMap<Annotation, Language, Runtime> *> & getIterationMaps(loop_mapping_t * loop_mapping);
    const std::set<IterationMap<Annotation, Language, Runtime> *> & getIterationMaps(loop_mapping_t * loop_mapping) const;

    void setKernel(loop_mapping_t * loop_mapping, IterationMap<Annotation, Language, Runtime> * iteration_map, a_kernel * kernel);
    a_kernel * getKernel(loop_mapping_t * loop_mapping, IterationMap<Annotation, Language, Runtime> * iteration_map) const;

  private:
    static unsigned long id_cnt;
};

template <class Annotation, class Language, class Runtime>
void collectReferencedSymbols(Kernel<Annotation, Language, Runtime> * kernel, std::set<SgVariableSymbol *> & symbols);

/** @} */

}

#endif /* __KLT_KERNEL_HPP__ */

