
#include "sage3basic.h"
#include "DeadCodeElimination.h"
#include "TrivialInlining.h"

using namespace std;
using namespace CodeThorn;

bool DeadCodeElimination::isVariableOfInterest(VariableId varId) {
  return variablesOfInterest.find(varId)!=variablesOfInterest.end();
}

void DeadCodeElimination::setVariablesOfInterest(VariableIdSet& vidSet) {
  variablesOfInterest=vidSet;
}

DeadCodeElimination::DeadCodeElimination():
  detailedOutput(false),
  elimVar(0),
  elimAssignment(0),
  elimVarUses(0),
  elimFunctions(0)
{
}

void DeadCodeElimination::setDetailedOutput(bool verbose) {
  detailedOutput=verbose;
}

bool DeadCodeElimination::isEmptyBlock(SgNode* node) {
  if(node==0)
    return true;
  //cout<<"isEmptyBasicBlock:"<<node->class_name()<<endl;
  if(SgBasicBlock* block=isSgBasicBlock(node)) {
    const SgStatementPtrList& stmtList=block->get_statements(); 
    return stmtList.size()==0;
  }
  return false;
}

// an if-stmt is empty if both branches are empty blocks (or do not exist).
bool DeadCodeElimination::isEmptyIfStmt(SgIfStmt* ifStmt) {
  SgStatement* trueBody=ifStmt->get_true_body();
  SgStatement* falseBody=ifStmt->get_false_body();
  return isEmptyBlock(trueBody) && isEmptyBlock(falseBody);
}

list<SgIfStmt*> DeadCodeElimination::listOfEmptyIfStmts(SgNode* node) {
  list<SgIfStmt*> ifstmts;
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();i++) {
    if(SgIfStmt* ifStmt=isSgIfStmt(*i)) {
      if(isEmptyIfStmt(ifStmt)) {
        ifstmts.push_back(ifStmt);
      }
    }
  }
  return ifstmts;
}

size_t DeadCodeElimination::eliminateEmptyIfStmts(SgNode* node) {
  size_t numElim=0;
  list<SgIfStmt*> ifstmts=listOfEmptyIfStmts(node);
  for(list<SgIfStmt*>::iterator i=ifstmts.begin();i!=ifstmts.end();i++) {
    SageInterface::removeStatement (*i, false);
    numElim++;
  }
  return numElim;
}

std::set<SgFunctionDefinition*> DeadCodeElimination::calledFunctionDefinitions(std::list<SgFunctionCallExp*> functionCalls) {
  std::set<SgFunctionDefinition*> calledFunctions;
  for(std::list<SgFunctionCallExp*>::iterator i=functionCalls.begin();i!=functionCalls.end();++i) {
    SgFunctionDefinition* functionDef=isSgFunctionDefinition(SgNodeHelper::determineFunctionDefinition(*i));
    if(functionDef) {
      calledFunctions.insert(functionDef);
    }
  }
  return calledFunctions;
}

std::set<SgFunctionDefinition*> DeadCodeElimination::NonCalledTrivialFunctions(SgNode* root0) {
  SgProject* root=isSgProject(root0);
  ROSE_ASSERT(root);
  list<SgFunctionDefinition*> funDefs=SgNodeHelper::listOfFunctionDefinitions(root);
  std::list<SgFunctionCallExp*> tfCalls=TrivialInlining::trivialFunctionCalls(root);
  std::set<SgFunctionDefinition*> calledFunDefs=calledFunctionDefinitions(tfCalls);
  std::set<SgFunctionDefinition*> nonCalledFunDefs;
  for(list<SgFunctionDefinition*>::iterator i=funDefs.begin();i!=funDefs.end();++i) {
    if(TrivialInlining::isTrivialFunctionDefinition(*i)) {
      if(calledFunDefs.find(*i)==calledFunDefs.end()) {
        nonCalledFunDefs.insert(*i);
      }
    }
  }
  return nonCalledFunDefs;
}

void DeadCodeElimination::eliminateFunctions(std::set<SgFunctionDefinition*>& funDefs) {
  for(std::set<SgFunctionDefinition*>::iterator i=funDefs.begin();i!=funDefs.end();++i) {
    string funName=SgNodeHelper::getFunctionName(*i);
    SgFunctionDeclaration* funDecl=(*i)->get_declaration();
    if(funName!="main") {
      if(detailedOutput) cout<<"Deleting function: "<<funName<<endl;
      SgStatement* stmt=funDecl;
      SageInterface::removeStatement(stmt, false);
    }
  }
}

size_t DeadCodeElimination::eliminateNonCalledTrivialFunctions(SgNode* root) {
  std::set<SgFunctionDefinition*> nonCalledTrivialFunctions=NonCalledTrivialFunctions(root);
  eliminateFunctions(nonCalledTrivialFunctions);
  elimFunctions=nonCalledTrivialFunctions.size();
  return elimFunctions;
}

// returns the number of all eliminated elements (variables+decls+subexpr)
int DeadCodeElimination::eliminateDeadCodePhase1(SgNode* root,
                                                 VariableIdMapping* variableIdMapping,
                                                 VariableConstInfo& vci) {
  RoseAst ast1(root);

  // eliminate variables with one value only
  // 1) eliminate declaration of variable
  // 2) eliminate assignment to variable
  // 3) replace use of variable
  cout<<"STATUS: Dead code elimination phase 1: Eliminating variables."<<endl;
  cout<<"STATUS: Collecting variables, assignments, and expressions."<<endl;
  list<SgVariableDeclaration*> toDeleteVarDecls;
  list<SgAssignOp*> toDeleteAssignments;
  list<pair<SgExpression*,SgExpression*> > toReplaceExpressions;
  for(RoseAst::iterator i=ast1.begin();i!=ast1.end();++i) {
    if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
      VariableId declVarId=variableIdMapping->variableId(varDecl);
      if(isVariableOfInterest(declVarId) && vci.isUniqueConst(declVarId)) {
        toDeleteVarDecls.push_back(varDecl);
      }
    }
    if(SgAssignOp* assignOp=isSgAssignOp(*i)) {
      VariableValuePair varValPair;
      bool found=FIConstAnalysis::analyzeAssignment(assignOp,*variableIdMapping, &varValPair);
      if(found) {
        VariableId varId=varValPair.varId;
        if(isVariableOfInterest(varId) && vci.isUniqueConst(varId)) {
          toDeleteAssignments.push_back(assignOp);
        }
      }
    }
    if(SgVarRefExp* varRef=isSgVarRefExp(*i)) {
      VariableId varId;
      bool found=FIConstAnalysis::determineVariable(varRef, varId, *variableIdMapping);
      if(found) {
        if(isVariableOfInterest(varId) && vci.isUniqueConst(varId)) {
          SgIntVal* newSgIntValExpr=SageBuilder::buildIntVal(vci.uniqueConst(varId));
          toReplaceExpressions.push_back(make_pair(varRef,newSgIntValExpr));
        }
      }
    }
  }
  
  cout<<"STATUS: eliminating declarations."<<endl;
  elimVar=0;
  for(list<SgVariableDeclaration*>::iterator i=toDeleteVarDecls.begin();
      i!=toDeleteVarDecls.end();
      ++i) {
    elimVar++;
    if(detailedOutput) cout<<"Eliminating dead variable's declaration: "<<(*i)->unparseToString()<<endl;
    SageInterface::removeStatement(*i, false);
  }
  cout<<"STATUS: eliminating assignments."<<endl;
  elimAssignment=0;
  for(list<SgAssignOp*>::iterator i=toDeleteAssignments.begin();
      i!=toDeleteAssignments.end();
      ++i) {
    SgExprStatement* exprStatAssign=isSgExprStatement(SgNodeHelper::getParent(*i));
    if(!exprStatAssign) {
      cerr<<"Error: assignments inside expressions are not supported yet.";
      cerr<<"Problematic assignment: "<<(*i)->unparseToString()<<endl;
      exit(1);
    }
    elimAssignment++;
    if(detailedOutput) cout<<"Eliminating dead variable assignment: "<<(*i)->unparseToString()<<endl;
    SageInterface::removeStatement(exprStatAssign, false);
  }
  
  cout<<"STATUS: eliminating expressions."<<endl;
  elimVarUses=0;
  for(list<pair<SgExpression*,SgExpression*> >::iterator i=toReplaceExpressions.begin();
      i!=toReplaceExpressions.end();
      ++i) {
    elimVarUses++;
    if(detailedOutput) cout<<"Replacing use of variable with constant: "<<(*i).first->unparseToString()<<" replaced by "<<(*i).second->unparseToString()<<endl;
    SgNodeHelper::replaceExpression((*i).first, (*i).second);
  }
  return elimVar+elimAssignment+elimVarUses;
}

int DeadCodeElimination::numElimVars() { return elimVar; }
int DeadCodeElimination::numElimAssignments() { return elimAssignment; }
int DeadCodeElimination::numElimVarUses() { return elimVarUses; }
int DeadCodeElimination::numElimFunctions() { return elimFunctions; }
