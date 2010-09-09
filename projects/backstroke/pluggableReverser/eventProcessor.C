#include "eventProcessor.h"
#include <boost/lexical_cast.hpp>
#include <utilities/Utilities.h>

#include <VariableRenaming.h>

#include <utilities/CPPDefinesAndNamespaces.h>


using namespace SageInterface;
using namespace SageBuilder;

vector<EvaluationResult> EventProcessor::evaluateExpression(SgExpression* exp, const VariableVersionTable& var_table, bool is_value_used)
{
	vector<EvaluationResult> results;

	// If two results have the same variable table, we remove the one which has the higher cost.

	foreach(ExpressionReversalHandler* exp_processor, exp_processors_)
	{
		vector<EvaluationResult> res = exp_processor->evaluate(exp, var_table, is_value_used);

		foreach(const EvaluationResult& r1, res)
		{
			bool discard = false;
#if 1
			for (size_t i = 0; i < results.size(); ++i)
			{
				EvaluationResult& r2 = results[i];
				if (r1.getVarTable() == r2.getVarTable())
				{
					if (r1.getCost() > r2.getCost())
					{
						discard = true;
						break;
					} else if (r1.getCost() < r2.getCost())
					{
						results.erase(results.begin() + i);
						--i;
					}
				}
			}
#endif

			if (!discard)
				results.push_back(r1);
		}
		//output.insert(output.end(), result.begin(), result.end());
	}
	return results;
}

vector<EvaluationResult> EventProcessor::evaluateStatement(SgStatement* stmt, const VariableVersionTable& var_table)
{
	vector<EvaluationResult> results;

	// Here we update the variable version table, remove those which are not used any more.
	// Note that we process statements in reverse order in any scope, a variable is not used means
	// it is not used in statements above this point. For example,
	//  1   int t = 0;
	//  2   a = b;
	//  3   c = t;
	// after processing statement 3, we can remove t from the variable version table because it's not
	// useful for our transformation anymore.

	vector<SgExpression*> vars = VariableVersionTable::getAllVariables(stmt);
	vector<SgExpression*> vars_to_remove;
#if 0

	foreach(SgExpression* var, vars)
	{
		if (stmt_pkg.var_table.isUsingFirstUse(var))
			vars_to_remove.push_back(var);
	}
#endif

	foreach(StatementReversalHandler* stmt_processor, stmt_processors_)
	{
		vector<EvaluationResult> res = stmt_processor->evaluate(stmt, var_table);

		foreach(EvaluationResult& r1, res)
		{
			// Remove those variables from variable version table if they are not useful anymore.

			foreach(SgExpression* var, vars_to_remove)
			{
				r1.getVarTable().removeVariable(var);
			}

			// If two results have the same variable table, we remove the one which has the higher cost.
			bool discard = false;
#if 1
			for (size_t i = 0; i < results.size(); ++i)
			{
				EvaluationResult& r2 = results[i];
				if (r1.getVarTable() == r2.getVarTable())
				{
					if (r1.getCost() > r2.getCost())
					{
						discard = true;
						break;
					} else if (r1.getCost() < r2.getCost())
					{
						results.erase(results.begin() + i);
						--i;
					}
				}
			}
#endif

			if (!discard)
				results.push_back(r1);
		}
		//results.insert(results.end(), result.begin(), result.end());
	}
	return results;
}

/**
 * Given a variable and a version, returns an expression evaluating to the value of the variable
 * at the given version.
 *
 * @param variable name of the variable to be restored
 * @param availableVariables variables whos values are currently available
 * @return definitions the version of the variable which should be restored
 */
vector<SgExpression*> EventProcessor::restoreVariable(VariableRenaming::VarName variable, const VariableVersionTable& availableVariables,
				VariableRenaming::NumNodeRenameEntry definitions)
{
	vector<SgExpression*> results;

	//Check if we're already trying to restore this variable to this version. Prevents infinite recursion
	pair<VariableRenaming::VarName, VariableRenaming::NumNodeRenameEntry> variableAndVersion(variable, definitions);
	if (activeValueRestorations.count(variableAndVersion) > 0)
	{
		return results;
	} else
	{
		activeValueRestorations.insert(variableAndVersion);
	}


	//Check if the variable needs restoring at all. If it has the desired version, there is nothing to do
	if (availableVariables.matchesVersion(variable, definitions))
	{
		results.push_back(VariableRenaming::buildVariableReference(variable));
		return results;
	}

	//Call the variable value restoreration handlers

	foreach(VariableValueRestorer* variableRestorer, variableValueRestorers)
	{
		vector<SgExpression*> restorerOutput = variableRestorer->restoreVariable(variable, availableVariables, definitions);

		results.insert(results.end(), restorerOutput.begin(), restorerOutput.end());
	}

	//Remove this variable from the active set
	activeValueRestorations.erase(variableAndVersion);

	//FIXME: Here, just pick the first restorer result. In the future we should use some model to help
	//us choose which result to use
	if (results.size() > 1)
	{
		printf("Warning: Truncating some VariableValueRestorer results\n");
		for (size_t i = 1; i < results.size(); i++)
		{
			SageInterface::deepDelete(results[i]);
		}
		results.resize(1);
	}

	return results;
}

SgExpression* EventProcessor::getStackVar(SgType* type)
{
	string type_name;

	if (isSgTypeInt(type))
		type_name = "int";
	if (isSgTypeBool(type))
		type_name = "bool";
	if (isSgTypeFloat(type))
		type_name = "float";

	string stack_name = event_->get_name() + "_" + type_name + "_stack";
	if (stack_decls_.count(stack_name) == 0)
	{
		SgType* stack_type = buildStructDeclaration("std::stack<" + type_name + ">")->get_type();
		ROSE_ASSERT(stack_type);
		stack_decls_[stack_name] = buildVariableDeclaration(stack_name, stack_type);
	}

	return buildVarRefExp(stack_decls_[stack_name]->get_variables()[0]);
}

bool EventProcessor::isStateVariable(SgExpression* exp)
{
	// First, get the most lhs operand, which may be the model object.
	while (isSgBinaryOp(exp))
		exp = isSgBinaryOp(exp)->get_lhs_operand();

	SgVarRefExp* var = isSgVarRefExp(exp);
	ROSE_ASSERT(var);

	// The pointer parameter of the event function is state.

	// We should 

	foreach(SgInitializedName* name, event_->get_args())
	{
		if (name == var->get_symbol()->get_declaration())
			if (isSgPointerType(name->get_type()) || isReferenceType(name->get_type()))
				return true;
	}

	return false;
}

bool EventProcessor::isStateVariable(const VariableRenaming::VarName& var)
{
	// The pointer parameter of the event function is state.

	foreach(SgInitializedName* name, event_->get_args())
	{
		if (name == var[0])
			if (isSgPointerType(name->get_type()) || isReferenceType(name->get_type()))
				return true;
	}

	return false;
}

std::vector<SgVariableDeclaration*> EventProcessor::getAllStackDeclarations() const
{
	vector<SgVariableDeclaration*> output;
	typedef std::pair<std::string, SgVariableDeclaration*> pair_t;
	foreach(const pair_t& decl_pair, stack_decls_)
	output.push_back(decl_pair.second);
	return output;
}

SgExpression* EventProcessor::pushVal(SgExpression* exp, SgType* type)
{
	return buildFunctionCallExp("push", type, buildExprListExp(
					getStackVar(type), exp));
}

SgExpression* EventProcessor::popVal(SgType* type)
{
	return buildFunctionCallExp("pop", type,
					buildExprListExp(getStackVar(type)));
}

bool EventProcessor::checkVersion(const VariableVersionTable& var_table)
{
	typedef std::map<VariableRenaming::VarName, std::set<int> > TableType;

	foreach(const TableType::value_type& var, var_table.getTable())
	{
		if (isStateVariable(var.first))
		{
			if (var.second.size() != 1 || var.second.count(1) == 0)
			{
				return false;
			}
		}
	}
	return true;
}

FuncDeclPairs EventProcessor::processEvent()
{
	// Before processing, build a variable version table for the event function.
	VariableVersionTable var_table(event_, var_renaming_);

	SgBasicBlock* body =
					isSgFunctionDeclaration(event_->get_definingDeclaration())->get_definition()->get_body();
	FuncDeclPairs outputs;

	SimpleCostModel cost_model;
	vector<EvaluationResult> results = evaluateStatement(body, var_table);


	int ctr = 0;
	// Sort the generated bodies so that those with the least cost appears first.
	sort(results.begin(), results.end());

	foreach(EvaluationResult& res, results)
	{
		// Here we check the validity for each result above. We have to make sure
		// every state variable has the version 1.
		if (!checkVersion(res.getVarTable()))
			continue;
		

		StatementReversal stmt = res.generateReverseAST(body);

		fixVariableReferences(stmt.fwd_stmt);
		fixVariableReferences(stmt.rvs_stmt);

		string ctr_str = lexical_cast<string> (ctr++);

		SgName fwd_func_name = event_->get_name() + "_forward" + ctr_str;
		SgFunctionDeclaration* fwd_func_decl =
						buildDefiningFunctionDeclaration(
						fwd_func_name, event_->get_orig_return_type(),
						isSgFunctionParameterList(copyStatement(event_->get_parameterList())));
		SgFunctionDefinition* fwd_func_def = fwd_func_decl->get_definition();
		SageInterface::replaceStatement(fwd_func_def->get_body(), isSgBasicBlock(stmt.fwd_stmt));

		SgName rvs_func_name = event_->get_name() + "_reverse" + ctr_str;
		SgFunctionDeclaration* rvs_func_decl =
						buildDefiningFunctionDeclaration(
						rvs_func_name, event_->get_orig_return_type(),
						isSgFunctionParameterList(copyStatement(event_->get_parameterList())));
		SgFunctionDefinition* rvs_func_def = rvs_func_decl->get_definition();
		SageInterface::replaceStatement(rvs_func_def->get_body(), isSgBasicBlock(stmt.rvs_stmt));


		// Add the cost information as comments to generated functions.
		string comment = "Cost: " + lexical_cast<string> (res.getCost().getCost());
		attachComment(fwd_func_decl, comment);
		attachComment(rvs_func_decl, comment);

		outputs.push_back(FuncDeclPair(fwd_func_decl, rvs_func_decl));
	}

	return outputs;
}

