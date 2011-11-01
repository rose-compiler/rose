#ifndef IFSTATEMENTHANDLER_H
#define	IFSTATEMENTHANDLER_H

#include "handlerTypes.h"

class SgIfStmt_Handler : public StatementReversalHandler
{
public:
    SgIfStmt_Handler() { name_ = "If Statement Handler"; }

    virtual StatementReversal generateReverseAST(SgStatement* stmt, const EvaluationResult&);
    virtual std::vector<EvaluationResult> evaluate(SgStatement* stmt, const VariableVersionTable& var_table);

};




#endif	/* IfStatementHandler_H */

