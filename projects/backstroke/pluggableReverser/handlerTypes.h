#ifndef HANDLERBASE_H
#define	HANDLERBASE_H

#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include "variableVersionTable.h"
#include "ssa/staticSingleAssignment.h"
#include "costModel.h"

//Forward declarations
class ExpressionReversalHandler;
class StatementReversalHandler;
class ReversalHandlerBase;
class EventProcessor;
class IVariableFilter;

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
	boost::any attribute_;

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

	/** Generate the commit method portion of the AST. */
	SgStatement* generateCommitStatement() const;

	ExpressionReversalHandler* getExpressionHandler() const;

	StatementReversalHandler* getStatementHandler() const;

	const VariableVersionTable& getVarTable() const;

	const std::vector<EvaluationResult>& getChildResults() const;

	VariableVersionTable& getVarTable();

	void setVarTable(const VariableVersionTable& table);

	const SimpleCostModel& getCost() const;

	void setCost(const SimpleCostModel& cost);

	template <class T>
	void setAttribute(const T& attr)
	{ attribute_ = attr; }

	template<class T>
	T getAttribute() const
	{ return boost::any_cast<T>(attribute_); }

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
	EventProcessor* event_handler_;

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

	//! Restores the value of an expression given a set of currently available variables. For example, if the
	//! expression is (a + b), the values of a and b will be extracted from the currently available variables, and then
	//! the expression val(a) + val(b) will be returned.
	//!
	//! @returns expression evaluating to the same value as the original, or NULL on failure
	SgExpression* restoreExpressionValue(SgExpression* expression, const VariableVersionTable& availableVariables);

	SgExpression* pushVal(SgExpression* exp, SgType* type);
	SgExpression* pushVal(SgExpression* exp)
	{ return pushVal(exp, exp->get_type()); }

	//! Generates an expression which pops a value from the back of a stack and evaluates to it
	SgExpression* popVal(SgType* type);

	//!Generates an expression which pops a value from the front of a stack and discards the value.
	//! This is used for fossil collection (commit methods)
	SgExpression* popVal_front(SgType* type);

	//! Return if the given variable is a state variable
	bool isStateVariable(SgExpression* exp);

	//! Returns an object with def-use analysis. To be replaced by SSA analysis in the future
	VariableRenaming* getVariableRenaming();

	//! Returns an object with interprocedural SSA analysis
	const StaticSingleAssignment* getSsa() const;

	//! Returns an object that determines whether the value of a given variable nees to be reversed.
	const IVariableFilter* getVariableFilter() const;

public:

	ReversalHandlerBase() : event_handler_(NULL) { }

	ReversalHandlerBase(const std::string& name) : event_handler_(NULL), name_(name) { }

	std::string getName() const
	{
		return name_;
	}

	void setEventHandler(EventProcessor* handler)
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

	/** Generate the commit code. This code should release any state saved by the forward code, output any cached
	 * output, etc.
	 * TODO: Make this pure virtual. Right now, the default implementation does nothing. */
	virtual SgStatement* generateCommitAST(const EvaluationResult& evaluationResult);
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

	void setEventHandler(EventProcessor* eventHandler)
	{
		this->eventHandler = eventHandler;
	}

	EventProcessor* getEventHandler()
	{
		return eventHandler;
	}

private:

	EventProcessor* eventHandler;
};


#endif	

