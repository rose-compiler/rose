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
    StatementObjectVec processFunctionDeclaration(
        SgFunctionDeclaration* func_decl,
        const VariableVersionTable& var_table);

    StatementObjectVec processExprStatement(
        SgExprStatement* exp_stmt,
        const VariableVersionTable& var_table);

    StatementObjectVec processVariableDeclaration(
        SgVariableDeclaration* var_decl,
        const VariableVersionTable& var_table);

    StatementObjectVec processBasicBlock(
        SgBasicBlock* body,
        const VariableVersionTable& var_table);

    StatementObjectVec processIfStmt(
        SgIfStmt* if_stmt,
        const VariableVersionTable& var_table);

public:
    virtual StatementObjectVec process(SgStatement* stmt, const VariableVersionTable& var_table);
};



#endif
