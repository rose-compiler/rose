
#include "toolboxes/rose-toolbox.hpp"

#include "rose.h"

SgExpression * vecToExp(std::vector<std::pair<RoseVariable, int> > & vec) {
  if (vec.size() == 0)
    return SageBuilder::buildIntVal(0);

  SgScopeStatement * scope = SageBuilder::topScopeStack();
  SgExpression * res = SageBuilder::buildMultiplyOp(
    SageBuilder::buildIntVal(vec[0].second),
    vec[0].first.generate(scope)
  );

  if (vec.size() == 1)
    return res;

  std::vector<std::pair<RoseVariable, int> >::iterator it;
  for (it = vec.begin()+1; it != vec.end(); it++) {
    res = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(
      SageBuilder::buildIntVal(it->second),
      it->first.generate(scope)
    ));
  }
}

SgExpression * genAnd(std::vector<SgExpression *> & terms) {
  // TODO
}

SgExpression * genMin(std::vector<SgExpression *> & terms) {
  // TODO
}

SgExpression * genMax(std::vector<SgExpression *> & terms) {
  // TODO
}

