#ifndef PATRANSFERFUNCTIONS_CPP
#define PATRANSFERFUNCTIONS_CPP

#include "sage3basic.h"

#include "sageGeneric.h"

using namespace std;

#include "CollectionOperators.h"
#include "DFTransferFunctions.h"
#include "AnalysisAbstractionLayer.h"

using namespace CodeThorn;

namespace
{
  std::ostream& logWarn() { return std::cerr << "[WARN] "; } 
}

DFTransferFunctions::DFTransferFunctions():_programAbstractionLayer(0){}

void DFTransferFunctions::transfer(Edge edge, Lattice& element) {
    Label lab0=edge.source();
    //SgNode* node=getLabeler()->getNode(lab0);
    //cout<<"DEBUG: transfer @"<<lab0<<":"<<node->unparseToString()<<endl;

    // switch statement has its own transfer functions which are selected in transfer function
    if(getLabeler()->isConditionLabel(lab0)&&!getLabeler()->isSwitchExprLabel(lab0)) {
      transferCondition(edge,element);
    } else {
      transfer(lab0,element);
    }
}

void DFTransferFunctions::transferCondition(Edge edge, Lattice& element) {
  Label lab0=edge.source();
  transfer(lab0,element);
}

static inline
VariableId withoutVariable() { return VariableId(); }

static inline 
VariableId getVariableId(CodeThorn::VariableIdMapping* vmap, SgNode* n)
{
  ROSE_ASSERT(vmap && n);
  
  if (SgVarRefExp* ref = isSgVarRefExp(n))
    return vmap->variableId(ref);
    
  // \todo handle x->field expressions    
  if(SgArrowExp* arrow = isSgArrowExp(n)) 
    return getVariableId(vmap, arrow->get_lhs_operand());
  
  // \todo handle this expression
  if(isSgThisExp(n)) 
    return withoutVariable();
    
  cerr<<"Transfer: unknown lhs of function call result assignment."<<endl;
  cerr<<n->unparseToString()<<endl;
  exit(1);
  
  //~ return withoutVariable();
}

void DFTransferFunctions::transfer(Label lab, Lattice& element) {
  ROSE_ASSERT(getLabeler());
  //cout<<"transfer @label:"<<lab<<endl;
  SgNode* node=getLabeler()->getNode(lab);
  //cout<<"Analyzing:"<<node->class_name()<<endl;
  //cout<<"DEBUG: transfer: @"<<lab<<": "<<node->class_name()<<":"<<node->unparseToString()<<endl;

  if(element.isBot()) {
    // transfer function applied to the bottom element is the bottom element (non-reachable state)
    // the extremal value must be different to the bottom element.
    return;
  }
  
  if(getLabeler()->isFunctionCallLabel(lab)) {
    // 1) f(x), 2) y=f(x) (but not y+=f(x))
    SgNodeHelper::ExtendedCallInfo callinfo = SgNodeHelper::matchExtendedNormalizedCall(node);
    
    //~ if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(node)) {
    if(SgFunctionCallExp* funCall = callinfo.callExpression()) {
      SgExpressionPtrList& arguments=SgNodeHelper::getFunctionCallActualParameterList(funCall);
      std::string calltext = funCall->unparseToString(); 
      
      if (calltext == "\"INFO: \"+PREFIX")
      {
        cerr<<"InfoX: callexp: fcall found " << funCall->unparseToString() << std::endl;
      }
      
      transferFunctionCall(lab, funCall, arguments, element);
    } else if (SgConstructorInitializer* ctorCall = callinfo.ctorInitializer()) {
      SgExpressionPtrList& arguments=SG_DEREF(ctorCall->get_args()).get_expressions();
      cerr<<"InfoY: callexp: ccall found " << ctorCall->unparseToString() << std::endl;
      transferConstructorCall(lab, ctorCall, arguments, element);
    } else {
      cerr<<"Error: DFTransferFunctions::callexp: no function call on rhs of assignment found. Only found "
          <<node->class_name() << " for " << node->unparseToString() 
          <<endl;
      exit(1);
    }
    
    return;
  }

  if(getLabeler()->isFunctionCallReturnLabel(lab)) {
    if(isSgExprStatement(node)) {
      node=SgNodeHelper::getExprStmtChild(node);
    }
    // case x=f(y);
    if(isSgAssignOp(node)||isSgCompoundAssignOp(node)) {
      VariableId varId = getVariableId(getVariableIdMapping(), SgNodeHelper::getLhs(node));
      SgNode* rhs=SgNodeHelper::getRhs(node);
      while(isSgCastExp(rhs))
        rhs=SgNodeHelper::getFirstChild(rhs);
      if(SgFunctionCallExp* funCall=isSgFunctionCallExp(rhs)) {
        transferFunctionCallReturn(lab, varId, funCall, element);
        return;
      }
      if(SgConstructorInitializer* ctorCall=isSgConstructorInitializer(rhs)) {
        transferConstructorCallReturn(lab, varId, ctorCall, element);
        return;
      }
      if(SgNewExp* newExp=isSgNewExp(rhs))
      {
        transferConstructorCallReturn(lab, varId, newExp->get_constructor_args(), element);
        return;
      }
      cerr<<"Transfer: no function call on rhs of assignment."<<endl;
      cerr<<node->unparseToString()<<endl;
      exit(1);
    } else if(SgFunctionCallExp* funCall=isSgFunctionCallExp(node)) {
      transferFunctionCallReturn(lab, withoutVariable(), funCall, element);
      return;
    } else if(SgConstructorInitializer* ctorCall=isSgConstructorInitializer(node)) {
      transferConstructorCallReturn(lab, withoutVariable(), ctorCall, element);
      return;
    } else if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCallExpInVariableDeclaration(node)) {
      // handle special case of function call in variable declaration: type var=f();
      SgVariableDeclaration* varDecl=SgNodeHelper::Pattern::matchVariableDeclarationWithFunctionCall(node);
      VariableId lhsVarId=getVariableIdMapping()->variableId(varDecl);
      ROSE_ASSERT(lhsVarId.isValid());
      transferFunctionCallReturn(lab, lhsVarId, funCall, element);
    } else if(isSgReturnStmt(node)) {
      // special case of return f(...);
      node=SgNodeHelper::getFirstChild(node);
      if(SgFunctionCallExp* funCall=isSgFunctionCallExp(node)) {
        transferFunctionCallReturn(lab, withoutVariable(), funCall, element);
        return;
      } else {
        SgNodeHelper::ExtendedCallInfo callinfo = SgNodeHelper::matchExtendedNormalizedCall(node);
        ROSE_ASSERT(callinfo && callinfo.ctorInitializer());
        transferConstructorCallReturn(lab, withoutVariable(), callinfo.ctorInitializer(), element);
      }
    } else if(SgNodeHelper::ExtendedCallInfo callinfo = SgNodeHelper::matchExtendedNormalizedCall(node)) {
      SgVariableDeclaration* varDecl=isSgVariableDeclaration(node);
      ROSE_ASSERT(varDecl);
      VariableId lhsVarId=getVariableIdMapping()->variableId(varDecl);
      ROSE_ASSERT(lhsVarId.isValid());
      transferConstructorCallReturn(lab, lhsVarId, callinfo.ctorInitializer(), element);
    } 
    else {
      if(getSkipUnknownFunctionCalls()) {
        // ignore unknown function call (requires command line option --ignore-unknown-functions)
      } else {
        cerr<<"Error: function-call-return unhandled function call."<<endl;
        cerr<<node->unparseToString()<<endl;
        exit(1);
      }
    }
  }

  if(getLabeler()->isFunctionEntryLabel(lab)) {
    if(SgFunctionDefinition* funDef=isSgFunctionDefinition(getLabeler()->getNode(lab))) {
      // 1) obtain formal parameters
      std::string xyz = funDef->get_mangled_name();
      
      if (xyz == "L2167R")
      {
        std::cout << "! " << funDef->unparseToString() << std::endl; 
      } 
      else
      {
        std::cout << ": " << xyz << std::endl; 
      }
      
      SgInitializedNamePtrList& formalParameters=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
      transferFunctionEntry(lab, funDef, formalParameters, element);
      return;
    } else {
      ROSE_ASSERT(0);
    }
  }

  if(getLabeler()->isFunctionExitLabel(lab)) {
    if(SgFunctionDefinition* funDef=isSgFunctionDefinition(getLabeler()->getNode(lab))) {
      // 1) determine all local variables (including formal parameters) of function
      // 2) delete all local variables from state
      // 2a) remove variable from state

      // ad 1)
      set<SgVariableDeclaration*> varDecls=SgNodeHelper::localVariableDeclarationsOfFunction(funDef);
      // ad 2)
      VariableIdMapping::VariableIdSet localVars=getVariableIdMapping()->determineVariableIdsOfVariableDeclarations(varDecls);
      SgInitializedNamePtrList& formalParamInitNames=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
      VariableIdMapping::VariableIdSet formalParams=getVariableIdMapping()->determineVariableIdsOfSgInitializedNames(formalParamInitNames);
      VariableIdMapping::VariableIdSet vars=localVars+formalParams;
      transferFunctionExit(lab,funDef,vars,element); // TEST ONLY
      return;
    } else {
      ROSE_ASSERT(0);
    }
  }
  if(getLabeler()->isEmptyStmtLabel(lab)||getLabeler()->isBlockBeginLabel(lab)) {
    SgStatement* stmt=isSgStatement(node);
    ROSE_ASSERT(stmt);
    transferEmptyStmt(lab,stmt,element);
    return;
  }

  if(isSgExprStatement(node)) {
    node=SgNodeHelper::getExprStmtChild(node);
  }

  // detect SgReturn statement expressions
  if(isSgReturnStmt(node)) {
    node=SgNodeHelper::getFirstChild(node);
    SgExpression* expr=isSgExpression(node);
    ROSE_ASSERT(expr);
    transferReturnStmtExpr(lab,expr,element);
    return;
  }

  // default identity functions
  if(isSgBreakStmt(node)
     || isSgContinueStmt(node)
     || isSgLabelStatement(node)
     || isSgGotoStatement(node)) {
    return;
  }

  if(SgCaseOptionStmt* caseStmt=isSgCaseOptionStmt(node)) {
    SgStatement* blockStmt=isSgBasicBlock(caseStmt->get_parent());
    ROSE_ASSERT(blockStmt);
    SgSwitchStatement* switchStmt=isSgSwitchStatement(blockStmt->get_parent());
    ROSE_ASSERT(switchStmt);
    SgStatement* condStmt=isSgStatement(SgNodeHelper::getCond(switchStmt));
    ROSE_ASSERT(condStmt);
    transferSwitchCase(lab,condStmt, caseStmt,element);
    return;
  }
  if(SgDefaultOptionStmt* defaultStmt=isSgDefaultOptionStmt(node)) {
    SgStatement* blockStmt=isSgBasicBlock(defaultStmt->get_parent());
    ROSE_ASSERT(blockStmt);
    SgSwitchStatement* switchStmt=isSgSwitchStatement(blockStmt->get_parent());
    ROSE_ASSERT(switchStmt);
    SgStatement* condStmt=isSgStatement(SgNodeHelper::getCond(switchStmt));
    ROSE_ASSERT(condStmt);
    transferSwitchDefault(lab,condStmt, defaultStmt,element);
    return;
  }
  if(SgVariableDeclaration* vardecl=isSgVariableDeclaration(node)) {
    transferDeclaration(lab,vardecl,element);
    return;
  }
  if(SgExpression* expr=isSgExpression(node)) {
    transferExpression(lab,expr,element);
    return;
  }

  if(isSgSwitchStatement(node)) {
    cout<<"WARNING: transfer at SgSwitchStatement node."<<endl;
    return;
  }

#if 0
  /*
     ensure there is no fall through as this would mean that not the correct transferfunction is invoked.
  */
  cerr<<"Error: unknown language construct in transfer function."<<endl;
  cerr<<"  Label: "<<lab<<endl;
  cerr<<"  NodeType: "<<node->class_name()<<endl;
  cerr<<"  Source Code: "<<node->unparseToString()<<endl;
  exit(1);
#else
  /* fall trough for all other stmts */
  return;
#endif
}

void DFTransferFunctions::transferExpression(Label lab, SgExpression* node, Lattice& element) {
  // default identity function
}

void DFTransferFunctions::transferReturnStmtExpr(Label lab, SgExpression* expr, Lattice& element) {
  transferExpression(lab,expr,element);
}



void DFTransferFunctions::transferDeclaration(Label label, SgVariableDeclaration* decl, Lattice& element) {
  // default identity function
}

void DFTransferFunctions::transferEmptyStmt(Label label, SgStatement* stmt, Lattice& element) {
  // default identity function
}

void DFTransferFunctions::transferSwitchCase(Label lab,SgStatement* condStmt, SgCaseOptionStmt* caseStmt,Lattice& pstate) {
  // default identity function
}

void DFTransferFunctions::transferSwitchDefault(Label lab,SgStatement* condStmt, SgDefaultOptionStmt* caseStmt,Lattice& pstate) {
  // default identity function
}

void DFTransferFunctions::transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, Lattice& element) {
  // default identity function
}

void DFTransferFunctions::transferConstructorCall(Label, SgConstructorInitializer*, SgExpressionPtrList&, Lattice&) {
  // default identity function
}

void DFTransferFunctions::transferExternalFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, Lattice& element) {
  // default identity function
}

/*
void DFTransferFunctions::transferFunctionCallReturn(Label lab, SgVarRefExp* lhsVar, SgFunctionCallExp* callExp, Lattice& element) {
  // default function implementation
  VariableId varId;
  if(lhsVar) {
    varId=getVariableIdMapping()->variableId(lhsVar);
  }
  // for void functions, varId remains invalid
  transferFunctionCallReturn(lab,varId,callExp,element);
}
*/

void DFTransferFunctions::transferFunctionCallReturn(Label lab, VariableId varId, SgFunctionCallExp* callExp, Lattice& element) {
  // default identity function
}

void DFTransferFunctions::transferConstructorCallReturn(Label, VariableId, SgConstructorInitializer*, Lattice&) {
  // default identity function
}

void DFTransferFunctions::transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element) {
  // default identity function
}


void DFTransferFunctions::transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, Lattice& element) {
  // default identity function
}

void DFTransferFunctions::addParameterPassingVariables() {
  std::string nameprefix="$p";
  /* this variable is necessary to know the id-range where parameter
     passing variable-ids are starting in the id-range.
  */
  parameter0VariableId=getVariableIdMapping()->createUniqueTemporaryVariableId(nameprefix+"0");
  for(int i=1;i<20;i++) {
    std::stringstream ss;
    ss<<nameprefix<<i;
    string varName=ss.str();
    getVariableIdMapping()->createUniqueTemporaryVariableId(varName);
  }
  resultVariableId=getVariableIdMapping()->createUniqueTemporaryVariableId("$r");
}

VariableId DFTransferFunctions::getParameterVariableId(int paramNr) {
  if(paramNr>=20) {
    cerr<<"Error: only 20 formal parameters are supported."<<endl;
    exit(1);
  }
  int idCode = parameter0VariableId.getIdCode();
  int elemIdCode = idCode + paramNr;
  VariableId elemVarId;
  elemVarId.setIdCode(elemIdCode);
  return elemVarId;
}

VariableId DFTransferFunctions::getResultVariableId() {
  return resultVariableId;
}

void DFTransferFunctions::setSkipUnknownFunctionCalls(bool flag) {
  _skipSelectedFunctionCalls=flag;
}

bool DFTransferFunctions::getSkipUnknownFunctionCalls() {
  return _skipSelectedFunctionCalls;
}

bool DFTransferFunctions::isExternalFunctionCall(Label lab) {
  return _programAbstractionLayer->getLabeler()->isExternalFunctionCallLabel(lab);
}

void DFTransferFunctions::setInitialElementFactory(PropertyStateFactory* pf) {
  _initialElementFactory=pf;
}

PropertyStateFactory* DFTransferFunctions::getInitialElementFactory() {
  return _initialElementFactory;
}

void DFTransferFunctions::initializeExtremalValue(Lattice& element) {
  // default empty function
}

static
SgInitializer*
getVariableInitializer(SgVariableDeclaration* vd)
{
  ROSE_ASSERT(vd);

  SgInitializedNamePtrList& lst = vd->get_variables();

  if (lst.size() == 0)
    return nullptr;

  ROSE_ASSERT(lst.size() == 1 && lst[0]);
  return lst.front()->get_initptr();
}

static
SgVariableDeclaration*
chooseInitializer(SgVariableDeclaration* one, SgVariableDeclaration* two)
{
  ROSE_ASSERT(one || two);

  // eliminate nullptr
  if (!one) return two;
  if (!two) return one;

  // compare extern modifier
  const bool     oneext = one->get_declarationModifier().get_storageModifier().isExtern();
  const bool     twoext = two->get_declarationModifier().get_storageModifier().isExtern();

  if (oneext && !twoext) return two;
  if (twoext && !oneext) return one;

  // compare initialization
  SgInitializer* oneini = getVariableInitializer(one);
  SgInitializer* twoini = getVariableInitializer(two);

  // assume equally good
  //  \todo add comparison that oneini and twoini are different
  if (oneini && twoini) return nullptr;

  // oneini is not set -> choose two
  if (twoini) return two;

  // twoini is not set -> choose one
  return one;
}

namespace 
{
  // auxiliary wrapper for printing Sg_File_Info objects 
  struct SrcLoc
  {
    explicit
    SrcLoc(SgLocatedNode& n)
    : info(n.get_file_info())
    {}
    
    Sg_File_Info* info;
  };

  inline  
  std::ostream& operator<<(std::ostream& os, SrcLoc el)
  {
    return os << el.info->get_filenameString() 
              << "@" << el.info->get_line() << ":" << el.info->get_col();
  } 
}

typedef std::map<VariableId, SgVariableDeclaration*> VariableInitialzationMap;

static
void storeIfBetter( std::map<VariableId, SgVariableDeclaration*>& initmap,
                    std::pair<VariableId, SgVariableDeclaration*> cand
                  )
{
  ROSE_ASSERT (cand.second != nullptr); 
  
  VariableInitialzationMap::mapped_type& curr   = initmap[cand.first];
  VariableInitialzationMap::mapped_type  choice = chooseInitializer(cand.second, curr);

  if (!choice)
  {
    std::string currstr = curr->unparseToString();
    std::string candstr = cand.second->unparseToString();
    
    // suppress the warning if the two candidates unparse to the same string
    if (currstr != candstr)
    {
      logWarn() << "two equally good initializers found\n"
                << "      " << currstr << " [chosen]\n"
                << "      " << candstr << " [ignored]"
                << std::endl;
    }
  }
  else if (curr != choice)
  {
    //~ logWarn() << "+: " << (curr ? curr->unparseToString() : "<null>") << "/"
              //~ << choice->unparseToString()
              //~ << " - " << typeid(*choice).name()  
              //~ << SrcLoc(*choice)
              //~ << std::endl;
    curr = choice;
  }
  //~ else
  //~ {
    //~ logWarn() << "-: " << curr->unparseToString() << std::endl;
  //~ }
}

Lattice* DFTransferFunctions::initializeGlobalVariables(SgProject* root) {
  ROSE_ASSERT(root);
  cout << "INFO: Initializing property state with global variables."<<endl;
  VariableIdSet globalVars=AnalysisAbstractionLayer::globalVariables(root,getVariableIdMapping());
  VariableIdSet usedVarsInFuncs=AnalysisAbstractionLayer::usedVariablesInsideFunctions(root,getVariableIdMapping());
  VariableIdSet usedVarsInGlobalVarsInitializers=AnalysisAbstractionLayer::usedVariablesInGlobalVariableInitializers(root,getVariableIdMapping());
  VariableIdSet usedGlobalVarIds=globalVars; //*usedVarsInFuncs; //+usedVarsInGlobalVarsInitializers;;
  //  usedGlobalVarIds.insert(usedVarsInGlobalVarsInitializers.begin(),
  //        usedVarsInGlobalVarsInitializers.end());
  cout <<"INFO: number of global variables: "<<globalVars.size()<<endl;
  //  cout <<"INFO: used variables in functions: "<<usedVarsInFuncs.size()<<endl;
  //cout <<"INFO: used global vars: "<<usedGlobalVarIds.size()<<endl;
  Lattice* elem=_initialElementFactory->create();
  this->initializeExtremalValue(*elem);
  //cout << "INIT: initial element: ";elem->toStream(cout,getVariableIdMapping());
  list<SgVariableDeclaration*> globalVarDecls=SgNodeHelper::listOfGlobalVars(root);

  // map from VariableIds to the best initializer
  VariableInitialzationMap varinit;

  // collects "best" initializers for global variables
  for(SgVariableDeclaration* var : globalVarDecls) {
    VariableId varid = getVariableIdMapping()->variableId(var);

    if (  !isSgTemplateVariableDeclaration(var) 
       && usedGlobalVarIds.find(varid) != usedGlobalVarIds.end()
       ) {
      // cout << "DEBUG: transfer for global var @" << getLabeler()->getLabel(*i) << " : " << (*i)->unparseToString()
      //     << "id = " << getVariableIdMapping()->variableId(*i).toString()
      //     << endl;
      // ->get_declarationModifier().get_storageModifier().isExtern()
      storeIfBetter(varinit, std::make_pair(varid, var));
    } else {
      cout<<"INFO: filtered from initial state: "<< var->unparseToString() << endl;
    }
  }

  for (VariableInitialzationMap::value_type& init : varinit) {
    ROSE_ASSERT(init.second);

    cout<<"INFOx: "<< init.second->unparseToString() << endl;
    transfer(getLabeler()->getLabel(init.second), *elem);
  }

  //cout << "INIT: initial state: ";
  //elem->toStream(cout,getVariableIdMapping());
  //cout<<endl;
  return elem;
}

#endif
