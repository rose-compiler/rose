#include "sage3basic.h"
#include "SingleStatementToBlockNormalization.h"

#include "Normalization.h"
#include "RoseAst.h"
#include "SgNodeHelper.h"
#include "CFAnalysis.h"
#include <list>

// Author: Markus Schordan, 2018

using namespace std;
using namespace Rose;

namespace SPRAY {

  int32_t Normalization::tmpVarNr=1;
  int32_t Normalization::labelNr=1;
  string Normalization::labelPrefix="__label";

  Normalization::Normalization() {
    _inliner=new RoseInliner();
  }
  Normalization::~Normalization() {
    if(_defaultInliner) {
      delete _inliner;
    }
  }

  void Normalization::Options::setLevel(unsigned int level) {
    if(level==0) {
      normalization=false;
      return;
    }
    restrictToFunCallExpressions=(level==1);
    if(level==1||level==2) {
      normalization=true;
      normalizeSingleStatements=true;
      eliminateForStatements=true;
      eliminateWhileStatements=false;
      normalizeVariableDeclarations=true;
      hoistConditionExpressions=true;
      normalizeExpressions=true;
      encapsulateNormalizedExpressionsInBlocks=false;
      transformBreakToGotoInSwitchStmt=false;
      transformBreakToGotoInLoopStmts=false;
      return;
    }
    cerr<<"Error: unsupported normalization level "<<level<<endl;
    exit(1);
  }

  void Normalization::removeDefaultInliner() {
    if(_defaultInliner) {
      delete _inliner;
      _defaultInliner=false;
    }
  }
  InlinerBase* Normalization::getInliner() {
    return _inliner;
  }
  void Normalization::setInliner(SPRAY::InlinerBase* userDefInliner) {
    removeDefaultInliner();
    _inliner=userDefInliner;
  }

  void Normalization::normalizeAllVariableDeclarations(SgNode* root) {
    RoseAst ast(root);
    typedef std::list<std::pair<SgVariableDeclaration*,SgStatement*>> DeclAssignListType;
    DeclAssignListType declAssignList;
    for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
      SgNode* node=*i;
      if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(node)) {
        // do not transform assignments to static variables (must remain initializations because of different semantics)
        if(!SageInterface::isStatic(varDecl)) {
          if(SgStatement* newVarAssignment=buildNormalizedVariableDeclaration(varDecl)) {
            declAssignList.push_back(std::make_pair(varDecl,newVarAssignment));
          }
        }
        i.skipChildrenOnForward();
      } else {
        //cout<<"DEBUG: NOT a variable declaration: "<<node->class_name()<<endl;
      }
    }
    for(auto declAssign : declAssignList) {
      // insert new assignment statement after original variable declaration
      SageInterface::insertStatementAfter(declAssign.first, declAssign.second);
    }
  }


  SgStatement* Normalization::buildNormalizedVariableDeclaration(SgVariableDeclaration* varDecl) {
    ROSE_ASSERT(varDecl);
    // check that variable is within a scope where it can be normalized
    SgScopeStatement* scopeStatement=varDecl->get_scope();
    ROSE_ASSERT(scopeStatement);
    if(!isSgGlobal(scopeStatement)) {
      //cout<<"DEBUG normalizing decl: "<<varDecl->unparseToString()<<endl;
      SgExpression* declInitializer=SgNodeHelper::getInitializerExpressionOfVariableDeclaration(varDecl);
      // if there is no initializer, the declaration remains unchanged
      if(declInitializer) {
        SgInitializedName* declInitName=SgNodeHelper::getInitializedNameOfVariableDeclaration(varDecl);
        // detach initializer from declaration such that is has no initializer
        varDecl->reset_initializer(0); 
        // build new variable
        SgVarRefExp* declVarRefExp=SageBuilder::buildVarRefExp(declInitName,varDecl->get_declarationScope());
        // build assignment with new variable and initializer from original declaration
        SgAssignOp* varAssignOp=SageBuilder::buildAssignOp(declVarRefExp,declInitializer);
        // build exprstatement from assignOp expression
        SgStatement* varAssignStatement=SageBuilder::buildExprStatement(varAssignOp);
        return varAssignStatement;
      }
    }
    return nullptr;
  }

  void Normalization::normalizeSingleStatementsToBlocks(SgNode* root) {
    SingleStatementToBlockNormalizer singleStatementToBlockNormalizer;
    singleStatementToBlockNormalizer.Normalize(root);
  }

  void Normalization::setLabelPrefix(std::string prefix) {
    Normalization::labelPrefix=prefix;
  }

  string Normalization::newLabelName() {
    return labelPrefix + StringUtility::numberToString(Normalization::labelNr++);
  }

  void Normalization::normalizeAst(SgNode* root) {
    if(options.normalizeSingleStatements) {
      normalizeSingleStatementsToBlocks(root);
    }
    if(options.eliminateForStatements) {
      convertAllForStmtsToWhileStmts(root);
    }

    // uses options to select which breaks are transformed (can be none)
    normalizeBreakAndContinueStmts(root);

    if(options.eliminateWhileStatements) {
      // transforms while and do-while loops
      createLoweringSequence(root);
      applyLoweringSequence();
    }
    if(options.hoistConditionExpressions) {
      hoistConditionsInAst(root,options.restrictToFunCallExpressions);
    }
    if(options.normalizeExpressions) {
      normalizeExpressionsInAst(root,options.restrictToFunCallExpressions);
    }
    if(options.normalizeVariableDeclarations) {
      normalizeAllVariableDeclarations(root);
    }
    if(options.inlining) {
      InlinerBase* inliner=getInliner();
      ROSE_ASSERT(inliner);
      inliner->inlineFunctions(root);
    }
  }

  // transformation: if(C) ... => T t=C; if(t) ...
   // transformation: switch(C) ... => T t=C; switch(t) ...
  // while/do-while/for: not applicable. Transform those before cond-hoisting.
  void Normalization::hoistCondition(SgStatement* stmt) {
    ROSE_ASSERT(isSgIfStmt(stmt)||isSgSwitchStatement(stmt));
    SgNode* condNode=SgNodeHelper::getCond(stmt);
    ROSE_ASSERT(condNode);
    if(isSgExprStatement(condNode)) {
      condNode=SgNodeHelper::getExprStmtChild(condNode);
    }
    SgExpression* condExpr=isSgExpression(condNode);
    ROSE_ASSERT(condExpr);
    // (i) build tmp var with cond as initializer
    SgVariableDeclaration* tmpVarDeclaration = 0;
    SgExpression* tmpVarReference = 0;
    SgScopeStatement* scope=stmt->get_scope();
    tie(tmpVarDeclaration, tmpVarReference) = SageInterface::createTempVariableAndReferenceForExpression(condExpr, scope);
    tmpVarDeclaration->set_parent(scope);
    ROSE_ASSERT(tmpVarDeclaration!= 0);

    // (ii) replace cond with new tmp-varref
    bool deleteReplacedExpression=false;
    SgNodeHelper::replaceExpression(condExpr,tmpVarReference,deleteReplacedExpression);

    // (iii) set cond as initializer in new variable declaration
    //TODO turn condition into initializer
    //tmpVarDeclaration->reset_initializer(cond);

    // (iv) insert declaration with initializer before stmt
    SageInterface::insertStatementBefore(stmt, tmpVarDeclaration);
  }

  void Normalization::hoistConditionsInAst(SgNode* node, bool onlyNormalizeFunctionCallExpressions) {
    list<SgStatement*> hoistingTransformationList;
    RoseAst ast(node);
    // build list of stmts to transform
    for (auto node : ast) {
      if(SgNodeHelper::isCond(node)) {
        SgStatement* stmt=isSgStatement(node->get_parent());
        if(isSgIfStmt(stmt)||isSgSwitchStatement(stmt)) {
          if(onlyNormalizeFunctionCallExpressions) {
            if(hasFunctionCall(isSgExpression(SgNodeHelper::getCond(stmt)))) {
              hoistingTransformationList.push_back(stmt);
            } else {
              // do not hoist
            }
          } else {
            hoistingTransformationList.push_back(stmt);
          }
        }
      }
    }
    // transform stmts
    for (auto stmt: hoistingTransformationList) {
      hoistCondition(stmt);
    }
  }

  void Normalization::setInliningOption(bool flag) {
    options.inlining=flag;
  }

  bool Normalization::getInliningOption() {
    return options.inlining;
  }
  
  void Normalization::createLoweringSequence(SgNode* node) {
    RoseAst ast(node);
    for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
      if(SgWhileStmt* stmt=isSgWhileStmt(*i)) {
        loweringSequence.push_back(new NormalizationOpWhileStmt(stmt));
      } else if(SgDoWhileStmt* stmt=isSgDoWhileStmt(*i)) {
        loweringSequence.push_back(new NormalizationOpDoWhileStmt(stmt));
      }
    }
  }

  void Normalization::applyLoweringSequence() {
    BOOST_FOREACH(NormalizationOp* loweringOp,loweringSequence) {
      loweringOp->analyse();
      loweringOp->transform();
    }
  }

  void Normalization::convertAllForStmtsToWhileStmts(SgNode* top) {
    SageInterface::convertAllForsToWhiles (top);
  }
 
  void Normalization::transformContinueToGotoStmts(SgWhileStmt* whileStmt) {
    cerr<<"Error: transforming continue to goto stmt in while loop not supported yet."<<endl;
    exit(1);
  }

  void Normalization::transformContinueToGotoStmts(SgDoWhileStmt* whileStmt) {
    cerr<<"Error: transforming continue to goto stmt in do-while loop not supported yet."<<endl;
    exit(1);
  }

  void Normalization::normalizeBreakAndContinueStmts(SgNode* root) {
    RoseAst ast(root);
    list<SgStatement*> breakTransformationList;
    list<SgStatement*> continueTransformationList;
    // analysis phase
    for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
      SgStatement* stmt=isSgStatement(*i);
      if(stmt) {
        if(isSgSwitchStatement(stmt)) {
          // only change break statements in switch stmt if explicitly requested
          if(options.transformBreakToGotoInSwitchStmt) {
            breakTransformationList.push_back(stmt);
          } 
        } else {
          if(options.transformBreakToGotoInLoopStmts && CFAnalysis::isLoopConstructRootNode(stmt)) {
            breakTransformationList.push_back(stmt);
          }
          if(options.transformContinueToGotoInWhileStmts) {
            continueTransformationList.push_back(stmt);
          }
        }
      }
    }
    // transformation phase (must be separate from analysis, since transformations happen ahead of the iterator)
    for( auto stmt : breakTransformationList) {
      SageInterface::changeBreakStatementsToGotos(stmt);
    }
    for( auto stmt : continueTransformationList) {
      if(SgWhileStmt* whileStmt=isSgWhileStmt(stmt)) {
        transformContinueToGotoStmts(whileStmt);
      } else if(SgDoWhileStmt* doWhileStmt=isSgDoWhileStmt(stmt)) {
        transformContinueToGotoStmts(doWhileStmt);
      }
    }
  }

  void Normalization::normalizeExpressionsInAst(SgNode* node, bool onlyNormalizeFunctionCallExpressions) {
    // TODO: if temporary variables are generated, the initialization-list
    // must be put into a block, otherwise some generated gotos are
    // not legal (crossing initialization).

    // find all SgExprStatement, SgReturnStmt
    RoseAst ast(node);
    for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
      if(SgExprStatement* exprStmt=isSgExprStatement(*i)) {
        if(!SgNodeHelper::isCond(exprStmt)) {
          //cout<<"Found SgExprStatement: "<<(*i)->unparseToString()<<endl;
          SgExpression* expr=exprStmt->get_expression();
          if(isWithinBlockStmt(expr)) {
            if(onlyNormalizeFunctionCallExpressions) {
              if(hasFunctionCall(expr)) {
                normalizeExpression(exprStmt,expr);
              }
            } else {
              normalizeExpression(exprStmt,expr);
            }
          }
          i.skipChildrenOnForward();
        }
      }
      if(isSgReturnStmt(*i)) {
        //cout<<"Found SgReturnStmt: "<<(*i)->unparseToString()<<endl;
        i.skipChildrenOnForward();
      }
    }
    for(ExprTransformationList::iterator i=exprTransformationList.begin();i!=exprTransformationList.end();++i) {
      SubExprTransformationList subExprTransformationList=*i;
      for(SubExprTransformationList::iterator j=subExprTransformationList.begin();j!=subExprTransformationList.end();++j) {
        SgStatement* stmt=(*j).first;
        SgExpression* expr=(*j).second;
        SgVariableDeclaration* tmpVarDeclaration = 0;
        SgExpression* tmpVarReference = 0;
        SgScopeStatement* scope=stmt->get_scope();
        tie(tmpVarDeclaration, tmpVarReference) = SageInterface::createTempVariableAndReferenceForExpression(expr, scope);
        tmpVarDeclaration->set_parent(scope);
        ROSE_ASSERT(tmpVarDeclaration!= 0);
        if(SgBasicBlock* block=isSgBasicBlock(stmt)) {
          block->append_statement(tmpVarDeclaration);
        } else {
          SageInterface::insertStatementBefore(stmt, tmpVarDeclaration);
        }
        SageInterface::replaceExpression(expr, tmpVarReference);
        //      }
        //cout<<"tmp"<<tmpVarNr<<": replaced @"<<(stmt)->unparseToString()<<" inserted: "<<tmpVarDeclaration->unparseToString()<<endl;
        tmpVarNr++;
      }
    }
  }
  
  void Normalization::normalizeExpression(SgExprStatement* stmt, SgExpression* expr) {
    SubExprTransformationList subExprTransformationList;
    if(options.encapsulateNormalizedExpressionsInBlocks) {
      ROSE_ASSERT(options.normalizeVariableDeclarations==true);
      // normalized subexpressions (and declared variables) are generated inside an additional block
      // move the ExprStatement into the new block
      SgNode* stmtParent=stmt->get_parent();
      SgBasicBlock* block=SageBuilder::buildBasicBlock();
      ROSE_ASSERT(block->get_parent()==0);
      SgStatement* stmtParent2=isSgStatement(stmtParent);
      ROSE_ASSERT(stmtParent2);
      stmtParent2->replace_statement(stmt,block);
      stmt->set_parent(0);
      block->append_statement(stmt);
      ROSE_ASSERT(stmt->get_parent()==block);
      normalizeSubExpression(stmt,expr,subExprTransformationList);
    } else {
      // normalized subexpressions (and declared variables) are replacing the current expression
      normalizeSubExpression(stmt,expr,subExprTransformationList);
    }
    exprTransformationList.push_back(subExprTransformationList);
  }

  void Normalization::normalizeSubExpression(SgExprStatement* stmt, SgExpression* expr, SubExprTransformationList& subExprTransformationList) {
    if(isSgPntrArrRefExp(expr)) {
        // TODO: normalize index-expressions
    } else if(SgAssignOp* assignOp=isSgAssignOp(expr)) {
      normalizeSubExpression(stmt,isSgExpression(SgNodeHelper::getRhs(assignOp)),subExprTransformationList);
      //TODO: normalize subexpressions of LHS
      //normalizeSubExpression(stmt,isSgExpression(SgNodeHelper::getLhs(assignOp)));
    } else if(SgCompoundAssignOp* compoundAssignOp=isSgCompoundAssignOp(expr)) {
      normalizeSubExpression(stmt,isSgExpression(SgNodeHelper::getRhs(compoundAssignOp)),subExprTransformationList);
      //TODO: normalize subexpressions of LHS
      //normalizeSubExpression(stmt,isSgExpression(SgNodeHelper::getLhs(assignOp)));
    } else if(isSgBinaryOp(expr)) {
      normalizeSubExpression(stmt,isSgExpression(SgNodeHelper::getLhs(expr)),subExprTransformationList);
      normalizeSubExpression(stmt,isSgExpression(SgNodeHelper::getRhs(expr)),subExprTransformationList);
      generateTmpVarAssignment(stmt,expr,subExprTransformationList);
    } else if(isSgUnaryOp(expr)) {
      normalizeSubExpression(stmt,isSgExpression(SgNodeHelper::getUnaryOpChild(expr)),subExprTransformationList);
      generateTmpVarAssignment(stmt,expr,subExprTransformationList);
    } else if(SgFunctionCallExp* funCallExp=isSgFunctionCallExp(expr)) {
      SgExpressionPtrList& expList=SgNodeHelper::getFunctionCallActualParameterList(expr);
      for(SgExpressionPtrList::iterator i=expList.begin();i!=expList.end();++i) {
        normalizeSubExpression(stmt,*i,subExprTransformationList);
      }
      // check if function has a return value
      SgType* functionReturnType=funCallExp->get_type();
      //cout<<"DEBUG: function call type: "<<SgNodeHelper::sourceLineColumnToString(funCallExp)<<":"<<functionReturnType->unparseToString()<<endl;

      // generate tmp var only if return value exists and it is used (i.e. there exists an expression as parent).
      SgNode* parentNode=funCallExp->get_parent();
      if(!isSgTypeVoid(functionReturnType)
         &&  isSgExpression(parentNode)
         && !isSgExpressionRoot(parentNode)) {
        generateTmpVarAssignment(stmt,expr,subExprTransformationList);
      }
    }
  }

  void Normalization::generateTmpVarAssignment(SgExprStatement* stmt, SgExpression* expr, SubExprTransformationList& subExprTransformationList) {
    // 1) generate tmp-var assignment node with expr as lhs
    // 2) replace use of expr with tmp-var
    SgVariableDeclaration* tmpVarDeclaration = 0;
    SgExpression* tmpVarReference = 0;
    SgScopeStatement* scope=stmt->get_scope();
    tie(tmpVarDeclaration, tmpVarReference) = SageInterface::createTempVariableAndReferenceForExpression(expr, scope);
    tmpVarDeclaration->set_parent(scope);
    ROSE_ASSERT(tmpVarDeclaration!= 0);
    //cout<<"tmp"<<tmpVarNr<<": replaced @"<<(stmt)->unparseToString()<<" inserted: "<<tmpVarDeclaration->unparseToString()<<endl;
    tmpVarNr++;
    subExprTransformationList.push_back(make_pair(stmt,expr));
  }

  bool Normalization::isWithinBlockStmt(SgExpression* exp) {
    SgNode* current=exp;
    while(isSgExpression(current)||isSgExprStatement(current)) {
      current=current->get_parent();
    };
    return isSgBasicBlock(current);
  }

  bool Normalization::hasFunctionCall(SgExpression* expr) {
    RoseAst ast(expr);
    for(auto node:ast) {
      if(isSgFunctionCallExp(node)) {
        return true;
      }
    }
    return false;
  }

  // creates a goto at end of 'block', and inserts a label before statement 'target'.
  SgLabelStatement* Normalization::createLabel(SgStatement* target) {
    SgLabelStatement* newLabel =
      SageBuilder::buildLabelStatement(Normalization::newLabelName(),
                                       SageBuilder::buildBasicBlock(),
                                         // MS: scope should be function scope?
                                       isSgScopeStatement(target->get_parent()));
    return newLabel;
  }
  
    // creates a goto at end of 'block', and inserts a label before statement 'target'.
  SgGotoStatement* Normalization::createGotoStmtAndInsertLabel(SgLabelStatement* newLabel, SgStatement* target) {
    SageInterface::insertStatement(target, newLabel, true);
    SgGotoStatement* newGoto = SageBuilder::buildGotoStatement(newLabel);
    return newGoto;
  }
  
  // creates a goto at end of 'block', and inserts a label before statement 'target'.
  void Normalization::createGotoStmtAtEndOfBlock(SgLabelStatement* newLabel, SgBasicBlock* block, SgStatement* target) {
    SgGotoStatement* newGoto=createGotoStmtAndInsertLabel(newLabel, target);
    block->append_statement(newGoto);
  }

} // end of namespace SPRAY

