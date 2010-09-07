#ifndef REVERSE_COMPUTATION_STMT_PROCESSOR_H
#define REVERSE_COMPUTATION_STMT_PROCESSOR_H

#include <rose.h>
#include "eventProcessor.h"

class CombinatorialExprStatementHandler : public StatementReversalHandler
{
public:
    CombinatorialExprStatementHandler() { name_ = "Combinatorial Expression Statement Handler"; }

    virtual StatementReversal generateReverseAST(SgStatement* stmt, const EvaluationResult& evaluationResult);
    virtual std::vector<EvaluationResult> evaluate(SgStatement* stmt, const VariableVersionTable& var_table);
};

class CombinatorialBasicBlockHandler : public StatementReversalHandler
{
public:
    CombinatorialBasicBlockHandler() { name_ = "Combinatorial Basic Block Handler"; }

    virtual StatementReversal generateReverseAST(SgStatement* stmt, const EvaluationResult& evaluationResult);
    virtual std::vector<EvaluationResult> evaluate(SgStatement* stmt, const VariableVersionTable& var_table);
};



#endif
