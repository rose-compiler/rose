#include "ArmaOptimizer.h"

#include "rose.h"
#include "sageInterface.h"
#include "sageBuilder.h"
#include "sageGeneric.h"

#include "utility/utils.h"

namespace si = SageInterface;
namespace sb = SageBuilder;
namespace ru = RoseUtils;

namespace ArmaOpt
{
  typedef std::pair<SgExpression*, SgExpression*> ExprPair;

  static
  ExprPair makeMemberCall(SgFunctionCallExp& parent, std::string armaname)
  {
    SgExpression*        receiver = ru::deepCopy(&ru::argN(parent, 0));
    SgFunctionCallExp*   memcall  =
             ru::createMemberFunctionCall( "Matrix",  // \todo replace with type
                                           receiver,
                                           armaname,
                                           ru::buildEmptyParams(),
                                           &sg::ancestor<SgScopeStatement>(parent)
                                         );

    return ExprPair(&parent, memcall);
  }

  template <class Builder>
  static
  ExprPair makeBinaryExpr(SgFunctionCallExp& parent, Builder builder)
  {
    SgExpression* lhs = ru::deepCopy(&ru::argN(parent, 0));
    SgExpression* rhs = ru::deepCopy(&ru::argN(parent, 1));

    return ExprPair(&parent, builder(lhs, rhs));
  }

  struct OptimizerBase : sg::DispatchHandler<ExprPair>
  {
    OptimizerBase()
    : sg::DispatchHandler<ExprPair>(ReturnType(NULL, NULL))
    {}
  };

  struct InnerCalleeHandler : OptimizerBase
  {
    InnerCalleeHandler(SgFunctionCallExp& grandpa, SgFunctionCallExp& pa)
    : OptimizerBase(), grandparent(grandpa), parent(pa)
    {}

    void handle(SgNode& n) {}

    void handle(SgVarRefExp& n)
    {
      if (ru::nameOf(n) == "diag")
      {
        ROSE_ASSERT(ru::arglist(parent).size() == 1);

        SgScopeStatement* scope   = &sg::ancestor<SgScopeStatement>(n);
        SgExpression*     arg     = ru::deepCopy(&ru::argN(parent, 0));
        SgExprListExp*    args    = sb::buildExprListExp(arg);
        SgExpression*     newcall = ru::createFunctionCall("diagmat", scope, args);

        res = ReturnType(&grandparent, newcall);
      }
    }

    SgFunctionCallExp& grandparent;
    SgFunctionCallExp& parent;
  };

  struct InnerCallHandler : OptimizerBase
  {
    explicit
    InnerCallHandler(SgFunctionCallExp& pa)
    : OptimizerBase(), parent(pa)
    {}

    void handle(SgNode&) {}

    void handle(SgFunctionCallExp& n)
    {
      res = sg::dispatch(InnerCalleeHandler(parent, n), n.get_function());
    }

    SgFunctionCallExp& parent;
  };

  struct CalleeHandler : OptimizerBase
  {
    explicit
    CalleeHandler(SgFunctionCallExp& pa)
    : OptimizerBase(), parent(pa)
    {}

    void handle(SgNode&) {}

    void handle(SgVarRefExp& n)
    {
      std::string varname = ru::nameOf(n);

      if (varname == "transpose")
        res = makeMemberCall(parent, "t");

      else if (varname == "times")
        res = makeBinaryExpr(parent, sb::buildModOp);

      else if (varname == "diag")
      {
        res = sg::dispatch(InnerCallHandler(parent), &ru::argN(parent, 0));
      }
    }

    SgFunctionCallExp& parent;
  };


  struct CallHandler : OptimizerBase
  {
    void handle(SgNode& n) {}

    void handle(SgFunctionCallExp& n)
    {
      res = sg::dispatch(CalleeHandler(n), n.get_function());
    }
  };

  struct ArmaExpressionOptimizer : AstSimpleProcessing
  {
    void visit(SgNode*) ROSE_OVERRIDE;
  };

  static
  void optimizeForArmadilloCode(SgNode* n)
  {
    typedef OptimizerBase::ReturnType ReturnType;

    ReturnType res = sg::dispatch(CallHandler(), n);

    if (res.first == NULL) return;

    ROSE_ASSERT(res.second && res.first == n);
    si::replaceExpression(res.first, res.second);

    optimizeForArmadilloCode(res.first);
  }

  void ArmaExpressionOptimizer::visit(SgNode* n)
  {
    optimizeForArmadilloCode(n);
  }

  void optimize(SgProject* proj)
  {
    ArmaExpressionOptimizer opt;

    opt.traverse(proj, postorder);
  }
}
