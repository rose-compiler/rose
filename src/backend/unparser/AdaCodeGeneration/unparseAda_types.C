/* Unparse_Ada.C
 * This C file contains the general function to unparse types as well as
 * functions to unparse every kind of type.
 */

#include "sage3basic.h"
#include "unparser.h" //charles4:  I replaced this include:   #include "unparseX10.h"

#include "sageGeneric.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
//~ #include "rose_config.h"


//~ void replaceString (std::string& str, const std::string& from, const std::string& to);

namespace
{
  struct AdaTypeUnparser
  {
    AdaTypeUnparser(Unparse_Ada& unp, SgScopeStatement* where, SgUnparse_Info& inf, std::ostream& outp)
    : unparser(unp), ctx(where), info(inf), os(outp)
    {}

    void prn(const std::string& s)
    {
      unparser.curprint(s);
      //~ os << s;
    }

    std::string scopeQual(SgScopeStatement& local, SgDeclarationStatement& remote);

    std::string scopeQual(SgScopeStatement& local, SgDeclarationStatement* remote)
    {
      return scopeQual(local, SG_DEREF(remote));
    }

    void handle(SgNode& n)    { SG_UNEXPECTED_NODE(n); }

    //
    // Ada constraints

    void handle(SgAdaRangeConstraint& n)
    {
      prn(" range ");
      expr(n.get_range());
    }

    void handle(SgAdaIndexConstraint& n)
    {
      prn(" (");
      rangeList(n.get_indexRanges());
      prn(")");
    }

    //
    // Fundamental types

    void handle(SgTypeBool&)   { prn(" Boolean"); }
    void handle(SgTypeInt&)    { prn(" Integer"); }
    void handle(SgTypeChar&)   { prn(" Character"); }
    void handle(SgTypeFloat&)  { prn(" Float"); }
    void handle(SgTypeString&) { prn(" String"); }
    void handle(SgTypeVoid&)   { prn(" -- void\n"); }

    //
    // Ada types

    void handle(SgAdaSubtype& n)
    {
      type(n.get_base_type());
      support_opt(n.get_constraint());
    }

    void handle(SgAdaModularType& n)
    {
      prn("mod ");
      expr(n.get_modexpr());
    }

    void handle(SgModifierType& n)
    {
      if (n.get_typeModifier().get_constVolatileModifier().isConst())
        prn("constant ");

      type(n.get_base_type());
    }

    void handle(SgTypeDefault& n)
    {
      prn(" private");
    }

    void handle(SgAutoType& n)
    {
      /* print nothing - used for Integer and Real Number constants */
    }

    void setCtxIfNeeded(SgDeclarationStatement& dcl)
    {
      if (ctx) return;

      ctx = &sg::ancestor<SgScopeStatement>(dcl);
    }


    void handle(SgNamedType& n)
    {
      setCtxIfNeeded(SG_DEREF(n.get_declaration()));

      prn(" ");
      prn(scopeQual(SG_DEREF(ctx), n.get_declaration()));
      prn(n.get_name());
    }

    void handle(SgDeclType& n)
    {
      prn(" ");
      expr(n.get_base_expression());
    }

    void handle(SgTypeTuple& n)
    {
      SgTypePtrList& lst = n.get_types();

      for (size_t i = 0; i < lst.size()-1; ++i)
      {
        type(lst[i]);
        prn(" |");
      }

      type(lst.back());
    }

    void handle(SgArrayType& n)
    {
      prn(" array");
      prn("(");
      arrayDimList(SG_DEREF(n.get_dim_info()), (n.get_is_variable_length_array() ? " range <>" : ""));
      prn(")");
      prn(" of");
      type(n.get_base_type());
    }

    void handle(SgAdaTaskType& n)
    {
      // \todo fix in AST and override get_name and get_declaration in AdaTaskType
      prn(" ");
      prn(scopeQual(SG_DEREF(ctx), n.get_decl()));
      // prn(scopeQual(SG_DEREF(ctx), n.get_declaration()));
      prn(SG_DEREF(n.get_decl()).get_name());
    }

    void handle(SgAdaFloatType& n)
    {
      prn(" is ");
      expr(n.get_digits());

      support_opt(n.get_constraint());
    }

    void type(SgType* ty)
    {
      sg::dispatch(*this, ty);
    }

    void support_opt(SgNode* n)
    {
      if (n) sg::dispatch(*this, n);
    }

    void expr(SgExpression* e)
    {
      unparser.unparseExpression(e, info);
    }

    void arrayDimList(SgExprListExp& dims, const std::string& constraintSuffix)
    {
      SgExpressionPtrList& lst = dims.get_expressions();
      if (lst.empty()) return;

      expr(lst[0]);
      prn(constraintSuffix);

      for (size_t i = 1; i < lst.size(); ++i)
      {
        prn(", ");
        expr(lst[i]);
        prn(constraintSuffix);
      }
    }

    void rangeList(SgRangeExpPtrList& lst)
    {
      if (lst.empty()) return;

      expr(lst[0]);

      for (size_t i = 1; i < lst.size(); ++i)
      {
        prn(", ");
        expr(lst[i]);
      }
    }

    Unparse_Ada&      unparser;
    SgScopeStatement* ctx;
    SgUnparse_Info&   info;
    std::ostream&     os;
  };

  std::string
  AdaTypeUnparser::scopeQual(SgScopeStatement& local, SgDeclarationStatement& remote)
  {
    return unparser.computeScopeQual(local, SG_DEREF(remote.get_scope()));
  }
}


//-----------------------------------------------------------------------------------
//  void Unparse_Ada::unparseType
//
//  General function that gets called when unparsing an Ada type. Then it routes
//  to the appropriate function to unparse each Ada type.
//-----------------------------------------------------------------------------------
void
Unparse_Ada::unparseType(SgType* type, SgScopeStatement* where, SgUnparse_Info& info)
{
  ASSERT_not_null(type);

  sg::dispatch(AdaTypeUnparser(*this, where, info, std::cerr), type);
}

