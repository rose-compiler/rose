#ifndef REVERSE_COMPUTATION_STMT_PROCESSOR_H
#define REVERSE_COMPUTATION_STMT_PROCESSOR_H

#include <rose.h>
#include "eventProcessor.h"

class ExprStatementProcessor : public StatementReversalHandler
{
public:
    ExprStatementProcessor() { name_ = "Expression Statement Processor"; }

    virtual StatementReversal generateReverseAST(SgStatement* stmt, const EvaluationResult& evaluationResult);
    virtual std::vector<EvaluationResult> evaluate(SgStatement* stmt, const VariableVersionTable& var_table);
};

class BasicBlockProcessor : public StatementReversalHandler
{
public:
    BasicBlockProcessor() { name_ = "Basic Block Processor"; }

    virtual StatementReversal generateReverseAST(SgStatement* stmt, const EvaluationResult& evaluationResult);
    virtual std::vector<EvaluationResult> evaluate(SgStatement* stmt, const VariableVersionTable& var_table);
};



#endif
