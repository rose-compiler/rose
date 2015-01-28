
#ifndef __KLT_KERNEL_HPP__
#define __KLT_KERNEL_HPP__

#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/loop-tiler.hpp"

#include <vector>
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
      /// Private Datas : reference order for call argument
      std::list<Data<Annotation> *> privates;
      /// Context : used to communicate information (loop shape, ...) from the host to the device
      SgVariableSymbol * context;
    };

    struct local_symbol_maps_t {
      std::map<SgVariableSymbol *, SgVariableSymbol *> parameters;
      std::map<SgVariableSymbol *, SgVariableSymbol *> scalars;
      std::map<Data<Annotation> *, SgVariableSymbol *> datas;
      std::map<Data<Annotation> *, SgVariableSymbol *> privates;
      std::map<Data<Annotation> *, SgVariableSymbol *> data_offsets;
      std::map<SgVariableSymbol *, SgVariableSymbol *> iterators;
      SgVariableSymbol * context;
    };

    struct a_kernel {
      std::string kernel_name;

      typename Runtime::exec_config_t config;

      std::vector<typename Runtime::a_loop> loops;
    };

  protected:
    const LoopTrees<Annotation> & p_loop_tree;

    /// List of trees forming the kernel (can be loops or statements)
    std::list<typename LoopTrees<Annotation>::node_t *> p_looptree_roots;

    /// All loops in text order
    std::vector<typename LoopTrees<Annotation>::loop_t *> p_loops;

    /// All nodes in text order
    std::vector<typename LoopTrees<Annotation>::node_t *> p_nodes;

    /// Set of data sorted accordingly to how they flow through the kernel
    dataflow_t p_data_flow;

    /// ordered symbol lists
    arguments_t p_arguments;

    /// All actual kernels that have been generated for this kernel (different decisions made in shape interpretation)
    std::vector<a_kernel *> p_generated_kernels;

    void registerLoopsAndNodes(typename LoopTrees<Annotation>::node_t * node) {
      assert(node != NULL);

      p_nodes.push_back(node);

      typename ::KLT::LoopTrees<Annotation>::loop_t  * loop  = dynamic_cast<typename ::KLT::LoopTrees<Annotation>::loop_t  *>(node);
      typename ::KLT::LoopTrees<Annotation>::cond_t  * cond  = dynamic_cast<typename ::KLT::LoopTrees<Annotation>::cond_t  *>(node);
      typename ::KLT::LoopTrees<Annotation>::block_t * block = dynamic_cast<typename ::KLT::LoopTrees<Annotation>::block_t *>(node);
      typename ::KLT::LoopTrees<Annotation>::stmt_t  * stmt  = dynamic_cast<typename ::KLT::LoopTrees<Annotation>::stmt_t  *>(node);

      if (loop != NULL) {
        p_loops.push_back(loop);
        registerLoopsAndNodes(loop->block);
      }
      else if (cond != NULL) {
        if (cond->block_true != NULL)
          registerLoopsAndNodes(cond->block_true);
        if (cond->block_false != NULL)
          registerLoopsAndNodes(cond->block_false);
      }
      else if (block != NULL) {
        typename std::vector<typename LoopTrees<Annotation>::node_t * >::const_iterator it_child;
        for (it_child = block->children.begin(); it_child != block->children.end(); it_child++)
            registerLoopsAndNodes(*it_child);
      }
      else assert(stmt != NULL);
    }

  public:
    Kernel(const LoopTrees<Annotation> & loop_tree) :
      id(id_cnt++),
      p_loop_tree(loop_tree),
      p_looptree_roots(),
      p_loops(),
      p_nodes(),
      p_data_flow(),
      p_arguments(),
      p_generated_kernels()
    {}
      
    ~Kernel() {
      /// \todo
    }

    void appendRoot(typename LoopTrees<Annotation>::node_t * node) {
      p_looptree_roots.push_back(node);
      registerLoopsAndNodes(node);
    }
    const std::list<typename LoopTrees<Annotation>::node_t *> & getRoots() const { return p_looptree_roots; }

    dataflow_t & getDataflow() { return p_data_flow; }
    const dataflow_t & getDataflow() const { return p_data_flow; }

    arguments_t & getArguments() { return p_arguments; }
    const arguments_t & getArguments() const { return p_arguments; }

    void addKernel(a_kernel * kernel) { p_generated_kernels.push_back(kernel); }
    const std::vector<a_kernel *> & getKernels() const { return p_generated_kernels; }

    const LoopTrees<Annotation> & getLoopTree() const { return p_loop_tree; }

    const std::vector<typename LoopTrees<Annotation>::loop_t *> & getLoops() const { return p_loops; }

    const std::vector<typename LoopTrees<Annotation>::node_t *> & getNodes() const { return p_nodes; }

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
  const typename Kernel<Annotation, Language, Runtime>::local_symbol_maps_t & local_symbol_maps
);

template <class Annotation, class Language, class Runtime>
std::pair<SgStatement *, std::vector<SgScopeStatement *> > generateLoops(
  typename LoopTrees<Annotation>::loop_t * loop,
  size_t & loop_cnt,
  size_t & tile_cnt,
  std::map<typename ::KLT::LoopTrees<Annotation>::loop_t *, typename Runtime::a_loop> & loop_descriptors_map,
  typename ::KLT::LoopTiler<Annotation, Language, Runtime>::loop_tiling_t & tiling,
  const typename Kernel<Annotation, Language, Runtime>::local_symbol_maps_t & local_symbol_maps
);

template <class Annotation, class Language, class Runtime>
typename Runtime::exec_mode_t changeExecutionMode(
  const typename Runtime::exec_mode_t & exec_mode,
  const typename Runtime::exec_config_t & exec_cfg,
  typename ::KLT::LoopTiler<Annotation, Language, Runtime>::loop_tiling_t & tiling
);

template <class Annotation, class Language, class Runtime>
void generateSynchronizations(
  typename Runtime::exec_mode_t prev_exec_mode,
  typename Runtime::exec_mode_t next_exec_mode,
  const typename Runtime::exec_config_t & exec_cfg,
  SgScopeStatement * scope,
  const typename ::KLT::Kernel<Annotation, Language, Runtime>::local_symbol_maps_t & local_symbol_maps
);

template <class Annotation, class Language, class Runtime>
SgScopeStatement * generateExecModeGuards(
  typename Runtime::exec_mode_t exec_mode,
  const typename Runtime::exec_config_t & exec_cfg,
  SgScopeStatement * scope,
  const typename ::KLT::Kernel<Annotation, Language, Runtime>::local_symbol_maps_t & local_symbol_maps
);

template <class Annotation, class Language, class Runtime>
void generateKernelBody(
  typename ::KLT::LoopTrees<Annotation>::node_t * node,
  size_t & loop_cnt,
  size_t & tile_cnt,
  std::map<typename ::KLT::LoopTrees<Annotation>::loop_t *, typename Runtime::a_loop> & loop_descriptors_map,
  typename Runtime::exec_mode_t exec_mode,
  const typename Runtime::exec_config_t & exec_cfg,
  const std::map<
    typename ::KLT::LoopTrees<Annotation>::loop_t *,
    typename ::KLT::LoopTiler<Annotation, Language, Runtime>::loop_tiling_t *
  > & tilings,
  const typename ::KLT::Kernel<Annotation, Language, Runtime>::local_symbol_maps_t & local_symbol_maps,
  SgScopeStatement * scope
) {
  assert(node != NULL);

  typename ::KLT::LoopTrees<Annotation>::loop_t  * loop  = dynamic_cast<typename ::KLT::LoopTrees<Annotation>::loop_t  *>(node);
  typename ::KLT::LoopTrees<Annotation>::cond_t  * cond  = dynamic_cast<typename ::KLT::LoopTrees<Annotation>::cond_t  *>(node);
  typename ::KLT::LoopTrees<Annotation>::block_t * block = dynamic_cast<typename ::KLT::LoopTrees<Annotation>::block_t *>(node);
  typename ::KLT::LoopTrees<Annotation>::stmt_t  * stmt  = dynamic_cast<typename ::KLT::LoopTrees<Annotation>::stmt_t  *>(node);

  if (loop != NULL) {
//  std::cout << "[generateKernelBody]  loop != NULL" << std::endl;
    typename ::KLT::LoopTiler<Annotation, Language, Runtime>::loop_tiling_t * tiling = NULL;
    typename std::map<
      typename ::KLT::LoopTrees<Annotation>::loop_t *,
      typename ::KLT::LoopTiler<Annotation, Language, Runtime>::loop_tiling_t *
    >::const_iterator it_tiling = tilings.find(loop);
    if (it_tiling != tilings.end()) {
      assert(it_tiling->second != NULL);
      tiling = it_tiling->second;
    }

    std::pair<SgStatement *, std::vector<SgScopeStatement *> > sg_loop = generateLoops<Annotation, Language, Runtime>(
      loop, loop_cnt, tile_cnt, loop_descriptors_map, *tiling, local_symbol_maps
    );
    SageInterface::appendStatement(sg_loop.first, scope);

    typename Runtime::exec_mode_t next_exec_mode = changeExecutionMode<Annotation, Language, Runtime>(exec_mode, exec_cfg, *tiling);

    std::vector<SgScopeStatement *>::const_iterator it_scope;
    for (it_scope = sg_loop.second.begin(); it_scope != sg_loop.second.end(); it_scope++) {

      generateKernelBody<Annotation, Language, Runtime>(
        loop->block, loop_cnt, tile_cnt, loop_descriptors_map, next_exec_mode, exec_cfg, tilings, local_symbol_maps, *it_scope
      );
    }

    generateSynchronizations<Annotation, Language, Runtime>(next_exec_mode, exec_mode, exec_cfg, scope, local_symbol_maps);
  }
  else if (cond != NULL) {
//  std::cout << "[generateKernelBody]  cond != NULL" << std::endl;

    /// \todo translation of 'cond->condition'
    SgExprStatement * cond_stmt = SageBuilder::buildExprStatement(cond->condition);
    SgBasicBlock * bb_true = SageBuilder::buildBasicBlock();
    SgBasicBlock * bb_false = SageBuilder::buildBasicBlock();

    SgIfStmt * if_stmt = SageBuilder::buildIfStmt(cond_stmt, bb_true, bb_false);
    SageInterface::appendStatement(if_stmt, scope);

    if (cond->block_true != NULL)
      generateKernelBody<Annotation, Language, Runtime>(
        cond->block_true, loop_cnt, tile_cnt, loop_descriptors_map, exec_mode, exec_cfg, tilings, local_symbol_maps, bb_true
      );
    if (cond->block_false != NULL)
      generateKernelBody<Annotation, Language, Runtime>(
        cond->block_false, loop_cnt, tile_cnt, loop_descriptors_map, exec_mode, exec_cfg, tilings, local_symbol_maps, bb_false
      );
  }
  else if (block != NULL) {
//  std::cout << "[generateKernelBody]  block != NULL" << std::endl;

    SgScopeStatement * bb_scope = scope;
    if (block->children.size() > 1) {
      bb_scope = SageBuilder::buildBasicBlock();
      SageInterface::appendStatement(bb_scope, scope);
    }

    typename std::vector<typename LoopTrees<Annotation>::node_t * >::const_iterator it_child;
    for (it_child = block->children.begin(); it_child != block->children.end(); it_child++)
      generateKernelBody<Annotation, Language, Runtime>(
        *it_child, loop_cnt, tile_cnt, loop_descriptors_map, exec_mode, exec_cfg, tilings, local_symbol_maps, bb_scope
      );
  }
  else if (stmt != NULL) {
//  std::cout << "[generateKernelBody]  stmt != NULL" << std::endl;
    SgStatement * sg_stmt = generateStatement<Annotation, Language, Runtime>(stmt, local_symbol_maps);

    SgScopeStatement * bb_scope = generateExecModeGuards<Annotation, Language, Runtime>(exec_mode, exec_cfg, scope, local_symbol_maps);

    SageInterface::appendStatement(sg_stmt, bb_scope);
  }
  else assert(false);
}

/** @} */

}

#endif /* __KLT_KERNEL_HPP__ */

