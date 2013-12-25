
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
      /// the loop nests to be distributed over threads/work-item/processor/...
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
      std::list<SgVariableSymbol *> scalars;
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
    Kernel(typename LoopTrees<Annotation>::node_t * root) :
      id(id_cnt++),
      p_root(root),
      p_data_flow(),
      p_argument_order(),
      p_loop_mappings(),
      p_iteration_maps(),
      p_kernel_map()
    {}
      
    ~Kernel() {
      /// \todo
    }

    void setRoot(typename LoopTrees<Annotation>::node_t * root) { p_root = root; }
    typename LoopTrees<Annotation>::node_t * getRoot() const { return p_root; }

    dataflow_t & getDataflow() { return p_data_flow; }
    const dataflow_t & getDataflow() const { return p_data_flow; }

    arguments_t & getArguments() { return p_argument_order; }
    const arguments_t & getArguments() const { return p_argument_order; }

    std::set<loop_mapping_t *> & getLoopMappings() { return p_loop_mappings; }
    const std::set<loop_mapping_t *> & getLoopMappings() const  { return p_loop_mappings; }

    std::set<IterationMap<Annotation, Language, Runtime> *> & getIterationMaps(loop_mapping_t * loop_mapping) {
      typename std::map<loop_mapping_t *, std::set<IterationMap<Annotation, Language, Runtime> *> >::iterator it_map = p_iteration_maps.find(loop_mapping);
      if (it_map == p_iteration_maps.end()) {
        it_map = p_iteration_maps.insert(std::pair<loop_mapping_t *, std::set<IterationMap<Annotation, Language, Runtime> *> >(
                     loop_mapping, std::set<IterationMap<Annotation, Language, Runtime> *>()
                 )).first;
      }
      return it_map->second;
    }

    const std::set<IterationMap<Annotation, Language, Runtime> *> & getIterationMaps(loop_mapping_t * loop_mapping) const {
      typename std::map<loop_mapping_t *, std::set<IterationMap<Annotation, Language, Runtime> *> >::const_iterator it_map = p_iteration_maps.find(loop_mapping);
      if (it_map == p_iteration_maps.end()) {
        it_map = p_iteration_maps.insert(std::pair<loop_mapping_t *, std::set<IterationMap<Annotation, Language, Runtime> *> >(
                     loop_mapping, std::set<IterationMap<Annotation, Language, Runtime> *>()
                 )).first;
      }
      return it_map->second;
    }

    void setKernel(loop_mapping_t * loop_mapping, IterationMap<Annotation, Language, Runtime> * iteration_map, a_kernel * kernel) {
      typename std::map<loop_mapping_t *, std::map<IterationMap<Annotation, Language, Runtime> *, a_kernel *> >::iterator it1 = p_kernel_map.find(loop_mapping);
      if (it1 == p_kernel_map.end()) {
        it1 = p_kernel_map.insert(std::pair<loop_mapping_t *, std::map<IterationMap<Annotation, Language, Runtime> *, a_kernel *> >(
                     loop_mapping, std::map<IterationMap<Annotation, Language, Runtime> *, a_kernel *>()
                 )).first;
      }
      typename std::map<IterationMap<Annotation, Language, Runtime> *, a_kernel *>::iterator it2 = it1->second.find(iteration_map);
      assert(it2 == it1->second.end());
      it1->second.insert(std::pair<IterationMap<Annotation, Language, Runtime> *, a_kernel *>(iteration_map, kernel));
    }

    a_kernel * getKernel(loop_mapping_t * loop_mapping, IterationMap<Annotation, Language, Runtime> * iteration_map) const {
      typename std::map<loop_mapping_t *, std::map<IterationMap<Annotation, Language, Runtime> *, a_kernel *> >::iterator it1 = p_kernel_map.find(loop_mapping);
      if (it1 == p_kernel_map.end()) {
        it1 = p_kernel_map.insert(std::pair<loop_mapping_t *, std::map<IterationMap<Annotation, Language, Runtime> *, a_kernel *> >(
                     loop_mapping, std::map<IterationMap<Annotation, Language, Runtime> *, a_kernel *>()
                 )).first;
      }
      typename std::map<IterationMap<Annotation, Language, Runtime> *, a_kernel *>::iterator it2 = it1->second.find(iteration_map);
      assert(it2 != it1->second.end());
      return it2->second;
    }

  private:
    static unsigned long id_cnt;
};

template <class Annotation, class Language, class Runtime>
void collectReferencedSymbols(Kernel<Annotation, Language, Runtime> * kernel, std::set<SgVariableSymbol *> & symbols) {
  assert(kernel->getRoot() != NULL); // Root should have been setup
  collectReferencedSymbols<Annotation>(kernel->getRoot(), symbols);

  assert(!kernel->getDataflow().datas.empty()); // Should always have some data
  collectReferencedSymbols<Annotation>(kernel->getDataflow().datas, symbols);
}

/** @} */

}

#endif /* __KLT_KERNEL_HPP__ */

