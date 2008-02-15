#ifndef REPLACEEXPRESSIONWITHSTATEMENT_H
#define REPLACEEXPRESSIONWITHSTATEMENT_H

// DQ (3/12/2006): This is now not required, the file name is 
// changed to rose_config.h and is included directly by rose.h.
// #include "config.h"

#include "rose.h"
#include <string>

// DQ (3/12/2006): This is available from rose.h
// #include <list>

extern int gensym_counter;

//! Create a list with one element
inline static Rose_STL_Container<SgStatement*> make_unit_list(SgStatement* x) {
  Rose_STL_Container<SgStatement*> result;
  result.push_back(x);
  return result;
}

//! Interface for creating a statement whose computation writes its answer into
//! a given variable.
class StatementGenerator {
  public:
  virtual ~StatementGenerator() {};
  virtual SgStatement* generate(SgExpression* where_to_write_answer) = 0;
};

//! Replace an assignment statement with the result of a statement
//! generator.
//! Assumptions: from has assignment expr at top level of statement
//!              from is not in the test of a loop or if statement
//!              not currently traversing from
void replaceAssignmentStmtWithStatement(SgExprStatement* from,
					StatementGenerator* to);

// DQ (12/14/2006): Modified this function to find the root node of an expression, which 
// is not always a SgExpressionRoot, now that we have removed these from the AST.
//! Get the topmost enclosing expression of a given expression.  Needs to be moved.
SgExpression* getRootOfExpression(SgExpression* n);

//! Similar to replaceExpressionWithStatement, but with more restrictions.
//! Assumptions: from is not within the test of a loop or if
//!              not currently traversing from or the statement it is in
void replaceSubexpressionWithStatement(SgExpression* from,
				       StatementGenerator* to);

//! Change continue statements in a given block of code to gotos to a label
void changeContinuesToGotos(SgStatement* stmt, SgLabelSymbol* label);

//! Generic routines to get and set the body of a loop
template <class Loop>
inline SgBasicBlock* getLoopBody(Loop* loop) {
  return loop->get_body();
}

template <class Loop>
inline void setLoopBody(Loop* loop, SgBasicBlock* body) {
     loop->set_body(body);

  // DQ (6/24/2006): Set the parent as well
     body->set_parent(loop);
}

template <>
inline SgBasicBlock* getLoopBody(SgForStatement* loop) {
  return loop->get_loop_body();
}

template <>
inline void setLoopBody(SgForStatement* loop, SgBasicBlock* body) {
     loop->set_loop_body(body);

  // DQ (6/24/2006): Set the parent as well
     body->set_parent(loop);
}

//! Generic routines to get and set the condition of a loop
template <class Loop>
inline SgExpression* getLoopConditionRoot(Loop* loop) {
  return isSgExprStatement(loop->get_condition())->get_expression();
}

template <class Loop>
inline void setLoopConditionRoot(Loop* loop, SgExpression* body) {
  // DQ (3/8/2006): Replaces use of SgNULL_FILE macro!
  // loop->set_condition(new SgExprStatement(SgNULL_FILE, body->get_operand_i()));
  // loop->set_condition(new SgExprStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode(), body->get_operand_i()));
     SgExprStatement* exprStatement = new SgExprStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode(), body);
     exprStatement->set_endOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
     loop->set_condition(exprStatement);

  // DQ (6/24/2006): Set the parent as well
     body->set_parent(exprStatement);
     exprStatement->set_parent(loop);
}

template <>
inline SgExpression* getLoopConditionRoot(SgForStatement* loop) {
  return loop->get_test_expr();
}

template <>
inline void setLoopConditionRoot(SgForStatement* loop, SgExpression* body) {
     loop->set_test_expr(body);

  // DQ (6/24/2006): Set the parent as well
     body->set_parent(loop);
}

//! Rewrites a while or for loop so that the official test is changed to
//! "true" and what had previously been the test is now an if-break
//! combination (with an inverted condition) at the beginning of the loop
//! body
//!
//! For example, "while (a < 5) ++a;" becomes:
//! "while (true) {bool temp; temp = (a < 5); if (!temp) break; ++a;}"
template <class LoopStatement>
void pushTestIntoBody(LoopStatement* loopStmt);

//! Add a step statement to the end of a loop body
//! Add a new label to the end of the loop, with the step statement after
//! it; then change all continue statements in the old loop body into
//! jumps to the label
//!
//! For example:
//! while (a < 5) {if (a < -3) continue;} (adding "a++" to end) becomes
//! while (a < 5) {if (a < -3) goto label; label: a++;}
template <class LoopStatement>
void addStepToLoopBody(LoopStatement* loopStmt, SgStatement* step);

//! Replace a given expression with a list of statements produced by a
//! generator.  The generator, when given a variable as input, must produce
//! some code which leaves its result in the given variable.  The output
//! from the generator is then inserted into the original program in such a
//! way that whenever the expression had previously been evaluated, the
//! statements produced by the generator are run instead and their result is
//! used in place of the expression.
//! Assumptions: not currently traversing from or the statement it is in
void replaceExpressionWithStatement(SgExpression* from,
				    StatementGenerator* to);

//! Get a reference to the last element of an STL container
template <class Cont>
inline typename Cont::value_type&
lastElementOfContainer(Cont& c) {
  // Get reference to last element of an STL container;
  assert (!c.empty());
  typename Cont::iterator last = c.end();
  --last;
  return *last;
}

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

//! Add a new temporary variable to contain the value of from
//! Change reference to from to use this new variable
//! Assumptions: from is not within the test of a loop or if
//!              not currently traversing from or the statement it is in
SgAssignInitializer* splitExpression(SgExpression* from, 
				     std::string newName = "");

//! Convert something like "int a = foo();" into "int a; a = foo();"
SgAssignOp* convertInitializerIntoAssignment(SgAssignInitializer* init);

//! Get the statement of an expression.  Somewhat smarter than the standard
//! version of this routine, but needs to be merged.
SgStatement* getStatementOfExpression(SgNode* n);

#endif // REPLACEEXPRESSIONWITHSTATEMENT_H
