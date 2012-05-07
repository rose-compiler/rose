
#include "common/spmd-generator.hpp"
#include "common/spmd-driver.hpp"
#include "common/spmd-tree.hpp"
#include "common/comm-analysis.hpp"
#include "common/array-analysis.hpp"
#include "common/placement.hpp"

#include "toolboxes/rose-toolbox.hpp"
#include "toolboxes/algebra-container.hpp"

#include "rose.h"

SgStatement * SPMD_Generator::codeGeneration(SPMD_Tree * tree) {
  SPMD_NativeStmt * nat = dynamic_cast<SPMD_NativeStmt *>(tree);
  SPMD_Loop * loop = dynamic_cast<SPMD_Loop *>(tree);
  SPMD_DomainRestriction * dr = dynamic_cast<SPMD_DomainRestriction *>(tree);
  SPMD_KernelCall * kc = dynamic_cast<SPMD_KernelCall *>(tree);
  SPMD_Comm * comm = dynamic_cast<SPMD_Comm *>(tree);
  SPMD_Sync * sync = dynamic_cast<SPMD_Sync *>(tree);
  SPMD_Root * root = dynamic_cast<SPMD_Root *>(tree);

  if (nat != NULL)  return codeGeneration(nat);
  if (loop != NULL) return codeGeneration(loop);
  if (dr != NULL)   return codeGeneration(dr);
  if (kc != NULL)   return codeGeneration(kc);
  if (comm != NULL) return codeGeneration(comm);
  if (sync != NULL) return codeGeneration(sync);
  if (root != NULL) return codeGeneration(root);
}

SgStatement * SPMD_Generator::codeGeneration(SPMD_NativeStmt * tree) {
  SgExprStatement * expr_stmt = isSgExprStatement(tree->getStatement());
  SgExpression * exp = SageInterface::copyExpression(expr_stmt->get_expression());

  ComputeSystem * placement = driver.getPlacement().assigned(tree);
  assert(placement != NULL);

  std::map<ComputeSystem *, std::map<ArrayPartition *, ArrayAlias *> >::iterator it_cs = array_aliases.find(placement);
  assert(it_cs != array_aliases.end());

  std::map<ArrayPartition *, ArrayAlias *>::iterator it_alias;
  for (it_alias = it_cs->second.begin(); it_alias != it_cs->second.end(); it_alias++)
    exp = it_alias->second->propagate(exp);

  return SageBuilder::buildExprStatement(exp);
}

SgStatement * SPMD_Generator::codeGeneration(SPMD_Loop * tree) {
  SgForStatement * for_stmt = NULL;

  RoseVariable & iterator = tree->getIterator();
  Domain * domain = tree->getDomain();

  SgExprStatement * lb_stmt = domain->genInit();
  SgExprStatement * ub_stmt = domain->genTest();
  SgExpression * inc = domain->genIncrement();

  for_stmt = SageBuilder::buildForStatement(lb_stmt, ub_stmt, inc, NULL);

  SageBuilder::pushScopeStack(for_stmt);

  std::vector<SPMD_Tree *> & children = tree->getChildren();
  if (children.size() == 1) {
    for_stmt->set_loop_body(codeGeneration(children[0]));
  }
  else if (children.size() > 1) {
    SgBasicBlock * bb = SageBuilder::buildBasicBlock();
    SageBuilder::pushScopeStack(bb);

    std::vector<SPMD_Tree *>::iterator it;
    for (it = children.begin(); it != children.end(); it++) {
      SgStatement * child_res = codeGeneration(*it);
      SgBasicBlock * child_bb = isSgBasicBlock(child_res);
      if (child_bb != NULL) {
        std::vector<SgStatement *>::iterator it_child_bb;
        for (it_child_bb = child_bb->get_statements().begin(); it_child_bb != child_bb->get_statements().end(); it_child_bb++)
          bb->append_statement(*it_child_bb);
      }
      else
        bb->append_statement(child_res);
    }

    SageBuilder::popScopeStack();
    for_stmt->set_loop_body(bb);
  }
  else assert(false);

  SageBuilder::popScopeStack();

  return for_stmt;
}

SgStatement * SPMD_Generator::codeGeneration(SPMD_DomainRestriction * tree) {
  SgIfStmt * if_stmt = NULL;
  
  const std::vector<std::pair<Expression *, bool> > & restriction = tree->getRestriction();

  std::vector<SgExpression *> sg_restriction;
  std::vector<std::pair<Expression *, bool> >::const_iterator it;
  for (it = restriction.begin(); it != restriction.end(); it++)
    if (it->second)
      sg_restriction.push_back(it->first->genEqualToZero());
    else
      sg_restriction.push_back(it->first->genGreaterOrEqualToZero());

  SgExpression * condition = genAnd(sg_restriction);

  SgStatement * dummy_null_stmt = SageBuilder::buildNullStatement();

  if_stmt = SageBuilder::buildIfStmt(condition, dummy_null_stmt, NULL);

  SageBuilder::pushScopeStack(if_stmt);

  std::vector<SPMD_Tree *> & children = tree->getChildren();
  if (children.size() == 1) {
    if_stmt->set_true_body(codeGeneration(children[0]));
  }
  else if (children.size() > 1) {
    SgBasicBlock * bb = SageBuilder::buildBasicBlock();
    SageBuilder::pushScopeStack(bb);

    std::vector<SPMD_Tree *>::iterator it;
    for (it = children.begin(); it != children.end(); it++) {
      SgStatement * child_res = codeGeneration(*it);
      SgBasicBlock * child_bb = isSgBasicBlock(child_res);
      if (child_bb != NULL) {
        std::vector<SgStatement *>::iterator it_child_bb;
        for (it_child_bb = child_bb->get_statements().begin(); it_child_bb != child_bb->get_statements().end(); it_child_bb++)
          bb->append_statement(*it_child_bb);
      }
      else
        bb->append_statement(child_res);
    }

    SageBuilder::popScopeStack();
    if_stmt->set_true_body(bb);
  }
  else assert(false);

  SageBuilder::popScopeStack();

  return if_stmt;
}

SgStatement * SPMD_Generator::codeGeneration(SPMD_Root * tree) {
  SgBasicBlock * bb = SageBuilder::buildBasicBlock();
  SageBuilder::pushScopeStack(bb);

  std::vector<SPMD_Tree *> & children = tree->getChildren();
  std::vector<SPMD_Tree *>::iterator it;
  for (it = children.begin(); it != children.end(); it++) {
    SgStatement * child_res = codeGeneration(*it);
    SgBasicBlock * child_bb = isSgBasicBlock(child_res);
    if (child_bb != NULL) {
      std::vector<SgStatement *>::iterator it_child_bb;
      for (it_child_bb = child_bb->get_statements().begin(); it_child_bb != child_bb->get_statements().end(); it_child_bb++)
        bb->append_statement(*it_child_bb);
    }
    else
      bb->append_statement(child_res);
  }

  SageBuilder::popScopeStack();
  return bb;
}

SPMD_Generator::SPMD_Generator(SPMD_Driver & driver_) :
  driver(driver_),
  top_scope(NULL),
  array_aliases()
{}

SPMD_Generator::~SPMD_Generator() {}

SgStatement * SPMD_Generator::generate(
  SgStatement * insert_init_after,
  SgStatement * insert_final_after,
  SgStatement * first,
  SgStatement * last,
  SgScopeStatement * top_scope_, 
  std::string kernel_file_name
) {
  if (top_scope_ == NULL)
    top_scope = SageBuilder::topScopeStack();
  else
    top_scope = top_scope_;

  std::map<ComputeSystem *, std::pair<std::set<ArrayPartition *>, std::set<ArrayPartition *> > > to_be_aliased;
  SPMD_Tree * spmd_tree = driver.generateTree(first, last, to_be_aliased);

  insertInit(spmd_tree, to_be_aliased, insert_init_after, kernel_file_name);
  SgStatement * body = codeGeneration(spmd_tree);
  insertFinal(spmd_tree, insert_final_after);
  generateKernel(spmd_tree, buildKernelFile(kernel_file_name));

  return body;
}

