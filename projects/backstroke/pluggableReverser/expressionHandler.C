#include "processorPool.h"
#include "storage.h"
#include "expressionHandler.h"
#include "utilities/Utilities.h"
#include "utilities/CPPDefinesAndNamespaces.h"

using namespace SageInterface;
using namespace SageBuilder;
using namespace backstroke_util;

ExpPairs storeAndRestore(SgExpression* exp)
{
    ExpPairs output;

    // If an expression modifies any value, we consider to store the value 
    // before being modified and restore it in reverse event.


    // Every type can be stored using the following way.
    if (isSgPlusPlusOp(exp) || isSgMinusMinusOp(exp))
    {
        SgExpression* operand = isSgUnaryOp(exp)->get_operand();

        // For integer type, it's better to reverse it directly, not state saving.
        if (!(operand->get_type()->isIntegerType()))
        {
            SgExpression* fwd_exp = buildBinaryExpression<SgCommaOpExp>(
                    pushVal(operand, operand->get_type(), exp),
                    copyExpression(exp));
            SgExpression* rvs_exp = buildBinaryExpression<SgAssignOp>(
                    copyExpression(operand),
                    popVal(operand->get_type(), exp));
            output.push_back(ExpPair(fwd_exp, rvs_exp));
        }
    }

    if (isAssignmentOp(exp))
    {
        SgExpression* lhs_operand = isSgBinaryOp(exp)->get_lhs_operand();
        SgExpression* fwd_exp = buildBinaryExpression<SgCommaOpExp>(
                pushVal(lhs_operand, lhs_operand->get_type(), exp),
                copyExpression(exp));
        SgExpression* rvs_exp = buildBinaryExpression<SgAssignOp>(
                copyExpression(lhs_operand),
                popVal(lhs_operand->get_type(), exp));
        output.push_back(ExpPair(fwd_exp, rvs_exp));
    }

    // function call?

    return output;
}

ExpPairs processConstructiveExp(SgExpression* exp)
{
    ExpPairs output;

    if (isSgPlusPlusOp(exp) || isSgMinusMinusOp(exp))
    {
        // Note that after normalization, a plusplus or minusminus operator expression
        // is not used by another expression, which makes its transformation much easier.
        
        // Make sure the expression is of integer type.
        SgExpression* operand = isSgUnaryOp(exp)->get_operand();
        if (operand->get_type()->isIntegerType())
        {
            // ++ and -- can both be reversed without state saving
            if (SgPlusPlusOp* pp_op = isSgPlusPlusOp(exp))
                output.push_back(ExpPair(
                        copyExpression(exp),
                        buildMinusMinusOp(
                            copyExpression(operand), 
                            backstroke_util::reverseOpMode(pp_op->get_mode()))));

            if (SgMinusMinusOp* mm_op = isSgMinusMinusOp(exp))
                output.push_back(ExpPair(
                        copyExpression(exp),
                        buildPlusPlusOp(
                            copyExpression(operand), 
                            backstroke_util::reverseOpMode(mm_op->get_mode()))));
        }
    }

    if (isAssignmentOp(exp))
    {
        SgExpression* lhs_operand = isSgBinaryOp(exp)->get_lhs_operand();
        SgExpression* rhs_operand = isSgBinaryOp(exp)->get_rhs_operand();

        if (lhs_operand->get_type()->isIntegerType())
        {
            // We must make sure that the rhs operand does not contain the lhs operand.
            // Or else, this operation is not constructive. For example, a += a or a += a + b.
            // This can also be done by def-use analysis.

            bool constructive = true;
            Rose_STL_Container<SgNode*> node_list = NodeQuery::querySubTree(rhs_operand, V_SgExpression);
            foreach (SgNode* node, node_list)
            {
                SgExpression* exp = isSgExpression(node);
                ROSE_ASSERT(exp);
                if (areSameVariable(exp, lhs_operand))
                {
                    constructive = false;
                    break;
                }
            }

            if (constructive)
            {
                if (isSgPlusAssignOp(exp))
                    output.push_back(ExpPair(copyExpression(exp),
                            buildBinaryExpression<SgMinusAssignOp>(
                                copyExpression(lhs_operand), 
                                copyExpression(rhs_operand))));

                if (isSgMinusAssignOp(exp))
                    output.push_back(ExpPair(copyExpression(exp),
                            buildBinaryExpression<SgPlusAssignOp>(
                                copyExpression(lhs_operand), 
                                copyExpression(rhs_operand))));

                if (isSgXorAssignOp(exp))
                    output.push_back(ExpPair(copyExpression(exp),
                            buildBinaryExpression<SgXorAssignOp>(
                                copyExpression(lhs_operand), 
                                copyExpression(rhs_operand))));

#if 0
                // we must ensure that the rhs operand of *= is not ZERO
                if (isSgMultAssignOp(bin_op) && !isZero(isSgValueExp(rhs_operand)))
                    // if the rhs_operand is a value and the value is not 0
                    return ExpPair(
                            fwd_exp,
                            buildBinaryExpression<SgDivAssignOp>(
                                copyExpression(model_var), 
                                copyExpression(rhs_operand)));

                if (isSgLshiftAssignOp(bin_op))
                    return ExpPair(
                            fwd_exp,
                            buildBinaryExpression<SgRshiftAssignOp>(
                                copyExpression(model_var), 
                                copyExpression(rhs_operand)));
#endif
            }
        } // if (lhs_operand->get_type()->isIntegerType())
    }

    return ExpPairs();
}


// This function deals with assignment like a = b + c + a,
// which is still constructive.
ExpPairs processConstructiveAssignment(SgExpression* exp)
{
    ExpPairs output;

    if (isSgAssignOp(exp))
    {
        SgExpression* lhs_operand = isSgBinaryOp(exp)->get_lhs_operand();
        SgExpression* rhs_operand = isSgBinaryOp(exp)->get_rhs_operand();

        // We have to record the sign of each variable.
        typedef pair<SgExpression*, bool> VarWithSign;
        vector<VarWithSign> vars;

        // The following loop get all variables with their signs in the add/subtract expression.
        queue<VarWithSign> to_process;
        to_process.push(VarWithSign(rhs_operand, true));
        while (!to_process.empty())
        {
            VarWithSign var = to_process.front();
            SgExpression* e = var.first;
            bool is_plus = var.second;
            to_process.pop();

            if (SgAddOp* add_op = isSgAddOp(e))
            {
                to_process.push(VarWithSign(add_op->get_lhs_operand(), is_plus));
                to_process.push(VarWithSign(add_op->get_rhs_operand(), is_plus));
            }
            else if (SgSubtractOp* sub_op = isSgSubtractOp(e))
            {
                to_process.push(VarWithSign(sub_op->get_lhs_operand(), is_plus));
                to_process.push(VarWithSign(sub_op->get_rhs_operand(), !is_plus));
            }
            else if (SgUnaryAddOp* plus_op = isSgUnaryAddOp(e))
                to_process.push(VarWithSign(plus_op->get_operand(), is_plus));
            else if (SgMinusOp* minus_op = isSgMinusOp(e))
                to_process.push(VarWithSign(minus_op->get_operand(), !is_plus));
            else
                vars.push_back(var);
        }

        int count = 0;
        size_t index;
        bool constructive = true;
        for (size_t i = 0; i < vars.size(); ++i)
        {
            if (areSameVariable(vars[i].first, lhs_operand))
            {
                ++count;
                index = i;
            }
            else if (containsVariable(vars[i].first, lhs_operand))
            {
                constructive = false;
                break;
            }
        }

        SgExpression* fwd_exp = copyExpression(exp);

        // Only if the count of lhs operand in rhs operand is 1 can this expression be 
        // constructive. For example, a = a + a + b is not constructive (considering overflow).
        if (constructive && count == 1)
        {
            // The form a = b - a, the reverse expression is the same.
            if (!vars[index].second)
            {
                SgExpression* rvs_exp = buildBinaryExpression<SgAssignOp>(
                        copyExpression(lhs_operand),
                        copyExpression(rhs_operand));
                output.push_back(ExpPair(fwd_exp, rvs_exp));
            }

            // The following code may be replaced with using replaceExpression() function.

            //vars[index].first = lhs_operand;

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
#if 1
            else
            {
                // a = b - a  ->  a = b - a
                if (rvs_exp)
                    rvs_exp = buildBinaryExpression<SgSubtractOp>(
                            rvs_exp,
                            copyExpression(lhs_operand)); 
                // a = -a
                else
                    rvs_exp = buildUnaryExpression<SgMinusOp>(
                            copyExpression(lhs_operand));

            }
#endif
            rvs_exp = buildBinaryExpression<SgAssignOp>(
                    copyExpression(lhs_operand),
                    rvs_exp);

            output.push_back(ExpPair(fwd_exp, rvs_exp));
        }
    }

    return output;
}


// Process conditional expressions. Note that after normalization, the return value of 
// a conditional expression will not be used so that we can push the flag after the 
// evaluation of the true or false expression. That is:
//     a ? b : c  ==>  a ? (b, push(1)) : (c, push(0))
//                     pop() ? r(b) : r(c)
ExpPairs processConditionalExpression(SgExpression* exp)
{
    ExpPairs output;

    if (isSgConditionalExp(exp))
    {
    }

    return output;
}
