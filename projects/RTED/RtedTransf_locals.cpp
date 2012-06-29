#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include <sageGeneric.h>
#include "RtedTransformation.h"

namespace SI = SageInterface;
namespace SB = SageBuilder;

namespace
{
  /// Determines a nice scope name for @c stmt.  This is only used for convenience
  /// in the debugger, and does not affect any checks.
  struct ScopeName : sg::DispatchHandler<std::string>
  {
    ScopeName() : Base() {}

    void handle(const SgNode&)               { res = "unknown"; }
    void handle(const SgWhileStmt&)          { res = "while"; }
    void handle(const SgIfStmt&)             { res = "if"; }
    void handle(const SgForStatement&)       { res = "for"; }
    void handle(const SgUpcForAllStatement&) { res = "upc_forall"; }
    void handle(const SgDoWhileStmt&)        { res = "do while"; }
  };
}


SgStatement*
RtedTransformation::buildEnterScopeGuard(SgScopeStatement& scope)
{
  static const bool needname = false;
  static const bool needqual = false;
  static const bool needparens = true;
  static const bool classknown = false;

  ROSE_ASSERT(symbols.roseScopeGuard);

  SgMemberFunctionDeclaration* ctor = NULL;
  SgExprListExp*               ctorargs = SB::buildExprListExp();

  SI::appendExpression(ctorargs, SB::buildStringVal("C++-ScopeGuard"));
  appendFileInfo(ctorargs, &scope, scope.get_endOfConstruct());

  SgConstructorInitializer*    guardinit = SB::buildConstructorInitializer(ctor, ctorargs, symbols.roseScopeGuard, needname, needqual, needparens, classknown);

  return SB::buildVariableDeclaration("scopeguard", symbols.roseScopeGuard, guardinit, &scope);
}

/// add @c beginScope before @c stmt and @c endScope after @c stmt.
/// use @c end_of_scope to determine the @c Sg_File_Info to use for reporting
/// error location.
void
RtedTransformation::bracketWithScopeEnterExit( ScopeContainer::value_type pseudoscope )
{
    SgScopeStatement* const stmt_or_block = pseudoscope.stmt;
    Sg_File_Info*     const exit_file_info = stmt_or_block->get_endOfConstruct();

    ROSE_ASSERT(stmt_or_block && exit_file_info);

    // \pp \todo this works for C/UPC but not for C++ where
    //           exceptions can lead to scope unwinding.
    //           Use RAII to guarantee that the Rted scope information
    //           is properly managed.
    SgStatement*      guard = pseudoscope.cxx_lang
                                   ? buildEnterScopeGuard(*stmt_or_block)
                                   : buildEnterBlockStmt(sg::dispatch(ScopeName(), stmt_or_block))
                                   ;

    SgBasicBlock*     block = isSgBasicBlock( stmt_or_block );

    // order is important here... a block is a statement, but a statement is not
    // necessarily a block
    // \pp \todo since we create a block (at least in some cases), I suggest
    //           to create the block up front (when needed/desired) and then
    //           handle these cases with the same code.
    if( block )
        block -> prepend_statement( guard );
    else
    {
      std::cerr << "@@@ inserting scope : " << "   " << stmt_or_block->class_name() << std::endl;

      // tps : 10/07/2009: what if the statement before is a for loop, then we have to insert a block as well
      SgStatement* parentStmt = isSgStatement(stmt_or_block->get_parent());
      ROSE_ASSERT(parentStmt);

      std::cerr << "    @@@ parent == " << parentStmt->class_name() << std::endl;

      // this loop handles scope introducing constructs
      // \pp is the if/switch missing?
      if (GeneralizdFor::is(parentStmt) || isSgWhileStmt(parentStmt))
      {
        SgBasicBlock* bb = SB::buildBasicBlock();
        bb->set_parent(parentStmt);
        bb->append_statement(guard);
        bb->append_statement(stmt_or_block);

        if (isSgForStatement(parentStmt))
          isSgForStatement(parentStmt)->set_loop_body(bb);
        else if (isSgUpcForAllStatement(parentStmt))
          isSgUpcForAllStatement(parentStmt)->set_loop_body(bb);
        // \pp \todo else while handling missing?
      }
      else
      {
        // this handles the do loop
        SI::insertStatementBefore( stmt_or_block, guard );
      }
    }

    // \todo insert falloff protection when requested (pseudoscope.falloff_protection)
    //       i.e., to guard against int foo() { /* missing return statement */ }

    // we are done for C++
    if (pseudoscope.cxx_lang) return;

    SgScopeStatement* scope = stmt_or_block->get_scope();
    ROSE_ASSERT(scope);

    SgStatement*      close_call = buildExitBlockStmt(1, *scope, exit_file_info);

    // order is important here... a block is a statement, but a statement is not
    // necessarily a block
    if( block )
        block -> append_statement( close_call );
    else
        SI::insertStatementAfter( stmt_or_block, close_call );

    SI::attachComment( close_call, "", PreprocessingInfo::before );
    SI::attachComment( close_call, "RS : exitScope, parameters : (closingBlocks, file_info)", PreprocessingInfo::before);
}

#endif
