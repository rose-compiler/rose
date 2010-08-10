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
    InstrumentedStatementVec processReturnStatement(SgStatement* stmt, const VariableVersionTable& var_table);

    InstrumentedStatementVec processFunctionDeclaration(SgStatement* stmt, const VariableVersionTable& var_table);

    InstrumentedStatementVec processExprStatement(SgStatement* stmt, const VariableVersionTable& var_table);

    InstrumentedStatementVec processVariableDeclaration(SgStatement* stmt, const VariableVersionTable& var_table);

    InstrumentedStatementVec processBasicBlock(SgStatement* stmt, const VariableVersionTable& var_table);

    InstrumentedStatementVec processIfStmt(SgStatement* stmt, const VariableVersionTable& var_table);

public:
    virtual InstrumentedStatementVec process(SgStatement* stmt, const VariableVersionTable& var_table);
};



#endif
