#include "expNormalization.h"
#include "utilities/Utilities.h"
#include <boost/foreach.hpp>
#include <utility>
#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>

#define foreach BOOST_FOREACH


using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;
using namespace backstroke_util;



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
    if (!backstroke_util::isReturnValueUsed(e))
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
            e = comma_exp;

#if 0
            SgExpression* exp = copyExpression(e);
            SgExpression* new_exp = copyExpression(isSgUnaryOp(e)->get_operand());
            replaceExpression(e, new_exp);
            e = new_exp;
            return exp;
#endif
        }
    }
#if 0
    else if (isSgAndOp(e) || isSgOrOp(e))
    {
        // a && b  ==>  t = a, t && (t = b), t
        // a || b  ==>  t = a, t || (t = b), t
       
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

        ass1 = propagateCommaOpExp(ass1);
        ass2 = propagateCommaOpExp(ass2);
    }
#endif
    else if (isSgConditionalExp(e))
    {
        // If the conditional expression returns rvalue, we can transform it into
        // a ? b : c  ==>  a ? (t = b) : (t = c), t
        // Else, if it returns and is used as a lvalue, we need more agressive transformation:
        // (a ? b : c) = d  ==>  a ? (b = d) : (c = d)
    }
}

SgExpression* propagateCommaOpExp(SgExpression* exp)
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

            propagateCommaOpAndConditionalExp(new_comma_op->get_lhs_operand());
            propagateCommaOpAndConditionalExp(new_comma_op->get_rhs_operand());

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

                propagateCommaOpAndConditionalExp(new_comma_op->get_lhs_operand());
                propagateCommaOpAndConditionalExp(new_comma_op->get_rhs_operand());

                return new_comma_op;
            }
        }
    }

    if (SgUnaryOp* unary_op = isSgUnaryOp(exp))
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

            propagateCommaOpAndConditionalExp(new_comma_op->get_lhs_operand());
            propagateCommaOpAndConditionalExp(new_comma_op->get_rhs_operand());

            return new_comma_op;
        }
    }

    if (SgConditionalExp* cond_exp = isSgConditionalExp(exp))
    {   
        SgExpression* cond = cond_exp->get_conditional_exp();

        if (SgCommaOpExp* comma_op = isSgCommaOpExp(cond))
        {
            SgConditionalExp* new_cond_exp = isSgConditionalExp(copyExpression(cond_exp));
            new_cond_exp->set_conditional_exp(copyExpression(comma_op->get_rhs_operand()));
            SgCommaOpExp* new_comma_op = buildBinaryExpression<SgCommaOpExp>(
                    copyExpression(comma_op->get_lhs_operand()), new_cond_exp);
            replaceExpression(cond_exp, new_comma_op);

            propagateCommaOpAndConditionalExp(new_comma_op->get_lhs_operand());
            propagateCommaOpAndConditionalExp(new_comma_op->get_rhs_operand());

            return new_comma_op;
        }
    }
    return exp;
}

// After the following transformation, a conditional expression should not be the operand
// of another expression except comma and conditional expressions. This will make its forward
// and reverse code generation easier.
SgExpression* propagateConditionalExp(SgExpression* exp)
{
    if (SgBinaryOp* bin_op = isSgBinaryOp(exp))
    {
        // Ignore comma operator.
        if (isSgCommaOpExp(bin_op))
            return exp;

        SgExpression* lhs = bin_op->get_lhs_operand();
        SgExpression* rhs = bin_op->get_rhs_operand();

        // (a ? b : c) + d  ==>  a ? (b + d) : (c + d)
        if (SgConditionalExp* cond_op = isSgConditionalExp(lhs))
        {
            SgExpression* true_exp = cond_op->get_true_exp();
            SgExpression* false_exp = cond_op->get_false_exp();

            SgBinaryOp* new_true_exp = isSgBinaryOp(copyExpression(bin_op));
            new_true_exp->set_lhs_operand(copyExpression(true_exp));
            new_true_exp->set_rhs_operand(copyExpression(rhs));
            replaceExpression(true_exp, new_true_exp);

            SgBinaryOp* new_false_exp = isSgBinaryOp(copyExpression(bin_op));
            new_false_exp->set_lhs_operand(copyExpression(false_exp));
            new_false_exp->set_rhs_operand(copyExpression(rhs));
            replaceExpression(false_exp, new_false_exp);

            // Copy the old cond_op since replacement will destroy it.
            SgConditionalExp* new_cond_op = isSgConditionalExp(copyExpression(cond_op));
            replaceExpression(bin_op, new_cond_op);

            propagateCommaOpAndConditionalExp(new_cond_op->get_true_exp());
            propagateCommaOpAndConditionalExp(new_cond_op->get_false_exp());

            return new_cond_op;
        }

        // Operator || and && cannot use the following transformation
        if (!isSgAndOp(bin_op) && !isSgOrOp(bin_op))
        {
            // a + (b ? c : d)  ==>  b ? (a + c) : (a + d)
            if (SgConditionalExp* cond_op = isSgConditionalExp(rhs))
            {
                SgExpression* true_exp = cond_op->get_true_exp();
                SgExpression* false_exp = cond_op->get_false_exp();

                SgBinaryOp* new_true_exp = isSgBinaryOp(copyExpression(bin_op));
                new_true_exp->set_lhs_operand(copyExpression(lhs));
                new_true_exp->set_rhs_operand(copyExpression(true_exp));
                replaceExpression(true_exp, new_true_exp);

                SgBinaryOp* new_false_exp = isSgBinaryOp(copyExpression(bin_op));
                new_false_exp->set_lhs_operand(copyExpression(lhs));
                new_false_exp->set_rhs_operand(copyExpression(false_exp));
                replaceExpression(false_exp, new_false_exp);

                // Copy the old cond_op since replacement will destroy it.
                SgConditionalExp* new_cond_op = isSgConditionalExp(copyExpression(cond_op));
                replaceExpression(bin_op, new_cond_op);

                propagateCommaOpAndConditionalExp(new_cond_op->get_true_exp());
                propagateCommaOpAndConditionalExp(new_cond_op->get_false_exp());

                return new_cond_op;
            }
        }
    }

    if (SgUnaryOp* unary_op = isSgUnaryOp(exp))
    {
        SgExpression* operand = unary_op->get_operand();

        // !(a ? b : c)  ==>  a ? !b : !c
        if (SgConditionalExp* cond_op = isSgConditionalExp(operand))
        {
            SgExpression* true_exp = cond_op->get_true_exp();
            SgExpression* false_exp = cond_op->get_false_exp();

            SgUnaryOp* new_true_exp = isSgUnaryOp(copyExpression(unary_op));
            new_true_exp->set_operand(copyExpression(true_exp));
            replaceExpression(true_exp, new_true_exp);

            SgUnaryOp* new_false_exp = isSgUnaryOp(copyExpression(unary_op));
            new_false_exp->set_operand(copyExpression(false_exp));
            replaceExpression(false_exp, new_false_exp);

            // Copy the old cond_op since replacement will destroy it.
            SgConditionalExp* new_cond_op = isSgConditionalExp(copyExpression(cond_op));
            replaceExpression(unary_op, new_cond_op);

            propagateCommaOpAndConditionalExp(new_cond_op->get_true_exp());
            propagateCommaOpAndConditionalExp(new_cond_op->get_false_exp());

            return new_cond_op;
        }
    }

    // Condtional exp as condition of another conditional exp.
    // (a ? b : c) ? d : e  ==>  a ? (b ? d : e) : (c ? d : e)
    if (SgConditionalExp* cond_exp = isSgConditionalExp(exp))
    {   
        SgExpression* cond = cond_exp->get_conditional_exp();

        if (SgConditionalExp* cond_op = isSgConditionalExp(cond))
        {
            SgConditionalExp* cond_true_exp = isSgConditionalExp(copyExpression(cond_exp));
            SgConditionalExp* cond_false_exp = isSgConditionalExp(copyExpression(cond_exp));

            cond_true_exp->set_conditional_exp(copyExpression(cond_op->get_true_exp()));
            cond_false_exp->set_conditional_exp(copyExpression(cond_op->get_false_exp()));

            SgConditionalExp* new_cond_op = buildConditionalExp(
                    copyExpression(cond_op->get_conditional_exp()), 
                    cond_true_exp, 
                    cond_false_exp);
            replaceExpression(cond_exp, new_cond_op);

            propagateCommaOpAndConditionalExp(new_cond_op->get_true_exp());
            propagateCommaOpAndConditionalExp(new_cond_op->get_false_exp());

            return new_cond_op;
        }
    }

    return exp;
}

#if 0
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
#endif

void preprocess(SgFunctionDefinition* func)
{
    /******************************************************************************/
    // To ensure every if, while, etc. has a basic block as its body.
    vector<SgStatement*> stmt_list = querySubTree<SgStatement>(func->get_body());
    foreach (SgStatement* stmt, stmt_list)
        ensureBasicBlockAsParent(stmt);

    /******************************************************************************/
    // Before dealing with variable declarations, all for loops should be processed first.
    // This is because that SgForInitStatement is special in which several declarations can
    // coexist. We will hoist it outside of its for loop statement.

    vector<SgForInitStatement*> for_init_stmts = querySubTree<SgForInitStatement>(func->get_body());
    foreach (SgForInitStatement* for_init_stmt, for_init_stmts)
    {
        // A SgForInitStatement object can contain several variable declarations, or one expression statement.
        SgStatementPtrList stmts = for_init_stmt->get_init_stmt();
        if (!stmts.empty() && isSgVariableDeclaration(stmts[0]))
        {
            SgForStatement* for_stmt = isSgForStatement(for_init_stmt->get_parent());
            SgBasicBlock* new_block = buildBasicBlock();
            foreach (SgStatement* decl, stmts)
            {
                SgVariableDeclaration* var_decl = isSgVariableDeclaration(decl);
                ROSE_ASSERT(var_decl);
                new_block->append_statement(copyStatement(var_decl));
                //delete var_decl;
            }

            // Since there is no builder function for SgForInitStatement, we build it by ourselves
            SgForInitStatement* null_for_init = new SgForInitStatement();
            setOneSourcePositionForTransformation(null_for_init);
            replaceStatement(for_init_stmt, null_for_init);

            // It seems that 'for_init_stmt' should be deleted explicitly.
            //deepDelete(for_init_stmt);

            new_block->append_statement(copyStatement(for_stmt));
            replaceStatement(for_stmt, new_block);
            //deepDelete(for_stmt);
        }
    }

    // Separate variable's definition from its declaration
    // FIXME It is not sure that whether to permit declaration in condition of if (if the varible declared 
    // is not of scalar type?).
    vector<SgVariableDeclaration*> var_decl_list = querySubTree<SgVariableDeclaration>(func->get_body());
    foreach (SgVariableDeclaration* var_decl, var_decl_list)
    {
        SgInitializedName* init = var_decl->get_variables()[0];
        SgAssignInitializer* initializer = isSgAssignInitializer(init->get_initializer());
        
        SgStatement* parent = isSgStatement(var_decl->get_parent());
        ROSE_ASSERT(parent);

        if (isScalarType(init->get_type()) ||
                isSgPointerType(init->get_type()))
        {
            // Miss catch here?
            if (isSgIfStmt(parent) ||
                    isSgWhileStmt(parent) ||
                    isSgSwitchStatement(parent) ||
                    isSgForStatement(parent))
            {
                // if (int i = j);  ==>  { int i; if (i = j); }
                // new_exp  <==  i = j
                SgExpression* new_exp = buildBinaryExpression<SgAssignOp>(
                        buildVarRefExp(init, getScope(var_decl)),
                        copyExpression(initializer->get_operand()));

                // new_decl  <==  int i;
                // Here we cannot just build a new variable declaration, which will cause a problem.
                //SgVariableDeclaration* new_decl = buildVariableDeclaration(
                 //       init->get_name(), init->get_type());
                SgVariableDeclaration* new_decl = isSgVariableDeclaration(copyStatement(var_decl));
                new_decl->get_variables()[0]->set_initializer(NULL);

                replaceStatement(var_decl, buildExprStatement(new_exp));
                SgBasicBlock* block = buildBasicBlock(copyStatement(parent));
                block->prepend_statement(new_decl);

                replaceStatement(parent, block);
            }
        }

#if 0
        // Make sure the variable declared has scalar type.
        if (    isScalarType(init->get_type()) &&
                isSgPointerType(init->get_type()) &&
                initializer && 
                containsModifyingExpression(initializer->get_operand()))
        {
            // int i = ++j;  ==>  int i; i = ++j;

            // new_exp  <==  i = ++j
            SgExpression* new_exp = buildBinaryExpression<SgAssignOp>(
                    buildVarRefExp(init, getScope(var_decl)),
                    copyExpression(initializer->get_operand()));
            // new_stmt  <==  i = ++j;
            SgStatement* new_stmt = buildExprStatement(new_exp);
            //initializer->set_operand(buildIntVal(0));
            initializer->set_operand(NULL);

            SgStatement* parent = isSgStatement(var_decl->get_parent());
            if (isSgBasicBlock(parent))
            {
                insertStatement(var_decl, new_stmt, false);
            }
            // At this moment, we forbid the for init statement.
            else if (isSgForInitStatement(parent))
                ROSE_ASSERT(false);
            else
            {
                // Here we deal with the variable declaration in condition part of if, while, etc.
                // if (int i = ++j);  ==>  { int i; if (i = ++j); }
                SgStatement* new_decl = copyStatement(var_decl);
                replaceStatement(var_decl, new_stmt);
                ROSE_ASSERT(new_stmt->get_parent());
                SgBasicBlock* block = buildBasicBlock(copyStatement(parent));
                block->prepend_statement(new_decl);
                replaceStatement(parent, block);
            }
        }
#endif
    }

    /******************************************************************************/
    // Transform logical and & or operators into conditional expression.
    // a && b  ==>  a ? b : false
    // a || b  ==>  a ? true : b
    vector<SgAndOp*> and_exps = querySubTree<SgAndOp>(func->get_body());
    foreach (SgAndOp* and_op, and_exps)
    {
        if (containsModifyingExpression(and_op->get_rhs_operand()))
        {
            SgConditionalExp* cond = buildConditionalExp(
                    copyExpression(and_op->get_lhs_operand()),
                    copyExpression(and_op->get_rhs_operand()),
                    buildBoolValExp(true));
            replaceExpression(and_op, cond);
        }
    }

    vector<SgOrOp*> or_exps = querySubTree<SgOrOp>(func->get_body());
    foreach (SgOrOp* or_op, or_exps)
    {
        if (containsModifyingExpression(or_op->get_rhs_operand()))
        {
            SgConditionalExp* cond = buildConditionalExp(
                    copyExpression(or_op->get_lhs_operand()),
                    buildBoolValExp(true),
                    copyExpression(or_op->get_rhs_operand()));
            replaceExpression(or_op, cond);
        }
    }
}

void normalizeEvent(SgFunctionDefinition* func)
{
    preprocess(func);

    //Rose_STL_Container<SgNode*> exp_list = NodeQuery::querySubTree(func->get_body(), V_SgExpression, postorder);
    vector<SgExpression*> exp_list = querySubTree<SgExpression>(func->get_body(), postorder);
    foreach (SgExpression* exp, exp_list)
    {
        if (isSgExpression(exp->get_parent()))
        {
            getAndReplaceModifyingExpression(exp);
            //SgVariableDeclaration* decl = getAndReplaceModifyingExpression2(exp);
            //if (decl)
              //  all_temp_decl.push_back(decl);
        }

        exp = propagateCommaOpAndConditionalExp(exp);
        splitCommaOpExp(exp);
    }

    removeUselessBraces(func->get_body());
    removeUselessParen(func->get_body());

#if 0
    Rose_STL_Container<SgNode*> body_list = NodeQuery::querySubTree(func->get_body(), V_SgBasicBlock);
    foreach (SgNode* node, body_list)
    {
        SgBasicBlock* body = isSgBasicBlock(node);
        removeUselessBraces(func->get_body());
    }
#endif
}

void splitCommaOpExp(SgExpression* exp)
{
    if (SgCommaOpExp* comma_op = isSgCommaOpExp(exp))
    {
        SgExpression* lhs = comma_op->get_lhs_operand();
        SgExpression* rhs = comma_op->get_rhs_operand();

        //lhs->set_need_paren(false);
        //rhs->set_need_paren(false);

        SgNode* parent = comma_op->get_parent();

        if (SgStatement* stmt = isSgExprStatement(parent))
        {
            // Note that in Rose, the condition part in if statement can be an expression statement.
            // (a, b);  ==>  a; b;
            if (isSgBasicBlock(stmt->get_parent()))
            {
                SgExprStatement* stmt1 = buildExprStatement(copyExpression(lhs));
                SgExprStatement* stmt2 = buildExprStatement(copyExpression(rhs));
                replaceStatement(stmt, buildBasicBlock(stmt1, stmt2), true);

                splitCommaOpExp(stmt1->get_expression());
                splitCommaOpExp(stmt2->get_expression());
            }

            if (SgIfStmt* if_stmt = isSgIfStmt(stmt->get_parent()))
            {
                SgExprStatement* new_stmt = buildExprStatement(copyExpression(lhs));
                SgExpression* new_exp = copyExpression(rhs);
                replaceExpression(comma_op, new_exp);
                insertStatement(if_stmt, new_stmt);

                splitCommaOpExp(new_stmt->get_expression());
                splitCommaOpExp(new_exp);
            }
        }

        //if (SgVariableDefinition* var_def = isSgVariableDefinition(comma_op->get_parent()))
        if (SgAssignInitializer* ass_init = isSgAssignInitializer(parent))
        {
            // int a = (b, c);  ==>  b; int a = c;
            if (SgVariableDeclaration* var_decl = isSgVariableDeclaration(ass_init->get_parent()->get_parent()))
            {
                // Note that a variable declaration can appear in the condition part of if, for, etc.
                if (isSgBasicBlock(var_decl->get_parent()))
                {
                    SgExprStatement* exp_stmt = buildExprStatement(copyExpression(lhs));
                    SgExpression* new_exp = copyExpression(rhs);
                    replaceExpression(comma_op, new_exp);
                    insertStatement(var_decl, exp_stmt);

                    splitCommaOpExp(exp_stmt->get_expression());
                    splitCommaOpExp(new_exp);
                }
            }
            //FIXME other cases
        }

    }
}

