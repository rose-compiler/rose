#ifndef BACKSTROKE_VG_EVENTREVERSER_H
#define	BACKSTROKE_VG_EVENTREVERSER_H

#include <rose.h>

namespace Backstroke
{

class ValueNode;

//! Build a variable declaration.
SgStatement* buildVarDeclaration(ValueNode* newVar, SgExpression* expr = 0);

void instrumentPushFunction(ValueNode* node);

SgStatement* buildPopFunction(ValueNode* node);

// If rhs is NULL, it's an assignment to itself, like a_1 = a;
SgStatement* buildAssignOpertaion(ValueNode* lhs, ValueNode* rhs = NULL);

SgStatement* buildOperation(
        ValueNode* result,
        VariantT type,
        ValueNode* lhs,
        ValueNode* rhs = NULL);

} // end of Backstroke

#endif	/* BACKSTROKE_VG_EVENTREVERSER_H */

