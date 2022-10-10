#include "sage3basic.h"
#include "SingleStatementToBlockNormalization.h"

#include "Normalization.h"
#include "RoseAst.h"
#include "SgNodeHelper.h"
#include "inliner.h"
#include "CFAnalysis.h"
#include <list>
#include "NormalizationOp.h"

using namespace std;
using namespace Rose;

namespace CodeThorn {
  NormalizationOp::NormalizationOp() {}
  NormalizationOp::~NormalizationOp() {}
  void NormalizationOp::analyse() {}

  NormalizationOpWhileStmt::NormalizationOpWhileStmt(SgWhileStmt* node) {
    this->node=node;
  }
  void NormalizationOpWhileStmt::transform() {
    //cout<<"DEBUG: transforming WhileStmt."<<endl;
    SgBasicBlock* block=isSgBasicBlock(SgNodeHelper::getLoopBody(node));
    ROSE_ASSERT(block); // must hold because all branches are normalized to be blocks
    
    SgLabelStatement* newLabel=Normalization::createLabel(node);
    Normalization::createGotoStmtAtEndOfBlock(newLabel,block,node);

    // build IfStmt
    SgIfStmt* newIfStmt=SageBuilder::buildIfStmt(isSgExprStatement(SgNodeHelper::getCond(node)),
                                                 block,
                                                 0);
    // replace while-stmt with if-stmt
    isSgStatement(node->get_parent())->replace_statement(node,newIfStmt);
    newIfStmt->set_parent(node->get_parent());

    // handle continue statements
    std::set<SgContinueStmt*> continueStmts=SgNodeHelper::loopRelevantContinueStmtNodes(node);

    if(continueStmts.size()>0) {
      // reuse same label (compact code) for gotos replacing continues
      SgLabelStatement* labelForContinue = newLabel; 
      BOOST_FOREACH(SgContinueStmt* continueStmt,continueStmts) {
        SgGotoStatement* newGoto = SageBuilder::buildGotoStatement(labelForContinue);
        // replace continue with newGoto
        isSgStatement(continueStmt->get_parent())->replace_statement(continueStmt,newGoto);
        newGoto->set_parent(continueStmt->get_parent());
      }
    }
  }

  NormalizationOpDoWhileStmt::NormalizationOpDoWhileStmt(SgDoWhileStmt* node) {
    this->node=node;
  }
  void NormalizationOpDoWhileStmt::transform() {
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
      SageBuilder::buildLabelStatement(Normalization::newLabelName(),
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
        SageBuilder::buildLabelStatement(Normalization::newLabelName(),
                                         SageBuilder::buildBasicBlock(),
                                         // MS: scope should be function scope?
                                         scopeContainingDoWhileStmt);
      SageInterface::insertStatementBefore(newIfStmt, newLabelForContinue);
      BOOST_FOREACH(SgContinueStmt* continueStmt,continueStmts) {
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
}
