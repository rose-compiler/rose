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
  
  struct Ternary
  {
    enum value { unknown, trueval, falseval };
  };
  
  struct IsTemplate : sg::DispatchHandler<Ternary::value>
  {
    typedef sg::DispatchHandler<Ternary::value> base;
    
    IsTemplate()
    : base(Ternary::unknown)
    {}
    
    void nope() { res = Ternary::falseval; }
    void yes()  { res = Ternary::trueval; }
    
    void handle(const SgNode&)                              { /* unknown */}
                                                            
    void handle(const SgGlobal&)                            { nope(); }
    void handle(const SgFile&)                              { nope(); }
    void handle(const SgProject&)                           { nope(); }
    
    void handle(const SgTemplateClassDeclaration&)          { yes(); }
    void handle(const SgTemplateClassDefinition&)           { yes(); }
    void handle(const SgTemplateFunctionDeclaration&)       { yes(); }
    void handle(const SgTemplateFunctionDefinition&)        { yes(); }
    void handle(const SgTemplateMemberFunctionDeclaration&) { yes(); }
    void handle(const SgTemplateVariableDeclaration&)       { yes(); }
    
    // \note this is a using declaration, thus could be outside template
    void handle(const SgTemplateTypedefDeclaration& n)        
    { 
      if (n.get_templateParameters().size())
        yes();
      else
        nope();
    } 
  };  

  // \note SgConstructorInitializer should not be handled through CallGraph..
  std::vector<SgFunctionDeclaration*>
  callTargets(SgCallExpression* callexp, ClassHierarchyWrapper* classHierarchy)
  {
    std::vector<SgFunctionDeclaration*> targets;
  
    ROSE_ASSERT(callexp);
    
    CallTargetSet::getPropertiesForExpression(callexp, classHierarchy, targets);
  
    //~ if (targets.size() == 0)    
      //~ logWarn() << typeid(*callexp).name() << " - " 
                //~ << sg::ancestor<SgStatement>(callexp)->unparseToString() 
                //~ << ": found = " << targets.size() 
                //~ << std::endl;
    
    // no result indicates an error, which is logged in the caller
    //~ ROSE_ASSERT(targets.size());
    return std::move(targets);
  }
  
  std::string typeRep(SgExpression& targetexp)
  {
    ROSE_ASSERT(targetexp.get_type());

    SgFunctionType* funty = isSgFunctionType( targetexp.get_type()->findBaseType() );
    ROSE_ASSERT(funty);
    
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
  inline
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

  int numCalls             = 0;
  int unresolvedFunptrCall = 0; // could correctly remain unresolved
  for (Label lbl : *labeler)
  {
    if (!labeler->isFunctionCallLabel(lbl))
      continue;
      
    SgNode* theNode = labeler->getNode(lbl);
    
    //~ Goal: ROSE_ASSERT(!insideTemplatedCode(theNode)); // should not be reachable

    // filtering for templated code is not strictly necessary
    //   but it avoids misleading logging output 
    //   and diluted resolution numbers.
    if (insideTemplatedCode(theNode))
    {
      // \todo shall we mark the entry as templated?
      continue;
    }

    ++numCalls;
    SgNodeHelper::ExtendedCallInfo callinfo = SgNodeHelper::matchExtendedNormalizedCall(theNode);

    if (SgExpression* targetNode = callinfo.functionPointer())
    {
      // function pointer calls are handled separately in order to  
      //   use the ROSE AST instead of the memory pool.
      std::string  key = typeRep(*targetNode);
      auto         aa = funDecls.lower_bound(key);
      decltype(aa) zz = funDecls.end();
      bool         added = false;
      map_entry_t& map_entry = mapping[lbl];  
      
      while (aa != zz && aa->first == key)
      {
        addEntry(map_entry, aa->second);

        added = true; 
        ++aa;
      }
      
      if (added == 0)
      {
        mapping.erase(lbl);
        ++unresolvedFunptrCall;
        logWarn() << "unresolved pointer call (possibly no function with that type exists): " 
                  << callinfo.callExpression()->unparseToString()
                  << std::endl;
      }
    }
    else if (SgCallExpression* callexpr = callinfo.callExpression())
    {
      // handles explicit function calls (incl. virtual functions)
      std::vector<SgFunctionDeclaration*> tgts(callTargets(callexpr, classHierarchy));
      map_entry_t&                        map_entry = mapping[lbl];
      
      for (SgFunctionDeclaration* fdcl : tgts)
      {
        addEntry(map_entry, fdcl);
      }
      
      if (tgts.size() == 0)
      {
        mapping.erase(lbl);
        logWarn() << "unable to resolve target for calling: " << callexpr->unparseToString() 
                  << std::endl;
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
        // print the parent, b/c ctorinit may produce an empty string
        logWarn() << "unable to resolve target for initializing: " << ctorinit->get_parent()->unparseToString()
                  << std::endl;
      }
    }
    else
    {
      // \todo handle implicit (ctor) calls
    }
  }
  
  const int resolved = unresolvedFunptrCall + mapping.size();

  logInfo() << "Resolved " << mapping.size() << " (" << percent(resolved, numCalls) << "%) function calls."
            << std::endl;
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

bool insideTemplatedCode(const SgNode* n)
{
  Ternary::value res = sg::dispatch(IsTemplate(), n);
  
  if (res != Ternary::unknown) return res == Ternary::trueval;
  
  return insideTemplatedCode(n->get_parent()); 
} 
} // namespace CodeThorn
