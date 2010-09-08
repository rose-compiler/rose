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

	//! Make those two classes the friends to let them use some private methods.
	friend class ReversalHandlerBase;

private:

	//! Given an expression, return all evaluation results using all expression processors.
	std::vector<EvaluationResult> evaluateExpression(SgExpression* exp, const VariableVersionTable& var_table, bool is_value_used);

	//! Given a statement, return all evaluation results using all statement processors.
	std::vector<EvaluationResult> evaluateStatement(SgStatement* stmt, const VariableVersionTable& var_table);

	//! The following methods are for expression and statement processors for store and restore.
	SgExpression* getStackVar(SgType* type);
	SgExpression* pushVal(SgExpression* exp, SgType* type);
	SgExpression* popVal(SgType* type);

	/** This set is used to prevent infinite recursion when calling restoreVariable. */
	std::set<std::pair<VariableRenaming::VarName, VariableRenaming::NumNodeRenameEntry> > activeValueRestorations;

public:

	EventProcessor(SgFunctionDeclaration* func_decl = NULL, VariableRenaming* var_renaming = NULL)
	: event_(func_decl), var_renaming_(var_renaming) { }

	void addExpressionProcessor(ExpressionReversalHandler* exp_processor)
	{
		exp_processor->setEventProcessor(this);
		exp_processors_.push_back(exp_processor);
	}

	void addStatementProcessor(StatementReversalHandler* stmt_processor)
	{
		stmt_processor->setEventProcessor(this);
		stmt_processors_.push_back(stmt_processor);
	}

	void addVariableValueRestorer(VariableValueRestorer* restorer)
	{
		restorer->setEventProcessor(this);
		variableValueRestorers.push_back(restorer);
	}

	FuncDeclPairs processEvent();

	FuncDeclPairs processEvent(SgFunctionDeclaration* event)
	{
		event_ = event;
		//stack_decls_.clear();
		return processEvent();
	}

	//! Return if the given variable is a state variable (currently, it should be the parameter of event function).
	bool isStateVariable(SgExpression* exp);
	bool isStateVariable(const VariableRenaming::VarName& var);

	/** Check if every state variable in the given variable version table has the initial version
	 * which should be 1. */
	bool checkVersion(const VariableVersionTable& var_table);

	//! Get all declarations of stacks which store values of different types.
	std::vector<SgVariableDeclaration*> getAllStackDeclarations() const;

	VariableRenaming* getVariableRenaming()
	{
		return var_renaming_;
	}

	/**
	* Given a variable and a version, returns an expression evaluating to the value of the variable
	* at the given version.
	*
	* @param variable name of the variable to be restored
	* @param availableVariables variables whos values are currently available
	* @return definitions the version of the variable which should be restored
	*/
	std::vector<SgExpression*> restoreVariable(VariableRenaming::VarName variable, const VariableVersionTable& availableVariables,
			VariableRenaming::NumNodeRenameEntry definitions);
};


#endif	/* BACKSTROKE_EVENTPROCESSOR_H */

