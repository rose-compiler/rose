#include "ifStatementHandler.h"
#include "pluggableReverser/variableVersionTable.h"
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>

using namespace std;
using namespace boost;
using namespace SageInterface;
using namespace SageBuilder;

#define foreach BOOST_FOREACH



StatementReversal IfStatementHandler::generateReverseAST(SgStatement* stmt, const EvaluationResult& evalResult)
{
	ROSE_ASSERT(evalResult.getChildResults().size() == 3);
    SgIfStmt* if_stmt = isSgIfStmt(stmt);
    ROSE_ASSERT(if_stmt);

    // Get the attribute which tells us whether to store the branch flag.
    EvaluationResultAttributePtr attr = evalResult.getAttribute();
    ROSE_ASSERT(attr);
    IfStmtConditionAttribute* cond_attr = dynamic_cast<IfStmtConditionAttribute*>(attr.get());
    ROSE_ASSERT(cond_attr);

    StatementReversal proc_cond = evalResult.getChildResults()[2].generateReverseStatement();

    StatementReversal proc_true_body = evalResult.getChildResults()[1].generateReverseStatement();
    StatementReversal proc_false_body = evalResult.getChildResults()[0].generateReverseStatement();

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
}

vector<EvaluationResult> IfStatementHandler::evaluate(SgStatement* stmt, const VariableVersionTable& var_table)
{
    vector<EvaluationResult> results;
    SgIfStmt* if_stmt = isSgIfStmt(stmt);
    if (if_stmt == NULL)
        return results;

    // Make sure every if statement has a true and false body after being normalized.
    ROSE_ASSERT(if_stmt->get_false_body());

	SgStatement* true_body = if_stmt->get_true_body();
	SgStatement* false_body = if_stmt->get_false_body();
	VariableVersionTable true_body_var_table, false_body_var_table;
	tie(true_body_var_table, false_body_var_table) = var_table.getVarTablesForIfBodies(true_body, false_body);

	cout << "true_body_var_table:\n";
	true_body_var_table.print();
	cout << "false_body_var_table:\n";
	false_body_var_table.print();

    vector<EvaluationResult> true_body_res = evaluateStatement(true_body, true_body_var_table);
    vector<EvaluationResult> false_body_res = evaluateStatement(false_body, false_body_var_table);

    SimpleCostModel cost;

#if 0
    cout << "Original versions:\n";
    var_table.print();
    cout << endl;
#endif
	
    foreach (const EvaluationResult& res1, false_body_res)
    {
        // Set the cost of true body.
        cost.setBranchCost(if_stmt, res1.getCost(), true);

        foreach (const EvaluationResult& res2, true_body_res)
        {
            // Set the cost of false body.
            cost.setBranchCost(if_stmt, res2.getCost(), false);

            //EvaluationResult new_res = res1;

            // FIXME Note here we cannot use update function. We may create a new function
            // to combine variable tables from true and false bodies.
            //new_res.update(res2);

            // Here we merge two variable version table from true and false bodies.
            // For each variable, if it has the different versions from those two tables, we
            // clear its version (it has no version now).
            VariableVersionTable new_table = res1.getVarTable();
            new_table.setUnion(res2.getVarTable());
            //new_res.setVarTable(new_table);
			
#if 0
            cout << "True:\n";
            res2.getVarTable().print();
            cout << "False:\n";
            res1.getVarTable().print();
            cout << endl;
            
            cout << "New versions:\n";
            new_table.print();
            cout << endl;
#endif

            vector<EvaluationResult> cond_results = evaluateStatement(if_stmt->get_conditional(), new_table);

            foreach (const EvaluationResult& res3, cond_results)
            {
				EvaluationResult totalEvaluationResult(this, stmt, new_table);
				totalEvaluationResult.addChildEvaluationResult(res1);
				totalEvaluationResult.addChildEvaluationResult(res2);
				totalEvaluationResult.addChildEvaluationResult(res3);

				// FIXME Should addChildEvaluationResult update var table?
				totalEvaluationResult.setVarTable(new_table);

                // Here we should do an analysis to decide whether to store the branch flag.
                // If the value of the condition is not modified during both true and false bodies,
                // we can still use that expression. Or we can retieve the same value from other
                // expressions. Even more comlicated analysis may be performed here.

                SimpleCostModel new_cost = cost;
                IfStmtConditionAttributePtr attr(new IfStmtConditionAttribute);

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

                totalEvaluationResult.setCost(new_cost);
                totalEvaluationResult.setAttribute(attr);

                results.push_back(totalEvaluationResult);
            }
        }
    }

    return results;
}