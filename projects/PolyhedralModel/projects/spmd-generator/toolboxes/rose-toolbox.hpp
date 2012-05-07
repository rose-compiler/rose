
#ifndef __ROSE_TOOLBOX_HPP__
#define __ROSE_TOOLBOX_HPP__

#include <vector>
#include <utility>

class SgExpression;

#include "rose/Variable.hpp"

SgExpression * vecToExp(std::vector<std::pair<RoseVariable, int> > & vec);
SgExpression * genAnd(std::vector<SgExpression *> & terms);
SgExpression * genMin(std::vector<SgExpression *> & terms);
SgExpression * genMax(std::vector<SgExpression *> & terms);

SgExpression * simplify(SgExpression * exp);

#endif /* __ROSE_TOOLBOX_HPP__ */
