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
  queryDecl(ada_base_entity* lal_element, AstContext ctx);

  /// converts a Libadalang declaration and adds the new node to the current scope
  void handleDeclaration(ada_base_entity* lal_element, AstContext ctx, bool isPrivate);

  /// converts a Libadalang statement
  void handleStmt(ada_base_entity* lal_stmt, AstContext ctx, const std::string& lblname = "");

  /// converts a Libadalang exception
  void handleExceptionHandler(ada_base_entity* lal_exception, SgTryStmt& tryStmt, AstContext ctx);

  /// processes inherited subprograms and (TODO) declarations
  /// \param derivedType  the derived or extended type
  /// \param tydef        the def of the original type
  /// \param ctx          the translation context
  void processInheritedSubroutines( SgNamedType& derivedType,
                                    ada_base_entity* tydef,
                                    AstContext ctx
                                  );

}


#endif /* _LIBADALANG_STATEMENT */
