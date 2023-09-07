#ifndef _ADA_STATEMENT
#define _ADA_STATEMENT 1

/// Contains functions and classes for converting Ada statements and
///   declarations from Asis to ROSE.

#include "Ada_to_ROSE.h"
#include "a_nodes.h"

namespace Ada_ROSE_Translation
{
  /// represents data extracted from an Asis name
  struct NameData
  {
      NameData(std::string id, std::string full, SgScopeStatement& scope, Element_Struct& el)
      : ident(id), fullName(full), parent(&scope), asisElem(&el)
      {}

      /// returns the main (right-most) element defining this name
      Element_Struct& elem() const { return SG_DEREF(asisElem); }

      /// returns the id of the main element
      Element_ID      id()   const { return elem().ID; }

      /// returns the parent scope
      SgScopeStatement& parent_scope() const { return SG_DEREF(parent); }

      std::string       ident;    ///< the element's name
      std::string       fullName; ///< full, scope-qualified name

    private:
      SgScopeStatement* parent;   ///< the parent scope
      Element_Struct*   asisElem; ///< the main asis element, accessed through elem() and id().

      NameData() = delete;
  };


  /// functor to convert statements
  /// \details
  ///    the created statements are added to the most recent scope
  struct StmtCreator
  {
      explicit
      StmtCreator(AstContext astctx)
      : ctx(astctx)
      {}

      void operator()(Element_Struct& elem);

    private:
      AstContext ctx;
  };

  /// call-back to complete a function/procedure/entry declarations
  ///   by adding parameters to the scopes (after they have been created)
  struct ParameterCompletion
  {
      ParameterCompletion(ElemIdRange paramrange, AstContext astctx)
      : range(paramrange), ctx(astctx)
      {}

      void operator()(SgFunctionParameterList& lst, SgScopeStatement& parmscope);

    private:
      ElemIdRange range;
      AstContext  ctx;
      ParameterCompletion() = delete;
  };

  /// converts an Asis clause and adds the new node to the current scope
  /// \todo currently only with clauses are handled
  void handleClause(Element_Struct& elem, AstContext ctx);

  /// converts an Asis declaration and adds the new node to the current scope
  void handleDeclaration(Element_Struct& elem, AstContext ctx, bool isPrivate = false);

  /// converts an Asis definition and adds the new node to the current scope
  void handleDefinition(Element_Struct& elem, AstContext ctx);

  /// returns the declaration identified by \ref expr
  ///   if no record was found, nullptr is returned
  /// \note the function does not look up variables and exceptions, and built-in operators.
  SgDeclarationStatement*
  queryDecl(Expression_Struct& expr, AstContext ctx);

  /// returns the inner scope (*) of a preceding declaration
  /// \note the inner scope of a spec-decl, body-decl, func is the spec, body, func-def respectively ...
  SgScopeStatement&
  queryScopeOfID(Element_ID id, AstContext ctx);

  /// returns the NameData object for a name that is represented
  /// as expression in Asis (e.g., identifier or selected)
  NameData
  getQualName(Element_Struct& elem, AstContext ctx);

  /// returns a NameData object for the element \ref el
  NameData
  getNameID(Element_ID el, AstContext ctx);

  /// returns the name information associated with \ref decl
  /// \pre decl only has one name
  NameData
  singleName(Declaration_Struct& decl, AstContext ctx);

  /// creates the representation for all names in the range \ref range
  std::vector<NameData>
  allNames(ElemIdRange range, AstContext ctx);

  /// extracts NameData from \ref elem
  /// \pre elem is An_Expression
  NameData
  getName(Element_Struct& elem, AstContext ctx);

  /// creates a discriminated declaration in scope ctx.scope() iff id is not 0.
  /// \returns a SgAdaDiscriminatedTypeDecl object, iff id is not 0
  ///          nullptr, otherwise
  SgAdaDiscriminatedTypeDecl*
  createDiscriminatedDeclID_opt(Element_ID id, AstContext ctx);

  /// processes inherited subprograms and (TODO) declarations
  /// \param derivedType  the derived or extended type
  /// \param subprograms  an Asis subrogram list
  /// \param declarations an Asis declarations list
  /// \param ctx          the translation context
  void processInheritedSubroutines( SgNamedType& derivedType,
                                    ElemIdRange subprograms,
                                    ElemIdRange declarations,
                                    AstContext ctx
                                  );
}

#endif /* _ADA_STATEMENT */
