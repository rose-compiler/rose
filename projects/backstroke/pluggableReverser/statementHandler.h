#ifndef REVERSE_COMPUTATION_STMT_HANDLER_H
#define REVERSE_COMPUTATION_STMT_HANDLER_H

#include <rose.h>
#include "utilities/types.h"

StmtPairs processBasicStatement(SgStatement* stmt);
StmtPairs processFunctionDeclaration(SgFunctionDeclaration* func_decl);
StmtPairs processExprStatement(SgExprStatement* exp_stmt);
StmtPairs processVariableDeclaration(SgVariableDeclaration* var_decl);
StmtPairs processBasicBlock(SgBasicBlock* body);
StmtPairs processIfStmt(SgIfStmt* if_stmt);

#endif
