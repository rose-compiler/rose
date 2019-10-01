/*
 * The goal of this transformation is to remove all short circuiting, to aid in analysis work.
 */

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "sageBuilder.h"
#include <iostream>
#include <analysisUtils.h>

#if ROSE_WITH_LIBHARU
#include "AstPDFGeneration.h"
#endif

#include <shortCircuitingTransformation.h>

using namespace std;

#define SgNULL_FILE Sg_File_Info::generateDefaultFileInfoForTransformationNode()


void MarkSCGenerated(SgNode *n)
   {
     n->setAttribute("SCGenerated", new AstAttribute);
   }

bool IsSCGenerated(const SgNode *n)
   {
     return n->attributeExists("SCGenerated");
   }

template <class Base>
class SCPreservingCopy : public Base
   {
     public:
          SCPreservingCopy(const Base &base = Base()) : Base(base)
          {
          }

          SgNode *copyAst(const SgNode *orig)
             {
               SgNode *copy = Base::copyAst(orig);
               if (IsSCGenerated(orig))
                  {
                    MarkSCGenerated(copy);
                  }
               return copy;
             }

   };

/*
 * See implementation notes for details on the three different types of sc.
 */
inline bool isSCType1(SgNode *n)
   {
     return isSgConditionalExp(n) && !isSgValueExp(n->get_parent());
   }

inline bool isSCType2(SgNode *n)
   {
     return isSgCommaOpExp(n);
   }

bool isSC(SgNode *n)
   {
     if (isSCType1(n))
        {
          return !isSCType1(n->get_parent());
        }
     else if (isSCType2(n))
        {
          Rose_STL_Container<SgNode *> exprs = NodeQuery::querySubTree(n, V_SgExpression);

          for (Rose_STL_Container<SgNode *>::iterator i = exprs.begin(); i != exprs.end(); i++)
             {
               if (*i != n && isSCType1(*i))
                  {
                    return true;
                  }
             }
          return false;
        }
     else
        {
          return false;
        }
   }

bool hasSC(SgNode *er)
   {
     Rose_STL_Container<SgNode *> exprs = NodeQuery::querySubTree(er, V_SgExpression);

     for (Rose_STL_Container<SgNode *>::iterator i = exprs.begin(); i != exprs.end(); i++)
        {
          if (isSC(*i))
             {
               return true;
             }
        }

     return false;
   }

template<typename T>
SgConditionalExp *createEquivalentConditional(T *booleanOp);

template<>
inline SgConditionalExp *createEquivalentConditional(SgAndOp *andOp)
   {
     SgBoolValExp *zero = new SgBoolValExp(SgNULL_FILE, 0);
#if 0
     cout << "zero->get_originalExpressionTree() = " << zero->get_originalExpressionTree() << endl;
#endif
     return new SgConditionalExp(SgNULL_FILE,
                                 andOp->get_lhs_operand(),
                                 andOp->get_rhs_operand(),
                                 zero);
   }

template<>
inline SgConditionalExp *createEquivalentConditional(SgOrOp *orOp)
   {
     SgBoolValExp *one = new SgBoolValExp(SgNULL_FILE, 1);
#if 0
     cout << "one->get_originalExpressionTree() = " << one->get_originalExpressionTree() << endl;
#endif
     return new SgConditionalExp(SgNULL_FILE,
                                 orOp->get_lhs_operand(),
                                 one,
                                 orOp->get_rhs_operand());
   }

template<typename T, VariantT V_T, T *(isT)(SgNode *)>
inline void rewriteConditionals(SgNode *n)
   {
     Rose_STL_Container<SgNode *> booleanOps = NodeQuery::querySubTree(n, V_T);

     for (Rose_STL_Container<SgNode *>::iterator i = booleanOps.begin(); i != booleanOps.end(); i++)
        {
          T *booleanOp = isT(*i);
          ROSE_ASSERT(booleanOp != NULL);
          if (isSgValueExp(booleanOp->get_parent()))
             {
               continue;
             }

          SgExpression *lhs = booleanOp->get_lhs_operand();
          SgExpression *rhs = booleanOp->get_rhs_operand();
          
          SgConditionalExp *condExp = createEquivalentConditional(booleanOp);

          SgExpression *booleanOpParent = isSgExpression(booleanOp->get_parent());
#if 0
          condExp->set_parent(booleanOpParent);
          lhs->set_parent(condExp);
          rhs->set_parent(condExp);
#else
  // DQ (12/16/2006): Modified to reflect expressions attached directly to statements (SgExpressionRoot IR nodes are no longer generated).
          condExp->set_parent(booleanOp->get_parent());
          lhs->set_parent(condExp);
          rhs->set_parent(condExp);
#endif

#if 0
     ROSE_ASSERT(booleanOpParent != NULL);
          cout << "parent is a " << booleanOpParent->class_name() << ", replace_expression= " <<  booleanOpParent->replace_expression(booleanOp, condExp) << endl;
#else
  // DQ (12/16/2006): Need to handle case where parent is not an expression!
     if (booleanOpParent != NULL)
               booleanOpParent->replace_expression(booleanOp, condExp);
       else
        {
               SgStatement *booleanOpParent = isSgStatement(booleanOp->get_parent());
          if (booleanOpParent != NULL)
             {
               booleanOpParent->replace_expression(booleanOp, condExp);
             }
            else
             {
               printf ("Error: booleanOp->get_parent() = %p is unknown \n",booleanOp->get_parent());
               ROSE_ASSERT(false);
             }
        }
#endif

          booleanOp->set_lhs_operand(0); // so they won't be deleted with the booleanOp
          booleanOp->set_rhs_operand(0);
          delete booleanOp;
        }
   }

static SgStatement* replaceContinues(SgStatement *stmt, SgLabelStatement *dest)
   {
     if (isSgContinueStmt(stmt)) {
       SgGotoStatement *gotoStmt = new SgGotoStatement(SgNULL_FILE, dest);
       delete stmt;
       return gotoStmt;
     } else {
       SageInterface::changeContinuesToGotos(stmt, dest);
       return stmt;
     }
   }

// Split a variable declaration into two statements: the variable declaration
// itself and an expression statement setting the variable to the value specified in
// its initializer.  Sets the initializer to null and does not insert the new expression
// statement itself, as it may be used in a different way (e.g. condition of if statement).
// Caveat: for constructor initializers this is not the correct behavior (as
// 0-argument constructor called first) but the best we
// can do if we want the initializer in an expression statement.  
SgExprStatement *splitVarDecl(SgVariableDeclaration *varDecl)
   {
     SgInitializedName *in = varDecl->get_variables().front();

     SgInitializer *init = in->get_initializer();

     SgExpression *operand;
     switch (init->variantT())
        {
          case V_SgAssignInitializer:
                {
                  SgAssignInitializer *assignInit = isSgAssignInitializer(init);
                  ROSE_ASSERT(assignInit);

                  operand = assignInit->get_operand_i();

                  break;
                }

          case V_SgConstructorInitializer:
                {
                  SgConstructorInitializer *ctorInit = isSgConstructorInitializer(init);
                  ROSE_ASSERT(ctorInit != NULL);

                  operand = ctorInit;
                  break;
                }

          default:
                {
                  cout << "splitVarDecl: unknown init type: " << init->class_name() << endl;
                  ROSE_ABORT();
                }
        }

     in->set_initializer(NULL);

     SgVariableSymbol *var = in->get_scope()->lookup_var_symbol(in->get_name());

     SgVarRefExp *varRefForAssign = new SgVarRefExp(SgNULL_FILE, var);
     SgAssignOp *assignOp = new SgAssignOp(SgNULL_FILE, varRefForAssign, operand);
     varRefForAssign->set_parent(assignOp);
     operand->set_parent(assignOp);

     SgExprStatement *condExprStmt = new SgExprStatement(SgNULL_FILE, assignOp);
     assignOp->set_parent(condExprStmt);

     return condExprStmt;
   }

/*
 * Move condition in a loop that tests at the beginning of each iteration (for, while) into
 * the body.  Returns replacement condition
 */
SgStatement *moveConditionToBody(SgStatement *cond, SgBasicBlock *body)
   {
     SgStatement *newCond;

     SgBoolValExp *trueExp = new SgBoolValExp(SgNULL_FILE, 1);

     SgExprStatement *trueStmt = new SgExprStatement(SgNULL_FILE, trueExp);
     trueExp->set_parent(trueStmt);

     newCond = trueStmt;
     trueStmt->set_parent(cond->get_parent());

     SgBasicBlock *trueBody = new SgBasicBlock(SgNULL_FILE);

     SgBreakStmt *breakStmt = new SgBreakStmt(SgNULL_FILE);
     SgBasicBlock *falseBody = new SgBasicBlock(SgNULL_FILE, breakStmt);
     breakStmt->set_parent(falseBody);

     SgIfStmt *ifStmt = new SgIfStmt(SgNULL_FILE, NULL, trueBody, falseBody);
     trueBody->set_parent(ifStmt);
     falseBody->set_parent(ifStmt);
     body->prepend_statement(ifStmt);
     ifStmt->set_parent(body);

     switch (cond->variantT())
        {
          case V_SgExprStatement:
                {
                  ifStmt->set_conditional(cond);
                  cond->set_parent(ifStmt);

                  break;
                }

          case V_SgVariableDeclaration:
                {
                  body->prepend_statement(cond);
                  cond->set_parent(body);

                  SgVariableDeclaration *varDecl = isSgVariableDeclaration(cond);
#if 1
                  SgExprStatement *varAssign = splitVarDecl(varDecl);
                  ifStmt->set_conditional(varAssign);
                  varAssign->set_parent(ifStmt);
#else
                  SgInitializedName *varName = varDecl->get_variables().front();
                  SgVariableSymbol *varSym = varName->get_scope()->lookup_var_symbol(varName->get_name());

                  SgVarRefExp *varRef = new SgVarRefExp(SgNULL_FILE, varSym);

                  SgExprStatement *varStmt = new SgExprStatement(SgNULL_FILE, varRef);
                  varRef->set_parent(varStmt);

                  ifStmt->set_conditional(varStmt);
                  varStmt->set_parent(ifStmt);
#endif

                  break;
                }

          default:
                {
                  ROSE_ASSERT(false);
                }
        }

     MarkSCGenerated(ifStmt);
     return newCond;
   }

void initialTransformation(SgNode *n)
   {
#if 0
     Rose_STL_Container<SgNode *> andOps = NodeQuery::querySubTree(n, V_SgAndOp);

     for (Rose_STL_Container<SgNode *>::iterator i = andOps.begin(); i != andOps.end(); ++i)
        {
          SgAndOp *andOp = isSgAndOp(*i);
          ROSE_ASSERT(andOp);

          SgExpression *lhs = andOp->get_lhs_operand();
          SgExpression *rhs = andOp->get_rhs_operand();
          

          SgConditionalExp *condExp = 0;
          SgExpression *andOpParent = isSgExpression(andOp->get_parent());
          condExp->set_parent(andOpParent);
          lhs->set_parent(condExp);
          rhs->set_parent(condExp);

          andOpParent->replace_expression(andOp, condExp);

          andOp->set_lhs_operand(0); // so they won't be deleted with the andOp
          andOp->set_rhs_operand(0);
          delete andOp;
        }
#endif
     rewriteConditionals<SgAndOp, V_SgAndOp, isSgAndOp>(n);
#if 0
     AstPDFGeneration pdf;
     pdf.generate("shortCircuitingStage1", n);
#endif
     rewriteConditionals<SgOrOp, V_SgOrOp, isSgOrOp>(n);

     Rose_STL_Container<SgNode *> ifStmts = NodeQuery::querySubTree(n, V_SgIfStmt);
     for (Rose_STL_Container<SgNode *>::iterator i = ifStmts.begin(); i != ifStmts.end(); ++i)
        {
          SgIfStmt *ifStmt = isSgIfStmt(*i);
          ROSE_ASSERT(ifStmt != NULL);
          
          SgBasicBlock *ifStmtParent = isSgBasicBlock(ifStmt->get_parent());
          ROSE_ASSERT(ifStmtParent != NULL);

          SgStatementPtrList::iterator ifStmtI = findIterator(ifStmtParent->get_statements(), ifStmt);
          ROSE_ASSERT(ifStmtI != ifStmtParent->get_statements().end());
          unsigned int ifStmtPos = ifStmtI - ifStmtParent->get_statements().begin();

          SgStatement *cond = ifStmt->get_conditional();
          if (hasSC(cond))
             {
               cout << "hasSC(cond)" << endl;
               SgExprStatement *condExprStmt;
               switch (cond->variantT())
                  {
                    case V_SgExprStatement:
                          {
                            condExprStmt = isSgExprStatement(cond);
                            ROSE_ASSERT(condExprStmt != NULL);

                            break;
                          }

                    case V_SgVariableDeclaration:
                          {
                            SgVariableDeclaration *varDecl = isSgVariableDeclaration(cond);
                            ROSE_ASSERT(varDecl != NULL);

                            condExprStmt = splitVarDecl(varDecl);

                            ifStmtParent->get_statements().insert(ifStmtParent->get_statements().begin() + ifStmtPos, varDecl);
                            varDecl->set_parent(ifStmtParent);

                            break;
                          }

                    default:
                          {
                            cout << "initialTransformation (if): unknown cond type " << cond->class_name() << endl;
                            ROSE_ABORT();
                          }
                  }

               static int counter = 0;

               stringstream varNameSS;
               varNameSS << "rose_sc_bool_" << counter++;
               SgName varName = varNameSS.str();

               SgBoolValExp *falseExp = new SgBoolValExp(SgNULL_FILE, 0);

               SgAssignInitializer *falseAssign = new SgAssignInitializer(SgNULL_FILE, falseExp);
               falseExp->set_parent(falseAssign);

               SgInitializedName *in = new SgInitializedName(varName, SgTypeBool::createType(), falseAssign);
               in->set_file_info(SgNULL_FILE);
               in->set_scope(ifStmtParent);
               falseAssign->set_parent(in);

               SgVariableDeclaration *varDecl = new SgVariableDeclaration(SgNULL_FILE);
               varDecl->set_firstNondefiningDeclaration(varDecl);
               varDecl->get_variables().insert(varDecl->get_variables().end(), in);
               in->set_parent(varDecl);

               SgVariableSymbol *varSym = new SgVariableSymbol(in);
               ifStmtParent->insert_symbol(varName, varSym);

               ifStmtParent->get_statements().insert(ifStmtParent->get_statements().begin() + ifStmtPos, varDecl);
               varDecl->set_parent(ifStmtParent);

               SgVarRefExp *varRef = new SgVarRefExp(SgNULL_FILE, varSym);
               SgBoolValExp *trueExp = new SgBoolValExp(SgNULL_FILE, 1);

               SgAssignOp *varAssignOp = new SgAssignOp(SgNULL_FILE, varRef, trueExp);
               varRef->set_parent(varAssignOp);
               trueExp->set_parent(varAssignOp);

               SgExprStatement *varAssignStmt = new SgExprStatement(SgNULL_FILE, varAssignOp);
               varAssignOp->set_parent(varAssignStmt);

               SgBasicBlock *trueBody = new SgBasicBlock(SgNULL_FILE, varAssignStmt);
               varAssignStmt->set_parent(trueBody);
               SgBasicBlock *falseBody = new SgBasicBlock(SgNULL_FILE);

               SgIfStmt *newIfStmt = new SgIfStmt(SgNULL_FILE, condExprStmt, trueBody, falseBody);
               condExprStmt->set_parent(newIfStmt);
               trueBody->set_parent(newIfStmt);
               falseBody->set_parent(newIfStmt);

               varRef = new SgVarRefExp(SgNULL_FILE, varSym);

               SgExprStatement *exprStmt = new SgExprStatement(SgNULL_FILE, varRef);
               varRef->set_parent(exprStmt);

               ifStmt->set_conditional(exprStmt);
               exprStmt->set_parent(ifStmt);

               ifStmtParent->get_statements().insert(ifStmtParent->get_statements().begin() + ifStmtPos, newIfStmt);
               newIfStmt->set_parent(ifStmtParent);
             }
        }

     Rose_STL_Container<SgNode *> whileStmts = NodeQuery::querySubTree(n, V_SgWhileStmt);
     for (Rose_STL_Container<SgNode *>::iterator i = whileStmts.begin(); i != whileStmts.end(); ++i)
        {
          SgWhileStmt *whileStmt = isSgWhileStmt(*i);
          ROSE_ASSERT(whileStmt != NULL);

          SgStatement *cond = whileStmt->get_condition();
          if (hasSC(cond))
             {
               whileStmt->set_condition(
                       moveConditionToBody(whileStmt->get_condition(), SageInterface::ensureBasicBlockAsBodyOfWhile(whileStmt)));
#if 0
               SgBoolValExp *trueExp = new SgBoolValExp(SgNULL_FILE, 1);
               SgExpressionRoot *trueRoot = new SgExpressionRoot(SgNULL_FILE, trueExp);
               trueExp->set_parent(trueRoot);

               SgExprStatement *trueStmt = new SgExprStatement(SgNULL_FILE, trueRoot);
               trueRoot->set_parent(trueStmt);

               whileStmt->set_condition(trueStmt);
               trueStmt->set_parent(whileStmt);
               
               SgBasicBlock *trueBody = new SgBasicBlock(SgNULL_FILE);

               SgBreakStmt *breakStmt = new SgBreakStmt(SgNULL_FILE);
               SgBasicBlock *falseBody = new SgBasicBlock(SgNULL_FILE, breakStmt);
               breakStmt->set_parent(falseBody);

               SgIfStmt *ifStmt = new SgIfStmt(SgNULL_FILE, NULL, trueBody, falseBody);
               trueBody->set_parent(ifStmt);
               falseBody->set_parent(ifStmt);
               whileStmt->get_body()->prepend_statement(ifStmt);
               ifStmt->set_parent(whileStmt->get_body());

               switch (cond->variantT())
                  {
                    case V_SgExprStatement:
                          {
                            ifStmt->set_conditional(cond);
                            cond->set_parent(ifStmt);

                            break;
                          }

                    case V_SgVariableDeclaration:
                          {
                            whileStmt->get_body()->prepend_statement(cond);
                            cond->set_parent(whileStmt->get_body());

                            SgVariableDeclaration *varDecl = isSgVariableDeclaration(cond);
#if 1
                            SgExprStatement *varAssign = splitVarDecl(varDecl);
                            ifStmt->set_conditional(varAssign);
                            varAssign->set_parent(ifStmt);
#else
                            SgInitializedName *varName = varDecl->get_variables().front();
                            SgVariableSymbol *varSym = varName->get_scope()->lookup_var_symbol(varName->get_name());

                            SgVarRefExp *varRef = new SgVarRefExp(SgNULL_FILE, varSym);

                            SgExpressionRoot *varRoot = new SgExpressionRoot(SgNULL_FILE, varRef);
                            varRef->set_parent(varRoot);

                            SgExprStatement *varStmt = new SgExprStatement(SgNULL_FILE, varRoot);
                            varRoot->set_parent(varStmt);

                            ifStmt->set_conditional(varStmt);
                            varStmt->set_parent(ifStmt);
#endif

                            break;
                          }

                    default:
                          {
                            ROSE_ASSERT(false);
                          }
                  }

               MarkSCGenerated(ifStmt);
#endif
             }
        }

     Rose_STL_Container<SgNode *> dowhileStmts = NodeQuery::querySubTree(n, V_SgDoWhileStmt);
     for (Rose_STL_Container<SgNode *>::iterator i = dowhileStmts.begin(); i != dowhileStmts.end(); ++i)
        {
          SgDoWhileStmt *dowhileStmt = isSgDoWhileStmt(*i);
          ROSE_ASSERT(dowhileStmt != NULL);

          SgStatement *cond = dowhileStmt->get_condition();
          if (hasSC(cond))
             {
               SgBoolValExp *trueExp = new SgBoolValExp(SgNULL_FILE, 1);
               SgExprStatement *trueStmt = new SgExprStatement(SgNULL_FILE, trueExp);
               trueExp->set_parent(trueStmt);

               dowhileStmt->set_condition(trueStmt);
               trueStmt->set_parent(dowhileStmt);
               
               static int counter = 0;

               stringstream labelNameSS;
               labelNameSS << "rose_sc_label_" << counter++;
               SgName labelName = labelNameSS.str();

               SgLabelStatement *labelStmt = new SgLabelStatement(SgNULL_FILE, labelName);
               SageInterface::ensureBasicBlockAsBodyOfDoWhile(dowhileStmt)->append_statement(labelStmt);
               labelStmt->set_parent(dowhileStmt->get_body());

               dowhileStmt->set_body(replaceContinues(dowhileStmt->get_body(), labelStmt));
               dowhileStmt->get_body()->set_parent(dowhileStmt);

               SgBasicBlock *trueBody = new SgBasicBlock(SgNULL_FILE);

               SgBreakStmt *breakStmt = new SgBreakStmt(SgNULL_FILE);
               SgBasicBlock *falseBody = new SgBasicBlock(SgNULL_FILE, breakStmt);
               breakStmt->set_parent(falseBody);

               SgIfStmt *ifStmt = new SgIfStmt(SgNULL_FILE, NULL, trueBody, falseBody);
               trueBody->set_parent(ifStmt);
               falseBody->set_parent(ifStmt);
               SageInterface::ensureBasicBlockAsBodyOfDoWhile(dowhileStmt)->append_statement(ifStmt);
               ifStmt->set_parent(dowhileStmt->get_body());

               ifStmt->set_conditional(cond);
               cond->set_parent(ifStmt);
             }
        }

     Rose_STL_Container<SgNode *> forStmts = NodeQuery::querySubTree(n, V_SgForStatement);
     for (Rose_STL_Container<SgNode *>::iterator i = forStmts.begin(); i != forStmts.end(); ++i)
        {
          SgForStatement *forStmt = isSgForStatement(*i);
          ROSE_ASSERT(forStmt != NULL);

          SgForInitStatement *forInit = forStmt->get_for_init_stmt();
          if (hasSC(forInit))
             {
               cerr << "hasSC(forInit))" << endl;
            // If any of the init statements have an sc, we need to
            // transform all of them anyway, to enforce correct
            // execution order
               SgNode *forStmtParent = forStmt->get_parent();
               SgBasicBlock *forStmtParentBB = isSgBasicBlock(forStmtParent);
               ROSE_ASSERT(forStmtParentBB != NULL);

               SgStatementPtrList &forParentStmts = forStmtParentBB->get_statements();
               SgStatementPtrList::iterator forParentStmtsIter = findIterator(forParentStmts, forStmt);

               SgStatementPtrList &initStmts = forInit->get_init_stmt();
               for (SgStatementPtrList::iterator i = initStmts.begin(); i != initStmts.end(); ++i)
                  {
                    SgStatement *initStmt = *i;
                    forStmtParentBB->get_statements().insert(forParentStmtsIter, *i);
                    initStmt->set_parent(forStmtParentBB);
                  }

               initStmts.clear();
             }

          SgStatement *forTest = forStmt->get_test();
          if (hasSC(forTest))
             {
               forStmt->set_test(
                       moveConditionToBody(forTest, SageInterface::ensureBasicBlockAsBodyOfFor(forStmt)));
             }

  // DQ (11/7/2006): modified to reflect removal of SgExpressionRoot IR node
  // SgExpressionRoot *forInc = forStmt->get_increment_expr_root();
          SgExpression *forInc = forStmt->get_increment();
          if (hasSC(forInc))
             {
               SgExprStatement *forIncStmt = new SgExprStatement(SgNULL_FILE, forInc);
               forInc->set_parent(forIncStmt);

               SgBasicBlock *forBody = SageInterface::ensureBasicBlockAsBodyOfFor(forStmt);
               forBody->append_statement(forIncStmt);
               forIncStmt->set_parent(forBody);

               SgNullExpression *blankIncExpr = new SgNullExpression(SgNULL_FILE);

       // DQ (11/7/2006): modified to reflect removal of SgExpressionRoot IR node
       // SgExpressionRoot *blankIncRoot = new SgExpressionRoot(SgNULL_FILE, blankIncExpr);
            // blankIncExpr->set_parent(blankIncRoot);

            // forStmt->set_increment_expr_root(blankIncRoot);
            // blankIncRoot->set_parent(forStmt);
               forStmt->set_increment(blankIncExpr);
               blankIncExpr->set_parent(forStmt);
             }
        }

     Rose_STL_Container<SgNode *> varDeclStmts = NodeQuery::querySubTree(n, V_SgVariableDeclaration);
     for (Rose_STL_Container<SgNode *>::iterator i = varDeclStmts.begin(); i != varDeclStmts.end(); ++i)
        {
          SgVariableDeclaration *varDecl = isSgVariableDeclaration(*i);
          ROSE_ASSERT(varDecl != NULL);

          cout << "in varDecl loop" << varDecl << endl;
          if (hasSC(varDecl))
             {
               cout << "doing varDecl" << endl;
               SgBasicBlock *varDeclParent = isSgBasicBlock(varDecl->get_parent());
               ROSE_ASSERT(varDeclParent);

               SgExprStatement *varAssign = splitVarDecl(varDecl);
               varDeclParent->get_statements().insert(++findIterator(varDeclParent->get_statements(), varDecl), varAssign);
               varAssign->set_parent(varDeclParent);
             }
        }
   }

#if 0
inline bool isApplicableSC(SgNode *n)
   {
     if (!isSC(n))
        {
          return false;
        }
     while ((n = n->get_parent()) != NULL)
        {
          if (isSC(n))
             {
               return false;
             }
        }
     return true;
   }

/*
 * Find applicable short-circuiting expressions beneath the node node.
 */
Rose_STL_Container<SgExpression *> findApplicableSCs(SgNode *node)
   {
     Rose_STL_Container<SgNode *> exprs = NodeQuery::querySubTree(node, V_SgExpression);
     Rose_STL_Container<SgExpression *> scs;

     for (Rose_STL_Container<SgNode *>::iterator i = exprs.begin(); i != exprs.end(); i++)
        {
          if (isApplicableSC(*i))
             {
               scs.push_back(isSgExpression(*i));
             }
        }
     return scs;
   }
#else

// Find applicable SCs more efficiently, by only searching the relevant parts of the tree.

static void doFindApplicableSCs(SgNode *node, Rose_STL_Container<SgExpression *> &appSCs)
   {
     vector<SgNode *> subnodes = node->get_traversalSuccessorContainer();
     for (vector<SgNode *>::iterator i = subnodes.begin(); i != subnodes.end(); ++i)
        {
          if (*i == NULL)
             {
               continue;
             }
          SgExpression *expr = isSgExpression(*i);
          if (expr != NULL && isSC(expr))
             {
               appSCs.push_back(expr);
             }
          else
             {
               doFindApplicableSCs(*i, appSCs);
             }
        }
   }

Rose_STL_Container<SgExpression *> findApplicableSCs(SgNode *node)
   {
     SgExpressionPtrList scList;
     doFindApplicableSCs(node, scList);
     return scList;
   }

#endif

// Perform an optimization where if (constant) { ... } else { ... } is replaced with { ... }
// This function takes what might be the root expression of an if statement, and only does
// the optimization if this is the case.  The function is designed this way since it is
// passed the expression that is the result of reducing a conditional expression (which may
// appear anywhere).
void ifConstOptimization(SgExpression *expr)
   {
     if (SgBoolValExp *boolExpr = isSgBoolValExp(expr))
        {
          if (SgExpression *boolExprRoot = boolExpr) // JJW removed use of SgExpressionRoot here
             {
               if (SgExprStatement *boolExprStmt = isSgExprStatement(boolExprRoot->get_parent()))
                  {
                    if (SgIfStmt *constIfStmt = isSgIfStmt(boolExprStmt->get_parent()))
                       {
                         SgBasicBlock *constIfStmtParent = isSgBasicBlock(constIfStmt->get_parent());
                         ROSE_ASSERT(constIfStmtParent != NULL);

                         SgStatement *replacementStmt = constIfStmt,
                                     *replacementStmtParent = constIfStmtParent;

#if 1
                              cout << "replacementStmt is now " << replacementStmt << " " << replacementStmt->class_name() << endl;
                              cout << "replacementStmtParent is now " << replacementStmtParent << " " << replacementStmtParent->class_name() << endl;
                         while (isSgBasicBlock(replacementStmtParent)
                             && isSgBasicBlock(replacementStmtParent)->get_statements().size() == 1
                             && isSgStatement(replacementStmtParent->get_parent()))
                            {
                              cout << "replacementStmt was " << replacementStmt << " " << replacementStmt->class_name() << endl;
                              replacementStmt = replacementStmtParent;
                              cout << "replacementStmt is now " << replacementStmt << " " << replacementStmt->class_name() << endl;
                              cout << "replacementStmtParent was " << replacementStmtParent << " " << replacementStmtParent->class_name() << endl;
                              replacementStmtParent = isSgStatement(replacementStmtParent->get_parent());
                              cout << "replacementStmtParent is now " << replacementStmtParent << " " << replacementStmtParent->class_name() << endl;
                            }
                             
#else                    
                         if (constIfStmtParent->get_statements().size() == 1
                                 && isSgStatement(constIfStmtParent->get_parent()))
                            {
                              replacementStmt = constIfStmtParent;
                              replacementStmtParent = isSgStatement(replacementStmt->get_parent());
                            }
                         else
                            {
                              replacementStmt = constIfStmt;
                              replacementStmtParent = constIfStmtParent;
                            }
#endif

                         if (boolExpr->get_value() != 0)
                            {
                           // true branch
                              replacementStmtParent->replace_statement(replacementStmt, constIfStmt->get_true_body());
                              delete constIfStmt->get_false_body();
                            }
                         else
                            {
                           // false branch
                              SgStatement* falseBranch = constIfStmt->get_false_body();
                              if (!falseBranch) falseBranch = SageBuilder::buildBasicBlock();
                              replacementStmtParent->replace_statement(replacementStmt, falseBranch);
                              delete constIfStmt->get_true_body();
                            }
                       }
                  }
             }
        }
   }
                         
bool reduceIfStmtsWithSCchild(SgProject *prj)
   {
     bool retVal = false;
     Rose_STL_Container<SgExpression *> appSCs = findApplicableSCs(prj);
     static int callCount = 0;
     callCount++;
     
     for (Rose_STL_Container<SgExpression *>::iterator i = appSCs.begin(); i != appSCs.end(); ++i)
        {
          SgExpression *expr = *i;
          cout << "reduceIfStmtsWithSCchild: doing " << expr << " iter " << callCount << endl;
          SgStatement *stmt = findStatementForExpression(expr);
          pair<SgBasicBlock *, SgNode *> bbPair = findBasicBlockForStmt(stmt); 
          SgBasicBlock *basicBlock = bbPair.first;
          SgStatement *basicBlockChild = isSgStatement(bbPair.second);
          ROSE_ASSERT(basicBlockChild != NULL);

          retVal = true;

          SgStatement *fullStmt;

          switch (basicBlockChild->variantT())
             {
               case V_SgExprStatement:
                     {
                       if (IsSCGenerated(basicBlockChild))
                          {
                            fullStmt = basicBlock;
                          }
                       else
                          {
                            fullStmt = basicBlockChild;
                          }
                       break;
                     }

               case V_SgIfStmt:
               case V_SgReturnStmt:
                     {
                       fullStmt = basicBlockChild;
                       break;
                     }

               default:
                     {

#if ROSE_WITH_LIBHARU
                       AstPDFGeneration().generate("error", prj);
#else
                       cout << "Warning: libharu support is not enabled" << endl;
#endif
                       cerr << "Error: reduceIfStmtsWithSCchild: basicBlockChild has unknown type " << basicBlockChild->class_name() << basicBlockChild << endl;
                       ROSE_ASSERT(false);
                     }
             }

          switch (expr->variantT())
             {
               case V_SgCommaOpExp:
                     {
                       SgCommaOpExp *commaOpExp = isSgCommaOpExp(expr);
                       ROSE_ASSERT(commaOpExp != NULL);

                       SgExpression *dummyExpr = commaOpExp->get_lhs_operand();

                       SgExprStatement *dummyExprStmt = new SgExprStatement(SgNULL_FILE, dummyExpr);
                       dummyExpr->set_parent(dummyExprStmt);

                       MarkSCGenerated(dummyExprStmt);

                       SgExpression *commaOpExpParent = isSgExpression(commaOpExp->get_parent());
                       ROSE_ASSERT(commaOpExpParent != NULL);

                       SgExpression *rhsExpr = commaOpExp->get_rhs_operand();
                       commaOpExpParent->replace_expression(commaOpExp, rhsExpr);
                       rhsExpr->set_parent(commaOpExpParent);

                       commaOpExp->set_lhs_operand(NULL);
                       commaOpExp->set_rhs_operand(NULL);
                       delete commaOpExp;

                       SgBasicBlock *newBB = new SgBasicBlock(SgNULL_FILE, dummyExprStmt);
                       dummyExprStmt->set_parent(newBB);

                       newBB->set_parent(fullStmt->get_parent());
                       isSgStatement(fullStmt->get_parent())->replace_statement(fullStmt, newBB);
                       newBB->append_statement(fullStmt);
                       fullStmt->set_parent(newBB);

                       break;
                     }

               case V_SgConditionalExp:
                     {
                       SgConditionalExp *condExp = isSgConditionalExp(expr);
                       ROSE_ASSERT(condExp != NULL);

                       SgStatement *fullStmtParent = isSgStatement(fullStmt->get_parent());
                       ROSE_ASSERT(fullStmtParent != NULL);

                    // Deduce a suitable block for attaching to the if statement
                       SgBasicBlock *fullStmtBlock;
                       if (SgBasicBlock *fullStmtBB = isSgBasicBlock(fullStmt))
                          {
                            fullStmtBlock = fullStmtBB;
                          }
                       else
                          {
                            fullStmtBlock = new SgBasicBlock(SgNULL_FILE, fullStmt);
                            //fullStmt->set_parent(fullStmtBlock);
                          }

                    // Build the true branch of the if statement into fullStmtBlockCopy
                    // where the conditional is replaced with its true clause
                       SgExpression *trueExp = condExp->get_true_exp();

                       SCPreservingCopy<SgCapturingCopy<SgTreeCopy> >
                               ctc(vector<SgNode *>(1, condExp));
                       SgNode *fullStmtBlockCopyNode = fullStmtBlock->copy(ctc);
                       SgBasicBlock *fullStmtBlockCopy = isSgBasicBlock(fullStmtBlockCopyNode);
                       ROSE_ASSERT(fullStmtBlockCopy != NULL);

                       SgNode *condCopyNode = ctc.get_copyList()[0];
                       SgExpression *condCopy = isSgExpression(condCopyNode);
                       ROSE_ASSERT(condCopy != NULL);
                       
                       SgNode *condCopyParentNode = condCopy->get_parent();
                       SgExpression *condCopyParent = isSgExpression(condCopyParentNode);

          // DQ (12/16/2006): Need to handle separate case of where the parent is a SgExpression 
          // or a SgStatement (might could be a SgInitializedName, which is not handled yet).
                    // ROSE_ASSERT(condCopyParent != NULL);
                    // condCopyParent->replace_expression(condCopy, trueExp);
                    // delete condCopy;
                    // trueExp->set_parent(condCopyParent);
             if (condCopyParent != NULL)
                {
                  condCopyParent->replace_expression(condCopy, trueExp);
                  delete condCopy;
                  trueExp->set_parent(condCopyParent);
                }
               else
                {
                  SgStatement *condCopyParent = isSgStatement(condCopyParentNode);
                  if (condCopyParent != NULL)
                     {
                       condCopyParent->replace_expression(condCopy, trueExp);
                       delete condCopy;
                       trueExp->set_parent(condCopyParent);
                     }
                    else
                     {
                       printf ("What is this condCopyParentNode = %p \n",condCopyParentNode);
                       ROSE_ASSERT(false);
                     }
                }
                       
#ifdef SCDBG
                       static int optCounter = 0;

                       optCounter++;
                       cout << "optCounter = " << optCounter << endl;

                       stringstream beforeNameSS;
                       beforeNameSS << "beforeIfConst" << optCounter;
#if ROSE_WITH_LIBHARU
                       AstPDFGeneration().generate(beforeNameSS.str(), prj);
#else
                       cout << "Warning: libharu support is not enabled" << endl;
#endif
#endif
                       
                       ifConstOptimization(trueExp);
#ifdef SCDBG
                       stringstream afterNameSS;
                       afterNameSS << "afterIfConst" << optCounter;
#if ROSE_WITH_LIBHARU
                       AstPDFGeneration().generate(afterNameSS.str(), prj);
#else
                       cout << "Warning: libharu support is not enabled" << endl;
#endif
#endif

                    // Build the false branch of the if statement into fullStmtBlock
                    // (reusing the current tree)
                    // where the conditional is replaced with its false clause
                       SgExpression *falseExp = condExp->get_false_exp();

                       SgNode *condParent = condExp->get_parent();
                       SgExpression *condParentExp = isSgExpression(condParent);

          // DQ (12/16/2006): Need to handle separate case of where the parent is a SgExpression 
          // or a SgStatement (might could be a SgInitializedName, which is not handled yet).
          // ROSE_ASSERT(condParentExp != NULL);
          // condParentExp->replace_expression(condExp, falseExp);

             if (condParentExp != NULL)
                {
                  condParentExp->replace_expression(condExp, falseExp);
                }
               else
                {
                  SgStatement *condParentStmt = isSgStatement(condParent);
                  if (condParentStmt != NULL)
                     {
                       condParentStmt->replace_expression(condExp, falseExp);
                     }
                    else
                     {
                       printf ("What is this condParent = %p \n",condParent);
                       ROSE_ASSERT(false);
                     }
                }

#ifdef SCDBG
                       optCounter++;
                       cout << "optCounter = " << optCounter << endl;

                       stringstream beforeNameSS2;
                       beforeNameSS2 << "beforeIfConst" << optCounter;
#if ROSE_WITH_LIBHARU
                       AstPDFGeneration().generate(beforeNameSS2.str(), prj);
#else
                       cout << "Warning: libharu support is not enabled" << endl;
#endif
#endif
                       ifConstOptimization(falseExp);
#ifdef SCDBG
                       stringstream afterNameSS2;
                       afterNameSS2 << "afterIfConst" << optCounter;
#if ROSE_WITH_LIBHARU
                       AstPDFGeneration().generate(afterNameSS2.str(), prj);
#else
                       cout << "Warning: libharu support is not enabled" << endl;
#endif
#endif

                    // done creating branches, now create if statement

                       SgExpression *conditionExp = condExp->get_conditional_exp();

                       SgStatement *replacementStmt;
                    // optimisation in case the condition is true
                       if (SgBoolValExp *conditionBoolVal = isSgBoolValExp(conditionExp))
                          {
                            if (conditionBoolVal->get_value() != 0)
                               {
                              // true branch
                                 replacementStmt = fullStmtBlockCopy;
                                 delete fullStmtBlock;
                               }
                            else
                               {
                              // false branch
                                 replacementStmt = fullStmtBlock;
                                 delete fullStmtBlockCopy;
                               }
                          }
                       else
                          {
                            SgExprStatement *conditionExpStmt = new SgExprStatement(SgNULL_FILE, conditionExp);
                            conditionExp->set_parent(conditionExpStmt);

                            SgIfStmt *ifStmt = new SgIfStmt(SgNULL_FILE, conditionExpStmt, fullStmtBlockCopy, fullStmtBlock);
                            conditionExpStmt->set_parent(ifStmt); 
                            fullStmtBlockCopy->set_parent(ifStmt); 
                            fullStmtBlock->set_parent(ifStmt); 
                            MarkSCGenerated(ifStmt);

                         // if the parent statement is an if statement, then it must have an
                         // SgBasicBlock which contains the if statement just created as a child
                            if (isSgIfStmt(fullStmtParent))
                               {
                                 replacementStmt = new SgBasicBlock(SgNULL_FILE, ifStmt);
                                 ifStmt->set_parent(replacementStmt);
                               }
                            else
                               {
                                 replacementStmt = ifStmt;
                               }
                          }

                       // XXX : this is a kludge
                       SgNode *fullStmtParentTmp = fullStmt->get_parent();
                       fullStmt->set_parent(fullStmtParent);
                       fullStmtParent->replace_statement(fullStmt, replacementStmt);
                       fullStmt->set_parent(fullStmtParentTmp);

                       replacementStmt->set_parent(fullStmtParent);

                       break;
                     }

               default:
                     {
                       puts("whoops");
                       ROSE_ABORT();
                     }
             }
        }
     return retVal;
   }

void shortCircuitingTransformation(SgProject *prj)
   {
     initialTransformation(prj);

#ifdef SCDBG
     int pass = 0;
     do {
          stringstream ss;
          ss << "scdbg_p" << pass++;
#if ROSE_WITH_LIBHARU
          AstPDFGeneration().generate(ss.str(), prj);
#else
          cout << "Warning: libharu support is not enabled" << endl;
#endif
          AstTests::runAllTests(prj);
          ss << ".C";
          ofstream f(ss.str().c_str());
          f << prj->unparseToCompleteString();
     }
     while (reduceIfStmtsWithSCchild(prj));
#endif
   }

#ifdef SCDBG
int main(int argc, char **argv)
   {
     SgProject *prj = frontend(argc, argv);
     shortCircuitingTransformation(prj);

     AstTests::runAllTests(prj);

     //return backend(prj);
     prj->unparse();
   }
#endif
