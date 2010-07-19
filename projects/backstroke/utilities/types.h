#ifndef REVERSE_COMPUTATION_TYPES
#define REVERSE_COMPUTATION_TYPES

#include <rose.h>

typedef std::pair<SgExpression*, SgExpression*> ExpPair;
typedef std::pair<SgStatement*, SgStatement*> StmtPair;
typedef std::pair<SgFunctionDeclaration*, SgFunctionDeclaration*> FuncDeclPair;

typedef std::vector<ExpPair> ExpPairs;
typedef std::vector<StmtPair> StmtPairs;
typedef std::vector<FuncDeclPair> FuncDeclPairs;

const static ExpPair NULL_EXP_PAIR = ExpPair(NULL, NULL);
const static StmtPair NULL_STMT_PAIR = StmtPair(NULL, NULL);

#endif
