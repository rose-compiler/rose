#include <iostream>

#include "rose.h"
#include "sageBuilder.h"
#include "sageGeneric.h"

namespace sb = SageBuilder;
namespace si = SageInterface;

#include "f2cxx_analyzer.hpp"
#include "f2cxx_convenience.hpp"

namespace f2cxx
{
  //

  std::set<SgInitializedName*>&
  AnalyzerAttribute::associates(ParamAttr attr)
  {
    std::set<SgInitializedName*>* res = NULL;

    switch (attr)
    {
      case f2cxx::data:
        res = &datablocks;
        break;

      case f2cxx::space_lower:
        res = &data_lower;
        break;

      case f2cxx::space_upper:
        res = &data_upper;
        break;

      case f2cxx::loop_lower:
        res = &iter_lower;
        break;

      case f2cxx::loop_upper:
        res = &iter_upper;
        break;

      default:
        sg::report_error("missing case label");
    }

    return sg::deref(res);
  }

  AnalyzerAttribute&
  AnalyzerAttribute::role(ParamAttr attr)
  {
    reinterpret_cast<int&>(val) |= attr;
    return *this;
  }

  bool
  AnalyzerAttribute::hasRole(ParamAttr attr) const
  {
    return (val & attr) == attr;
  }

  AnalyzerAttribute&
  AnalyzerAttribute::associate(ParamAttr attr, SgInitializedName* dcl)
  {
    ROSE_ASSERT(dcl != NULL);

    associates(attr).insert(dcl);
    return *this;
  }

  AnalyzerAttribute&
  AnalyzerAttribute::access_dim(size_t dim, SgInitializedName* idx)
  {
    // artificial limit to 4 dimensions
    sg::report_error_if(dim >= 4, "unexpected number of dimensions");

    std::set<SgInitializedName*>& idxset = indexvars[dim];

    idxset.insert(idx);
    return *this;
  }

  void AnalyzerAttribute::clear_array_access()
  {
    indexvars.clear();
  }

  std::pair<size_t, SgInitializedName*>
  AnalyzerAttribute::array_access() const
  {
    sg::report_error_if(indexvars.size() == 0, "induction variable w/o array access");

    if (indexvars.size() > 1)
      std::cerr << "induction variable w/ multiple different array accesses" << std::endl;

    const std::pair<const size_t, std::set<SgInitializedName*>>& data = *indexvars.begin();

    return std::make_pair(data.first, *data.second.begin());
  }

  //
  // convenience functions for a more uniform access to sage nodes

  template <class SageNode>
  struct NoFailTypeRecoveryHandler : sg::DispatchHandler<std::pair<bool, SageNode*> >
  {
    typedef sg::DispatchHandler<std::pair<bool, SageNode*> > base;

    NoFailTypeRecoveryHandler()
    : base()
    {}

    void handle(SgNode& n)   {}
    void handle(SageNode& n) { this->res = std::make_pair(true, &n); }
  };


  template <class SageNode>
  bool match_node_type(SgNode* n, SageNode*& matched_node)
  {
    typedef typename NoFailTypeRecoveryHandler<SageNode>::ReturnType result_type;

    result_type res = sg::dispatch(NoFailTypeRecoveryHandler<SageNode>(), n);

    if (res.first)
    {
      if (matched_node == NULL)
        matched_node = res.second;
      else if (matched_node != res.second)
        res.first = false;
    }

    ROSE_ASSERT(!res.first || res.second);
    return res.first;
  }

  template <class SageNode>
  SageNode* recover_sage_type(SgNode* n)
  {
    return sg::dispatch(NoFailTypeRecoveryHandler<SageNode>(), n).second;
  }

  template <class SageDecl>
  bool _match_decl(SageDecl& decl, SageDecl*& patvar)
  {
    if (patvar) return patvar == &decl;

    patvar = &decl;
    return true;
  }

  template <class SafeRef, class SageDecl>
  bool match_decl(SafeRef& n, SageDecl*& patvar)
  {
    return _match_decl(get_decl(sg::deref(n)), patvar);
  }

  template <class SageValueNode, class Val>
  bool has_value_exp(SgExpression* n, Val v)
  {
    SageValueNode* sageval = NULL;

    return (  match_node_type<SageValueNode>(n, sageval)
           && sageval->get_value() == v
           );
  }


  //
  //

  bool check_bound(SgExpression* n, SgInitializedName*& boundvar, SgIntVal*& idx)
  {
    SgPntrArrRefExp* arridx  = NULL;
    SgExprListExp*   indices = NULL;
    SgVarRefExp*     varref  = NULL;

    return (  match_node_type(n, arridx)
           && match_node_type(arridx->get_lhs_operand(), varref)
           && match_decl(varref, boundvar)
           && match_node_type(arridx->get_rhs_operand(), indices)
           && arg_count(indices) == 1
           && match_node_type(arg_at(indices, 0), idx)
           );
  }

  struct DimPattern
  {
    DimPattern()
    : match(true), last_index(0), lb(NULL), ub(NULL)
    {}

    void handle(SgNode&) {}

    void handle(SgSubscriptExpression& n)
    {
      SgIntVal* lbidx = NULL;
      SgIntVal* ubidx = NULL;

      match = (  match
              && has_value_exp<SgIntVal>(n.get_stride(), 1)
              && check_bound(n.get_lowerBound(), lb, lbidx)
              && check_bound(n.get_upperBound(), ub, ubidx)
              && lbidx->get_value() == ubidx->get_value()
              && last_index + 1     == ubidx->get_value()
              );

      ++last_index;
    }

    bool               match;
    int                last_index;
    SgInitializedName* lb;
    SgInitializedName* ub;
  };

  struct TypeAttrHandler
  {
    explicit
    TypeAttrHandler(SgInitializedName& p)
    : parm(p)
    {}

    void handle(const SgNode& n) { SG_UNEXPECTED_NODE(n); }

    void handle(const SgType& n) {}

    void handle(const SgArrayType& n)
    {
      SgExprListExp* indices = n.get_dim_info();
      int            num_dim = n.get_rank();

      if (num_dim != 3) return;

      DimPattern result = sg::traverseChildren(DimPattern(), indices);

      if (!result.match) return;

      getAttribute(sg::deref(result.lb))
        .role(f2cxx::space_lower)
        .associate(f2cxx::data, &parm);

      getAttribute(sg::deref(result.ub))
        .role(f2cxx::space_upper)
        .associate(f2cxx::data, &parm);

      getAttribute(parm)
        .role(f2cxx::data)
        .associate(f2cxx::space_upper, result.ub)
        .associate(f2cxx::space_lower, result.lb);
    }

    SgInitializedName& parm;
  };

  // currently this takes a simplified view that all array indices are linear;
  // thus arr[i*y, h] is not properly handled.
  struct ArrayAccessAnalyzer
  {
    ArrayAccessAnalyzer(SgInitializedName& varparam, size_t index)
    : var(&varparam), idx(index)
    {}

    void handle(SgNode& n)          { SG_UNEXPECTED_NODE(n); }
    void handle(SgExpression& n)    { sg::traverseChildren(*this, n); }

    // nested array access currently unsupported
    void handle(SgPntrArrRefExp& n) { SG_UNEXPECTED_NODE(n); }

    void handle(SgVarRefExp& n)
    {
      SgInitializedName& vardcl = get_decl(n);
      AnalyzerAttribute& attr   = getAttribute(vardcl);

      if (!attr.hasRole(f2cxx::induction))
        return;

      attr.access_dim(idx, var);
    }

    SgInitializedName* var;
    size_t             idx;
  };

  /** extracts the index array and dimension of the loop bound expression. */
  struct LoopBoundExpr
  {
    LoopBoundExpr()
    : limit(NULL), val(NULL)
    {}

    void handle(SgNode& n)          { SG_UNEXPECTED_NODE(n); }

    // for now we are overly restrictive
    void handle(SgExpression& n)    { SG_UNEXPECTED_NODE(n); }

    void handle(SgPntrArrRefExp& n)
    {
      bool structural_match = check_bound(&n, limit, val);

      sg::report_error_if(!structural_match, "unexpected loop bound expression");
    }

    SgInitializedName& var() const { return sg::deref(limit); }

    SgInitializedName* limit;
    SgIntVal*          val;
  };


  struct ProcAnalyzer
  {
    void descend(SgNode& n)
    {
      sg::traverseChildren(*this, n);
    }

    void handle(SgNode& n)       { SG_UNEXPECTED_NODE(n); }
    void handle(SgStatement& n)  { descend(n); }
    void handle(SgExpression& n) { descend(n); }

    void handle(SgInitializedName& n)
    {
      descend(n);

      sg::dispatch(TypeAttrHandler(n), n.get_type());
    }

    void handle(SgFunctionParameterList& n)
    {
      // descend ?
      SgInitializedNamePtrList& args = n.get_args();

      for (size_t i = 0; i < args.size(); ++i)
      {
        SgInitializedName& child = sg::deref(args.at(i));

        getAttribute(child).role(f2cxx::param);
      }
    }

    void handle(SgPntrArrRefExp& n)
    {
      SgVarRefExp&         arr = *SG_ASSERT_TYPE(SgVarRefExp,   n.get_lhs_operand());
      SgInitializedName&   prm = get_decl(arr);

      if (!getAttribute(prm).hasRole(f2cxx::dataparam))
      {
        descend(n);
        return;
      }

      SgExprListExp&       idx = *SG_ASSERT_TYPE(SgExprListExp, n.get_rhs_operand());
      SgExpressionPtrList& lst = idx.get_expressions();

      for (size_t i = 0; i < lst.size(); ++i)
        sg::dispatch(ArrayAccessAnalyzer(get_decl(arr), i), lst.at(i));
    }


    void handle(SgFortranDo& n)
    {
      SgExpression* init  = n.get_initialization();
      SgAssignOp*   asgn  = SG_ASSERT_TYPE(SgAssignOp, init);
      SgVarRefExp*  loop  = SG_ASSERT_TYPE(SgVarRefExp, asgn->get_lhs_operand());
      LoopBoundExpr start = sg::dispatch(LoopBoundExpr(), asgn->get_rhs_operand());
      LoopBoundExpr limit = sg::dispatch(LoopBoundExpr(), n.get_bound());

      if (  !getAttribute(start.var()).hasRole(f2cxx::param)
         || !getAttribute(limit.var()).hasRole(f2cxx::param)
         )
      {
        std::cerr << "found non-translatable do-loop." << std::endl;
        return;
      }

      AnalyzerAttribute& loopattr = getAttribute(n);
      AnalyzerAttribute& indcattr = getAttribute(get_decl(*loop));

      loopattr.role(f2cxx::amrex_loop);

      getAttribute(start.var()).role(f2cxx::loop_lower);
      getAttribute(limit.var()).role(f2cxx::loop_upper);

      indcattr.role(f2cxx::induction)
              .associate(f2cxx::loop_lower, &start.var())
              .associate(f2cxx::loop_upper, &limit.var());

      // descend rest
      descend(sg::deref(n.get_increment()));
      descend(sg::deref(n.get_body()));

      // mv analysis result from induction variable to loop
      std::pair<size_t, SgInitializedName*> acc = indcattr.array_access();

      indcattr.clear_array_access();
      loopattr.access_dim(acc.first, acc.second);
    }
  };

  static inline
  std::string attributeKey() { return "F2CXXATTRKEY"; }

  AnalyzerAttribute& getAttribute(SgNode& n)
  {
    void* curr = n.getAttribute(attributeKey());

    if (curr != NULL)
      return *static_cast<AnalyzerAttribute*>(curr);

    AnalyzerAttribute* attr = new AnalyzerAttribute();

    n.setAttribute(attributeKey(), attr);
    return *attr;
  }

  /// \brief
  ///   Analyzes the procedure parameters and its uses in order to
  ///   determine whether they need to be translated to special Amrex
  ///   types.
  /// \details
  ///    Supported type migration:
  ///    - any three dimensional array in Fortran will be translated
  ///      to an amrex::Box
  void Analyzer::operator()(SgProcedureHeaderStatement* n)
  {
    ProcAnalyzer analyzer;

    analyzer.handle(sg::deref(n));
  }

  void print_tags(ParamAttr attr)
  {
    if (attr & f2cxx::param)       std::cerr << "p ";
    if (attr & f2cxx::data)        std::cerr << "d3 ";
    if (attr & f2cxx::space_lower) std::cerr << "[ ";
    if (attr & f2cxx::space_upper) std::cerr << "] ";
    if (attr & f2cxx::loop_lower)  std::cerr << "< ";
    if (attr & f2cxx::loop_upper)  std::cerr << "> ";
    if (attr & f2cxx::normal_expr) std::cerr << "* ";
  }

  void print_tags(AnalyzerAttribute& n)
  {
    print_tags(n.flags());
  }

  void print_tags0(SgInitializedName* n)
  {
    std::cerr << sg::deref(n).get_name() << ": ";
    print_tags(getAttribute(sg::deref(n)));
    std::cerr << std::endl;
  }

  void print_param_tags(SgProcedureHeaderStatement* proc)
  {
    SgFunctionParameterList&  params = sg::deref(proc->get_parameterList());
    SgInitializedNamePtrList& lst    = params.get_args();

    std::for_each(lst.begin(), lst.end(), print_tags0);
  }

  /// \brief
  ///   translates fortran parameter type into C++ Amrex Type
  //~ SgType& type_translator::translate(SgType& t)
  //~ {
    //~ return t;
  //~ }

  /// \brief
  ///   translates fortran expressions into C++ expressions
  ///   under consideration of the type analysis
  //~ SgExpression& type_translator::translate(SgExpression& e)
  //~ {
    //~ return e;
  //~ }
}
