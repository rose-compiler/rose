
#include "sage3basic.h"
#include "SingleStatementToBlockNormalization.h"
#include "Normalization.h"
#include "RoseAst.h"
#include "SgNodeHelper.h"

using namespace std;

namespace SPRAY {

  int32_t Normalization::tmpVarNr=1;
  //Normalization::TransformationList Normalization::transformationList;

  void Normalization::normalizeAst(SgNode* root) {
    SingleStatementToBlockNormalizer singleStatementToBlockNormalizer;
    singleStatementToBlockNormalizer.Normalize(root);

    //convertAllForsToWhiles(root);
    //changeBreakStatementsToGotos(root);
    normalizeExpressions(root);
#if 0
    ExtractFunctionArguments efa;
    if(!efa.IsNormalized(root)) {
      cout<<"STATUS: Normalizing function call arguments."<<endl;
      efa.NormalizeTree(root,true);
    }
    FunctionCallNormalization fn;
    cout<<"STATUS: Normalizing function calls in expressions."<<endl;
    fn.visit(root);
#endif
  }

  void Normalization::convertAllForsToWhiles (SgNode* top) {
    SageInterface::convertAllForsToWhiles (top);
  }
 
  void Normalization::changeBreakStatementsToGotos (SgStatement *loopOrSwitch) {
    SageInterface::changeBreakStatementsToGotos(loopOrSwitch);
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
} // end of namespace SPRAY

