#include "handlerTypes.h"
#include "eventProcessor.h"

using namespace std;

SgExpression* ReversalHandlerBase::pushVal(SgExpression* exp, SgType* type)
{
    return event_processor_->pushVal(exp, type);
}

SgExpression* ReversalHandlerBase::popVal(SgType* type)
{
    return event_processor_->popVal(type);
}


ExpressionReversalHandler* EvaluationResult::getExpressionProcessor() const
{
	const ExpressionReversalHandler* expressionHandler = dynamic_cast<ExpressionReversalHandler*>(processor_used_);
	ROSE_ASSERT(expressionHandler != NULL);
	return const_cast<ExpressionReversalHandler*>(expressionHandler);
}

StatementReversalHandler* EvaluationResult::getStatementProcessor() const
{
	const StatementReversalHandler* statementHandler = dynamic_cast<const StatementReversalHandler*>(processor_used_);
	ROSE_ASSERT(statementHandler != NULL);
	return const_cast<StatementReversalHandler*>(statementHandler);
}

void EvaluationResult::addChildEvaluationResult(const EvaluationResult& result)
{
	child_results.push_back(result);
	cost_ += result.cost_;
	var_table_ = result.var_table_;
}

ExpressionReversal EvaluationResult::generateReverseAST(SgExpression* expression) const
{
	return getExpressionProcessor()->generateReverseAST(expression, *this);
}

StatementReversal EvaluationResult::generateReverseAST(SgStatement* statement) const
{
	return getStatementProcessor()->generateReverseAST(statement, *this);
}

vector<EvaluationResult> ReversalHandlerBase::evaluateExpression(SgExpression* exp, const VariableVersionTable& var_table, bool is_value_used)
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

vector<SgExpression*> ReversalHandlerBase::restoreVariable(VariableRenaming::VarName variable, const VariableVersionTable& availableVariables,
			VariableRenaming::NumNodeRenameEntry definitions)
{
	return event_processor_->restoreVariable(variable, availableVariables, definitions);
}

VariableRenaming* ReversalHandlerBase::getVariableRenaming()
{
	return event_processor_->getVariableRenaming();
}
