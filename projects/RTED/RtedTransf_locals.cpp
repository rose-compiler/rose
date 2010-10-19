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

    bracketWithScopeEnterExit( isSgNode( body ), body -> get_endOfConstruct() );
}

/// add @c beginScope before @c stmt and @c endScope after @c stmt.
/// use @c end_of_scope to determine the @c Sg_File_Info to use for reporting
/// error location.
void 
RtedTransformation::bracketWithScopeEnterExit( SgStatement* stmt, SgNode* end_of_scope) {
    bracketWithScopeEnterExit( isSgNode( stmt ), end_of_scope -> get_endOfConstruct() );
}

void 
RtedTransformation::bracketWithScopeEnterExit( SgNode* stmt_or_block, Sg_File_Info* exit_file_info ) {

    SgStatement* stmt = isSgStatement( stmt_or_block );
    SgBasicBlock* block = isSgBasicBlock( stmt_or_block );
    if (!stmt || stmt->get_file_info()->isCompilerGenerated())
      return;

    ROSE_ASSERT( stmt );
    ROSE_ASSERT( exit_file_info );
    ROSE_ASSERT( symbols->roseEnterScope );

    SgExprListExp* enter_scope_args = buildExprListExp();
    appendExpression(
      enter_scope_args,
      buildString(
        scope_name( stmt_or_block ) + 
        ":" 
        + RoseBin_support::ToString( stmt_or_block -> get_file_info() -> get_line() )
      )
    );
   

    // enterScope( "foo:23");
    SgExprStatement* fncall_enter 
        = buildExprStatement(
            buildFunctionCallExp(
              buildFunctionRefExp( symbols->roseEnterScope),
              enter_scope_args
            )
          );
    // order is important here... a block is a statement, but a statement is not
    // necessarily a block
    if( block )
        block -> prepend_statement( fncall_enter );
    else {
      
      cerr << "@@@ inserting scope : " << stmt->unparseToString() << 
      	"   " << stmt->class_name() << "  " << stmt->get_file_info()->isCompilerGenerated() << endl;
      // tps : 10/07/2009: what if the statement before is a for loop, then we have to insert a block as well
      SgStatement* parentStmt = isSgStatement(stmt->get_parent());
      cerr << "    @@@ parent == " << parentStmt->class_name() << endl;
      if (parentStmt && (
                         isSgUpcForAllStatement(parentStmt) ||
			 isSgForStatement(parentStmt) ||
			 isSgWhileStmt(parentStmt))) {
        SgBasicBlock* bb = buildBasicBlock();
	bb->set_parent(parentStmt);
	bb->append_statement(fncall_enter);
	bb->append_statement(stmt);
	if (isSgForStatement(parentStmt))
	  isSgForStatement(parentStmt)->set_loop_body(bb);
	if (isSgUpcForAllStatement(parentStmt))
	  isSgUpcForAllStatement(parentStmt)->set_loop_body(bb);
      } else
      insertStatementBefore( stmt, fncall_enter );
    }

    // exitScope( (char*) filename, (char*) line, (char*) lineTransformed, (char*) stmtStr);
    SgExprListExp* exit_scope_args = buildExprListExp();
    appendExpression(
      exit_scope_args,
      buildString( exit_file_info -> get_filename() )
    );
    appendExpression(
      exit_scope_args,
      buildString( RoseBin_support::ToString( exit_file_info -> get_line() ))
    );
    appendExpression(
      exit_scope_args,
      buildString( "x%%x" )
    );
    appendExpression(
      exit_scope_args,
      buildString( removeSpecialChar( stmt->unparseToString()))
    );

    SgExprStatement* exit_scope_call = 
        buildExprStatement(
          buildFunctionCallExp(
            buildFunctionRefExp( symbols->roseExitScope),
            exit_scope_args
          )
        );
    // order is important here... a block is a statement, but a statement is not
    // necessarily a block
    if( block )
        block -> append_statement( exit_scope_call );
    else
        insertStatementAfter( stmt, exit_scope_call );


    attachComment(
        exit_scope_call,
        "",
        PreprocessingInfo::before
    );
    attachComment(
        exit_scope_call,
        "RS : exitScope, parameters : ( filename, line, lineTransformed, error stmt)",
        PreprocessingInfo::before
    );
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
