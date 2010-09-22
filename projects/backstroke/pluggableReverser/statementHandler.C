#include "statementHandler.h"
#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>
#include "utilities/CPPDefinesAndNamespaces.h"

using namespace SageInterface;
using namespace SageBuilder;

StatementReversal CombinatorialExprStatementHandler::generateReverseAST(SgStatement* stmt, const EvaluationResult& evaluationResult)
{
    SgExprStatement* exp_stmt = isSgExprStatement(stmt);
    ROSE_ASSERT(exp_stmt);
	ROSE_ASSERT(evaluationResult.getStatementHandler() == this);
	ROSE_ASSERT(evaluationResult.getChildResults().size() == 1);
    
    ExpressionReversal exp = evaluationResult.getChildResults().front().generateReverseExpression();

    SgStatement *fwd_stmt = NULL, *rvs_stmt = NULL;

    if (exp.fwd_exp)
        fwd_stmt = buildExprStatement(exp.fwd_exp);
    if (exp.rvs_exp)
        rvs_stmt = buildExprStatement(exp.rvs_exp);

    return StatementReversal(fwd_stmt, rvs_stmt);
}

vector<EvaluationResult> CombinatorialExprStatementHandler::evaluate(SgStatement* stmt, const VariableVersionTable& var_table)
{
    vector<EvaluationResult> results;
    SgExprStatement* exp_stmt = isSgExprStatement(stmt);
    if (exp_stmt == NULL)
        return results;

	vector<EvaluationResult> potentialExprReversals = evaluateExpression(exp_stmt->get_expression(), var_table, false);

	foreach(const EvaluationResult& potentialExprReversal, potentialExprReversals)
	{
		EvaluationResult statementResult(this, stmt, var_table);
		statementResult.addChildEvaluationResult(potentialExprReversal);
		results.push_back(statementResult);
	}

    ROSE_ASSERT(!results.empty());

    return results;
}

StatementReversal CombinatorialBasicBlockHandler::generateReverseAST(SgStatement* stmt, const EvaluationResult& evaluationResult)
{
    SgBasicBlock* body = isSgBasicBlock(stmt);
    ROSE_ASSERT(body);
	ROSE_ASSERT(evaluationResult.getChildResults().size() == body->get_statements().size());

    SgBasicBlock* fwd_body = buildBasicBlock();
    SgBasicBlock* rvs_body = buildBasicBlock();

	// Handle all declarations of local variables first.
	foreach (SgStatement* stmt, body->get_statements())
	{
		if (SgVariableDeclaration* var_decl = isSgVariableDeclaration(stmt))
		{
			foreach (SgInitializedName* init_name, var_decl->get_variables())
			{
				LocalVarRestoreAttribute* attr =
						dynamic_cast<LocalVarRestoreAttribute*> (evaluationResult.getAttribute().get());

				ROSE_ASSERT(attr->local_var_restorer.count(init_name) > 0);

				if (attr->local_var_restorer[init_name].first)
				{
					if (attr->local_var_restorer[init_name].second)
					{
						// Retrieve its value from another expression.
						SgStatement* decl = buildVariableDeclaration(
								init_name->get_name(),
								init_name->get_type(),
								buildAssignInitializer(copyExpression(attr->local_var_restorer[init_name].second)),
								rvs_body);

						appendStatement(decl, rvs_body);
					}
					else
					{
						// Store and restore this local variable using stack.
						
						// Store the value of local variables at the end of the basic block.
						SgVarRefExp* var_stored = buildVarRefExp(init_name->get_name());
						SgStatement* store_var = buildExprStatement(
								pushVal(var_stored, init_name->get_type()));

						// Retrieve the value which is used to initialize that local variable.
						SgVariableDeclaration* decl_restore_var = buildVariableDeclaration(
								init_name->get_name(),
								init_name->get_type(),
								buildAssignInitializer(popVal(init_name->get_type())),
								isSgBasicBlock(rvs_body));

						appendStatement(store_var, fwd_body);
						appendStatement(decl_restore_var, rvs_body);
					}
				}
				else
				{
					SgStatement* just_decl = buildVariableDeclaration(init_name->get_name(), init_name->get_type(), NULL, rvs_body);

					appendStatement(just_decl, rvs_body);
				}
			}
		}
	}


	foreach(const EvaluationResult& childResult, evaluationResult.getChildResults())
    {
        StatementReversal proc_stmt = childResult.generateReverseStatement();

        if (proc_stmt.fwd_stmt)
            prependStatement(proc_stmt.fwd_stmt, fwd_body);
        if (proc_stmt.rvs_stmt)
            appendStatement(proc_stmt.rvs_stmt, rvs_body);
    }

    return StatementReversal(fwd_body, rvs_body);
}


vector<EvaluationResult> CombinatorialBasicBlockHandler::evaluate(SgStatement* stmt, const VariableVersionTable& var_table)
{
    vector<EvaluationResult> results;
	VariableVersionTable new_var_table = var_table;
	vector<SgInitializedName*> local_vars;
    
    SgBasicBlock* body = isSgBasicBlock(stmt);
    if (body == NULL)
        return results;

    //cout << body->get_statements().size() << endl;
    if (body->get_statements().empty())
    {
        results.push_back(EvaluationResult(this, stmt, var_table));
        return results;
    }
	
    // Use two vectors to store intermediate results.
    vector<EvaluationResult> queue[2];
    int i = 0;

	// Set the initial result and push it into the first vector.
	EvaluationResult init_res(this, stmt, new_var_table);
	init_res.setAttribute(LocalVarRestoreAttributePtr(new LocalVarRestoreAttribute));
    queue[i].push_back(init_res);

	// For each local variable, we try to restore it using akgul's method first. If we cannot get its
	// final value for free, we should consider whether to store its value in forward event. We use attribute
	// to record our selections.
	foreach (SgStatement* stmt, body->get_statements())
	{
		if (SgVariableDeclaration* var_decl = isSgVariableDeclaration(stmt))
		{
			foreach (SgInitializedName* init_name, var_decl->get_variables())
			{
				foreach (EvaluationResult& res, queue[i])
				{
					LocalVarRestoreAttribute attr =
							*dynamic_cast<LocalVarRestoreAttribute*> (res.getAttribute().get());

					//First, check if we can restore the variable without savings its value.
					VariableRenaming::VarName var_name;
					var_name.push_back(init_name);
					//cout << "!!!" << VariableRenaming::keyToString(var_name) << ":" << getLastVersion(init_name).begin()->first << endl;
					//res.getVarTable().print();
					SgExpression* restored_value = restoreVariable(var_name, res.getVarTable(), getLastVersion(init_name));

					if (restored_value != NULL)
					{
						cout << "Retrieving value from " << get_name(restored_value) << endl;
						EvaluationResult new_res = res;
						attr.local_var_restorer[init_name] = make_pair(true, restored_value);
						new_res.setAttribute(LocalVarRestoreAttributePtr(new LocalVarRestoreAttribute(attr)));
						// Remember to update the version of this variable.
						new_res.getVarTable().setLastVersion(init_name);
						queue[1 - i].push_back(new_res);

						//new_res.getVarTable().print();
					} 
					else
					{
						/****************************************************************************************/
						// Here we choose not to restore its value.
						EvaluationResult new_res1 = res;
						attr.local_var_restorer[init_name] = make_pair(false, static_cast<SgExpression*> (NULL));
						new_res1.setAttribute(LocalVarRestoreAttributePtr(new LocalVarRestoreAttribute(attr)));
						queue[1 - i].push_back(new_res1);


						/****************************************************************************************/
						// Here we choose to restore its value.
						EvaluationResult new_res2 = res;
						attr.local_var_restorer[init_name] = make_pair(true, static_cast<SgExpression*> (NULL));
						new_res2.setAttribute(LocalVarRestoreAttributePtr(new LocalVarRestoreAttribute(attr)));

						// Assign the correct version to this variable and add the cost by 1.
						new_res2.getVarTable().setLastVersion(init_name);

						SimpleCostModel cost = new_res2.getCost();
						cost.increaseStoreCount(1);
						new_res2.setCost(cost);

						queue[1 - i].push_back(new_res2);
					}
				}
				queue[i].clear();
				// Switch the index between 0 and 1.
				i = 1 - i;
				//var_table.setNullVersion(init_name);
			}
		}
	}

    reverse_foreach (SgStatement* stmt, body->get_statements())
    {
        foreach (const EvaluationResult& existingPartialResult, queue[i])
        {
            vector<EvaluationResult> results = evaluateStatement(stmt, existingPartialResult.getVarTable());
            
            ROSE_ASSERT(!results.empty());

            foreach (const EvaluationResult& res, results)
            {
                // Update the result.
                EvaluationResult new_result(existingPartialResult);
				new_result.addChildEvaluationResult(res);
                queue[1-i].push_back(new_result);
            }
        }
        queue[i].clear();
        // Switch the index between 0 and 1.
        i = 1 - i;
    }

    // Remove all local variables from variable version table since we will not use them anymore. 
    // This is helpful to prune branches by comparing variable version tables. 
    foreach (EvaluationResult& result, queue[i])
    {
        foreach (SgInitializedName* var, local_vars)
            result.getVarTable().removeVariable(var);
    }

    return queue[i];
}


VariableRenaming::NumNodeRenameEntry CombinatorialBasicBlockHandler::getLastVersion(SgInitializedName* init_name)
{
	VariableRenaming::VarName var_name;
	var_name.push_back(init_name);
	SgFunctionDefinition* enclosing_func = SageInterface::getEnclosingFunctionDefinition(init_name->get_declaration());
	return getVariableRenaming()->getReachingDefsAtFunctionEndForName(enclosing_func, var_name);
}

StatementReversal NullStatementHandler::generateReverseAST(SgStatement* stmt, const EvaluationResult& evaluationResult)
{
	ROSE_ASSERT(evaluationResult.getStatementHandler() == this && evaluationResult.getChildResults().empty());
	return StatementReversal(NULL, NULL);
}

vector<EvaluationResult> NullStatementHandler::evaluate(SgStatement* stmt, const VariableVersionTable& var_table)
{
	vector<EvaluationResult> results;
	if (isSgNullStatement(stmt))
	{
		results.push_back(EvaluationResult(this, stmt, var_table));
	}
	return results;
}