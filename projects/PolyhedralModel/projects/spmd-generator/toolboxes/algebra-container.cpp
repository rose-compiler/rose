
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

SgEqualityOp * Expression::genEqualToZero() const {
  return SageBuilder::buildEqualityOp(generate(), SageBuilder::buildIntVal(0));
}

LinearExpression::LinearExpression() :
  map()
{}

LinearExpression::LinearExpression(const std::map<RoseVariable, int> & map_) :
  map(map_)
{
  std::map<RoseVariable, int>::iterator it;
  for (it = map.begin(); it != map.end(); it++)
    if (it->second == 0)
      map.erase(it);
}

LinearExpression::LinearExpression(const std::vector<std::pair<RoseVariable, int> > & vect) :
  map()
{
  std::vector<std::pair<RoseVariable, int> >::const_iterator it;
  for (it = vect.begin(); it != vect.end(); it++) {
    std::map<RoseVariable, int>::iterator it_map = map.find(it->first);
    if (it_map == map.end())
      map.insert(*it);
    else
      it_map->second += it->second;
  }
  std::map<RoseVariable, int>::iterator it_;
  for (it_ = map.begin(); it_ != map.end(); it_++)
    if (it_->second == 0)
      map.erase(it_);
}

LinearExpression::~LinearExpression() {}

void LinearExpression::set(const RoseVariable & var, int value) {
  if (value == 0) map.erase(var);
  else map[var] = value;
}

int LinearExpression::get(const RoseVariable & var) const {
  std::map<RoseVariable, int>::const_iterator it = map.find(var);
  return it == map.end() ? 0 : it->second;
}

void LinearExpression::add(const RoseVariable & var, int value) {
  std::map<RoseVariable, int>::iterator it = map.find(var);
  if (it == map.end() && value != 0)
    map.insert(std::pair<RoseVariable, int>(var, value));
  else if (it->second + value == 0)
    map.erase(it);
  else
    it->second += value;
}

void LinearExpression::neg() {
  std::map<RoseVariable, int>::iterator it;
  for (it = map.begin(); it != map.end(); it++)
    it->second = -it->second;
}

const std::map<RoseVariable, int> & LinearExpression::getRawData() const { return map; }

SgExpression * LinearExpression::generate() const {
  std::vector<std::pair<RoseVariable, int> > vect(map.begin(), map.end());
  return vecToExp(vect);
}

void LinearExpression::print(std::ostream & out) const {
  std::map<RoseVariable, int>::const_iterator it;
  out << "(";
  for (it = map.begin(); it != map.end(); it++) {
    if (it != map.begin()) out << ") + (";

    if (isConstant(it->first)) out << it->second;
    else out << it->second << " * " << it->first.getString();
  } 
  out << ")";
}

LinearExpression * LinearExpression::copy() const {
  return new LinearExpression(map);
}

SageExpression::SageExpression(SgExpression * exp) :
  Expression(),
  expression(exp)
{}

SageExpression::~SageExpression() {}
    
SgExpression * SageExpression::generate() const { return expression; }

void SageExpression::print(std::ostream & out) const {
  out << expression->unparseToString();
}

Expression * SageExpression::copy() const {
  return new SageExpression(SageInterface::copyExpression(expression));
}


Domain::Domain(RoseVariable iterator_, int stride_) :
  iterator(iterator_),
  stride(stride_)
{}

Domain::~Domain() {}

RoseVariable & Domain::getIterator() { return iterator; }

int Domain::getStride() const { return stride; }

SgExprStatement * Domain::genInit() const {
  SgExpression * sg_lb = genLowerBound();
  SgAssignOp * assign = SageBuilder::buildAssignOp(
    iterator.generate(SageBuilder::topScopeStack()), sg_lb
  );
  return SageBuilder::buildExprStatement(assign);
}

SgExprStatement * Domain::genTest() const {
  SgExpression * sg_ub = genUpperBound();
  SgExpression * tmp = NULL;
  if (stride > 0)
    tmp = SageBuilder::buildLessOrEqualOp(iterator.generate(SageBuilder::topScopeStack()), sg_ub);
  else
    tmp = SageBuilder::buildGreaterOrEqualOp(iterator.generate(SageBuilder::topScopeStack()), sg_ub);
  return SageBuilder::buildExprStatement(tmp);
}

SgExpression * Domain::genIncrement() const {
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

SgExpression * Domain::genNumberOfPoints() const {
  if (stride == 1 || stride == -1)
    return SageBuilder::buildAddOp(SageBuilder::buildSubtractOp(genUpperBound(), genLowerBound()), SageBuilder::buildIntVal(1));
  if (stride < 0)
    return SageBuilder::buildDivideOp(
      SageBuilder::buildAddOp(SageBuilder::buildSubtractOp(genUpperBound(), genLowerBound()), SageBuilder::buildIntVal(1)),
      SageBuilder::buildIntVal(-stride)
    );
  if (stride > 0)
    return SageBuilder::buildDivideOp(
      SageBuilder::buildAddOp(SageBuilder::buildSubtractOp(genUpperBound(), genLowerBound()), SageBuilder::buildIntVal(1)),
      SageBuilder::buildIntVal(stride)
    );
  assert(stride != 0); // If reach the assert should fail.
  assert(false); // this should never be reach
}

LinearDomain::LinearDomain(RoseVariable iterator_, int stride_) :
  Domain(iterator_, stride_),
  lb(),
  ub()
{}

LinearDomain::~LinearDomain() {
  std::vector<std::pair<LinearExpression *, int> >::iterator it;
  for (it = lb.begin(); it != lb.end(); it++)
    delete it->first;
  for (it = ub.begin(); it != ub.end(); it++)
    delete it->first;
}

SgExpression * LinearDomain::genLowerBound() const {
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

SgExpression * LinearDomain::genUpperBound() const {
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


void LinearDomain::addLowerBound(LinearExpression * lb_, int div) {
  lb.push_back(std::pair<LinearExpression *, int>(lb_, div));
}

void LinearDomain::addUpperBound(LinearExpression * ub_, int div) {
  ub.push_back(std::pair<LinearExpression *, int>(ub_, div));
}

const std::vector<std::pair<LinearExpression *, int> > & LinearDomain::getRawLowerBound() const { return stride < 0 ? ub : lb; }
const std::vector<std::pair<LinearExpression *, int> > & LinearDomain::getRawUpperBound() const { return stride < 0 ? lb : ub; }

void LinearDomain::print(std::ostream & out) const {
  // TODO
}

LinearDomain * LinearDomain::copy() const {
  LinearDomain * res = new LinearDomain(iterator, stride);
  std::vector<std::pair<LinearExpression *, int> >::const_iterator it;
  for (it = lb.begin(); it != lb.end(); it++)
    res->addLowerBound(it->first->copy(), it->second);
  for (it = ub.begin(); it != ub.end(); it++)
    res->addUpperBound(it->first->copy(), it->second);
  return res;
}

