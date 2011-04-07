#include "eventReverser.h"
#include "valueGraphNode.h"
#include <sageBuilder.h>
#include <sageInterface.h>

namespace Backstroke
{

using namespace std;
using namespace SageBuilder;

//! Build a variable expression from a value node in the value graph.
SgExpression* buildVariable(ValueNode* node)
{
    SgExpression* var;
    if (node->isAvailable())
        var = SageInterface::copyExpression(isSgExpression(node->astNode));
    else
        var = buildVarRefExp(node->vars[0].name[0]);
    return var;
}

SgExpression* buildPushFunction(ValueNode* node)
{
	SgExprListExp* parameters = buildExprListExp(buildVariable(node));
	return buildFunctionCallExp("push", buildVoidType(), parameters);
}

SgExpression* buildPopFunction(ValueNode* node, SgType* type)
{
    
	SgExpression* popFunc = buildFunctionCallExp("pop< " + get_type_name(type) + " >",
            type, SageBuilder::buildExprListExp());
    return buildAssignOp(buildVariable(node), popFunc);
}

SgExpression* buildOperation(
        ValueNode* result,
        VariantT type,
        ValueNode* lhs,
        ValueNode* rhs)
{
    SgExpression* resExpr = buildVariable(result);
    SgExpression* lhsExpr = buildVariable(lhs);
    SgExpression* rhsExpr = rhs ? buildVariable(rhs) : NULL;
    SgExpression* opExpr  = NULL;

    // Unary expression case.
    if (rhs == NULL)
    {
        switch (type)
        {

#define BUILD_UNARY_OP(suffix) \
case V_Sg##suffix: opExpr = build##suffix(lhsExpr); break;

            BUILD_UNARY_OP(AddressOfOp)
            BUILD_UNARY_OP(BitComplementOp)
            BUILD_UNARY_OP(MinusOp)
            BUILD_UNARY_OP(NotOp)
            BUILD_UNARY_OP(PointerDerefExp)
            BUILD_UNARY_OP(UnaryAddOp)
            BUILD_UNARY_OP(MinusMinusOp)
            BUILD_UNARY_OP(PlusPlusOp)
            BUILD_UNARY_OP(RealPartOp)
            BUILD_UNARY_OP(ImagPartOp)
            BUILD_UNARY_OP(ConjugateOp)
            BUILD_UNARY_OP(VarArgStartOneOperandOp)
            BUILD_UNARY_OP(VarArgEndOp)

#undef BUILD_BINARY_OP

            default:
                break;
        }
    }
    else
    {
        switch (type)
        {

#define BUILD_BINARY_OP(suffix) \
case V_Sg##suffix: opExpr = build##suffix(lhsExpr, rhsExpr); break;

            BUILD_BINARY_OP(AddOp)
            BUILD_BINARY_OP(AndAssignOp)
            BUILD_BINARY_OP(AndOp)
            BUILD_BINARY_OP(ArrowExp)
            BUILD_BINARY_OP(ArrowStarOp)
            BUILD_BINARY_OP(AssignOp)
            BUILD_BINARY_OP(BitAndOp)
            BUILD_BINARY_OP(BitOrOp)
            BUILD_BINARY_OP(BitXorOp)

            BUILD_BINARY_OP(CommaOpExp)
            BUILD_BINARY_OP(ConcatenationOp)
            BUILD_BINARY_OP(DivAssignOp)
            BUILD_BINARY_OP(DivideOp)
            BUILD_BINARY_OP(DotExp)
            BUILD_BINARY_OP(DotStarOp)
            BUILD_BINARY_OP(EqualityOp)

            BUILD_BINARY_OP(ExponentiationOp)
            BUILD_BINARY_OP(GreaterOrEqualOp)
            BUILD_BINARY_OP(GreaterThanOp)
            BUILD_BINARY_OP(IntegerDivideOp)
            BUILD_BINARY_OP(IorAssignOp)

            BUILD_BINARY_OP(LessOrEqualOp)
            BUILD_BINARY_OP(LessThanOp)
            BUILD_BINARY_OP(LshiftAssignOp)
            BUILD_BINARY_OP(LshiftOp)

            BUILD_BINARY_OP(MinusAssignOp)
            BUILD_BINARY_OP(ModAssignOp)
            BUILD_BINARY_OP(ModOp)
            BUILD_BINARY_OP(MultAssignOp)
            BUILD_BINARY_OP(MultiplyOp)

            BUILD_BINARY_OP(NotEqualOp)
            BUILD_BINARY_OP(OrOp)
            BUILD_BINARY_OP(PlusAssignOp)
            BUILD_BINARY_OP(PntrArrRefExp)
            BUILD_BINARY_OP(RshiftAssignOp)

            BUILD_BINARY_OP(RshiftOp)
            BUILD_BINARY_OP(ScopeOp)
            BUILD_BINARY_OP(SubtractOp)
            BUILD_BINARY_OP(XorAssignOp)

            BUILD_BINARY_OP(VarArgCopyOp)
            BUILD_BINARY_OP(VarArgStartOp)

#undef BUILD_BINARY_OP

            default:
                break;
        }
    }

    return buildAssignOp(resExpr, opExpr);
}

} // end of Backstroke