/* unparseAda_expressions.C
 *
 *
 */
#include "sage3basic.h"
#include "unparser.h"
//~ #include "Utf8.h"
#include "sageGeneric.h"

//~ using namespace std;

//~ #define OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES 0
//~ #define OUTPUT_HIDDEN_LIST_DATA 0
//~ #define OUTPUT_DEBUGGING_INFORMATION 0

//~ #ifdef _MSC_VER
//~ #include "Cxx_Grammar.h"
//~ #endif

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
//~ #include "rose_config.h"

namespace
{
  SgVariableSymbol& symOf(const SgVarRefExp& n)
  {
    return SG_DEREF(n.get_symbol());
  }

  SgFunctionSymbol& symOf(const SgFunctionRefExp& n)
  {
    return SG_DEREF(n.get_symbol());
  }

  SgName nameOf(const SgSymbol& sy)
  {
    return sy.get_name();
  }

  SgName nameOf(const SgVarRefExp& n)
  {
    return nameOf(symOf(n));
  }

  SgName nameOf(const SgFunctionRefExp& n)
  {
    return nameOf(symOf(n));
  }

  SgInitializedName& declOf(const SgVarRefExp& n)
  {
    SgVariableSymbol& sy = symOf(n);

    return SG_DEREF(sy.get_declaration());
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

  struct AdaExprUnparser
  {
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

    void handle(SgPntrArrRefExp& n)
    {
      SgExpression* lhs    = n.get_lhs_operand();
      SgExpression* rhs    = n.get_rhs_operand();

      expr(lhs);
      prn("(");
      expr(rhs);
      prn(")");
    }

    void handle(SgDotExp& n)
    {
      SgExpression* lhs    = n.get_lhs_operand();
      SgExpression* rhs    = n.get_rhs_operand();

      expr(lhs);
      prn(".");
      expr(rhs, false /* no need to scope qual right hand side */);
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

    void handle(SgCallExpression& n)
    {
      SgExprListExp& args = SG_DEREF(n.get_args());

      expr(n.get_function());
      arglst_opt(args);
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
      const bool typequal = n.get_cast_type() == SgCastExp::e_ada_type_qualification;

      // only type qualifications have expression lists as arguments
      //~ ROSE_ASSERT(typequal ^ (!isSgExprListExp(n.get_operand())));

      type(n.get_type());
      prn(typequal ? "'" : "(");
      expr(n.get_operand());
      if (!typequal) prn(")");
    }

    void handle(SgTypeExpression& n)
    {
      type(n.get_type());
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
      if (ctxRequiresScopeQualification)
      {
        SgInitializedName& init = declOf(n);

        prn(scopeQual(init.get_scope()));
      }

      prn(nameOf(n));
    }

    void handle(SgAdaRenamingRefExp& n)
    {
      SgAdaRenamingDecl& dcl = SG_DEREF(n.get_decl());

      prn(scopeQual(dcl.get_scope()));
      prn(dcl.get_name());
    }

    void handle(SgAggregateInitializer& n)
    {
      prn("(");
      exprlst(SG_DEREF(n.get_initializers()));
      prn(")");
    }

    void handle(SgDesignatedInitializer& n)
    {
      exprlst(SG_DEREF(n.get_designatorList()), "| ");
      prn(" => ");
      expr(n.get_memberInit());
    }

    void handle(SgAssignInitializer& n)
    {
      expr(n.get_operand());
    }

    void handle(SgConstructorInitializer& n)
    {
      //~ prn("(");
      exprlst(SG_DEREF(n.get_args()));
      //~ prn(")");
    }

    void handle(SgNullExpression& n)
    {
      // \todo should not be reached
      prn("<null>");
    }

    void handle(SgFunctionRefExp& n)
    {
      SgFunctionDeclaration& fundcl = SG_DEREF(n.getAssociatedFunctionDeclaration());

      if (ctxRequiresScopeQualification)
        prn(scopeQual(fundcl.get_scope()));

      prn(nameOf(n));
    }

    void handle(SgAdaTaskRefExp& n)
    {
      SgAdaTaskSpecDecl& tskdcl = SG_DEREF(n.get_decl());

      prn(scopeQual(tskdcl.get_scope()));
      prn(tskdcl.get_name());
    }

    void handle(SgNewExp& n)
    {
      SgConstructorInitializer* init = n.get_constructor_args();

      prn("new");
      type(n.get_specified_type());

      if (init) { prn("'"); expr(init); }
    }

    void expr(SgExpression* exp, bool requiresScopeQual = true);
    void exprlst(SgExprListExp& exp, std::string sep = ", ");
    void arglst_opt(SgExprListExp& args);

    void operator()(SgExpression* exp)
    {
      expr(exp);
    }

    void type(SgType* t)
    {
      unparser.unparseType(t, info);
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
    //~ std::cerr << "XXXXXXX " << typeid(n).name() << std::endl;
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

    SgExpression* lhs    = n.get_lhs_operand();
    SgExpression* rhs    = n.get_rhs_operand();
    const bool    prefix = (  argRequiresCallSyntax(lhs)
                           || argRequiresCallSyntax(rhs)
                           );

    if (prefix)
    {
      prn("\"");
      prn(operator_sym(n));
      prn("\" (");
    }

    expr(lhs);
    prn(" ");
    prn(prefix ? std::string(", ") : operator_sym(n));
    prn(" ");
    expr(rhs);

    if (prefix) prn(")");
  }

  void AdaExprUnparser::handle(SgUnaryOp& n)
  {
    const bool isprefix = true; // \todo

    if (isprefix) { prn(operator_sym(n)); prn(" "); }
    expr(n.get_operand());
    if (!isprefix) prn(operator_sym(n));
  }

  void AdaExprUnparser::exprlst(SgExprListExp& exp, std::string sep)
  {
    SgExpressionPtrList& lst = exp.get_expressions();

    if (lst.empty()) return;

    expr(lst[0]);

    for (size_t i = 1; i < lst.size(); ++i)
    {
      prn(sep);
      expr(lst[i]);
    }
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

void Unparse_Ada::unparseExpression(SgExpression* expr, SgUnparse_Info& info)
{
  const bool withScopeQual = info.get_current_scope() != NULL;

  sg::dispatch(AdaExprUnparser{*this, info, std::cerr, withScopeQual}, expr);
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
