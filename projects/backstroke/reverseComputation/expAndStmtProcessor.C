#include "eventReverser.h"
#include "utilities/utilities.h"
#include <boost/tuple/tuple.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#define foreach BOOST_FOREACH

using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;

ExpPair EventReverser::processUnaryOp(SgUnaryOp* unary_op)
{
    SgExpression *operand = unary_op->get_operand();
    SgExpression *fwd_operand_exp, *rvs_operand_exp;
    tie(fwd_operand_exp, rvs_operand_exp) = instrumentAndReverseExpression(operand);

    // The default forward version should instrument its operand.
    // For example, ++(a = 5), where a is a state variable.
    // Its forward expression is like ++(s = a, a = 5), where s is for state saving.
    
    SgUnaryOp* fwd_exp = isSgUnaryOp(copyExpression(unary_op));
    fwd_exp->set_operand(fwd_operand_exp);

    ////// If the left-hand side of the assign-op is the state
    ////// (note that only in this situation do we consider to reverse this expression)
    //if (SgExpression* model_var = isStateVar(unary_op->get_operand()))
    {
        // Make sure the type is integer type.
        if (operand->get_type()->isIntegerType())
        {
            // Note that transform ++a into a-- makes a lvalue expression a rvalue one, 
            // which may bring trouble in this recursive way. For example, what is the
            // reverse expression of ++++a?

            // ++ and -- can both be reversed without state saving
            if (SgPlusPlusOp* pp_op = isSgPlusPlusOp(unary_op))
                return ExpPair(
                        fwd_exp,
                        buildMinusMinusOp(
                            rvs_operand_exp, 
                            BackstrokeUtility::reverseOpMode(pp_op->get_mode())));

            if (SgMinusMinusOp* mm_op = isSgMinusMinusOp(unary_op))
                return ExpPair(
                        fwd_exp,
                        buildPlusPlusOp(
                            rvs_operand_exp, 
                            BackstrokeUtility::reverseOpMode(mm_op->get_mode())));
        }
        // If the type is float point type (float & double), we use state saving.
        else if (operand->get_type()->isFloatType())
        {
            if (isSgPlusPlusOp(unary_op) || isSgMinusMinusOp(unary_op))
            {
                SgExpression* fwd_exp = buildBinaryExpression<SgCommaOpExp>(
                        pushFloatVal(copyExpression(operand)),
                        fwd_exp);

                SgExpression* rvs_exp = buildBinaryExpression<SgAssignOp>(
                        copyExpression(operand),
                        popFloatVal());

                return ExpPair(fwd_exp, rvs_exp);
            }
        }
    }

    SgUnaryOp* rvs_exp = isSgUnaryOp(copyExpression(unary_op));
    rvs_exp->set_operand(rvs_operand_exp);

    return ExpPair(fwd_exp, rvs_exp);
}

ExpPair EventReverser::processBinaryOp(SgBinaryOp* bin_op)
{
    // To deal with the short circuit problem, we set a flag to begin FIFO, end LIFO.
    if (isSgAndOp(bin_op) || isSgOrOp(bin_op))
        beginFIFO();   // Flags are FIFO now

    // A binary operation contains two expressions which need to be processed first.
    SgExpression *fwd_lhs_exp, *fwd_rhs_exp, *rvs_lhs_exp, *rvs_rhs_exp;
    tie(fwd_lhs_exp, rvs_lhs_exp) = instrumentAndReverseExpression(bin_op->get_lhs_operand());
    tie(fwd_rhs_exp, rvs_rhs_exp) = instrumentAndReverseExpression(bin_op->get_rhs_operand());

    // The default forward version. Unless state saving is needed, we use the following one.
    SgBinaryOp* fwd_exp = isSgBinaryOp(copyExpression(bin_op));
    setLhsOperand(fwd_exp, fwd_lhs_exp);
    setRhsOperand(fwd_exp, fwd_rhs_exp);

    // For binary operations which don't modify the value of any variable, their 
    // reverse expressions should reorder the evaluation of each operand, in case that
    // reordering leads to different result. For example, ++a + (a *= 2)  ->  (a = pop()) + a--.  
    // Therefore, we first check if the order of evaluations matters. If yes, for commutative
    // operations, like add, sub(can be regarded as add), multiply, etc, we can just reorder their
    // operands, and for other operations, we may have to save the value, and use comma operator to 
    // reorder the evaluation of their operands. If no, we can just remain the original operation,
    // but replace its operands with reverse ones.
    if (    isSgAddOp(bin_op) ||
            //isSgAndOp(bin_op) ||
            isSgBitAndOp(bin_op) ||
            isSgBitOrOp(bin_op) ||
            isSgBitXorOp(bin_op) ||
            isSgCommaOpExp(bin_op) ||
            isSgDivideOp(bin_op) ||
            isSgEqualityOp(bin_op) ||
            isSgGreaterOrEqualOp(bin_op) ||
            isSgGreaterThanOp(bin_op) ||
            isSgLessOrEqualOp(bin_op) ||
            isSgLessThanOp(bin_op) ||
            isSgLshiftOp(bin_op) ||
            isSgModOp(bin_op) ||
            isSgMultiplyOp(bin_op) ||
            isSgNotEqualOp(bin_op) ||
            //isSgOrOp(bin_op) ||
            isSgRshiftOp(bin_op) ||
            isSgSubtractOp(bin_op))
    {
#if 1
        if (BackstrokeUtility::canBeReordered(rvs_lhs_exp, rvs_rhs_exp))
        {
            SgBinaryOp* rvs_exp = isSgBinaryOp(copyExpression(bin_op));
            rvs_exp->set_lhs_operand(rvs_lhs_exp);
            rvs_exp->set_rhs_operand(rvs_rhs_exp);
            return ExpPair(fwd_exp, rvs_exp);
        }
        else
        {
            // The following operations are commutative.
            if (    isSgAddOp(bin_op) ||
                    //isSgAndOp(bin_op) ||
                    isSgBitAndOp(bin_op) ||
                    isSgBitOrOp(bin_op) ||
                    isSgBitXorOp(bin_op) ||
                    isSgEqualityOp(bin_op) ||
                    isSgMultiplyOp(bin_op) ||
                    isSgNotEqualOp(bin_op))
            {
                // In those cases, just reorder its two operands.
                SgBinaryOp* rvs_exp = isSgBinaryOp(copyExpression(bin_op));
                rvs_exp->set_lhs_operand(rvs_rhs_exp);
                rvs_exp->set_rhs_operand(rvs_lhs_exp);
                return ExpPair(fwd_exp, rvs_exp);
            }

            if (isSgSubtractOp(bin_op))
                return ExpPair(fwd_exp, buildBinaryExpression<SgAddOp>(
                            buildUnaryExpression<SgMinusOp>(rvs_rhs_exp), 
                            rvs_lhs_exp));
            if (isSgGreaterOrEqualOp(bin_op))
                return ExpPair(fwd_exp, buildBinaryExpression<SgLessOrEqualOp>(rvs_rhs_exp, rvs_lhs_exp));
            if (isSgGreaterThanOp(bin_op))
                return ExpPair(fwd_exp, buildBinaryExpression<SgLessThanOp>(rvs_rhs_exp, rvs_lhs_exp));
            if (isSgLessOrEqualOp(bin_op))
                return ExpPair(fwd_exp, buildBinaryExpression<SgGreaterOrEqualOp>(rvs_rhs_exp, rvs_lhs_exp));
            if (isSgLessThanOp(bin_op))
                return ExpPair(fwd_exp, buildBinaryExpression<SgGreaterThanOp>(rvs_rhs_exp, rvs_lhs_exp));

            // The remain binary operations are those which we have to store 
            // their values to make them reversable. For example, the instumented
            // and reverse expression of  a / b  are  push(a / b)  and
            // (r(b), r(a), pop())  seperately. Note that r(b) or r(a) may also 
            // push values so we have to make the value contain FIFO temporarily.

            // UPDATED: In the situation above, we don't have to store the value of the 
            // expression. The return value of the reverse expression does not have to be
            // the same as the original expression.
        }
#endif
    }

    // If the left-hand side of the assign-op is the state
    // (note that only in this situation do we consider to reverse this expression)
#if 0
    if (SgExpression* model_var = isStateVar(bin_op->get_lhs_operand()))
#endif
    {
        // The operators +=, -=, *=, <<=, ^= can be reversed without state saving,
        // ////////////// if the rhs operand is a constant value or state variable.
        // (*= and <<= may be optional, since the possibility of overflow).
        // Also note that the expression in the 'sizeof' operator will not be evaluated,
        // which we don't have to process. For example, i += sizeof(++i) equals to i += sizeof(i)

        SgExpression* lhs_operand = bin_op->get_lhs_operand();
        SgExpression* rhs_operand = bin_op->get_rhs_operand();

        // Make sure the type is integer type.
        if (lhs_operand->get_type()->isIntegerType())
        {
            // Now try to reverse this operation whatever the rhs operand is.
#if 0
            if (isStateVar(rhs_operand) || 
                    isSgValueExp(rhs_operand) ||
                    isSgSizeOfOp(rhs_operand))
#endif
            {
                // We must make sure that the rhs operand does not contain the lhs operand.
                // Or else, this operation is not constructive. For example, a += a or a += a + b.
                // This can also be done by def-use analysis.

                // Note that sometimes we don't save a local variable. Even for a state variable, we may not 
                // restore it everytime it is modified. This can make the constructive operations not 
                // constructive. One solution is that we add a prerequisite to the following transformation
                // which needs that all variables except modified one need to exist or contain the proper
                // value.

                bool constructive = true;
                Rose_STL_Container<SgNode*> node_list = NodeQuery::querySubTree(rhs_operand, V_SgExpression);
                foreach (SgNode* node, node_list)
                {
                    SgExpression* exp = isSgExpression(node);
                    ROSE_ASSERT(exp);
                    if (BackstrokeUtility::areSameVariable(exp, lhs_operand))
                    {
                        constructive = false;
                        break;
                    }
                }

                if (constructive)
                {
                    if (isSgPlusAssignOp(bin_op))
                        return ExpPair(
                                fwd_exp,
                                buildBinaryExpression<SgMinusAssignOp>(
                                    rvs_lhs_exp, 
                                    rvs_rhs_exp));

                    if (isSgMinusAssignOp(bin_op))
                        return ExpPair(
                                fwd_exp,
                                buildBinaryExpression<SgPlusAssignOp>(
                                    rvs_lhs_exp, 
                                    rvs_rhs_exp));

                    if (isSgXorAssignOp(bin_op))
                        return ExpPair(
                                fwd_exp,
                                buildBinaryExpression<SgXorAssignOp>(
                                    rvs_lhs_exp, 
                                    rvs_rhs_exp));

                }
            }

            // FIXME
            // Reverse constructive assignment, like a = a + b (if a is a variable, it's equal to
            // a += b. Leave it to preprocessor or handle it now?), a = b - a. The reverse expressions
            // are a -= b and a = b - a. For integer plus and minus, order does not matter, which means
            // a = (a + b) + c is also constructive which equals to a = a + (b + c).
            // We still don't consider the following case: a = 2 * a + b, although it can be reversed as
            // a = (a - b) / 2 if there is no overflow. This issue will be handled in the future.

            // Note that sometimes we don't save a local variable. Even for a state variable, we may not 
            // restore it everytime it is modified. This can make the constructive operations not 
            // constructive. One solution is that we add a prerequisite to the following transformation
            // which needs that all variables except modified one need to exist or contain the proper
            // value.

            if (isSgAssignOp(bin_op))
            {
                // We have to record the sign of each variable.
                typedef pair<SgExpression*, bool> VarWithSign;
                vector<VarWithSign> vars;

                // The following loop get all variables with their signs in the add/subtract expression.
                queue<VarWithSign> to_process;
                to_process.push(VarWithSign(rvs_rhs_exp, true));
                while (!to_process.empty())
                {
                    VarWithSign var = to_process.front();
                    SgExpression* exp = var.first;
                    bool is_plus = var.second;
                    to_process.pop();

                    if (SgAddOp* add_op = isSgAddOp(exp))
                    {
                        to_process.push(VarWithSign(add_op->get_lhs_operand(), is_plus));
                        to_process.push(VarWithSign(add_op->get_rhs_operand(), is_plus));
                    }
                    else if (SgSubtractOp* sub_op = isSgSubtractOp(exp))
                    {
                        to_process.push(VarWithSign(sub_op->get_lhs_operand(), is_plus));
                        to_process.push(VarWithSign(sub_op->get_rhs_operand(), !is_plus));
                    }
                    else if (SgUnaryAddOp* plus_op = isSgUnaryAddOp(exp))
                        to_process.push(VarWithSign(plus_op->get_operand(), is_plus));
                    else if (SgMinusOp* minus_op = isSgMinusOp(exp))
                        to_process.push(VarWithSign(minus_op->get_operand(), !is_plus));
                    else
                        vars.push_back(var);
                }

                int count = 0;
                size_t index;
                bool constructive = true;
                for (size_t i = 0; i < vars.size(); ++i)
                {
                    if (BackstrokeUtility::areSameVariable(vars[i].first, lhs_operand))
                    {
                        ++count;
                        index = i;
                    }
                    else if (BackstrokeUtility::containsVariable(vars[i].first, lhs_operand))
                    {
                        constructive = false;
                        break;
                    }
                }

                // Only if the count of lhs operand in rhs operand is 1 can this expression be 
                // constructive. For example, a = a + a + b is not constructive (considering overflow).
                if (constructive && count == 1)
                {
                    // The form a = b - a, the reverse expression is the same.
                    if (!vars[index].second)
                    {
                        SgExpression* rvs_exp = buildBinaryExpression<SgAssignOp>(
                                copyExpression(lhs_operand),
                                rvs_rhs_exp);
                        return ExpPair(fwd_exp, rvs_exp);
                    }


                    // Concatenate all other variables.
                    SgExpression* rvs_exp = NULL;
                    for (size_t i = 0; i < vars.size(); ++i)
                    {
                        if (i == index) continue;

                        if (rvs_exp == NULL)
                        {
                            rvs_exp = copyExpression(vars[i].first);
                            if (!vars[i].second)
                                rvs_exp = buildUnaryExpression<SgMinusOp>(rvs_exp);
                        }
                        else
                        {
                            if (vars[i].second)
                                rvs_exp = buildBinaryExpression<SgAddOp>(
                                        rvs_exp, 
                                        copyExpression(vars[i].first));
                            else
                                rvs_exp = buildBinaryExpression<SgSubtractOp>(
                                        rvs_exp, 
                                        copyExpression(vars[i].first));
                        }
                    }

                    if (vars[index].second)
                    {
                        // a = b + a  ->  a = a - b
                        if (rvs_exp)
                            rvs_exp = buildBinaryExpression<SgSubtractOp>(
                                    copyExpression(lhs_operand), 
                                    rvs_exp);
                        // a = +a
                        else
                            rvs_exp = copyExpression(lhs_operand);
                    }

                    rvs_exp = buildBinaryExpression<SgAssignOp>(
                            copyExpression(lhs_operand),
                            rvs_exp);

                    return ExpPair(fwd_exp, rvs_exp);
                }
            }
        }

        // The following assignment operations which alter the value of the lhs operand
        // can be reversed by state saving
        if (    isSgAssignOp(bin_op) ||
                isSgPlusAssignOp(bin_op) ||
                isSgMinusAssignOp(bin_op) ||
                isSgMultAssignOp(bin_op) ||
                isSgDivAssignOp(bin_op) ||
                isSgModAssignOp(bin_op) ||
                isSgIorAssignOp(bin_op) ||
                isSgAndAssignOp(bin_op) ||
                isSgXorAssignOp(bin_op) ||
                //isSgPointerAssignOp(bin_op) ||          // what is this?
                isSgLshiftAssignOp(bin_op) ||
                isSgRshiftAssignOp(bin_op))
        {
            // FIXME An important analysis should be performed here based on the 
            // value graph built. If we can track the destroyed value which is held
            // by another live variable, we don't have to push it into stack, but 
            // directly assign that value to this assignee. The "swap" example is a 
            // classic one: 
            //     void swap(int& a, int& b) { int t = a; a = b; b = t; }
            // It's reverse function is like:
            //     void swap(int& a, int& b) { int t = b; b = a; a = t; }
                 
            SgExpression* rvs_exp = NULL;
            // The following function returns the expression which holds the proper value.
            //if (SgExpression* exp = valueCanBeRecovered())
            if (false)
            {
                fwd_exp = isSgBinaryOp(copyExpression(bin_op));
                rvs_exp = buildBinaryExpression<SgAssignOp>(
                        copyExpression(lhs_operand),
                        NULL);//exp);
            }
            else if (toSave(lhs_operand))
            {
                fwd_exp = buildBinaryExpression<SgCommaOpExp>(
                        pushIntVal(copyExpression(lhs_operand)),
                        fwd_exp);
                rvs_exp = buildBinaryExpression<SgAssignOp>(
                        copyExpression(lhs_operand),
                        popIntVal());
            }

            // If the rhs operand expression can be reversed, we have to deal with 
            // both sides at the same time. For example: b = ++a, where a and b are 
            // both state variables.
            if (rvs_exp && rvs_rhs_exp)
                rvs_exp = buildBinaryExpression<SgCommaOpExp>(rvs_rhs_exp, rvs_exp);
            else if (rvs_rhs_exp)
                rvs_exp = rvs_rhs_exp;
            return ExpPair(fwd_exp, rvs_exp);
        }
    }
    // The arrow expression should be regarded as a variable.
    // FIXME What if the rhs operand is a member function?
    // FIXME (++m)->i ?
    if (isSgArrowExp(bin_op))
        return ExpPair(copyExpression(bin_op), copyExpression(bin_op));

    // logical 'and' and 'or' should be taken care of since short circuit may happen
    //if (SgAndOp* and_op = isSgAndOp(bin_op))
    if (isSgAndOp(bin_op))
    {
        if (rvs_rhs_exp == NULL)
        {
            endFIFO();     // Flags are LIFO now
            return ExpPair(fwd_exp, rvs_lhs_exp);
        }

        // Forward event should record the lhs value of the or operator.
        SgExpression* fwd_exp = buildBinaryExpression<SgAndOp>(
                putBranchFlagExp(fwd_lhs_exp),
                fwd_rhs_exp);
        endFIFO();     // Flags are LIFO now

        SgExpression* rvs_exp;
        // We also have to check if rvs_lhs_exp == NULL
        if (rvs_lhs_exp)
            // Use the flag recorded to choose whether to reverse just lhs operand or both.
            rvs_exp = buildConditionalExp(
                    checkBranchFlagExp(),
                    buildBinaryExpression<SgCommaOpExp>(rvs_rhs_exp, rvs_lhs_exp),
                    rvs_lhs_exp);
        else
            rvs_exp = buildConditionalExp(
                    checkBranchFlagExp(),
                    rvs_rhs_exp,
                    buildIntVal(0));

        return ExpPair(fwd_exp, rvs_exp);
    }

    //if (SgOrOp* or_op = isSgOrOp(bin_op))
    if (isSgOrOp(bin_op))
    {
        if (rvs_rhs_exp == NULL)
        {
            endFIFO();     // Flags are LIFO now
            return ExpPair(fwd_exp, rvs_lhs_exp);
        }

        // Forward event should record the lhs value of the or operator.
        SgExpression* fwd_exp = buildBinaryExpression<SgOrOp>(
                putBranchFlagExp(fwd_lhs_exp),
                fwd_rhs_exp);
        endFIFO();     // Flags are LIFO now

        SgExpression* rvs_exp;
        // We also have to check if rvs_lhs_exp == NULL
        if (rvs_lhs_exp)
            // Use the flag recorded to choose whether to reverse just lhs operand or both.
            rvs_exp = buildConditionalExp(
                    checkBranchFlagExp(),
                    rvs_lhs_exp,
                    buildBinaryExpression<SgCommaOpExp>(rvs_rhs_exp, rvs_lhs_exp));
        else
            rvs_exp = buildConditionalExp(
                    checkBranchFlagExp(),
                    buildIntVal(0),
                    rvs_rhs_exp);

        return ExpPair(fwd_exp, rvs_exp);
    }


    // The reverse version of an expression should still be an expression, not NULL
    // The following is the default reverse expression which is a combination
    // of reverse expressions of both sides.
    SgExpression* rvs_exp = NULL;
    if (rvs_rhs_exp && rvs_lhs_exp)
        rvs_exp = buildBinaryExpression<SgCommaOpExp>(rvs_rhs_exp, rvs_lhs_exp);
    else if (rvs_lhs_exp)
        rvs_exp = rvs_lhs_exp;
    else if (rvs_rhs_exp)
        rvs_exp = rvs_rhs_exp;

    return ExpPair(fwd_exp, rvs_exp);
}

ExpPair EventReverser::processConditionalExp(SgConditionalExp* cond_exp)
{
    // Since conditional expression is quite like a if statement,
    // we will reverse it using flags like reversing if statement
    SgExpression *fwd_cond_exp, *fwd_true_exp, *fwd_false_exp;
    SgExpression *rvs_cond_exp, *rvs_true_exp, *rvs_false_exp;

    tie(fwd_cond_exp, rvs_cond_exp) = instrumentAndReverseExpression(cond_exp->get_conditional_exp());

    beginFIFO();   // Flags are FIFO now
    fwd_cond_exp = putBranchFlagExp(fwd_cond_exp);
    tie(fwd_true_exp, rvs_true_exp) = instrumentAndReverseExpression(cond_exp->get_true_exp());
    tie(fwd_false_exp, rvs_false_exp) = instrumentAndReverseExpression(cond_exp->get_false_exp());
    endFIFO();     // Flags are LIFO now

    //fwd_true_exp = buildBinaryExpression<SgCommaOpExp>(putBranchFlagExp(true), fwd_true_exp);
    //fwd_false_exp = buildBinaryExpression<SgCommaOpExp>(putBranchFlagExp(false),fwd_false_exp);


    // Since the conditional operator does not allow NULL expressions appear to its 2nd and 3rd
    // operand, we have to pull a place holder which does nothing if the reverse expression of that
    // operand is NULL. 0 can represents arithmetic or pointer types.
    // FIXME If the operand is of type void or structure type???
    // If its return value does not matter, can we return 0 always? 
    // (For example,     
    //        bool_val ? foo1(i) : foo2()        
    // where foo1 and foo2 return void. Then its reverse expression is
    //        flag? (foo1_rev(i), 0) : 0
    if (!rvs_true_exp) rvs_true_exp = buildIntVal(0);
    if (!rvs_false_exp) rvs_false_exp = buildIntVal(0);

    SgExpression* fwd_exp = buildConditionalExp(fwd_cond_exp, fwd_true_exp, fwd_false_exp); 
    SgExpression* rvs_exp = buildConditionalExp(checkBranchFlagExp(), rvs_true_exp, rvs_false_exp);
    if (rvs_cond_exp)
        rvs_exp = buildBinaryExpression<SgCommaOpExp>(rvs_exp, rvs_cond_exp);

    // FIXME:!!! the reverse version is not right now! Note the return value!
    return ExpPair(fwd_exp, rvs_exp);
}

ExpPair EventReverser::processFunctionCallExp(SgFunctionCallExp* func_exp)
{
    // When there is a function call in the event function, we should consider to
    // instrument and reverse this function.
    // First, check if the parameters of this function call contain any model state.
    bool needs_reverse = false;
    foreach (SgExpression* exp, func_exp->get_args()->get_expressions())
    {
        if (isStateVar(exp)) 
        {
            needs_reverse = true;
            break;
        }
    }
    if (!needs_reverse) 
    {
        // If the function call does not modify model's state, we can 
        // just reverse its parameters
        SgExpression *fwd_exp = NULL, *rvs_exp = NULL;
        vector<SgExpression*> fwd_args;
        foreach (SgExpression* exp, func_exp->get_args()->get_expressions())
        {
            SgExpression *fwd_arg, *rvs_arg;
            tie(fwd_arg, rvs_arg) = instrumentAndReverseExpression(exp);
            fwd_args.push_back(fwd_arg);
            rvs_exp = (rvs_exp == NULL) ?
                rvs_arg :
                buildBinaryExpression<SgCommaOpExp>(rvs_arg, rvs_exp);
        }
        // The forward expression is still a function call, while the reverse expression
        // just reverse it's parameters. 
        //SgExprListExp* fwd_para_list = buildExprListExp_nfi(fwd_args);
        //SgExprListExp* fwd_para_list = buildExprListExp();


        // FIXME We will use new interface buildExprListExp(const vector<SgExpression*>&) later.
        SgExprListExp* fwd_para_list = new SgExprListExp();
        setOneSourcePositionForTransformation(fwd_para_list);
        foreach (SgExpression* e, fwd_args)
            appendExpression(fwd_para_list, e);

        fwd_exp = buildFunctionCallExp(
                func_exp->getAssociatedFunctionSymbol(), 
                fwd_para_list);

        // FIXME Temporarily add this to make a non-parameter function call be reversed
        if (rvs_exp == NULL)
            rvs_exp = copyExpression(fwd_exp);
        return ExpPair(fwd_exp, rvs_exp);
    }


    // Then we have to reverse the function called
    SgFunctionDeclaration* func_decl = func_exp->getAssociatedFunctionDeclaration();
    EventReverser func_generator(func_decl);
    map<SgFunctionDeclaration*, FuncDeclPair> func_pairs = func_generator.outputFunctions();
    if (func_processed_.count(func_decl) == 0)
	{
		pair<SgFunctionDeclaration*, FuncDeclPair> originalAndInstrumented;
        foreach(originalAndInstrumented, func_pairs)
		{
            output_func_pairs_.insert(originalAndInstrumented);
		}
	}
	
    SgFunctionDeclaration* fwd_func = func_pairs[func_decl].first;
    SgFunctionDeclaration* rvs_func = func_pairs[func_decl].second;

    SgExprListExp* fwd_args = buildExprListExp();
    SgExprListExp* rvs_args = buildExprListExp();
    foreach (SgExpression* exp, func_exp->get_args()->get_expressions())
    {
        SgExpression *fwd_exp, *rvs_exp;
        fwd_exp = copyExpression(exp);
        rvs_exp = copyExpression(exp);
        fwd_args->append_expression(fwd_exp);
        rvs_args->append_expression(rvs_exp);
    }

    func_processed_.insert(func_decl);

    return ExpPair(
            buildFunctionCallExp(fwd_func->get_name(), fwd_func->get_orig_return_type(), fwd_args),
            buildFunctionCallExp(rvs_func->get_name(), rvs_func->get_orig_return_type(), rvs_args));
}




StmtPair EventReverser::processExprStatement(SgExprStatement* exp_stmt)
{   
    // For a simple expression statement, we just process its 
    // expression then build new statements for the expression pair.
    SgExpression* exp = exp_stmt->get_expression();
    SgExpression *fwd_exp, *rvs_exp;
    tie(fwd_exp, rvs_exp) = instrumentAndReverseExpression(exp);

    if (fwd_exp && rvs_exp)
        return StmtPair(
                buildExprStatement(fwd_exp), 
                buildExprStatement(rvs_exp));
    else if (fwd_exp)
        return StmtPair(
                buildExprStatement(fwd_exp), 
                NULL);
    else
        return NULL_STMT_PAIR;
}


StmtPair EventReverser::processBasicBlock(SgBasicBlock* body)
{
	// A basic block normally contains a group of statements.
	// We process them one by one. Note that the reverse version
	// of the basic block has its reverse statements in reverse order.
	SgBasicBlock* fwd_body = buildBasicBlock();
	SgBasicBlock* rvs_body = buildBasicBlock();

	vector<SgStatement*> scopeExitStores;
	vector<SgStatement*> localVarDeclarations;

	foreach(SgStatement* s, body->get_statements())
	{
		// Put the declarations of local variables in the beginning of reverse
		// basic block and retrieve their values. Store values of all local
		// variables in the end of of forward basic block.
		// Also refer to the function "processVariableDeclaration"
		if (SgVariableDeclaration * var_decl = isSgVariableDeclaration(s))
		{
			SgStatement *store_var, *decl_var;
			tie(store_var, decl_var) = pushAndPopLocalVar(var_decl);

			scopeExitStores.push_back(store_var);
			localVarDeclarations.push_back(decl_var);
		}

		SgStatement *fwd_stmt, *rvs_stmt;
		tie(fwd_stmt, rvs_stmt) = instrumentAndReverseStatement(s);

		//The return statement should go at the very end of the forward statement
		//after the variables that exit scope have been stored
		if (isSgReturnStmt(fwd_stmt))
		{
			scopeExitStores.push_back(fwd_stmt);
		}
		else if (fwd_stmt)
		{
			fwd_body->append_statement(fwd_stmt);
		}

		//Insert the reverse statement
		if (isSgBreakStmt(rvs_stmt) || isSgContinueStmt(rvs_stmt))
		{
			// It's is unclear whether these special statements
			// should be added in the reverse version. In switch
			// statement, 'break' should be retained.
			//rvs_body->append_statement(rvs_stmt);
			ROSE_ASSERT(false);
			delete rvs_stmt;
			break;
		}
		else if (isSgReturnStmt(rvs_stmt))
		{
			//The reverse of a return statement is a null statement since the return statement
			//can only come at the end of a function. Hence, this case should never occur.
			ROSE_ASSERT(false);
		}
		else if (rvs_stmt)
		{
			rvs_body->prepend_statement(rvs_stmt);
		}
	}

	//Before exiting scope, store all local variables
	foreach(SgStatement* stmt, scopeExitStores)
		fwd_body->append_statement(stmt);

	//Restore all local variables upon returning into the scope
	foreach(SgStatement* stmt, localVarDeclarations)
		rvs_body->prepend_statement(stmt);

	return StmtPair(fwd_body, rvs_body);
}


StmtPair EventReverser::processVariableDeclaration(SgVariableDeclaration* var_decl)
{
    // A local variable declaration may have side effect, like 
    //    int i = ++m->i;
    // Its reverse statement actually has two parts: declaration and reverse statement of 
    // its initialization part. The first part should always be put in the beginning of a 
    // basic block, but the second part should be positioned in reverse order just like
    // other reverse statements. We just return the second part here, and generate the
    // first part in the basic block processor. 

    // I found that the number of variables of each variable declaration is always 1,
    // since Rose will seperate multi-vars into several declarations.
    // For example,  
    //           int i, j, k;
    // will be turned into:
    //           int i;
    //           int j;
    //           int k;
    // Therefore, currently we just use the first element of its variables returned below.

    const SgInitializedNamePtrList& names = var_decl->get_variables();
    ROSE_ASSERT(names.size() == 1);

    SgInitializedName* init_name = names[0];
    //SgVariableDefinition* var_def = var_decl->get_definition();
    SgAssignInitializer* init = isSgAssignInitializer(init_name->get_initializer());

    // If not defined
    if (!init)
        return StmtPair(copyStatement(var_decl), NULL);
        //return StmtPair(copyStatement(var_decl), copyStatement(var_decl));

    SgExpression *fwd_exp, *rvs_exp;
    tie(fwd_exp, rvs_exp) = instrumentAndReverseExpression(init->get_operand());

    SgStatement *fwd_stmt, *rvs_stmt = NULL;
    fwd_stmt = buildVariableDeclaration(
            init_name->get_name(),
            init_name->get_type(),
            buildAssignInitializer(fwd_exp));
#if 1
    if (rvs_exp)
        rvs_stmt = buildExprStatement(rvs_exp);
#else
    rvs_stmt = buildVariableDeclaration(
            init_name->get_name(),
            init_name->get_type(),
            rvs_exp ? buildAssignInitializer(rvs_exp) : NULL);
#endif
    return StmtPair(fwd_stmt, rvs_stmt);
}

StmtPair EventReverser::processIfStmt(SgIfStmt* if_stmt)
{
    // For if statement, we have to use the external flags to save which 
    // branch is taken. The reverse version check these flags to select
    // the proper branch.
    // Currently we use a stack to store those flags in case that the if statement is 
    // inside a loop. The stack is written in C. For "if()else if()... else",
    // the current strategy does not make the optimal space use.
    // FIXME: Note that sometimes we don't have to use flags. For example, 
    // if the condition is decided by model state.

    SgStatement *fwd_true_body, *fwd_false_body;
    SgStatement *rvs_true_body, *rvs_false_body;

    SgStatement* true_body = if_stmt->get_true_body();
    SgStatement* false_body = if_stmt->get_false_body();

    tie(fwd_true_body, rvs_true_body) = instrumentAndReverseStatement(true_body);
    tie(fwd_false_body, rvs_false_body) = instrumentAndReverseStatement(false_body);

    SgBasicBlock* fwd_true_block_body =  isSgBasicBlock(fwd_true_body);
    SgBasicBlock* fwd_false_block_body = isSgBasicBlock(fwd_false_body);

    // putBranchFlag is used to store which branch is chosen
    if (fwd_true_block_body == NULL)
        fwd_true_block_body = buildBasicBlock(fwd_true_body);
    fwd_true_block_body->append_statement(putBranchFlagStmt(true));

    if (fwd_false_block_body == NULL)
        fwd_false_block_body = buildBasicBlock(fwd_false_body);
    fwd_false_block_body->append_statement(putBranchFlagStmt(false));


    SgExpression* rvs_cond_exp = checkBranchFlagExp();

    // Do not switch the position of the following two statements to the one above;
    // make sure the current flag is used before generating new statement.
    SgStatement* cond = if_stmt->get_conditional();
    SgStatement *fwd_cond_stmt, *rvs_post_stmt;
    tie(fwd_cond_stmt, rvs_post_stmt) = instrumentAndReverseStatement(cond);


    // In C++ standard, the condition part of a selection statement can have the following
    // two forms:
    //     expression
    //     type-specifier-seq declarator = assignment-expression
    // Sometimes we want to use the local variable declared in condition, when we have to store
    // and retrieve its value in forward and reverse if statement.
    
    if (SgVariableDeclaration* var_decl = isSgVariableDeclaration(cond))
    {
        SgStatement *store_var, *decl_var;
        tie(store_var, decl_var) = pushAndPopLocalVar(var_decl);

        fwd_true_block_body->append_statement(store_var);
        fwd_false_block_body->append_statement(copyStatement(store_var));

        if (SgBasicBlock* block = isSgBasicBlock(rvs_true_body))
            block->prepend_statement(decl_var);
        else
            rvs_true_body = buildBasicBlock(decl_var, rvs_true_body);

        decl_var = copyStatement(decl_var);

        if (SgBasicBlock* block = isSgBasicBlock(rvs_false_body))
            block->prepend_statement(decl_var);
        else
            rvs_false_body = buildBasicBlock(decl_var, rvs_false_body);
    }

    return StmtPair(
            buildIfStmt(fwd_cond_stmt, fwd_true_block_body, fwd_false_block_body),
            buildBasicBlock(
                buildIfStmt(rvs_cond_exp, rvs_true_body, rvs_false_body),
                rvs_post_stmt));
}

StmtPair EventReverser::processForInitStatement(SgForInitStatement* for_init_stmt)
{
    // In C standard, the initialization part in for loop can either be a <declaration> or
    // <expression;>. However, it's only regarded as a statement in Rose.

    SgForInitStatement* fwd_stmt = NULL;
    SgBasicBlock* rvs_stmt = NULL;

    // If the initialization part is of form <expression;>, it is regarded as ONE expression statement.
    // Or else, if it's <declaration>, there will be several statements depending on how many variables
    // are declared. For example, 
    //     for (int i = 0, j = 0;;)
    // there are two variables declared, hence two statements are processed below.

    foreach (SgStatement* stmt, for_init_stmt->get_init_stmt())
    {
        SgStatement *fwd_stmt_i, *rvs_stmt_i;
        tie(fwd_stmt_i, rvs_stmt_i) = instrumentAndReverseStatement(stmt);

        if (fwd_stmt == NULL)
        {
            // Since there is no builder function for SgForInitStatement, we build it by ourselves
            fwd_stmt = new SgForInitStatement();
            setOneSourcePositionForTransformation(fwd_stmt);
        }
        fwd_stmt->append_init_stmt(fwd_stmt_i);

        if (rvs_stmt_i)
        {
            if (rvs_stmt == NULL)
                rvs_stmt = buildBasicBlock();
            rvs_stmt->prepend_statement(rvs_stmt_i);
        }
    }

    return StmtPair(fwd_stmt, rvs_stmt);
}

StmtPair EventReverser::processForStatement(SgForStatement* for_stmt)
{
    // FIXME If the condition is a variable declaration: for (int i = 10; int j = i--;);
    // FIXME break and continue.
    
    SgForInitStatement* init = for_stmt->get_for_init_stmt();
    SgStatement* test = for_stmt->get_test();
    SgExpression* incr = for_stmt->get_increment();
    SgStatement* body = for_stmt->get_loop_body();

    SgStatement *fwd_init, *rvs_init, *fwd_test, *rvs_test;
    SgExpression *fwd_incr, *rvs_incr;
    SgStatement *fwd_body, *rvs_body;

    tie(fwd_init, rvs_init) = instrumentAndReverseStatement(init);
    tie(fwd_test, rvs_test) = instrumentAndReverseStatement(test);
    tie(fwd_incr, rvs_incr) = instrumentAndReverseExpression(incr);
    tie(fwd_body, rvs_body) = instrumentAndReverseStatement(body);

    // To process a for statement, we have to know the loop count. If the for statement
    // is a simple for (int i = 0; i < N; ++i) where N is an constant number, we don't have 
    // to save this count. However, if we don't know the count when reverse is, we have to 
    // record the count by ourselves.

    // if (isSimpleForLoop()) do ...

    //SgExpression* counter = getLoopCounter();

    SgStatement* fwd_stmt = buildForStatement(fwd_init, fwd_test, fwd_incr, fwd_body);
    // Add the loop counter related statements (counter declaration, increase, and store)
    // to a forward for statement.
    fwd_stmt = assembleLoopCounter(fwd_stmt);


    // We have to disorder every statement including those in for()

    // Make the reverse incr part the first statement of the body.
    if (rvs_incr)
    {
        SgStatement* rvs_incr_stmt = buildExprStatement(rvs_incr);
        if (SgBasicBlock* rvs_block_body = isSgBasicBlock(rvs_body))
            rvs_block_body->prepend_statement(rvs_incr_stmt);
        else
            rvs_body = buildBasicBlock(rvs_incr_stmt, rvs_body);
    }
    // Make the reverse test part the last statement of the body.
    if (rvs_test)
    {
        if (SgBasicBlock* rvs_block_body = isSgBasicBlock(rvs_body))
            rvs_block_body->append_statement(rvs_test);
        else
            rvs_body = buildBasicBlock(rvs_body, rvs_test);
    }

    SgStatement* rvs_stmt = NULL;
    // We don't want too many unnecessary {}  :)
    if (rvs_test == NULL && rvs_init == NULL)
        rvs_stmt = buildForLoop(rvs_body);
    else
        rvs_stmt = buildBasicBlock(rvs_test, buildForLoop(rvs_body), rvs_init);

    return StmtPair(fwd_stmt, rvs_stmt);
}

StmtPair EventReverser::processWhileStmt(SgWhileStmt* while_stmt)
{
    // We use an external counter to record the loop number.
    // In the reverse version, we use "for" loop with the counter.

    SgStatement* cond = while_stmt->get_condition();
    SgStatement* body = while_stmt->get_body();

    SgStatement *fwd_cond, *rvs_cond, *fwd_body, *rvs_body;
    tie(fwd_cond, rvs_cond) = instrumentAndReverseStatement(cond);
    tie(fwd_body, rvs_body) = instrumentAndReverseStatement(body);


    // In C++ standard, the condition part of a loop statement can have the following
    // two forms:
    //     expression
    //     type-specifier-seq declarator = assignment-expression
    // Sometimes we want to use the local variable declared in condition, when we have to store
    // and retrieve its value in forward and reverse if statement.

    if (SgVariableDeclaration* var_decl = isSgVariableDeclaration(cond))
    {
        SgStatement *store_var, *decl_var;
        tie(store_var, decl_var) = pushAndPopLocalVar(var_decl);

        if (SgBasicBlock* block = isSgBasicBlock(fwd_body))
            block->append_statement(store_var);
        else
            fwd_body = buildBasicBlock(fwd_body, store_var);

        if (SgBasicBlock* block = isSgBasicBlock(rvs_body))
            block->prepend_statement(decl_var);
        else
            rvs_body = buildBasicBlock(decl_var, rvs_body);
    }

    SgStatement* fwd_stmt = buildWhileStmt(fwd_cond, fwd_body);
    // Add the loop counter related statements (counter declaration, increase, and store)
    // to a forward while statement.
    fwd_stmt = assembleLoopCounter(fwd_stmt);

    //ROSE_ASSERT(isSgBasicBlock(isfwd_stmt

    if (rvs_cond)
    {
        if (SgBasicBlock* body = isSgBasicBlock(rvs_body))
            body->append_statement(rvs_cond);
        else if (rvs_body)
            rvs_body = buildBasicBlock(rvs_body, rvs_cond);
    }

    SgStatement* rvs_stmt = buildBasicBlock(rvs_cond, buildForLoop(rvs_body));

    return StmtPair(fwd_stmt, rvs_stmt);
}

StmtPair EventReverser::processDoWhileStmt(SgDoWhileStmt* do_while_stmt)
{
    // This is similar to while statement.

    SgStatement* cond = do_while_stmt->get_condition();
    SgStatement* body = do_while_stmt->get_body();

    SgStatement *fwd_cond, *rvs_cond, *fwd_body, *rvs_body;
    tie(fwd_cond, rvs_cond) = instrumentAndReverseStatement(cond);
    tie(fwd_body, rvs_body) = instrumentAndReverseStatement(body);

    SgStatement* fwd_stmt = buildDoWhileStmt(fwd_body, fwd_cond);
    // Add the loop counter related statements (counter declaration, increase, and store)
    // to a forward do while statement.
    fwd_stmt = assembleLoopCounter(fwd_stmt);

    if (rvs_cond != NULL)
    {
        if (SgBasicBlock* body = isSgBasicBlock(rvs_body))
            body->prepend_statement(rvs_cond);
        else if (rvs_body)
            rvs_body = buildBasicBlock(rvs_cond, rvs_body);
    }

    SgStatement* rvs_stmt = buildBasicBlock(rvs_cond, buildForLoop(rvs_body));

    return StmtPair(fwd_stmt, rvs_stmt);
}


StmtPair EventReverser::processSwitchStatement(SgSwitchStatement* switch_stmt)
{
    // FIXME If the selector is a variable declaration: switch(int i = j){}
    
    SgStatement* item_selector = switch_stmt->get_item_selector();
    SgBasicBlock* body = isSgBasicBlock(switch_stmt->get_body());

    SgBasicBlock *fwd_body, *rvs_body;
    SgExpression *fwd_item_selector_exp, *rvs_item_selector_exp;
    //SgStatement *fwd_item_selector, *rvs_item_selector;

    const SgStatementPtrList& stmts = body->get_statements();
    SgStatementPtrList fwd_stmts, rvs_stmts;

    // Sometimes we cannot process a case option instantly because of lacking break statement.
    // Then we put those corresponding reverse statements in the following wait list and process 
    // them when we meet a break or the end of switch statement.
    vector<pair<SgCaseOptionStmt*, SgStatement*> > case_wait_list;

    // Assume that after dead code elimination, if there is one 'break' in a case option
    // statement, it must be the last statement.
    foreach (SgStatement* s, stmts)
    {
        SgBasicBlock* body = NULL;

        if (SgCaseOptionStmt* case_opt_stmt = isSgCaseOptionStmt(s))
        {
            body = isSgBasicBlock(case_opt_stmt->get_body());
            ROSE_ASSERT(body);

            SgStatement *fwd_body, *rvs_body;
            tie(fwd_body, rvs_body) = instrumentAndReverseStatement(body);

            fwd_stmts.push_back(buildCaseOptionStmt(
                        case_opt_stmt->get_key(),
                        fwd_body));
            case_wait_list.push_back(make_pair(case_opt_stmt, rvs_body));
        }

        else if (SgDefaultOptionStmt* default_opt_stmt = isSgDefaultOptionStmt(s))
        { 
            body = isSgBasicBlock(default_opt_stmt->get_body());
            ROSE_ASSERT(body);

            SgStatement *fwd_body, *rvs_body;
            tie(fwd_body, rvs_body) = instrumentAndReverseStatement(body);

            fwd_stmts.push_back(buildDefaultOptionStmt(fwd_body));
            // Use NULL to express a default statement.
            case_wait_list.push_back(make_pair((SgCaseOptionStmt*)NULL, rvs_body));
        }

        // Declarations may appear in switch statement.
        if (body == NULL) continue;

        if (!BackstrokeUtility::hasBreakStmt(body))
        {
            // If the last case or default statament does not contain a break, we have to process wait list.
            if (s != stmts.back())
                continue;
        }

        // For those case options which don't have a break, we take care of them specially.
        // We have to go down until a break found or the end of the switch statement, then 
        // stack all reverse statements. 
        if (!case_wait_list.empty())
        {
            SgBasicBlock* rvs_body = buildBasicBlock();
            for (size_t i = 0; i < case_wait_list.size(); ++i)
            {
                SgCaseOptionStmt* case_opt_stmt;
                SgStatement* prev_rvs_body;
                tie(case_opt_stmt, prev_rvs_body) = case_wait_list[case_wait_list.size() - i - 1];

                // For the case option which is empty, we group it to the next non-empty one
                // to avoid code duplication.
                if ((i != 0) && isSgBasicBlock(prev_rvs_body)->get_statements().empty())
                {
                    ROSE_ASSERT(!rvs_stmts.empty());
                    rvs_stmts.insert(
                            rvs_stmts.end() - 1, 
                            buildCaseOptionStmt(
                                case_opt_stmt->get_key(),
                                NULL));
                    continue;
                }

                rvs_body->append_statement(prev_rvs_body);

                SgBasicBlock* rvs_body_copy = isSgBasicBlock(copyStatement(rvs_body));
                rvs_body_copy->append_statement(buildBreakStmt());

                if (case_opt_stmt)
                    rvs_stmts.push_back(buildCaseOptionStmt(
                                case_opt_stmt->get_key(),
                                rvs_body_copy));
                else
                    rvs_stmts.push_back(buildDefaultOptionStmt(rvs_body_copy));
            }

            // Clear unused nodes.
            for (size_t i = 0; i < case_wait_list.size(); ++i)
                delete case_wait_list[i].second;
            delete rvs_body;
            case_wait_list.clear();
        }
    }

    fwd_body = buildBasicBlock();
    foreach(SgStatement* s, fwd_stmts)
        fwd_body->append_statement(s);

    rvs_body = buildBasicBlock();
    foreach(SgStatement* s, rvs_stmts)
        rvs_body->append_statement(s); 

    ROSE_ASSERT(isSgExprStatement(item_selector));
    SgExpression* item_selector_exp = isSgExprStatement(item_selector)->get_expression();
    tie(fwd_item_selector_exp, rvs_item_selector_exp) = instrumentAndReverseExpression(item_selector_exp);

    //tie(fwd_item_selector, rvs_item_selector) = instrumentAndReverseStatement(item_selector);

    bool save_selector = true;

    // Here an analysis should be performed to detect if the value of the selector will be modified
    // inside the switch body. If not, we don't have to save it.
    //
    if (save_selector)
    {
        // Use a temporary variable to store the value passed into switch statement, and push it into stack
        // after the switch statement.
        string var_name = function_name_ + "_switch_item_" + lexical_cast<string>(counter_++);
        BackstrokeUtility::validateName(var_name, fwd_item_selector_exp);
        BackstrokeUtility::validateName(var_name, fwd_body);

        SgStatement* var_decl = buildVariableDeclaration(
                var_name, 
                buildIntType(), 
                buildAssignInitializer(fwd_item_selector_exp));

        SgStatement* fwd_item_selector = buildExprStatement(buildVarRefExp(var_name));
        SgStatement* rvs_item_selector = buildExprStatement(popIntVal());

        // We push the switch selector value into integer stack, not flag stack, since the latter only accpets 0 or 1
        // which can then be stored in a bit stack.
        // Also note that we store the selector after switch statement for correct stack access order.
        SgStatement* fwd_stmt = buildBasicBlock(
                var_decl,
                buildSwitchStatement(fwd_item_selector, fwd_body),
                buildExprStatement(pushIntVal(buildVarRefExp(var_name))));

        SgStatement* rvs_stmt = buildSwitchStatement(rvs_item_selector, rvs_body);
        if (rvs_item_selector_exp)
            rvs_stmt = buildBasicBlock(rvs_stmt, buildExprStatement(rvs_item_selector_exp));

        return StmtPair(fwd_stmt, rvs_stmt);
    }
    else
    {
        SgStatement* fwd_stmt = buildSwitchStatement(fwd_item_selector_exp, fwd_body);
        SgStatement* rvs_stmt = buildSwitchStatement(copyExpression(fwd_item_selector_exp), rvs_body);
        return StmtPair(fwd_stmt, rvs_stmt);
    }
}
