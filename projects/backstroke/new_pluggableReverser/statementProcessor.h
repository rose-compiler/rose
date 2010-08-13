#ifndef REVERSE_COMPUTATION_STMT_PROCESSOR_H
#define REVERSE_COMPUTATION_STMT_PROCESSOR_H

#include <rose.h>
#include "eventProcessor.h"

#if 0
StmtPairs processBasicStatement(SgStatement* stmt);
StmtPairs processFunctionDeclaration(SgFunctionDeclaration* func_decl);
StmtPairs processExprStatement(SgExprStatement* exp_stmt);
StmtPairs processVariableDeclaration(SgVariableDeclaration* var_decl);
StmtPairs processBasicBlock(SgBasicBlock* body);
StmtPairs processIfStmt(SgIfStmt* if_stmt);
#endif


class ExprStatementProcessor : public StatementProcessor
{
public:
    virtual ProcessedStatement process(
                SgStatement* stmt, 
                std::stack<ExpressionProcessor*>& exp_processors,
                std::stack<StatementProcessor*>& stmt_processors);
    virtual std::vector<EvaluationResult> evaluate(const StatementPackage& stmt_pkg);
};

class BasicBlockProcessor : public StatementProcessor
{
public:
    virtual ProcessedStatement process(
                SgStatement* stmt, 
                std::stack<ExpressionProcessor*>& exp_processors,
                std::stack<StatementProcessor*>& stmt_processors);
    virtual std::vector<EvaluationResult> evaluate(const StatementPackage& stmt_pkg);
};



#endif
