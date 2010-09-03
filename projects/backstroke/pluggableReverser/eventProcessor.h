#ifndef BACKSTROKE_EVENTPROCESSOR_H
#define	BACKSTROKE_EVENTPROCESSOR_H

#include <rose.h>
#include <utilities/types.h>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include "variableVersionTable.h"
#include "costModel.h"

class ExpressionProcessor;
class StatementProcessor;
class ProcessorBase;

class EvaluationResultAttribute
{
public:

	virtual ~EvaluationResultAttribute() { }
};

typedef boost::shared_ptr<EvaluationResultAttribute> EvaluationResultAttributePtr;

class EvaluationResult
{
	//TODO: This table is not necessary once the result is added to the parent results
	VariableVersionTable var_table_;
	// Cost model
	SimpleCostModel cost_;

	/** The processor which produced this evaluation result. This is used during the
	 * generation phrase to generate the actual expression. */
	ProcessorBase* processor_used_;

	/** Additional attribute that the processor may choose to attach to the evaluation result. */
	EvaluationResultAttributePtr attribute_;

	/** Evaluation choices made in order to get this result. For example, for a basic block, what
	 * were the evaluations of all the statements? */
	std::vector<EvaluationResult> child_results;

public:

	EvaluationResult(ProcessorBase* processorUsed, const VariableVersionTable& table, const SimpleCostModel& cost_model = SimpleCostModel())
	:  var_table_(table), cost_(cost_model), processor_used_(processorUsed){ }

	/** Add an evaluation result to the evalutions used in order to construct the current one.
	  * This adds the cost of the child result to the total cost and adds the result to the list of
	  * evaluation results. It also replaces the variable version table! */
	void addChildEvaluationResult(const EvaluationResult& result)
	{
		child_results.push_back(result);
		cost_ += result.cost_;
		var_table_ = result.var_table_;
	}

	ExpressionProcessor* getExpressionProcessor() const;

	StatementProcessor* getStatementProcessor() const;

	void setVarTable(const VariableVersionTable& table)
	{ var_table_ = table; }

	const std::vector<EvaluationResult>& getChildResults() const
	{
		return child_results;
	}

	const VariableVersionTable& getVarTable() const
	{
		return var_table_;
	}

	VariableVersionTable& getVarTable()
	{
		return var_table_;
	}

	const SimpleCostModel& getCost() const
	{
		return cost_;
	}

	void setCost(const SimpleCostModel& cost)
	{
		cost_ = cost;
	}

	EvaluationResultAttributePtr getAttribute() const
	{
		return attribute_;
	}

	void setAttribute(EvaluationResultAttributePtr attr)
	{
		attribute_ = attr;
	}
};

struct ExpressionReversal
{
	ExpressionReversal(SgExpression* fwd, SgExpression * rvs)
	: fwd_exp(fwd), rvs_exp(rvs) { }

	SgExpression* fwd_exp;
	SgExpression* rvs_exp;
};

struct StatementReversal
{
	StatementReversal(SgStatement* fwd, SgStatement * rvs)
	: fwd_stmt(fwd), rvs_stmt(rvs) { }

	SgStatement* fwd_stmt;
	SgStatement* rvs_stmt;
};


//! Comparison functions for structure InstrumentedStatement and InstrumentedExpression.
inline bool operator<(const EvaluationResult& r1, const EvaluationResult& r2)
{
	return r1.getCost().getCost() < r2.getCost().getCost();
}


// Forward declaration of the class EventProcessor.
class EventProcessor;

class ProcessorBase
{
	EventProcessor* event_processor_;

protected:
	std::string name_;

	ExpressionReversal processExpression(SgExpression* exp, const EvaluationResult& evaluationResult);
	StatementReversal processStatement(SgStatement* stmt, const EvaluationResult& evaluationResult);

	std::vector<EvaluationResult> evaluateExpression(SgExpression* exp, const VariableVersionTable& var_table, bool is_value_used);
	std::vector<EvaluationResult> evaluateStatement(SgStatement* stmt, const VariableVersionTable& var_table);

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

	SgExpression* pushVal(SgExpression* exp, SgType* type);
	SgExpression* popVal(SgType* type);

	//! Return if the given variable is a state variable (currently, it should be the parameter of event function).
	bool isStateVariable(SgExpression* exp);

	VariableRenaming* getVariableRenaming();

public:

	ProcessorBase() : event_processor_(NULL) { }

	ProcessorBase(const std::string& name) : event_processor_(NULL), name_(name) { }

	std::string getName() const
	{
		return name_;
	}

	void setEventProcessor(EventProcessor* processor)
	{
		event_processor_ = processor;
	}

	virtual ~ProcessorBase()
	{
		
	}
};

class ExpressionProcessor : public ProcessorBase
{
public:

	virtual ExpressionReversal process(SgExpression* exp, const EvaluationResult& evaluationResult) = 0;
	virtual std::vector<EvaluationResult> evaluate(SgExpression* exp, const VariableVersionTable& var_table, bool is_value_used) = 0;
};

class StatementProcessor : public ProcessorBase
{
public:

	virtual StatementReversal process(SgStatement* stmt, const EvaluationResult& evaluationResult) = 0;
	virtual std::vector<EvaluationResult> evaluate(SgStatement* stmt, const VariableVersionTable& var_table) = 0;
};

/** These types of reverse handlers recalculate a specific value of a variable at a different point
 * in the program. */
class VariableValueRestorer
{
public:

	/**
	 * Given a variable and a version, returns an expression evaluating to the value of the variable
	 * at the given version.
	 *
	 * @param variable name of the variable to be restored
	 * @param availableVariables variables whos values are currently available
	 * @return definitions the version of the variable which should be restored
	 */
	virtual std::vector<SgExpression*> restoreVariable(VariableRenaming::VarName variable, const VariableVersionTable& availableVariables,
			VariableRenaming::NumNodeRenameEntry definitions) = 0;

	VariableValueRestorer() : eventProcessor(NULL) { }

	void setEventProcessor(EventProcessor* eventProcessor)
	{
		this->eventProcessor = eventProcessor;
	}

	EventProcessor* getEventProcessor()
	{
		return eventProcessor;
	}

private:

	EventProcessor* eventProcessor;
};

class VariableRenaming;

class EventProcessor
{
	//! This is the current event function to handle.
	SgFunctionDeclaration* event_;

	//! All expression processors which are added by the user.
	std::vector<ExpressionProcessor*> exp_processors_;

	//! All statement processors which are added by the user.
	std::vector<StatementProcessor*> stmt_processors_;

	/** Handlers which can restore a variable value without state saving. */
	std::vector<VariableValueRestorer*> variableValueRestorers;

	//! All declarations of stacks which store values of different types.
	std::map<std::string, SgVariableDeclaration*> stack_decls_;

	//! The variable renaming analysis object.
	VariableRenaming* var_renaming_;

	//! Make those two classes the friends to let them use some private methods.
	friend class ProcessorBase;

private:

	ExpressionReversal processExpression(SgExpression* exp, const EvaluationResult& evaluationResult);
	StatementReversal processStatement(SgStatement* stmt, const EvaluationResult& result);

	//! Given an expression, return all evaluation results using all expression processors.
	std::vector<EvaluationResult> evaluateExpression(SgExpression* exp, const VariableVersionTable& var_table, bool is_value_used);

	//! Given a statement, return all evaluation results using all statement processors.
	std::vector<EvaluationResult> evaluateStatement(SgStatement* stmt, const VariableVersionTable& var_table);

	//! The following methods are for expression and statement processors for store and restore.
	SgExpression* getStackVar(SgType* type);
	SgExpression* pushVal(SgExpression* exp, SgType* type);
	SgExpression* popVal(SgType* type);

	/** This set is used to prevent invfinite recursion when calling restoreVariable. */
	std::set<std::pair<VariableRenaming::VarName, VariableRenaming::NumNodeRenameEntry> > activeValueRestorations;

public:

	EventProcessor(SgFunctionDeclaration* func_decl = NULL, VariableRenaming* var_renaming = NULL)
	: event_(func_decl), var_renaming_(var_renaming) { }

	void addExpressionProcessor(ExpressionProcessor* exp_processor)
	{
		exp_processor->setEventProcessor(this);
		exp_processors_.push_back(exp_processor);
	}

	void addStatementProcessor(StatementProcessor* stmt_processor)
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

