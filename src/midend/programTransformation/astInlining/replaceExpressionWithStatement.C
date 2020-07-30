// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

#include "sageBuilder.h"
#include "astPostProcessing.h"
#include "rewrite.h"
#include <iostream>
#include <iomanip>
#include <vector>

// DQ (8/1/2005): test use of new static function to create 
// Sg_File_Info object that are marked as transformations
#undef SgNULL_FILE
#define SgNULL_FILE Sg_File_Info::generateDefaultFileInfoForTransformationNode()

#include "replaceExpressionWithStatement.h"
#include "inlinerSupport.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;
using namespace SageInterface;
// void FixSgTree(SgNode*);

// int gensym_counter = 0; // moved into sageInterface

// Replace an assignment statement with the result of a statement
// generator.
// Assumptions: from has assignment expr at top level of statement
//              from is not in the test of a loop or if statement
//              not currently traversing from
void replaceAssignmentStmtWithStatement(SgExprStatement* from,
                                        StatementGenerator* to) {
  assert(from);
  assert(to);
// SgAssignOp* assignment = isSgAssignOp(from->get_expression_root()->get_operand_i());
  SgAssignOp* assignment = isSgAssignOp(from->get_expression());
  assert (assignment);
  SgVarRefExp* var = isSgVarRefExp(assignment->get_lhs_operand());
  assert (var);
  SgStatement* replacement = to->generate(var);
  // replacement->set_parent(from->get_parent());
  myStatementInsert(from, replacement, false);
  SageInterface::myRemoveStatement(from);
}

// Get the expression root of an expression.  Needs to be moved.
SgExpression* getRootOfExpression(SgExpression* n)
   {
     ROSE_ASSERT(n);
#if 0
     std::cout << "Starting getRootOfExpression on 0x" << std::hex << (int)n << ", which has type " << n->sage_class_name() << std::endl;
#endif

     SgNode* parent = n->get_parent();
     while (isSgExpression(parent) != NULL)
        {
       // cout << "n is " << n->sage_class_name() << ": " << n->unparseToString() << endl;
          n = isSgExpression(parent);
          parent = n->get_parent();
#if 0
          std::cout << "Continuing getRootOfExpression on 0x" << std::hex << (int)n;
          if (n) std::cout << ", which has type " << n->sage_class_name();
          std::cout << std::endl;
#endif
          ROSE_ASSERT(n != NULL);
       }

     return n;
   }

// Get the statement of an expression.  Somewhat smarter than the standard
// version of this routine, but needs to be merged.
SgStatement* getStatementOfExpression(SgNode* n)
   {
     assert(n);
  // std::cout << "Starting getStatementOfExpression on 0x" << std::hex << (int)n << ", which has type " << n->sage_class_name() << std::endl;

     SgExpression* expression = isSgExpression(n);
     ROSE_ASSERT(expression != NULL);
     SgExpression* rootExpression = getRootOfExpression(expression);
     ROSE_ASSERT(rootExpression->get_parent() != NULL);
     SgInitializedName* initializedName = isSgInitializedName(rootExpression->get_parent());

     SgNode* root = rootExpression;
     if (initializedName != NULL)
        root = initializedName;

     ROSE_ASSERT(root != NULL);
     SgStatement* statement = isSgStatement(root->get_parent());
     ROSE_ASSERT(statement != NULL);
     return statement;
   }

// Get the initializer containing an expression if it is within an
// initializer.
SgInitializer* getInitializerOfExpression(SgExpression* n) {
  assert(n);
#if 0
  std::cout << "Starting getInitializerOfExpression on 0x" << std::hex << (int)n << ", which has type " << n->sage_class_name() << std::endl;
#endif
  while (!isSgInitializer(n)) {
    n = isSgExpression(n->get_parent());
#if 0
    std::cout << "Continuing getInitializerOfExpression on 0x" << std::hex << (int)n;
    if (n) std::cout << ", which has type " << n->sage_class_name();
    std::cout << std::endl;
#endif
    assert(n);
  }
  return isSgInitializer(n);
}

// Insert a new statement before or after a target statement.  If
// allowForInit is true, the new statement can be inserted into the
// initializer of a for statement.
// Needs to be merged
void myStatementInsert ( SgStatement* target, SgStatement* newstmt, bool before, bool allowForInit )
   {
     ROSE_ASSERT(target != NULL);
     ROSE_ASSERT(newstmt != NULL);
#if 0
     printf ("In inlining: myStatementInsert(): newstmt = %p = %s \n",newstmt,newstmt->class_name().c_str());
     printf ("In inlining: myStatementInsert(): target = %p = %s \n",target,target->class_name().c_str());
#endif
     SgStatement* parent = isSgStatement(target->get_parent());
#if 0
     if (parent == NULL)
        {
          ROSE_ASSERT(target->get_file_info() != NULL);
          target->get_file_info()->display("problem IR node: debug");

          if (target != NULL)
             {
            // printf ("In inlining: myStatementInsert(): target->get_parent() = %p = %s \n",target->get_parent(),target->get_parent()->class_name().c_str());
               printf ("In inlining: myStatementInsert(): target->get_parent() = %p \n",target->get_parent());
             }
        }
#endif
  // cerr << "1: target is a " << target->sage_class_name() << ", target->get_parent() is a " << target->get_parent()->sage_class_name() << endl;
     if (isSgIfStmt(parent) && isSgIfStmt(parent)->get_conditional() == target)
        {
          target = parent;
          parent = isSgScopeStatement(target->get_parent());
        }

  // printf ("allowForInit = %s \n",allowForInit ? "true" : "false");
     if (isSgForInitStatement(target->get_parent()) && !allowForInit)
        {
          target = isSgScopeStatement(target->get_parent()->get_parent());
          parent = isSgScopeStatement(target->get_parent());
          assert (target);
        }

     if (isSgSwitchStatement(target->get_parent()) && target == isSgSwitchStatement(target->get_parent())->get_item_selector()) {
       target = isSgScopeStatement(target->get_parent()->get_parent());
       parent = isSgScopeStatement(target->get_parent());
       assert (target);
     }

     ROSE_ASSERT(target != NULL);
#if 0
  // DQ (8/1/2005): This fails because the parent at some point is not set and the unparseToString detects this (likely in qualifier generation)
     cerr << "2: target is a " << target->sage_class_name() << ", target->get_parent() is a " << target->get_parent()->sage_class_name() << endl;
     ROSE_ASSERT(parent != NULL);
     if (parent->get_parent() == NULL)
        {
          printf ("Found null parent of %p = %s \n",parent,parent->class_name().c_str());
        }
     ROSE_ASSERT(parent->get_parent() != NULL);
     cerr << "2: parent is a " << parent->sage_class_name() << ", parent->get_parent() is a " << parent->get_parent()->sage_class_name() << endl;
  // cerr << "2: target is " << target->unparseToString() << ", target->get_parent() is " << target->get_parent()->unparseToString() << endl;
#endif
     ROSE_ASSERT (parent);
     SgStatementPtrList* siblings_ptr;
     if (isSgForInitStatement(target->get_parent()))
        {
          siblings_ptr = &isSgForInitStatement(target->get_parent())->get_init_stmt();
        }
       else
        {
          assert (parent);
          if (isSgScopeStatement(parent))
             {
               ROSE_ASSERT(parent != NULL);
               siblings_ptr = &isSgScopeStatement(parent)->getStatementList();
               parent = isSgStatement(target->get_parent()); // getStatementList might have changed it when parent was a loop or something similar
               ROSE_ASSERT (parent);
             }
            else
             {
               assert (!"Bad parent type");
             }
        }

     ROSE_ASSERT(siblings_ptr != NULL);
     ROSE_ASSERT(target != NULL);

     SgStatementPtrList& siblings = *siblings_ptr;
     SgStatementPtrList::iterator stmt_iter = std::find(siblings.begin(), siblings.end(), target);
     ROSE_ASSERT (stmt_iter != siblings.end());

     if (!before)
          ++stmt_iter;

     newstmt->set_parent(parent);
     siblings.insert(stmt_iter, newstmt);
   }

// Replace the expression "from" with another expression "to", wherever it
// appears in the AST.  The expression "from" is not deleted, and may be
// reused elsewhere in the AST.
void replaceExpressionWithExpression(SgExpression* from, SgExpression* to) {
  SgNode* fromparent = from->get_parent();

  to->set_parent(fromparent);
  if (isSgExprStatement(fromparent)) {
    isSgExprStatement(fromparent)->set_expression(to);
  } else if (isSgReturnStmt(fromparent)) {
    isSgReturnStmt(fromparent)->set_expression(to);
  } else if (isSgDoWhileStmt(fromparent)) {
    ROSE_ASSERT (!"FIXME -- this case is present for when the test of a do-while statement is changed to an expression rather than a statement");
  } else if (isSgForStatement(fromparent)) {
    ROSE_ASSERT (isSgForStatement(fromparent)->get_increment() == from);
    isSgForStatement(fromparent)->set_increment(to);
  } else if (isSgExpression(fromparent)) {
    // std::cout << "Unparsed: " << fromparent->sage_class_name() << " --- " << from->unparseToString() << std::endl;
    // std::cout << "Unparsed 2: " << varref->sage_class_name() << " --- " << varref->unparseToString() << std::endl;
    int worked = isSgExpression(fromparent)->replace_expression(from, to);
    ROSE_ASSERT (worked);
  } else {
    ROSE_ASSERT (!"Parent of expression is an unhandled case");
  }
}
#if 0 // moved to sageInterface
SgAssignInitializer* splitExpression(SgExpression* from, string newName) {
#endif

// Convert something like "int a = foo();" into "int a; a = foo();"
SgAssignOp* convertInitializerIntoAssignment(SgAssignInitializer* init)
   {
#ifndef CXX_IS_ROSE_CODE_GENERATION
     using namespace SageBuilder;
     // Liao 2020/7/23: this function is not accurate for template classes with specialization
     //assert (SageInterface::isDefaultConstructible(init->get_operand_i()->get_type())); 
     SgStatement* stmt = getStatementOfExpression(init);
     assert (stmt);
     SgScopeStatement* parent = isSgScopeStatement(stmt->get_parent());
     if (!parent && isSgForInitStatement(stmt->get_parent()))
          parent = isSgScopeStatement(stmt->get_parent()->get_parent()->get_parent());
     assert (parent);
     SgNode* initparent = init->get_parent();
     assert (initparent);

     SgInitializedName* initname = NULL;
     if (isSgInitializedName(initparent))
          initname = isSgInitializedName(initparent);
       else
          if (isSgVariableDefinition(initparent))
               initname = isSgVariableDefinition(initparent)->get_vardefn();
            else
               if (isSgVariableDeclaration(initparent))
                  {
                    SgInitializedNamePtrList& vars = isSgVariableDeclaration(initparent)->get_variables();
                    for (SgInitializedNamePtrList::iterator i = vars.begin(); i != vars.end(); ++i)
                       {
                         if ((*i)->get_initializer() == init)
                            {
                              initname = *i;
                              break;
                            }
                       }
                  }
                 else
                  {
                    std::cout << "initparent is a " << initparent->sage_class_name() << std::endl;
                    assert (!"Should not happen");
                  }

     assert (initname);
     assert (initname->get_initializer() == init);
     assert (parent);
     SgSymbol* sym = initname->get_symbol_from_symbol_table();
     ROSE_ASSERT (isSgVariableSymbol(sym));
     SgVarRefExp* vr = buildVarRefExp(isSgVariableSymbol(sym));
     vr->set_lvalue(true);
     SgExprStatement* assign_stmt = buildAssignStatement(vr, init->get_operand());

     initname->set_initializer(NULL);

  // assignment->set_parent(assign_stmt);
  // cout << "stmt is " << stmt->unparseToString() << endl;
  // cout << "stmt->get_parent() is a " << stmt->get_parent()->sage_class_name() << endl;

     myStatementInsert(stmt, assign_stmt, false);
     assign_stmt->set_parent(parent);

  // FixSgTree(assign_stmt);
  // FixSgTree(parent);

  // AstPostProcessing(assign_stmt);
     return isSgAssignOp(assign_stmt->get_expression());
#else
     return NULL;
#endif
   }

// Rewrites a while or for loop so that the official test is changed to
// "true" and what had previously been the test is now an if-break
// combination (with an inverted condition) at the beginning of the loop
// body
//
// For example, "while (a < 5) ++a;" becomes:
// "while (true) {bool temp = (a < 5); if (!temp) break; ++a;}"
// "for (init; test; step) body;" becomes:
// "for (init; true; step) {bool temp = test; if (!temp) break; body;}"
// "do body; while (test);" becomes:
// "do {body; bool temp = test; if (!temp) break;} while (true);"
void pushTestIntoBody(SgScopeStatement* loopStmt) {
#ifndef CXX_IS_ROSE_CODE_GENERATION
  using namespace SageBuilder;
  AstPostProcessing(loopStmt);
  SgBasicBlock* new_body = buildBasicBlock();
  SgStatement* old_body = SageInterface::getLoopBody(loopStmt);
  SageInterface::setLoopBody(loopStmt, new_body);
  new_body->set_parent(loopStmt);
  AstPostProcessing(loopStmt);
  SgStatement* cond = SageInterface::getLoopCondition(loopStmt);
  ROSE_ASSERT (isSgExprStatement(cond));
  SgExpression* root = isSgExprStatement(cond)->get_expression();
  SgCastExp* cast = buildCastExp(root, SageInterface::getBoolType(loopStmt));
  // Name does not need to be unique, but must not be used in user code anywhere
  AstPostProcessing(loopStmt);
  SgVariableDeclaration* new_decl =
    buildVariableDeclaration("rose__temp",
                             SageInterface::getBoolType(loopStmt),
                             buildAssignInitializer(cast),
                             new_body);
  SgVariableSymbol* varsym = SageInterface::getFirstVarSym(new_decl);
  SageInterface::appendStatement(new_decl, new_body);
  AstPostProcessing(loopStmt);
  SgIfStmt* loop_break =
    buildIfStmt(buildExprStatement(
                  buildNotOp(buildVarRefExp(varsym))),
                buildBasicBlock(buildBreakStmt()),
                buildBasicBlock());
  SageInterface::appendStatement(loop_break, new_body);
  AstPostProcessing(loopStmt);
  if (isSgDoWhileStmt(loopStmt)) {
    SgName label = "rose_test_label__";
    static unsigned int gensym_counter = 0;
    label << ++gensym_counter;
    SgLabelStatement* ls = buildLabelStatement(label, buildBasicBlock(), SageInterface::getEnclosingProcedure(loopStmt));
    SageInterface::changeContinuesToGotos(old_body, ls);
    // Note that these two insertions are backwards of the resulting statement order
    SageInterface::prependStatement(ls, new_body);
    SageInterface::prependStatement(old_body, new_body);
  } else {
    SageInterface::appendStatement(old_body, new_body);
  }
  AstPostProcessing(loopStmt);
  SageInterface::setLoopCondition(loopStmt, buildExprStatement(buildBoolValExp(true)));
  AstPostProcessing(loopStmt);
#endif
}

