
#include "sage3basic.h"

#include "MFB/KLT/driver.hpp"

#include "KLT/Core/descriptor.hpp"
#include "KLT/Core/looptree.hpp"
#include "KLT/Core/generator.hpp"
#include "KLT/Core/api.hpp"
#include "KLT/Core/utils.hpp"

#ifndef VERBOSE
# define VERBOSE 0
#endif

namespace MFB {

Driver< ::MFB::KLT::KLT>::Driver(SgProject * project_) : ::MFB::Driver<Sage>(project_) {}
Driver< ::MFB::KLT::KLT>::~Driver() {}

Driver< ::MFB::KLT::KLT>::kernel_desc_t::kernel_desc_t(node_t * root_, const vsym_list_t & parameters_, const data_list_t & data_, ::KLT::Generator * generator_) :
  root(root_), parameters(parameters_), data(data_), generator(generator_) {}

Driver< ::MFB::KLT::KLT>::looptree_desc_t::looptree_desc_t(node_t * node_, ::KLT::Generator * generator_, const ::KLT::Utils::symbol_map_t & symbol_map_) :
  symbol_map(symbol_map_), generator(generator_), node(node_) {}

namespace KLT {

typedef std::vector<node_t *> node_list_t;
typedef node_list_t::const_iterator node_list_citer_t;

typedef std::vector< ::KLT::Descriptor::loop_t *> loop_vect_t;
typedef std::vector< ::KLT::Descriptor::tile_t *> tile_vect_t;

typedef ::MFB::Sage<SgFunctionDeclaration>::object_desc_t sage_func_desc_t;
typedef ::MFB::Sage<SgFunctionDeclaration>::build_result_t sage_func_res_t;

typedef ::MFB::Driver< ::MFB::KLT::KLT>::looptree_desc_t looptree_desc_t;

/////////////////////////////////////////////////////////////////////////////
///// kernel_t

size_t KLT<kernel_t>::kernel_cnt = 0;

::KLT::Descriptor::kernel_t * KLT<kernel_t>::buildKernelDesc(const std::string & kernel_prefix) {
  std::ostringstream oss; oss << kernel_prefix << "_" << kernel_cnt;
  return new ::KLT::Descriptor::kernel_t(kernel_cnt++, oss.str());
}

sage_func_res_t KLT<kernel_t>::buildKernelDecl(::KLT::Descriptor::kernel_t & res, ::KLT::Generator * generator) {
  ::MFB::Driver< ::MFB::Sage> & driver = generator->getModelBuilder().getDriver();
  SgFunctionParameterList * kernel_param_list = generator->getCallInterface().buildKernelParamList(res);
  SgType * kernel_ret_type = generator->getKernelAPI().buildKernelReturnType(res);

  sage_func_desc_t sage_func_desc(res.kernel_name, kernel_ret_type, kernel_param_list, NULL, generator->getKernelFileID(), generator->getKernelFileID());

#if VERBOSE
  std::cerr << "[Info] (MFB::KLT::KLT<kernel_t>::buildKernelDecl) Calling Driver<Sage>::build<SgFunctionDeclaration> to create " << res.kernel_name << " in file #" << generator->getKernelFileID() << "." << std::endl;
#endif

  sage_func_res_t sage_func_res = driver.build<SgFunctionDeclaration>(sage_func_desc);

  {
    SgFunctionDeclaration * kernel_decl = sage_func_res.symbol->get_declaration();
    assert(kernel_decl != NULL);

    SgFunctionDeclaration * first_kernel_decl = isSgFunctionDeclaration(kernel_decl->get_firstNondefiningDeclaration());
    assert(first_kernel_decl != NULL);
    generator->getKernelAPI().applyKernelModifiers(first_kernel_decl);

    SgFunctionDeclaration * defn_kernel_decl = isSgFunctionDeclaration(kernel_decl->get_definingDeclaration());
    assert(defn_kernel_decl != NULL);
    generator->getKernelAPI().applyKernelModifiers(defn_kernel_decl);

//  if (guard_kernel_decl) SageInterface::guardNode(defn_kernel_decl, std::string("defined(ENABLE_") + result->kernel_name + ")");
  }
  return sage_func_res;
}

KLT<kernel_t>::build_result_t KLT<kernel_t>::build(::MFB::Driver< ::MFB::KLT::KLT> & driver, const KLT<kernel_t>::object_desc_t & object) {
  std::string kernel_prefix = "klt_kernel";
  ::KLT::Descriptor::kernel_t * res = buildKernelDesc(kernel_prefix);

  std::map<const ::KLT::LoopTree::loop_t *, ::KLT::Descriptor::loop_t *> loop_translation_map;

  object.root->collectLoops(res->loops, loop_translation_map);
  object.root->collectTiles(res->tiles, loop_translation_map);

#if VERBOSE
  std::cerr << "[Info] (MFB::KLT::KLT<kernel_t>::build) " << res->loops.size() << " loops and " << res->tiles.size() << " tiles." << std::endl;
#endif

  res->parameters = object.parameters;
  res->data = object.data;

  sage_func_res_t sage_func_res = buildKernelDecl(*res, object.generator);

  ::KLT::Utils::symbol_map_t symbol_map;

  SgBasicBlock * body = object.generator->getCallInterface().generateKernelBody(*res, sage_func_res.definition, symbol_map);
  assert(body->get_parent() != NULL);

  looptree_desc_t looptree_desc(object.root, object.generator, symbol_map);
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
    return driver.build<node_t>(looptree_desc_t(block->children[0], object.generator, object.symbol_map));

  SgBasicBlock * bb = SageBuilder::buildBasicBlock();
  for (node_list_t::const_iterator it = block->children.begin(); it != block->children.end(); it++) {
    SgStatement * stmt = driver.build<node_t>(looptree_desc_t(*it, object.generator, object.symbol_map));
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

  SgExpression * condition = object.symbol_map.translate(cond->condition);

  SgStatement * true_body  = driver.build<node_t>(looptree_desc_t(cond->branch_true, object.generator, object.symbol_map));
  SgStatement * false_body = driver.build<node_t>(looptree_desc_t(cond->branch_false, object.generator, object.symbol_map));

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
  SgExpression * lower_bound = object.generator->getKernelAPI().buildGetLoopLower (loop->id, object.symbol_map.loop_context);
  SgExpression * upper_bound = object.generator->getKernelAPI().buildGetLoopUpper (loop->id, object.symbol_map.loop_context);
  SgExpression * stride      = object.generator->getKernelAPI().buildGetLoopStride(loop->id, object.symbol_map.loop_context);

  return SageBuilder::buildForStatement(
    SageBuilder::buildForInitStatement(SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(SageBuilder::buildVarRefExp(iterator), lower_bound))),
    SageBuilder::buildExprStatement(SageBuilder::buildLessOrEqualOp(SageBuilder::buildVarRefExp(iterator), upper_bound)),
    SageBuilder::buildPlusAssignOp(SageBuilder::buildVarRefExp(iterator), stride),
    driver.build<node_t>(looptree_desc_t(loop->body, object.generator, object.symbol_map))
  );
}

///// LoopTree::tile_t

std::pair<SgVariableSymbol *, SgForStatement *> buildTile(::MFB::Driver< ::MFB::KLT::KLT> & driver, tile_t * tile, ::KLT::Generator * generator, const ::KLT::Utils::symbol_map_t & symbol_map) {
  std::pair<SgVariableSymbol *, SgForStatement *> res;

  std::map<size_t, SgVariableSymbol *>::const_iterator it = symbol_map.iter_tiles.find(tile->id);
  assert(it != symbol_map.iter_tiles.end());
  res.first = it->second;
  res.second = NULL;

  // Only static (0) and dynamic (1) tiles are unparsed as loops other tiles are distributed (hence their iterators are constant for each instance of the kernel)
  if (tile->kind == 0 || tile->kind == 1) {
    SgExpression * lower_bound = SageBuilder::buildIntVal(0);
    SgExpression * upper_bound = generator->getKernelAPI().buildGetTileLength(tile->id, symbol_map.loop_context);
    SgExpression * stride      = generator->getKernelAPI().buildGetTileStride(tile->id, symbol_map.loop_context);

    res.second = SageBuilder::buildForStatement(
      SageBuilder::buildForInitStatement(SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(SageBuilder::buildVarRefExp(res.first), lower_bound))),
      SageBuilder::buildExprStatement(SageBuilder::buildLessThanOp(SageBuilder::buildVarRefExp(res.first), upper_bound)),
      SageBuilder::buildPlusAssignOp(SageBuilder::buildVarRefExp(res.first), stride), NULL
    );
  }

  return res;
}

KLT<tile_t>::build_result_t KLT<tile_t>::build(::MFB::Driver< ::MFB::KLT::KLT> & driver, const KLT<tile_t>::object_desc_t & object) {
  assert(object.node != NULL);
  assert(object.node->kind == ::KLT::LoopTree::e_tile);
  assert(dynamic_cast<tile_t *>(object.node) != NULL);

  tile_t * tile = (tile_t *)object.node;

  std::vector<tile_t *> tile_chain;
  std::map<size_t, SgExpression *> loop_iter_map;
  while (tile->next_tile != NULL) {
    tile_chain.push_back(tile);
    loop_iter_map[tile->loop->id] = NULL;
    tile = tile->next_tile;
  };
  tile_chain.push_back(tile);
  loop_iter_map[tile->loop->id] = NULL;

  std::map<size_t, SgExpression *>::iterator it_loop_iter;
  for (it_loop_iter = loop_iter_map.begin(); it_loop_iter != loop_iter_map.end(); it_loop_iter++)
    it_loop_iter->second = object.generator->getKernelAPI().buildGetLoopLower(it_loop_iter->first, object.symbol_map.loop_context);

  SgBasicBlock * bb = SageBuilder::buildBasicBlock();

  SgStatement * stmt = bb;
  std::vector<tile_t *>::const_reverse_iterator it = tile_chain.rbegin();
  while (it != tile_chain.rend()) {
    std::pair<SgVariableSymbol *, SgForStatement *> res = buildTile(driver, *it, object.generator, object.symbol_map);
    loop_iter_map[(*it)->loop->id] = SageBuilder::buildAddOp(loop_iter_map[(*it)->loop->id], SageBuilder::buildVarRefExp(res.first));
    if (res.second != NULL) {
      res.second->set_loop_body(stmt);
      stmt->set_parent(res.second);
      stmt = res.second;
    }
    it++;
  }

  for (it_loop_iter = loop_iter_map.begin(); it_loop_iter != loop_iter_map.end(); it_loop_iter++) {
    std::map<size_t, SgVariableSymbol *>::const_iterator it = object.symbol_map.iter_loops.find(it_loop_iter->first);
    assert(it != object.symbol_map.iter_loops.end());
    SageInterface::appendStatement(SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(SageBuilder::buildVarRefExp(it->second), it_loop_iter->second)), bb);
  }

  SageInterface::appendStatement(driver.build<node_t>(looptree_desc_t(tile->next_node, object.generator, object.symbol_map)), bb);

  return stmt;
}

///// LoopTree::stmt_t

KLT<stmt_t>::build_result_t KLT<stmt_t>::build(::MFB::Driver< ::MFB::KLT::KLT> & driver, const KLT<stmt_t>::object_desc_t & object) {
  assert(object.node != NULL);
  assert(object.node->kind == ::KLT::LoopTree::e_stmt);
  assert(dynamic_cast<stmt_t *>(object.node) != NULL);

  return object.symbol_map.translate(((stmt_t *)object.node)->statement);
}

} // namespace MFB::KLT

} // namespace MFB

