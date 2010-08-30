#include "ifStatementProcessor.h"
#include <boost/tuple/tuple.hpp>

using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;



StatementReversal IfStatementProcessor::process(SgStatement* stmt)
{
    SgIfStmt* if_stmt = isSgIfStmt(stmt);
    ROSE_ASSERT(if_stmt);

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

    appendStatement(buildExprStatement(
            pushVal(buildBoolValExp(true), buildBoolType())),
            fwd_true_block_body);
    appendStatement(buildExprStatement(
            pushVal(buildBoolValExp(false), buildBoolType())),
            fwd_false_block_body);

    SgExpression* rvs_cond_exp = popVal(buildBoolType());

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

    SgStatement* fwd_stmt = buildIfStmt(proc_cond.fwd_stmt, proc_true_body.fwd_stmt, proc_false_body.fwd_stmt);
    SgStatement* rvs_stmt = buildIfStmt(buildExprStatement(popVal(buildBoolType())), proc_true_body.rvs_stmt, proc_false_body.rvs_stmt);
    rvs_stmt = buildBasicBlock(rvs_stmt, proc_cond.rvs_stmt);

    return StatementReversal(fwd_stmt, rvs_stmt);
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

    foreach (const EvaluationResult& res1, false_body_res)
    {
        vector<EvaluationResult> true_body_res =
                evaluateStatement(if_stmt->get_true_body(), var_table);
        foreach (const EvaluationResult& res2, true_body_res)
        {
            EvaluationResult new_res = res1;
            new_res.update(res2);

            vector<EvaluationResult> cond_results = evaluateStatement(if_stmt->get_conditional(), var_table);

            foreach (const EvaluationResult& res3, cond_results)
            {
                EvaluationResult new_res2 = new_res;
                new_res2.update(res3);
                results.push_back(new_res2);
            }
        }
    }

    return results;
}
