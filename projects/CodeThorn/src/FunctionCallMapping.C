#include "sage3basic.h"                                 // every librose .C file must start with this
#include "Diagnostics.h"

#include "FunctionIdMapping.h"
#include "RoseAst.h"

#include "CallGraph.h"

#include "CodeThornException.h"
#include "FunctionCallMapping.h"

#include <sstream>
#include "AstTerm.h"

using namespace Sawyer::Message;
using namespace std;
using namespace CodeThorn;

Sawyer::Message::Facility CodeThorn::FunctionCallMapping::logger;

void FunctionCallInfo::print() {
  if(funCallType) {
    cout<<"NAME: "<<funCallName<<" TYPE: "<<funCallType<<":"<<funCallType->unparseToString()<<" MANGLEDFUNCALLTYPE: "<<mangledFunCallTypeName<<endl;
  } else {
    cout<<"NAME: "<<funCallName<<" TYPE: unknown"<<endl;
  }
}

bool FunctionCallInfo::isFunctionPointerCall() {
  return funCallName=="*";
}

FunctionCallInfo FunctionCallMapping::determineFunctionCallInfo(SgFunctionCallExp* fc) {
    SgExpression*    exp=fc->get_function();
    FunctionCallInfo fcInfo;
    bool             functionResolved = false;  
    if(SgFunctionRefExp* functionRef=isSgFunctionRefExp(exp)) {
      // direct function call
      SgFunctionSymbol* funSym=functionRef->get_symbol();
      assert(funSym);
      SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(funSym);
      fcInfo.funCallType = isSgFunctionType(functionSymbol->get_type());
      fcInfo.funCallName=functionSymbol->get_name();
      fcInfo.mangledFunCallTypeName=fcInfo.funCallType->get_mangled();
      functionResolved = true;
    } else if(SgVarRefExp* varRefExp=isSgVarRefExp(exp)) {
      // function pointer call
      SgType* type=varRefExp->get_type();
      
      if (const SgPointerType* pointerType = SgNodeHelper::isPointerType(type)) {
        SgFunctionType* funCallType=isSgFunctionType(pointerType->get_base_type());
        ROSE_ASSERT(funCallType);
        fcInfo.funCallName="*";
        fcInfo.funCallType=funCallType;
        fcInfo.mangledFunCallTypeName=funCallType->get_mangled();
        functionResolved = true;
      } 
    } 
    
    if (!functionResolved) {
      // provide information for error reporting
      fcInfo.funCallName=fc->unparseToString();
      fcInfo.funCallType=nullptr; // indicates unknown type
    }
    
    return fcInfo;
}

static
float percent(int part, int whole)
{
  return (part*100.0)/whole;
}

std::string nameOfType(SgFunctionDefinition* fn)
{
  return fn->get_declaration()->get_type()->unparseToString();
}

void FunctionCallMapping::computeFunctionCallMapping(SgNode* root) {
  RoseAst ast(root);
  std::vector<SgFunctionDeclaration*> funDeclList;
  std::vector<SgFunctionDefinition*>  funDefList;
  std::vector<SgFunctionCallExp*>     funCallList;
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
#if 0
  SAWYER_MESG(logger[TRACE])<<"DUMP FUNCTION CALLS:"<<endl;
  for (auto fc : funCallList) {
    FunctionCallInfo fcInfo=determineFunctionCallInfo(fc);
    fcInfo.print();
  }
  SAWYER_MESG(logger[TRACE])<<"DUMP FUNCTION TARGETS:"<<endl;
  for (auto fd : funDefList) {
    FunctionCallTarget fcTarget(fd);
    fcTarget.print();
  }

  SAWYER_MESG(logger[TRACE])<<"RESOLVING FUNCTION CALLS"<<endl;
#endif
  SAWYER_MESG(logger[INFO])<< "Found " << funDefList.size() << " function definitions." <<endl;
  SAWYER_MESG(logger[INFO])<< "Resolving " << funCallList.size() << " function call sites." <<endl;
  
  // experimental O(m lg n) + O(n lg n) matching
  ROSE_ASSERT(_matchMode == 3);
  
  if (_matchMode == 3) {
    std::sort( funDefList.begin(), funDefList.end(), 
               [](SgFunctionDefinition* lhs, SgFunctionDefinition* rhs) -> bool
               {
                 return nameOfType(lhs) < nameOfType(rhs); 
               }
             );
  }
  
  int n=0;
  for (auto fc : funCallList) {
    FunctionCallInfo fcInfo=determineFunctionCallInfo(fc);
    if(fcInfo.funCallType!=nullptr) {
      std::string funCallTypeName = fcInfo.funCallType->unparseToString();
      auto pos = std::lower_bound( funDefList.begin(), funDefList.end(),
                                   funCallTypeName, 
                                   [](SgFunctionDefinition* el, const std::string& criteria) -> bool
                                   {
                                     return nameOfType(el) < criteria;
                                   }
                                 );
      
      while (pos != funDefList.end() && (nameOfType(*pos) == funCallTypeName))
      {
        FunctionCallTarget fcTarget(*pos);
        
        if(fcInfo.isFunctionPointerCall()) {
          mapping[fc].insert(fcTarget);n++;
        } else if(fcInfo.funCallName==fcTarget.getFunctionName()) {
          mapping[fc].insert(fcTarget);n++;
        }
        
        ++pos;
      }
      /*
      for (auto fd : funDefList) {
        FunctionCallTarget fcTarget(fd);
        bool matching;
        switch(_matchMode) {
        case 1:
        // this one should be working (but has a bug)
          matching=fcInfo.funCallType==fcTarget.getFunctionType();break;
        case 2:
          // this one should be working (but has a bug)
          matching=fcInfo.mangledFunCallTypeName==fcTarget.getMangledFunctionTypeName();break;
        case 3:
          // this one works as workaround
          matching=fcInfo.funCallType->unparseToString()==fcTarget.getFunctionType()->unparseToString();break;
        default:
          SAWYER_MESG(logger[FATAL])<<"Error: FunctionCallMapping: unknown function matchmode "<<_matchMode<<endl;
          exit(1);
        }
        if(matching) {
          if(fcInfo.isFunctionPointerCall()) {
            mapping[fc].insert(fcTarget);n++;
          } else if(fcInfo.funCallName==fcTarget.getFunctionName()) {
            mapping[fc].insert(fcTarget);n++;
          }
        }
      }
      */
    } else {
      std::vector<SgFunctionDeclaration*> targets;

      CallTargetSet::getPropertiesForExpression(fc, classHierarchy, targets);

      if (targets.empty()) {      
        SAWYER_MESG(logger[WARN]) << "unresolved call " << fc->unparseToString()
                                  << std::endl;
      }

      for (SgFunctionDeclaration* fdcl : targets) {
        if (SgFunctionDeclaration* defdcl = isSgFunctionDeclaration(fdcl->get_definingDeclaration())) {
          mapping[fc].insert(FunctionCallTarget(defdcl->get_definition())); ++n;
        } else {
          SAWYER_MESG(logger[WARN]) << "unable to find definition for " << fdcl->unparseNameToString()
                                    << std::endl;
        }
      }

      // unknown function call
      // do not enter in mapping
    }
  }
  SAWYER_MESG(logger[INFO])<<"Resolved "<<n<<" ("<< percent(n, funCallList.size()) << "%) function calls."<<endl;
  return;
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
  if(funCall) {
    //SAWYER_MESG(logger[TRACE]) << "DEBUG: @FunctionCallMapping::resolveFunctionCall:"<<funCall->unparseToString()<<endl;
    auto iter=mapping.find(funCall);
    if(iter!=mapping.end()) {
      return (*iter).second;
    }
  }
  FunctionCallTargetSet emptySet;
  return emptySet;
}
