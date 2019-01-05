#include <numeric>
#include <set>

#include "NarginTransformer.h"

#include "rose.h"
#include "sageGeneric.h"

#include "utility/utils.h"

// credits to Rachel Adamec (UAB) for providing the initial Nargin transformer

namespace sb = SageBuilder;
namespace ru = RoseUtils;

static inline
const SgInitializedNamePtrList&
functionArgumentList(const SgFunctionParameterList* params)
{
  ROSE_ASSERT(params);

  return params->get_args();
}

static inline
const SgInitializedNamePtrList&
functionArgumentList(const SgFunctionDeclaration* fundecl)
{
  ROSE_ASSERT(fundecl);

  return functionArgumentList(fundecl->get_parameterList());
}

namespace MatlabToCpp
{
  struct NarginAnalyzer : AstSimpleProcessing
  {
    typedef std::map<std::string, SgFunctionDeclaration*> result_container;
    // typedef std::set<SgFunctionDeclaration*> result_container;

    void visit(SgNode* n) ROSE_OVERRIDE;

    result_container& computeUsers(SgProject* proj)
    {
      traverse(proj, preorder);

      return narginusers;
    }

    result_container narginusers;
  };

  struct NarginFinder
  {
    typedef NarginAnalyzer::result_container result_container;

    explicit
    NarginFinder(result_container& narginusers)
    : results(narginusers)
    {}

    void handle(SgNode&) {}

    void handle(SgVarRefExp& n)
    {
      SgName            varnm = ru::nameOf(n);

      if (varnm != "nargin") return;

      SgFunctionDefinition&  fundefn = sg::ancestor<SgFunctionDefinition>(n);
      SgFunctionDeclaration& fundecl = sg::deref(fundefn.get_declaration());

      std::cerr << &fundecl << " -- " << fundefn.unparseToString() << std::endl;
      std::cerr << fundecl.get_name() << "/" << fundecl.get_mangled_name() << std::endl;
      ROSE_ASSERT(&fundecl == fundecl.get_definingDeclaration());

      // results.insert(&fundecl);
      results.insert(std::make_pair(fundecl.get_name(), &fundecl));
    }

    result_container& results;
  };

  void NarginAnalyzer::visit(SgNode* n)
  {
    sg::dispatch(NarginFinder(narginusers), n);
  }

  struct CallListExtender
  {
    typedef NarginAnalyzer::result_container result_container;

    explicit
    CallListExtender(const result_container& nu)
    : narginusers(nu)
    {}

    const SgInitializedNamePtrList&
    expectedArglistSize(SgVarRefExp* n)
    {
      ROSE_ASSERT(n);

      std::string                      funname = ru::nameOf(n);
      result_container::const_iterator pos = narginusers.find(funname);

      ROSE_ASSERT(pos != narginusers.end());
      return functionArgumentList((*pos).second);
    }

    const SgInitializedNamePtrList&
    expectedArglistSize(SgFunctionCallExp* n)
    {
      ROSE_ASSERT(n);

      return expectedArglistSize(isSgVarRefExp(n->get_function()));
    }


    void operator()(SgFunctionCallExp* call)
    {
      // If here, it was a user-defined function call.
      // Pad the argument list with zeroes for any missing argument
      // and calculate the appropriate nargin.
      SgExprListExp* argList = call->get_args();

      // if there is no argument list, we create one :)
      if (!argList)
      {
        argList = sb::buildExprListExp(std::vector<SgExpression*>());

        call->set_args(argList);
      }

      size_t                          nargs     = argList->get_expressions().size();
      const SgInitializedNamePtrList& paramlist = expectedArglistSize(call);
      size_t                          expected  = paramlist.size();

      // add dummy
      for (size_t i = nargs; i < expected; ++i)
      {
        // \todo build dummy from real type
        call->append_arg(sb::buildIntVal(0));
      }

      call->append_arg(sb::buildIntVal(nargs));
    }

    const result_container& narginusers;
  };

  static inline
  void extend_parmlist(SgFunctionDeclaration* decl)
  {
    SgInitializedName* nargin = sb::buildInitializedName("nargin", sb::buildIntType());

    decl->append_arg(nargin);
  }

  struct ParameterListExtender : AstSimpleProcessing
  {
    typedef NarginAnalyzer::result_container    result_container;
    typedef std::vector<SgFunctionCallExp*>     call_container;
    typedef std::vector<SgFunctionDeclaration*> decl_container;

    explicit
    ParameterListExtender(const result_container& funs)
    : narginusers(funs), calls(), decls()
    {}

    void visit(SgNode*) ROSE_OVERRIDE;

    void updateLists(SgProject* proj)
    {
      traverse(proj, preorder);

      for_each(calls.begin(), calls.end(), CallListExtender(narginusers));
      for_each(decls.begin(), decls.end(), extend_parmlist);
    }

    const result_container& narginusers;
    call_container          calls;
    decl_container          decls;
  };


  struct ParamListHandler
  {
    typedef ParameterListExtender::result_container result_container;
    typedef ParameterListExtender::call_container   call_container;
    typedef ParameterListExtender::decl_container   decl_container;

    ParamListHandler(const result_container& res, call_container& calls, decl_container& decls)
    : narginusers(res), fncalls(calls), fndecls(decls)
    {}

    SgFunctionDeclaration* resolveFunName(SgVarRefExp& n)
    {
      result_container::const_iterator pos = narginusers.find(ru::nameOf(n));

      if (pos == narginusers.end()) return NULL;
      return (*pos).second;
    }

    bool isInteresting(SgFunctionDeclaration& n)
    {
      SgFunctionDeclaration* defdecl = isSgFunctionDeclaration(n.get_definingDeclaration());

      return (  (defdecl != NULL)
             // && (narginusers.find(defdecl) != narginusers.end())
             && (narginusers.find(defdecl->get_name()) != narginusers.end())
             );
    }

    void handle(SgNode&) {}

    void handle(SgFunctionRefExp& n)
    {
      // should not be executed before function calls are properly generated
      // afterard case belo can be removed.
      ROSE_ASSERT(false);

      SgFunctionDeclaration* callee = n.getAssociatedFunctionDeclaration();
      ROSE_ASSERT(callee);

      if (!isInteresting(*callee)) return;

      SgFunctionCallExp* call = sg::ancestor<SgFunctionCallExp>(callee);
      ROSE_ASSERT(call == callee->get_parent());

      fncalls.push_back(call);
    }

    void handle(SgVarRefExp& n)
    {
      SgFunctionDeclaration* callee = resolveFunName(n);
      if (!callee) return;

      SgFunctionCallExp* call = isSgFunctionCallExp(n.get_parent());
      if (!call) return;

      fncalls.push_back(call);
    }

    void handle(SgFunctionDeclaration& n)
    {
      if (!isInteresting(n)) return;

      fndecls.push_back(&n);
    }

    const result_container& narginusers;
    call_container&         fncalls;
    decl_container&         fndecls;
  };


  void ParameterListExtender::visit(SgNode* n)
  {
    sg::dispatch(ParamListHandler(narginusers, calls, decls), n);
  }

  void transformNargin(SgProject* project)
  {
    NarginAnalyzer        nt;
    ParameterListExtender ple(nt.computeUsers(project));

    ple.updateLists(project);
  }
} /* namespace MatlabTransformation */
