#ifndef STATESAVINGSTATEMENTHANDLER_H
#define	STATESAVINGSTATEMENTHANDLER_H

#include "handlerTypes.h"

class StateSavingStatementHandler : public StatementReversalHandler
{
	std::set<SgStatement*> evaluating_stmts_;

	//! Check if this handler can handle the given statement.
	bool checkStatement(SgStatement* stmt) const;
public:
    StateSavingStatementHandler() { name_ = "State Saving Statement Handler"; }

    virtual StatementReversal generateReverseAST(SgStatement* stmt, const EvaluationResult&);
    virtual std::vector<EvaluationResult> evaluate(SgStatement* stmt, const VariableVersionTable& var_table);
};

#endif	/* STATESAVINGSTATEMENTHANDLER_H */

