// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include <analysisUtils.h>
#include <shortCircuitingTransformation.h>

using namespace std;

stmt_pos findPosition(SgNode *n)
   {
     SgBasicBlock *bb = isSgBasicBlock(n->get_parent());
     ROSE_ASSERT(bb);
     SgStatementPtrList & stmts = bb->get_statements();
     for (SgStatementPtrList::iterator i = stmts.begin(); i != stmts.end(); ++i)
        {
          if (*i == n)
             {
               return stmt_pos(&stmts, i, bb);
             }
        }
     ROSE_ASSERT(false);

  /* Avoid MSVC warning */
     return stmt_pos(&stmts, stmts.end(), bb);
   }

stmt_pos_list findFollowingPositions(SgInitializedName *in)
   {
     if (SgFunctionParameterList *pl = isSgFunctionParameterList(in->get_parent()))
        {
          stmt_pos_list poss;
          SgFunctionDeclaration *fnDecl = isSgFunctionDeclaration(pl->get_parent());
          ROSE_ASSERT(fnDecl);

          SgFunctionDeclaration *fnDefDecl = isSgFunctionDeclaration(fnDecl->get_definingDeclaration());
          if (fnDefDecl)
             {
               SgFunctionDefinition *fnDef = fnDefDecl->get_definition();
               SgBasicBlock *body = fnDef->get_body();

               SgStatementPtrList &bodyStmts = body->get_statements();
               poss.push_back(stmt_pos(&bodyStmts, bodyStmts.begin(), body));
             }
          return poss;
        }
     else
        {
          SgStatement *parent = isSgStatement(in->get_parent());
          ROSE_ASSERT(parent != NULL);
          return findFollowingPositions(parent);
        }
   }

stmt_pos_list findFollowingPositions(SgStatement *stmt)
   {
     stmt_pos_list poss;
     SgNode *parent = stmt->get_parent();
#if 0
     if (isSgExprStatement(stmt) && IsSCGenerated(stmt))
        {
          // This is the expression-statement of the former lhs of an SgCommaExp
          SgBasicBlock *bb = isSgBasicBlock(parent);
          ROSE_ASSERT(bb != NULL);

          SgStatementPtrList &stmts = bb->get_statements();
          poss.push_back(stmt_pos(&stmts, stmts.end()));
        }
     else
#endif
     if (isSgBasicBlock(parent))
        {
          stmt_pos pos = findPosition(stmt);
          ++pos.second;
          poss.push_back(pos);
#if 0
          SgStatementPtrList &stmts = bb->get_statements();
          for (SgStatementPtrList::iterator i = stmts.begin(); i != stmts.end(); ++i)
             {
               if (*i == stmt)
                  {
                    poss.push_back(stmt_pos(&stmts, ++i));
                    break;
                  }
             }
#endif
        }
     else if (SgIfStmt *is = isSgIfStmt(parent))
        {
#if 0
          if (IsSCGenerated(is))
             {
               stmt_pos pos = findPosition(is);
               ++pos.second;
               poss.push_back(pos);
             }
          else
#endif
             {
               SgStatementPtrList &ts = SageInterface::ensureBasicBlockAsTrueBodyOfIf(is)->get_statements(), &fs = SageInterface::ensureBasicBlockAsFalseBodyOfIf(is)->get_statements();
               poss.push_back(stmt_pos(&ts, ts.begin(), is->get_true_body()));
               if (is->get_false_body()) {
                 poss.push_back(stmt_pos(&fs, fs.begin(), is->get_false_body()));
               }
             }
        }
     else if (SgWhileStmt *ws = isSgWhileStmt(parent))
        {
          SgStatementPtrList &stmts = SageInterface::ensureBasicBlockAsBodyOfWhile(ws)->get_statements();
          poss.push_back(stmt_pos(&stmts, stmts.begin(), ws->get_body()));
          stmt_pos outerPos = findPosition(ws);
          ++outerPos.second;
          poss.push_back(outerPos);
        }
     else if (SgForStatement *fs = isSgForStatement(parent))
        {
       // we cannot check the inits here, as it would lead to a reference being made to a place which is passed multiple times for a one-time initializetion
          if (fs->get_test() == stmt)
             {
               SgStatementPtrList &stmts = SageInterface::ensureBasicBlockAsBodyOfFor(fs)->get_statements();
               poss.push_back(stmt_pos(&stmts, stmts.begin(), fs->get_loop_body()));
               stmt_pos outerPos = findPosition(fs);
               ++outerPos.second;
               poss.push_back(outerPos);
             }
        }
     return poss;
   }
          
stmt_pos_list findPrecedingPositions(SgStatement *stmt)
   {
     SgNode *parent = stmt->get_parent();
     if (SgIfStmt *is = isSgIfStmt(parent))
        {
          return findPrecedingPositions(is);
        }
     else if (SgWhileStmt *ws = isSgWhileStmt(parent))
        {
          stmt_pos_list poss;
          poss.push_back(findPosition(ws));
          
          SgStatementPtrList &stmts = SageInterface::ensureBasicBlockAsBodyOfWhile(ws)->get_statements();
          poss.push_back(stmt_pos(&stmts, stmts.end(), ws->get_body()));
          return poss;
        }
     return stmt_pos_list(1, findPosition(stmt));
   }

SgStatement *findStatementForExpression(SgExpression *expr)
   {
     return findNearestParentOfType<SgStatement, isSgStatement>(expr).first;
   }

pair<SgBasicBlock *, SgNode *> findBasicBlockForStmt(SgStatement *stmt)
   {
     return findNearestParentOfType<SgBasicBlock, isSgBasicBlock>(stmt);
   }
     
SgStatement *findIfStmtForSC(SgExpression *expr)
   {
     return findNearestParentOfType<SgIfStmt, isSgIfStmt>(expr).first;
   }

