#include "sage3basic.h"                                 // every librose .C file must start with this
#include "Diagnostics.h"
#include "sageGeneric.h"

#include "FunctionIdMapping.h"
#include "RoseAst.h"

#include "CallGraph.h"

#include "CodeThornException.h"
#include "FunctionCallMapping2.h"

#include <sstream>
#include <map>
#include "AstTerm.h"


namespace CodeThorn
{

namespace 
{
  Sawyer::Message::Facility logger;
  
  auto logWarn() -> decltype(logger[Sawyer::Message::WARN])
  {
    return logger[Sawyer::Message::WARN];
  }
  
  auto logInfo() -> decltype(logger[Sawyer::Message::INFO])
  {
    return logger[Sawyer::Message::INFO];
  }

  // \note SgConstructorInitializer should not be handled through CallGraph..
  std::vector<SgFunctionDeclaration*>
  callTargets(SgCallExpression* callexp, ClassHierarchyWrapper* classHierarchy)
  {
    std::vector<SgFunctionDeclaration*> targets;
  
    ROSE_ASSERT(callexp);
    
    CallTargetSet::getPropertiesForExpression(callexp, classHierarchy, targets);
  
    if (targets.size() == 0)    
      logWarn() << typeid(*callexp).name() << " - " 
                << sg::ancestor<SgStatement>(callexp)->unparseToString() 
                << ": found = " << targets.size() 
                << std::endl;
    //~ ROSE_ASSERT(targets.size());
    
    return std::move(targets);
  }
  
  std::string typeRep(SgPointerDerefExp* funderef)
  {
    ROSE_ASSERT(funderef);
    
    SgFunctionType* funty = isSgFunctionType( funderef->get_type()->findBaseType() );
    ROSE_ASSERT( funty );
    
    return funty->get_mangled().getString();
  }
}

#if WITHOUT_OLD_FUNCTION_CALL_MAPPING_1

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
#endif /* WITHOUT_OLD_FUNCTION_CALL_MAPPING_1 */

void addEntry(FunctionCallTargetSet& targetset, SgFunctionDeclaration* dcl)
{
  ROSE_ASSERT(dcl);
  
  SgFunctionDeclaration* defdcl = isSgFunctionDeclaration(dcl->get_definingDeclaration());
  
  if (!defdcl)
  {
    logWarn() << "unable to find definition for " << dcl->get_name()
              << (dcl->get_definition() ? "*" : "")
              << std::endl;
    return;
  }
  
  SgFunctionDefinition* def = defdcl->get_definition();
  ROSE_ASSERT(def);
  
  targetset.insert(FunctionCallTarget(def));
}

namespace
{
  float percent(int part, int whole) { return (part*100.0)/whole; }
}

void FunctionCallMapping2::computeFunctionCallMapping(SgProject* root)
{
  typedef std::unordered_map<Label, FunctionCallTargetSet, HashLabel>::mapped_type map_entry_t;
  
  ROSE_ASSERT(labeler && root);
  
  initDiagnostics();

  std::multimap<std::string, SgFunctionDeclaration*> funDecls;

  for(auto node : RoseAst(root)) {
    if(SgFunctionDeclaration* funDecl = isSgFunctionDeclaration(node)) {
      funDecls.emplace(funDecl->get_type()->get_mangled().getString(), funDecl);
    }
  }

  int numCalls = 0;
  for (Label lbl : *labeler)
  {
    if (!labeler->isFunctionCallLabel(lbl))
      continue;

    ++numCalls;
    SgNodeHelper::ExtendedCallInfo callinfo = SgNodeHelper::matchExtendedNormalizedCall(labeler->getNode(lbl));

    if (SgPointerDerefExp* callNode = callinfo.functionPointer())
    {
      // function pointer calls are handled separately in order to  
      //   use the ROSE AST instead of the memory pool.
      std::string  key = typeRep(callNode);
      auto         aa = funDecls.lower_bound(key);
      auto         zz = funDecls.end();
      map_entry_t& map_entry = mapping[lbl];  
      
      while (aa != zz && aa->first == key)
      {
        addEntry(map_entry, aa->second);

        ++aa;
      }
    }
    else if (SgCallExpression* callexpr = callinfo.callExpression())
    {
      // handles explicit function calls (incl. virtual functions)
      std::vector<SgFunctionDeclaration*> tgts(callTargets(callexpr, classHierarchy));
      
      if (tgts.size() == 0)
      {
        logWarn() << "unable to resolve target for " << callexpr->unparseToString() << std::endl;
      }
      
      for (SgFunctionDeclaration* fdcl : tgts)
      {
        addEntry(mapping[lbl], fdcl);
      }
    }
    else if (SgConstructorInitializer* ctorinit = callinfo.ctorInitializer())
    {
      // handles constructor calls
      if (SgFunctionDeclaration* ctor = ctorinit->get_declaration())
      {
        addEntry(mapping[lbl], ctor);
      }
      else
      {
        logWarn() << "unable to resolve target for " << ctorinit->unparseToString()
                  << std::endl;
      }
    }
    else
    {
      // \todo handle implicit (ctor) calls
    }
  }

  logInfo()<<"Resolved "<<mapping.size()<<" ("<< percent(mapping.size(), numCalls) << "%) function calls."<<std::endl;
}

std::string FunctionCallMapping2::toString()
{
  ROSE_ASSERT(labeler);

  std::stringstream ss;
  for(auto fcall : mapping)
  {
    SgLocatedNode* callNode = isSgLocatedNode(labeler->getNode(fcall.first));
    ROSE_ASSERT(callNode);

    ss<<SgNodeHelper::sourceFilenameLineColumnToString(callNode)<<" : "<<callNode->unparseToString()<<" RESOLVED TO ";
    for(auto target : fcall.second) {
      ss<<target.toString()<<" ";
    }
    ss<<std::endl;
  }
  return ss.str();
}

void FunctionCallMapping2::initDiagnostics() {
  static bool initialized = false;
  
  if (!initialized) {
    initialized = true;
    logger = Sawyer::Message::Facility("CodeThorn::FunctionCallMapping2", Rose::Diagnostics::destination);
    Rose::Diagnostics::mfacilities.insertAndAdjust(logger);
  }
}

FunctionCallTargetSet FunctionCallMapping2::resolveFunctionCall(Label callLabel)
{
  //SAWYER_MESG(logger[TRACE]) << "DEBUG: @FunctionCallMapping2::resolveFunctionCall:"<<funCall->unparseToString()<<endl;
  auto iter=mapping.find(callLabel);
  if(iter!=mapping.end()) {
    return (*iter).second;
  }

  return FunctionCallTargetSet();
}

} // namespace CodeThorn
