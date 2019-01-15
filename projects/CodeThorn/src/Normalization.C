#include "sage3basic.h"
#include "SingleStatementToBlockNormalization.h"

#include "Normalization.h"
#include "RoseAst.h"
#include "SgNodeHelper.h"
#include "CFAnalysis.h"
#include <list>
#include "AstConsistencyTests.h"

#include "AstFixup.h"
#include "astPostProcessing.h"

// Author: Markus Schordan, 2018

using namespace std;
using namespace Rose;

namespace SPRAY {

  int32_t Normalization::labelNr=1;
  string Normalization::labelPrefix="__label";

  int32_t Normalization::tmpVarNr=1;
  string Normalization::tmpVarPrefix="__ntmp";


  Normalization::Normalization() {
    _inliner=new RoseInliner();
  }
  Normalization::~Normalization() {
    if(_defaultInliner) {
      delete _inliner;
    }
  }

  void Normalization::Options::configureLevel(unsigned int level) {
    if(level==0) {
      normalization=false;
      return;
    }
    restrictToFunCallExpressions=(level==1);
    if(level==1||level==2) {
      normalization=true;
      normalizeSingleStatements=true;
      normalizeLabels=true;
      eliminateForStatements=true;
      eliminateWhileStatements=false;
      hoistConditionExpressions=true;

      // normalization is not applied to static variables (would be wrong)
      normalizeVariableDeclarations=false; 

      // temporary, until function calls inside variable initializers are supported.
      normalizeVariableDeclarationsWithFunctionCalls=false; 

      eliminateShortCircuitOperators=true; // not implemented yet
      //eliminateConditionalExpressionOp=true; // not implemented yet

      encapsulateNormalizedExpressionsInBlocks=false;
      normalizeExpressions=true;
      transformBreakToGotoInSwitchStmt=false;
      transformBreakToGotoInLoopStmts=false;
      //transformContinueToGotoInWhileStmts=false; // not iomplemented yet
      return;
    }
    if(level==3) {
      normalization=true;
      normalizeSingleStatements=true;
      normalizeLabels=true;
      eliminateForStatements=true;
      eliminateWhileStatements=true;  // different to level 1,2
      hoistConditionExpressions=true;
      normalizeVariableDeclarations=false;
      //eliminateShortCircuitOperators=true; // not implemented yet
      //eliminateConditionalExpressionOp=true; // not implemented yet
      encapsulateNormalizedExpressionsInBlocks=true; // different to level 1,2
      normalizeExpressions=true;
      transformBreakToGotoInSwitchStmt=true;  // different to level 1,2
      transformBreakToGotoInLoopStmts=true;  // different to level 1,2
      //transformContinueToGotoInWhileStmts=false; // not implemented yet  // different to level 1,2
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

  void Normalization::setTmpVarPrefix(std::string prefix) {
    Normalization::tmpVarPrefix=prefix;
  }

  void Normalization::normalizeLabel(SgLabelStatement* label) {
    //SgNode* parent=label->get_parent();ROSE_ASSERT(!isSgIfStmt(parent) && !isSgWhileStmt(parent) && !isSgDoWhileStmt(parent));
    SgStatement* stmt=label->get_statement();
    if(stmt==0 || isSgNullStatement(stmt)) {
      // nothing to normalize
      return;
    }
    // label is attached to a statement (= stmt is the child of the label node)
    // (i) create null statement
    SgNullStatement* nullStmt=SageBuilder::buildNullStatement();
    label->set_statement(nullStmt);
    nullStmt->set_parent(label);
    stmt->set_parent(0); // unset parent (was label)
    // (ii) insert statement stmt after label
    bool autoMovePreprocessingInfo=true;
    // sets parent pointer of stmt
    SageInterface::insertStatementAfter(label,stmt,autoMovePreprocessingInfo);
    ROSE_ASSERT(label->get_parent()==stmt->get_parent());
  }

  bool Normalization::isVarDeclWithFunctionCall(SgNode* node) {
    return SgNodeHelper::Pattern::matchVariableDeclarationWithFunctionCall(node);
  }

  // this function is obsolete
  // introduces assignment for initialization of variable declaration
  // cannot be applied to static, const, and ref variables.
  void Normalization::normalizeAllVariableDeclarations(SgNode* root, bool onlyFunctionCalls) {
    RoseAst ast(root);
    typedef std::list<std::pair<SgVariableDeclaration*,SgStatement*>> DeclAssignListType;
    DeclAssignListType declAssignList;
    for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
      SgNode* node=*i;
      if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(node)) { 
        // do not transform assignments to static variables (must remain initializations because of different semantics)
        if(!SageInterface::isStatic(varDecl)) {
          if(onlyFunctionCalls) {
            if(!isVarDeclWithFunctionCall(*i)) {
              i.skipChildrenOnForward();
              continue;
            } else {
              //cout<<"DEBUG: Normalizing variable initializer with function call: "<<SgNodeHelper::lineColumnNodeToString(node)<<endl;
            }
          }
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

  // this function is obsolete
  // introduces assignment for initialization of variable declaration
  SgStatement* Normalization::buildNormalizedVariableDeclaration(SgVariableDeclaration* varDecl) {
    ROSE_ASSERT(varDecl);
    // determine scope of variable within which it can be normalized
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

  string Normalization::newTmpVarName() {
    return tmpVarPrefix + StringUtility::numberToString(Normalization::tmpVarNr++);
  }

  void Normalization::normalizeAst(SgNode* root, unsigned int level) {
    options.configureLevel(level);
    normalizeAst(root);
#if 0
    // AST consistency tests
    if(SgProject* project=isSgProject(root)) {
      AstTests::runAllTests(project);
      AstPostProcessing(project);
    }
#endif
  }
  void Normalization::normalizeLabelStmts(SgNode* root) {
    RoseAst ast(root);
    list<SgLabelStatement*> list;
    // first determine statements to be normalized, then transform.
    for(auto node:ast) {
      if(SgLabelStatement* labelStmt=isSgLabelStatement(node)) {
        list.push_back(labelStmt);
      }
    }
    for(auto labelStmt:list) {
      normalizeLabel(labelStmt);
    }
  }

  void Normalization::normalizeAst(SgNode* root) {
    if(options.normalizeSingleStatements) {
      normalizeSingleStatementsToBlocks(root);
    }
    if(options.normalizeLabels) {
      normalizeLabelStmts(root);
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
      normalizeAllVariableDeclarations(root,false);
    }
    if(options.normalizeVariableDeclarationsWithFunctionCalls) {
      bool normalizeOnlyVariablesWithFunctionCallsFlag=true;
      normalizeAllVariableDeclarations(root,normalizeOnlyVariablesWithFunctionCallsFlag);
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
    // check if this statement has an attached label and if yes: normalize label
    if(SgLabelStatement* label=isSgLabelStatement(stmt->get_parent())) {
      normalizeLabel(label);
    }
    SgNode* condNode=SgNodeHelper::getCond(stmt);
    ROSE_ASSERT(condNode);
    if(isSgExprStatement(condNode)) {
      condNode=SgNodeHelper::getExprStmtChild(condNode);
    }
    SgExpression* condExpr=isSgExpression(condNode);
    ROSE_ASSERT(condExpr);
    if(isSgIfStmt(stmt)||isSgSwitchStatement(stmt)) {
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
      // cases if and switch
      SageInterface::insertStatementBefore(stmt, tmpVarDeclaration);
      
    } else if(isSgWhileStmt(stmt)||isSgDoWhileStmt(stmt)) {
      // transformation: while(C) {...} ==> while(1) { T t=C;if(t) break; ...} (implemented)
      // alternative: while(C) {...} ==> T t=C; while(t) { ...; t=C; } (duplicates condition, not implemented)
      // transformation: do {...} whilte (C) ==> do {...; T t=C; if(t) break; } (implemented)
      // note: do {...} whilte (C) ==> do {...; T t=C;} while(t) (not possible because of C/C++ scoping rules)

      // (i) replace while-condition with constant 1 condition
      SgStatement* oldWhileCond=isSgStatement(SgNodeHelper::getCond(stmt));
      SgExprStatement* exprStmt=SageBuilder::buildExprStatement(SageBuilder::buildIntValHex(1));
      SgNodeHelper::setCond(stmt,exprStmt);
      exprStmt->set_parent(stmt);

      // (iii) generate if-statement with old while-condition
      // (iii.1) generate not operator to negate while condition
      // (iii.2) build if stmt and insert into while/do-while loop
      SgStatement* negatedOldWhileCond=0;
      if(SgExprStatement* oldWhileCondExprStmt=isSgExprStatement(oldWhileCond)) {
        SgExpression* oldWhileCondExpr=oldWhileCondExprStmt->get_expression();
        ROSE_ASSERT(oldWhileCondExpr);
        oldWhileCondExpr->set_parent(0);
        SgExpression* negatedOldWhileCondExpr=SageBuilder::buildNotOp(oldWhileCondExpr);
        ROSE_ASSERT(negatedOldWhileCondExpr);
        negatedOldWhileCondExpr->set_parent(0);
        ROSE_ASSERT(negatedOldWhileCondExpr);
        oldWhileCondExprStmt->set_expression(negatedOldWhileCondExpr);
        negatedOldWhileCondExpr->set_parent(oldWhileCondExprStmt);
        negatedOldWhileCond=oldWhileCondExprStmt;
      } else {
        cerr<<"Error: Conditional of while-stmt not an expression ("<<oldWhileCond->class_name()<<"). Requires normalization."<<endl;
        exit(1);
      }
      
      ROSE_ASSERT(negatedOldWhileCond);
      SgIfStmt* ifStmt=SageBuilder::buildIfStmt(negatedOldWhileCond,
                                                SageBuilder::buildBreakStmt(),
                                                0);
      SgScopeStatement* body=isSgScopeStatement(SgNodeHelper::getLoopBody(stmt));
      ROSE_ASSERT(body);
      // (iv) insert if-statement
      if(isSgWhileStmt(stmt)) {
        // while loop
        SageInterface::prependStatement(ifStmt,body);
      } else {
        // do-while loop
        SageInterface::appendStatement(ifStmt,body);
      }
    
      // (v) hoistCondition from generated if-statement (recursive application of condition hoisting)
      hoistCondition(ifStmt);

    } else {
      cerr<<"Error: unsupported stmt selected for condition normalization at "<<SgNodeHelper::sourceLineColumnToString(stmt)<<endl;
      exit(1);
    }
  }

  void Normalization::hoistConditionsInAst(SgNode* node, bool onlyNormalizeFunctionCallExpressions) {
    list<SgStatement*> hoistingTransformationList;
    RoseAst ast(node);
    // build list of stmts to transform
    for (auto node : ast) {
      if(SgNodeHelper::isCond(node)) {
        SgStatement* stmt=isSgStatement(node->get_parent());
        if(isSgIfStmt(stmt)||isSgSwitchStatement(stmt)||isSgWhileStmt(stmt)||isSgDoWhileStmt(stmt)) {
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
    // find all SgExprStatement, SgReturnStmt, SgVariableDeclaration
    RoseAst ast(node);
    for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
      // match on expr stmts and transform the expression
      SgStatement* stmt=0;
      SgExpression* expr=0;
      if(SgExprStatement* exprStmt=isSgExprStatement(*i)) {
        if(!SgNodeHelper::isCond(exprStmt)) {
          expr=exprStmt->get_expression();
          stmt=exprStmt;
        }
      } else if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
        expr=SgNodeHelper::getInitializerExpressionOfVariableDeclaration(varDecl);
        stmt=varDecl;
      }
      if(SgReturnStmt* returnStmt=isSgReturnStmt(*i)) {
        //cout<<"Found SgReturnStmt: "<<(*i)->unparseToString()<<endl;
        // TODO: normalization
        expr=returnStmt->get_expression();
        stmt=returnStmt;
        i.skipChildrenOnForward();
      }
      if(stmt&&expr) {
        if(isWithinBlockStmt(expr)) {
          if(onlyNormalizeFunctionCallExpressions) {
            if(hasFunctionCall(expr)) {
              normalizeExpression(stmt,expr);
            }
          } else {
            //cout<<"DEBUG: normalizing "<<(expr)->unparseToString()<<endl;
            normalizeExpression(stmt,expr);
          }
        }
        i.skipChildrenOnForward();
      }
    }
    for(ExprTransformationList::iterator i=exprTransformationList.begin();i!=exprTransformationList.end();++i) {
      SubExprTransformationList subExprTransformationList=*i;
      for(SubExprTransformationList::iterator j=subExprTransformationList.begin();j!=subExprTransformationList.end();++j) {
        SgStatement* stmt=(*j).stmt;
        SgExpression* expr=(*j).expr;
        cout<<"DEBUG: TRANSFORM AT: "<<(*j).transformation<<" : "<<expr<<endl;
        switch((*j).transformation) {
        case Normalization::GEN_TMPVAR: {
          // 1) generate tmp-var initializer with expr as lhs
          SgVariableDeclaration* tmpVarDeclaration = 0;
          SgExpression* tmpVarReference = 0;
          SgScopeStatement* scope=stmt->get_scope();
          tie(tmpVarDeclaration, tmpVarReference) = SageInterface::createTempVariableAndReferenceForExpression(expr, scope);
          tmpVarDeclaration->set_parent(scope);
          ROSE_ASSERT(tmpVarDeclaration!= 0);
          // 2) insert tmp-var initializer
          if(SgBasicBlock* block=isSgBasicBlock(stmt)) {
            block->append_statement(tmpVarDeclaration);
          } else {
            SageInterface::insertStatementBefore(stmt, tmpVarDeclaration);
          }

          // 2) replace use of expr with tmp-var
          SageInterface::replaceExpression(expr, tmpVarReference);
          //cout<<"tmp"<<tmpVarNr<<": replaced @"<<(stmt)->unparseToString()<<" inserted: "<<tmpVarDeclaration->unparseToString()<<endl;
          break;
        }
        case Normalization::GEN_FALSE_BOOL_VAR_DECL: {
          SgVariableDeclaration* decl=(*j).decl;
          cout<<"GENERATING BOOL VAR DECL:"<<endl;
          break;
        }
        default:
          cerr<<"Error: Normalization: unknown subexpression transformation: "<<(*j).transformation<<endl;
          exit(1);
        } // end of switch
        tmpVarNr++;
      }
    }
  }
  
  // stmt is only passed through and used to determine the scope when generating tmp-variables
  void Normalization::normalizeExpression(SgStatement* stmt, SgExpression* expr) {
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
    // for each expression one SubExprTransformationList is inserted
    exprTransformationList.push_back(subExprTransformationList);
  }

  Normalization::RegisteredSubExprTransformation::RegisteredSubExprTransformation(SubExprTransformationEnum t,SgStatement* s, SgExpression* e)
    : transformation(t),
      stmt(s),
      expr(e),
      decl(0)
  {
  }
  Normalization::RegisteredSubExprTransformation::RegisteredSubExprTransformation(SubExprTransformationEnum t,SgVariableDeclaration* d)
    : transformation(t),
      stmt(0),
      expr(0),
      decl(d)
  {
  }

  // stmt is only used to detetermined scope, which is used when generating the tmp-variable.
  void Normalization::normalizeSubExpression(SgStatement* stmt, SgExpression* expr, SubExprTransformationList& subExprTransformationList) {
    /*if(SgCastExp* castExp=isSgCastExp(expr)) {
      normalizeSubExpression(stmt,castExp->get_operand(),subExprTransformationList);
      } else*/ 
    if(SgPntrArrRefExp* arrExp=isSgPntrArrRefExp(expr)) {
      // special case: normalize array index-expressions
      normalizeSubExpression(stmt,isSgExpression(SgNodeHelper::getRhs(arrExp)),subExprTransformationList);
      registerTmpVarAssignment(stmt,expr,subExprTransformationList);
    } else if(isSgAssignOp(expr)||isSgCompoundAssignOp(expr)) {
      // special case: normalize assignment with lhs/rhs-semantics
      // normalize rhs of assignment
      normalizeSubExpression(stmt,isSgExpression(SgNodeHelper::getRhs(expr)),subExprTransformationList);
      // normalize lhs of assignment
      SgExpression* lhs=isSgExpression(SgNodeHelper::getLhs(expr));
      ROSE_ASSERT(lhs);
      // skip normalizing top-most operator of lhs because a
      // lhs-expression must remain a lhs-expression! Introduction of
      // temporary would be wrong. Note: not all operators can appear as top-most op on lhs.
      if(isSgUnaryOp(lhs)) {
        normalizeSubExpression(stmt,isSgExpression(SgNodeHelper::getUnaryOpChild(lhs)),subExprTransformationList);
      } else if(isSgBinaryOp(lhs)) {
        normalizeSubExpression(stmt,isSgExpression(SgNodeHelper::getRhs(lhs)),subExprTransformationList);
        normalizeSubExpression(stmt,isSgExpression(SgNodeHelper::getLhs(lhs)),subExprTransformationList);
      }
    } else if(SgFunctionCallExp* funCallExp=isSgFunctionCallExp(expr)) {
      // special case: function call with normalization of arguments
      // and void return type (no temp var generation)
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
        registerTmpVarAssignment(stmt,expr,subExprTransformationList);
      }
    } else if(SgAndOp* andOp=isSgAndOp(expr)) {
      // special case: short circuit operator normalization
      normalizeSubExpression(stmt,isSgExpression(SgNodeHelper::getLhs(expr)),subExprTransformationList);
      normalizeSubExpression(stmt,isSgExpression(SgNodeHelper::getRhs(expr)),subExprTransformationList);
      registerTmpVarAssignment(stmt,expr,subExprTransformationList);
    } else if(SgOrOp* orOp=isSgOrOp(expr)) {
      // special case: short circuit operator normalization
      cerr<<"DEBUG: found OrOp"<<endl;
      SgScopeStatement* scope=stmt->get_scope();
      SgVariableDeclaration* decl=generateFalseBoolVarDecl(scope);
      registerFalseBoolVarDecl(decl,subExprTransformationList);
      normalizeSubExpression(stmt,isSgExpression(SgNodeHelper::getLhs(expr)),subExprTransformationList);
      normalizeSubExpression(stmt,isSgExpression(SgNodeHelper::getRhs(expr)),subExprTransformationList);
      //registerTmpVarAssignment(stmt,expr,subExprTransformationList);
    } else if(isSgBinaryOp(expr)) {
      // general case: binary operator
      normalizeSubExpression(stmt,isSgExpression(SgNodeHelper::getLhs(expr)),subExprTransformationList);
      normalizeSubExpression(stmt,isSgExpression(SgNodeHelper::getRhs(expr)),subExprTransformationList);
      registerTmpVarAssignment(stmt,expr,subExprTransformationList);
    } else if(isSgUnaryOp(expr)) {
      // general case: unary operator
      normalizeSubExpression(stmt,isSgExpression(SgNodeHelper::getUnaryOpChild(expr)),subExprTransformationList);
      registerTmpVarAssignment(stmt,expr,subExprTransformationList);
    }
  }

  void Normalization::registerFalseBoolVarDecl(SgVariableDeclaration* decl, SubExprTransformationList& subExprTransformationList) {
    subExprTransformationList.push_back(RegisteredSubExprTransformation(Normalization::GEN_FALSE_BOOL_VAR_DECL,decl));
  }

  SgVariableDeclaration* Normalization::generateFalseBoolVarDecl(SgScopeStatement* scope) {
    string varName=newTmpVarName();
    SgType* varType=SageBuilder::buildBoolType();
    SgBoolValExp* falseVal=SageBuilder::buildBoolValExp(false);
    SgAssignInitializer* varInit=SageBuilder::buildAssignInitializer(falseVal, varType);
    SgVariableDeclaration* decl=SageBuilder::buildVariableDeclaration(varName, varType, varInit, scope);
    return decl;
  }

  void Normalization::registerTmpVarAssignment(SgStatement* stmt, SgExpression  * expr, SubExprTransformationList& subExprTransformationList) {
    subExprTransformationList.push_back(RegisteredSubExprTransformation(Normalization::GEN_TMPVAR,stmt,expr));
  }

  bool Normalization::isWithinBlockStmt(SgExpression* exp) {
    SgNode* current=exp;
    while(!isSgGlobal(current)&&!isSgBasicBlock(current)&&current) {
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
  // ==>  Label: (function-scope is inferred from 'target')
  SgLabelStatement* Normalization::createLabel(SgStatement* target) {
    SgLabelStatement* newLabel =
      SageBuilder::buildLabelStatement(Normalization::newLabelName(),
                                       SageBuilder::buildBasicBlock(),
                                       // MS: scope should be function scope?
                                       isSgScopeStatement(target->get_parent()));
    return newLabel;
  }
  
  // creates a goto-stmt and inserts the goto-stmt referring to the
  // provided label before statement 'target'.
  // Label: ... targetStmt; ==>  Label: ... goto Label; targetStmt;
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

