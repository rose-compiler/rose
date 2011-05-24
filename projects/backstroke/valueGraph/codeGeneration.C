#include "functionReverser.h"
#include "valueGraphNode.h"
#include <utilities/utilities.h>
#include <sageBuilder.h>
#include <sageInterface.h>

namespace Backstroke
{

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

namespace
{
    //! Build a variable expression from a value node in the value graph.
    SgExpression* buildVariable(ValueNode* node)
    {
        SgExpression* var;
        if (node->isAvailable())
            var = SageInterface::copyExpression(isSgExpression(node->astNode));
        else
        {
            var = node->var.getVarRefExp();
            //var = buildVarRefExp(node->str);
        }
        return var;
    }
} // end of anonymous

SgStatement* getAncestorStatement(SgNode* node)
{
    SgStatement* stmt;
    while (!(stmt = isSgStatement(node)))
    {
        node = node->get_parent();
        if (node == NULL)
            return NULL;
    }
    return stmt;
}

SgStatement* buildVarDeclaration(ValueNode* newVar, SgExpression* expr)
{
    SgAssignInitializer* init = expr ? buildAssignInitializer(expr) : NULL;
    return buildVariableDeclaration(newVar->str,
                                    newVar->getType(),
                                    init);
}

void instrumentPushFunction(ValueNode* valNode, SgNode* astNode)
{
    //cout << astNode->class_name() << endl;
    
    // Build push function call.
	//SgExprListExp* parameters = buildExprListExp(valNode->var.getVarRefExp());
	//SgExpression* pushFunc = buildFunctionCallExp("push", buildVoidType(), parameters);    
    SgExpression* pushFunc = buildPushFunctionCall(valNode->var.getVarRefExp());
    SgStatement* pushFuncStmt = buildExprStatement(pushFunc);
    
    SgStatement* stmt = getAncestorStatement(astNode);
    SageInterface::insertStatementBefore(stmt, pushFuncStmt); 
}

void instrumentPushFunction(ValueNode* valNode, SgFunctionDefinition* funcDef)
{
    // Build push function call.
	//SgExprListExp* parameters = buildExprListExp(valNode->var.getVarRefExp());
	//SgExpression* pushFunc = buildFunctionCallExp("push", buildVoidType(), parameters);
    SgExpression* pushFunc = buildPushFunctionCall(valNode->var.getVarRefExp());
    //return buildExprStatement(pushFunc);

    SgNode* varNode = valNode->astNode;
    if (SgExpression* expr = isSgExpression(varNode))
    {
        // If the target node is an expression, we have to find a proper place
        // to place the push function. Here we insert the push function after its
        // belonged statement.
        SgExpression* commaOp = buildCommaOpExp(pushFunc, copyExpression(expr));
        replaceExpression(expr, commaOp);
    }
    else if (SgInitializedName* initName = isSgInitializedName(varNode))
    {
        // If the target node (def node) is a variable declaration, find this
        // declaration statement first.
        SgNode* stmtNode = initName;
        while (stmtNode && !isSgStatement(stmtNode))
            stmtNode = stmtNode->get_parent();

        SgStatement* pushFuncStmt = buildExprStatement(pushFunc);

        // If this node belongs to event parameters, or is defined outside the function, 
        // add the push function just at the beginning of the forward function.
        // Else, add the push function under the declaration.
        if (isSgClassDefinition(stmtNode->get_parent()) ||
                isSgFunctionParameterList(stmtNode))
        {
            // If this declaration belongs to parameter list.
            prependStatement(pushFuncStmt, funcDef->get_body());
        }
        else
        {
            // Or it is a normal declaration.
            SgVariableDeclaration* varDecl = isSgVariableDeclaration(stmtNode);
            ROSE_ASSERT(varDecl);

            // If this declaration is not in if's condition, while's condition, etc,
            // add the push function just below it.
            if (BackstrokeUtility::isTrueVariableDeclaration(varDecl))
                insertStatementAfter(varDecl, pushFuncStmt);
            else
                ROSE_ASSERT(!"Declaration in conditions is not handled yet!");
        }

        //cout << initName->get_parent()->class_name() << endl;
    }
}

SgExpression* buildPushFunctionCall(SgExpression* para)
{
    return buildFunctionCallExp("push", buildVoidType(), 
            SageBuilder::buildExprListExp(para));
}

SgStatement* buildPushStatement(ValueNode* valNode)
{
    return buildExprStatement(buildPushFunctionCall(valNode->var.getVarRefExp()));
}

SgExpression* buildPopFunctionCall(SgType* type)
{
    return buildFunctionCallExp("pop< " + get_type_name(type) + " >",
            type, SageBuilder::buildExprListExp());
}

SgStatement* buildPopStatement(SgType* type)
{
    return buildExprStatement(buildPopFunctionCall(type));
}


SgStatement* buildRestorationStmt(ValueNode* node)
{
    SgType* type = node->getType();
	SgExpression* popFunc = buildPopFunctionCall(type);
    return buildExprStatement(buildAssignOp(buildVariable(node), popFunc));
}

SgStatement* buildAssignOpertaion(ValueNode* lhs, ValueNode* rhs)
{
    SgExpression* expr;
    // If rhs is NULL, it's an assignment to itself, like a_1 = a;
    if (rhs == NULL)
        expr = lhs->var.getVarRefExp();
    else
        expr = buildVariable(rhs);
    return buildExprStatement(buildAssignOp(buildVariable(lhs), expr));
}

SgStatement* buildOperationStatement(
        ValueNode* result,
        VariantT type,
        ValueNode* lhs,
        ValueNode* rhs)
{
    //SgExpression* resExpr = buildVariable(result);
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

    // For ++ and -- operators, no assignment is needed.
    if (type == V_SgMinusMinusOp || type == V_SgPlusPlusOp)
        return buildExprStatement(opExpr);
    
    return buildExprStatement(buildAssignOp(buildVariable(result), opExpr));
}

} // end of Backstroke
