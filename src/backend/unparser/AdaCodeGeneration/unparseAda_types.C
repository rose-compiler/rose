/* Unparse_Ada.C
 * This C file contains the general function to unparse types as well as
 * functions to unparse every kind of type.
 */

#include "sage3basic.h"
#include "unparser.h" //charles4:  I replaced this include:   #include "unparseX10.h"

#include "sageGeneric.h"
#include "sageInterfaceAda.h"

namespace si = SageInterface;

namespace
{
  struct AdaTypeUnparser
  {
    AdaTypeUnparser(Unparse_Ada& unp, SgUnparse_Info& inf, std::ostream& outp)
    : unparser(unp), info(inf), os(outp)
    {}

    void prn(const std::string& s)
    {
      unparser.curprint(s);
      //~ os << s;
    }

    std::string scopeQual(SgDeclarationStatement& remote);

    std::string scopeQual(SgDeclarationStatement* remote)
    {
      return scopeQual(SG_DEREF(remote));
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

    void handle(SgTypeBool&)       { prn(" Boolean"); }
    void handle(SgTypeChar&)       { prn(" Character"); }
    void handle(SgTypeInt&)        { prn(" Integer"); }
    void handle(SgTypeFloat&)      { prn(" Float"); }
    void handle(SgTypeDouble&)     { prn(" Long_Float"); }
    void handle(SgTypeLongDouble&) { prn(" Long_Long_Float"); }
    void handle(SgTypeString&)     { prn(" String"); }
    void handle(SgTypeLong&)       { prn(" Long_Integer"); }
    void handle(SgTypeLongLong&)   { prn(" Long_Long_Integer"); }
    void handle(SgTypeVoid&)       { prn(" -- void\n"); }  // error, should not be in Ada

    //
    // Ada types

    void handle(SgAdaSubtype& n)
    {
      type(n.get_base_type());
      support_opt(n.get_constraint());
    }

    void handle(SgAdaDerivedType& n)
    {
      type(n.get_base_type());
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
      /* print nothing - used for forward declarations of unknown type */
    }

    void handle(SgAutoType& n)
    {
      /* print nothing - used for Integer and Real Number constants */
    }


    void handle(SgNamedType& n)
    {
      prn(" ");
      prn(scopeQual(n.get_declaration()));
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
      arrayDimList(SG_DEREF(n.get_dim_info()), (si::ada::unconstrained(n) ? " range <>" : ""));
      prn(")");
      prn(" of");
      type(n.get_base_type());
    }

    void handle(SgAdaTaskType& n)
    {
      // \todo fix in AST and override get_name and get_declaration in AdaTaskType
      prn(" ");
      prn(scopeQual(n.get_decl()));
      prn(SG_DEREF(n.get_decl()).get_name());
    }

    void handle(SgAdaFloatType& n)
    {
      prn("digits ");
      expr(n.get_digits());

      support_opt(n.get_constraint());
    }

    void handle(SgAdaAccessType& n)
    {
      prn("access");
      if (n.get_is_object_type()) {
        if (n.get_is_general_access()) {
          prn(" all");
        }

        if (n.get_is_constant()) {
          prn(" constant");
        }

        if (n.get_base_type() != NULL) {
          type(n.get_base_type());
        }

      } else {
        // subprogram access type
        if (n.get_is_protected()) {
          prn(" protected");
        }

        if (n.get_return_type() != NULL) {
          prn(" function");
        } else {
          prn(" procedure");
        }

        // TODO: print parameter profile here if it is specified.
        //       parameter profiles are not currently implemented for
        //       AdaAccessType nodes.

        if (n.get_return_type() != NULL) {
          prn(" return");
          type(n.get_return_type());
        }
      }

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

    void rangeList(SgExpressionPtrList& lst) // \todo remove
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
    SgUnparse_Info&   info;
    std::ostream&     os;
  };

  std::string
  AdaTypeUnparser::scopeQual(SgDeclarationStatement& remote)
  {
    if (info.get_current_scope() == NULL)
      return "<missing-scope>";

    SgScopeStatement& current = SG_DEREF(info.get_current_scope());

    return unparser.computeScopeQual(current, SG_DEREF(remote.get_scope()));
  }
}


//-----------------------------------------------------------------------------------
//  void Unparse_Ada::unparseType
//
//  General function that gets called when unparsing an Ada type. Then it routes
//  to the appropriate function to unparse each Ada type.
//-----------------------------------------------------------------------------------
void
Unparse_Ada::unparseType(SgType* type, SgUnparse_Info& info)
{
  ASSERT_not_null(type);

  sg::dispatch(AdaTypeUnparser(*this, info, std::cerr), type);
}

