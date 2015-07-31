
#include "sage3basic.h"

#include "MFB/KLT/driver.hpp"

#include "KLT/descriptor.hpp"
#include "KLT/looptree.hpp"
#include "KLT/runtime.hpp"
#include "KLT/api.hpp"
#include "KLT/utils.hpp"

namespace MFB {

Driver< ::MFB::KLT::KLT>::Driver(SgProject * project_) : ::MFB::Driver<Sage>(project_) {}
Driver< ::MFB::KLT::KLT>::~Driver() {}

Driver< ::MFB::KLT::KLT>::kernel_desc_t::kernel_desc_t(node_t * root_, const vsym_list_t & parameters_, const data_list_t & data_, ::KLT::Runtime * runtime_) :
  root(root_), parameters(parameters_), data(data_), runtime(runtime_) {}

Driver< ::MFB::KLT::KLT>::looptree_desc_t::looptree_desc_t(node_t * node_, ::KLT::Runtime * runtime_, const ::KLT::Utils::symbol_map_t & symbol_map_) :
  symbol_map(symbol_map_), runtime(runtime_), node(node_) {}

namespace KLT {

typedef std::vector<node_t *> node_list_t;
typedef node_list_t::const_iterator node_list_citer_t;

typedef std::vector< ::KLT::Descriptor::loop_t> loop_vect_t;
typedef std::vector< ::KLT::Descriptor::tile_t> tile_vect_t;

typedef ::MFB::Sage<SgFunctionDeclaration>::object_desc_t sage_func_desc_t;
typedef ::MFB::Sage<SgFunctionDeclaration>::build_result_t sage_func_res_t;

typedef ::MFB::Driver< ::MFB::KLT::KLT>::looptree_desc_t looptree_desc_t;

/////////////////////////////////////////////////////////////////////////////
///// kernel_t

size_t KLT<kernel_t>::kernel_cnt = 0;

::KLT::Descriptor::kernel_t * KLT<kernel_t>::buildKernelDesc(const std::string & kernel_prefix) {
  std::ostringstream oss; oss << kernel_prefix << "_" << kernel_cnt;
  return new ::KLT::Descriptor::kernel_t(kernel_cnt, oss.str());
}

void insert(loop_t * loop, loop_vect_t & loops) {
  loops.resize(loop->id + 1);
  loops[loop->id] = ::KLT::Descriptor::loop_t(loop->id, loop->lower_bound, loop->upper_bound, loop->stride, loop->iterator);
}

void insert(tile_t * tile, tile_vect_t & tiles) {
  tiles.resize(tile->id + 1);
  tiles[tile->id] = ::KLT::Descriptor::tile_t(tile->id, (::KLT::Descriptor::tile_kind_e)tile->kind, tile->order, tile->param);
}

void collectLoopsAndTiles(node_t * node, loop_vect_t & loops, tile_vect_t & tiles) {
  if (node == NULL) return;

  assert(dynamic_cast<node_t *>(node) != NULL);

  switch (node->kind) {
    case ::KLT::LoopTree::e_block:
    {
      const node_list_t & children = ((block_t*)node)->children;
      for (node_list_citer_t it = children.begin(); it != children.end(); it++)
        collectLoopsAndTiles(*it, loops, tiles);
      break;
    }
    case ::KLT::LoopTree::e_cond:
      collectLoopsAndTiles(((cond_t*)node)->branch_true , loops, tiles);
      collectLoopsAndTiles(((cond_t*)node)->branch_false, loops, tiles);
      break;
    case ::KLT::LoopTree::e_loop:
    {
      insert((loop_t *)node, loops);
      collectLoopsAndTiles(((loop_t*)node)->body, loops, tiles);
      break;
    }
    case ::KLT::LoopTree::e_tile:
    {
      tile_t * tile = (tile_t*)node;
      insert(tile, tiles);
      insert(tile->loop, loops);
      collectLoopsAndTiles(tile->tile , loops, tiles);
      collectLoopsAndTiles(tile->block, loops, tiles);
      break;
    }
    case ::KLT::LoopTree::e_stmt:
      break;
    case ::KLT::LoopTree::e_ignored:
    case ::KLT::LoopTree::e_unknown:
    default:
      assert(false);
  }
}

sage_func_res_t KLT<kernel_t>::buildKernelDecl(::KLT::Descriptor::kernel_t & res, ::KLT::Runtime * runtime) {
  ::MFB::Driver< ::MFB::Sage> & driver = runtime->getModelBuilder().getDriver();
  SgFunctionParameterList * kernel_param_list = runtime->getCallInterface().buildKernelParamList(res);
  SgType * kernel_ret_type = runtime->getCallInterface().buildKernelReturnType(res);

  sage_func_desc_t sage_func_desc(res.kernel_name, kernel_ret_type, kernel_param_list, NULL, runtime->getKernelFileID(), runtime->getKernelFileID());

  sage_func_res_t sage_func_res = driver.build<SgFunctionDeclaration>(sage_func_desc);

  {
    SgFunctionDeclaration * kernel_decl = sage_func_res.symbol->get_declaration();
    assert(kernel_decl != NULL);

    SgFunctionDeclaration * first_kernel_decl = isSgFunctionDeclaration(kernel_decl->get_firstNondefiningDeclaration());
    assert(first_kernel_decl != NULL);
    runtime->getCallInterface().applyKernelModifiers(first_kernel_decl);

    SgFunctionDeclaration * defn_kernel_decl = isSgFunctionDeclaration(kernel_decl->get_definingDeclaration());
    assert(defn_kernel_decl != NULL);
    runtime->getCallInterface().applyKernelModifiers(defn_kernel_decl);

//  if (guard_kernel_decl) SageInterface::guardNode(defn_kernel_decl, std::string("defined(ENABLE_") + result->kernel_name + ")");
  }
  return sage_func_res;
}

KLT<kernel_t>::build_result_t KLT<kernel_t>::build(::MFB::Driver< ::MFB::KLT::KLT> & driver, const KLT<kernel_t>::object_desc_t & object) {
  std::string kernel_prefix = "klt_kernel";
  ::KLT::Descriptor::kernel_t * res = buildKernelDesc(kernel_prefix);

  collectLoopsAndTiles(object.root, res->loops, res->tiles);

  res->parameters = object.parameters;
  res->data = object.data;

  sage_func_res_t sage_func_res = buildKernelDecl(*res, object.runtime);

  ::KLT::Utils::symbol_map_t symbol_map;

  SgBasicBlock * body = object.runtime->getCallInterface().generateKernelBody(*res, sage_func_res.definition, symbol_map);

  looptree_desc_t looptree_desc(object.root, object.runtime, symbol_map);
  SgStatement * stmt = driver.build<node_t>(looptree_desc);
  SageInterface::appendStatement(stmt, body);

  SageInterface::setSourcePositionForTransformation(body);

  return res;
}

/////////////////////////////////////////////////////////////////////////////
///// LoopTree::node_t

KLT<node_t>::build_result_t KLT<node_t>::build(::MFB::Driver< ::MFB::KLT::KLT> & driver, const KLT<node_t>::object_desc_t & object) {
  assert(object.node != NULL);
  assert(dynamic_cast<node_t *>(object.node) != NULL);

  SgStatement * result = NULL;

  switch (object.node->kind) {
    case ::KLT::LoopTree::e_block: result = driver.build<block_t>(object); break;
    case ::KLT::LoopTree::e_cond:  result = driver.build<cond_t >(object); break;
    case ::KLT::LoopTree::e_loop:  result = driver.build<loop_t >(object); break;
    case ::KLT::LoopTree::e_tile:  result = driver.build<tile_t >(object); break;
    case ::KLT::LoopTree::e_stmt:  result = driver.build<stmt_t >(object); break;
    case ::KLT::LoopTree::e_ignored:
    case ::KLT::LoopTree::e_unknown:
    default:
      assert(false);
  }

  return result;
}

/////////////////////////////////////////////////////////////////////////////
///// LoopTree::block_t

KLT<block_t>::build_result_t KLT<block_t>::build(::MFB::Driver< ::MFB::KLT::KLT> & driver, const KLT<block_t>::object_desc_t & object) {
  assert(object.node != NULL);
  assert(object.node->kind == ::KLT::LoopTree::e_block);
  assert(dynamic_cast<block_t *>(object.node) != NULL);

  block_t * block = (block_t *)object.node;

  if (block->children.size() == 0)
    return NULL;
  if (block->children.size() == 1)
    return driver.build<node_t>(looptree_desc_t(block->children[0], object.runtime, object.symbol_map));

  SgBasicBlock * bb = SageBuilder::buildBasicBlock();
  for (node_list_t::const_iterator it = block->children.begin(); it != block->children.end(); it++) {
    SgStatement * stmt = driver.build<node_t>(looptree_desc_t(*it, object.runtime, object.symbol_map));
    if (stmt != NULL)
      SageInterface::appendStatement(stmt, bb);
  }
  return bb;
}

///// LoopTree::cond_t

KLT<cond_t>::build_result_t KLT<cond_t>::build(::MFB::Driver< ::MFB::KLT::KLT> & driver, const KLT<cond_t>::object_desc_t & object) {
  assert(object.node != NULL);
  assert(object.node->kind == ::KLT::LoopTree::e_cond);
  assert(dynamic_cast<cond_t *>(object.node) != NULL);

  cond_t * cond = (cond_t *)object.node;

  SgExpression * condition = ::KLT::Utils::translateExpression(cond->condition, object.symbol_map);

  SgStatement * true_body  = driver.build<node_t>(looptree_desc_t(cond->branch_true, object.runtime, object.symbol_map));
  SgStatement * false_body = driver.build<node_t>(looptree_desc_t(cond->branch_false, object.runtime, object.symbol_map));

  return SageBuilder::buildIfStmt(SageBuilder::buildExprStatement(condition), true_body, false_body);
}

///// LoopTree::loop_t

KLT<loop_t>::build_result_t KLT<loop_t>::build(::MFB::Driver< ::MFB::KLT::KLT> & driver, const KLT<loop_t>::object_desc_t & object) {
  assert(object.node != NULL);
  assert(object.node->kind == ::KLT::LoopTree::e_loop);
  assert(dynamic_cast<loop_t *>(object.node) != NULL);

  loop_t * loop = (loop_t *)object.node;

  std::map<size_t, SgVariableSymbol *>::const_iterator it = object.symbol_map.iter_loops.find(loop->id);
  assert(it != object.symbol_map.iter_loops.end());

  SgVariableSymbol * iterator = it->second;
  SgExpression * lower_bound = object.runtime->getKernelAPI().buildGetLoopLower (loop->id, object.symbol_map.loop_context);
  SgExpression * upper_bound = object.runtime->getKernelAPI().buildGetLoopUpper (loop->id, object.symbol_map.loop_context);
  SgExpression * stride      = object.runtime->getKernelAPI().buildGetLoopStride(loop->id, object.symbol_map.loop_context);
//SgExpression * lower_bound = ::KLT::Utils::translateExpression(loop->lower_bound, object.symbol_map);
//SgExpression * upper_bound = ::KLT::Utils::translateExpression(loop->upper_bound, object.symbol_map);
//SgExpression * stride      = ::KLT::Utils::translateExpression(loop->stride     , object.symbol_map);

  return SageBuilder::buildForStatement(
    SageBuilder::buildForInitStatement(SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(SageBuilder::buildVarRefExp(iterator), lower_bound))),
    SageBuilder::buildExprStatement(SageBuilder::buildLessOrEqualOp(SageBuilder::buildVarRefExp(iterator), upper_bound)),
    SageBuilder::buildPlusAssignOp(SageBuilder::buildVarRefExp(iterator), stride),
    driver.build<node_t>(looptree_desc_t(loop->body, object.runtime, object.symbol_map))
  );
}

///// LoopTree::tile_t

KLT<tile_t>::build_result_t KLT<tile_t>::build(::MFB::Driver< ::MFB::KLT::KLT> & driver, const KLT<tile_t>::object_desc_t & object) {
  assert(object.node != NULL);
  assert(object.node->kind == ::KLT::LoopTree::e_tile);
  assert(dynamic_cast<tile_t *>(object.node) != NULL);

  tile_t * tile = (tile_t *)object.node;

  SgStatement * stmt = NULL;

  assert(false); // TODO

  return stmt;
}

///// LoopTree::stmt_t

KLT<stmt_t>::build_result_t KLT<stmt_t>::build(::MFB::Driver< ::MFB::KLT::KLT> & driver, const KLT<stmt_t>::object_desc_t & object) {
  assert(object.node != NULL);
  assert(object.node->kind == ::KLT::LoopTree::e_stmt);
  assert(dynamic_cast<stmt_t *>(object.node) != NULL);

  return ::KLT::Utils::translateStatement(((stmt_t *)object.node)->statement, object.symbol_map);
}

} // namespace MFB::KLT

} // namespace MFB

