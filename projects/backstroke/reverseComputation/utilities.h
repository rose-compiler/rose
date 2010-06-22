#ifndef REVERSE_COMPUTATION_UTILITIES_H
#define REVERSE_COMPUTATION_UTILITIES_H

#include <rose.h>

// Return if the value in a SgValueExp object is zero.
bool isZero(SgValueExp* value);

// Reverse the Sgop_mode from prefix to postfix, or vice versa.
SgUnaryOp::Sgop_mode reverseOpMode(SgUnaryOp::Sgop_mode mode);

// Check if there is another used variable with the same name in the current scope.
// If yes, alter the name until it does not conflict with any other variable name.
void validateName(std::string& name, SgNode* root);

// Identify if two variables are the same. A variable may be a SgVarRefExp object
// or a SgArrowExp object.
bool areSameVariable(SgExpression* exp1, SgExpression* exp2);

// If the expression contains the given variable
bool containsVariable(SgExpression* exp, SgExpression* var);

// Return whether a basic block contains a break statement. 
bool hasBreakStmt(SgBasicBlock* body);

// If two expressions can be reorderd (in other word, reordering does not change the result).
bool canBeReordered(SgExpression* exp1, SgExpression* exp2);

// Tell if a type is a STL container type.
bool isSTLContainer(SgType* type);

// Get the defined copy constructors in a given class. Returns empty vector if the copy constructor is implicit.
std::vector<SgMemberFunctionDeclaration*> 
getCopyConstructors(SgClassDeclaration* class_decl);

#endif






