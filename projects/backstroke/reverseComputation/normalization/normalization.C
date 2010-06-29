#include "normalization.h"
#include <boost/foreach.hpp>
#include <utility>
#include <boost/tuple/tuple.hpp>

#define foreach BOOST_FOREACH


using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;


bool isAssignmentOp(SgExpression* e)
{
    return isSgAssignOp(e) ||
        isSgPlusAssignOp(e) ||
        isSgMinusAssignOp(e) ||
        isSgMultAssignOp(e) ||
        isSgDivAssignOp(e) ||
        isSgModAssignOp(e) ||
        isSgIorAssignOp(e) ||
        isSgAndAssignOp(e) ||
        isSgXorAssignOp(e) ||
        isSgLshiftAssignOp(e) ||
        isSgRshiftAssignOp(e);
}

// Returns a boolean value to indicate whether the return value (rvalue) of the given expression is used.
bool isReturnValueUsed(SgExpression* exp)
{
    SgNode* parent_node = exp->get_parent();

    // If the expression is a full expression in an expression statement.
    if (SgExprStatement* expr_stmt = isSgExprStatement(parent_node))
    {
        SgNode* grandpa_node = expr_stmt->get_parent();

        if (SgIfStmt* if_stmt = isSgIfStmt(grandpa_node))
            if (if_stmt->get_conditional() == expr_stmt)
                return true;

        if (SgForStatement* for_stmt = isSgForStatement(grandpa_node))
            if (for_stmt->get_test() == expr_stmt)
                return true;

        if (SgWhileStmt* while_stmt = isSgWhileStmt(grandpa_node))
            if (while_stmt->get_condition() == expr_stmt)
                return true;

        if (SgSwitchStatement* switch_stmt = isSgSwitchStatement(grandpa_node))
            if (switch_stmt->get_item_selector() == expr_stmt)
                return true;

        return false;
    }

    if (SgCommaOpExp* comma_op = isSgCommaOpExp(parent_node))
    {
        if (comma_op->get_lhs_operand() == exp)
            return false;
        if (comma_op->get_rhs_operand() == exp)
            return isReturnValueUsed(comma_op);
    }

    if (SgConditionalExp* cond_exp = isSgConditionalExp(parent_node))
    {
        if ((cond_exp->get_true_exp() == exp) ||
                (cond_exp->get_false_exp() == exp))
            return isReturnValueUsed(cond_exp);
    }

    if (SgForStatement* for_stmt = isSgForStatement(parent_node))
    {
        if (for_stmt->get_increment() == exp)
            return false;
    }

    // Other cases: init and incr part in for, 

    if (SgExpression* parent_exp = isSgExpression(parent_node))
        return true;

    return true;
}

// The first return value is the expression which should be relocated, and the second 
// return value indicate the position (before(true)/after(false)).
pair<SgExpression*, bool> getAndReplaceModifyingExpression(SgExpression* e)
{
    SgExpression* exp = NULL;

    // The following binary expressions return lvalue.
    if (isAssignmentOp(e))
    {
        exp = copyExpression(e); 
        replaceExpression(e, copyExpression(isSgBinaryOp(e)->get_lhs_operand()));
    }
    else if (isSgPlusPlusOp(e) || isSgMinusMinusOp(e))
    {
        // prefix ++ or -- returns lvalues, which we can hoist.
        if (isSgUnaryOp(e)->get_mode() == SgUnaryOp::prefix)
        {
            exp = copyExpression(e); 
            replaceExpression(e, copyExpression(isSgUnaryOp(e)->get_operand()));
        }
        // postfix ++ or -- returns rvalues, which is not the same value as after 
        // this operation. We can delay them, but should take care when doing it.
        else
        {
            exp = copyExpression(e);
            replaceExpression(e, copyExpression(isSgUnaryOp(e)->get_operand()));
            return make_pair(exp, false);
        }
    }
    return make_pair(exp, true);
}


SgExpression* normalizeExpression(SgExpression* exp)
{
    if (SgBinaryOp* bin_op = isSgBinaryOp(exp))
    {
        SgExpression* lhs = bin_op->get_lhs_operand();
        SgExpression* rhs = bin_op->get_rhs_operand();

        // For assignment expressions, process its lhs and rhs operands to check
        // if there is other subexpression which modified value. (note that if lhs
        // operand modifies value, the result of this expression is unspecified, and 
        // we give it a possible definition).

        // The following three binary expressions have specific evaluation orders.
        if (isSgCommaOpExp(bin_op) || isSgAndOp(bin_op) || isSgOrOp(bin_op))
        {
            normalizeExpression(lhs);
            normalizeExpression(rhs);
        }
        else
        {
            // a = (b = c)  ==>  b = c, a = b
            // (a = b) = c  ==>  a = b, a = c
            SgExpression *lexp, *rexp;
            bool lflag, rflag;
            tie(lexp, lflag) = getAndReplaceModifyingExpression(lhs);
            tie(rexp, rflag) = getAndReplaceModifyingExpression(rhs);

            if (lexp || rexp)
            {
                SgExpression* new_exp = copyExpression(exp);
                if (rexp && rflag)
                    new_exp = buildBinaryExpression<SgCommaOpExp>(rexp, new_exp);
                else if (lexp && lflag)
                    new_exp = buildBinaryExpression<SgCommaOpExp>(lexp, new_exp);
                else if (lexp)
                {
                }
                else 
                {
                    // a = b++  ==>  (a = b, b++)  note that the return value may change
                    new_exp = buildBinaryExpression<SgCommaOpExp>(new_exp, rexp);

                }


                //isSgExprStatement(exp->get_parent())->set_expression(comma_op);
                //deepDelete(exp); // avoid dangling node in memory pool

                replaceExpression(exp, new_exp);

                return normalizeExpression(new_exp);
            }

            // (a, b) + c  ==>  (a, b + c)
            if (SgCommaOpExp* comma_op = isSgCommaOpExp(lhs))
            {
                SgBinaryOp* new_exp = isSgBinaryOp(copyExpression(bin_op));
                new_exp->set_lhs_operand(copyExpression(comma_op->get_rhs_operand()));
                new_exp->set_rhs_operand(copyExpression(bin_op->get_rhs_operand()));

                SgCommaOpExp* new_comma_op = buildBinaryExpression<SgCommaOpExp>(
                        copyExpression(comma_op->get_lhs_operand()), new_exp);
                replaceExpression(bin_op, new_comma_op);
                exp = new_comma_op;

                return normalizeExpression(new_comma_op);

                normalizeExpression(new_comma_op->get_lhs_operand());
                normalizeExpression(new_comma_op->get_rhs_operand());
            }
            // a + (b, c)  ==>  (b, a + c)
            else if (SgCommaOpExp* comma_op = isSgCommaOpExp(rhs))
            {
                SgBinaryOp* new_exp = isSgBinaryOp(copyExpression(bin_op));
                new_exp->set_lhs_operand(copyExpression(bin_op->get_lhs_operand()));
                new_exp->set_rhs_operand(copyExpression(comma_op->get_rhs_operand()));


                SgCommaOpExp* new_comma_op = buildBinaryExpression<SgCommaOpExp>(
                        copyExpression(comma_op->get_lhs_operand()), new_exp);
                replaceExpression(bin_op, new_comma_op);
                exp = new_comma_op;

                return normalizeExpression(new_comma_op);

                normalizeExpression(new_comma_op->get_lhs_operand());
                normalizeExpression(new_comma_op->get_rhs_operand());
            }
        }
    }

    else if (SgUnaryOp* unary_op = isSgUnaryOp(exp))
    {
        SgExpression* e;
        bool flag;

        SgExpression* operand = unary_op->get_operand();
        tie(e, flag) = getAndReplaceModifyingExpression(operand);

        if (e)
        {
            SgExpression* new_exp = buildBinaryExpression<SgCommaOpExp>(e, copyExpression(exp));
            replaceExpression(exp, new_exp);

            return normalizeExpression(new_exp);
        }

        // !(a, b)  ==>  (a, !b)
        if (SgCommaOpExp* comma_op = isSgCommaOpExp(operand))
        {
            SgUnaryOp* new_exp = isSgUnaryOp(copyExpression(unary_op));
            new_exp->set_operand(copyExpression(comma_op->get_rhs_operand()));

            SgCommaOpExp* new_comma_op = buildBinaryExpression<SgCommaOpExp>(
                    copyExpression(comma_op->get_lhs_operand()), new_exp);
            replaceExpression(unary_op, new_comma_op);

            return normalizeExpression(new_comma_op);

            normalizeExpression(new_comma_op->get_lhs_operand());
            normalizeExpression(new_comma_op->get_rhs_operand());
        }
    }

    return exp;
}

void splitCommaOpExp(SgExpression* exp)
{
    if (SgCommaOpExp* comma_op = isSgCommaOpExp(exp))
    {
        if (SgStatement* stmt = isSgExprStatement(comma_op->get_parent()))
        {
            // Note that in Rose, the condition part in if statement can be an expression statement.
            if (isSgBasicBlock(stmt->get_parent()))
            {
                SgExprStatement* stmt1 = buildExprStatement(comma_op->get_lhs_operand());
                SgExprStatement* stmt2 = buildExprStatement(comma_op->get_rhs_operand());
                replaceStatement(stmt, buildBasicBlock(stmt1, stmt2));

                splitCommaOpExp(stmt1->get_expression());
                splitCommaOpExp(stmt2->get_expression());
            }
        }
    }
}

void removeUselessBraces(SgBasicBlock* body)
{
    foreach (SgStatement* s, body->get_statements())
    {
        if (SgBasicBlock* block = isSgBasicBlock(s))
        {
            // First handle all blocks in this block.
            removeUselessBraces(block);

            // If there is no declaration in a basic block and this basic block 
            // belongs to another basic block, the braces can be removed.
            bool has_decl = false;
            foreach (SgStatement* stmt, block->get_statements())
            {
                if (isSgDeclarationStatement(stmt))
                {
                    has_decl = true;
                    break;
                }
            }

            if (!has_decl)
            {
                foreach (SgStatement* stmt, block->get_statements())
                    insertStatement(block, copyStatement(stmt));
                removeStatement(block);

                // Since statements in the 'body' has been changed, we have to re-run 
                // this function.
                removeUselessBraces(body);
                return;
            }
        }
    }
}

