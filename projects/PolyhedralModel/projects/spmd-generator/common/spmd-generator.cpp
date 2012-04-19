
#include "common/spmd-generator.hpp"
#include "common/spmd-driver.hpp"
#include "common/spmd-tree.hpp"
#include "common/comm-analysis.hpp"
#include "common/array-analysis.hpp"

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

  if (nat != NULL) codeGeneration(nat);
  if (loop != NULL) codeGeneration(loop);
  if (dr != NULL) codeGeneration(dr);
  if (kc != NULL) codeGeneration(kc);
  if (comm != NULL) codeGeneration(comm);
  if (sync != NULL) codeGeneration(sync);
  if (root != NULL) codeGeneration(root);
}

SgStatement * SPMD_Generator::codeGeneration(SPMD_NativeStmt * tree) {
  return tree->getStatement();
}

SgStatement * SPMD_Generator::codeGeneration(SPMD_Loop * tree) {
  SgForStatement * for_stmt = NULL;

  RoseVariable & iterator = tree->getIterator();
  Bounds * bounds = tree->getBounds();
  int increment = tree->getIncrement();


  SgExprStatement * lb_stmt = bounds->genInit();
  SgExprStatement * ub_stmt = bounds->genTest();
  SgExpression * inc = bounds->genIncrement();

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
    for (it = children.begin(); it != children.end(); it++)
      bb->append_statement(codeGeneration(*it));

    SageBuilder::popScopeStack();
    for_stmt->set_loop_body(bb);
  }
  else assert(false);

  SageBuilder::popScopeStack();

  return for_stmt;
}

SgStatement * SPMD_Generator::codeGeneration(SPMD_DomainRestriction * tree) {
  SgIfStmt * if_stmt = NULL;
  
  std::vector<Expression *> & restriction = tree->getRestriction();

  std::vector<SgExpression *> sg_restriction;
  std::vector<Expression *>::iterator it;
  for (it = restriction.begin(); it != restriction.end(); it++)
    sg_restriction.push_back((*it)->genGreaterOrEqualToZero());

  SgExpression * condition = genAnd(sg_restriction);

  if_stmt = SageBuilder::buildIfStmt(condition, NULL, NULL);

  SageBuilder::pushScopeStack(if_stmt);

  std::vector<SPMD_Tree *> & children = tree->getChildren();
  if (children.size() == 1) {
    if_stmt->set_true_body(codeGeneration(children[0]));
  }
  else if (children.size() > 1) {
    SgBasicBlock * bb = SageBuilder::buildBasicBlock();
    SageBuilder::pushScopeStack(bb);

    std::vector<SPMD_Tree *>::iterator it;
    for (it = children.begin(); it != children.end(); it++)
      bb->append_statement(codeGeneration(*it));

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
  for (it = children.begin(); it != children.end(); it++)
    bb->append_statement(codeGeneration(*it));

  SageBuilder::popScopeStack();
  return bb;
}

SPMD_Generator::SPMD_Generator(SPMD_Driver & driver_) :
  driver(driver_)
{}

SgStatement * SPMD_Generator::generate(
  SgStatement * insert_init_after,
  SgStatement * insert_final_after,
  SgStatement * first,
  SgStatement * last,
  std::string filename_for_kernels
) {
  std::set<ArrayPartition *> init_comm;
  std::set<ArrayPartition *> final_comm;
  SPMD_Tree * spmd_tree = driver.generateTree(first, last, init_comm, final_comm);

  SgStatement * body = codeGeneration(spmd_tree);
  insertInit(spmd_tree, init_comm, insert_init_after);
  insertFinal(spmd_tree, final_comm, insert_final_after);
  generateKernel(spmd_tree, filename_for_kernels);
}

