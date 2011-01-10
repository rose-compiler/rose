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


void
RtedTransformation::bracketWithScopeEnterExit( SgFunctionDefinition* fndef ) {
    SgBasicBlock* body = fndef -> get_body();
    ROSE_ASSERT( body );

    bracketWithScopeEnterExit( body, body -> get_endOfConstruct() );
}


/// add @c beginScope before @c stmt and @c endScope after @c stmt.
/// use @c end_of_scope to determine the @c Sg_File_Info to use for reporting
/// error location.
void
RtedTransformation::bracketWithScopeEnterExit( SgStatement* stmt_or_block, Sg_File_Info* exit_file_info )
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
    //           handle all cases alike.
    if( block )
        block -> prepend_statement( fncall_enter );
    else
    {
      cerr << "@@@ inserting scope : " << stmt_or_block->unparseToString() <<
      	"   " << stmt_or_block->class_name() << "  " << fiStmt->isCompilerGenerated() << endl;
      // tps : 10/07/2009: what if the statement before is a for loop, then we have to insert a block as well
      SgStatement* parentStmt = isSgStatement(stmt_or_block->get_parent());
      ROSE_ASSERT(parentStmt);

      cerr << "    @@@ parent == " << parentStmt->class_name() << endl;
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
        // \pp where/why is this needed???
        insertStatementBefore( stmt_or_block, fncall_enter );
      }
    }

    // exitScope( (char*) filename, (char*) line, (char*) lineTransformed, (char*) stmtStr);
    SgExprListExp* exit_scope_args = buildExprListExp();
    appendExpression(
      exit_scope_args,
      buildStringVal( removeSpecialChar( stmt_or_block->unparseToString()))
    );

    appendFileInfo(exit_scope_args, exit_file_info);

    SgExprStatement* exit_scope_call =
        buildExprStatement(
          buildFunctionCallExp(
            buildFunctionRefExp( symbols.roseExitScope),
            exit_scope_args
          )
        );

    // order is important here... a block is a statement, but a statement is not
    // necessarily a block
    if( block )
        block -> append_statement( exit_scope_call );
    else
        insertStatementAfter( stmt_or_block, exit_scope_call );

    attachComment( exit_scope_call, "", PreprocessingInfo::before );
    attachComment( exit_scope_call, "RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt)", PreprocessingInfo::before);
}


/// Determines a nice scope name for @c stmt.  This is only used for convenience
/// in the debugger, and does not affect any checks.
std::string
RtedTransformation::scope_name( SgNode* stmt) {

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
      vector<SgNode*> calls
        = NodeQuery::querySubTree( stmt, V_SgFunctionCallExp);

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

#endif
