#ifndef _LIBADALANG_STATEMENT
#define _LIBADALANG_STATEMENT 1

/// Contains functions and classes for converting Ada statements and
///   declarations from Libadalang to ROSE.

#include "Libadalang_to_ROSE.h"
#include "libadalang.h"

namespace Libadalang_ROSE_Translation
{

  /// call-back to complete a function/procedure/entry declarations
  ///   by adding parameters to the scopes (after they have been created)
  struct ParameterCompletion
  {
      ParameterCompletion(ada_base_entity* paramrange, AstContext astctx)
      : range(paramrange), ctx(astctx)
      {}

      void operator()(SgFunctionParameterList& lst, SgScopeStatement& parmscope);

    private:
      ada_base_entity* range;
      AstContext  ctx;
      ParameterCompletion() = delete;
  };

  /// returns the declaration identified by \ref lal_element
  ///   if no record was found, nullptr is returned
  /// \note the function does not look up variables and exceptions, and built-in operators.
  SgDeclarationStatement*
  queryDecl(ada_base_entity* lal_element, int defining_name_hash, AstContext ctx);

  /// converts a Libadalang clause (with, use) and adds the new node to the current scope
  void handleClause(ada_base_entity* lal_element, AstContext ctx);

  /// converts a Libadalang declaration and adds the new node to the current scope
  void handleDeclaration(ada_base_entity* lal_element, AstContext ctx, bool isPrivate);

  /// converts a Libadalang pragma and adds the new node to the current scope
  void handlePragma(ada_base_entity* lal_element, SgStatement* stmtOpt, AstContext ctx);

  /// converts a Libadalang definition and adds the new node to the current scope
  void handleDefinition(ada_base_entity* lal_element, AstContext ctx);

  /// converts a Libadalang statement
  /// \param lal_stmt the stmt to convert
  /// \param ctx the context
  /// \param lblname The name attached to this stmt, if one exists
  void handleStmt(ada_base_entity* lal_stmt, AstContext ctx, const std::string& lblname = "");

  /// converts a Libadalang exception
  /// \param lal_element an ada_exception_handler node to convert
  /// \param tryStmt the parent stmt
  /// \param ctx the context
  void handleExceptionHandler(ada_base_entity* lal_element, SgTryStmt& tryStmt, AstContext ctx);

  /// processes inherited subprograms and (TODO) declarations
  /// \param derivedType  the derived or extended type
  /// \param tydef        the def of the original type
  /// \param ctx          the translation context
  void processInheritedSubroutines( SgNamedType& derivedType,
                                    ada_base_entity* tydef,
                                    AstContext ctx
                                  );

  /// creates a ROSE expression for a libadalang declaration's initializer expression
  /// returns null, if no declaration exists.
  /// \param expectedType a type that is carried over from a lhs constant declaration
  SgExpression*
  getVarInit(ada_base_entity* lal_decl, SgType* /*expectedType*/, AstContext ctx);

  /// creates a sequence of initialized names for all names \ref names.
  /// \param m        a map that maintains mappings between libadalang hashes and variables/parameters
  /// \param names    the list of libadalang names
  /// \param dcltype  the type of all initialized name
  /// \param initexpr the initializer (if it exists) that will be cloned for each
  ///                 of the initialized names.
  SgInitializedNamePtrList
  constructInitializedNamePtrList( AstContext ctx,
                                   map_t<int, SgInitializedName*>& m,
                                   ada_base_entity* lal_name_list,
                                   SgType& dcltype,
                                   SgExpression* initexpr,
                                   std::vector<int>& secondaries
                                 );
  SgInitializedNamePtrList
  constructInitializedNamePtrList( AstContext ctx,
                                   map_t<int, SgInitializedName*>& m,
                                   ada_base_entity* lal_name_list,
                                   SgType& dcltype,
                                   SgExpression* initexpr
                                 );

  /// returns the ROSE scope of an already converted Asis element \ref elem.
  SgScopeStatement&
  queryScopeOf(int declHash, AstContext);
}


#endif /* _LIBADALANG_STATEMENT */
