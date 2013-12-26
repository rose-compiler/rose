
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
      /// Context : used to communicate information (loop shape, ...) from the host to the device
      SgVariableSymbol * context;
    };

    struct local_symbol_maps_t {
      std::map<SgVariableSymbol *, SgVariableSymbol *> parameters;
      std::map<SgVariableSymbol *, SgVariableSymbol *> scalars;
      std::map<Data<Annotation> *, SgVariableSymbol *> datas;
      std::map<SgVariableSymbol *, SgVariableSymbol *> iterators;
      SgVariableSymbol * context;
    };

    struct a_kernel {
      std::string kernel_name;
      /// \todo
    };

  protected:
    /// List of trees forming the kernel (can be loops or statements)
    std::list<typename LoopTrees<Annotation>::node_t *> p_looptree_roots;

    /// Set of data sorted accordingly to how they flow through the kernel
    dataflow_t p_data_flow;

    /// ordered symbol lists
    arguments_t p_arguments;

    /// Shapes (runtime dependent) associated to each loop in the kernel
    std::map<typename LoopTrees<Annotation>::loop_t *, typename Runtime::loop_shape_t *> p_loop_shapes;

    /// All actual kernels that have been generated for this kernel (different decisions made in shape interpretation)
    std::vector<a_kernel *> p_generated_kernels;

  public:
    Kernel() :
      id(id_cnt++),
      p_looptree_roots(),
      p_data_flow(),
      p_arguments(),
      p_loop_shapes(),
      p_generated_kernels()
    {}
      
    ~Kernel() {
      /// \todo
    }

    std::list<typename LoopTrees<Annotation>::node_t *> & getRoots() { return p_looptree_roots; }
    const std::list<typename LoopTrees<Annotation>::node_t *> & getRoots() const { return p_looptree_roots; }

    dataflow_t & getDataflow() { return p_data_flow; }
    const dataflow_t & getDataflow() const { return p_data_flow; }

    arguments_t & getArguments() { return p_arguments; }
    const arguments_t & getArguments() const { return p_arguments; }

    void setShape(typename LoopTrees<Annotation>::loop_t * loop, typename Runtime::loop_shape_t * shape) {
      p_loop_shapes.insert(std::pair<typename LoopTrees<Annotation>::loop_t *, typename Runtime::loop_shape_t *>(loop, shape));
    }

    const std::map<typename LoopTrees<Annotation>::loop_t *, typename Runtime::loop_shape_t *> & getShapes() const { return p_loop_shapes; }

    void addKernel(a_kernel * kernel) { p_generated_kernels.push_back(kernel); }

  private:
    static unsigned long id_cnt;
};

template <class Annotation, class Language, class Runtime>
void collectReferencedSymbols(Kernel<Annotation, Language, Runtime> * kernel, std::set<SgVariableSymbol *> & symbols) {
  const std::list<typename LoopTrees<Annotation>::node_t *> & roots = kernel->getRoots();
  typename std::list<typename LoopTrees<Annotation>::node_t *>::const_iterator it_root;
  for (it_root = roots.begin(); it_root != roots.end(); it_root++)
    collectReferencedSymbols<Annotation>(*it_root, symbols);

  assert(!kernel->getDataflow().datas.empty()); // Should always have some data
  collectReferencedSymbols<Annotation>(kernel->getDataflow().datas, symbols);
}

/** Generate a parameter list for a kernel
 */
template <class Annotation, class Language, class Runtime>
SgFunctionParameterList * createParameterList(Kernel<Annotation, Language, Runtime> * kernel);

/** @} */

}

#endif /* __KLT_KERNEL_HPP__ */

