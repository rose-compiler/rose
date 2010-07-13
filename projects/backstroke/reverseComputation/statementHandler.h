#ifndef REVERSE_COMPUTATION_STMT_HANDLER_H
#define REVERSE_COMPUTATION_STMT_HANDLER_H

#include <rose.h>
#include "types.h"

std::vector<StmtPair> processBasicStatement(SgStatement* stmt);
std::vector<StmtPair> processFunctionDeclaration(SgFunctionDeclaration* func_decl);
std::vector<StmtPair> processExprStatement(SgExprStatement* exp_stmt);
std::vector<StmtPair> processVariableDeclaration(SgVariableDeclaration* var_decl);
std::vector<StmtPair> processBasicBlock(SgBasicBlock* body);

#endif
