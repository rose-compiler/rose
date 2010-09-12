#ifndef RETURNSTATEMENTHANDLER_H
#define	RETURNSTATEMENTHANDLER_H

#include "handlerTypes.h"

//! This handler reverses a return statement that appears as the very last
//! statement of a function. The reverse statement is a no-op
class ReturnStatementHandler : public StatementReversalHandler
{
public:
	virtual std::vector<EvaluationResult> evaluate(SgStatement* statement, const VariableVersionTable& var_table);

	virtual StatementReversal generateReverseAST(SgStatement* exp, const EvaluationResult& evaluationResult);
};


#endif	/* RETURNSTATEMENTHANDLER_H */

