#include "sage3basic.h"
#include "SingleStatementToBlockNormalization.h"
#include "Lowering.h"
#include "RoseAst.h"
#include "SgNodeHelper.h"
#include "inliner.h"
#include "CFAnalysis.h"

using namespace std;
using namespace Rose;

namespace SPRAY {

  int32_t Lowering::tmpVarNr=1;
  int32_t Lowering::labelNr=1;
  string Lowering::labelPrefix="__label";

  void Lowering::setLabelPrefix(std::string prefix) {
    Lowering::labelPrefix=prefix;
  }

  string Lowering::newLabelName() {
    return labelPrefix + StringUtility::numberToString(Lowering::labelNr++);
  }

  void Lowering::lowerAst(SgNode* root) {
    normalizeBlocks(root);
    convertAllForsToWhiles(root);
    changeBreakStatementsToGotos(root);
    createLoweringSequence(root);
    applyLoweringSequence();
    normalizeExpressions(root);
    inlineFunctions(root);
  }

  void Lowering::createLoweringSequence(SgNode* node) {
    RoseAst ast(node);
    for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
      if(SgWhileStmt* stmt=isSgWhileStmt(*i)) {
        loweringSequence.push_back(new WhileStmtLoweringOp(stmt));
      } else if(SgDoWhileStmt* stmt=isSgDoWhileStmt(*i)) {
        loweringSequence.push_back(new DoWhileStmtLoweringOp(stmt));
      }
    }
  }

  void Lowering::applyLoweringSequence() {
    BOOST_FOREACH(LoweringOp* loweringOp,loweringSequence) {
      loweringOp->analyse();
      loweringOp->transform();
    }
  }

  void Lowering::normalizeBlocks(SgNode* root) {
    SingleStatementToBlockNormalizer singleStatementToBlockNormalizer;
    singleStatementToBlockNormalizer.Normalize(root);
  }

  void Lowering::convertAllForsToWhiles (SgNode* top) {
    SageInterface::convertAllForsToWhiles (top);
  }
 
  void Lowering::changeBreakStatementsToGotos (SgNode* root) {
    RoseAst ast(root);
    for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
      if(isSgSwitchStatement(*i)||CFAnalysis::isLoopConstructRootNode(*i)) {
         SageInterface::changeBreakStatementsToGotos(isSgStatement(*i));
      }
    }
  }

  void Lowering::generateTmpVarAssignment(SgExprStatement* stmt, SgExpression* expr) {
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

  void Lowering::normalizeExpression(SgExprStatement* stmt, SgExpression* expr) {
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
  
  void Lowering::normalizeExpressions(SgNode* node) {
    // TODO: if temporary variables are generated, the initialization-list
    // must be put into a block, otherwise some generates gotos are
    // not legal (crossing initialization).

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

  bool Lowering::isAstContaining(SgNode *haystack, SgNode *needle) {
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
  
  size_t Lowering::inlineFunctions(SgNode* root) {
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
  SgGotoStatement* Lowering::createGotoStmtAndInsertLabel(SgBasicBlock* block, SgStatement* target) {
    SgLabelStatement* newLabel =
      SageBuilder::buildLabelStatement(Lowering::newLabelName(),
                                       SageBuilder::buildBasicBlock(),
                                       // MS: scope should be function scope?
                                       isSgScopeStatement(target->get_parent()));
    SageInterface::insertStatement(target, newLabel, true);
    SgGotoStatement* newGoto = SageBuilder::buildGotoStatement(newLabel);
    return newGoto;
  }

  // creates a goto at end of 'block', and inserts a label before statement 'target'.
  void Lowering::createGotoStmtAtEndOfBlock(SgBasicBlock* block, SgStatement* target) {
    SgGotoStatement* newGoto=createGotoStmtAndInsertLabel(block, target);
    block->append_statement(newGoto);
  }

  LoweringOp::LoweringOp() {}
  LoweringOp::~LoweringOp() {}

  void LoweringOp::analyse() {
  }
  void WhileStmtLoweringOp::analyse() {
  }
  WhileStmtLoweringOp::WhileStmtLoweringOp(SgWhileStmt* node) {
    this->node=node;
  }
  void WhileStmtLoweringOp::transform() {
    //cout<<"DEBUG: transforming WhileStmt."<<endl;
    SgBasicBlock* block=isSgBasicBlock(SgNodeHelper::getLoopBody(node));
    ROSE_ASSERT(block); // must hold because all branches are normalized to be blocks
    Lowering::createGotoStmtAtEndOfBlock(block, node);
    // build IfStmt
    SgIfStmt* newIfStmt=SageBuilder::buildIfStmt(isSgExprStatement(SgNodeHelper::getCond(node)),
                                                 block,
                                                 0);
    // replace while-stmt with if-stmt
    isSgStatement(node->get_parent())->replace_statement(node,newIfStmt);
    newIfStmt->set_parent(node->get_parent());
  }

  WhileStmtLoweringOp::~WhileStmtLoweringOp() {
  }

  DoWhileStmtLoweringOp::DoWhileStmtLoweringOp(SgDoWhileStmt* node) {
    this->node=node;
  }
  void DoWhileStmtLoweringOp::transform() {
    //cout<<"DEBUG: transforming DoWhileStmt."<<endl;
    SgScopeStatement* scopeContainingDoWhileStmt=node->get_scope();
    SgExprStatement* conditionOfDoWhileStmt=isSgExprStatement(SgNodeHelper::getCond(node));
    ROSE_ASSERT(conditionOfDoWhileStmt);
    SgBasicBlock* blockOfDoWhileStmt=isSgBasicBlock(SgNodeHelper::getLoopBody(node));
    ROSE_ASSERT(blockOfDoWhileStmt);
    SgStatement* parentOfDoWhileStmt=isSgStatement(node->get_parent());
    ROSE_ASSERT(parentOfDoWhileStmt);

    // build label
    SgLabelStatement* newLabel =
      SageBuilder::buildLabelStatement(Lowering::newLabelName(),
                                       SageBuilder::buildBasicBlock(),
                                       // MS: scope should be function scope?
                                      scopeContainingDoWhileStmt);
    SageInterface::prependStatement(newLabel, blockOfDoWhileStmt);

    // build goto-stmt
    SgGotoStatement* newGoto = SageBuilder::buildGotoStatement(newLabel);

    // build if-stmt with condition of do-while-stmt and new goto
    SgIfStmt* newIfStmt=SageBuilder::buildIfStmt(conditionOfDoWhileStmt,
                                                 newGoto, // TODO: should be its own block?
                                                 0);
    newIfStmt->set_parent(blockOfDoWhileStmt);

    // replace do-while with new block (of do-while) and insert if-stmt with conditional goto.
    parentOfDoWhileStmt->replace_statement(node,blockOfDoWhileStmt);
    SageInterface::appendStatement(newIfStmt,blockOfDoWhileStmt);

    // handle continue statements
    std::set<SgContinueStmt*> continueStmts=SgNodeHelper::loopRelevantContinueStmtNodes(node);
    
    if(continueStmts.size()>0) {
      // create label for gotos replacing continues
      SgLabelStatement* newLabelForContinue =
        SageBuilder::buildLabelStatement(Lowering::newLabelName(),
                                         SageBuilder::buildBasicBlock(),
                                         // MS: scope should be function scope?
                                         scopeContainingDoWhileStmt);
      SageInterface::insertStatementBefore(newIfStmt, newLabelForContinue);
      BOOST_FOREACH(SgContinueStmt* continueStmt,continueStmts) {
        cout<<"TODO:"<<continueStmt->unparseToString()<<endl;
        SgGotoStatement* newGoto = SageBuilder::buildGotoStatement(newLabelForContinue);
        // replace continue with newGoto
        isSgStatement(continueStmt->get_parent())->replace_statement(continueStmt,newGoto);
        newGoto->set_parent(continueStmt->get_parent());
      }
    }
  }

  ForStmtLoweringOp::ForStmtLoweringOp(SgForStatement* node) {
    this->node=node;
  }
  void ForStmtLoweringOp::transform() {
  }

  SwitchStmtLoweringOp::SwitchStmtLoweringOp(SgSwitchStatement* node) {
    this->node=node;
  }
  void SwitchStmtLoweringOp::transform() {
  }

} // end of namespace SPRAY

