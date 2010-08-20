#ifndef REPLACEEXPRESSIONWITHSTATEMENT_H
#define REPLACEEXPRESSIONWITHSTATEMENT_H

// DQ (3/12/2006): This is now not required, the file name is 
// changed to rose_config.h and is included directly by rose.h.
// #include "config.h"


#include <string>

// DQ (3/12/2006): This is available from rose.h
// #include <list>

//extern int gensym_counter;

// King84: there seems to be a circular dependency with including sage interface (which happens before this file is included), so we just predeclare the one class we use from it.
namespace SageInterface { class StatementGenerator; };

//! Create a list with one element
inline static Rose_STL_Container<SgStatement*> make_unit_list(SgStatement* x) {
  Rose_STL_Container<SgStatement*> result;
  result.push_back(x);
  return result;
}

//! Replace an assignment statement with the result of a statement
//! generator.
//! Assumptions: from has assignment expr at top level of statement
//!              from is not in the test of a loop or if statement
//!              not currently traversing from
void replaceAssignmentStmtWithStatement(SgExprStatement* from,
					SageInterface::StatementGenerator* to);

// DQ (12/14/2006): Modified this function to find the root node of an expression, which 
// is not always a SgExpressionRoot, now that we have removed these from the AST.
//! Get the topmost enclosing expression of a given expression.  Needs to be moved.
SgExpression* getRootOfExpression(SgExpression* n);

//! Rewrites a while or for loop so that the official test is changed to
//! "true" and what had previously been the test is now an if-break
//! combination (with an inverted condition) at the beginning of the loop
//! body
//!
//! For example, "while (a < 5) ++a;" becomes:
//! "while (true) {bool temp; temp = (a < 5); if (!temp) break; ++a;}"
void pushTestIntoBody(SgScopeStatement* loopStmt);


//! Insert a new statement before or after a target statement.  If
//! allowForInit is true, the new statement can be inserted into the
//! initializer of a for statement.
//! Needs to be merged
void myStatementInsert(SgStatement* target, 
		       SgStatement* newstmt,
		       bool before,
		       bool allowForInit = false);

//! Replace the expression "from" with another expression "to", wherever it
//! appears in the AST.  The expression "from" is not deleted, and may be
//! reused elsewhere in the AST.
void replaceExpressionWithExpression(SgExpression* from, SgExpression* to);

//! Convert something like "int a = foo();" into "int a; a = foo();"
SgAssignOp* convertInitializerIntoAssignment(SgAssignInitializer* init);

//! Get the statement of an expression.  Somewhat smarter than the standard
//! version of this routine, but needs to be merged.
SgStatement* getStatementOfExpression(SgNode* n);

#endif // REPLACEEXPRESSIONWITHSTATEMENT_H
