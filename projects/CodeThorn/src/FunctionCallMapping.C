#include "sage3basic.h"                                 // every librose .C file must start with this
#include "Diagnostics.h"

#include "FunctionIdMapping.h"
#include "RoseAst.h"

#include "CodeThornException.h"
#include "FunctionCallMapping.h"
#include <sstream>
#include "AstTerm.h"

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
  // NOTE: SgFunctionDeclaration* funDecl=SgNodeHelper::findFunctionDeclarationWithFunctionSymbol(funSym);
  // SgName qfName=funDecl->get_qualified_name();

  cout<<"DUMP FUNCTON CALLS:"<<endl;
  for (auto fc : funCallList) {
    SgExpression* exp=fc->get_function();
    SgExprListExp* funCallArgs=fc->get_args();
    if(SgFunctionRefExp* functionRef=isSgFunctionRefExp(exp)) {
      // direct function call
      SgFunctionSymbol* funSym=functionRef->get_symbol();
      assert(funSym);
      SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(funSym);
      string funCallName=functionSymbol->get_name();
      SgFunctionType* funCallType = isSgFunctionType(functionSymbol->get_type());
      SgName mangledFunCallType=funCallType->get_mangled();
      cout<<"CALL: DIRECT-";
      cout<<"NAME: "<<funCallName<<" TYPE: "<<funCallType->unparseToString()<<" MANGLEDFUNCALLTYPE: "<<mangledFunCallType<<endl;
    } else if(SgVarRefExp* varRefExp=isSgVarRefExp(exp)) {
      // function pointer call
      SgType* type=varRefExp->get_type();
      const SgPointerType* pointerType = SgNodeHelper::isPointerType(type);
      ROSE_ASSERT(pointerType);
      SgFunctionType* funType=isSgFunctionType(pointerType->get_base_type());
      SgName mangledFunCallType=funType->get_mangled();
      ROSE_ASSERT(funType);
      cout<<"CALL: FP-"<<"TYPE: "<<funType->unparseToString()<<" MANGLEDFUNCALLTYPE: "<<mangledFunCallType<<endl;
    } else {
      cout<<"UNKNOWN FUNCTION CALL EXP."<<endl;
    }
  }
  for (auto fd : funDefList) {
    SgName funDefName=fd->get_declaration()->get_name();//fd->get_qualified_name();
    SgFunctionType* funDefType= fd->get_declaration()->get_type();
    SgName mangledFunDefType=funDefType->get_mangled();
    cout<<"DEF: NAME:"<<funDefName<<" TYPE:"<<funDefType->unparseToString();
    cout<<"  MANGLEDTYPE : "<<mangledFunDefType<<endl;

  }
  return;
  
  for (auto fc : funCallList) {
    // determine all components of call
    // name or fpointer or other
    // type of params
    SgExpression* exp=fc->get_function();
    //cout<<"get_function:exp:"<<AstTerm::astTermWithNullValuesAndTypesToString(exp)<<endl;
    SgExprListExp* funCallArgs=fc->get_args();
    //SgFunctionSymbol* funcSymbol = SgNodeHelper::getSymbolOfFunctionDeclaration(funcDecl);

    bool isFunctionPointer=false;
    SgName funCallName;
    SgFunctionType* funCallType=nullptr;
    FunctionCallTarget funCallTarget;
    if(SgFunctionDefinition* funDef=SgNodeHelper::determineFunctionDefinition(fc)) {
      funCallTarget.setDefinition(funDef);
      funCallTarget.setDeclaration(funDef->get_declaration());
      mapping[fc].insert(funCallTarget);
      return;
    } else if(SgFunctionRefExp* functionRef=isSgFunctionRefExp(exp)) {
      // direct function call
      SgFunctionSymbol* funSym=functionRef->get_symbol();
      assert(funSym);
      SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(funSym);
      funCallName=functionSymbol->get_name();
      funCallType = isSgFunctionType(functionSymbol->get_type());
    } else if(SgVarRefExp* varRefExp=isSgVarRefExp(exp)) {
      // function pointer call
      SgType* type=varRefExp->get_type();
      if(const SgPointerType* pointerType = SgNodeHelper::isPointerType(type)) {
        if(SgFunctionType* funType=isSgFunctionType(pointerType->get_base_type())) {
          isFunctionPointer=true;
          funCallType=funType;
          assert(funCallType);
        } else {
          SAWYER_MESG(logger[DEBUG])<<"DEBUG: unknown function call."<<endl;
          exit(1);
        }
      } else {
        SAWYER_MESG(logger[DEBUG])<<"DEBUG: unknown function call."<<endl;
        exit(1);
      }
      //cout<<AstTerm::astTermWithNullValuesAndTypesToString(type)<<endl;
    }
    
    SgName mangledFunCallType=funCallType->get_mangled();
    // try to match type now
    for(auto fd : funDefList) {
      //SgInitializedNamePtrList& funDefParams=SgNodeHelper::getFunctionDefinitionFormalParameterList(fd);
      //SgName funDefMangledName=fd->get_mangled_name();
      SgFunctionType* funDefType= fd->get_declaration()->get_type();
      SgName funDefName=fd->get_declaration()->get_name(); //fd->get_qualified_name(); 
      SgName mangledFunDefType=funCallType->get_mangled();
      cout<<"Source: "<<fc->unparseToString()<<endl;
      cout<<"FUNCALLTYPE: "<<AstTerm::astTermWithNullValuesAndTypesToString(funCallType)<<endl;
      cout<<"FUNDEFTYPE: "<<AstTerm::astTermWithNullValuesAndTypesToString(funCallType)<<endl;
      cout<<"MANGLEDFUNCALLTYPE: "<<mangledFunCallType<<endl;
      cout<<"MANGLEDFUNDEFTYPE : "<<mangledFunDefType<<endl;
      if(funCallType==funDefType || mangledFunCallType==mangledFunDefType) {
        if(funCallName==funDefName || isFunctionPointer) {
          //SAWYER_MESG(logger[TRACE])<<"RESOLVED CALL across translation units based on TYPES and Names: "<<fc->unparseToString()<<" ::: "<<fd->get_declaration()->unparseToString()<<endl;
	  //cout<<"RESOLVED CALL across translation units based on TYPES and Names: "<<fc->unparseToString()<<" ::: "<<fd->get_declaration()->unparseToString()<<endl;
	  if(isFunctionPointer)
	    cout<<"RESOLVED CALL: function pointer to def name: "<<funDefName<<endl;
	  else
	    cout<<"RESOLVED CALL: mismatching names:"<<funCallName<<":"<<funDefName<<endl;
          funCallTarget.setDefinition(fd);
          funCallTarget.setDeclaration(fd->get_declaration());
        } else {
	  cout<<"ONLY TYPES MATCH";
	  if(isFunctionPointer)
	    cout<<" function pointer"<<endl;
	  else
	    cout<<" mismatching names:"<<funCallName<<":"<<funDefName<<endl;
	}
      } else {
	cout<<"NOT MATCHED."<<endl;
      }
      cout<<endl;
    }
    mapping[fc].insert(funCallTarget);
  }
  SAWYER_MESG(logger[TRACE])<<"INFO: FunctionCallMapping established: resolved "<<mapping.size()<<" of "<<funCallList.size()<<" function calls."<<endl;
  SAWYER_MESG(logger[TRACE])<<toString()<<endl;
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
    SAWYER_MESG(logger[TRACE]) << "DEBUG: @FunctionCallMapping::resolveFunctionCall:"<<funCall->unparseToString()<<endl;
    auto iter=mapping.find(funCall);
    if(iter!=mapping.end()) {
      return (*iter).second;
    }
  }
  return fcs;
}
