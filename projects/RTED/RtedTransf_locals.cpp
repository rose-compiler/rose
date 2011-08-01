#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

/// Determines a nice scope name for @c stmt.  This is only used for convenience
/// in the debugger, and does not affect any checks.
static
std::string scope_name(SgStatement* stmt)
{
    if( isSgWhileStmt( stmt)) {
      return "while";
    } else if( isSgIfStmt( stmt)) {
      return "if";
    } else if( isSgForStatement( stmt)) {
      return "for";
    } else if( isSgUpcForAllStatement( stmt)) {
      return "upc_forall";
    } else if( isSgDoWhileStmt( stmt)) {
      return "do";
    } else {
      //cerr << " Trying to query stmt : " << stmt->class_name() << endl;
      const SgNodePtrList& calls = NodeQuery::querySubTree( stmt, V_SgFunctionCallExp);

      if( calls.size() > 0) {
        SgFunctionCallExp* fncall = isSgFunctionCallExp( calls[0]);
        ROSE_ASSERT( fncall);
        return fncall
                ->getAssociatedFunctionDeclaration()
                ->get_name().getString();
      } else {
        cerr << "Unable to determine scope name." << endl;
        return "unknown";
      }
    }
}

SgExprStatement* RtedTransformation::buildExitBlockStmt(size_t blocks, SgScopeStatement* scope, Sg_File_Info* fileinfo)
{
    ROSE_ASSERT(scope && fileinfo);
    SgExprListExp* args = buildExprListExp();

    appendExpression(args, buildIntVal(blocks));
    appendFileInfo  (args, scope, fileinfo);

    return buildFunctionCallStmt( buildFunctionRefExp(symbols.roseExitScope), args );
}


/// add @c beginScope before @c stmt and @c endScope after @c stmt.
/// use @c end_of_scope to determine the @c Sg_File_Info to use for reporting
/// error location.
void
RtedTransformation::bracketWithScopeEnterExit( SgScopeStatement* stmt_or_block, Sg_File_Info* exit_file_info )
{
    ROSE_ASSERT(stmt_or_block && exit_file_info);

    Sg_File_Info*  fiStmt = stmt_or_block -> get_file_info();

    // only instrument user code
    if (fiStmt->isCompilerGenerated())
      return;

    SgBasicBlock*  block = isSgBasicBlock( stmt_or_block );
    SgExprListExp* enter_scope_args = buildExprListExp();

    appendExpression( enter_scope_args,
                      buildStringVal( scope_name( stmt_or_block )
                                    + ":"
                                    + RoseBin_support::ToString( fiStmt->get_line() )
                                    )
                    );

    // enterScope( "foo:23");
    // \pp \todo this works for C/UPC but not for C++ where
    //           exceptions can lead to scope unwinding.
    //           Use RAII to guarantee that the Rted scope information
    //           is properly managed.
    ROSE_ASSERT( symbols.roseEnterScope );
    SgFunctionRefExp* checker = buildFunctionRefExp( symbols.roseEnterScope);
    SgExprStatement*  fncall_enter = buildFunctionCallStmt(checker, enter_scope_args);

    // order is important here... a block is a statement, but a statement is not
    // necessarily a block
    // \pp \todo since we create a block (at least in some cases), I suggest
    //           to create the block up front (when needed/desired) and then
    //           handle these cases with the same code.
    if( block )
        block -> prepend_statement( fncall_enter );
    else
    {
      cerr << "@@@ inserting scope : " << "   " << stmt_or_block->class_name()
           << "  " << fiStmt->isCompilerGenerated() << endl;

      // tps : 10/07/2009: what if the statement before is a for loop, then we have to insert a block as well
      SgStatement* parentStmt = isSgStatement(stmt_or_block->get_parent());
      ROSE_ASSERT(parentStmt);

      cerr << "    @@@ parent == " << parentStmt->class_name() << endl;

      // this loop handles scope introducing constructs
      // \pp is the if/switch missing?
      if (GeneralizdFor::is(parentStmt) || isSgWhileStmt(parentStmt))
      {
        SgBasicBlock* bb = buildBasicBlock();
        bb->set_parent(parentStmt);
        bb->append_statement(fncall_enter);
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
        insertStatementBefore( stmt_or_block, fncall_enter );
      }
    }

    SgScopeStatement* scope = stmt_or_block->get_scope();
    SgStatement*      close_call = buildExitBlockStmt(1, scope, stmt_or_block->get_file_info());

    // order is important here... a block is a statement, but a statement is not
    // necessarily a block
    if( block )
        block -> append_statement( close_call );
    else
        insertStatementAfter( stmt_or_block, close_call );

    attachComment( close_call, "", PreprocessingInfo::before );
    attachComment( close_call, "RS : exitScope, parameters : (closingBlocks, file_info)", PreprocessingInfo::before);
}

#endif
