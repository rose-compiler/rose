#ifndef WHILESTATEMENTPROCESSOR_H
#define	WHILESTATEMENTPROCESSOR_H

#include "handlerTypes.h"

class WhileStatementProcessor : public StatementReversalHandler
{
public:
    WhileStatementProcessor() { name_ = "While Statement Processor"; }

	SgStatement* assembleLoopCounter(SgStatement* loop_stmt);
	SgStatement* buildForLoop(SgStatement* loop_body);

    virtual StatementReversal generateReverseAST(SgStatement* stmt, const EvaluationResult&);
    virtual std::vector<EvaluationResult> evaluate(SgStatement* stmt, const VariableVersionTable& var_table);
};


#endif	/* WHILESTATEMENTPROCESSOR_H */

