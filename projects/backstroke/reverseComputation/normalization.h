#ifndef REVERSE_COMPUTATION_NORMALIZATION
#define REVERSE_COMPUTATION_NORMALIZATION

#include <rose.h>
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH


using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;


vector<SgExpression*> getAndReplaceModifyingExpression(SgExpression* exp)
{
    vector<SgExpression*> exps;
#if 0
    ROSE_ASSERT(exp);
    Rose_STL_Container<SgNode*> exp_list = NodeQuery::querySubTree(exp, V_SgExpression);
    foreach (SgNode* node, exp_list)
    {
        SgExpression* e = isSgExpression(node);
#endif
        //ROSE_ASSERT(e);
        SgExpression* e = exp;
        // The following binary expressions return lvalue.
        if (    isSgAssignOp(e) ||
                isSgPlusAssignOp(e) ||
                isSgMinusAssignOp(e) ||
                isSgMultAssignOp(e) ||
                isSgDivAssignOp(e) ||
                isSgModAssignOp(e) ||
                isSgIorAssignOp(e) ||
                isSgAndAssignOp(e) ||
                isSgXorAssignOp(e) ||
                isSgLshiftAssignOp(e) ||
                isSgRshiftAssignOp(e))
        {
            exps.push_back(copyExpression(e)); 
            replaceExpression(e, copyExpression(isSgBinaryOp(e)->get_lhs_operand()));
        }
    //}
//#endif
    return exps;
}


SgExpression* normalizeExpression(SgExpression* exp)
{
    if (SgBinaryOp* bin_op = isSgBinaryOp(exp))
    {
        if (    isSgAssignOp(bin_op) ||
                isSgPlusAssignOp(bin_op) ||
                isSgMinusAssignOp(bin_op) ||
                isSgMultAssignOp(bin_op) ||
                isSgDivAssignOp(bin_op) ||
                isSgModAssignOp(bin_op) ||
                isSgIorAssignOp(bin_op) ||
                isSgAndAssignOp(bin_op) ||
                isSgXorAssignOp(bin_op) ||
                isSgLshiftAssignOp(bin_op) ||
                isSgRshiftAssignOp(bin_op))
        {

            cout << "!" << endl;
   // deepDelete(exp); // avoid dangling node in memory pool
    //return 0;
            vector<SgExpression*> exps = getAndReplaceModifyingExpression(bin_op->get_rhs_operand());

            foreach (SgExpression* e, exps)
            {
                SgCommaOpExp* comma_op = buildBinaryExpression<SgCommaOpExp>(e, copyExpression(exp));
                ROSE_ASSERT(isSgExpression(exp));
                cout << get_name(exp->get_parent()) << endl;
                //isSgExprStatement(exp->get_parent())->set_expression(comma_op);
    //deepDelete(exp); // avoid dangling node in memory pool

                replaceExpression(exp, comma_op);
                exp = comma_op;
            }

#if 0
            // If the return value of the expression is not used, we can build several statements
            // to make the code more clear.
            if (!isReturnValueUsed(exp))
            {
                foreach (SgExpression* e, exps)
                {
                    SgStatement* stmt = buildExprStatement(e);
                }
            }
#endif
        }
        else if (isSgCommaOpExp(bin_op))
        {
            return 0;
            if (SgBinaryOp* parent = isSgBinaryOp(bin_op->get_parent()))
            {
                // FIXME There may be dangling nodes after copy.
                // (a, b) + c
                if (parent->get_lhs_operand() == bin_op)
                {
                    SgBinaryOp* new_exp = isSgBinaryOp(copyExpression(parent));
                    new_exp->set_lhs_operand(copyExpression(bin_op->get_rhs_operand()));
                    new_exp->set_rhs_operand(copyExpression(parent->get_rhs_operand()));
                    replaceExpression(parent, buildBinaryExpression<SgCommaOpExp>(
                                copyExpression(bin_op->get_lhs_operand()), new_exp));
                }
                // a + (b, c)
                else if (parent->get_rhs_operand() == bin_op)
                {
                    SgBinaryOp* new_exp = isSgBinaryOp(copyExpression(parent));
                    new_exp->set_lhs_operand(copyExpression(parent->get_lhs_operand()));
                    new_exp->set_rhs_operand(copyExpression(bin_op->get_rhs_operand()));
                    replaceExpression(parent, buildBinaryExpression<SgCommaOpExp>(
                                copyExpression(bin_op->get_lhs_operand()), new_exp));
                }
            }
        }
    }
}

void splitCommaOpExp(SgExpression* exp)
{
    if (SgCommaOpExp* comma_op = isSgCommaOpExp(exp))
    {
        if (SgStatement* stmt = isSgExprStatement(comma_op->get_parent()))
        {
            SgExprStatement* stmt1 = buildExprStatement(comma_op->get_lhs_operand());
            SgExprStatement* stmt2 = buildExprStatement(comma_op->get_rhs_operand());
            replaceStatement(stmt, buildBasicBlock(stmt1, stmt2));

            splitCommaOpExp(stmt1->get_expression());
            splitCommaOpExp(stmt2->get_expression());
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
            }
        }
    }
}

#endif
