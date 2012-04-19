
#ifndef __ROSE_UTILS_HPP__
#define __ROSE_UTILS_HPP__

#include <map>
#include <string>
#include <utility>

class SgExpression;
class SgStatement;
class SgBasicBlock;
class SgScopeStatement;
class SgVariableSymbol;

enum tri_state {
  vrai,
  faux,
  undefined
};

tri_state eval(SgExpression * exp);

bool getIntFromConstExpression(SgExpression * exp, int * res);

SgStatement * removeConstIf(SgStatement * stmt);

SgBasicBlock * cleanPoCC(SgBasicBlock * bb);

std::pair<int,int> getVariableBounds(SgVariableSymbol * symbol, SgScopeStatement * scope);
std::pair<int,int> evalExpressionBounds(SgExpression * exp, std::map<SgVariableSymbol *, std::pair<int,int> > bounds);

SgExpression * simplify(SgExpression * exp);

SgExpression * substract(SgExpression * lhs, SgExpression * rhs, int add = 0);

#endif /* __ROSE_UTILS_HPP__ */

