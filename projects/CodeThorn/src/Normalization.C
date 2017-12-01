
#include "sage3basic.h"
#include "SingleStatementToBlockNormalization.h"
#include "Normalization.h"
#include "RoseAst.h"
#include "SgNodeHelper.h"
#include "inliner.h"
#include "CFAnalysis.h"

using namespace std;

namespace SPRAY {

  int32_t Normalization::tmpVarNr=1;

  void Normalization::normalizeAst(SgNode* root) {
    SingleStatementToBlockNormalizer singleStatementToBlockNormalizer;
    singleStatementToBlockNormalizer.Normalize(root);

    convertAllForsToWhiles(root);
    changeBreakStatementsToGotos(root);
    normalizeExpressions(root);
    inlineFunctions(root);
  }

  void Normalization::convertAllForsToWhiles (SgNode* top) {
    SageInterface::convertAllForsToWhiles (top);
  }
 
  void Normalization::changeBreakStatementsToGotos (SgNode* root) {
    RoseAst ast(root);
    for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
      if(isSgSwitchStatement(*i)||CFAnalysis::isLoopConstructRootNode(*i)) {
         SageInterface::changeBreakStatementsToGotos(isSgStatement(*i));
      }
    }
  }

  void Normalization::generateTmpVarAssignment(SgExprStatement* stmt, SgExpression* expr) {
    // 1) generate tmp-var assignment node with expr as lhs
    // 2) replace use of expr with tmp-var
    SgVariableDeclaration* tmpVarDeclaration = 0;
    SgExpression* tmpVarReference = 0;
    SgScopeStatement* scope=stmt->get_scope();
    tie(tmpVarDeclaration, tmpVarReference) = SageInterface::createTempVariableAndReferenceForExpression(expr, scope);
    tmpVarDeclaration->set_parent(scope);
    ROSE_ASSERT(tmpVarDeclaration!= 0);
    cout<<"tmp"<<tmpVarNr<<": replaced @"<<(stmt)->unparseToString()<<" inserted: "<<tmpVarDeclaration->unparseToString()<<endl;
    tmpVarNr++;
    transformationList.push_back(make_pair(stmt,expr));
  }

  void Normalization::normalizeExpression(SgExprStatement* stmt, SgExpression* expr) {
    if(isSgPntrArrRefExp(expr)) {
        // TODO: evaluate index-expressions
    } else if(SgAssignOp* assignOp=isSgAssignOp(expr)) {
      //TODO: normalize subexpressions of LHS
      //normalizeExpression(stmt,isSgExpression(SgNodeHelper::getLhs(assignOp)));
      normalizeExpression(stmt,isSgExpression(SgNodeHelper::getRhs(assignOp)));
    } else if(SgCompoundAssignOp* compoundAssignOp=isSgCompoundAssignOp(expr)) {
      //TODO: normalize subexpressions of LHS
      //normalizeExpression(stmt,isSgExpression(SgNodeHelper::getLhs(assignOp)));
      normalizeExpression(stmt,isSgExpression(SgNodeHelper::getRhs(compoundAssignOp)));
    } else if(SgNodeHelper::isPrefixIncDecOp(expr)||SgNodeHelper::isPostfixIncDecOp(expr)) {
      /* TODO: ++,-- operators may need to moved in the generated assignment sequence
         and replaced with +=/-=.
      */
      normalizeExpression(stmt,isSgExpression(SgNodeHelper::getUnaryOpChild(expr)));
    } else if(isSgBinaryOp(expr)) {
      normalizeExpression(stmt,isSgExpression(SgNodeHelper::getLhs(expr)));
      normalizeExpression(stmt,isSgExpression(SgNodeHelper::getRhs(expr)));
      generateTmpVarAssignment(stmt,expr);
    } else if(isSgUnaryOp(expr)) {
      normalizeExpression(stmt,isSgExpression(SgNodeHelper::getUnaryOpChild(expr)));
      generateTmpVarAssignment(stmt,expr);
    } else if(isSgFunctionCallExp(expr)) {
      SgExpressionPtrList& expList=SgNodeHelper::getFunctionCallActualParameterList(expr);
      for(SgExpressionPtrList::iterator i=expList.begin();i!=expList.end();++i) {
        normalizeExpression(stmt,*i);
      }
      generateTmpVarAssignment(stmt,expr);
    }
  }
  
  void Normalization::normalizeExpressions(SgNode* node) {
    // find all SgExprStatement, SgReturnStmt
    RoseAst ast(node);
    for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
      if(SgExprStatement* exprStmt=isSgExprStatement(*i)) {
        if(!SgNodeHelper::isCond(exprStmt)) {
          cout<<"Found SgExprStatement: "<<(*i)->unparseToString()<<endl;
          SgExpression* expr=exprStmt->get_expression();
          normalizeExpression(exprStmt,expr);
          i.skipChildrenOnForward();
        }
      }
      if(isSgReturnStmt(*i)) {
        cout<<"Found SgReturnStmt: "<<(*i)->unparseToString()<<endl;
        i.skipChildrenOnForward();
      }
    }
    for(TransformationList::iterator i=transformationList.begin();i!=transformationList.end();++i) {
      SgStatement* stmt=(*i).first;
      SgExpression* expr=(*i).second;
      SgVariableDeclaration* tmpVarDeclaration = 0;
      SgExpression* tmpVarReference = 0;
      SgScopeStatement* scope=stmt->get_scope();
      tie(tmpVarDeclaration, tmpVarReference) = SageInterface::createTempVariableAndReferenceForExpression(expr, scope);
      tmpVarDeclaration->set_parent(scope);
      ROSE_ASSERT(tmpVarDeclaration!= 0);
      SageInterface::insertStatementBefore(stmt, tmpVarDeclaration);
      SageInterface::replaceExpression(expr, tmpVarReference);

      cout<<"tmp"<<tmpVarNr<<": replaced @"<<(stmt)->unparseToString()<<" inserted: "<<tmpVarDeclaration->unparseToString()<<endl;
      tmpVarNr++;
    }
  }

  bool Normalization::isAstContaining(SgNode *haystack, SgNode *needle) {
    struct T1: AstSimpleProcessing {
      SgNode *needle;
      T1(SgNode *needle): needle(needle) {}
      void visit(SgNode *node) {
        if (node == needle)
          throw this;
      }
    } t1(needle);
    try {
      t1.traverse(haystack, preorder);
      return false;
    } catch (const T1*) {
      return true;
    }
  }
  
  size_t Normalization::inlineFunctions(SgNode* root) {
    // Inline one call at a time until all have been inlined.  Loops on recursive code.
    //SgProject* project=isSgProject(root);
    //ROSE_ASSERT(project);
    size_t nInlined = 0;
    for (int count=0; count<10; ++count) {
      bool changed = false;
        BOOST_FOREACH (SgFunctionCallExp *call, SageInterface::querySubTree<SgFunctionCallExp>(root)) {
          if (doInline(call)) {
            ASSERT_always_forbid2(isAstContaining(root, call),
                                  "Inliner says it inlined, but the call expression is still present in the AST.");
            ++nInlined;
            changed = true;
            break;
          }
        }
        if (!changed)
          break;
    }
    return nInlined;
  }

} // end of namespace SPRAY

