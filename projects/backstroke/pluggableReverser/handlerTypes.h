#ifndef HANDLERBASE_H
#define	HANDLERBASE_H

#include <boost/shared_ptr.hpp>
#include "variableVersionTable.h"
#include "costModel.h"

//Forward declarations
class ExpressionReversalHandler;
class StatementReversalHandler;
class ReversalHandlerBase;
class EventHandler;

//TODO: Just use std::pair here
struct ExpressionReversal
{
	ExpressionReversal(SgExpression* fwd, SgExpression * rvs)
	: fwd_exp(fwd), rvs_exp(rvs) { }

	SgExpression* fwd_exp;
	SgExpression* rvs_exp;
};

//TODO: Just use std::pair here
struct StatementReversal
{
	StatementReversal(SgStatement* fwd, SgStatement * rvs)
	: fwd_stmt(fwd), rvs_stmt(rvs) { }

	SgStatement* fwd_stmt;
	SgStatement* rvs_stmt;
};

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

	/** The handler which produced this evaluation result. This is used during the
	* generation phrase to generate the actual expression. */
	ReversalHandlerBase* handler_used_;

	//!The expression or statement to which this evaluation result pertains
	SgNode* input_;

	/** Additional attribute that the handler may choose to attach to the evaluation result. */
	EvaluationResultAttributePtr attribute_;

	/** Evaluation choices made in order to get this result. For example, for a basic block, what
	* were the evaluations of all the statements? */
	std::vector<EvaluationResult> child_results;

public:

	EvaluationResult(ReversalHandlerBase* handler_used, SgNode* input,
			const VariableVersionTable& table,
			const SimpleCostModel& cost_model = SimpleCostModel())
	:  var_table_(table), cost_(cost_model), handler_used_(handler_used), input_(input){ }

	/** Add an evaluation result to the evalutions used in order to construct the current one.
	* This adds the cost of the child result to the total cost and adds the result to the list of
	* evaluation results. It also replaces the variable version table! */
	void addChildEvaluationResult(const EvaluationResult& result);

	/** Generate the reverse AST for the expression whose reversal result this class holds. */
	ExpressionReversal generateReverseExpression() const;

	/** Generate the reverse AST for the statement whose reversal result this class holds. */
	StatementReversal generateReverseStatement() const;

	ExpressionReversalHandler* getExpressionHandler() const;

	StatementReversalHandler* getStatementHandler() const;

	const VariableVersionTable& getVarTable() const;

	const std::vector<EvaluationResult>& getChildResults() const;

	VariableVersionTable& getVarTable();

	void setVarTable(const VariableVersionTable& table);

	const SimpleCostModel& getCost() const;

	void setCost(const SimpleCostModel& cost);

	EvaluationResultAttributePtr getAttribute() const;

	void setAttribute(EvaluationResultAttributePtr attr);

	//! Returns the expression which was processed to produce this evaluation result
	SgExpression* getExpressionInput() const;

	//! Returns the statement which was processed to produce this evaluation result
	SgStatement* getStatementInput() const;

	//! Print all handlers inside of this result.
	void printHandlers() const;
};



//! Comparison functions for structure InstrumentedStatement and InstrumentedExpression.
inline bool operator<(const EvaluationResult& r1, const EvaluationResult& r2)
{
	return r1.getCost().getCost() < r2.getCost().getCost();
}


class ReversalHandlerBase
{
	EventHandler* event_handler_;

protected:
	std::string name_;

	std::vector<EvaluationResult> evaluateExpression(SgExpression* exp, const VariableVersionTable& var_table, bool is_value_used);
	std::vector<EvaluationResult> evaluateStatement(SgStatement* stmt, const VariableVersionTable& var_table);

	/**
	* Given a variable and a version, returns an expression evaluating to the value of the variable
	* at the given version.
	*
	* @param variable name of the variable to be restored
	* @param availableVariables variables whos values are currently available
	* @param definitions the version of the variable which should be restored
	* @return expession that when evaluated will produce the desired version of the variable
	*/
	SgExpression* restoreVariable(VariableRenaming::VarName variable, const VariableVersionTable& availableVariables,
			VariableRenaming::NumNodeRenameEntry definitions);

	SgExpression* pushVal(SgExpression* exp, SgType* type);
	SgExpression* popVal(SgType* type);

	//! Return if the given variable is a state variable (currently, it should be the parameter of event function).
	bool isStateVariable(SgExpression* exp);

	VariableRenaming* getVariableRenaming();


public:

	ReversalHandlerBase() : event_handler_(NULL) { }

	ReversalHandlerBase(const std::string& name) : event_handler_(NULL), name_(name) { }

	std::string getName() const
	{
		return name_;
	}

	void setEventHandler(EventHandler* handler)
	{
		event_handler_ = handler;
	}

	virtual ~ReversalHandlerBase()
	{

	}
};

class ExpressionReversalHandler : public ReversalHandlerBase
{
public:

	virtual ExpressionReversal generateReverseAST(SgExpression* exp, const EvaluationResult& evaluationResult) = 0;
	virtual std::vector<EvaluationResult> evaluate(SgExpression* exp, const VariableVersionTable& var_table, bool is_value_used) = 0;
};

class StatementReversalHandler : public ReversalHandlerBase
{
public:

	virtual StatementReversal generateReverseAST(SgStatement* stmt, const EvaluationResult& evaluationResult) = 0;
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
	* @param definitions the version of the variable which should be restored
	* @return expessions that when evaluated will produce the desired version of the variable
	*/
	virtual std::vector<SgExpression*> restoreVariable(VariableRenaming::VarName variable, const VariableVersionTable& availableVariables,
			VariableRenaming::NumNodeRenameEntry definitions) = 0;

	VariableValueRestorer() : eventHandler(NULL) { }

	void setEventHandler(EventHandler* eventHandler)
	{
		this->eventHandler = eventHandler;
	}

	EventHandler* getEventHandler()
	{
		return eventHandler;
	}

private:

	EventHandler* eventHandler;
};


#endif	

