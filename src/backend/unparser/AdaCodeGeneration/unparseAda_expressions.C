/*
 * unparseAda_expressions.C
 */

#include "sage3basic.h"
#include "unparser.h"
#include "sageGeneric.h"
#include "sageInterfaceAda.h"

namespace si = SageInterface;

//~ using namespace std;

namespace
{
  constexpr bool USE_COMPUTED_NAME_QUALIFICATION_EXPR = true;

  inline
  SgVariableSymbol& symOf(const SgVarRefExp& n)
  {
    return SG_DEREF(n.get_symbol());
  }

  inline
  SgFunctionSymbol& symOf(const SgFunctionRefExp& n)
  {
    return SG_DEREF(n.get_symbol());
  }

  inline
  SgInitializedName& declOf(const SgVarRefExp& n)
  {
    SgVariableSymbol& sy = symOf(n);

    return SG_DEREF(sy.get_declaration());
  }

  inline
  SgAdaPackageSpecDecl& declOf(const SgAdaUnitRefExp& n)
  {
    return SG_DEREF(isSgAdaPackageSpecDecl(n.get_decl()));
  }

  inline
  SgName nameOf(const SgSymbol& sy)
  {
    return sy.get_name();
  }

  inline
  SgName nameOf(const SgVarRefExp& n)
  {
    return nameOf(symOf(n));
  }

  inline
  SgName nameOf(const SgEnumVal& n)
  {
    return n.get_name();
  }

  inline
  SgName nameOf(const SgFunctionRefExp& n)
  {
    return nameOf(symOf(n));
  }

  inline
  SgName nameOf(const SgAdaUnitRefExp& n)
  {
    return declOf(n).get_name();
  }

  const SgExprListExp* callArguments(const SgFunctionRefExp& n)
  {
    if (const SgCallExpression* callexp = isSgCallExpression(n.get_parent()))
      return callexp->get_args();

    return nullptr;
  }

  std::string
  operator_sym(SgNode& n)
  {
    typedef std::map<int, std::string> operator_symbols_map;

/*
    static const operator_symbols_map operator_symbols;
                = {
                    { V_SgAssignOp,         ":=" },
                    { V_SgOrOp,             "or else" },
                    { V_SgAndOp,            "and then" },
                    { V_SgBitAndOp,         "and" },
                    { V_SgBitOrOp,          "or" },
                    { V_SgBitXorOp,         "xor" },
                    { V_SgEqualityOp,       "=" },
                    { V_SgNotEqualOp,       "!=" },
                    { V_SgLessThanOp,       "<" },
                    { V_SgLessOrEqualOp,    "<=" },
                    { V_SgGreaterThanOp,    ">" },
                    { V_SgGreaterOrEqualOp, ">=" },
                    { V_SgAddOp,            "+" },
                    { V_SgSubtractOp,       "-" },
                    { V_SgConcatenationOp,  "&" },
                    { V_SgUnaryAddOp,       "+" },
                    { V_SgMinusOp,          "-" },
                    { V_SgMultiplyOp,       "*" },
                    { V_SgDivideOp,         "/" },
                    { V_SgModOp,            "mod" },
                    //~ { V_SgRemOp,            "rem" },
                    { V_SgExponentiationOp, "**" },
                    //~ { V_SgAbsOp,            "abs" },
                    { V_SgNotOp,            "not" },
                    // SgCommaOpExp is not really in Ada, but separates discrete choices in case-when.
                    { V_SgCommaOpExp,       "|" }
                  };
*/
    static operator_symbols_map operator_symbols;

    if (operator_symbols.size() == 0)
    {
      operator_symbols[V_SgAssignOp] =         ":=";
      operator_symbols[V_SgOrOp] =             "or else";
      operator_symbols[V_SgAndOp] =            "and then";
      operator_symbols[V_SgBitAndOp] =         "and";
      operator_symbols[V_SgBitOrOp] =          "or";
      operator_symbols[V_SgBitXorOp] =         "xor";
      operator_symbols[V_SgEqualityOp] =       "=";
      operator_symbols[V_SgNotEqualOp] =       "/=";
      operator_symbols[V_SgLessThanOp] =       "<";
      operator_symbols[V_SgLessOrEqualOp] =    "<=";
      operator_symbols[V_SgGreaterThanOp] =    ">";
      operator_symbols[V_SgGreaterOrEqualOp] = ">=";
      operator_symbols[V_SgAddOp] =            "+";
      operator_symbols[V_SgSubtractOp] =       "-";
      operator_symbols[V_SgConcatenationOp] =  "&";
      operator_symbols[V_SgUnaryAddOp] =       "+";
      operator_symbols[V_SgMinusOp] =          "-";
      operator_symbols[V_SgMultiplyOp] =       "*";
      operator_symbols[V_SgDivideOp] =         "/";
      operator_symbols[V_SgModOp] =            "mod";
      operator_symbols[V_SgExponentiationOp] = "**";
      operator_symbols[V_SgNotOp] =            "not";
      operator_symbols[V_SgAbsOp] =            "abs";
      operator_symbols[V_SgRemOp] =            "rem";

      // not really in Ada
      operator_symbols[V_SgCommaOpExp] =       "|";    // clause separator
      operator_symbols[V_SgPlusPlusOp] =       "loop"; // loop direction indicator
      operator_symbols[V_SgMinusMinusOp] =     "reverse loop"; // loop direction indicator

      // not an operator in Ada
      operator_symbols[V_SgMembershipOp] =     "in";
      operator_symbols[V_SgNonMembershipOp] =  "not in";
    }

    operator_symbols_map::const_iterator pos = operator_symbols.find(n.variantT());

    if (pos == operator_symbols.end())
    {
      //~ std::cerr << "unknown operator: " << typeid(n).name() << std::endl;
      return typeid(n).name();
    }

    return pos->second;
  }

  std::string nameOfUnitRef(const SgDeclarationStatement* n);

  struct NameOfUnitRef : sg::DispatchHandler<std::string>
  {
    void handle(const SgNode& n)                   { SG_UNEXPECTED_NODE(n); }
    void handle(const SgAdaPackageSpecDecl& n)     { res = n.get_name(); }
    void handle(const SgAdaGenericInstanceDecl& n) { res = n.get_name(); }
    void handle(const SgFunctionDeclaration& n)    { res = n.get_name(); }

    void handle(const SgAdaGenericDecl& n)         { res = nameOfUnitRef(n.get_declaration()); }
  };

  std::string nameOfUnitRef(const SgDeclarationStatement* n)
  {
    return sg::dispatch(NameOfUnitRef{}, n);
  };

  struct AdaExprUnparser
  {
    static constexpr bool SUPPRESS_SCOPE_QUAL = false; // no scope qual on selectors

    AdaExprUnparser(Unparse_Ada& unp, SgUnparse_Info& inf, std::ostream& outp, bool requiresScopeQual)
    : unparser(unp), info(inf), os(outp), ctxRequiresScopeQualification(requiresScopeQual)
    {}

    std::string
    scopeQual(SgScopeStatement& remote);

    std::string
    scopeQual(SgScopeStatement* remote)
    {
      return scopeQual(SG_DEREF(remote));
    }

    void prn(const std::string& s)
    {
      unparser.curprint(s);
      //~ os << s;
    }

    void prnNameQual(const SgNode& n)
    {
      using NodeQualMap = std::map<SgNode*, std::string>;
      using Iterator = NodeQualMap::const_iterator;

      const NodeQualMap& nameQualMap = unparser.nameQualificationMap();
      const Iterator     pos = nameQualMap.find(const_cast<SgNode*>(&n));

      if (pos != nameQualMap.end())
        prn(pos->second);
    }

    void handle(SgNode& n)      { SG_UNEXPECTED_NODE(n); }

    void handle(SgExpression& n);

    void handle(SgBinaryOp& n);
    void handle(SgUnaryOp& n);

    /*
    void handle(SgIntVal& n)
    {
      prn(n.get_valueString());
    }
    */

    void handle(SgAdaOthersExp&)
    {
      prn("others");
    }

    void handle(SgNullptrValExp&)
    {
      prn("null");
    }

    void handle(SgPntrArrRefExp& n)
    {
      SgExpression*  lhs = n.get_lhs_operand();
      SgExprListExp* rhs = isSgExprListExp(n.get_rhs_operand());

      expr(lhs);
      prn("(");
      exprlst(SG_DEREF(rhs));
      prn(")");
    }

    void handle(SgDotExp& n)
    {
      SgExpression* lhs    = n.get_lhs_operand();
      SgExpression* rhs    = n.get_rhs_operand();

      expr(lhs);
      prn(".");
      expr(rhs, SUPPRESS_SCOPE_QUAL /* no need to scope qual right hand side */);
    }

    void handle(SgCommaOpExp& n)
    {
      SgExpression* lhs    = n.get_lhs_operand();
      SgExpression* rhs    = n.get_rhs_operand();

      expr(lhs);
      prn(" | ");
      expr(rhs);
    }

    void handle(SgPointerDerefExp& n)
    {
      SgExpression* target = n.get_operand();
      expr(target);
      prn(".all");
    }

    void handle(SgRangeExp& n)
    {
      expr(n.get_start());
      prn(" .. ");
      expr(n.get_end());
    }

    void handle(SgExprListExp& n)
    {
      exprlst(n);
    }

    void handle(SgFunctionCallExp& n)
    {
      SgExprListExp&         args = SG_DEREF(n.get_args());
      SgFunctionDeclaration* fndcl = n.getAssociatedFunctionDeclaration();

      if ((fndcl == nullptr) || (n.get_uses_operator_syntax() == false))
      {
        expr(n.get_function());
        arglst_opt(args);
        return;
      }

      SgExpressionPtrList& lst = args.get_expressions();
      ROSE_ASSERT((lst.size() > 0) && (lst.size() < 3));

      std::string op = si::ada::convertRoseOperatorNameToAdaOperator(fndcl->get_name());
      ROSE_ASSERT(op.size());

      if (lst.size() == 2)
      {
        expr(lst.front());
        prn(" ");
      }

      prn(op);
      // add a space for binary and named unary (op.size > 1) operators
      if ((lst.size() == 2) || (op.size() > 1)) prn(" ");
      expr(lst.back());
    }

    void prnIfBranch(const si::ada::IfExpressionInfo& branch, const std::string& cond)
    {
      prn(cond);
      expr(branch.condition());
      prn(" then ");
      expr(branch.trueBranch());
    }

    void handle(SgConditionalExp& n)
    {
      using Iterator = std::vector<si::ada::IfExpressionInfo>::iterator;

      std::vector<si::ada::IfExpressionInfo> seq = si::ada::flattenIfExpressions(n);
      Iterator                               aa = seq.begin();
      const Iterator                         zz = seq.end();

      ROSE_ASSERT(aa != zz);
      prnIfBranch(*aa, " if ");

      ++aa;
      ROSE_ASSERT(aa != zz);
      while (!aa->isElse())
      {
        prnIfBranch(*aa, " elsif ");
        ++aa;
        ROSE_ASSERT(aa != zz);
      }

      prn(" else ");
      expr(aa->trueBranch());

      ++aa;
      ROSE_ASSERT(aa == zz);
    }

    // unparse expression attributes
    void handle(SgAdaAttributeExp& n)
    {
      expr(n.get_object());
      prn("'");
      prn(n.get_attribute());

      arglst_opt(SG_DEREF(n.get_args()));
    }

    void handle(SgCastExp& n)
    {
      const bool    qualexpr = n.get_cast_type() == SgCastExp::e_ada_type_qualification;
      SgExpression& operand  = SG_DEREF(n.get_operand());
      const bool    hasparen = operand.get_need_paren() || isSgAggregateInitializer(&operand);

      type(n, n.get_type());
      if (qualexpr) prn("'");

      // requires paren even if the expr has not set them
      if (!hasparen) prn("(");
      expr(n.get_operand());
      if (!hasparen) prn(")");
    }

    void handle(SgTypeExpression& n)
    {
      type(n, n.get_type());
    }

    void handle(SgStringVal& n)
    {
      std::stringstream buf;

      buf << n.get_stringDelimiter();

      for (char c : n.get_value())
      {
        buf << c;

        if (c == n.get_stringDelimiter())
          buf << c;
      }

      buf << n.get_stringDelimiter();

      prn(buf.str());
    }

    void handle(const SgVoidVal&)
    {
      prn("<>");
    }

    void handle(SgThrowOp& n)
    {
      prn("raise ");
      expr(n.get_operand());
    }

    void handle(SgActualArgumentExpression& n)
    {
      prn(n.get_argument_name());
      prn(" => ");
      expr(n.get_expression());
    }

    void handle(SgVarRefExp& n)
    {
      if (USE_COMPUTED_NAME_QUALIFICATION_EXPR)
        prnNameQual(n);
      else if (ctxRequiresScopeQualification)
      {
        SgInitializedName& init = declOf(n);

        prn(scopeQual(init.get_scope()));
      }

      prn(nameOf(n));
    }

    void handle(SgEnumVal& n)
    {
      if (USE_COMPUTED_NAME_QUALIFICATION_EXPR)
        prnNameQual(n);
      else if (ctxRequiresScopeQualification)
        prn(scopeQual(SG_DEREF(n.get_declaration()).get_scope()));

      prn(nameOf(n));
    }


    void handle(SgAdaRenamingRefExp& n)
    {
      SgAdaRenamingDecl& dcl = SG_DEREF(n.get_decl());

      if (USE_COMPUTED_NAME_QUALIFICATION_EXPR)
        prnNameQual(n);
      else
        prn(scopeQual(dcl.get_scope()));

      prn(dcl.get_name());
    }

    void handle(SgAggregateInitializer& n)
    {
      prn("(");
      aggregate(SG_DEREF(n.get_initializers()));
      prn(")");
    }

    void handle(SgDesignatedInitializer& n)
    {
      // suppress scope qual on selectors
      exprlst(SG_DEREF(n.get_designatorList()), "| ", SUPPRESS_SCOPE_QUAL);
      prn(" => ");
      expr(n.get_memberInit());
    }

    void handle(SgAssignInitializer& n)
    {
      expr(n.get_operand());
    }

    void handle(SgAdaAncestorInitializer& n)
    {
      expr(n.get_operand());
    }

    void handle(SgConstructorInitializer& n)
    {
      ROSE_ASSERT(n.get_need_paren());
      // n has get_need_paren set and thus they are printed by expr(...)

      //~ prn("(");
      aggregate(SG_DEREF(n.get_args()));
      //~ prn(")");
    }

    void handle(SgNullExpression& n)
    {
      // \todo should not be reached
      prn("<null>");
    }

    // Ada's derived types "inherit" the primitive functions of their base type.
    // Asis does not create new declaration for these functions, but instead links
    // to the original operation functions. However, the scope qualification needs to
    // be generated as if the functions were located in the scope of the derived type.
    // \returns the assumed scope of a function declaration if scope qualification is needed
    //          nullptr if no scope qualification is required
    SgScopeStatement*
    assumedDeclarativeScope(const SgFunctionRefExp& n)
    {
      if (!ctxRequiresScopeQualification)
        return nullptr;

      const SgExprListExp*   args = callArguments(n);
      if (!args)
        return nullptr;

      SgFunctionDeclaration& fundcl = SG_DEREF(n.getAssociatedFunctionDeclaration());
      auto                   primitiveArgs = si::ada::primitiveParameterPositions(fundcl);
      SgScopeStatement*      overridingScope = si::ada::overridingScope(args, primitiveArgs);

      return overridingScope ? overridingScope : fundcl.get_scope();
    }

    void handle(SgFunctionRefExp& n)
    {
      if (USE_COMPUTED_NAME_QUALIFICATION_EXPR)
        prnNameQual(n);
      else if (SgScopeStatement* dclscope = assumedDeclarativeScope(n))
        prn(scopeQual(dclscope));

      std::string fn = si::ada::convertRoseOperatorNameToAdaName(nameOf(n));

      prn(std::move(fn));
    }

    template <class SageAdaRefExp>
    void handleConcurrentObjectRef(SageAdaRefExp& n)
    {
      auto& dcl = SG_DEREF(n.get_decl());

      if (USE_COMPUTED_NAME_QUALIFICATION_EXPR)
        prnNameQual(n);
      else
        prn(scopeQual(dcl.get_scope()));

      prn(dcl.get_name());
    }

    void handle(SgAdaTaskRefExp& n) { handleConcurrentObjectRef(n); }

    void handle(SgAdaProtectedRefExp& n) { handleConcurrentObjectRef(n); }

    void handle(SgAdaUnitRefExp& n)
    {
      if (USE_COMPUTED_NAME_QUALIFICATION_EXPR)
        prnNameQual(n);
      else
        prn(scopeQual(SG_DEREF(n.get_decl()).get_scope()));

      // really needed?
      prn(nameOfUnitRef(n.get_decl()));
    }

    void handle(SgNewExp& n)
    {
      SgConstructorInitializer* init = n.get_constructor_args();

      prn("new");
      type(n, n.get_specified_type());

      if (init) { prn("'"); expr(init); }
    }

    // handled here, b/c language independent support converts large values
    //   into __builtin functions.
    void handle(SgLongDoubleVal& n)
    {
      std::string val = n.get_valueString();

      if (val.size() == 0)
        val = boost::lexical_cast<std::string>(n.get_value());

      prn(val);
    }

    void exprlst( SgExpressionPtrList::const_iterator aa,
                  SgExpressionPtrList::const_iterator zz,
                  std::string sep = ", ",
                  bool reqScopeQual = true
                );

    void expr(SgExpression* exp, bool requiresScopeQual = true);
    void exprlst(SgExprListExp& exp, std::string sep = ", ", bool requiresScopeQual = true);
    void aggregate(SgExprListExp& exp);
    void arglst_opt(SgExprListExp& args);

    void operator()(SgExpression* exp)
    {
      expr(exp);
    }

    void type(const SgExpression& ref, SgType* t)
    {
      unparser.unparseType(ref, t, info);
    }

    Unparse_Ada&    unparser;
    SgUnparse_Info& info;
    std::ostream&   os;
    bool            ctxRequiresScopeQualification;
  };

  bool argRequiresCallSyntax(SgExpression* n)
  {
    return isSgActualArgumentExpression(n);
  }

  void AdaExprUnparser::handle(SgExpression& n)
  {
    // if not handled here, have the language independent parser handle it..
    unparser.UnparseLanguageIndependentConstructs::unparseExpression(&n, info);
  }

  void AdaExprUnparser::expr(SgExpression* exp, bool requiresScopeQual)
  {
    // let the generic unparser handle its things..
    //~ unparser.unparseExpression(exp, info);

    // or just handle everything
    const bool withParens = exp->get_need_paren();

    if (withParens) prn("(");
    sg::dispatch(AdaExprUnparser{unparser, info, os, requiresScopeQual}, exp);
    if (withParens) prn(")");
  }

  void AdaExprUnparser::handle(SgBinaryOp& n)
  {
    // print either lhs binop rhs
    //           or "binop" (lhs, rhs)

    SgExpression* lhs        = n.get_lhs_operand();
    SgExpression* rhs        = n.get_rhs_operand();
    const bool    opref      = (lhs == nullptr) && (rhs == nullptr);
    const bool    callsyntax = (  argRequiresCallSyntax(lhs)
                               || argRequiresCallSyntax(rhs)
                               );

    if (opref || callsyntax)
    {
      prn("\"");
      prn(operator_sym(n));
      prn("\"");

      if (callsyntax) prn("(");
    }

    if (!opref)
    {
      expr(lhs);
      prn(" ");
      prn(callsyntax ? std::string(", ") : operator_sym(n));
      prn(" ");
      expr(rhs);
    }

    if (callsyntax) prn(")");
  }

  void AdaExprUnparser::handle(SgUnaryOp& n)
  {
    SgExpression* oper       = n.get_operand();
    const bool    callsyntax = argRequiresCallSyntax(oper);

    // are there any postfix operators in Ada

    if (callsyntax) prn("\"");
    prn(operator_sym(n));
    prn(callsyntax ? "\" (" : " ");
    expr(n.get_operand());
    if (callsyntax) prn(")");
  }

  void AdaExprUnparser::exprlst(SgExprListExp& exp, std::string sep, bool reqScopeQual)
  {
    SgExpressionPtrList& lst = exp.get_expressions();

    exprlst(lst.begin(), lst.end(), sep, reqScopeQual);
  }

  void AdaExprUnparser::exprlst( SgExpressionPtrList::const_iterator aa,
                                 SgExpressionPtrList::const_iterator zz,
                                 std::string sep,
                                 bool reqScopeQual
                               )
  {
    if (aa == zz) return;

    expr(*aa, reqScopeQual);

    while (++aa != zz)
    {
      prn(sep);
      expr(*aa, reqScopeQual);
    }
  }

  void AdaExprUnparser::aggregate(SgExprListExp& n)
  {
    si::ada::AggregateInfo info = si::ada::splitAggregate(n);

    if (SgAdaAncestorInitializer* ext = info.ancestor())
    {
      expr(ext->get_ancestor());
      prn(" with ");

      if (info.nullRecord())
        prn("null record");
    }

    exprlst(info.begin(), info.end());
  }


  void AdaExprUnparser::arglst_opt(SgExprListExp& args)
  {
    if (args.get_expressions().empty()) return;

    prn("(");
    exprlst(args);
    prn(")");
  }

  std::string
  AdaExprUnparser::scopeQual(SgScopeStatement& remote)
  {
    SgScopeStatement* current = info.get_current_scope();

    return current ? unparser.computeScopeQual(*current, remote)
                   : "<missing-scope>"; // <-- this used iff invoked from unparseToString..
  }

}

bool Unparse_Ada::requiresParentheses(SgExpression* expr, SgUnparse_Info& info)
{
  ASSERT_not_null(expr);

  SgStatement* stmt = sg::ancestor<SgStatement>(expr);

  // inside a type, etc. ?
  if (!stmt) return false;

  if (!isTransformed(stmt))
    return expr->get_need_paren();

  return base::requiresParentheses(expr, info);
}


void Unparse_Ada::unparseLanguageSpecificExpression(SgExpression* expr, SgUnparse_Info& info)
{
  ASSERT_not_null(expr);

  SG_UNEXPECTED_NODE(*expr);
}

void Unparse_Ada::unparseExpression(SgExpression* n, SgUnparse_Info& info)
{
  const bool    withScopeQual = info.get_current_scope() != nullptr;
  SgNode* const currentReferenceNode = info.get_reference_node_for_qualification();

  // set the reference node, unless the unparser is already in type mode
  if (&nameQualificationMap() == &SgNode::get_globalQualifiedNameMapForNames())
    info.set_reference_node_for_qualification(n);

  AdaExprUnparser exprUnparser{*this, info, std::cerr, false /* scope qual, will be passed to expr(...) */};

  exprUnparser.expr(n, withScopeQual);

  // restore reference node
  info.set_reference_node_for_qualification(currentReferenceNode);
}

void Unparse_Ada::unparseExprListExp(SgExprListExp* n, SgUnparse_Info& info, std::string sep)
{
  const bool withScopeQual = info.get_current_scope() != nullptr;

  AdaExprUnparser exprUnparser{*this, info, std::cerr, false /* scope qual, will be passed to expr(...) */};

  exprUnparser.exprlst(SG_DEREF(n), sep, withScopeQual);
}


void Unparse_Ada::unparseStringVal(SgExpression* expr, SgUnparse_Info& info)
{
  sg::dispatch(AdaExprUnparser{*this, info, std::cerr, false /* scope qual */}, expr);
}

void Unparse_Ada::setInitialScope(SgUnparse_Info& info, SgExpression* n)
{
  SgScopeStatement* scope = sg::ancestor<SgScopeStatement>(n);

  info.set_current_scope(scope);
}
