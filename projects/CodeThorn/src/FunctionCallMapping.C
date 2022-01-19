#include "sage3basic.h"                                 // every librose .C file must start with this
#include "Rose/Diagnostics.h"

#include "FunctionIdMapping.h"
#include "RoseAst.h"

#include "CallGraph.h"

#include "CodeThornException.h"
#include "FunctionCallMapping.h"

#include <sstream>
#include "AstTerm.h"

#include "CppStdUtilities.h"

using namespace Sawyer::Message;
using namespace std;
using namespace CodeThorn;

Sawyer::Message::Facility CodeThorn::FunctionCallMapping::logger;

FunctionCallMapping::FunctionCallMapping() {
  _matchMode=4;
}

void FunctionCallInfo::print() {
  if(funCallType) {
    cout<<"NAME: "<<funCallName<<" TYPE: "<<funCallType<<":"<<funCallType->unparseToString()<<" MANGLEDFUNCALLNAME: "<<mangledFunCallName<<endl;
  } else {
    cout<<"NAME: "<<funCallName<<" TYPE: unknown"<<endl;
  }
}

SgFunctionDeclaration* FunctionCallInfo::getFunctionDeclaration() {
  if(functionSymbol)
    return functionSymbol->get_declaration ();
  else
    return nullptr;
}

bool FunctionCallInfo::isFunctionPointerCall() {
  return _functionPointerCallFlag;
}

void FunctionCallInfo::setFunctionPointerCallFlag(bool flag) {
  _functionPointerCallFlag=flag;
}

FunctionCallInfo* FunctionCallMapping::getFunctionCallInfoPtr(SgFunctionCallExp* fc) {
  ROSE_ASSERT(isValidMapping());
  return getFunctionCallInfoPtrUnsafe(fc);
}

FunctionCallInfo* FunctionCallMapping::getFunctionCallInfoPtrUnsafe(SgFunctionCallExp* fc) {
  if(fc) {
    auto iter=mapping.find(fc);
    if(iter!=mapping.end()) {
      return &mapping[fc].first;
    }
  }
  return nullptr;
}

FunctionCallTargetSet* FunctionCallMapping::getFunctionCallTargetSetPtr(SgFunctionCallExp* fc) {
  ROSE_ASSERT(isValidMapping());
  return getFunctionCallTargetSetPtrUnsafe(fc);
}

FunctionCallTargetSet* FunctionCallMapping::getFunctionCallTargetSetPtrUnsafe(SgFunctionCallExp* fc) {
  if(fc) {
    auto iter=mapping.find(fc);
    if(iter!=mapping.end()) {
      return &mapping[fc].second;
    }
  }
  return nullptr;
}

FunctionCallTargetSet FunctionCallMapping::getFunctionCallTargetSet(SgFunctionCallExp* fc) {
  if(auto targetSetPtr=getFunctionCallTargetSetPtr(fc)) {
    return *targetSetPtr;
  } else {
    FunctionCallTargetSet emptySet;
    return emptySet;
  }
}


bool FunctionCallMapping::isFunctionPointerCall(SgFunctionCallExp* fc) {
  ROSE_ASSERT(isValidMapping());
  if(auto p=getFunctionCallInfoPtr(fc))
    return p->isFunctionPointerCall();
  else
    return false;
}

bool FunctionCallMapping::isValidMapping() {
  return _mappingValidFlag;
}

FunctionCallMapping::ExternalFunctionNameContainerType FunctionCallMapping::getExternalFunctionNames() {
  ROSE_ASSERT(isValidMapping());
  FunctionCallMapping::ExternalFunctionNameContainerType efnSet;
  for(auto p:mapping) {
    auto functionCallInfo=p.second.first;
    if(p.second.second.size()==0 && functionCallInfo.infoAvailable && !functionCallInfo.isFunctionPointerCall()) {
      string fName=functionCallInfo.funCallName;
      string fDecl;
      string fFile;
      if(auto funDecl=functionCallInfo.getFunctionDeclaration()) {
	fDecl=funDecl->unparseToString();
	fFile=SgNodeHelper::sourceFilenameToString(funDecl);
      } else {
	fFile="?";
	fDecl="?";
      }
      efnSet.insert(fFile+";"+fName+";"+fDecl);
    }
  }
  return efnSet;
}

bool FunctionCallMapping::isAstFunctionPointerCall(SgFunctionCallExp* fc) {
  SgExpression*    exp=fc->get_function();
  if(SgVarRefExp* varRefExp=isSgVarRefExp(exp)) {
    SgType* type=varRefExp->get_type();
    if(SgNodeHelper::isPointerType(type)) {
      return true;
    }
  }
  return false;
}

FunctionCallInfo FunctionCallMapping::determineFunctionCallInfo(SgFunctionCallExp* fc) {
    SgExpression*    exp=fc->get_function();
    FunctionCallInfo fcInfo;
    fcInfo.infoAvailable=false;  
    //cout<<"DEBUG:EXP:"<<AstTerm::astTermWithNullValuesToString(exp);
    //cout<<"DEBUG: determineFunctionCallInfo: "<<fc->unparseToString()<<":";
    if(SgFunctionRefExp* functionRef=isSgFunctionRefExp(exp)) {
      // direct function call
      //cout<<"DIRECT"<<endl;
      SgFunctionSymbol* funSym=functionRef->get_symbol();
      ROSE_ASSERT(funSym);
      SgFunctionSymbol* functionSymbol=isSgFunctionSymbol(funSym);
      fcInfo.functionSymbol=functionSymbol;
      fcInfo.funCallType=isSgFunctionType(functionSymbol->get_type());
      fcInfo.funCallName=functionSymbol->get_name();
      fcInfo.mangledFunCallName=fcInfo.funCallType->get_mangled();
      fcInfo.infoAvailable = true;
    } else if(SgVarRefExp* varRefExp=isSgVarRefExp(exp)) {
      //cout<<"FP INDIRECT"<<endl;
      // function pointer call
      //cout<<"DEBUG: FUNCTION POINTER CALL : determineFunctionCallInfo: "<<fc->unparseToString()<<endl;
      SgType* type=varRefExp->get_type();
      
      if (const SgPointerType* pointerType = SgNodeHelper::isPointerType(type)) {
        SgFunctionType* funCallType=isSgFunctionType(pointerType->get_base_type());
        ROSE_ASSERT(funCallType);
        fcInfo.funCallName="*";
        fcInfo.funCallType=funCallType;
        fcInfo.mangledFunCallName=funCallType->get_mangled();
        fcInfo.infoAvailable = true;
	fcInfo.setFunctionPointerCallFlag(true);
      } 
    } else if(SgPointerDerefExp* derefOp=isSgPointerDerefExp(exp)) {
      SgExpression* exp=derefOp->get_operand();
      ROSE_ASSERT(exp);
      SgVarRefExp* varRefExp=isSgVarRefExp(exp);
      if(varRefExp) {
        //cout<<"FP INDIRECT"<<endl;
        // function pointer call
        //cout<<"DEBUG: FUNCTION POINTER CALL : determineFunctionCallInfo: "<<fc->unparseToString()<<endl;
        SgType* type=varRefExp->get_type();
        //cout<<"DEBUG: FP TYPE:"<<type->unparseToString()<<endl;
        //cout<<"DEBUG: FP TYPE:"<<AstTerm::astTermWithNullValuesToString(type);
        if (const SgPointerType* pointerType = SgNodeHelper::isPointerType(type)) {
          SgFunctionType* funCallType=isSgFunctionType(pointerType->get_base_type());
          ROSE_ASSERT(funCallType);
          fcInfo.funCallName="*";
          fcInfo.funCallType=funCallType;
          fcInfo.mangledFunCallName=funCallType->get_mangled();
          fcInfo.infoAvailable = true;
	  fcInfo.setFunctionPointerCallFlag(true);
          //cout<<"DEBUG: FP : correct functin pointer:"<<fcInfo.mangledFunCallName<<endl;
        } else {
          // should not happen
        }
      } 
    } else {
      logger[WARN]<<"Function pointer dereference on non-normalized expression: "<<fc->unparseToString()<<endl;
    }
    if (!fcInfo.infoAvailable) {
      //cout<<"UNKNOWN: "<<fc->unparseToString()<<endl;
      // provide information for error reporting
      fcInfo.funCallName="unknown:"+fc->unparseToString();
      fcInfo.funCallType=nullptr; // indicates unknown type
    }
    
    return fcInfo;
}

std::string FunctionCallMapping::funDefTypeToString(SgFunctionDefinition* fn)
{
  return fn->get_declaration()->get_type()->unparseToString();
}

void FunctionCallMapping::collectRelevantNodes(SgNode* root) {
  RoseAst ast(root);
  for(auto node:ast) {
    if(SgFunctionCallExp* fc=isSgFunctionCallExp(node)) {
      funCallList.push_back(fc);
    } else if(SgFunctionDefinition* funDef=isSgFunctionDefinition(node)) {
      funDefList.push_back(funDef);
    }
  }
}

void FunctionCallMapping::dumpFunctionCallInfo() {
  ROSE_ASSERT(isValidMapping());
  for (auto fc : funCallList) {
    FunctionCallInfo fcInfo=determineFunctionCallInfo(fc);
    fcInfo.print();
  }
}

void FunctionCallMapping::dumpFunctionCallTargetInfo() {
  ROSE_ASSERT(isValidMapping());
  for (auto fd : funDefList) {
    FunctionCallTarget fcTarget(fd);
    fcTarget.print();
  }
}

void FunctionCallMapping::computeFunctionCallMapping(SgNode* root) {
  SAWYER_MESG(logger[INFO])<<"Computing FunctionCallMapping1"<<endl;
  collectRelevantNodes(root);
  SAWYER_MESG(logger[INFO])<< "Number of "<<" function call sites :" <<funCallList.size()<<endl;
  SAWYER_MESG(logger[INFO])<< "Number of "<<" function definitions:" <<funDefList.size()<<endl;
  // ensure that an entry exists for every function call (even if it cannot be resolved)
  for (auto fc : funCallList) {
    mapping[fc]=make_pair(determineFunctionCallInfo(fc),FunctionCallTargetSet());
  }
  // resolve all function calls
  for (auto fc : funCallList) {
    auto fcInfoPtr=getFunctionCallInfoPtrUnsafe(fc);
    ROSE_ASSERT(fcInfoPtr);
    auto fcInfo=*fcInfoPtr;
    if(fcInfo.infoAvailable) {
      SgFunctionDefinition* funDef1=SgNodeHelper::determineFunctionDefinition(fc);
      if(funDef1) {
	// if it can be resolved with the info in the AST, this is the only target
        FunctionCallTarget fcTarget(funDef1);
	getFunctionCallTargetSetPtrUnsafe(fc)->insert(fcTarget);
	continue;
      }
      for (auto fd : funDefList) {
        FunctionCallTarget fcTarget(fd);
        bool matching;
        switch(_matchMode) {
        case 1:
        // this one should be working (but has a bug)
          matching=(fcInfo.funCallType==fcTarget.getFunctionType());
          break;
        case 2:
          // this one should be working (but has a bug)
          matching=(fcInfo.mangledFunCallName==fcTarget.getMangledFunctionName());
          break;
        case 3:
          // this one works as workaround
          matching=(fcInfo.funCallType->unparseToString()==fcTarget.getFunctionType()->unparseToString());
          break;
        case 4:
          // this one works as workaround
          matching=(fcInfo.getFunctionName()==fcTarget.getFunctionName());
          break;
        case 5: {
          //bool matching1=(fcInfo.funCallType->unparseToString()==fcTarget.getFunctionType()->unparseToString());
          string funCallTypeName=fcInfo.mangledFunCallName;
          string fcTargetTypeName=fcTarget.getMangledFunctionName();
          bool matching1=(funCallTypeName==fcTargetTypeName);
          bool matching2=(fcInfo.getFunctionName()==fcTarget.getFunctionName());
          matching=matching1||matching2;
          // problematic handles special case of unspecified function pointers in C
          //SAWYER_MESG(logger[INFO])<<"FunctionMatching: "<<fcInfo.getFunctionName()<<" : "<<funCallTypeName<<"<=>"<<fcTargetTypeName<<":::"<<matching<<endl;
          break;
          }
        default:
          SAWYER_MESG(logger[FATAL])<<"Error: FunctionCallMapping: unknown function matchmode "<<_matchMode<<endl;
          exit(1);
        }
        if(matching) {
          if(fcInfo.isFunctionPointerCall()) {
            mapping[fc].second.insert(fcTarget);
          } else if(fcInfo.funCallName==fcTarget.getFunctionName()) {
            mapping[fc].second.insert(fcTarget);
          }
        }
      }
    } else {
      SAWYER_MESG(logger[WARN])<<"No function call site info available: "<<fc->unparseToString()<<endl;
    }
  }
  _mappingValidFlag=true;
}

std::string FunctionCallMapping::toString() {
  stringstream ss;
  for(auto fcEntry : mapping) {
    ss<<SgNodeHelper::sourceFilenameLineColumnToString(fcEntry.first)<<" : "<<fcEntry.first->unparseToString()<<" RESOLVED TO {";
    for(auto target : getFunctionCallTargetSet(fcEntry.first)) {
      ss<<target.toString()<<" ";
    }
    ss<<"}"<<endl;
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
  ROSE_ASSERT(isValidMapping());
  return getFunctionCallTargetSet(funCall);
}

