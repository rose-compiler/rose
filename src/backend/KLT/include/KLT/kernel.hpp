
#ifndef __KLT_KERNEL_HPP__
#define __KLT_KERNEL_HPP__

#include "KLT/loop-trees.hpp"

#include <vector>
#include <list>
#include <set>

#include "sage3basic.h"

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
    
    unsigned num_loops;

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

      std::vector<typename Runtime::a_loop> loops;
    };

  protected:
    /// List of trees forming the kernel (can be loops or statements)
    std::list<typename LoopTrees<Annotation>::node_t *> p_looptree_roots;

    /// Set of data sorted accordingly to how they flow through the kernel
    dataflow_t p_data_flow;

    /// ordered symbol lists
    arguments_t p_arguments;

    /// All actual kernels that have been generated for this kernel (different decisions made in shape interpretation)
    std::vector<a_kernel *> p_generated_kernels;

  public:
    Kernel() :
      id(id_cnt++),
      num_loops(0),
      p_looptree_roots(),
      p_data_flow(),
      p_arguments(),
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

    void addKernel(a_kernel * kernel) { p_generated_kernels.push_back(kernel); }
    const std::vector<a_kernel *> & getKernels() const { return p_generated_kernels; }

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

template <class Annotation, class Language, class Runtime>
SgStatement * generateStatement(
  typename LoopTrees<Annotation>::stmt_t * stmt,
  const typename Kernel<Annotation, Language, Runtime>::local_symbol_maps_t & local_symbol_maps,
  bool flatten_array_ref
);

template <class Annotation, class Language, class Runtime>
std::pair<SgStatement *, SgScopeStatement *> generateLoops(
  typename LoopTrees<Annotation>::loop_t * loop,
  unsigned & loop_id,
  std::vector<typename Runtime::a_loop> & loop_descriptors,
  typename Runtime::loop_shape_t * shape,
  const typename Kernel<Annotation, Language, Runtime>::local_symbol_maps_t & local_symbol_maps
);

template <class Annotation, class Language, class Runtime>
void generateKernelBody(
  typename ::KLT::LoopTrees<Annotation>::node_t * node,
  unsigned & loop_id,
  std::vector<typename Runtime::a_loop> & loop_descriptors,
  typename Runtime::exec_mode_e exec_mode,
  std::map<typename ::KLT::LoopTrees<Annotation>::loop_t *, typename Runtime::loop_shape_t *> shapes,
  const typename ::KLT::Kernel<Annotation, Language, Runtime>::local_symbol_maps_t & local_symbol_maps,
  SgScopeStatement * scope
) {
  typename ::KLT::LoopTrees<Annotation>::loop_t * loop = dynamic_cast<typename ::KLT::LoopTrees<Annotation>::loop_t *>(node);
  typename ::KLT::LoopTrees<Annotation>::stmt_t * stmt = dynamic_cast<typename ::KLT::LoopTrees<Annotation>::stmt_t *>(node);
  if (loop != NULL) {
    typename Runtime::loop_shape_t * shape = NULL;
    typename std::map<typename ::KLT::LoopTrees<Annotation>::loop_t *, typename Runtime::loop_shape_t *>::const_iterator it_shape = shapes.find(loop);
    if (it_shape != shapes.end()) shape = it_shape->second;

    std::pair<SgStatement *, SgScopeStatement *> sg_loop = generateLoops<Annotation, Language, Runtime>(
      loop, loop_id, loop_descriptors, shape, local_symbol_maps
    );
    SageInterface::appendStatement(sg_loop.first, scope);

    /// \todo change the execution mode if needed

    scope = sg_loop.second;

    typename std::list<typename LoopTrees<Annotation>::node_t * >::const_iterator it_child;
    for (it_child = loop->children.begin(); it_child != loop->children.end(); it_child++)
      generateKernelBody<Annotation, Language, Runtime>(
        *it_child, loop_id, loop_descriptors, exec_mode, shapes, local_symbol_maps, scope
      );
  }
  else if (stmt != NULL) {
    SgStatement * sg_stmt = generateStatement<Annotation, Language, Runtime>(stmt, local_symbol_maps, true);

    /// \todo guard execution function of the curret Execution Mode

    SageInterface::appendStatement(sg_stmt, scope);
  }
  else assert(false);
}

/** @} */

}

#endif /* __KLT_KERNEL_HPP__ */

