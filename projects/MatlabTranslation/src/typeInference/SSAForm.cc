#include <set>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream> // TODO: delete later

#include "SSAForm.h"

#include "rose.h"
#include "sageGeneric.h"

//~ #include "utility/utils.h"
//~ #include "FastNumericsRoseSupport.h"
//~ #include "TypeAttribute.h"

namespace si = SageInterface;
namespace sb = SageBuilder;
namespace sg = SageGeneric;

struct SSAContext
{
  typedef std::vector<std::pair<SgExpression*, int> > node_updates;
  typedef std::map<SgName, size_t>                    symbol_state;

    SSAContext()
    : lvalue_candidate(false), updates(), symbols(NULL)
    {}

    static
    SSAContext create()
    {
      return SSAContext(node_updates(), new symbol_state);
    }

    static
    void destroy(SSAContext& ctx)
    {
      delete ctx.symbols;
    }

    void setLValue(bool lval) { lvalue = lval; }
    bool isLValue() const     { return lvalue; }

  private:
    SSAContext(node_updates nodes, symbol_state* symbols)
    : updates(), symbols(symbols)
    {}

    bool                     lvalue;
    node_updates             updates;
    symbol_state*            symbols;
};

struct SSAResult
{
  std::set<SgName> updated_symbols;
};


struct SSAConverter : AstTopDownBottomUpProcessing<SSAContext, SSAResult>
{
  typedef AstTopDownBottomUpProcessing<InheritedAttr, SynthesizedAttr> base;

  typedef SSAContext                      inherited_type;
  typedef SSAResult                       synthesized_type;
  typedef base::SynthesizedAttributesList synthesized_attributes;

  inherited_type evaluateInheritedAttribute(SgNode* n, inherited_type inh) ROSE_OVERRIDE;
  synthesized_type evaluateSynthesizedAttribute(SgNode* n, inherited_type inh, synthesized_attributes syn) ROSE_OVERRIDE;
};

struct LValueCandidateSetter : sg::DispatchHandler<SSAContext>
{
    LValueCandidateSetter() = delete;

    LValueCandidateSetter(SSAContext ssactx, SgExpression& node)
    : base(ssactx), expr(n)
    {}

    void handle(SgNode&)
    {
      ROSE_ASSERT(false);
    }

    void handle(SgAssignOp& n)
    {
      // set lvalue, if this expr is on the left hand side of an assignment
      //   expr = 1872;
      res.setLValueCand(n.get_lhs_operand() == &expr);
    }

    void handle(SgArrayRefExp& n)
    {
      // clear lvalue, if this expr is on the right hand side of an array index
      //   e.g., arr[expr] = 1234;
      if (!res.isLValueCand() || (n.get_rhs_operand() != &expr)) return;

      res.setLValueCand(false);
    }

    // operator SSAContext() { return res; }
  private:
    SgExpression& expr;
};

struct SSAInheritedEval : sg::DispatchHandler<SSAContext>
{
    typedef sg::DispatchHandler<SSAContext> base;

    SSAInheritedEval() = delete;

    explicit
    SSAInheritedEval(SSAContext ssactx)
    : base(ssactx), ssaContext(ssactx)
    {}

    void handle(SgNode&)
    {
      ROSE_ASSERT(false);
    }

    void setLValueInfo(SgExpression& e)
    {
      res = sg::dispatch( LValueCandidateSetter(e, res),
                          sg::deref(e.get_parent()
                        );
    }

    void handle(SgStatement&)
    {}

    void handle(SgExpression& e)
    {
      setLValueInfo(e);
    }

    void handle(SgVarRefExp& n)
    {
      setLValueInfo(e);
    }

    void handle(SgFunctionDefinition&)
    {
      // context is created here and will be destroyed
      //   by the bottom up evaluator.
      res = SSAContext::create();
    }

  private:
    SSAContext ssactx;
};

struct SSASynthesizedEval : sg::DispatchHandler<SSAResult>
{
    typedef sg::DispatchHandler<SSAContext> base;

    SSASynthesizedEval() = delete;

    SSASynthesizedEval(SSAContext ssactx, SSAConverter::synthesized_attributes lst)
    : base(), ctx(ssactx), attr(lst)
    {}

    void handle(SgNode&)
    {
      ROSE_ASSERT(false);
    }

    void handle(SgStatement&)
    {}

    void handle(SgExpression& n)
    {
    }

    void handle(SgVarRefExp& n)
    {
      if (ctx.isLValue())
      {
        updateLValueName(n, ctx);
      }
      else
      {
        updateRValueName(n, ctx);
      }
    }

    void handle(SgFunctionDefinition&)
    {
      SSAContext::destroy(ctx);
    }

  private:
    SSAContext                           ctx;
    SSAConverter::synthesized_attributes attr;
};


SSAConverter::inherited_type
evaluateInheritedAttribute(SgNode* n, inherited_type inh)
{
  return sg::dispatch(SSAInheritedEval(inh), sg::deref(n));
}

SSAConverter::synthesized_type
evaluateSynthesizedAttribute(SgNode* n, inherited_type inh, synthesized_attributes syn)
{
  return sg::dispatch(SSASynthesizedEval(inh, syn), sg::deref(n));
}


/// converts a function to SSA form
void convertToSSA  (SgFunctionDefinition* def)
{

}


void convertToSSA(SgProject* proj)
{

}

/// converts a function from SSA to normal form
void convertFromSSA(SgFunctionDefinition* def) {}
void convertFromSSA(SgProject* proj) {}

