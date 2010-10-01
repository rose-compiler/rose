#ifndef STATESAVINGSTATEMENTHANDLER_H
#define	STATESAVINGSTATEMENTHANDLER_H

#include "handlerTypes.h"

class StateSavingStatementHandler : public StatementReversalHandler
{
	// A collection to avoid this handler calls itself by calling evaluateStatement function for the same statement.
	std::set<SgStatement*> evaluating_stmts_;

	//! Check if this handler can handle the given statement.
	bool checkStatement(SgStatement* stmt) const;

	//! Get all modified variables from the given statement.
	std::vector<VariableRenaming::VarName> getAllModifiedVariables(SgStatement* stmt);
public:
    StateSavingStatementHandler() { name_ = "State Saving Statement Handler"; }

    virtual StatementReversal generateReverseAST(SgStatement* stmt, const EvaluationResult&);
    virtual std::vector<EvaluationResult> evaluate(SgStatement* stmt, const VariableVersionTable& var_table);
};

#endif	/* STATESAVINGSTATEMENTHANDLER_H */

