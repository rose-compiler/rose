#ifndef BACKSTROKE_VG_EVENTREVERSER_H
#define	BACKSTROKE_VG_EVENTREVERSER_H

#include <rose.h>

namespace Backstroke
{

class ValueNode;

SgExpression* buildPushFunction(ValueNode* node);

SgExpression* buildPopFunction(ValueNode* node, SgType* type);

SgExpression* buildOperation(
        ValueNode* result,
        VariantT type,
        ValueNode* lhs,
        ValueNode* rhs = NULL);

} // end of Backstroke

#endif	/* BACKSTROKE_VG_EVENTREVERSER_H */

