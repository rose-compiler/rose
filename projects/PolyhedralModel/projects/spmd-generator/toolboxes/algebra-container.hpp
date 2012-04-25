
#ifndef __ALGEBRA_CONTAINER_HPP__
#define __ALGEBRA_CONTAINER_HPP__

#include "rose/Variable.hpp"

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

    void set(RoseVariable & var, int value);
    int get(RoseVariable & var) const;

    void add(RoseVariable & var, int value);

    virtual SgExpression * generate() const;

    virtual LinearExpression * copy() const;
};

class Bounds {
  protected:
    RoseVariable iterator;
    int stride;

  public:
    Bounds(RoseVariable iterator_, int stride_);
    virtual ~Bounds();

    RoseVariable & getIterator();

    virtual SgExpression * genLowerBound() const = 0;
    virtual SgExpression * genUpperBound() const = 0;

    SgExprStatement * genInit() const;
    SgExprStatement * genTest() const;
    SgExpression * genIncrement() const;

    virtual Bounds * copy() const = 0;
};

class LinearBounds : public Bounds {
  protected:
    std::vector<std::pair<LinearExpression *, int> > lb;
    std::vector<std::pair<LinearExpression *, int> > ub;

  public:
    LinearBounds(RoseVariable iterator_, int stride_);
    ~LinearBounds();

    virtual SgExpression * genLowerBound() const;
    virtual SgExpression * genUpperBound() const;

    void addLowerBound(LinearExpression * lb_, int div);
    void addUpperBound(LinearExpression * ub_, int div);

    virtual LinearBounds * copy() const;
};

#endif /* __ALGEBRA_CONTAINER_HPP__ */

