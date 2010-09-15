#ifndef IFSTATEMENTPROCESSOR_H
#define	IFSTATEMENTPROCESSOR_H

#include "handlerTypes.h"

class IfStatementProcessor : public StatementReversalHandler
{
	struct IfStmtConditionAttribute : public EvaluationResultAttribute
	{

		IfStmtConditionAttribute() : cond(NULL)
		{}
		SgExpression* cond;
	};

	typedef boost::shared_ptr<IfStmtConditionAttribute> IfStmtConditionAttributePtr;

public:
    IfStatementProcessor() { name_ = "If Statement Processor"; }

    virtual StatementReversal generateReverseAST(SgStatement* stmt, const EvaluationResult&);
    virtual std::vector<EvaluationResult> evaluate(SgStatement* stmt, const VariableVersionTable& var_table);

};




#endif	/* IFSTATEMENTPROCESSOR_H */

