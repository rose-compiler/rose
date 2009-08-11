#include <rose.h>
#include <string>
#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;


void 
RtedTransformation::bracketWithScopeEnterExit( SgStatement* stmt, SgNode* end_of_scope) {
    ROSE_ASSERT( stmt);
    ROSE_ASSERT( roseEnterScope);

    SgExprListExp* enter_scope_args = buildExprListExp();
    appendExpression(
      enter_scope_args,
      buildString(
        scope_name( stmt) + 
        ":" 
        + RoseBin_support::ToString( stmt->get_file_info()->get_line())
      )
    );
    // enterScope( "foo:23");
    insertStatementBefore( 
      stmt,
      buildExprStatement(
        buildFunctionCallExp(
          buildFunctionRefExp( roseEnterScope),
          enter_scope_args
        )
      )
    );

    Sg_File_Info* exit_file_info = end_of_scope -> get_endOfConstruct();

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
            buildFunctionRefExp( roseExitScope),
            exit_scope_args
          )
        );

    insertStatementAfter( stmt, exit_scope_call);
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


std::string
RtedTransformation::scope_name( SgStatement* stmt) {

    if( isSgWhileStmt( stmt)) {
      return "while";
    } else if( isSgIfStmt( stmt)) {
      return "if";
    } else if( isSgForStatement( stmt)) {
      return "for";
    } else if( isSgDoWhileStmt( stmt)) {
      return "do";
    } else {
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
