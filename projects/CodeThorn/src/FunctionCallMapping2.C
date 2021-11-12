#include "sage3basic.h"                                 // every librose .C file must start with this
#include "Rose/Diagnostics.h"
#include "sageGeneric.h"

#include "FunctionIdMapping.h"
#include "RoseAst.h"

#include "CallGraph.h"

#include "CodeThornException.h"
#include "FunctionCallMapping2.h"

#include <sstream>
#include <map>

#include "AstTerm.h"

#include "RoseCompatibility.h"

namespace CodeThorn
{

namespace
{
  enum class Ternary { unknown, trueval, falseval };

  struct IsTemplate : sg::DispatchHandler<Ternary>
  {
    typedef sg::DispatchHandler<Ternary> base;

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

  // cloned and mildly modified from CallGraph.C
  SgExpression*
  getTargetExpression(SgCallExpression* sgFunCallExp)
  {
    SgExpression* res = sgFunCallExp->get_function();

    while (SgCommaOpExp* comma = isSgCommaOpExp(res))
      res = comma->get_rhs_operand();

    ASSERT_not_null(res);
    return res;
  }

  struct CallTargetFinder : sg::DispatchHandler< std::vector<SgFunctionDeclaration*> >
  {
      using base = sg::DispatchHandler< std::vector<SgFunctionDeclaration*> >;

      CallTargetFinder(ClassAnalysis& classAnalysis, VirtualFunctionAnalysis& vfuncs, SgCallExpression& call)
      : base(), cha(classAnalysis), vfa(vfuncs), callexp(call)
      {}

      ReturnType
      getCallTargets(const SgMemberFunctionRefExp& memref);

      void handle(const SgNode& n)       { SG_UNEXPECTED_NODE(n); }

      /// fallback method for regular function calls
      void handle(const SgExpression& n)
      {
        CallTargetSet::getPropertiesForExpression(&callexp, nullptr /*classHierarchy*/, res);

        std::cerr << "PP4: " << n.unparseToString() << " " << res.size()
                  << std::endl;
      }

      /// catches member calls
      void handle(const SgMemberFunctionRefExp& n)
      {
        res = getCallTargets(n);
      }

      /// catches pointer-to-member calls
      // \todo ..

    private:
      ClassAnalysis&           cha; // class hierarchy analysis
      VirtualFunctionAnalysis& vfa;
      SgCallExpression&        callexp;
  };

  namespace
  {
    SgExpression*
    receiverExpression(const SgCallExpression& call)
    {
      SgExprListExp&       args = SG_DEREF(call.get_args());
      SgExpressionPtrList& lst  = args.get_expressions();

      ROSE_ASSERT(lst.size());
      return lst[0];
    }


    bool requiresVirtualDispatch(const SgMemberFunctionRefExp& memref, const SgCallExpression& callexp)
    {
      // virtual dispatch is not required if the function is prefixed with a name qualifier
      if (memref.get_need_qualifier())
        return false;

      // or if the receiver argument is not a polymorphic type
      SgExpression*  receiver = receiverExpression(callexp);
      SgAddressOfOp* adrOf    = isSgAddressOfOp(receiver);

      if (adrOf == nullptr)
        return true;

      SgExpression&  objexpr = SG_DEREF(adrOf->get_operand());
      SgType&        ty      = SG_DEREF(objexpr.get_type());
      SgType*        underTy = ty.stripType(STRIP_MODIFIER_ALIAS);

      // test if the type is polymorphic
      return (  isSgReferenceType(underTy)
             || isSgRvalueReferenceType(underTy)
             // in these cases the first argument would not be an address
             //~ || isSgPointerType(underTy)
             //~ || isSgArrayType(underTy)
             );
    }


    template <class AssociativeContainer>
    auto lookup(AssociativeContainer& m, const typename AssociativeContainer::key_type& key) -> decltype(&m[key])
    {
      auto pos = m.find(key);

      return pos == m.end() ? nullptr : &pos->second;
    }
  }


  CallTargetFinder::ReturnType
  CallTargetFinder::getCallTargets(const SgMemberFunctionRefExp& memref)
  {
    SgMemberFunctionDeclaration*        mdcl = memref.getAssociatedMemberFunctionDeclaration();
    SgMemberFunctionDeclaration*        mkey = mdcl ? isSgMemberFunctionDeclaration(mdcl->get_firstNondefiningDeclaration())
                                                    : mdcl;
    SgMemberFunctionDeclaration&        mfn  = SG_DEREF(mkey);
    SgClassDefinition&                  clsdef = getClassDef(mfn);
    std::vector<SgFunctionDeclaration*> res = { &mfn };

    // return early if this is not a virtual call
    if (!requiresVirtualDispatch(memref, callexp))
      return res;

/*
    for (auto& x : vfa)
      std::cerr << "PP3 i: " << x.first->get_name() << " " << x.first
                << " " << x.first->get_firstNondefiningDeclaration()
                << std::endl;
*/
    const VirtualFunctionDesc* vfunc = lookup(vfa, &mfn);

    // this is not a virtual function
    if (vfunc == nullptr)
      return res;

    // query overriders and add them to the candidate list
    for (const OverrideDesc& desc : vfunc->overriders())
    {
      const SgFunctionDeclaration*       fundcl = desc.functionId();
      const SgMemberFunctionDeclaration& ovrdcl = SG_DEREF(isSgMemberFunctionDeclaration(fundcl));
      const SgClassDefinition&           ovrcls = getClassDef(ovrdcl);

      if (cha.areBaseDerived(&clsdef, &ovrcls))
        res.push_back(const_cast<SgFunctionDeclaration*>(fundcl));
    }

    //~ std::cerr << "PP3: exiting late " << memref.unparseToString() << " " << res.size()
              //~ << std::endl;
    return res;
  }


  // \note SgConstructorInitializer should not be handled through CallGraph..
  std::vector<SgFunctionDeclaration*>
  callTargets(SgCallExpression* callexp, ClassAnalysis* cha, VirtualFunctionAnalysis* vfa)
  {
    ASSERT_not_null(callexp); ASSERT_not_null(cha); ASSERT_not_null(vfa);

    SgExpression* calltgt = getTargetExpression(callexp);

    return sg::dispatch(CallTargetFinder{*cha, *vfa, *callexp}, calltgt);
  }

  std::string typeRep(SgExpression& targetexp)
  {
    ASSERT_not_null(targetexp.get_type());

    SgFunctionType* funty = isSgFunctionType( targetexp.get_type()->findBaseType() );
    ASSERT_not_null(funty);

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
  ASSERT_not_null(dcl);

  SgFunctionDeclaration* defdcl = isSgFunctionDeclaration(dcl->get_definingDeclaration());

  if (!defdcl)
  {
    logWarn() << "unable to find definition for " << dcl->get_name()
              << (dcl->get_definition() ? "*" : "")
              << std::endl;
    return;
  }

  SgFunctionDefinition* def = defdcl->get_definition();
  ASSERT_not_null(def);

  targetset.insert(FunctionCallTarget(def));
}

namespace
{
  inline
  float percent(int part, int whole) { return (part*100.0)/whole; }
}


void FunctionCallMapping2::computeFunctionCallMapping(SgProject* root)
{
  using map_entry_t = std::unordered_map<Label, FunctionCallTargetSet, HashLabel>::mapped_type;

  ASSERT_not_null(_labeler); ASSERT_not_null(root);

  std::multimap<std::string, SgFunctionDeclaration*> funDecls;
  FunctionCallMapping2*                              fm = this;

  // lambda that computes all possible targets for function pointer calls
  auto computeFunctionPointerCalls = [&funDecls,fm](Label lbl, SgExpression& expr, bool& added) -> void
       {
         // function pointer calls are handled separately in order to
         //   use the ROSE AST instead of the memory pool.
         std::string  key = typeRep(expr);
         auto         aa  = funDecls.lower_bound(key);
         decltype(aa) zz  = funDecls.end();
         map_entry_t& map_entry = fm->mapping[lbl];

         while (aa != zz && aa->first == key)
         {
           addEntry(map_entry, aa->second);

           added = true;
           ++aa;
         }
       };

  // lambda that computes all possible targets for pointer-to-member calls
  auto computeMemberFunctionPointerCalls = [](Label, SgExpression&, bool&) -> void
       {
       };

  for(auto node : RoseAst(root)) {
    if(SgFunctionDeclaration* funDecl = isSgFunctionDeclaration(node)) {
      if (!isSgMemberFunctionDeclaration(funDecl))
        funDecls.emplace(funDecl->get_type()->get_mangled().getString(), funDecl);
    }
  }

  int numCalls             = 0;
  int unresolvedFunptrCall = 0; // could correctly remain unresolved
  for (Label lbl : *_labeler)
  {
    if (!_labeler->isFunctionCallLabel(lbl))
      continue;

    SgNode* theNode = _labeler->getNode(lbl);

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
      bool added = false;

      if (isSgMemberFunctionType(targetNode->get_type()))
        computeMemberFunctionPointerCalls(lbl, *targetNode, added);
      else
        computeFunctionPointerCalls(lbl, *targetNode, added);

      if (!added)
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
      using function_container = std::vector<SgFunctionDeclaration*>;

      function_container tgts = callTargets(callexpr, _classAnalysis, _virtualFunctions);
      map_entry_t&       mapEntry = mapping[lbl];

      for (SgFunctionDeclaration* fdcl : tgts)
      {
        addEntry(mapEntry, fdcl);
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
        addEntry(mapping[lbl], ctor);
      else // print the parent, b/c ctorinit may produce an empty string
        logWarn() << "unable to resolve target for initializing: " << ctorinit->get_parent()->unparseToString()
                  << std::endl;
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
  ASSERT_not_null(_labeler);

  std::stringstream ss;
  for(auto fcall : mapping)
  {
    SgLocatedNode* callNode = isSgLocatedNode(_labeler->getNode(fcall.first));
    ASSERT_not_null(callNode);

    ss<<SgNodeHelper::sourceFilenameLineColumnToString(callNode)<<" : "<<callNode->unparseToString()<<" RESOLVED TO ";
    for(auto target : fcall.second) {
      ss<<target.toString()<<" ";
    }
    ss<<std::endl;
  }
  return ss.str();
}

FunctionCallTargetSet FunctionCallMapping2::resolveFunctionCall(Label callLabel)
{
  //SAWYER_MESG(logger[TRACE]) << "DEBUG: @FunctionCallMapping2::resolveFunctionCall:"<<funCall->unparseToString()<<endl;
  auto iter=mapping.find(callLabel);
  if(iter!=mapping.end()) {
    return (*iter).second;
  }

  return FunctionCallTargetSet{};
}

bool insideTemplatedCode(const SgNode* n)
{
  Ternary res = sg::dispatch(IsTemplate{}, n);

  if (res != Ternary::unknown) return res == Ternary::trueval;

  return insideTemplatedCode(n->get_parent());
}
} // namespace CodeThorn
