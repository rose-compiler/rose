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
      NameData(std::string id, std::string full, SgScopeStatement* scope, Element_Struct* el)
      : ident(id), fullName(full), parent(scope), asisElem(el)
      {}

      /// returns the main (right-most) element defining this name
      Element_Struct& elem() const { return SG_DEREF(asisElem); }

      /// returns the id of the main element
      Element_ID      id()   const { return elem().ID; }

      std::string       ident;    ///< the name of the r
      std::string       fullName; ///< full, scope qualified name
      SgScopeStatement* parent;   ///< the parent scope
      Element_Struct*   asisElem; ///< the main asis element, accessed through elem() and id().

    private:
      NameData() = delete;
  };

  /// returns the name information associated with \ref decl
  /// \pre decl only has one name
  NameData
  singleName(Declaration_Struct& decl, AstContext ctx);

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

  /// converts an Asis clause and adds the new node to the current scope
  /// \todo currently only with clauses are handled
  void handleClause(Element_Struct& elem, AstContext ctx);

  /// converts an Asis declaration and adds the new node to the current scope
  void handleDeclaration(Element_Struct& elem, AstContext ctx, bool isPrivate = false);

  /// converts an Asis definition and adds the new node to the current scope
  void handleDefinition(Element_Struct& elem, AstContext ctx);

  /// returns the declaration identified by \ref expr
  ///   if no record was found, nullptr is returned
  SgDeclarationStatement*
  getDecl_opt(Expression_Struct& expr, AstContext ctx);

  /// returns a NameData object for the element \ref el
  NameData
  getNameID(Element_ID el, AstContext ctx);
}

#endif /* _ADA_STATEMENT */
