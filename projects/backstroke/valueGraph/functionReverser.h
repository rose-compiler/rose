#ifndef BACKSTROKE_VG_EVENTREVERSER_H
#define	BACKSTROKE_VG_EVENTREVERSER_H

#include <rose.h>

namespace Backstroke
{

class ValueNode;

//! Build a variable expression from a value node in the value graph.
SgExpression* buildVariable(ValueNode* node);

//! Build a variable declaration.
SgStatement* buildVarDeclaration(ValueNode* newVar, SgExpression* expr = 0);

//! Instrument a push function for the variable in the given value node.
//! The function definition is needed in case that the variable declaration is
//! in a class, but not in a function (data member).
void instrumentPushFunction(ValueNode* node, SgFunctionDefinition* funcDef);

//! Insert a state saving statement (push function call) before the statement containing
//! the given AST node.
void instrumentPushFunction(ValueNode* valNode, SgNode* astNode);

//! Build a push function call expression. The input parameter is the paramenter of the push function.
SgExpression* buildPushFunctionCall(SgExpression* para);

//! Build a push function call statement. The input parameter is the paramenter of the push function.
SgStatement* buildPushStatement(ValueNode* valNode);

//! Build a push function call statement for a pointer variable.
SgStatement* buildPushStatementForPointerType(ValueNode* valNode);

//! Build a pop function call.
SgExpression* buildPopFunctionCall(SgType* type);

//! Build a pop function call statement.
SgStatement* buildPopStatement(SgType* type);

//! Build an expression to restore the given node.
SgExpression* buildRestorationExp(ValueNode* node);

//! Build a statement to restore the given node.
SgStatement* buildRestorationStmt(ValueNode* node);

// If rhs is NULL, it's an assignment to itself, like a_1 = a;
SgStatement* buildAssignOpertaion(ValueNode* lhs, ValueNode* rhs = NULL);

SgStatement* buildOperationStatement(
        ValueNode* result,
        VariantT type,
        ValueNode* lhs,
        ValueNode* rhs = NULL);

} // end of Backstroke

#endif	/* BACKSTROKE_VG_EVENTREVERSER_H */


