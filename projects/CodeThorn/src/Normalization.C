#include "sage3basic.h"
#include "SingleStatementToBlockNormalization.h"

#include "Normalization.h"
#include "NormalizationCxx.h"
#include "RoseAst.h"
#include "SgNodeHelper.h"
#include "CFAnalysis.h"
#include <list>
#include "AstTerm.h"
#include "AstConsistencyTests.h"

#include "AstFixup.h"
#include "astPostProcessing.h"

#include "Diagnostics.h"
#include "sageGeneric.h"

using namespace Sawyer::Message;

// Author: Markus Schordan, 2018

using namespace std;
using namespace Rose;
using namespace Sawyer::Message;

namespace CodeThorn {

  int32_t Normalization::uniqueVarCounter=1;
  int32_t Normalization::labelNr=1;
  string Normalization::labelPrefix="__label";

  Normalization::TmpVarNrType Normalization::tmpVarNrCounter=1;
  string Normalization::tmpVarPrefix="__logOpTmp";

  Sawyer::Message::Facility Normalization::logger;
  void Normalization::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
      initialized = true;
      logger = Sawyer::Message::Facility("CodeThorn::Normalization", Rose::Diagnostics::destination);
      Rose::Diagnostics::mfacilities.insertAndAdjust(logger);
    }
  }

  Normalization::Normalization() {
    _inliner=new RoseInliner();
  }
  Normalization::~Normalization() {
    if(_defaultInliner) {
      delete _inliner;
    }
  }

  // configuration
  void Normalization::setTmpVarPrefix(std::string prefix) {
    Normalization::tmpVarPrefix=prefix;
  }

  void Normalization::setLabelPrefix(std::string prefix) {
    Normalization::labelPrefix=prefix;
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

      // obsolete, replaces initializers with assignments
      normalizeVariableDeclarations=false;

      // obsolete
      normalizeVariableDeclarationsWithFunctionCalls=false;

      eliminateShortCircuitOperators=true;
      //eliminateConditionalExpressionOp=true;

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
      //eliminateShortCircuitOperators=true;
      //eliminateConditionalExpressionOp=true;
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

  void Normalization::normalizeAst(SgNode* root) {
    options.configureLevel(2); // default level of normalization
    normalizeAstPhaseByPhase(root);
  }

  void Normalization::normalizeAst(SgNode* root, unsigned int level) {
    options.configureLevel(level);
    normalizeAstPhaseByPhase(root);
#if 0
    // AST consistency tests
    if(SgProject* project=isSgProject(root)) {
      AstTests::runAllTests(project);
      AstPostProcessing(project);
    }
#endif
  }

  void Normalization::normalizeAstPhaseByPhase(SgNode* root) {
    if (options.normalizeCplusplus) {
      normalizeCxx(*this, root);
    }
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
    if(options.hoistBranchInitStatements) {
      hoistBranchInitStatementsInAst(root);
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

  Normalization::RegisteredSubExprTransformation::RegisteredSubExprTransformation(SubExprTransformationEnum t,SgStatement* s, SgExpression* e)
    : transformation(t),
      stmt(s),
      expr(e),
      decl(0)
  {
  }
  Normalization::RegisteredSubExprTransformation::RegisteredSubExprTransformation(SubExprTransformationEnum t,Normalization::TmpVarNrType tmpVarNrParam, SgStatement* s, SgExpression* e)
    : transformation(t),
      stmt(s),
      expr(e),
      tmpVarNr(tmpVarNrParam)
  {
  }
  Normalization::RegisteredSubExprTransformation::RegisteredSubExprTransformation(SubExprTransformationEnum t,Normalization::TmpVarNrType tmpVarNrParam, SgStatement* s, SgExpression* e, Normalization::TmpVarNrType declVarNr)
    : transformation(t),
      stmt(s),
      expr(e),
      tmpVarNr(tmpVarNrParam),
      declVarNr(declVarNr)
  {
  }
  Normalization::RegisteredSubExprTransformation::RegisteredSubExprTransformation(SubExprTransformationEnum t,SgStatement* s, SgExpression* e, Normalization::TmpVarNrType declVarNr)
    : transformation(t),
      stmt(s),
      expr(e),
      declVarNr(declVarNr)
  {
  }
  Normalization::RegisteredSubExprTransformation::RegisteredSubExprTransformation(SubExprTransformationEnum t,SgStatement* s, SgExpression* e, Normalization::TmpVarNrType declVarNr, SgStatement* trueBody, SgStatement* falseBody)
    : transformation(t),
      stmt(s),
      expr(e),
      trueBody(trueBody),
      falseBody(falseBody),
      declVarNr(declVarNr)
  {
  }

  /***************************************************************************
   * QUERY FUNCTIONS
   **************************************************************************/

  bool Normalization::isVarDeclWithFunctionCall(SgNode* node) {
    return SgNodeHelper::Pattern::matchVariableDeclarationWithFunctionCall(node);
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

  /***************************************************************************
   * INLINING (obsolete)
   **************************************************************************/

  void Normalization::setInliningOption(bool flag) {
    options.inlining=flag;
  }

  bool Normalization::getInliningOption() {
    return options.inlining;
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
  void Normalization::setInliner(CodeThorn::InlinerBase* userDefInliner) {
    removeDefaultInliner();
    _inliner=userDefInliner;
  }

  /***************************************************************************
   * LABEL NORMALIZATION
   **************************************************************************/

  // Label normalization (breaks up attached labels into separate statements)
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

  /***************************************************************************
   * DECLARATION CONVERSION (obsolete)
   **************************************************************************/

  // converting variable initializations into declarations with separate assignment (obsolete)
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
              SAWYER_MESG(logger[TRACE])<<"Normalizing variable initializer with function call: "<<SgNodeHelper::lineColumnNodeToString(node)<<endl;
            }
          }
          if(SgStatement* newVarAssignment=buildNormalizedVariableDeclaration(varDecl)) {
            declAssignList.push_back(std::make_pair(varDecl,newVarAssignment));
          }
        }
        i.skipChildrenOnForward();
      } else {
        SAWYER_MESG(logger[TRACE])<<"NOT a variable declaration: "<<node->class_name()<<endl;
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
      SAWYER_MESG(logger[TRACE])<<"normalizing decl: "<<varDecl->unparseToString()<<endl;
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

  /***************************************************************************
   * BLOCK NORMALIZATION
   **************************************************************************/

  void Normalization::normalizeSingleStatementsToBlocks(SgNode* root) {
    SingleStatementToBlockNormalizer singleStatementToBlockNormalizer;
    singleStatementToBlockNormalizer.Normalize(root);
  }

  /***************************************************************************
   * UTILITY FUNCTIONS
   **************************************************************************/

  string Normalization::newTmpVarName() {
    return tmpVarPrefix + StringUtility::numberToString(Normalization::tmpVarNrCounter++);
  }

  // Level 3 normalization (not enabled)
  string Normalization::newLabelName() {
    return labelPrefix + StringUtility::numberToString(Normalization::labelNr++);
  }

  SgClassDeclaration* Normalization::isSpecialization(SgNode* node) {
    if(SgClassDeclaration* classDecl=isSgClassDeclaration(node)) {
      if(classDecl->isSpecialization()) {
        return classDecl;
      }
    }
    return 0;
  }

  bool Normalization::isTemplateInstantiationNode(SgNode* node) {
    return RoseAst::isTemplateInstantiationNode(node);
  }

  bool Normalization::isTemplateNode(SgNode* node) {
    return RoseAst::isTemplateNode(node);
  }

  /***************************************************************************
   * HOISTING OF INIT STATEMENTS (if, switch)
   **************************************************************************/

struct GetInitStatement
{
    typedef std::pair<SgVariableDeclaration*, SgScopeStatement*> matched_node;
    typedef std::vector<matched_node>                            container;

    explicit
    GetInitStatement(container& results)
    : res(results)
    {}

    void handleInitStatement(SgStatement& init, SgScopeStatement& branch);

    void handle(SgNode&) {}

    void handle(SgIfStmt& n)
    {
      handleInitStatement(SG_DEREF(n.get_conditional()), n);
    }

    void handle(SgSwitchStatement& n)
    {
      handleInitStatement(SG_DEREF(n.get_item_selector()), n);
    }

  private:
    container& res;
};

void GetInitStatement::handleInitStatement(SgStatement& init, SgScopeStatement& branch)
{
  SgVariableDeclaration* var = isSgVariableDeclaration(&init);

  // \todo handle C++17 init statements
  if (var == nullptr) return;

  res.push_back(std::make_pair(var, &branch));
}

struct SetBranchCondition
{
  void handle(SgNode& n)            { SG_UNEXPECTED_NODE(n); }
  void handle(SgIfStmt& n)          { n.set_conditional(cond); }
  void handle(SgSwitchStatement& n) { n.set_item_selector(cond); }

  SgExprStatement* cond;
};



static
void hoistBranchInitStatement(GetInitStatement::matched_node n)
{
  SgBasicBlock*    block = SageBuilder::buildBasicBlock();

  SageInterface::replaceStatement(n.second /*branch*/, block);
  block->append_statement(n.second);
  SageInterface::moveVariableDeclaration(n.first /*var decl*/, block);

  SgExpression*    cond = SageBuilder::buildVarRefExp(n.first);
  ROSE_ASSERT(cond);

  SgExprStatement* stmt = SageBuilder::buildExprStatement(cond);
  ROSE_ASSERT(stmt);

  stmt->set_parent(n.second);
  sg::dispatch( SetBranchCondition{stmt}, n.second );
}

void Normalization::hoistBranchInitStatementsInAst(SgNode* node)
{
    GetInitStatement::container hoistingTransformationList;

    RoseAst ast(node);
    // build list of stmts to transform
    for (auto i=ast.begin();i!=ast.end();++i) {
      // TEMPLATESKIP this will skip all templates that are found (mostly in header files). This could also be integrated into the iterator itself.
      if(isTemplateNode(*i)) {
        i.skipChildrenOnForward();
        continue;
      }

      sg::dispatch(GetInitStatement(hoistingTransformationList), *i);
    }

    // transform stmts
    for (auto match: hoistingTransformationList) {
      hoistBranchInitStatement(match);
    }
  }

  /***************************************************************************
   * HOISTING OF CONDITION EXPRESSIONS (if, switch, do, do-while)
   **************************************************************************/

  void Normalization::hoistConditionsInAst(SgNode* node, bool onlyNormalizeFunctionCallExpressions) {
    list<SgStatement*> hoistingTransformationList;
    RoseAst ast(node);
    // build list of stmts to transform
    for (auto i=ast.begin();i!=ast.end();++i) {
      // TEMPLATESKIP this will skip all templates that are found (mostly in header files). This could also be integrated into the iterator itself.
      if(isTemplateNode(*i)) {
        i.skipChildrenOnForward();
        continue;
      }
      SgNode* node=*i;
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

  // transformation: if(C) ... => T t=C; if(t) ...
   // transformation: switch(C) ... => T t=C; switch(t) ...
  // while/do-while/for: (see below)
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

    if(isSgIfStmt(stmt)||isSgSwitchStatement(stmt)) {
      if(SgExpression* condExpr=isSgExpression(condNode)) {
      // (i) build tmp var with cond as initializer
      SgScopeStatement* scope=stmt->get_scope();
      auto tmpVarDeclaration=buildVariableDeclarationWithInitializerForExpression(condExpr, scope);
      tmpVarDeclaration->set_parent(scope);
      auto tmpVarReference=buildVarRefExpForVariableDeclaration(tmpVarDeclaration);
      ROSE_ASSERT(tmpVarDeclaration!= 0);

      // (ii) replace cond with new tmp-varref
      bool deleteReplacedExpression=false;
      SgNodeHelper::replaceExpression(condExpr,tmpVarReference,deleteReplacedExpression);

      // (iii) insert declaration with initializer before stmt
      // cases if and switch
      SageInterface::insertStatementBefore(stmt, tmpVarDeclaration);
      } else if(SgVariableDeclaration* condVarDecl=isSgVariableDeclaration(condNode)) {
        cerr<<"Error at "<<SgNodeHelper::sourceFilenameLineColumnToString(stmt)<<endl;
        cerr<<"Error: Normalization: Variable declaration in condition of if statement. Not supported yet."<<endl;
        exit(1);

        // temporary "fix"

        //if (!condExpr && isSgIfStmt(stmt)) {
          // PP (03/02/20) handle variable declarations in conditions
        //  SgVariableDeclaration* condVar = isSgVariableDeclaration(condNode);

        //  if (!condVar)
        //    std::cerr << typeid(*condNode).name() << "\n" << stmt->unparseToString() << std::endl;

        //  ROSE_ASSERT(condVar);

          // \todo do we also have to fix up the symbol scope?

        //  SgVarRefExp*           condRef = SageBuilder::buildVarRefExp(condVar);
        //  SgExprStatement*       refStmt = SageBuilder::buildExprStatement(condRef);

        //  SageInterface::replaceStatement(condVar, refStmt, true /* movePreprocessingInfo */);
        //  SageInterface::insertStatementBefore(stmt, condVar);
        //  return;
        //}

        //if (!condExpr)
        //  std::cerr << typeid(*condNode).name() << "\n" << stmt->unparseToString() << std::endl;

      } else {
        cerr<<"Error at "<<SgNodeHelper::sourceFilenameLineColumnToString(stmt)<<endl;
        cerr<<"Error: Normalization: Unknown language construct in condition of if statement. Not supported."<<endl;
        exit(1);

      }

    } else if(isSgWhileStmt(stmt)||isSgDoWhileStmt(stmt)) {
      // transformation: while(C) {...} ==> while(1) { T t=C;if(t) break; ...} (implemented)
      // alternative: while(C) {...} ==> T t=C; while(t) { ...; t=C; } (duplicates condition, not implemented)
      // transformation: do {...} while (C) ==> do {...; T t=C; if(t) break; } (implemented)
      // alternative: do {...} while (C) ==> do {...; T t=C; } while(t); (not possible because of C/C++ scoping rules)

      // (i) replace while-condition with constant 1 condition
      SgStatement* oldWhileCond=isSgStatement(SgNodeHelper::getCond(stmt));
      if(isSgVariableDeclaration(oldWhileCond)) {
        cerr<<"Error at "<<SgNodeHelper::sourceFilenameLineColumnToString(stmt)<<endl;
        cerr<<"Error: Normalization: Variable declaration in condition of while or do-while statement. Not supported yet."<<endl;
        exit(1);
      }
      SgExprStatement* exprStmt=SageBuilder::buildExprStatement(SageBuilder::buildIntValHex(1));
      SgNodeHelper::setCond(stmt,exprStmt);
      exprStmt->set_parent(stmt);

      // (ii) generate if-statement with old while-condition
      // (ii.1) generate not-operator to negate while condition
      // (ii.2) build if-stmt and insert into while/do-while loop
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
      // (iii) insert if-statement
      if(isSgWhileStmt(stmt)) {
        // while loop
        SageInterface::prependStatement(ifStmt,body);
      } else {
        // do-while loop
        SageInterface::appendStatement(ifStmt,body);
      }

      // (iv) hoistCondition from generated if-statement (recursive application of condition hoisting)
      hoistCondition(ifStmt);

    } else {
      cerr<<"Error: unsupported stmt selected for condition normalization at "<<SgNodeHelper::sourceLineColumnToString(stmt)<<endl;
      exit(1);
    }
  }

  /***************************************************************************
   * NORMALIZE EXPRESSIONS
   **************************************************************************/

  void Normalization::normalizeExpressionsInAst(SgNode* node, bool onlyNormalizeFunctionCallExpressions) {
    // find all expressions in SgExprStatement, SgReturnStmt,
    // SgVariableDeclaration. Conditions are normalized in previous
    // normalization steps to have only one variable in the condition
    // expression
    RoseAst ast(node);
    // phase one: generate transformation sequence for expression
    for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
      // match on expr stmts and transform the expression
      SgStatement* stmt=0;
      SgExpression* expr=0;

      // TEMPLATESKIP this will skip all templates that are found (mostly in header files). This could also be integrated into the iterator itself.
      if(isTemplateNode(*i)) {
        i.skipChildrenOnForward();
        continue;
      }

      if(SgExprStatement* exprStmt=isSgExprStatement(*i)) {
        if(!SgNodeHelper::isCond(exprStmt)) {
          stmt=exprStmt;
          expr=exprStmt->get_expression();
        }
      } else if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
        stmt=varDecl;
        expr=SgNodeHelper::getInitializerExpressionOfVariableDeclaration(varDecl);
      }
      if(SgReturnStmt* returnStmt=isSgReturnStmt(*i)) {
        SAWYER_MESG(logger[TRACE])<<"Found SgReturnStmt: "<<(*i)->unparseToString()<<endl;
        stmt=returnStmt;
        expr=returnStmt->get_expression();
        i.skipChildrenOnForward();
      }
      if(stmt&&expr) {
        if(isWithinBlockStmt(expr)) {
          if(onlyNormalizeFunctionCallExpressions) {
            if(hasFunctionCall(expr)) {
              normalizeExpression(stmt,expr);
            }
          } else {
            normalizeExpression(stmt,expr);
          }
        }
        i.skipChildrenOnForward();
      }
    }
    // phase two: apply transformation sequence
    for(ExprTransformationList::iterator i=exprTransformationList.begin();i!=exprTransformationList.end();++i) {
      SubExprTransformationList subExprTransformationList=*i;
      for(SubExprTransformationList::iterator j=subExprTransformationList.begin();j!=subExprTransformationList.end();++j) {
        SgStatement* stmt=(*j).stmt;
        SgExpression* expr=(*j).expr;
        SAWYER_MESG(logger[TRACE])<<"TRANSFORMATION "<<(*j).transformation<<" at "<<expr<<endl;
        switch((*j).transformation) {
        case Normalization::GEN_STMT_REMOVAL: {
          SAWYER_MESG(logger[TRACE])<<"GENERATING STMT REMOVAL:"<<endl;
          SageInterface::removeStatement(stmt);
          break;
        }
        case Normalization::GEN_TMP_VAR_INIT: {
          SAWYER_MESG(logger[TRACE])<<"GENERATING TMP VAR INIT:"<<endl;
          // special cases
          // i) generate tmp-var initializer with expr as lhs
          SgScopeStatement* scope=stmt->get_scope();
          bool shareExpression=false;
          auto tmpVarDeclaration=buildVariableDeclarationWithInitializerForExpression(expr,scope,shareExpression);
          addToTmpVarMapping((*j).tmpVarNr,tmpVarDeclaration);

          ROSE_ASSERT(tmpVarDeclaration);
          tmpVarDeclaration->set_parent(scope);
          auto tmpVarReference=buildVarRefExpForVariableDeclaration(tmpVarDeclaration);
          ROSE_ASSERT(tmpVarReference);

          // ii) insert tmp-var initializer
          insertNormalizedSubExpressionFragment(tmpVarDeclaration,stmt);
          // ii) replace use of expr with tmp-var
          bool deleteReplacedExpression=false;
          SgNodeHelper::replaceExpression(expr,tmpVarReference,deleteReplacedExpression);
          //SAWYER_MESG(logger[TRACE])<<"inserted: "<<tmpVarDeclaration->unparseToString()<<endl;
          break;
        }
        case Normalization::GEN_VOID_EXPR: {
          SAWYER_MESG(logger[TRACE])<<"GENERATING VOID EXPR:"<<endl;
          // only copy expression, no tmpvar
          SgExpression* exprCopy=SageInterface::copyExpression((*j).expr);
          ROSE_ASSERT(exprCopy);
          //SAWYER_MESG(logger[TRACE])<<"exprCopy: "<<exprCopy->unparseToString()<<endl; (causes ROSE warning)
          insertNormalizedSubExpressionFragment(SageBuilder::buildExprStatement(exprCopy),(*j).stmt);
          break;
        }
        case Normalization::GEN_TMP_VAR_DECL: {
          SAWYER_MESG(logger[TRACE])<<"GENERATING TMP VAR DECL:"<<endl;
          SgScopeStatement* scope=stmt->get_scope();
          SgVariableDeclaration* tmpVarDeclaration=generateVarDecl((*j).tmpVarDeclType,scope);
          tmpVarDeclaration->set_parent(stmt->get_parent());
          ROSE_ASSERT(tmpVarDeclaration);
          // using declVarNr instead of tmpVarNr for control-flow operator transformations
          addToTmpVarMapping((*j).tmpVarNr,tmpVarDeclaration);
          insertNormalizedSubExpressionFragment(tmpVarDeclaration,stmt);
          break;
        }
        case Normalization::GEN_FALSE_BOOL_VAR_DECL: {
          SAWYER_MESG(logger[TRACE])<<"GENERATING FALSE BOOL VAR DECL:"<<endl;
            SgScopeStatement* scope=stmt->get_scope();
            SgVariableDeclaration* tmpVarDeclaration=generateFalseBoolVarDecl(scope);
            tmpVarDeclaration->set_parent(stmt->get_parent());
            ROSE_ASSERT(tmpVarDeclaration);
            // using declVarNr instead of tmpVarNr for control-flow operator transformations
            addToTmpVarMapping((*j).declVarNr,tmpVarDeclaration);
            insertNormalizedSubExpressionFragment(tmpVarDeclaration,stmt);
          break;
        }
        case Normalization::GEN_IF_ELSE_STMT: {
          // declVarNr is the tmp var nr of the binary log op
          TmpVarNrType condTmpVarNr=(*j).declVarNr;
          SAWYER_MESG(logger[TRACE])<<"GENERATING IF ELSE STMT: condtmpvarNr "<<condTmpVarNr<<endl;
          ROSE_ASSERT(isValidGeneratedTmpVarDeclNr(condTmpVarNr));
          SgExpression* cond=getVarRefExp(condTmpVarNr);
          SgStatement* true_body=(*j).trueBody;
          SgStatement* false_body=(*j).falseBody;
          SgIfStmt* ifStmt=Normalization::generateIfElseStmt(cond,true_body,false_body);
          insertNormalizedSubExpressionFragment(ifStmt,stmt);
          break;
        }

        case Normalization::GEN_BOOL_VAR_IF_ELSE_STMT: {
          SAWYER_MESG(logger[TRACE])<<"GENERATING BOOL VAR IF ELSE STMT: logOpTmpVarNr: "<<(*j).declVarNr<<" condVarNr: "<<(*j).condVarNr<<endl;
          SgVariableDeclaration* decl=getVarDecl((*j).declVarNr);
          SgVarRefExp* varRefExp=SageBuilder::buildVarRefExp(decl);
          SgScopeStatement* scope=stmt->get_scope();
          SgExpression* cond=getVarRefExp((*j).condVarNr);
          SgStatement* true_body=(*j).trueBody;
          SgStatement* false_body=(*j).falseBody;
          SgIfStmt* ifStmt=Normalization::generateBoolVarIfElseStmt(cond,varRefExp,true_body,false_body,scope);
          insertNormalizedSubExpressionFragment(ifStmt,stmt);
          break;
        }
        case Normalization::GEN_BOOL_VAR_IF_STMT: {
          SAWYER_MESG(logger[TRACE])<<"GENERATING BOOL VAR IF STMT:"<<endl;
          SgVariableDeclaration* decl=getVarDecl((*j).declVarNr);
          SgVarRefExp* varRefExp=SageBuilder::buildVarRefExp(decl); // to be used in condition of if-stmt
          SgScopeStatement* scope=stmt->get_scope();
          SgExpression* cond=getVarRefExp((*j).declVarNr);
          SgStatement* true_body=(*j).trueBody;
          SgIfStmt* ifStmt=Normalization::generateBoolVarIfElseStmt(cond,varRefExp,true_body,0,scope);
          insertNormalizedSubExpressionFragment(ifStmt,stmt);
          break;
        }
        case Normalization::GEN_LOG_OP_REPLACEMENT: {
          //if(isSgOrOp(expr)||isSgAndOp(expr)) {
            // replace the binary logical operator with introduced tmp truth variable
            SgVariableDeclaration* decl=getVarDecl((*j).declVarNr);
            SgVarRefExp* varRefExp=SageBuilder::buildVarRefExp(decl);
            SAWYER_MESG(logger[TRACE])<<"GEN_LOG_OP: REPLACING "<<expr->unparseToString()<<" with tmp var."<<endl;
            SageInterface::replaceExpression(expr,varRefExp);
            //}
          break;
        }
        case Normalization::GEN_CONDOP_IF_ELSE_STMT: {
          SAWYER_MESG(logger[TRACE])<<"GENERATING CONDOP IF ELSE STMT: condvarnr:"<<(*j).condVarNr<<endl;
          SgExpression* cond=getVarRefExp((*j).condVarNr);
          SgStatement* true_body=(*j).trueBody;
          SgStatement* false_body=(*j).falseBody;
          SgIfStmt* ifStmt=Normalization::generateIfElseStmt(cond,true_body,false_body);
          insertNormalizedSubExpressionFragment(ifStmt,stmt);
          break;
        }
        case Normalization::GEN_TMP_VAR_ASSIGN: {
          SAWYER_MESG(logger[TRACE])<<"GENERATING TMP VAR ASSIGNMENT:"<<endl;
          SgExprStatement* tmpVarAssignment=generateTmpVarAssignment((*j).lhsTmpVarNr,(*j).rhsTmpVarNr);
          insertNormalizedSubExpressionFragment(tmpVarAssignment,stmt);
          break;
        }
        case Normalization::GEN_TMP_VAR_ASSIGN_WITH_EXPR: {
          SAWYER_MESG(logger[TRACE])<<"GENERATING TMP VAR ASSIGNMENT WITH EXPR:"<<endl;
          SgExprStatement* tmpVarAssignment=generateTmpVarAssignmentWithExpr((*j).tmpVarNr,expr);
          insertNormalizedSubExpressionFragment(tmpVarAssignment,stmt);
          break;
        }
        default:
          cerr<<"Error: Normalization: unknown subexpression transformation: "<<(*j).transformation<<endl;
          exit(1);
        } // end of switch
      } // end of transformation loop
    }
  }

  // stmt is only passed through and used to determine the scope when generating tmp-variables
  void Normalization::normalizeExpression(SgStatement* stmt, SgExpression* expr) {
    SAWYER_MESG(logger[TRACE])<<"normalizing "<<(expr)->unparseToString()<<endl;
    // clear mapping for each expression normalization
    tmpVarMapping.clear();
    SubExprTransformationList subExprTransformationList;
    if(!options.encapsulateNormalizedExpressionsInBlocks) {
      // normalized subexpressions (and declared variables) are replacing the current expression
      registerSubExpressionTempVars(stmt,expr,subExprTransformationList,false);
    } else {
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
      registerSubExpressionTempVars(stmt,expr,subExprTransformationList,false);
    }
    // for each expression one SubExprTransformationList is inserted
    exprTransformationList.push_back(subExprTransformationList);
  }

#if 0
  Normalization::TmpVarNrType Normalization::skipSubExpressionOperator(SgStatement* stmt, SgExpression* expr, SubExprTransformationList& subExprTransformationList,bool insideExprToBeEliminated) {
add
      ROSE_ASSERT(addressOfOp);
      if(isSgUnaryOp(addressOfOp)) {
        mostRecentTmpVarNr=registerSubExpressionTempVars(stmt,isSgExpression(SgNodeHelper::getUnaryOpChild(addressOfOp)),subExprTransformationList,insideExprToBeEliminated);
      } else if(isSgBinaryOp(addressOfOp)) {
        registerSubExpressionTempVars(stmt,isSgExpression(SgNodeHelper::getRhs(addressOfOp)),subExprTransformationList,insideExprToBeEliminated);
        mostRecentTmpVarNr=registerSubExpressionTempVars(stmt,isSgExpression(SgNodeHelper::getLhs(addressOfOp)),subExprTransformationList,insideExprToBeEliminated);
      }
  }
#endif

  static
  bool isNullThrow(SgExpression* expr)
  {
    SgThrowOp* throwexpr = isSgThrowOp(expr);

    return throwexpr && (throwexpr->get_operand() == NULL);
  }

  // stmt is only used to detetermined scope, which is used when generating the tmp-variable.
  Normalization::TmpVarNrType Normalization::registerSubExpressionTempVars(SgStatement* stmt, SgExpression* expr, SubExprTransformationList& subExprTransformationList,bool insideExprToBeEliminated) {
    ROSE_ASSERT(stmt);
    ROSE_ASSERT(expr);
    SAWYER_MESG(logger[TRACE])<<"registerSubExpressionTempVars:insideExpToBeElim:"<<insideExprToBeEliminated<<" @"<<SgNodeHelper::sourceLineColumnToString(expr)<<expr->class_name()<<":"<<AstTerm::astTermWithNullValuesToString(expr)<<endl;
    Normalization::TmpVarNrType mostRecentTmpVarNr=-1;
    /*if(SgCastExp* castExp=isSgCastExp(expr)) {
      registerSubExpressionTempVars(stmt,castExp->get_operand(),subExprTransformationList);
      } else*/
    /*if(SgPntrArrRefExp* arrExp=isSgPntrArrRefExp(expr)) {
      // special case: normalize array index-expressions
      registerSubExpressionTempVars(stmt,isSgExpression(SgNodeHelper::getRhs(arrExp)),subExprTransformationList,insideExprToBeEliminated);
      mostRecentTmpVarNr=registerTmpVarInitialization(stmt,expr,subExprTransformationList);
      } else*/ if(SgAddressOfOp* addressOfOp=isSgAddressOfOp(expr)) {
      // never normalize address operator - skip all address operators
      SAWYER_MESG(logger[TRACE])<<"skipping argument of address operator to be normalized: "<<addressOfOp->unparseToString()<<endl;
      SgExpression* addressOfOperand=isSgExpression(SgNodeHelper::getUnaryOpChild(addressOfOp));
      // same as for lhs of assignment
      ROSE_ASSERT(addressOfOperand);
      if(isSgUnaryOp(addressOfOperand)) {
        mostRecentTmpVarNr=registerSubExpressionTempVars(stmt,isSgExpression(SgNodeHelper::getUnaryOpChild(addressOfOperand)),subExprTransformationList,insideExprToBeEliminated);
      } else if(isSgBinaryOp(addressOfOperand)) {
        registerSubExpressionTempVars(stmt,isSgExpression(SgNodeHelper::getRhs(addressOfOperand)),subExprTransformationList,insideExprToBeEliminated);
        mostRecentTmpVarNr=registerSubExpressionTempVars(stmt,isSgExpression(SgNodeHelper::getLhs(addressOfOperand)),subExprTransformationList,insideExprToBeEliminated);
      }
    } else if(isSgAssignOp(expr)||isSgCompoundAssignOp(expr)) {
      // special case: normalize assignment with lhs/rhs-semantics

      if(isSgExprStatement(expr->get_parent())) {
        // special handling of assignment that is not inside an
        // expression normalize rhs of assignment
        mostRecentTmpVarNr=registerSubExpressionTempVars(stmt,isSgExpression(SgNodeHelper::getRhs(expr)),subExprTransformationList,insideExprToBeEliminated);
        // normalize lhs of assignment skip normalizing top-most
        // operator of lhs because an lvalue-expression must remain an
        // lvalue-expression. Introduction of temporary would be
        // wrong. Note: not all operators can appear as top-most op on
        // lhs.
        SgExpression* lhs=isSgExpression(SgNodeHelper::getLhs(expr));
        ROSE_ASSERT(lhs);
        // v1: do not reserve lhs as temporary variable
        if(isSgUnaryOp(lhs)) {
          mostRecentTmpVarNr=registerSubExpressionTempVars(stmt,isSgExpression(SgNodeHelper::getUnaryOpChild(lhs)),subExprTransformationList,insideExprToBeEliminated);
        } else if(isSgBinaryOp(lhs)) {
          registerSubExpressionTempVars(stmt,isSgExpression(SgNodeHelper::getRhs(lhs)),subExprTransformationList,insideExprToBeEliminated);
          mostRecentTmpVarNr=registerSubExpressionTempVars(stmt,isSgExpression(SgNodeHelper::getLhs(lhs)),subExprTransformationList,insideExprToBeEliminated);
        }
      } else {
        // v2: treat it like any other unary or binary operator (duplicates the two cases)
        if(isSgUnaryOp(expr)) {
          Normalization::TmpVarNrType unaryResultTmpVarNr=registerSubExpressionTempVars(stmt,isSgExpression(SgNodeHelper::getUnaryOpChild(expr)),subExprTransformationList,insideExprToBeEliminated);
          mostRecentTmpVarNr=registerTmpVarInitialization(stmt,expr,unaryResultTmpVarNr,subExprTransformationList);
        } else {
          Normalization::TmpVarNrType rhsResultTmpVarNr=registerSubExpressionTempVars(stmt,isSgExpression(SgNodeHelper::getRhs(expr)),subExprTransformationList,insideExprToBeEliminated);
          Normalization::TmpVarNrType lhsResultTmpVarNr=registerSubExpressionTempVars(stmt,isSgExpression(SgNodeHelper::getLhs(expr)),subExprTransformationList,insideExprToBeEliminated);
          mostRecentTmpVarNr=registerTmpVarInitialization(stmt,expr,lhsResultTmpVarNr,rhsResultTmpVarNr,subExprTransformationList);
        }
      }
    } else if(SgFunctionCallExp* funCallExp=isSgFunctionCallExp(expr)) {
      // special case: function call with normalization of arguments
      // and void return type (no temp var generation)
      SgExpressionPtrList& expList=SgNodeHelper::getFunctionCallActualParameterList(expr);
      for(SgExpressionPtrList::iterator i=expList.begin();i!=expList.end();++i) {
        mostRecentTmpVarNr=registerSubExpressionTempVars(stmt,*i,subExprTransformationList,insideExprToBeEliminated);
      }
      // check if function has a return value
      SgType* functionReturnType=funCallExp->get_type();
      SAWYER_MESG(logger[TRACE])<<"function call type (*): "<<SgNodeHelper::sourceLineColumnToString(funCallExp)<<":"<<functionReturnType->unparseToString()<<endl;

      // generate tmp var only if return value exists and it is used (i.e. there exists an expression as parent).
      SgNode* parentNode=funCallExp->get_parent();
      SAWYER_MESG(logger[TRACE])<<"Normalizing: funCall: stmt:"<<AstTerm::astTermWithNullValuesToString(stmt)<<endl;
      SAWYER_MESG(logger[TRACE])<<"Normalizing: funCall: expr:"<<AstTerm::astTermWithNullValuesToString(expr)<<endl;
      if((!isSgTypeVoid(functionReturnType)
          &&  isSgExpression(parentNode)
          && !isSgExpressionRoot(parentNode))||isSgReturnStmt(parentNode)) {
        mostRecentTmpVarNr=registerTmpVarInitialization(stmt,expr,subExprTransformationList);
      } else {
        // generate function call, but without assignment to temporary
        if(insideExprToBeEliminated) {
          SAWYER_MESG(logger[TRACE])<<"register void-expr for void-function call: "<<funCallExp->unparseToString()<<endl;
          registerVoidExpression(stmt,expr,subExprTransformationList);
        } else {
          SAWYER_MESG(logger[TRACE])<<"DO NOT register void-expr for void-function call (inside cond-op): "<<funCallExp->unparseToString()<<endl;
        }
        // this ensures that the expression is generated even when no temporary is assigned to it
        mostRecentTmpVarNr=0;
      }
    } else if(isSgAndOp(expr)) {
      // special case: short circuit operator normalization
      //SgScopeStatement* scope=stmt->get_scope();
      //SgVariableDeclaration* decl=generateFalseBoolVarDecl(scope);
      Normalization::TmpVarNrType declVarNr=registerTmpFalseBoolVarDecl(stmt,expr,subExprTransformationList); // tmpVarNr of and-op
      Normalization::TmpVarNrType lhsResultTmpVarNr=registerSubExpressionTempVars(stmt,isSgExpression(SgNodeHelper::getLhs(expr)),subExprTransformationList,insideExprToBeEliminated);
      SgBasicBlock* block=SageBuilder::buildBasicBlock();
      registerIfElseStmt(stmt,expr,lhsResultTmpVarNr,block,0,subExprTransformationList);
      Normalization::TmpVarNrType rhsResultTmpVarNr=registerSubExpressionTempVars(block,isSgExpression(SgNodeHelper::getRhs(expr)),subExprTransformationList,insideExprToBeEliminated);
      registerLogOpReplacement(stmt,expr,declVarNr,subExprTransformationList); // will be used for replacing Or operator
      SAWYER_MESG(logger[TRACE])<<"AND-normalization: declVarId: "<<declVarNr<<" rhsResultTmpVarNr:"<<rhsResultTmpVarNr<<endl;
      // TODO: rhsResultTmpVarNr can be 0!
      registerBoolVarIfElseStmt(block,expr,declVarNr,rhsResultTmpVarNr,0,0,subExprTransformationList);
      mostRecentTmpVarNr=declVarNr;
    } else if(isSgOrOp(expr)) {
      // special case: short circuit operator normalization
      Normalization::TmpVarNrType declVarNr=registerTmpFalseBoolVarDecl(stmt,expr,subExprTransformationList); // tmpVarNr of and-op
      Normalization::TmpVarNrType lhsResultTmpVarNr=registerSubExpressionTempVars(stmt,isSgExpression(SgNodeHelper::getLhs(expr)),subExprTransformationList,insideExprToBeEliminated);
      SgBasicBlock* elseBlock=SageBuilder::buildBasicBlock();
      registerBoolVarIfElseStmt(stmt,expr,declVarNr,lhsResultTmpVarNr,0,elseBlock,subExprTransformationList);
      Normalization::TmpVarNrType rhsResultTmpVarNr=registerSubExpressionTempVars(elseBlock,isSgExpression(SgNodeHelper::getRhs(expr)),subExprTransformationList,insideExprToBeEliminated);
      registerLogOpReplacement(stmt,expr,declVarNr,subExprTransformationList); // will be used for replacing Or operator
      registerBoolVarIfElseStmt(elseBlock,expr,declVarNr,rhsResultTmpVarNr,0,0,subExprTransformationList);
      mostRecentTmpVarNr=declVarNr;
    } else if(SgConditionalExp* conditionalExp=isSgConditionalExp(expr)) {
      SAWYER_MESG(logger[TRACE])<<"detected conditional Exp @"<<SgNodeHelper::sourceLineColumnToString(conditionalExp)<<endl;
      // determine type of then-branche for tmp var
      SgType* thenExprType=conditionalExp->get_true_exp()->get_type();
      bool isVoidType=isSgTypeVoid(thenExprType);
      SAWYER_MESG(logger[TRACE])<<"conditional Exp Type:@"<<SgNodeHelper::sourceLineColumnToString(conditionalExp)<<":"<<AstTerm::astTermWithNullValuesToString(thenExprType)<<endl;
      Normalization::TmpVarNrType declVarNr=0;
      SAWYER_MESG(logger[TRACE])<<"isVoidType: "<<isVoidType<<endl;
      if(!isVoidType) {
        declVarNr=registerTmpVarDeclaration(stmt,thenExprType,subExprTransformationList); // tmpVarNr of conditional-op
      }
      SAWYER_MESG(logger[TRACE])<<"declVarNr: "<<declVarNr<<endl;

      //Normalization::TmpVarNrType declVarNr=registerTmpFalseBoolVarDecl(stmt,expr,subExprTransformationList); // tmpVarNr of conditional-op
      // hoist condition
      SgExpression* cond=conditionalExp->get_conditional_exp();
      Normalization::TmpVarNrType condResultTempVarNr=registerSubExpressionTempVars(stmt,cond,subExprTransformationList,insideExprToBeEliminated);
      SAWYER_MESG(logger[TRACE])<<"condResultTempVarNr: "<<condResultTempVarNr<<endl;
      // handle both branches xxx
      SgBasicBlock* thenBlock=SageBuilder::buildBasicBlock();
      SgBasicBlock* elseBlock=SageBuilder::buildBasicBlock();
      registerCondOpIfElseStmt(stmt,cond,condResultTempVarNr,thenBlock,elseBlock,subExprTransformationList);
      // use two blocks to ensure variables don't get mixed up
      {
        Normalization::TmpVarNrType tbResultTempVarNr
          =registerSubExpressionTempVars(thenBlock,isSgExpression(conditionalExp->get_true_exp()),subExprTransformationList,true);
        SAWYER_MESG(logger[TRACE])<<"declVarNr: "<<declVarNr<<", "<<"tbResultTempVarNr: "<<tbResultTempVarNr<<endl;
        if(!isVoidType) {
          if(tbResultTempVarNr==0) {
            registerTmpVarAssignmentWithExpression(thenBlock,conditionalExp->get_true_exp(),declVarNr,subExprTransformationList);
            tbResultTempVarNr=declVarNr;
          } else {
            (void)registerTmpVarAssignment(thenBlock,expr,declVarNr,tbResultTempVarNr,subExprTransformationList);
          }
        }
      }
      {
        Normalization::TmpVarNrType fbResultTempVarNr
          =registerSubExpressionTempVars(elseBlock,isSgExpression(conditionalExp->get_false_exp()),subExprTransformationList,true);
        // assignment: declVarNr=fbResultTempVarNr;
        SAWYER_MESG(logger[TRACE])<<"declVarNr: "<<declVarNr<<", "<<"fbResultTempVarNr: "<<fbResultTempVarNr<<endl;
        if(!isVoidType) {
          if(fbResultTempVarNr==0) {
            (void)registerTmpVarAssignmentWithExpression(elseBlock,conditionalExp->get_false_exp(),declVarNr,subExprTransformationList);
            fbResultTempVarNr=declVarNr;
          } else {
            (void)registerTmpVarAssignment(elseBlock,expr,declVarNr,fbResultTempVarNr,subExprTransformationList);
          }
          registerLogOpReplacement(stmt,expr,declVarNr,subExprTransformationList); // replacing conditional operator
          mostRecentTmpVarNr=fbResultTempVarNr; // note both branches must be either void or non-void.
        } else {
          // special case: if void-expr then ?-op must be removed. In
          // this case the operator must be inside an ExprStmt (casts
          // may exist as well, therefore the pattern can be more
          // complicated, but the root is stmt.
          registerStmtRemoval(stmt,subExprTransformationList);
        }
      }
      //ROSE_ASSERT(isValidRegisteredTmpVarNr(tbResultTempVarNr)==isValidRegisteredTmpVarNr(fbResultTempVarNr));

    } else if(isSgCastExp(expr) && isSgTypeVoid(expr->get_type())) {
        // note: other non-void cast expressions are handled by unary operator case
      SAWYER_MESG(logger[TRACE])<<"register void-expr for void-cast: "<<expr->unparseToString()<<endl;
      registerVoidExpression(stmt,expr,subExprTransformationList);
      mostRecentTmpVarNr=0;
    } else if(isSgBinaryOp(expr)) {
      // general case: binary operator
      Normalization::TmpVarNrType rhsResultTmpVarNr=registerSubExpressionTempVars(stmt,isSgExpression(SgNodeHelper::getRhs(expr)),subExprTransformationList,insideExprToBeEliminated);
      Normalization::TmpVarNrType lhsResultTmpVarNr=registerSubExpressionTempVars(stmt,isSgExpression(SgNodeHelper::getLhs(expr)),subExprTransformationList,insideExprToBeEliminated);
      mostRecentTmpVarNr=registerTmpVarInitialization(stmt,expr,lhsResultTmpVarNr,rhsResultTmpVarNr,subExprTransformationList);
    } else if (isNullThrow(expr)) {
      mostRecentTmpVarNr=0; // PP correct?
    } else if(isSgUnaryOp(expr)) {
      // general case: unary operator
      Normalization::TmpVarNrType unaryResultTmpVarNr=registerSubExpressionTempVars(stmt,isSgExpression(SgNodeHelper::getUnaryOpChild(expr)),subExprTransformationList,insideExprToBeEliminated);
      mostRecentTmpVarNr=registerTmpVarInitialization(stmt,expr,unaryResultTmpVarNr,subExprTransformationList);
    } else {
      // leave node.
      // MS: 05/09/2019: register tmp var if parent is AndOp or OrOp
      // to ensure logop (and/or)s can operate on registered tmp vars
      // (NORM41)
      SgNode* p=expr->get_parent();
      if(isSgAndOp(p)||isSgOrOp(p)||isSgConditionalExp(p)) {
        mostRecentTmpVarNr=registerTmpVarInitialization(stmt,expr,subExprTransformationList);
      } else {
        mostRecentTmpVarNr=0;
      }
    }

    SAWYER_MESG(logger[TRACE])<<SAWYER_MESG(logger[TRACE])<<"registerSubExpressionTempVars@"<<":"<<SgNodeHelper::sourceLineColumnToString(expr)<<": mostRecentTmpVarNr on return: "<<mostRecentTmpVarNr<<endl;
    return mostRecentTmpVarNr;
  }

  SgVarRefExp* Normalization::getVarRefExp(TmpVarNrType tmpVarNr) {
    SgVariableDeclaration* varDecl=getVarDecl(tmpVarNr);
    return buildVarRefExpForVariableDeclaration(varDecl);
  }

  SgVariableDeclaration* Normalization::getVarDecl(TmpVarNrType tmpVarNr) {
    ROSE_ASSERT(isValidGeneratedTmpVarDeclNr(tmpVarNr));
    return tmpVarMapping[tmpVarNr];
  }

  SgVariableDeclaration* Normalization::generateVarDecl(SgType* varType, SgScopeStatement* scope) {
    string varName=newTmpVarName();
    SgAssignInitializer* varInit=nullptr;
    return SageBuilder::buildVariableDeclaration(varName, varType, varInit, scope);
  }

  SgVariableDeclaration* Normalization::generateFalseBoolVarDecl(SgScopeStatement* scope) {
    string varName=newTmpVarName();
    SgType* varType=SageBuilder::buildBoolType();
    SgBoolValExp* falseVal=SageBuilder::buildBoolValExp(false);
    SgAssignInitializer* varInit=SageBuilder::buildAssignInitializer(falseVal, varType);
    return SageBuilder::buildVariableDeclaration(varName, varType, varInit, scope);
  }

  SgIfStmt* Normalization::generateIfElseStmt(SgExpression* cond, SgStatement* true_body, SgStatement* false_body) {
    SgIfStmt* ifStmt=SageBuilder::buildIfStmt(cond,true_body,false_body);
    return ifStmt;
  }

  SgIfStmt* Normalization::generateBoolVarIfElseStmt(SgExpression* cond, SgVarRefExp* varRefExp,SgStatement* true_body, SgStatement* false_body, SgScopeStatement* scope) {
    ROSE_ASSERT(varRefExp);
    SgExpression* lhs=varRefExp;
    SgExpression* rhs=SageBuilder::buildBoolValExp(true);
    SgExprStatement* exprStmt=SageBuilder::buildAssignStatement(lhs, rhs);
    if(SgBasicBlock* trueBodyBlock=isSgBasicBlock(true_body)) {
      trueBodyBlock->append_statement(exprStmt);
    } else {
      true_body=exprStmt;
    }
    // generated code has to be normalized code (requires non empty-else branch)
    //SgStatement* false_body=SageBuilder::buildBasicBlock();
    SgIfStmt* ifStmt=SageBuilder::buildIfStmt(cond,true_body,false_body);
    return ifStmt;
  }

  Normalization::TmpVarNrType Normalization::getTmpVarNr() {
    return tmpVarNrCounter;
  }

  void Normalization::incTmpVarNr() {
    (void)(tmpVarNrCounter++);
  }

  bool Normalization::tmpVarNrDeclExists(Normalization::TmpVarNrType tmpVarNr) {
    return tmpVarMapping.find(tmpVarNr)!=tmpVarMapping.end();
  }

  bool Normalization::isValidGeneratedTmpVarDeclNr(Normalization::TmpVarNrType tmpVarNr) {
    if(tmpVarNr<1) {
      SAWYER_MESG(logger[TRACE])<<"isValidGeneratedTmpVarDeclNr: tmpVarnr:"<<tmpVarNr<<endl;
    }
    if(!tmpVarNrDeclExists(tmpVarNr)) {
      SAWYER_MESG(logger[TRACE])<<"isValidGeneratedTmpVarDeclNr: tmpVarnr:"<<tmpVarNr<<" does not exist!"<<endl;
    }
    return tmpVarNr>0 && tmpVarNrDeclExists(tmpVarNr);
  }

  bool Normalization::isValidRegisteredTmpVarNr(Normalization::TmpVarNrType tmpVarNr) {
    return tmpVarNr>0;
  }

  void Normalization::registerStmtRemoval(SgStatement* stmt, SubExprTransformationList& subExprTransformationList) {
    RegisteredSubExprTransformation seTrans(Normalization::GEN_STMT_REMOVAL,stmt,nullptr);
    subExprTransformationList.push_back(seTrans);
  }

  void Normalization::registerTmpVarAssignmentWithExpression(SgStatement* stmt,SgExpression* expr, Normalization::TmpVarNrType tmpVarNr, SubExprTransformationList& subExprTransformationList) {
    SAWYER_MESG(logger[TRACE])<<"registerTmpVarAssignmentWithExpression: temp"<<tmpVarNr<<" = "<<expr->unparseToString()<<endl;
    ROSE_ASSERT(isValidRegisteredTmpVarNr(tmpVarNr));
    RegisteredSubExprTransformation seTrans(Normalization::GEN_TMP_VAR_ASSIGN_WITH_EXPR,stmt,expr);
    seTrans.tmpVarNr=tmpVarNr;
    subExprTransformationList.push_back(seTrans);
  }

  void Normalization::registerTmpVarAssignment(SgStatement* stmt,SgExpression* expr, Normalization::TmpVarNrType lhsVarNr, Normalization::TmpVarNrType rhsVarNr, SubExprTransformationList& subExprTransformationList) {
    SAWYER_MESG(logger[TRACE])<<"registerTmpVarAssignment: "<<lhsVarNr<<"("<<isValidRegisteredTmpVarNr(lhsVarNr)<<")"<<" = "<<rhsVarNr<<"("<<isValidRegisteredTmpVarNr(rhsVarNr)<<")"<<endl;
    ROSE_ASSERT(isValidRegisteredTmpVarNr(lhsVarNr));
    ROSE_ASSERT(isValidRegisteredTmpVarNr(rhsVarNr));
    RegisteredSubExprTransformation seTrans(Normalization::GEN_TMP_VAR_ASSIGN,stmt,expr);
    seTrans.lhsTmpVarNr=lhsVarNr;
    seTrans.rhsTmpVarNr=rhsVarNr;
    subExprTransformationList.push_back(seTrans);
  }

  Normalization::TmpVarNrType Normalization::registerTmpVarDeclaration(SgStatement* stmt, SgType* type, SubExprTransformationList& subExprTransformationList) {
    incTmpVarNr();
    TmpVarNrType nr=getTmpVarNr();
    SAWYER_MESG(logger[TRACE])<<"registerTmpVarDeclaration nr: "<<nr<<endl;
    RegisteredSubExprTransformation seTrans(Normalization::GEN_TMP_VAR_DECL,nr,stmt,nullptr);
    seTrans.tmpVarDeclType=type;
    subExprTransformationList.push_back(seTrans);
    return nr;
  }

  Normalization::TmpVarNrType Normalization::registerTmpVarInitialization(SgStatement* stmt, SgExpression  * expr, SubExprTransformationList& subExprTransformationList) {
    incTmpVarNr();
    TmpVarNrType nr=getTmpVarNr();
    RegisteredSubExprTransformation seTrans(Normalization::GEN_TMP_VAR_INIT,nr,stmt,expr);
    subExprTransformationList.push_back(seTrans);
    return nr;
  }

  Normalization::TmpVarNrType Normalization::registerTmpVarInitialization(SgStatement* stmt, SgExpression  * expr, Normalization::TmpVarNrType unaryTmpVarNr, SubExprTransformationList& subExprTransformationList) {
    incTmpVarNr();
    TmpVarNrType nr=getTmpVarNr();
    RegisteredSubExprTransformation seTrans(Normalization::GEN_TMP_VAR_INIT,nr,stmt,expr);
    seTrans.unaryTmpVarNr=unaryTmpVarNr;
    subExprTransformationList.push_back(seTrans);
    return nr;
  }

  Normalization::TmpVarNrType Normalization::registerTmpVarInitialization(SgStatement* stmt, SgExpression  * expr, Normalization::TmpVarNrType lhs, Normalization::TmpVarNrType rhs, SubExprTransformationList& subExprTransformationList) {
    incTmpVarNr();
    TmpVarNrType nr=getTmpVarNr();
    RegisteredSubExprTransformation seTrans(Normalization::GEN_TMP_VAR_INIT,nr,stmt,expr);
    seTrans.lhsTmpVarNr=lhs;
    seTrans.rhsTmpVarNr=rhs;
    subExprTransformationList.push_back(seTrans);
    return nr;
  }

  Normalization::TmpVarNrType Normalization::registerTmpVarInitialization(SgStatement* stmt, SgExpression  * expr, Normalization::TmpVarNrType condTmpVarNr, Normalization::TmpVarNrType tbTmpVarNr, Normalization::TmpVarNrType fbTmpVarNr, SubExprTransformationList& subExprTransformationList) {
    incTmpVarNr();
    TmpVarNrType nr=getTmpVarNr();
    RegisteredSubExprTransformation seTrans(Normalization::GEN_TMP_VAR_INIT,nr,stmt,expr);
    seTrans.unaryTmpVarNr=condTmpVarNr;
    seTrans.lhsTmpVarNr=tbTmpVarNr;
    seTrans.rhsTmpVarNr=fbTmpVarNr;
    subExprTransformationList.push_back(seTrans);
    return nr;
  }

  Normalization::TmpVarNrType Normalization::registerTmpFalseBoolVarDecl(SgStatement* stmt, SgExpression  * expr, SubExprTransformationList& subExprTransformationList) {
    incTmpVarNr();
    TmpVarNrType nr=getTmpVarNr();
    subExprTransformationList.push_back(RegisteredSubExprTransformation(Normalization::GEN_FALSE_BOOL_VAR_DECL,nr,stmt,expr,nr));
    return nr;
  }

  void Normalization::insertNormalizedSubExpressionFragment(SgStatement* fragment, SgStatement* stmt) {
    if(SgBasicBlock* block=isSgBasicBlock(stmt)) {
      block->append_statement(fragment);
    } else {
      SageInterface::insertStatementBefore(stmt,fragment);
    }
  }

  void Normalization::registerVoidExpression(SgStatement* stmt, SgExpression  * expr, SubExprTransformationList& subExprTransformationList) {
    subExprTransformationList.push_back(RegisteredSubExprTransformation(Normalization::GEN_VOID_EXPR,stmt,expr));
  }

  void Normalization::registerLogOpReplacement(SgStatement* stmt, SgExpression  * expr, Normalization::TmpVarNrType declVarNr, SubExprTransformationList& subExprTransformationList) {
    subExprTransformationList.push_back(RegisteredSubExprTransformation(Normalization::GEN_LOG_OP_REPLACEMENT,stmt,expr,declVarNr));
  }

  void Normalization::registerBoolVarIfElseStmt(SgStatement* stmt, SgExpression  * expr, Normalization::TmpVarNrType declVarNr, Normalization::TmpVarNrType condVarNr, SgStatement* trueBody, SgStatement* falseBody, SubExprTransformationList& subExprTransformationList) {
    auto transOp=RegisteredSubExprTransformation(Normalization::GEN_BOOL_VAR_IF_ELSE_STMT,stmt,expr,declVarNr,trueBody,falseBody);
    SAWYER_MESG(logger[TRACE])<<"registerBoolVarIfElseStmt: condVarNr: "<<condVarNr<<endl;
    transOp.condVarNr=condVarNr;
    subExprTransformationList.push_back(transOp);
  }

  void Normalization::registerIfElseStmt(SgStatement* stmt, SgExpression  * expr, Normalization::TmpVarNrType declVarNr, SgStatement* trueBody, SgStatement* falseBody, SubExprTransformationList& subExprTransformationList) {
    subExprTransformationList.push_back(RegisteredSubExprTransformation(Normalization::GEN_IF_ELSE_STMT,stmt,expr,declVarNr,trueBody,falseBody));
  }

  void Normalization::registerCondOpIfElseStmt(SgStatement* stmt, SgExpression  *cond, Normalization::TmpVarNrType condVarNr, SgStatement* trueBody, SgStatement* falseBody, SubExprTransformationList& subExprTransformationList) {
    auto transOp=RegisteredSubExprTransformation(Normalization::GEN_CONDOP_IF_ELSE_STMT,stmt,cond,condVarNr,trueBody,falseBody);
    //ROSE_ASSERT(isValidGeneratedTmpVarDeclNr(condVarNr));
    ROSE_ASSERT(isValidRegisteredTmpVarNr(condVarNr));
    transOp.condVarNr=condVarNr;
    transOp.declVarNr=0;
    subExprTransformationList.push_back(transOp);
  }

  void Normalization::registerBoolVarIfStmt(SgStatement* stmt, SgExpression  * expr, Normalization::TmpVarNrType declVarNr, SubExprTransformationList& subExprTransformationList) {
    subExprTransformationList.push_back(RegisteredSubExprTransformation(Normalization::GEN_BOOL_VAR_IF_STMT,stmt,expr,declVarNr));
  }

  /***************************************************************************
   * LOWERING (Level 3 Normalization - not completed and not enabled)
   **************************************************************************/

  // eliminates while/do-while
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
          if(options.transformBreakToGotoInLoopStmts && CodeThorn::CFAnalysis::isLoopConstructRootNode(stmt)) {
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

  void Normalization::setUniqueVariablePrefix(std::string prefix) {
    _uniqueVarPrefix=prefix;
  }

  void Normalization::setUniqueVariablePostfix(std::string postfix) {
    _uniqueVarPostfix=postfix;
  }

  /** Generate a name that is unique in the current scope and any parent and children scopes.
   * @param baseName the word to be included in the variable names. */
  // TODO: backport this function to SageInterface with the additional features
  string Normalization::generateUniqueVariableName(SgScopeStatement* scope, std::string baseName) {
    string name;
    bool collision = false;
    do
    {
      name = _uniqueVarPrefix + baseName + boost::lexical_cast<string > (uniqueVarCounter++) + _uniqueVarPostfix;

      SgSymbol* nameSymbol = SageInterface::lookupSymbolInParentScopes(SgName(name), scope,NULL,NULL);
      collision = (nameSymbol != NULL);

      //Look up the name in the children scopes
      Rose_STL_Container<SgNode*> childScopes = NodeQuery::querySubTree(scope, V_SgScopeStatement);

      BOOST_FOREACH(SgNode* childScope, childScopes) {
        SgScopeStatement* childScopeStatement = isSgScopeStatement(childScope);
          nameSymbol = childScopeStatement->lookup_symbol(SgName(name),NULL,NULL);
          collision = collision || (nameSymbol != NULL);
      }
    } while (collision);

    return name;
  }

  SgExprStatement* Normalization::generateTmpVarAssignment(Normalization::TmpVarNrType lhsVarNr, Normalization::TmpVarNrType rhsVarNr) {
    auto lhsVarRefExp=getVarRefExp(lhsVarNr);
    auto rhsVarRefExp=getVarRefExp(rhsVarNr);
    return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(lhsVarRefExp,rhsVarRefExp));
  }

  SgExprStatement* Normalization::generateTmpVarAssignmentWithExpr(Normalization::TmpVarNrType lhsVarNr, SgExpression* expr) {
    auto lhsVarRefExp=getVarRefExp(lhsVarNr);
    return SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(lhsVarRefExp,SageInterface::deepCopy(expr)));
  }

  SgVariableDeclaration*
  Normalization::buildVariableDeclarationForExpression(SgExpression* expression, SgScopeStatement* scope, bool shareExpression) {
    return buildVariableDeclarationForExpression(expression,scope,false,shareExpression);
  }
  SgVariableDeclaration*
  Normalization::buildVariableDeclarationWithInitializerForExpression(SgExpression* expression, SgScopeStatement* scope, bool shareExpression) {
    return buildVariableDeclarationForExpression(expression,scope,true,shareExpression);
  }
  SgVariableDeclaration*
  Normalization::buildVariableDeclarationForExpression(SgExpression* expression, SgScopeStatement* scope, bool initWithExpression, bool shareExpression) {
    SgType* expressionType = expression->get_type();
    SgType* variableType = expressionType;

    //MS 10/24/2018: If variable has referece type, use a value type for the temporary variable (otherwise reference would be duplicated into 2 memory locations)
    if (SgReferenceType* referenceType=isSgReferenceType(expressionType))
    {
      if(SgReferenceType* strippedReferenceType = isSgReferenceType(referenceType->stripType(SgType::STRIP_TYPEDEF_TYPE))) {
        SgType* strippedReferenceBaseType = strippedReferenceType->get_base_type();
        variableType = strippedReferenceBaseType;
      }
    }

    //MS 10/24/2018: If the expression has array type, use a pointer type for the temporary variable.
    if (SgArrayType* arrayType=isSgArrayType(expressionType))
    {
      if(SgArrayType* strippedArrayType = isSgArrayType(arrayType->stripType(SgType::STRIP_TYPEDEF_TYPE))) {
        SgType* strippedArrayBaseType = strippedArrayType->get_base_type();
        variableType = SageBuilder::buildPointerType(strippedArrayBaseType);
      }
    }

    //Generate a unique variable name
    string name = generateUniqueVariableName(scope,_tmpVarBaseName);

    //initialize the variable in its declaration
    SgAssignInitializer* initializer=nullptr;
    if(initWithExpression) {
      SgExpression* initExpression = shareExpression?expression:SageInterface::copyExpression(expression);
      initializer = SageBuilder::buildAssignInitializer(initExpression);
    }

    /* special case: check if expression is a struct/class/union copied by value. If yes introduce a reference type for the tmp var (to avoid
     copy semantics which would make assignments to the members of the struct not having any effect on the original data */
    if(isSgClassType(variableType)) {
      variableType = SageBuilder::buildReferenceType(variableType);
    }

    SgVariableDeclaration* newVarDeclaration = SageBuilder::buildVariableDeclaration(name, variableType, initializer, scope);
    ROSE_ASSERT(newVarDeclaration);

    return newVarDeclaration;
  }

  SgVarRefExp* Normalization::buildVarRefExpForVariableDeclaration(SgVariableDeclaration* decl) {
    return SageBuilder::buildVarRefExp(decl);
  }

  void Normalization::addToTmpVarMapping(TmpVarNrType tmpVarNr, SgVariableDeclaration* decl) {
    SAWYER_MESG(logger[TRACE])<<"addToTmpVarMapping: "<<"("<<tmpVarNr<<","<< decl<<")"<<endl;
    tmpVarMapping.insert(TmpVarMappingPair(tmpVarNr,decl));
  }
} // end of namespace CodeThorn

