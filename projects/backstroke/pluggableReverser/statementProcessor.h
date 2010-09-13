#ifndef REVERSE_COMPUTATION_STMT_PROCESSOR_H
#define REVERSE_COMPUTATION_STMT_PROCESSOR_H

#include <rose.h>
#include "handlerTypes.h"

class CombinatorialExprStatementHandler : public StatementReversalHandler
{
public:

	CombinatorialExprStatementHandler()
	{
		name_ = "Combinatorial Expression Statement Handler";
	}

	virtual StatementReversal generateReverseAST(SgStatement* stmt, const EvaluationResult& evaluationResult);
	virtual std::vector<EvaluationResult> evaluate(SgStatement* stmt, const VariableVersionTable& var_table);
};

class CombinatorialBasicBlockHandler : public StatementReversalHandler
{
public:

	CombinatorialBasicBlockHandler()
	{
		name_ = "Combinatorial Basic Block Handler";
	}

	virtual StatementReversal generateReverseAST(SgStatement* stmt, const EvaluationResult& evaluationResult);
	virtual std::vector<EvaluationResult> evaluate(SgStatement* stmt, const VariableVersionTable& var_table);

private:

	struct LocalVarRestoreAttribute : public EvaluationResultAttribute
	{
		/** For each local variable, we record whether to restore it and how to restore it.
		 * (if SgExpression* is NULL, we push and pop its value. Otherwise, we use it to restore) */
		std::map < SgInitializedName*, std::pair<bool, SgExpression*> > local_var_restorer;
	};
	typedef boost::shared_ptr<LocalVarRestoreAttribute> LocalVarRestoreAttributePtr;

	//! Get the final version for a local variable before leaving its scope.
	VariableRenaming::NumNodeRenameEntry getLastVersion(SgInitializedName* init_name);
};


//! Handles SgNullStatement
class NullStatementHandler : public StatementReversalHandler
{
public:
	NullStatementHandler()
	{
		name_ = "SgNullStatement handler";
	}

	virtual StatementReversal generateReverseAST(SgStatement* stmt, const EvaluationResult& evaluationResult);
	virtual std::vector<EvaluationResult> evaluate(SgStatement* stmt, const VariableVersionTable& var_table);
};

#endif
