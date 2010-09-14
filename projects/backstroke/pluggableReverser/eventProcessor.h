#ifndef BACKSTROKE_EVENTPROCESSOR_H
#define	BACKSTROKE_EVENTPROCESSOR_H

#include <rose.h>
#include <utilities/types.h>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include "variableVersionTable.h"
#include "handlerTypes.h"


class VariableRenaming;

class EventProcessor
{
	//! This is the current event function to handle.
	SgFunctionDeclaration* event_;

	//! All expression processors which are added by the user.
	std::vector<ExpressionReversalHandler*> exp_processors_;

	//! All statement processors which are added by the user.
	std::vector<StatementReversalHandler*> stmt_processors_;

	/** Handlers which can restore a variable value without state saving. */
	std::vector<VariableValueRestorer*> variableValueRestorers;

	//! All declarations of stacks which store values of different types.
	std::map<std::string, SgVariableDeclaration*> stack_decls_;

	//! The variable renaming analysis object.
	VariableRenaming* var_renaming_;

	//! This set is used to prevent infinite recursion when calling restoreVariable.
	std::set<std::pair<VariableRenaming::VarName, VariableRenaming::NumNodeRenameEntry> > activeValueRestorations;

	//! Make those two classes the friends to let them use some private methods.
	friend class ReversalHandlerBase;

private:

	//! Given an expression, return all evaluation results using all expression processors.
	std::vector<EvaluationResult> evaluateExpression(SgExpression* exp, const VariableVersionTable& var_table, bool is_value_used);

	//! Given a statement, return all evaluation results using all statement processors.
	std::vector<EvaluationResult> evaluateStatement(SgStatement* stmt, const VariableVersionTable& var_table);

	/** Given a set of results, if two results of them have the same variable table, we remove
	 *  the one which has the higher cost. */
	std::vector<EvaluationResult> filterResults(const std::vector<EvaluationResult>& results);

	//! The following methods are for expression and statement processors for store and restore.
	SgExpression* getStackVar(SgType* type);
	SgExpression* pushVal(SgExpression* exp, SgType* type);
	SgExpression* popVal(SgType* type);


public:

	EventProcessor(SgFunctionDeclaration* func_decl = NULL, VariableRenaming* var_renaming = NULL)
	: event_(func_decl), var_renaming_(var_renaming) { }

	//! Add an expression handler to the pool of expression handlers.
	void addExpressionHandler(ExpressionReversalHandler* exp_processor);

	//! Add a statement handler to the pool of statement handlers
	void addStatementHandler(StatementReversalHandler* stmt_processor);

	//! Add a value extractor to the pool of variable value restorers
	void addVariableValueRestorer(VariableValueRestorer* restorer);

	//! The main interface which proceses an event function.
	FuncDeclPairs processEvent();
	FuncDeclPairs processEvent(SgFunctionDeclaration* event);

	//! Return if the given variable is a state variable (currently we assume all variables except
	//! those defined inside the event function are state varibles).
	bool isStateVariable(SgExpression* exp);
	bool isStateVariable(const VariableRenaming::VarName& var);

	//! Check if every state variable in the given variable version table has the initial version
	//! which should be 1.
	bool checkForInitialVersions(const VariableVersionTable& var_table);

	//! Get all declarations of stacks which store values of different types.
	std::vector<SgVariableDeclaration*> getAllStackDeclarations() const;

	VariableRenaming* getVariableRenaming() const
	{
		return var_renaming_;
	}

	/**
	* Given a variable and a version, returns an expression evaluating to the value of the variable
	* at the given version.
	*
	* @param variable name of the variable to be restored
	* @param availableVariables variables whos values are currently available
	* @param definition the version of the variable which should be restored
	* @return NULL on failure; else an expression that evaluates to the value of the variable
	*/
	SgExpression* restoreVariable(VariableRenaming::VarName variable, const VariableVersionTable& availableVariables,
			VariableRenaming::NumNodeRenameEntry definitions);

	//! Restores the value of an expression given a set of currently available variables. For example, if the
	//! expression is (a + b), the values of a and b will be extracted from the currently available variables, and then
	//! the expression val(a) + val(b) will be returned.
	//!
	//! @returns expression evaluating to the same value as the original, or NULL on failure
	SgExpression* restoreExpressionValue(SgExpression* expression, const VariableVersionTable& availableVariables);
};


#endif	/* BACKSTROKE_EVENTPROCESSOR_H */

