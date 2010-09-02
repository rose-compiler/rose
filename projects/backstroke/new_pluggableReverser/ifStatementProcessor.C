#include "ifStatementProcessor.h"
#include "pluggableReverser/variableVersionTable.h"
#include <boost/tuple/tuple.hpp>

using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;

struct IfStmtConditionAttribute : public EvaluationResultAttribute
{
    IfStmtConditionAttribute() : cond(NULL) {}
    SgExpression* cond;
};

StatementReversal IfStatementProcessor::process(SgStatement* stmt)
{
    SgIfStmt* if_stmt = isSgIfStmt(stmt);
    ROSE_ASSERT(if_stmt);

    // Get the attribute which tells us whether to store the branch flag.
    EvaluationResultAttribute* attr = getAttribute();
    ROSE_ASSERT(attr);
    IfStmtConditionAttribute* cond_attr = dynamic_cast<IfStmtConditionAttribute*>(attr);
    ROSE_ASSERT(cond_attr);

    SgStatement* cond = if_stmt->get_conditional();
    StatementReversal proc_cond = processStatement(cond);

    SgStatement* true_body = if_stmt->get_true_body();
    SgStatement* false_body = if_stmt->get_false_body();

    StatementReversal proc_true_body = processStatement(true_body);
    StatementReversal proc_false_body = processStatement(false_body);

    SgBasicBlock* fwd_true_block_body =  isSgBasicBlock(proc_true_body.fwd_stmt);
    SgBasicBlock* fwd_false_block_body = isSgBasicBlock(proc_false_body.fwd_stmt);

    ROSE_ASSERT(fwd_true_block_body);
    ROSE_ASSERT(fwd_false_block_body);

    SgStatement* cond_stmt;
    if (cond_attr->cond)
        cond_stmt = buildExprStatement(copyExpression(cond_attr->cond));
    else
    {
        // In this situation, we don't have to store branch flag.
        appendStatement(buildExprStatement(
                pushVal(buildBoolValExp(true), buildBoolType())),
                fwd_true_block_body);
        appendStatement(buildExprStatement(
                pushVal(buildBoolValExp(false), buildBoolType())),
                fwd_false_block_body);
        cond_stmt = buildExprStatement(popVal(buildBoolType()));
    }

    SgStatement* fwd_stmt = buildIfStmt(proc_cond.fwd_stmt, proc_true_body.fwd_stmt, proc_false_body.fwd_stmt);
    SgStatement* rvs_stmt = buildIfStmt(cond_stmt, proc_true_body.rvs_stmt, proc_false_body.rvs_stmt);
    rvs_stmt = buildBasicBlock(rvs_stmt, proc_cond.rvs_stmt);

    return StatementReversal(fwd_stmt, rvs_stmt);

    // Do not switch the position of the following two statements to the one above;
    // make sure the current flag is used before generating new statement.

#if 0
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
#endif
}

vector<EvaluationResult> IfStatementProcessor::evaluate(SgStatement* stmt, const VariableVersionTable& var_table)
{
    vector<EvaluationResult> results;
    SgIfStmt* if_stmt = isSgIfStmt(stmt);
    if (if_stmt == NULL)
        return results;

    // Make sure every if statement has a true and false body after being normalized.
    ROSE_ASSERT(if_stmt->get_false_body());

    vector<EvaluationResult> false_body_res =
            evaluateStatement(if_stmt->get_false_body(), var_table);
    vector<EvaluationResult> true_body_res =
            evaluateStatement(if_stmt->get_true_body(), var_table);

    SimpleCostModel cost;

    cout << "Original versions:\n";
    var_table.print();
    cout << endl;

    foreach (const EvaluationResult& res1, false_body_res)
    {
        // Set the cost of true body.
        cost.setBranchCost(if_stmt, res1.getCost(), true);

        foreach (const EvaluationResult& res2, true_body_res)
        {
            // Set the cost of false body.
            cost.setBranchCost(if_stmt, res2.getCost(), false);

            EvaluationResult new_res = res1;

            // FIXME Note here we cannot use update function. We may create a new function
            // to combine variable tables from true and false bodies.
            new_res.update(res2);

            // Here we merge two variable version table from true and false bodies.
            // For each variable, if it has the different versions from those two tables, we
            // clear its version (it has no version now).
            VariableVersionTable new_table = res1.getVarTable();
            new_table.intersect(res2.getVarTable());
            new_res.setVarTable(new_table);

            cout << "True:\n";
            res2.getVarTable().print();
            cout << "False:\n";
            res1.getVarTable().print();
            cout << endl;
            
            cout << "New versions:\n";
            new_table.print();
            cout << endl;

            vector<EvaluationResult> cond_results = evaluateStatement(if_stmt->get_conditional(), var_table);

            foreach (const EvaluationResult& res3, cond_results)
            {
                EvaluationResult new_res2 = new_res;
                new_res2.update(res3);

                // Here we should do an analysis to decide whether to store the branch flag.
                // If the value of the condition is not modified during both true and false bodies,
                // we can still use that expression. Or we can retieve the same value from other
                // expressions. Even more comlicated analysis may be performed here.

                SimpleCostModel new_cost = cost;
                IfStmtConditionAttribute* attr = new IfStmtConditionAttribute;

                SgExpression* cond = isSgExprStatement(if_stmt->get_conditional())->get_expression();
                if (cond && var_table.checkVersionForUse(cond))
                {
                    attr->cond = cond;
                }
                else
                {
                    // Since we store the branch flag here, we add the cost by 1.
                    new_cost.increaseStoreCount();
                }

                new_res2.setCost(new_cost);
                new_res2.addAttribute(attr);

                results.push_back(new_res2);
            }
        }
    }

    return results;
}
