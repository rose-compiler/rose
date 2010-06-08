#ifndef REVERSE_COMPUTATION_UTILITIES_H
#define REVERSE_COMPUTATION_UTILITIES_H

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

#endif
