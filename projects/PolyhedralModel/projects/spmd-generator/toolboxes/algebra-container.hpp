
#ifndef __ALGEBRA_CONTAINER_HPP__
#define __ALGEBRA_CONTAINER_HPP__

#include "rose/Variable.hpp"

#include <iostream>
#include <vector>
#include <map>

class SgExpression;
class SgAssignOp;
class SgLessOrEqualOp;
class SgGreaterOrEqualOp;
class SgEqualityOp;
class SgExprStatement;

class Expression {
  public:
    SgAssignOp * genAssignTo(const RoseVariable & var) const;
    SgLessOrEqualOp * genLessOrEqualTo(const RoseVariable & var) const;
    SgGreaterOrEqualOp * genGreaterOrEqualTo(const RoseVariable & var) const;

    SgGreaterOrEqualOp * genGreaterOrEqualToZero() const;
    SgLessOrEqualOp * genLessOrEqualToZero() const;
    SgEqualityOp * genEqualToZero() const;

    virtual SgExpression * generate() const = 0;

    virtual void print(std::ostream & out) const = 0;

    virtual Expression * copy() const = 0;

/*
    virtual Expression & operator + (const Expression & rhs) = 0;
    virtual Expression & operator - (const Expression & rhs) = 0;
    virtual Expression & operator * (int mul) = 0;
    virtual Expression & operator / (int div) = 0;
*/
};

class LinearExpression : public Expression {
  protected:
    std::map<RoseVariable, int> map;

  public:
    LinearExpression();
    LinearExpression(const std::map<RoseVariable, int> & map_);
    LinearExpression(const std::vector<std::pair<RoseVariable, int> > & vect);
    ~LinearExpression();

    void set(const RoseVariable & var, int value);
    int get(const RoseVariable & var) const;
    void add(const RoseVariable & var, int value);
    void neg();

    const std::map<RoseVariable, int> & getRawData() const;

    virtual SgExpression * generate() const;

    virtual void print(std::ostream & out) const;

    virtual LinearExpression * copy() const;
};

class SageExpression : public Expression {
  protected:
    SgExpression * expression;

  public:
    SageExpression(SgExpression * exp);
    ~SageExpression();
    
    virtual SgExpression * generate() const;

    virtual void print(std::ostream & out) const;

    virtual Expression * copy() const;
};

class Domain {
  protected:
    RoseVariable iterator;
    int stride;

  public:
    Domain(RoseVariable iterator_, int stride_);
    virtual ~Domain();

    RoseVariable & getIterator();
    int getStride() const;

    virtual SgExpression * genLowerBound() const = 0;
    virtual SgExpression * genUpperBound() const = 0;

    SgExprStatement * genInit() const;
    SgExprStatement * genTest() const;
    SgExpression * genIncrement() const;
    SgExpression * genNumberOfPoints() const;

    virtual void print(std::ostream & out) const = 0;

    virtual Domain * copy() const = 0;
};

class LinearDomain : public Domain {
  protected:
    std::vector<std::pair<LinearExpression *, int> > lb;
    std::vector<std::pair<LinearExpression *, int> > ub;

  public:
    LinearDomain(RoseVariable iterator_, int stride_);
    ~LinearDomain();

    virtual SgExpression * genLowerBound() const;
    virtual SgExpression * genUpperBound() const;

    void addLowerBound(LinearExpression * lb_, int div);
    void addUpperBound(LinearExpression * ub_, int div);

    const std::vector<std::pair<LinearExpression *, int> > & getRawLowerBound() const;
    const std::vector<std::pair<LinearExpression *, int> > & getRawUpperBound() const;

    virtual void print(std::ostream & out) const;

    virtual LinearDomain * copy() const;
};

#endif /* __ALGEBRA_CONTAINER_HPP__ */

