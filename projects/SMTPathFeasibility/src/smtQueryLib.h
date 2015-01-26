#ifndef HH_SMTQUERYLIB_HH
#define HH_SMTQUERYLIB_HH
#include <iostream>
#include <sstream>
#include <set>
#include <stdio.h>
#include <string>
#include "rose.h"
extern int if_statement_count;
extern int expression_count;
extern std::vector<std::string> variables;
extern std::vector<std::string> declarations;
extern std::vector<std::string> expressions;
bool isBooleanExpr(SgNode);
void writeStringToFileZ3(std::string);
std::string writeSgUnaryOpZ3(std::string);
std::string getSgInitializedName(SgInitializedName*);
std::string writeSgBinaryOpZ3(SgBinaryOp*, SgExpression*, SgExpression*);
std::string getSgValueExp(SgValueExp* valExp);
std::string getSgUnaryOp(SgUnaryOp* unaryOp);
std::string getSgBinaryOp(SgBinaryOp* binOp);
std::map<SgSymbol*, std::string> SymbolToZ3;
std::string getSgStatement(SgStatement* stat);
std::map<SgSymbol*, int> SymbolToInstances;
std::string getSgExpressionString(SgExpression*);
//std::string getSgExprStatement(SgExprStatement*);
std::string getSgVarRefExp(SgVarRefExp*);
std::string SMTFile;
std::map<std::string, int> variablesOfNameX;


void getSgDeclarationStatement(SgDeclarationStatement* declStat);

void getSgFunctionDefinition(SgFunctionDeclaration* funcDecl);

void getSgScopeStatement(SgScopeStatement* scopStat, int);

std::string getSgIOStatement(SgIOStatement* ioStat);

std::string initializeVariable(SgInitializedName* initName);

void getSgFunctionParameterList(SgFunctionParameterList* funcParamList);

void getSgBasicBlock(SgBasicBlock*);


void getSgIfStmt(SgIfStmt*);

int t_num;

std::map<SgNode*, bool> pathNodeTruthValue;


std::string getSgCompoundAssignOp(SgCompoundAssignOp*);

int scopeStatNum;


#endif
