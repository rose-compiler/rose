#include "statementProcessor.h"
#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>
#include "utilities/CPPDefinesAndNamespaces.h"

using namespace SageInterface;
using namespace SageBuilder;

#if 0
InstrumentedStatementVec BasicStatementProcessor::process(const StatementPackage& stmt_pkg)
{
    if (isSgExprStatement(stmt_pkg.stmt))
        return processExprStatement(stmt_pkg);

    else if (isSgVariableDeclaration(stmt_pkg.stmt))
        return processVariableDeclaration(stmt_pkg);

    else if (isSgBasicBlock(stmt_pkg.stmt))
        return processBasicBlock(stmt_pkg);

        //The forward of a return statement is a return; the reverse is a no-op.
    else if (isSgReturnStmt(stmt_pkg.stmt))
        return processReturnStatement(stmt_pkg);

        //if (SgIfStmt* if_stmt = isSgIfStmt(stmt))
        // return processIfStmt(if_stmt, var_table);

    return InstrumentedStatementVec();
}

InstrumentedStatementVec BasicStatementProcessor::processReturnStatement(const StatementPackage& stmt_pkg)
{
    SgReturnStmt* return_stmt = isSgReturnStmt(stmt_pkg.stmt);
    ROSE_ASSERT(return_stmt);

    InstrumentedStatementVec stmts;
    stmts.push_back(InstrumentedStatement(copyStatement(return_stmt), NULL, stmt_pkg.var_table));
    return stmts;
}
#endif

StatementReversal ExprStatementProcessor::process(SgStatement* stmt) 
{
    SgExprStatement* exp_stmt = isSgExprStatement(stmt);
    ROSE_ASSERT(exp_stmt);
    
    ExpressionReversal exp = processExpression(exp_stmt->get_expression());

    SgStatement *fwd_stmt = NULL, *rvs_stmt = NULL;

    if (exp.fwd_exp)
        fwd_stmt = buildExprStatement(exp.fwd_exp);
    if (exp.rvs_exp)
        rvs_stmt = buildExprStatement(exp.rvs_exp);

    return StatementReversal(fwd_stmt, rvs_stmt);
}

vector<EvaluationResult> ExprStatementProcessor::evaluate(SgStatement* stmt, const VariableVersionTable& var_table)
{
    vector<EvaluationResult> results;
    SgExprStatement* exp_stmt = isSgExprStatement(stmt);
    if (exp_stmt == NULL)
        return results;
    
    results = evaluateExpression(
            exp_stmt->get_expression(), var_table);

    ROSE_ASSERT(!results.empty());

    return results;
}


#if 0
InstrumentedStatementVec BasicStatementProcessor::processVariableDeclaration(const StatementPackage& stmt_pkg)
{
    SgVariableDeclaration* var_decl = isSgVariableDeclaration(stmt_pkg.stmt);
    ROSE_ASSERT(var_decl);

    InstrumentedStatementVec outputs;

    // Note the store and restore of local variables are processd in
    // basic block, not here. We just forward the declaration to forward
    // event function.

    // FIXME copyStatement also copies preprocessing info
    outputs.push_back(InstrumentedStatement(copyStatement(var_decl), NULL, stmt_pkg.var_table));

    //outputs.push_back(InstrumentedStatement(NULL, NULL, var_table));
    //outputs.push_back(pushAndPopLocalVar(var_decl));

    // FIXME  other cases
    
    return outputs;
}
#endif

StatementReversal BasicBlockProcessor::process(SgStatement* stmt) 
{
    SgBasicBlock* body = isSgBasicBlock(stmt);
    ROSE_ASSERT(body);

    SgBasicBlock* fwd_body = buildBasicBlock();
    SgBasicBlock* rvs_body = buildBasicBlock();

    foreach (SgStatement* stmt, body->get_statements())
    {
        StatementReversal proc_stmt = processStatement(stmt);

        if (proc_stmt.fwd_stmt)
            appendStatement(proc_stmt.fwd_stmt, fwd_body);
        if (proc_stmt.rvs_stmt)
            prependStatement(proc_stmt.rvs_stmt, rvs_body);
    }

    return StatementReversal(fwd_body, rvs_body);
}


vector<EvaluationResult> BasicBlockProcessor::evaluate(SgStatement* stmt, const VariableVersionTable& var_table)
{
    vector<EvaluationResult> results;
    
    SgBasicBlock* body = isSgBasicBlock(stmt);
    if (body == NULL)
        return results;

    cout << body->get_statements().size() << endl;
    if (body->get_statements().empty())
    {
        results.push_back(EvaluationResult(var_table));
        return results;
    }

    // Use two vectors to store intermediate results.
    vector<EvaluationResult> queue[2];

    int i = 0;
    queue[i].push_back(EvaluationResult(var_table));

    reverse_foreach (SgStatement* stmt, body->get_statements())
    {
        foreach (EvaluationResult& result, queue[i])
        {
            vector<EvaluationResult> results = evaluateStatement(stmt, result.getVarTable());
            
            ROSE_ASSERT(!results.empty());

            foreach (EvaluationResult& res, results)
            {
                // Update the result.
                EvaluationResult new_result(result);
                new_result.update(res);
                queue[1-i].push_back(new_result);
            }
        }
        
        queue[i].clear();
        // Switch the index between 0 and 1.
        i = 1 - i;
    }

#if 0
    // Remove all local variables from variable version table since we will not use them anymore. 
    // This is helpful to prune branches by comparing variable version tables. 
    foreach (EvaluationResult& result, queue[i])
    {
        foreach (SgInitializedName* var, local_vars)
            result.var_table.removeVariable(var);
    }
#endif

    return queue[i];
}
