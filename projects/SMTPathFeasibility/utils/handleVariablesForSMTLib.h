#ifndef HH_HANDLE_VARIABLES_FOR_SMT2_HH
#define HH_HANDLE_VARIABLES_FOR_SMT2_HH
#include <string>
#include <vector>
#include "rose.h"
#include "DefUseAnalysis.h"

std::string getSExpr(SgExpression*);
std::string convertAssignOp(SgBinaryOp* binOp, SgExpression* lhs, SgExpression* rhs);
std::string getNewVar(SgInitializedName*);
std::string getNewVar(SgVarRefExp*);
std::map<SgVariableSymbol*, std::string> varExpressionValue;
#endif
