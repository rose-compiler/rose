#ifndef REVERSE_COMPUTATION_STMT_PROCESSOR_H
#define REVERSE_COMPUTATION_STMT_PROCESSOR_H

#include <rose.h>
#include "eventProcessor.h"

class ExprStatementProcessor : public StatementProcessor
{
public:
    ExprStatementProcessor() { name_ = "Expression Statement Processor"; }

    virtual StatementReversal process(SgStatement* stmt, const EvaluationResult& evaluationResult);
    virtual std::vector<EvaluationResult> evaluate(SgStatement* stmt, const VariableVersionTable& var_table);
};

class BasicBlockProcessor : public StatementProcessor
{
public:
    BasicBlockProcessor() { name_ = "Basic Block Processor"; }

    virtual StatementReversal process(SgStatement* stmt, const EvaluationResult& evaluationResult);
    virtual std::vector<EvaluationResult> evaluate(SgStatement* stmt, const VariableVersionTable& var_table);
};



#endif
