#include "sage3basic.h"
#include "TrivialInlining.h"
#include "SgNodeHelper.h"
#include "RoseAst.h"

using namespace std;

TrivialInlining::TrivialInlining():detailedOutput(false) {
}
void TrivialInlining::setDetailedOutput(bool val) {
  detailedOutput=val;
}

bool TrivialInlining::trivialInline(SgFunctionCallExp* funCall) {
  /*
    0) check if it is a trivial function call (no return value, no params)
    1) find function to inline
    2) determine body of function to inline
    3) delete function call
    4) clone body of function to inline
    5) insert cloned body as block
  */
  //string fname=SgNodeHelper::getFunctionName(funCall);
  SgFunctionDefinition* functionDef=isSgFunctionDefinition(SgNodeHelper::determineFunctionDefinition(funCall));
  if(!functionDef)
    return false;
  SgBasicBlock* functionBody=isSgBasicBlock(functionDef->get_body());
  if(!functionBody)
    return false;
  SgTreeCopy tc;
  SgBasicBlock* functionBodyClone=isSgBasicBlock(functionBody->copy(tc));
  // set current basic block as parent of body
  if(!functionBodyClone)
    return false;
  SgExprStatement* functionCallExprStmt=isSgExprStatement(funCall->get_parent());
  if(!functionCallExprStmt)
    return false;
  SgBasicBlock* functionCallBlock=isSgBasicBlock(functionCallExprStmt->get_parent());
  if(!functionCallBlock)
    return false;
  if(functionCallBlock->get_statements().size()>0) {
    SgStatement* oldStmt=functionCallExprStmt;
    SgStatement* newStmt=functionBodyClone;
    SageInterface::replaceStatement(oldStmt, newStmt,false);
    return true;
  }
  return false;
}

bool TrivialInlining::isTrivialFunctionDefinition(SgFunctionDefinition* funDef) {
  SgType* returnType=SgNodeHelper::getFunctionReturnType(funDef);
  SgInitializedNamePtrList& paramList=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
  return isSgTypeVoid(returnType) && paramList.size()==0;
}

SgFunctionCallExp* TrivialInlining::isTrivialFunctionCall(SgNode* node) {
  if(SgFunctionCallExp* funCall=isSgFunctionCallExp(node)) {
    SgExpressionPtrList& args=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    if(args.size()==0) {
      if(SgFunctionDefinition* funDef=SgNodeHelper::determineFunctionDefinition(funCall)) {
        SgType* returnType=SgNodeHelper::getFunctionReturnType(funDef);
        if(isSgTypeVoid(returnType)) {
          return funCall;
        }                
      }
    }
  }
  return 0;
}

list<SgFunctionCallExp*> TrivialInlining::trivialFunctionCalls(SgNode* node) {
  RoseAst ast(node);
  list<SgFunctionCallExp*> funCallList;
  for(RoseAst::iterator i=ast.begin();i!=ast.end();i++) {
    if(SgFunctionCallExp* funCall=isTrivialFunctionCall(*i)) {
      funCallList.push_back(funCall);
    }
  }
  return funCallList;
}


size_t TrivialInlining::inlineFunctionCalls(list<SgFunctionCallExp*>& funCallList) {
  size_t num=0;
  for(list<SgFunctionCallExp*>::iterator i=funCallList.begin();i!=funCallList.end();i++) {
    SgFunctionCallExp* funCall=*i;
    if(detailedOutput) cout<< "function call:"<<SgNodeHelper::nodeToString(*i)<<": ";
    bool success=trivialInline(funCall);
    if(success) {
      if(detailedOutput) cout<<"inlined."<<endl;
      num++;
    }
    else
      cout<<"not inlined."<<endl;
  }
  return num;
}

bool TrivialInlining::inlineFunctions(SgNode* root) {
  bool performedInlining=false;
  list<SgFunctionCallExp*> funCallList=trivialFunctionCalls(root);
  if(detailedOutput)
    cout<<"STATUS: Inlining: Number of trivial function calls (with existing function bodies): "<<funCallList.size()<<endl;
  list<SgFunctionCallExp*> remainingFunCalls=trivialFunctionCalls(root);
  while(remainingFunCalls.size()>0) {
    performedInlining=true;
    size_t numFunCall=remainingFunCalls.size();
    if(detailedOutput)
      cout<<"INFO: Remaining function calls: "<<numFunCall<<endl;
    if(numFunCall>0)
      inlineFunctionCalls(remainingFunCalls);
    remainingFunCalls=trivialFunctionCalls(root);
  }
  return performedInlining;
}
