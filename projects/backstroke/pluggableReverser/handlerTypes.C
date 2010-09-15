#include "handlerTypes.h"
#include "eventProcessor.h"
#include <boost/foreach.hpp>

#define reverse_foreach BOOST_REVERSE_FOREACH
using namespace std;


const std::vector<EvaluationResult>& EvaluationResult::getChildResults() const
{
	return child_results;
}

VariableVersionTable& EvaluationResult::getVarTable()
{
	return var_table_;
}

const VariableVersionTable& EvaluationResult::getVarTable() const
{
	return var_table_;
}


void EvaluationResult::setVarTable(const VariableVersionTable& table)
{
	var_table_ = table;
}

const SimpleCostModel& EvaluationResult::getCost() const
{
	return cost_;
}

void EvaluationResult::setCost(const SimpleCostModel& cost)
{
	cost_ = cost;
}

EvaluationResultAttributePtr EvaluationResult::getAttribute() const
{
	return attribute_;
}

void EvaluationResult::setAttribute(EvaluationResultAttributePtr attr)
{
	attribute_ = attr;
}

SgExpression* EvaluationResult::getExpressionInput() const
{
	SgExpression* expression = dynamic_cast<SgExpression*>(input_);
	ROSE_ASSERT(expression != NULL);
	return expression;
}

SgStatement* EvaluationResult::getStatementInput() const
{
	SgStatement* statement = dynamic_cast<SgStatement*>(input_);
	ROSE_ASSERT(statement != NULL);
	return statement;
}

ExpressionReversalHandler* EvaluationResult::getExpressionHandler() const
{
	const ExpressionReversalHandler* expressionHandler = dynamic_cast<ExpressionReversalHandler*>(handler_used_);
	ROSE_ASSERT(expressionHandler != NULL);
	return const_cast<ExpressionReversalHandler*>(expressionHandler);
}

StatementReversalHandler* EvaluationResult::getStatementHandler() const
{
	const StatementReversalHandler* statementHandler = dynamic_cast<const StatementReversalHandler*>(handler_used_);
	ROSE_ASSERT(statementHandler != NULL);
	return const_cast<StatementReversalHandler*>(statementHandler);
}

void EvaluationResult::addChildEvaluationResult(const EvaluationResult& result)
{
	child_results.push_back(result);
	cost_ += result.cost_;
	var_table_ = result.var_table_;
}

ExpressionReversal EvaluationResult::generateReverseExpression() const
{
	return getExpressionHandler()->generateReverseAST(getExpressionInput(), *this);
}

StatementReversal EvaluationResult::generateReverseStatement() const
{
	return getStatementHandler()->generateReverseAST(getStatementInput(), *this);
}

void EvaluationResult::printHandlers() const
{
	static int tab_num = 0;

	for (int i = 0; i < tab_num; ++i)
		cout << "\t";
	cout << handler_used_->getName() << endl;

	++tab_num;
	reverse_foreach (const EvaluationResult& result, child_results)
		result.printHandlers();
	--tab_num;
}

SgExpression* ReversalHandlerBase::pushVal(SgExpression* exp, SgType* type)
{
    return event_processor_->pushVal(exp, type);
}

SgExpression* ReversalHandlerBase::popVal(SgType* type)
{
    return event_processor_->popVal(type);
}

vector<EvaluationResult> ReversalHandlerBase::evaluateExpression(SgExpression* exp,
		const VariableVersionTable& var_table, bool is_value_used)
{
    return event_processor_->evaluateExpression(exp, var_table, is_value_used);
}

vector<EvaluationResult> ReversalHandlerBase::evaluateStatement(SgStatement* stmt, const VariableVersionTable& var_table)
{
    return event_processor_->evaluateStatement(stmt, var_table);
}


bool ReversalHandlerBase::isStateVariable(SgExpression* exp)
{
    return event_processor_->isStateVariable(exp);
}

SgExpression* ReversalHandlerBase::restoreVariable(VariableRenaming::VarName variable, 
		const VariableVersionTable& availableVariables, VariableRenaming::NumNodeRenameEntry definitions)
{
	return event_processor_->restoreVariable(variable, availableVariables, definitions);
}

VariableRenaming* ReversalHandlerBase::getVariableRenaming()
{
	return event_processor_->getVariableRenaming();
}
