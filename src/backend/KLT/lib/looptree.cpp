
#include "sage3basic.h"

#include "KLT/looptree.hpp"

#include <iostream>

namespace KLT {

namespace LoopTree {

extraction_context_t::extraction_context_t(loop_map_t & loop_map_, const vsym_set_t & data_) :
  loop_map(loop_map_), data(data_), loop_cnt(0), parameters(), iterators(), locals() {}

void extraction_context_t::addParameter(vsym_t * vsym) {
  if (iterators.find(vsym) == iterators.end() && locals.find(vsym) == locals.end() && data.find(vsym) == data.end())
    parameters.insert(vsym);
}

void extraction_context_t::addIterator(vsym_t * vsym) {
  iterators.insert(vsym);
}

void extraction_context_t::addLocal(vsym_t * vsym) {
  locals.insert(vsym);
}

void extraction_context_t::processVarRefs(SgNode * node) {
  std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(node);
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;
  for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
    SgVariableSymbol * sym = (*it_var_ref)->get_symbol();
    assert(sym->get_declaration() != NULL);
    assert(sym->get_declaration()->get_parent() != NULL);
    assert(sym->get_declaration()->get_parent()->get_parent() != NULL);

    if (isSgClassDefinition(sym->get_declaration()->get_parent()->get_parent())) continue; // reference to a field ('x' in 'a.x')

    addParameter(sym);
  }
}

size_t extraction_context_t::nextLoopID() {
  return loop_cnt++;
}

void extraction_context_t::registerLoop(SgForStatement * for_stmt, loop_t * loop) {
  loop_map.insert(std::pair<SgForStatement *, size_t>(for_stmt, loop->id));
}

const extraction_context_t::vsym_set_t & extraction_context_t::getParameters() const {
  return parameters;
}

////////////////////////////////////////////////

kind_e kindOf(SgStatement * stmt) {
       if ( isSgBasicBlock          (stmt)    ) return e_block;
  else if ( isSgForStatement        (stmt)    ) return e_loop;
  else if ( isSgIfStmt              (stmt)    ) return e_cond;
  else if ( isSgExprStatement       (stmt) ||
            isSgVariableDeclaration (stmt)    ) return e_stmt;
  else if ( isSgPragmaDeclaration   (stmt)    ) return e_ignored;
  else                                         return e_unknown;
}

// Constructors

node_t::node_t(kind_e kind_) :
  kind(kind_), parent(NULL) {}

node_t::~node_t() {}

block_t::block_t() :
  node_t(e_block), children() {}

block_t::~block_t() {}

cond_t::cond_t(SgExpression * cond) :
  node_t(e_cond), condition(cond), branch_true(NULL), branch_false(NULL) {}

cond_t::~cond_t() {}

loop_t::loop_t(size_t id_, SgVariableSymbol * it, SgExpression * lb, SgExpression * ub, SgExpression * stride_) :
  node_t(e_loop), id(id_), iterator(it), lower_bound(lb), upper_bound(ub), stride(stride_), body(NULL) {}

loop_t::~loop_t() {}

stmt_t::stmt_t(SgStatement * stmt) :
  node_t(e_stmt), statement(stmt) {}

stmt_t::~stmt_t() {}

tile_t::tile_t(size_t id_, unsigned long kind_, size_t order_, SgExpression * param_, loop_t * loop_) :
  node_t(e_tile), id(id_), kind(kind_), order(order_), param(param_), loop(loop_), tile(NULL), block(NULL) {}

tile_t::~tile_t() {}

// Extract Methods

node_t * node_t::extract(SgStatement * stmt, extraction_context_t & ctx) {
  node_t * res = NULL;

  switch (kindOf(stmt)) {
    case e_block: res = block_t::extract(stmt, ctx); break;
    case e_cond:  res = cond_t::extract (stmt, ctx); break;
    case e_loop:  res = loop_t::extract (stmt, ctx); break;
    case e_stmt:  res = stmt_t::extract (stmt, ctx); break;

    case e_ignored:
      std::cerr << "[KLT][LoopTrees][Warning] Ignored statement " << stmt << " (type: " << stmt->class_name() << ")." << std::endl;
      break;

    case e_unknown:
    default: 
      std::cerr << "[KLT][LoopTrees][Error] Uknown statement " << stmt << " (type: " << stmt->class_name() << ")." << std::endl;
      assert(false);
  }

  if (res == NULL) {
    std::cerr << "[KLT][LoopTrees][Warning] Statement " << stmt << " (type: " << stmt->class_name() << ") cannot be translated." << std::endl;
  }

  return res;
}

block_t * block_t::extract(SgStatement * stmt, extraction_context_t & ctx) {
  SgBasicBlock * bb = isSgBasicBlock(stmt);
  assert(bb != NULL);

  block_t * block = new block_t();

  std::vector<SgStatement *>::const_iterator it_stmt;
  for (it_stmt = bb->get_statements().begin(); it_stmt != bb->get_statements().end(); it_stmt++) {
    node_t * node = node_t::extract(*it_stmt, ctx);
    if (node != NULL)
      block->children.push_back(node);
  }

  return block;
}

cond_t * cond_t::extract(SgStatement * stmt, extraction_context_t & ctx) {
  SgIfStmt * if_stmt = isSgIfStmt(stmt);
  assert(if_stmt != NULL);

  SgExprStatement * cond_stmt = isSgExprStatement(if_stmt->get_conditional());
  assert(cond_stmt != NULL);
  SgExpression * cond_expr = cond_stmt->get_expression();
  assert(cond_expr != NULL);

  ctx.processVarRefs(cond_expr);

  cond_t * cond = new cond_t(cond_expr);
      
  cond->branch_true = node_t::extract(if_stmt->get_true_body(), ctx);
  cond->branch_false = node_t::extract(if_stmt->get_false_body(), ctx);
      
  return cond;
}

loop_t * loop_t::extract(SgStatement * stmt, extraction_context_t & ctx) {
  SgForStatement * for_stmt = isSgForStatement(stmt);
  assert(for_stmt != NULL);

  SgVariableSymbol * iterator = NULL;
  SgExpression * lower_bound = NULL;
  SgExpression * upper_bound = NULL;
  SgExpression * stride = NULL;

  bool canon_loop = SageInterface::getForLoopInformations(for_stmt, iterator, lower_bound, upper_bound, stride);
  assert(canon_loop == true);

  ctx.addIterator(iterator);

  ctx.processVarRefs(for_stmt->get_for_init_stmt());
  ctx.processVarRefs(for_stmt->get_test());
  ctx.processVarRefs(for_stmt->get_increment());

  loop_t * loop = new loop_t(ctx.nextLoopID(), iterator, lower_bound, upper_bound, stride);

  ctx.registerLoop(for_stmt, loop);

  loop->body = node_t::extract(for_stmt->get_loop_body(), ctx);

  return loop;
}

stmt_t * stmt_t::extract(SgStatement * stmt, extraction_context_t & ctx) {
  if (isSgExprStatement(stmt)) {
    SgExprStatement * expr_stmt = (SgExprStatement *)stmt;
    SgExpression * expr = expr_stmt->get_expression();
    assert(expr != NULL);

    ctx.processVarRefs(expr);

    return new stmt_t(stmt);
  }
  else if (isSgVariableDeclaration(stmt)) {
    SgVariableDeclaration * var_decl = isSgVariableDeclaration(stmt);
    assert(var_decl != NULL);
    SgScopeStatement * scope = var_decl->get_scope();
    assert(scope != NULL);

    const std::vector<SgInitializedName *> & decls = var_decl->get_variables();
    std::vector<SgInitializedName *>::const_iterator it_decl;
    for (it_decl = decls.begin(); it_decl != decls.end(); it_decl++) {
      SgVariableSymbol * var_sym = scope->lookup_variable_symbol((*it_decl)->get_name());
      assert(var_sym != NULL);
      ctx.addLocal(var_sym);
    }

    ctx.processVarRefs(stmt);

    return new stmt_t(stmt);
  }
  else assert(false);
}

}

}

