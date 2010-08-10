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

class BasicStatementProcessor : public StatementProcessor
{
    InstrumentedStatementVec processReturnStatement(const StatementPackage& stmt_pkg);

    InstrumentedStatementVec processFunctionDeclaration(const StatementPackage& stmt_pkg);

    InstrumentedStatementVec processExprStatement(const StatementPackage& stmt_pkg);

    InstrumentedStatementVec processVariableDeclaration(const StatementPackage& stmt_pkg);

    InstrumentedStatementVec processBasicBlock(const StatementPackage& stmt_pkg);

    InstrumentedStatementVec processIfStmt(const StatementPackage& stmt_pkg);

public:
    virtual InstrumentedStatementVec process(const StatementPackage& stmt_pkg);
};



#endif
