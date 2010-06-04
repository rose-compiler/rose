#ifndef REVERSE_COMPUTATION_UTILITIES_H
#define REVERSE_COMPUTATION_UTILITIES_H

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

#endif
