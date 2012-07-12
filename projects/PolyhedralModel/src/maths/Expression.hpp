
#ifndef _EXPRESSION_HPP_
#define _EXPRESSION_HPP_

#include "maths/PPLUtils.hpp"

#include <vector>
#include <utility>

template <class T>
class Expression {
  public:
    typedef std::pair<int, std::vector<std::pair<T *, unsigned> > > Term;

  protected:
    std::vector<T *>  p_symbols;
    std::vector<Term> p_coefficients;

  public:
    Expression();
    ~Expression();

    void addTerm(Term &);

    template <class ID_src>
    bool linearForm(LinearExpression_ppl &, ID_src *) const;

    void print(std::ostream &) const;

};

#include "maths/Expression.tpp"

#endif /* _EXPRESSION_HPP_ */

