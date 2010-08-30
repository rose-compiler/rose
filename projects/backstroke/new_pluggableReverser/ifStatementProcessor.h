#ifndef IFSTATEMENTPROCESSOR_H
#define	IFSTATEMENTPROCESSOR_H

#include "statementProcessor.h"

class IfStatementProcessor : public StatementProcessor
{
public:
    IfStatementProcessor() { name_ = "If Statement Processor"; }

    virtual StatementReversal process(SgStatement* stmt);
    virtual std::vector<EvaluationResult> evaluate(SgStatement* stmt, const VariableVersionTable& var_table);
};


#endif	/* IFSTATEMENTPROCESSOR_H */

