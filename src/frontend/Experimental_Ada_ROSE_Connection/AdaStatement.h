#ifndef _ADA_STATEMENT
#define _ADA_STATEMENT 1

/// Contains functions and classes for converting Ada statements and
///   declarations from Asis to ROSE.

#include "Ada_to_ROSE.h"
#include "a_nodes.h"

namespace Ada_ROSE_Translation
{
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

  /// returns the declaration identified by \ref expr
  ///   if no record was found, nullptr is returned
  SgDeclarationStatement*
  getDecl_opt(Expression_Struct& expr, AstContext ctx);
}

#endif /* _ADA_STATEMENT */
