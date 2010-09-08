#ifndef HANDLERBASE_H
#define	HANDLERBASE_H

#include <boost/shared_ptr.hpp>
#include "variableVersionTable.h"
#include "costModel.h"

//Forward declarations
class ExpressionReversalHandler;
class StatementReversalHandler;
class ReversalHandlerBase;
class EventProcessor;

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
	ReversalHandlerBase* processor_used_;

	/** Additional attribute that the processor may choose to attach to the evaluation result. */
	EvaluationResultAttributePtr attribute_;

	/** Evaluation choices made in order to get this result. For example, for a basic block, what
	* were the evaluations of all the statements? */
	std::vector<EvaluationResult> child_results;

public:

	EvaluationResult(ReversalHandlerBase* processorUsed, const VariableVersionTable& table, const SimpleCostModel& cost_model = SimpleCostModel())
	:  var_table_(table), cost_(cost_model), processor_used_(processorUsed){ }

	/** Add an evaluation result to the evalutions used in order to construct the current one.
	* This adds the cost of the child result to the total cost and adds the result to the list of
	* evaluation results. It also replaces the variable version table! */
	void addChildEvaluationResult(const EvaluationResult& result);

	/** Generate the reverse AST for the expression whose reversal result this class holds. */
	ExpressionReversal generateReverseAST(SgExpression* expression) const;

	/** Generate the reverse AST for the statement whose reversal result this class holds. */
	StatementReversal generateReverseAST(SgStatement* statement) const;

	ExpressionReversalHandler* getExpressionProcessor() const;

	StatementReversalHandler* getStatementProcessor() const;

	const VariableVersionTable& getVarTable() const
	{
		return var_table_;
	}

	const std::vector<EvaluationResult>& getChildResults() const
	{
		return child_results;
	}

	VariableVersionTable& getVarTable()
	{
		return var_table_;
	}

	void setVarTable(const VariableVersionTable& table)
	{
		var_table_ = table;
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



//! Comparison functions for structure InstrumentedStatement and InstrumentedExpression.
inline bool operator<(const EvaluationResult& r1, const EvaluationResult& r2)
{
	return r1.getCost().getCost() < r2.getCost().getCost();
}


class ReversalHandlerBase
{
	EventProcessor* event_processor_;

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

	ReversalHandlerBase() : event_processor_(NULL) { }

	ReversalHandlerBase(const std::string& name) : event_processor_(NULL), name_(name) { }

	std::string getName() const
	{
		return name_;
	}

	void setEventProcessor(EventProcessor* processor)
	{
		event_processor_ = processor;
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


#endif	

