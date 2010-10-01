#ifndef IFSTATEMENTHANDLER_H
#define	IFSTATEMENTHANDLER_H

#include "handlerTypes.h"

class IfStatementHandler : public StatementReversalHandler
{
public:
    IfStatementHandler() { name_ = "If Statement Processor"; }

    virtual StatementReversal generateReverseAST(SgStatement* stmt, const EvaluationResult&);
    virtual std::vector<EvaluationResult> evaluate(SgStatement* stmt, const VariableVersionTable& var_table);

};




#endif	/* IfStatementHandler_H */

