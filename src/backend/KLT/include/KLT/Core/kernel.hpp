
#ifndef __KLT_KERNEL_HPP__
#define __KLT_KERNEL_HPP__

#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/loop-tiler.hpp"
#include "KLT/Core/data.hpp"

#include "MFB/utils.hpp"

#include <vector>
#include <list>
#include <set>

class SgVariableSymbol;

namespace KLT {

/*!
 * \addtogroup grp_klt_kernel
 * @{
*/

template <class Annotation, class Runtime>
class Kernel {
  public:
    typedef Annotation Annotation_;
    typedef Runtime Runtime_;

    typedef typename LoopTrees<Annotation>::node_t node_t;
    typedef typename LoopTrees<Annotation>::loop_t loop_t;
    typedef typename LoopTrees<Annotation>::tile_t tile_t;
    typedef typename LoopTrees<Annotation>::block_t block_t;
    typedef typename LoopTrees<Annotation>::cond_t cond_t;
    typedef typename LoopTrees<Annotation>::stmt_t stmt_t;

    const size_t id;

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

    struct kernel_desc_t {
      static size_t id_cnt;
      size_t id;
      std::string kernel_name;

      std::vector<typename Runtime::loop_desc_t *> loops;
      std::vector<typename Runtime::tile_desc_t *> tiles;

      kernel_desc_t(const std::string & kernel_name_) : 
        id(id_cnt++), kernel_name(kernel_name_), loops(), tiles()
      {}
    };

    struct runtime_specific_t {};

  protected:
    const LoopTrees<Annotation> & p_loop_tree;

    /// List of trees forming the kernel (can be loops or statements)
    std::list<node_t *> p_looptree_roots;

    /// All loops in text order
    std::vector<loop_t *> p_loops;

    /// All nodes in text order
    std::vector<node_t *> p_nodes;

    /// Set of data sorted accordingly to how they flow through the kernel
    dataflow_t p_data_flow;

    /// ordered symbol lists
    arguments_t p_arguments;

    /// All actual kernels that have been generated for this kernel (different decisions made in shape interpretation)
    std::vector<kernel_desc_t *> p_generated_kernels;

    void registerLoopsAndNodes(node_t * node);

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

    void appendRoot(node_t * node) {
      p_looptree_roots.push_back(node);
      registerLoopsAndNodes(node);
    }
    const std::list<node_t *> & getRoots() const { return p_looptree_roots; }

    dataflow_t & getDataflow() { return p_data_flow; }
    const dataflow_t & getDataflow() const { return p_data_flow; }

    arguments_t & getArguments() { return p_arguments; }
    const arguments_t & getArguments() const { return p_arguments; }

    void addKernel(kernel_desc_t * kernel) { p_generated_kernels.push_back(kernel); }
    const std::vector<kernel_desc_t *> & getKernels() const { return p_generated_kernels; }

    const LoopTrees<Annotation> & getLoopTree() const { return p_loop_tree; }

    const std::vector<loop_t *> & getLoops() const { return p_loops; }

    const std::vector<node_t *> & getNodes() const { return p_nodes; }

    SgFunctionParameterList * createParameterList() const;

    void collectReferencedSymbols(std::set<SgVariableSymbol *> & symbols) const;

    void setRuntimeSpecificKernelField(SgVariableSymbol * kernel_sym, SgBasicBlock * bb) const;

  private:
    static size_t id_cnt;
};

template <class Annotation, class Runtime>
void Kernel<Annotation, Runtime>::registerLoopsAndNodes(node_t * node) {
  assert(node != NULL);

  p_nodes.push_back(node);

  loop_t  * loop  = dynamic_cast<loop_t  *>(node);
  cond_t  * cond  = dynamic_cast<cond_t  *>(node);
  block_t * block = dynamic_cast<block_t *>(node);
  stmt_t  * stmt  = dynamic_cast<stmt_t  *>(node);

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
    typename std::vector<node_t * >::const_iterator it_child;
    for (it_child = block->children.begin(); it_child != block->children.end(); it_child++)
      registerLoopsAndNodes(*it_child);
  }
  else assert(stmt != NULL);
}

template <class Annotation, class Runtime>
void Kernel<Annotation, Runtime>::collectReferencedSymbols(std::set<SgVariableSymbol *> & symbols) const {
  typename std::list<node_t *>::const_iterator it_root;
  for (it_root = p_looptree_roots.begin(); it_root != p_looptree_roots.end(); it_root++)
    (*it_root)->collectReferencedSymbols(symbols, true);

  if (!p_data_flow.datas.empty())
    ::KLT::collectReferencedSymbols<Annotation>(p_data_flow.datas, symbols);
}

template <class Annotation, class Runtime>
SgStatement * generateStatement(
  typename LoopTrees<Annotation>::stmt_t * stmt,
  const typename Kernel<Annotation, Runtime>::local_symbol_maps_t & local_symbol_maps
) {
  SgStatement * result = SageInterface::copyStatement(stmt->statement);

  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_sym_to_local;
  typename std::map<Data<Annotation> *, SgVariableSymbol *>::const_iterator it_data_to_local;

  std::map<SgVariableSymbol *, SgVariableSymbol *> data_sym_to_local;
  std::map<SgVariableSymbol *, Data<Annotation> *> data_sym_to_data;

  for (it_data_to_local = local_symbol_maps.datas.begin(); it_data_to_local != local_symbol_maps.datas.end(); it_data_to_local++) {
    Data<Annotation> * data = it_data_to_local->first;
    SgVariableSymbol * data_sym = it_data_to_local->first->getVariableSymbol();
    SgVariableSymbol * local_sym = it_data_to_local->second;

    data_sym_to_local.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(data_sym, local_sym));
    data_sym_to_data.insert(std::pair<SgVariableSymbol *, Data<Annotation> *>(data_sym, data));
  }

  std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(result);
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;

  for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
    SgVarRefExp * var_ref = *it_var_ref;
    SgVariableSymbol * var_sym = var_ref->get_symbol();

    typename std::map<SgVariableSymbol *, Data<Annotation> *>::const_iterator it_data_sym_to_data = data_sym_to_data.find(var_sym);
    if (it_data_sym_to_data == data_sym_to_data.end()) continue; // Not a variable reference to a Data

    Data<Annotation> * data = it_data_sym_to_data->second;

    SgPntrArrRefExp * arr_ref = isSgPntrArrRefExp(var_ref->get_parent());
    SgPntrArrRefExp * top_arr_ref = NULL;
    std::vector<SgExpression *> subscripts;
    while (arr_ref != NULL) {
      top_arr_ref = arr_ref;
      subscripts.push_back(arr_ref->get_rhs_operand_i());
      arr_ref = isSgPntrArrRefExp(arr_ref->get_parent());
    }
    assert(subscripts.size() == data->getSections().size());

    if (data->getSections().size() > 1) {
      assert(top_arr_ref != NULL);
      std::vector<SgExpression *>::const_iterator it_subscript = subscripts.begin();
      SgExpression * subscript = SageInterface::copyExpression(*it_subscript);
      it_subscript++;
      size_t cnt = 1;
      while (it_subscript != subscripts.end()) {
        SgExpression * dim_size = SageInterface::copyExpression(data->getSections()[cnt].size);
        subscript = SageBuilder::buildMultiplyOp(subscript, dim_size);
        subscript = SageBuilder::buildAddOp(subscript, SageInterface::copyExpression(*it_subscript));
        cnt++; it_subscript++;
      }
      SageInterface::replaceExpression(top_arr_ref, SageBuilder::buildPntrArrRefExp(SageInterface::copyExpression(var_ref), subscript), true);
    }
  }

  var_refs = SageInterface::querySubTree<SgVarRefExp>(result);
  for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
    SgVarRefExp * var_ref = *it_var_ref;
    SgVariableSymbol * var_sym = var_ref->get_symbol();

    SgVariableSymbol * local_sym = NULL;
    it_sym_to_local = local_symbol_maps.parameters.find(var_sym);
    if (it_sym_to_local != local_symbol_maps.parameters.end())
      local_sym = it_sym_to_local->second;

    it_sym_to_local = local_symbol_maps.scalars.find(var_sym);
    if (it_sym_to_local != local_symbol_maps.scalars.end()) {
      assert(local_sym == NULL);

      local_sym = it_sym_to_local->second;
    }

    it_sym_to_local = data_sym_to_local.find(var_sym);
    if (it_sym_to_local != data_sym_to_local.end()) {
      assert(local_sym == NULL);

      local_sym = it_sym_to_local->second;
    }

    it_sym_to_local = local_symbol_maps.iterators.find(var_sym);
    if (it_sym_to_local != local_symbol_maps.iterators.end()) {
      assert(local_sym == NULL);

      local_sym = it_sym_to_local->second;
    }

    if (local_sym != NULL)
      SageInterface::replaceExpression(var_ref, SageBuilder::buildVarRefExp(local_sym), true);
    else {
      /// \todo valid if sym can be found in local scope
    }
  }

  assert(result != NULL);

  return result;
}

template <class Annotation, class Runtime>
std::pair<SgStatement *, SgScopeStatement *> generateLoops(
  typename LoopTrees<Annotation>::loop_t * loop,
  const typename Kernel<Annotation, Runtime>::local_symbol_maps_t & local_symbol_maps
) {
  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_sym_to_local = local_symbol_maps.iterators.find(loop->iterator);
  assert(it_sym_to_local != local_symbol_maps.iterators.end());

  SgExprStatement * init_stmt = SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(
                                  SageBuilder::buildVarRefExp(it_sym_to_local->second),
                                  MFB::Utils::translateConstExpression(loop->lower_bound, local_symbol_maps.iterators, local_symbol_maps.parameters)
                                ));
  SgExprStatement * test_stmt  = SageBuilder::buildExprStatement(SageBuilder::buildLessOrEqualOp(
                                   SageBuilder::buildVarRefExp(it_sym_to_local->second),
                                   MFB::Utils::translateConstExpression(loop->upper_bound, local_symbol_maps.iterators, local_symbol_maps.parameters))
                                 );
  SgExpression * inc_expr = SageBuilder::buildPlusAssignOp(
                              SageBuilder::buildVarRefExp(it_sym_to_local->second),
                              MFB::Utils::translateConstExpression(loop->stride, local_symbol_maps.iterators, local_symbol_maps.parameters)
                            );
  SgBasicBlock * for_body = SageBuilder::buildBasicBlock();
  SgForStatement * for_stmt = SageBuilder::buildForStatement(init_stmt, test_stmt, inc_expr, for_body);

  return std::pair<SgStatement *, SgScopeStatement *>(for_stmt, for_body);
}

template <class Annotation>
struct tile_generation_t {
  typedef typename ::KLT::LoopTrees<Annotation>::block_t block_t;

  tile_generation_t(
    SgStatement * gen_stmt_, SgScopeStatement * new_scope_, block_t * next_block_
  ) :
    gen_stmt(gen_stmt_), new_scope(new_scope_), next_block(next_block_)
  {}

  SgStatement * gen_stmt;
  SgScopeStatement * new_scope;
  block_t * next_block;
};

template <class Annotation, class Runtime>
tile_generation_t<Annotation> generateTiles(
  typename LoopTrees<Annotation>::tile_t * tile,
  const typename Kernel<Annotation, Runtime>::local_symbol_maps_t & local_symbol_maps
) {
  typedef LoopTrees<Annotation> LoopTrees;
  typedef typename LoopTrees::loop_t loop_t;
  typedef typename LoopTrees::block_t block_t;

  bool disordered_tiles = true;

  std::map<loop_t *, SgVariableSymbol *> loop_iterator_map;
  std::map<loop_t *, SgExpression *> loop_iterator_expression_map;

  SgForStatement * first_for_stmt = NULL;
  SgForStatement * last_for_stmt = NULL;
  block_t * block = NULL;
  while (tile != NULL) {
    loop_t * loop = tile->loop;
    assert(loop != NULL);

    SgVariableSymbol * tile_iterator = tile->iterator_sym;
    assert(tile_iterator != NULL);

    if (tile->kind == 0 || tile->kind == 1) {
      typename std::map<loop_t *, SgVariableSymbol *>::iterator it_loop_iterator = loop_iterator_map.find(loop);
      SgVariableSymbol * previous_iterator = NULL;
      if (it_loop_iterator != loop_iterator_map.end()) {
        previous_iterator = it_loop_iterator->second;
        it_loop_iterator->second = tile_iterator;
      }
      else {
        loop_iterator_map.insert(std::pair<loop_t *, SgVariableSymbol *>(loop, tile_iterator));
      }

      if (disordered_tiles) {
        typename std::map<loop_t *, SgExpression *>::iterator it_loop_iterator_expression = loop_iterator_expression_map.find(loop);
        if (it_loop_iterator_expression != loop_iterator_expression_map.end())
          it_loop_iterator_expression->second = SageBuilder::buildAddOp(it_loop_iterator_expression->second, SageBuilder::buildVarRefExp(tile_iterator));
        else
          loop_iterator_expression_map.insert(
            std::pair<loop_t *, SgExpression *>(
              loop,
              SageBuilder::buildAddOp(
                Runtime::kernel_api.buildGetLoopLower(loop->id, local_symbol_maps.context),
                SageBuilder::buildVarRefExp(tile_iterator)
              )
            )
          );
      }

      SgExpression * lower_bound = NULL;
      SgExpression * upper_bound = NULL;
      if (disordered_tiles) {
        lower_bound = SageBuilder::buildIntVal(0);
        upper_bound = Runtime::kernel_api.buildGetTileLength(tile->id, local_symbol_maps.context);
      }
      else {
        if (previous_iterator == NULL) {
          lower_bound = Runtime::kernel_api.buildGetLoopLower(loop->id, local_symbol_maps.context);
          upper_bound = Runtime::kernel_api.buildGetLoopUpper(loop->id, local_symbol_maps.context);
        }
        else {
          lower_bound = SageBuilder::buildVarRefExp(previous_iterator);
          upper_bound = SageBuilder::buildAddOp(
                          SageBuilder::buildVarRefExp(previous_iterator),
                          Runtime::kernel_api.buildGetTileLength(tile->id, local_symbol_maps.context)
                        ); // 'tile_iterator' + 'ctx'->tile['tile_id'].length
        }
      }

      SgExprStatement * init_stmt = SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(SageBuilder::buildVarRefExp(tile_iterator), lower_bound));
      SgExprStatement * test_stmt = NULL;
      if (!disordered_tiles && previous_iterator == NULL) {
        test_stmt = SageBuilder::buildExprStatement(SageBuilder::buildLessOrEqualOp(SageBuilder::buildVarRefExp(tile_iterator), upper_bound));
      }
      else {
        test_stmt = SageBuilder::buildExprStatement(SageBuilder::buildLessThanOp(SageBuilder::buildVarRefExp(tile_iterator), upper_bound));
      }
      SgExpression * inc_expr = SageBuilder::buildPlusAssignOp(
                                  SageBuilder::buildVarRefExp(tile_iterator),
                                  Runtime::kernel_api.buildGetTileStride(tile->id, local_symbol_maps.context)
                                ); // 'tile_iterator' += 'ctx'->tile['tile_id'].stride

      SgForStatement * for_stmt = SageBuilder::buildForStatement(init_stmt, test_stmt, inc_expr, NULL);

      if (last_for_stmt != NULL)
        SageInterface::setLoopBody(last_for_stmt, for_stmt);
      else
        first_for_stmt = for_stmt;
      last_for_stmt = for_stmt;
    }
    else {
      // Distributed tile: value of its iterator is specific to each instance of the kernel (See initializer).
    }

    block = tile->block;
    tile = tile->tile;
  }
  assert(block != NULL);

  SgBasicBlock * body = SageBuilder::buildBasicBlock();
  SageInterface::setLoopBody(last_for_stmt, body);

  if (disordered_tiles) {
    typename std::map<loop_t *, SgExpression *>::const_iterator it_loop_iterator_expression;
    for (it_loop_iterator_expression = loop_iterator_expression_map.begin(); it_loop_iterator_expression != loop_iterator_expression_map.end(); it_loop_iterator_expression++) {
      std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_sym_to_local = local_symbol_maps.iterators.find(it_loop_iterator_expression->first->iterator);
      assert(it_sym_to_local != local_symbol_maps.iterators.end());
      SageInterface::appendStatement(SageBuilder::buildAssignStatement(
        SageBuilder::buildVarRefExp(it_sym_to_local->second), it_loop_iterator_expression->second
      ), body);
    }
  }
  else {
    typename std::map<loop_t *, SgVariableSymbol *>::const_iterator it_loop_iterator;
    for (it_loop_iterator = loop_iterator_map.begin(); it_loop_iterator != loop_iterator_map.end(); it_loop_iterator++) {
      std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_sym_to_local = local_symbol_maps.iterators.find(it_loop_iterator->first->iterator);
      assert(it_sym_to_local != local_symbol_maps.iterators.end());
      SageInterface::appendStatement(SageBuilder::buildAssignStatement(
        SageBuilder::buildVarRefExp(it_sym_to_local->second), SageBuilder::buildVarRefExp(it_loop_iterator->second)
      ), body);
    }
  }

  return tile_generation_t<Annotation>(first_for_stmt, body, block);
}

template <class Annotation, class Runtime>
typename Runtime::exec_mode_t changeExecutionMode(
  const typename Runtime::exec_mode_t & exec_mode
) {
  return exec_mode;
}

template <class Annotation, class Runtime>
void generateSynchronizations(
  typename Runtime::exec_mode_t prev_exec_mode,
  typename Runtime::exec_mode_t next_exec_mode,
  SgScopeStatement * scope,
  const typename Kernel<Annotation, Runtime>::local_symbol_maps_t & local_symbol_maps
) {}

template <class Annotation, class Runtime>
SgScopeStatement * generateExecModeGuards(
  typename Runtime::exec_mode_t exec_mode,
  SgScopeStatement * scope,
  const typename Kernel<Annotation, Runtime>::local_symbol_maps_t & local_symbol_maps
) {
  return scope;
}

template <class Annotation, class Runtime>
void generateKernelBody(
  typename ::KLT::LoopTrees<Annotation>::node_t * node,
  typename Runtime::exec_mode_t exec_mode,
  const typename Kernel<Annotation, Runtime>::local_symbol_maps_t & local_symbol_maps,
  SgScopeStatement * scope
) {
  typedef typename LoopTrees<Annotation>::node_t node_t;
  typedef typename LoopTrees<Annotation>::loop_t loop_t;
  typedef typename LoopTrees<Annotation>::tile_t tile_t;
  typedef typename LoopTrees<Annotation>::cond_t cond_t;
  typedef typename LoopTrees<Annotation>::block_t block_t;
  typedef typename LoopTrees<Annotation>::stmt_t stmt_t;

  assert(node != NULL);

  loop_t  * loop  = dynamic_cast<loop_t  *>(node);
  tile_t  * tile  = dynamic_cast<tile_t  *>(node);
  cond_t  * cond  = dynamic_cast<cond_t  *>(node);
  block_t * block = dynamic_cast<block_t *>(node);
  stmt_t  * stmt  = dynamic_cast<stmt_t  *>(node);

  if (loop != NULL) {
    std::pair<SgStatement *, SgScopeStatement *> sg_loop = generateLoops<Annotation, Runtime>(loop, local_symbol_maps);
    SageInterface::appendStatement(sg_loop.first, scope);
    typename Runtime::exec_mode_t next_exec_mode = changeExecutionMode<Annotation, Runtime>(exec_mode);
    generateKernelBody<Annotation, Runtime>(loop->block, next_exec_mode, local_symbol_maps, sg_loop.second);
    generateSynchronizations<Annotation, Runtime>(next_exec_mode, exec_mode, scope, local_symbol_maps);
  }
  else if (tile != NULL) {
    tile_generation_t<Annotation> sg_tile = generateTiles<Annotation, Runtime>(tile, local_symbol_maps);
    if (sg_tile.gen_stmt != NULL) SageInterface::appendStatement(sg_tile.gen_stmt, scope);
    typename Runtime::exec_mode_t next_exec_mode = changeExecutionMode<Annotation, Runtime>(exec_mode);
    generateKernelBody<Annotation, Runtime>(sg_tile.next_block, next_exec_mode, local_symbol_maps, sg_tile.new_scope != NULL ? sg_tile.new_scope : scope);
    generateSynchronizations<Annotation, Runtime>(next_exec_mode, exec_mode, scope, local_symbol_maps);
  }
  else if (cond != NULL) {
    SgExprStatement * cond_stmt = SageBuilder::buildExprStatement(cond->condition); /// \todo translation of 'cond->condition'
    SgBasicBlock * bb_true = SageBuilder::buildBasicBlock();
    SgBasicBlock * bb_false = SageBuilder::buildBasicBlock();
    SgIfStmt * if_stmt = SageBuilder::buildIfStmt(cond_stmt, bb_true, bb_false);
    SageInterface::appendStatement(if_stmt, scope);
    if (cond->block_true != NULL)
      generateKernelBody<Annotation, Runtime>(cond->block_true, exec_mode, local_symbol_maps, bb_true);
    if (cond->block_false != NULL)
      generateKernelBody<Annotation, Runtime>(cond->block_false, exec_mode, local_symbol_maps, bb_false);
  }
  else if (block != NULL) {
    SgScopeStatement * bb_scope = scope;
    if (block->children.size() > 1) {
      bb_scope = SageBuilder::buildBasicBlock();
      SageInterface::appendStatement(bb_scope, scope);
    }
    typename std::vector<node_t * >::const_iterator it_child;
    for (it_child = block->children.begin(); it_child != block->children.end(); it_child++)
      generateKernelBody<Annotation, Runtime>(*it_child, exec_mode, local_symbol_maps, bb_scope);
  }
  else if (stmt != NULL) {
    SgStatement * sg_stmt = generateStatement<Annotation, Runtime>(stmt, local_symbol_maps);
    SgScopeStatement * bb_scope = generateExecModeGuards<Annotation, Runtime>(exec_mode, scope, local_symbol_maps);
    SageInterface::appendStatement(sg_stmt, bb_scope);
  }
  else assert(false);
}

/** @} */

template <class Annotation, class Runtime>
SgBasicBlock * intantiateOnHost(Kernel<Annotation, Runtime> * kernels) {

  // Expects only one version of the kernel
  assert(kernels->getKernels().size() == 1);

  typename Kernel<Annotation, Runtime>::kernel_desc_t * kernel = kernels->getKernels()[0];
  assert(kernel != NULL);

  // Replace kernel region by empty basic block
  SgBasicBlock * bb = SageBuilder::buildBasicBlock();
  // Insert: "struct kernel_t * kernel = build_kernel('kernel->id');"
  SgVariableSymbol * kernel_sym = Runtime::host_api.insertKernelInstance("kernel", kernel->id, bb);

  kernels->setRuntimeSpecificKernelField(kernel_sym, bb);

  // Arguments
  const typename Kernel<Annotation, Runtime>::arguments_t & arguments = kernels->getArguments();
  // Set kernel's parameters
  std::list<SgVariableSymbol *>::const_iterator it_param;
  size_t param_cnt = 0;
  for (it_param = arguments.parameters.begin(); it_param != arguments.parameters.end(); it_param++) {
    // Append: 'kernel'->params['param_cnt'] = '*it_param'
    SageInterface::appendStatement(Runtime::host_api.buildParamAssign(kernel_sym, param_cnt++, SageBuilder::buildVarRefExp(*it_param)), bb);
  }
  // Set kernel's scalars
  std::list<SgVariableSymbol *>::const_iterator it_scalar;
  size_t scalar_cnt = 0;
  for (it_scalar = arguments.scalars.begin(); it_scalar != arguments.scalars.end(); it_scalar++) {
    // Append: 'kernel'->scalars['param_cnt'] = &'*it_scalar'
    SageInterface::appendStatement(Runtime::host_api.buildScalarAssign(kernel_sym, scalar_cnt++, SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(*it_scalar))), bb);
  }
  // Set kernel's data
  typename std::list<KLT::Data<Annotation> *>::const_iterator it_data;
  size_t data_cnt = 0;
  for (it_data = arguments.datas.begin(); it_data != arguments.datas.end(); it_data++) {
    // 'data_first_elem' = 'symbol'
    SgExpression * data_first_elem = SageBuilder::buildVarRefExp((*it_data)->getVariableSymbol());
    for (size_t i = 0; i < (*it_data)->getSections().size(); i++)
      // 'data_first_elem' = 'data_first_elem'[0]
      data_first_elem = SageBuilder::buildPntrArrRefExp(data_first_elem, SageBuilder::buildIntVal(0));
    // Append: 'kernel'->datas['data_cnt'] = &'data_first_elem'
    SageInterface::appendStatement(Runtime::host_api.buildDataAssign(kernel_sym, data_cnt++, SageBuilder::buildAddressOfOp(data_first_elem)), bb);
  }

  // Set kernel's loop's bounds
  typename std::vector<typename Runtime::loop_desc_t *>::const_iterator it_loop;
  size_t loop_cnt = 0;
  for (it_loop = kernel->loops.begin(); it_loop != kernel->loops.end(); it_loop++) {
    // Append: 'kernel'->loops['loop_cnt'].lower = '(*it_loop)->lb'
    SageInterface::appendStatement(Runtime::host_api.buildLoopLowerAssign(kernel_sym, loop_cnt, SageInterface::copyExpression((*it_loop)->lb)), bb);
    // Append: 'kernel'->loops['loop_cnt'].upper = '(*it_loop)->ub'
    SageInterface::appendStatement(Runtime::host_api.buildLoopUpperAssign(kernel_sym, loop_cnt, SageInterface::copyExpression((*it_loop)->ub)), bb);
    // Append: 'kernel'->loops['loop_cnt'].stride = '(*it_loop)->stride'
    SageInterface::appendStatement(Runtime::host_api.buildLoopStrideAssign(kernel_sym, loop_cnt, SageInterface::copyExpression((*it_loop)->stride)), bb);

    loop_cnt++;
  }

  // Insert: "execute_kernel(kernel);"
  Runtime::host_api.insertKernelExecute(kernel_sym, bb);

  return bb;
}

}

#endif /* __KLT_KERNEL_HPP__ */

