#include <rose.h>
#include <string>
#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"
//#include "RuntimeSystem.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;


// TODO 1 djh: test scope statements
//    while loop
//    if loop
//    for loop
//    block


void 
RtedTransformation::bracketWithScopeEnterExit( SgStatement* stmt) {
    ROSE_ASSERT( stmt);
    ROSE_ASSERT( roseEnterScope);

    // enterScope();
    insertStatementBefore( 
      stmt,
      buildExprStatement(
        buildFunctionCallExp(
          buildFunctionRefExp( roseEnterScope),
          buildExprListExp()
        )
      )
    );


    // exitScope( (char*) filename, (char*) line, (char*) stmtStr);
    SgExprListExp* exit_scope_args = buildExprListExp();
    appendExpression(
      exit_scope_args,
      buildString( stmt->get_file_info()->get_filename())
    );
    appendExpression(
      exit_scope_args,
      buildString( RoseBin_support::ToString( stmt->get_file_info()->get_line()))
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
        "RS : exitScope, parameters : ( filename, line, error line)",
        PreprocessingInfo::before
    );
}
