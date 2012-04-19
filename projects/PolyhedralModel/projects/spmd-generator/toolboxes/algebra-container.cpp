
#include "toolboxes/algebra-container.hpp"
#include "toolboxes/rose-toolbox.hpp"

#include "rose.h"

SgAssignOp * Expression::genAssignTo(const RoseVariable & var) const {
  return SageBuilder::buildAssignOp(var.generate(SageBuilder::topScopeStack()), generate());
}

SgLessOrEqualOp * Expression::genLessOrEqualTo(const RoseVariable & var) const {
  return SageBuilder::buildLessOrEqualOp(var.generate(SageBuilder::topScopeStack()), generate());
}

SgGreaterOrEqualOp * Expression::genGreaterOrEqualTo(const RoseVariable & var) const {
  return SageBuilder::buildGreaterOrEqualOp(var.generate(SageBuilder::topScopeStack()), generate());
}

SgGreaterOrEqualOp * Expression::genGreaterOrEqualToZero() const {
  return SageBuilder::buildGreaterOrEqualOp(generate(), SageBuilder::buildIntVal(0));
}

SgLessOrEqualOp * Expression::genLessOrEqualToZero() const {
  return SageBuilder::buildLessOrEqualOp(generate(), SageBuilder::buildIntVal(0));
}

LinearExpression::LinearExpression() :
  map()
{}

LinearExpression::LinearExpression(const std::map<RoseVariable, int> & map_) :
  map(map_)
{}

LinearExpression::~LinearExpression() {}

void LinearExpression::set(RoseVariable & var, int value) {
  map[var] = value;
}

int LinearExpression::get(RoseVariable & var) const {
  std::map<RoseVariable, int>::const_iterator it = map.find(var);
  return it == map.end() ? 0 : it->second;
}

void LinearExpression::add(RoseVariable & var, int value) {
  std::map<RoseVariable, int>::iterator it = map.find(var);
  if (it == map.end()) map.insert(std::pair<RoseVariable, int>(var, value));
  else it->second += value;
}

SgExpression * LinearExpression::generate() const {
  std::vector<std::pair<RoseVariable, int> > vect(map.begin(), map.end());
  return vecToExp(vect);
}

LinearExpression * LinearExpression::copy() const {
  return new LinearExpression(map);
}

Bounds::Bounds(RoseVariable iterator_, int stride_) :
  iterator(iterator_),
  stride(stride_)
{}

Bounds::~Bounds() {}

RoseVariable & Bounds::getIterator() { return iterator; }

SgExprStatement * Bounds::genInit() const {
  SgExpression * sg_lb = genLowerBound();
  SgAssignOp * assign = SageBuilder::buildAssignOp(
    iterator.generate(SageBuilder::topScopeStack()), sg_lb
  );
  return SageBuilder::buildExprStatement(assign);
}

SgExprStatement * Bounds::genTest() const {
  SgExpression * sg_ub = genUpperBound();
  SgExpression * tmp = NULL;
  if (stride > 0)
    tmp = SageBuilder::buildLessOrEqualOp(iterator.generate(SageBuilder::topScopeStack()), sg_ub);
  else
    tmp = SageBuilder::buildGreaterOrEqualOp(iterator.generate(SageBuilder::topScopeStack()), sg_ub);
  return SageBuilder::buildExprStatement(tmp);
}

SgExpression * Bounds::genIncrement() const {
  if (stride == 1)
    return SageBuilder::buildPlusPlusOp(iterator.generate(SageBuilder::topScopeStack()));
  else if (stride == -1)
    return SageBuilder::buildMinusMinusOp(iterator.generate(SageBuilder::topScopeStack()));
  else if (stride > 0)
    return SageBuilder::buildPlusAssignOp(iterator.generate(SageBuilder::topScopeStack()), SageBuilder::buildIntVal(stride));
  else if (stride < 0)
    return SageBuilder::buildPlusAssignOp(iterator.generate(SageBuilder::topScopeStack()), SageBuilder::buildIntVal(-stride));
  else
    assert(false);
}

LinearBounds::LinearBounds(RoseVariable iterator_, int stride_) :
  Bounds(iterator_, stride_),
  lb(),
  ub()
{}

LinearBounds::~LinearBounds() {
  std::vector<std::pair<LinearExpression *, int> >::iterator it;
  for (it = lb.begin(); it != lb.end(); it++)
    delete it->first;
  for (it = ub.begin(); it != ub.end(); it++)
    delete it->first;
}

SgExpression * LinearBounds::genLowerBound() const {
  std::vector<std::pair<LinearExpression *, int> >::const_iterator it;
  const std::vector<std::pair<LinearExpression *, int> > & vect = stride > 0 ? lb : ub;
  assert(vect.size() != 0);
  std::vector<SgExpression *> sg_vect;
  for (it = vect.begin(); it != vect.end(); it++) {
    int coef = it->second;
    SgExpression * exp = it->first->generate();
    if (coef == -1)
      exp = SageBuilder::buildMinusOp(exp);
    else if (coef != 1) {
      exp = SageBuilder::buildDivideOp(exp, SageBuilder::buildIntVal(coef));
      if (stride > 0) {
        exp = SageBuilder::buildConditionalExp(
          SageBuilder::buildNotEqualOp(SageBuilder::buildModOp(it->first->generate(), SageBuilder::buildIntVal(coef)), 0),
          SageBuilder::buildAddOp(exp, SageBuilder::buildIntVal(1)),
          SageInterface::copyExpression(exp)
        );
      }
    }
    sg_vect.push_back(exp);
  }
  if (sg_vect.size() > 1)
    if (stride > 0)
      return genMax(sg_vect);
    else
      return genMin(sg_vect);
  else
    return sg_vect[0];
}

SgExpression * LinearBounds::genUpperBound() const {
  std::vector<std::pair<LinearExpression *, int> >::const_iterator it;
  const std::vector<std::pair<LinearExpression *, int> > & vect = stride < 0 ? lb : ub;
  assert(vect.size() != 0);
  std::vector<SgExpression *> sg_vect;
  for (it = vect.begin(); it != vect.end(); it++) {
    int coef = it->second;
    SgExpression * exp = it->first->generate();
    if (coef == -1)
      exp = SageBuilder::buildMinusOp(exp);
    else if (coef != 1) {
      exp = SageBuilder::buildDivideOp(exp, SageBuilder::buildIntVal(coef));
      if (stride < 0) {
        exp = SageBuilder::buildConditionalExp(
          SageBuilder::buildNotEqualOp(SageBuilder::buildModOp(it->first->generate(), SageBuilder::buildIntVal(coef)), 0),
          SageBuilder::buildAddOp(exp, SageBuilder::buildIntVal(1)),
          SageInterface::copyExpression(exp)
        );
      }
    }
    sg_vect.push_back(exp);
  }
  if (sg_vect.size() > 1)
    if (stride < 0)
      return genMax(sg_vect);
    else
      return genMin(sg_vect);
  else
    return sg_vect[0];
}

void LinearBounds::addLowerBound(LinearExpression * lb_, int div) {
  lb.push_back(std::pair<LinearExpression *, int>(lb_, div));
}

void LinearBounds::addUpperBound(LinearExpression * ub_, int div) {
  ub.push_back(std::pair<LinearExpression *, int>(ub_, div));
}

LinearBounds * LinearBounds::copy() const {
  LinearBounds * res = new LinearBounds(iterator, stride);
  std::vector<std::pair<LinearExpression *, int> >::const_iterator it;
  for (it = lb.begin(); it != lb.end(); it++)
    res->addLowerBound(it->first, it->second);
  for (it = ub.begin(); it != ub.end(); it++)
    res->addUpperBound(it->first, it->second);
  return res;
}

