#include "eventProcessor.h"
#include <boost/lexical_cast.hpp>
#include <utilities/utilities.h>

#include <VariableRenaming.h>

#include <utilities/cppDefinesAndNamespaces.h>


using namespace std;
using namespace boost;
using namespace SageInterface;
using namespace SageBuilder;

void EventProcessor::addExpressionHandler(ExpressionReversalHandler* exp_handler)
{
	exp_handler->setEventHandler(this);
	exp_handlers_.push_back(exp_handler);
}

void EventProcessor::addStatementHandler(StatementReversalHandler* stmt_handler)
{
	stmt_handler->setEventHandler(this);
	stmt_handlers_.push_back(stmt_handler);
}

void EventProcessor::addVariableValueRestorer(VariableValueRestorer* restorer)
{
	restorer->setEventHandler(this);
	variableValueRestorers.push_back(restorer);
}

FuncDeclPairs EventProcessor::processEvent(SgFunctionDeclaration* event)
{
	event_ = event;
	return processEvent();
}


vector<EvaluationResult> EventProcessor::filterResults(const vector<EvaluationResult>& results)
{
	//// Temporarily do not filter results here.
	//return results;
	
	set<size_t> discarded_idx;
	for (size_t i = 0; i < results.size(); ++i)
	{
		for (size_t j = i + 1; j < results.size(); ++j)
		{
			if (results[i].getVarTable() == results[j].getVarTable())
			{
				if (results[i].getCost() < results[j].getCost())
					discarded_idx.insert(j);
				if (results[j].getCost() < results[i].getCost())
					discarded_idx.insert(i);
			}
		}
	}

	vector<EvaluationResult> new_results;
	for (size_t i = 0; i < results.size(); ++i)
	{
		if (discarded_idx.count(i) == 0)
			new_results.push_back(results[i]);
	}
	return new_results;
}

vector<EvaluationResult> EventProcessor::evaluateExpression(SgExpression* exp, const VariableVersionTable& var_table, bool is_value_used)
{
	vector<EvaluationResult> results;

	foreach(ExpressionReversalHandler* exp_handler, exp_handlers_)
	{
		vector<EvaluationResult> res = exp_handler->evaluate(exp, var_table, is_value_used);

		foreach(const EvaluationResult& r1, res)
		{
			ROSE_ASSERT(r1.getExpressionInput() == exp);
			results.push_back(r1);
		}
		//output.insert(output.end(), result.begin(), result.end());
	}
	// If two results have the same variable table, we remove the one which has the higher cost.
	return filterResults(results);
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

	vector<SgExpression*> vars = BackstrokeUtility::getAllVariables(stmt);
	vector<SgExpression*> vars_to_remove;
#if 0

	foreach(SgExpression* var, vars)
	{
		if (stmt_pkg.var_table.isUsingFirstUse(var))
			vars_to_remove.push_back(var);
	}
#endif

	foreach(StatementReversalHandler* stmt_handler, stmt_handlers_)
	{
		vector<EvaluationResult> res = stmt_handler->evaluate(stmt, var_table);

		foreach(EvaluationResult& r1, res)
		{
			ROSE_ASSERT(r1.getStatementInput() == stmt);
			// Remove those variables from variable version table if they are not useful anymore.
			foreach(SgExpression* var, vars_to_remove)
			{
				r1.getVarTable().removeVariable(var);
			}
			results.push_back(r1);
		}
		//results.insert(results.end(), result.begin(), result.end());
	}

	// If two results have the same variable table, we remove the one which has the higher cost.
	return filterResults(results);
}


SgExpression* EventProcessor::restoreVariable(VariableRenaming::VarName variable, const VariableVersionTable& availableVariables,
				VariableRenaming::NumNodeRenameEntry definitions)
{
	vector<SgExpression*> results;

	//Check if we're already trying to restore this variable to this version. Prevents infinite recursion
	pair<VariableRenaming::VarName, VariableRenaming::NumNodeRenameEntry> variableAndVersion(variable, definitions);
	if (activeValueRestorations.count(variableAndVersion) > 0)
	{
		return NULL;
	}
	else
	{
		activeValueRestorations.insert(variableAndVersion);
	}


	//Check if the variable needs restoring at all. If it has the desired version, there is nothing to do
	if (availableVariables.matchesVersion(variable, definitions))
	{
		results.push_back(VariableRenaming::buildVariableReference(variable));
	}
	else
	{
		//Call the variable value restoreration handlers
		foreach(VariableValueRestorer* variableRestorer, variableValueRestorers)
		{
			vector<SgExpression*> restorerOutput = variableRestorer->restoreVariable(variable, availableVariables, definitions);

			results.insert(results.end(), restorerOutput.begin(), restorerOutput.end());
		}
	}

	//Remove this variable from the active set
	activeValueRestorations.erase(variableAndVersion);

	//FIXME: Here, just pick the first restorer result. In the future we should use some model to help
	//us choose which result to use
	if (results.size() > 1)
	{
		for (size_t i = 1; i < results.size(); i++)
		{
			SageInterface::deepDelete(results[i]);
		}
		results.resize(1);
	}

	return results.empty() ? NULL : results.front();
}

SgExpression* EventProcessor::getStackVar(SgType* type)
{
	string type_name;

	if (isSgTypeInt(type))
		type_name = "int";
	else if (isSgTypeBool(type))
		type_name = "bool";
	else if (isSgTypeFloat(type))
		type_name = "float";
	else
		type_name = "any";

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
	VariableRenaming::VarName var_name = VariableRenaming::getVarName(exp);
	if (var_name.empty())
		return false;
	return isStateVariable(var_name);
}

bool EventProcessor::isStateVariable(const VariableRenaming::VarName& var)
{
	// Currently we assume all variables except those defined inside the event function
	// are state varibles.
	return !SageInterface::isAncestor(
			BackstrokeUtility::getFunctionBody(event_),
			var[0]->get_declaration());

#if 0
	foreach(SgInitializedName* name, event_->get_args())
	{
		if (name == var[0])
			if (isSgPointerType(name->get_type()) || isReferenceType(name->get_type()))
				return true;
	}

	return false;
#endif
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
	return buildFunctionCallExp("pop< " + get_type_name(type) + " >", type,
					buildExprListExp(getStackVar(type)));
}

bool EventProcessor::checkForInitialVersions(const VariableVersionTable& var_table)
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

	//cout << "VVT:\n";
	//var_table.print();

	SgBasicBlock* body = isSgFunctionDeclaration(event_->get_definingDeclaration())->get_definition()->get_body();
	FuncDeclPairs outputs;

	SimpleCostModel cost_model;
	vector<EvaluationResult> results = evaluateStatement(body, var_table);


	// Sort the generated bodies so that those with the least cost appear first.
	sort(results.begin(), results.end());

	int ctr = 0;
	foreach(EvaluationResult& res, results)
	{
		// Here we check the validity for each result above. We have to make sure
		// every state variable has the version 1.
		if (!checkForInitialVersions(res.getVarTable()))
			;//continue;

		// Print all handlers used in this result.
		if (SgProject::get_verbose() > 0 )
		{
			res.printHandlers();
			res.getCost().print();
			res.getVarTable().print();
		}

		StatementReversal stmt = res.generateReverseStatement();

		// Normalize the result.
		BackstrokeUtility::removeUselessBraces(stmt.fwd_stmt);
		BackstrokeUtility::removeUselessBraces(stmt.rvs_stmt);
		BackstrokeUtility::removeUselessParen(stmt.fwd_stmt);
		BackstrokeUtility::removeUselessParen(stmt.rvs_stmt);

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


SgExpression* EventProcessor::restoreExpressionValue(SgExpression* expression, const VariableVersionTable& availableVariables)
{
	ROSE_ASSERT(expression != NULL);
	//Right now, if the expression has side effects we just assume we can't reevaluate it
	if (BackstrokeUtility::containsModifyingExpression(expression))
	{
		return NULL;
	}

	//Ok, so the expression has no side effects! We can just re-execute it
	//However, the variables used in the expression might have been changed between its location and the current node
	VariableRenaming::NumNodeRenameTable variablesInExpression = var_renaming_->getOriginalUsesAtNode(expression);

	//Go through all the variables used in the definition expression and check if their values have changed
	pair<VariableRenaming::VarName, VariableRenaming::NumNodeRenameEntry> nameDefinitionPair;
	SgExpression* expressionCopy = SageInterface::copyExpression(expression);

	foreach(nameDefinitionPair, variablesInExpression)
	{
		VariableRenaming::NumNodeRenameEntry originalVarVersion = nameDefinitionPair.second;

		if (!availableVariables.matchesVersion(nameDefinitionPair.first, originalVarVersion))
		{
			printf("Recursively restoring variable '%s' to its value at line %d.\n",
					VariableRenaming::keyToString(nameDefinitionPair.first).c_str(),
					expression->get_file_info()->get_line());

			//See if we can recursively restore the variable so we can re-execute the definition
			SgExpression* restoredOldValue = restoreVariable(nameDefinitionPair.first, availableVariables, originalVarVersion);
			if (restoredOldValue != NULL)
			{
				vector<SgExpression*> restoredVarReferences = BackstrokeUtility::findVarReferences(nameDefinitionPair.first, expressionCopy);

				foreach (SgExpression* restoredVarReference, restoredVarReferences)
				{
					printf("Replacing '%s' with '%s'\n", restoredVarReference->unparseToString().c_str(),
							restoredOldValue->unparseToString().c_str());

					//If the expression itself is a variable reference, eg (t = a),  we can't use SageInterface::replaceExpression
					//because the parent of the variable reference is null. Manually replace the expression with the restored value
					if (expressionCopy == restoredVarReference)
					{
						SageInterface::deepDelete(expressionCopy);
						expressionCopy = SageInterface::copyExpression(restoredOldValue);
						break;
					}

					SageInterface::replaceExpression(restoredVarReference, SageInterface::copyExpression(restoredOldValue));
				}

				SageInterface::deepDelete(restoredOldValue);
			}
			else
			{
				//There is a variable whose value we could not extract
				SageInterface::deepDelete(expressionCopy);
				return NULL;
			}
		}
	}

	//Ok, we restored all variables to the correct value. This should evaluate the same as the original expression
	return expressionCopy;
}

