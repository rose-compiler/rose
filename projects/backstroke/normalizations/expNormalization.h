#ifndef REVERSE_COMPUTATION_NORMALIZATION
#define REVERSE_COMPUTATION_NORMALIZATION

#include <rose.h>

// Normalize an event function.
void normalizeEvent(SgFunctionDefinition* func_def);

bool isAssignmentOp(SgExpression* e);

// Transform a modifying expression into several ones contained in a comma 
// operator expression. The transformations are:
//
//    a = b  ==>  a = b, a
//    --a  ==>  --a, a
//    a--  ==>  t = a, --a, t
//    a && b  ==>  t = a, t && t = b, t
//    a || b  ==>  t = a, t || t = b, t
//
void getAndReplaceModifyingExpression(SgExpression*& exp);

SgExpression* normalizeExpression(SgExpression* exp);

// Split a comma expression into several statements.
void splitCommaOpExp(SgExpression* exp);

// Extend comma operation expressions, e.g. (a, b) + c ==> (a, b + c).
SgExpression* extendCommaOpExp(SgExpression* exp);

// Remove braces of a basic block in which there is no variable declared.
void removeUselessBraces(SgBasicBlock* body);



#endif
