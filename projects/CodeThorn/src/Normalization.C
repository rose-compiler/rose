
#include "sage3basic.h"
#include "SingleStatementToBlockNormalization.h"
#include "Normalization.h"
#include "RoseAst.h"
#include "SgNodeHelper.h"
#include "inliner.h"
#include "CFAnalysis.h"

using namespace std;
using namespace Rose;

namespace SPRAY {

  int32_t Normalization::tmpVarNr=1;
  int32_t Normalization::labelNr=1;

  void Normalization::normalizeAst(SgNode* root) {
    normalizeBlocks(root);
    convertAllForsToWhiles(root);
    changeBreakStatementsToGotos(root);
    createLoweringSequence(root);
    applyLoweringSequence();
    normalizeExpressions(root);
    inlineFunctions(root);
  }

  void Normalization::createLoweringSequence(SgNode* node) {
    RoseAst ast(node);
    for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
      if(SgWhileStmt* stmt=isSgWhileStmt(*i)) {
        loweringSequence.push_back(new WhileStmtLowering(stmt));
      } else if(SgDoWhileStmt* stmt=isSgDoWhileStmt(*i)) {
        loweringSequence.push_back(new DoWhileStmtLowering(stmt));
      }
    }
  }

  void Normalization::applyLoweringSequence() {
    BOOST_FOREACH(Lowering* loweringOp,loweringSequence) {
      loweringOp->analyse();
      loweringOp->transform();
    }
  }

  void Normalization::normalizeBlocks(SgNode* root) {
    SingleStatementToBlockNormalizer singleStatementToBlockNormalizer;
    singleStatementToBlockNormalizer.Normalize(root);
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
      /* TODO: ++,-- operators may need to be moved in the generated assignment sequence
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

  // creates a goto at end of 'block', and inserts a label before statement 'target'.
  void Normalization::createGotoStmtAtEndOfBlock(SgBasicBlock* block, SgStatement* target) {
    SgLabelStatement* newLabel =
      SageBuilder::buildLabelStatement("__loopLabel" +
                                       StringUtility::numberToString(labelNr++),
                                       SageBuilder::buildBasicBlock(),
                                       isSgScopeStatement(target->get_parent()));
    SageInterface::insertStatement(target, newLabel, true);
    SgGotoStatement* newGoto = SageBuilder::buildGotoStatement(newLabel);
    block->append_statement(newGoto);
  }

  Lowering::Lowering() {}
  Lowering::~Lowering() {}

  void Lowering::analyse() {
  }
  void WhileStmtLowering::analyse() {
  }
  WhileStmtLowering::WhileStmtLowering(SgWhileStmt* node) {
    this->node=node;
  }
  void WhileStmtLowering::transform() {

    cout<<"DEBUG: transforming WhileStmt."<<endl;
    SgBasicBlock* block=isSgBasicBlock(SgNodeHelper::getLoopBody(node));
    ROSE_ASSERT(block); // must hold because all branches are normalized to be blocks
    Normalization::createGotoStmtAtEndOfBlock(block, node);

    // replace WhileStmt with IfStmt
    SgIfStmt* newIfStmt=SageBuilder::buildIfStmt(isSgExprStatement(SgNodeHelper::getCond(node)),
                                                 block,
                                                 0);
    isSgStatement(node->get_parent())->replace_statement(node,newIfStmt);
    newIfStmt->set_parent(node->get_parent());
  }

  WhileStmtLowering::~WhileStmtLowering() {
  }

  DoWhileStmtLowering::DoWhileStmtLowering(SgDoWhileStmt* node) {
    this->node=node;
  }
  void DoWhileStmtLowering::transform() {
    //cout<<"DEBUG: transforming DoWhileStmt."<<endl;
  }

  ForStmtLowering::ForStmtLowering(SgForStatement* node) {
    this->node=node;
  }
  void ForStmtLowering::transform() {
  }

  SwitchStmtLowering::SwitchStmtLowering(SgSwitchStatement* node) {
    this->node=node;
  }
  void SwitchStmtLowering::transform() {
  }

} // end of namespace SPRAY

