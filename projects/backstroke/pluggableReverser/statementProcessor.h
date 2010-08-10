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
    StatementReversalVec processReturnStatement(SgStatement* stmt, const VariableVersionTable& var_table);

    StatementReversalVec processFunctionDeclaration(SgStatement* stmt, const VariableVersionTable& var_table);

    StatementReversalVec processExprStatement(SgStatement* stmt, const VariableVersionTable& var_table);

    StatementReversalVec processVariableDeclaration(SgStatement* stmt, const VariableVersionTable& var_table);

    StatementReversalVec processBasicBlock(SgStatement* stmt, const VariableVersionTable& var_table);

    StatementReversalVec processIfStmt(SgStatement* stmt, const VariableVersionTable& var_table);

public:
    virtual StatementReversalVec process(SgStatement* stmt, const VariableVersionTable& var_table);
};



#endif
