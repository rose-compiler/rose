#include "expNormalization.h"
#include "Utilities.h"
#include <boost/foreach.hpp>
#include <utility>
#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>

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
bool isReturnedValueUsed(SgExpression* exp)
{
    SgNode* parent_node = exp->get_parent();

    // If the expression is a full expression in an expression statement.
    if (SgExprStatement* expr_stmt = isSgExprStatement(parent_node))
    {
        SgNode* grandpa_node = expr_stmt->get_parent();

        // In Rose, the condition part of if, for, while and switch statement may be a SgExprStatement.
        
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

    // In (a, b),  a is not used.
    if (SgCommaOpExp* comma_op = isSgCommaOpExp(parent_node))
    {
        if (comma_op->get_lhs_operand() == exp)
            return false;
        if (comma_op->get_rhs_operand() == exp)
            return isReturnedValueUsed(comma_op);
    }

    if (SgConditionalExp* cond_exp = isSgConditionalExp(parent_node))
    {
        if ((cond_exp->get_true_exp() == exp) ||
                (cond_exp->get_false_exp() == exp))
            return isReturnedValueUsed(cond_exp);
    }

    if (SgForStatement* for_stmt = isSgForStatement(parent_node))
    {
        if (for_stmt->get_increment() == exp)
            return false;
    }


    //if (SgExpression* parent_exp = isSgExpression(parent_node))
      //  return true;

    return true;
}

#if 0
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
#endif

SgBasicBlock* getCurrentBody(SgExpression* e)
{
    SgNode* parent = e->get_parent();
    SgBasicBlock* body = isSgBasicBlock(parent);
    while (body == NULL)
    {
        parent = parent->get_parent();
        if (parent == NULL)
            return NULL;
        body = isSgBasicBlock(parent);
    }
    return body;
}

void getAndReplaceModifyingExpression(SgExpression*& e)
{
    // Avoid to bring variable reference expression: a = b  ==>  (a = b, a) 
    // if a is not used.
    if (!isReturnedValueUsed(e))
        return;

    // The following binary expressions return lvalue.
    if (isAssignmentOp(e))
    {
        // a = b  ==>  a = b, a
        SgExpression* new_exp = buildBinaryExpression<SgCommaOpExp>(
                    copyExpression(e), 
                    copyExpression(isSgBinaryOp(e)->get_lhs_operand()));
        replaceExpression(e, new_exp);
        e = new_exp;
    }
    else if (isSgPlusPlusOp(e) || isSgMinusMinusOp(e))
    {
        // prefix ++ or -- returns lvalues, which we can hoist.
        // --a  ==>  --a, a
        if (isSgUnaryOp(e)->get_mode() == SgUnaryOp::prefix)
        {
            SgExpression* new_exp = buildBinaryExpression<SgCommaOpExp>(
                    copyExpression(e), 
                    copyExpression(isSgUnaryOp(e)->get_operand()));
            replaceExpression(e, new_exp);
            e = new_exp;
        }
        // postfix ++ or -- returns rvalues, which is not the same value as after 
        // this operation. We can delay them, but should take care when doing it.
        // a--  ==>  t = a, --a, t
        else
        {
            SgBasicBlock* body = getCurrentBody(e);
            SgName name = backstroke_util::GenerateUniqueVariableName(body, "t");
            SgVariableDeclaration* temp_decl = buildVariableDeclaration(name, e->get_type(), NULL, getScope(e));
            body->prepend_statement(temp_decl);

            SgInitializedName* init_name = temp_decl->get_decl_item(name);
            ROSE_ASSERT(init_name->get_scope());
            SgExpression* ass = buildBinaryExpression<SgAssignOp>(
                    buildVarRefExp(init_name, getScope(e)), 
                    copyExpression(isSgUnaryOp(e)->get_operand()));
            SgExpression* comma_exp = buildBinaryExpression<SgCommaOpExp>(
                    ass, copyExpression(e));
            comma_exp = buildBinaryExpression<SgCommaOpExp>(comma_exp, buildVarRefExp(init_name, getScope(e)));
            replaceExpression(e, comma_exp);

#if 0
            SgExpression* exp = copyExpression(e);
            SgExpression* new_exp = copyExpression(isSgUnaryOp(e)->get_operand());
            replaceExpression(e, new_exp);
            e = new_exp;
            return exp;
#endif
        }
    }
    else if (isSgAndOp(e) || isSgOrOp(e))
    {
        // a && b  ==>  t = a, t && t = b, t
        // a || b  ==>  t = a, t || t = b, t
       
        SgBasicBlock* body = getCurrentBody(e);
        SgName name = backstroke_util::GenerateUniqueVariableName(body, "t");
        SgVariableDeclaration* temp_decl = buildVariableDeclaration(name, buildBoolType(), NULL, getScope(e));
        body->prepend_statement(temp_decl);

        SgInitializedName* init_name = temp_decl->get_decl_item(name);
        SgVarRefExp* temp_var = buildVarRefExp(init_name, getScope(e));

        SgExpression* ass1 = buildBinaryExpression<SgAssignOp>(
                copyExpression(temp_var),
                copyExpression(isSgBinaryOp(e)->get_lhs_operand()));
        SgExpression* ass2 = buildBinaryExpression<SgAssignOp>(
                copyExpression(temp_var),
                copyExpression(isSgBinaryOp(e)->get_rhs_operand()));


        SgBinaryOp* new_exp = isSgBinaryOp(copyExpression(e));
        new_exp->set_lhs_operand(copyExpression(temp_var));
        new_exp->set_rhs_operand(ass2);

        SgExpression* comma_exp = buildBinaryExpression<SgCommaOpExp>(ass1, new_exp);
        comma_exp = buildBinaryExpression<SgCommaOpExp>(comma_exp, copyExpression(temp_var));
        replaceExpression(e, comma_exp);

        ass1 = extendCommaOpExp(ass1);
        ass2 = extendCommaOpExp(ass2);
    }
    else if (isSgConditionalExp(e))
    {
    }
}

SgExpression* extendCommaOpExp(SgExpression* exp)
{
    if (SgBinaryOp* bin_op = isSgBinaryOp(exp))
    {
        // Ignore comma operator.
        if (isSgCommaOpExp(bin_op))
            return exp;

        SgExpression* lhs = bin_op->get_lhs_operand();
        SgExpression* rhs = bin_op->get_rhs_operand();

        // (a, b) + c  ==>  (a, b + c)
        if (SgCommaOpExp* comma_op = isSgCommaOpExp(lhs))
        {
            SgBinaryOp* new_exp = isSgBinaryOp(copyExpression(bin_op));
            new_exp->set_lhs_operand(copyExpression(comma_op->get_rhs_operand()));
            new_exp->set_rhs_operand(copyExpression(rhs));

            SgCommaOpExp* new_comma_op = buildBinaryExpression<SgCommaOpExp>(
                    copyExpression(comma_op->get_lhs_operand()), new_exp);
            replaceExpression(bin_op, new_comma_op);

            extendCommaOpExp(new_comma_op->get_lhs_operand());
            extendCommaOpExp(new_comma_op->get_rhs_operand());

            return new_comma_op;
        }

        // Operator || and && cannot use the following transformation
        if (!isSgAndOp(bin_op) && !isSgOrOp(bin_op))
        {
            // a + (b, c)  ==>  (b, a + c)
            if (SgCommaOpExp* comma_op = isSgCommaOpExp(rhs))
            {
                SgBinaryOp* new_exp = isSgBinaryOp(copyExpression(bin_op));
                new_exp->set_lhs_operand(copyExpression(lhs));
                new_exp->set_rhs_operand(copyExpression(comma_op->get_rhs_operand()));

                SgCommaOpExp* new_comma_op = buildBinaryExpression<SgCommaOpExp>(
                        copyExpression(comma_op->get_lhs_operand()), new_exp);
                replaceExpression(bin_op, new_comma_op);

                extendCommaOpExp(new_comma_op->get_lhs_operand());
                extendCommaOpExp(new_comma_op->get_rhs_operand());

                return new_comma_op;
            }
        }
    }

    else if (SgUnaryOp* unary_op = isSgUnaryOp(exp))
    {
        SgExpression* operand = unary_op->get_operand();

        // !(a, b)  ==>  (a, !b)
        if (SgCommaOpExp* comma_op = isSgCommaOpExp(operand))
        {
            SgUnaryOp* new_exp = isSgUnaryOp(copyExpression(unary_op));
            new_exp->set_operand(copyExpression(comma_op->get_rhs_operand()));

            SgCommaOpExp* new_comma_op = buildBinaryExpression<SgCommaOpExp>(
                    copyExpression(comma_op->get_lhs_operand()), new_exp);
            replaceExpression(unary_op, new_comma_op);

            extendCommaOpExp(new_comma_op->get_lhs_operand());
            extendCommaOpExp(new_comma_op->get_rhs_operand());

            return new_comma_op;
        }
    }
    return exp;
}

vector<SgExpression*> getAllExpressions(SgNode* root)
{
    struct GetExpTraversal : public AstSimpleProcessing
    {
        vector<SgExpression*> all_exp;
        virtual void visit(SgNode* n)
        {
            SgExpression* exp = isSgExpression(n);
            if (exp) all_exp.push_back(exp);
        }
    }; 

    GetExpTraversal traversal;
    traversal.traverse(root, postorder);
    return traversal.all_exp;
}

void normalizeEvent(SgFunctionDefinition* func)
{
    vector<SgVariableDeclaration*> all_temp_decl;

    //Rose_STL_Container<SgNode*> exp_list = NodeQuery::querySubTree(func->get_body(), V_SgExpression, postorder);
    vector<SgExpression*> exp_list = getAllExpressions(func->get_body());
    foreach (SgExpression* exp, exp_list)
    {
        if (isSgExpression(exp->get_parent()))
        {
            getAndReplaceModifyingExpression(exp);
            //SgVariableDeclaration* decl = getAndReplaceModifyingExpression2(exp);
            //if (decl)
              //  all_temp_decl.push_back(decl);
        }

        exp = extendCommaOpExp(exp);
        splitCommaOpExp(exp);
    }

    foreach (SgVariableDeclaration* decl, all_temp_decl)
    {
        func->get_body()->prepend_statement(decl);
    }

    removeUselessBraces(func->get_body());

#if 0
    Rose_STL_Container<SgNode*> body_list = NodeQuery::querySubTree(func->get_body(), V_SgBasicBlock);
    foreach (SgNode* node, body_list)
    {
        SgBasicBlock* body = isSgBasicBlock(node);
        removeUselessBraces(func->get_body());
    }
#endif
}

#if 0
SgExpression* normalizeExpression(SgExpression* exp)
{
    if (isSgExpression(exp->get_parent()))
    {
        getAndReplaceModifyingExpression(exp);
        //SgVariableDeclaration* decl = getAndReplaceModifyingExpression2(exp);
#if 0
        if (decl)
        {
            SgNode* parent = exp->get_parent();
            SgBasicBlock* body = isSgBasicBlock(parent);
            while (body == NULL)
            {
                parent = parent->get_parent();
                body = isSgBasicBlock(parent);
            }
            body->prepend_statement(decl);
#if 0
            while (SgExpression* pexp = isSgExpression(parent))
            {
                if (!isReturnedValueUsed(pexp))
                {
                    // a = b++  ==>  a = b, b++
                    SgExpression* new_exp = buildBinaryExpression<SgCommaOpExp>(
                            copyExpression(pexp), e);
                    replaceExpression(pexp, new_exp);
                }
            }
#endif
        }
#endif


#if 0
        if (e)
        {
            SgExpression* new_exp = buildBinaryExpression<SgCommaOpExp>(e, exp);
            replaceExpression(exp, new_exp);
            return normalizeExpression(new_exp);
        }
#endif
    }
    return exp;

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
#if 0
            // a = (b = c)  ==>  a = (b = c, b)  ==>  b = c, a = b
            // (a = b) = c  ==>  (a = b, a) = c  ==>  a = b, a = c
            SgExpression *lexp, *rexp;
            bool lflag, rflag;

            tie(lexp, lflag) = getAndReplaceModifyingExpression(lhs);
            tie(rexp, rflag) = getAndReplaceModifyingExpression(rhs);

            if (lexp || rexp)
            {
                SgExpression* new_exp = copyExpression(exp);
                if (rexp && rflag)
                    new_exp = buildBinaryExpression<SgCommaOpExp>(rexp, new_exp);
                if (lexp && lflag)
                    new_exp = buildBinaryExpression<SgCommaOpExp>(lexp, new_exp);
                if (lexp && !lflag)
                {
                }
                if (rexp && !rflag)
                {
                    // a = b++  ==>  (a = b, b++)  note that the return value may change
                    new_exp = buildBinaryExpression<SgCommaOpExp>(new_exp, rexp);

                }


                //isSgExprStatement(exp->get_parent())->set_expression(comma_op);
                //deepDelete(exp); // avoid dangling node in memory pool

                replaceExpression(exp, new_exp);

                return normalizeExpression(new_exp);
            }
#endif

            // (a, b) + c  ==>  (a, b + c)
            if (SgCommaOpExp* comma_op = isSgCommaOpExp(lhs))
            {
                SgBinaryOp* new_exp = isSgBinaryOp(copyExpression(bin_op));
                new_exp->set_lhs_operand(copyExpression(comma_op->get_rhs_operand()));
                new_exp->set_rhs_operand(copyExpression(rhs));

                SgCommaOpExp* new_comma_op = buildBinaryExpression<SgCommaOpExp>(
                        copyExpression(comma_op->get_lhs_operand()), new_exp);
                normalizeExpression(new_exp);

                replaceExpression(bin_op, new_comma_op);

                return new_comma_op;

                exp = new_comma_op;

                return normalizeExpression(new_comma_op);

                normalizeExpression(new_comma_op->get_lhs_operand());
                normalizeExpression(new_comma_op->get_rhs_operand());
            }
            // a + (b, c)  ==>  (b, a + c)
            else if (SgCommaOpExp* comma_op = isSgCommaOpExp(rhs))
            {
                SgBinaryOp* new_exp = isSgBinaryOp(copyExpression(bin_op));
                new_exp->set_lhs_operand(copyExpression(lhs));
                new_exp->set_rhs_operand(copyExpression(comma_op->get_rhs_operand()));


                SgCommaOpExp* new_comma_op = buildBinaryExpression<SgCommaOpExp>(
                        copyExpression(comma_op->get_lhs_operand()), new_exp);
                normalizeExpression(new_exp);

                replaceExpression(bin_op, new_comma_op);

                return new_comma_op;

                exp = new_comma_op;

                return normalizeExpression(new_comma_op);

                normalizeExpression(new_comma_op->get_lhs_operand());
                normalizeExpression(new_comma_op->get_rhs_operand());
            }
        }
    }

    else if (SgUnaryOp* unary_op = isSgUnaryOp(exp))
    {
        SgExpression* operand = unary_op->get_operand();
#if 0
        SgExpression* e;
        bool flag;

        tie(e, flag) = getAndReplaceModifyingExpression(operand);

        if (e)
        {
            SgExpression* new_exp = buildBinaryExpression<SgCommaOpExp>(e, copyExpression(exp));
            replaceExpression(exp, new_exp);

            return normalizeExpression(new_exp);
        }
#endif

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
#endif

void splitCommaOpExp(SgExpression* exp)
{
    if (SgCommaOpExp* comma_op = isSgCommaOpExp(exp))
    {
        if (SgStatement* stmt = isSgExprStatement(comma_op->get_parent()))
        {
            // Note that in Rose, the condition part in if statement can be an expression statement.
            if (isSgBasicBlock(stmt->get_parent()))
            {
                SgExpression* lhs = comma_op->get_lhs_operand();
                SgExpression* rhs = comma_op->get_rhs_operand();

                lhs->set_need_paren(false);
                rhs->set_need_paren(false);

                SgExprStatement* stmt1 = buildExprStatement(lhs);
                SgExprStatement* stmt2 = buildExprStatement(rhs);
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

