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
  constexpr bool USE_COMPUTED_NAME_QUALIFICATION_TYPE = true;

  /// returns m[key] iff \ref key is in \ref m
  ///                otherwise \ref defaultVal
  template <class Map>
  const typename Map::mapped_type&
  getQualMapping( const Map& m,
                  const typename Map::key_type& key,
                  const typename Map::mapped_type& defaultVal
                )
  {
    auto pos = m.find(key);

    return (pos == m.end()) ? defaultVal : pos->second;
  }

  struct AdaTypeUnparser
  {
    AdaTypeUnparser(Unparse_Ada& unp, SgUnparse_Info& inf, std::ostream& outp)
    : unparser(unp), info(inf), os(outp)
    {}

    void prnNameQual(const SgNode& n)
    {
      using NodeQualMap = std::map<SgNode*, std::string>;
      using Iterator = NodeQualMap::const_iterator;

      const NodeQualMap& nameQualMap = unparser.nameQualificationMap();
      const Iterator     pos = nameQualMap.find(const_cast<SgNode*>(&n));

      //~ std::cerr << "retr " << &n << " from " << &nameQualMap << std::endl;

      if (pos != nameQualMap.end())
        prn(pos->second);
    }

    void prn(const std::string& s)
    {
      unparser.curprint(s);
      //~ os << s;
    }

    std::string scopeQual(const SgDeclarationStatement& remote);

    std::string scopeQual(const SgDeclarationStatement* remote)
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
      exprSequence(n.get_indexRanges());
      prn(")");
    }

    void handle(SgAdaDiscriminantConstraint& n)
    {
      prn(" (");
      exprSequence(n.get_discriminants());
      prn(")");
    }

    //
    // Fundamental types

    void handle(SgTypeBool&)       { prn(" Boolean"); }
    void handle(SgTypeChar&)       { prn(" Character"); }
    void handle(SgTypeChar16&)     { prn(" Wide_Character"); }
    void handle(SgTypeChar32&)     { prn(" Wide_Wide_Character"); }
    void handle(SgTypeInt&)        { prn(" Integer"); }
    void handle(SgTypeFloat&)      { prn(" Float"); }
    void handle(SgTypeDouble&)     { prn(" Long_Float"); }
    void handle(SgTypeLongDouble&) { prn(" Long_Long_Float"); }
    void handle(SgTypeString&)     { prn(" String"); }
    void handle(SgTypeLong&)       { prn(" Long_Integer"); }
    void handle(SgTypeShort&)      { prn(" Short_Integer"); }
    void handle(SgTypeLongLong&)   { prn(" Long_Long_Integer"); }
    void handle(SgTypeVoid&)       { prn(" -- void\n"); }  // error, should not be in Ada

    //
    // Ada types

    void handle(SgAdaSubtype& n)
    {
      ROSE_ASSERT(!n.get_fromRootType() || n.get_constraint());

      if (!n.get_fromRootType())
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
/*
    void handle(SgAdaFormalType& n)
    {
      prn(" ");
      prn(n.get_type_name());
    }
*/
    void handle(SgModifierType& n)
    {
      if (n.get_typeModifier().isAliased())
        prn("aliased ");

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

      if (USE_COMPUTED_NAME_QUALIFICATION_TYPE)
      {
        //~ std::cerr << "unp named type: " << n.get_declaration()
                  //~ << " / " << &unparser.nameQualificationMap()
                  //~ << std::endl;
        prnNameQual(SG_DEREF(n.get_declaration()));
      }
      else
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
      prn(" of ");
      type(n.get_base_type());
    }


    void handle(SgAdaTaskType& n)
    {
      // \todo fix in AST and override get_name and get_declaration in AdaTaskType
      SgAdaTaskTypeDecl&      tyDcl  = SG_DEREF( isSgAdaTaskTypeDecl(n.get_declaration()) );

      prn(" ");

      if (USE_COMPUTED_NAME_QUALIFICATION_TYPE)
        prnNameQual(tyDcl);
      else
        prn(scopeQual(tyDcl));

      prn(tyDcl.get_name());
    }

    void handle(SgAdaProtectedType& n)
    {
      // \todo fix in AST and override get_name and get_declaration in AdaTaskType
      SgAdaProtectedTypeDecl& tyDcl  = SG_DEREF( isSgAdaProtectedTypeDecl(n.get_declaration()) );

      prn(" ");

      if (USE_COMPUTED_NAME_QUALIFICATION_TYPE)
        prnNameQual(tyDcl);
      else
        prn(scopeQual(tyDcl));

      prn(tyDcl.get_name());
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

        // consider: ASSERT_not_null(n.get_base_type());
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

    void exprSequence(const SgExpressionPtrList& lst);

    Unparse_Ada&      unparser;
    SgUnparse_Info&   info;
    std::ostream&     os;
  };

  void AdaTypeUnparser::exprSequence(const SgExpressionPtrList& lst)
  {
    if (lst.empty()) return;

    expr(lst[0]);

    for (size_t i = 1; i < lst.size(); ++i)
    {
      prn(", ");
      expr(lst[i]);
    }
  }

  std::string
  AdaTypeUnparser::scopeQual(const SgDeclarationStatement& remote)
  {
    const SgScopeStatement* current = info.get_current_scope();

    return current ? unparser.computeScopeQual(*current, SG_DEREF(remote.get_scope()))
                   : std::string{"<missing-scope>"};
  }
}


//-----------------------------------------------------------------------------------
//  void Unparse_Ada::unparseType
//
//  General function that gets called when unparsing an Ada type. Then it routes
//  to the appropriate function to unparse each Ada type.
//-----------------------------------------------------------------------------------
void
Unparse_Ada::unparseType(const SgLocatedNode& ref, SgType* ty, SgUnparse_Info& info)
{
  using MapOfNameQualMap = std::map<SgNode*, NameQualMap>;

  ASSERT_not_null(ty);

  SgNode* const currentReferenceNode = info.get_reference_node_for_qualification();

  // set the reference node, unless the unparser is already in type mode
  if (&nameQualificationMap() == &SgNode::get_globalQualifiedNameMapForNames())
    info.set_reference_node_for_qualification(const_cast<SgLocatedNode*>(&ref));

  SgNode*            refNode = info.get_reference_node_for_qualification();
  const NameQualMap& currentNameQualMap = nameQualificationMap();
  const MapOfNameQualMap& typeQualMap = SgNode::get_globalQualifiedNameMapForMapsOfTypes();
  const NameQualMap& nameQualMapForTypeSubtree = getQualMapping(typeQualMap, refNode, SgNode::get_globalQualifiedNameMapForTypes());

  withNameQualificationMap(nameQualMapForTypeSubtree);
  sg::dispatch(AdaTypeUnparser{*this, info, std::cerr}, ty);
  withNameQualificationMap(currentNameQualMap);

  // restore reference node
  info.set_reference_node_for_qualification(currentReferenceNode);
}
