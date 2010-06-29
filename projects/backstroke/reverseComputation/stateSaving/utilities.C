#include "utilities.h"
#include <boost/foreach.hpp>

using namespace std;

#define foreach BOOST_FOREACH

#define ISZERO(value, ValType) \
    if (ValType* val = is##ValType(value)) \
return val->get_value() == 0;

// Return if the value in a SgValueExp object is zero.
bool isZero(SgValueExp* value)
{
    if (!value)
        return true;
    //ROSE_ASSERT(false);

    ISZERO(value, SgBoolValExp);
    ISZERO(value, SgCharVal);
    ISZERO(value, SgDoubleVal);
    ISZERO(value, SgEnumVal);
    ISZERO(value, SgFloatVal);
    ISZERO(value, SgIntVal);
    ISZERO(value, SgLongDoubleVal);
    ISZERO(value, SgLongIntVal);
    ISZERO(value, SgLongLongIntVal);
    ISZERO(value, SgShortVal);
    ISZERO(value, SgUnsignedCharVal);
    ISZERO(value, SgUnsignedIntVal);
    ISZERO(value, SgUnsignedLongLongIntVal);
    ISZERO(value, SgUnsignedLongVal);
    ISZERO(value, SgUnsignedShortVal);

    ROSE_ASSERT(false);
    return true;
}

// Reverse the Sgop_mode from prefix to postfix, or vice versa.
SgUnaryOp::Sgop_mode reverseOpMode(SgUnaryOp::Sgop_mode mode)
{
    if (mode == SgUnaryOp::prefix)
        return SgUnaryOp::postfix;
    else
        return SgUnaryOp::prefix;
}

// Check if there is another used variable with the same name in the current scope.
// If yes, alter the name until it does not conflict with any other variable name.
void validateName(string& name, SgNode* root)
{
    Rose_STL_Container<SgNode*> ref_list = NodeQuery::querySubTree(root, V_SgVarRefExp);
    foreach (SgNode* node, ref_list)
    {
        SgVarRefExp* var_ref = isSgVarRefExp(node);
        ROSE_ASSERT(var_ref);
        if (var_ref->get_symbol()->get_name() == name)
        {
            name += "_";
            validateName(name, root);
            break;
        }
    }
}

// Identify if two variables are the same. A variable may be a SgVarRefExp object
// or a SgArrowExp object.
bool areSameVariable(SgExpression* exp1, SgExpression* exp2)
{
    SgVarRefExp* var_ref1 = isSgVarRefExp(exp1);
    SgVarRefExp* var_ref2 = isSgVarRefExp(exp2);
    if (var_ref1 && var_ref2)
        return var_ref1->get_symbol() == var_ref2->get_symbol();

    SgArrowExp* arrow_exp1 = isSgArrowExp(exp1);
    SgArrowExp* arrow_exp2 = isSgArrowExp(exp2);
    if (arrow_exp1 && arrow_exp2)
        return areSameVariable(arrow_exp1->get_lhs_operand(), arrow_exp2->get_lhs_operand()) &&
         areSameVariable(arrow_exp1->get_rhs_operand(), arrow_exp2->get_rhs_operand());

    return false;
}

// If the expression contains the given variable
bool containsVariable(SgExpression* exp, SgExpression* var)
{
    Rose_STL_Container<SgNode*> exp_list = NodeQuery::querySubTree(exp, V_SgExpression);
    foreach (SgNode* node, exp_list)
        if (areSameVariable(isSgExpression(node), var))
            return true;
    return false;
}


// Return whether a basic block contains a break statement. 
bool hasBreakStmt(SgBasicBlock* body)
{
    ROSE_ASSERT(body);

    if (body->get_statements().empty())
        return false;

    // Recursively retrieve the last SgBasicBlock statement in case of {...{...{...}}}.
    SgStatement* stmt = body->get_statements().back();
    SgBasicBlock* another_body = isSgBasicBlock(stmt);
    while (another_body)
    {
        body = another_body;
        another_body = isSgBasicBlock(another_body->get_statements().back());
    }
    return isSgBreakStmt(body->get_statements().back());
}

// If two expressions can be reorderd (in other word, reordering does not change the result).
bool canBeReordered(SgExpression* exp1, SgExpression* exp2)
{
    return false;
}
