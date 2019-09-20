#include "sage3basic.h"                                 // every librose .C file must start with this
#include "Diagnostics.h"

#include "FunctionIdMapping.h"
#include "RoseAst.h"

#include "CodeThornException.h"
#include "FunctionCallMapping.h"
#include <sstream>

using namespace Sawyer::Message;
using namespace std;
using namespace CodeThorn;

Sawyer::Message::Facility CodeThorn::FunctionCallMapping::logger;

void FunctionCallMapping::computeFunctionCallMapping(SgNode* root) {
  RoseAst ast(root);
  std::list<SgFunctionDeclaration*> funDeclList;
  std::list<SgFunctionDefinition*> funDefList;
  std::list<SgFunctionCallExp*> funCallList;
  for(auto node:ast) {
    if(SgFunctionCallExp* fc=isSgFunctionCallExp(node)) {
      funCallList.push_back(fc);
    } else if(SgFunctionDefinition* funDef=isSgFunctionDefinition(node)) {
      funDefList.push_back(funDef);
    } else if(SgFunctionDeclaration* funDecl=isSgFunctionDeclaration(node)) {
      funDeclList.push_back(funDecl);
    }
  }
  for (auto fc : funCallList) {
    // determine all components of call
    // name or fpointer or other
    // type of params
    FunctionCallTarget funCallTarget;
    SgFunctionDefinition* funDef1=SgNodeHelper::determineFunctionDefinition(fc);
    if(funDef1) {
      funCallTarget.setDefinition(funDef1);
    }
    mapping[fc].insert(funCallTarget);
  }
  cout<<"INFO: FunctionCallMapping established: "<<mapping.size()<<" resolved functions calls of "<<funCallList.size()<<"."<<endl;
}

std::string FunctionCallMapping::toString() {
  stringstream ss;
  for(auto fcall : mapping) {
    ss<<SgNodeHelper::sourceFilenameLineColumnToString(fcall.first)<<" : "<<fcall.first->unparseToString()<<" RESOLVED TO ";
    for(auto target : fcall.second) {
      ss<<target.toString()<<" ";
    }
    ss<<endl;
  }
  return ss.str();
}

void FunctionCallMapping::initDiagnostics() {
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
    logger = Sawyer::Message::Facility("CodeThorn::FunctionCallMapping", Rose::Diagnostics::destination);
    Rose::Diagnostics::mfacilities.insertAndAdjust(logger);
  }
}

FunctionCallTargetSet FunctionCallMapping::resolveFunctionCall(SgFunctionCallExp* funCall) {
  FunctionCallTargetSet fcs;
  if(funCall) {
    cout << "DEBUG: @FunctionCallMapping::resolveFunctionCall:"<<funCall->unparseToString()<<endl;
    auto iter=mapping.find(funCall);
    if(iter!=mapping.end()) {
      return (*iter).second;
    }
  }
  return fcs;
}
