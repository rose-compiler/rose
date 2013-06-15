
#ifndef __DATA_HPP__
#define __DATA_HPP__

#include <vector>
#include <utility>

class SgExpression;
class SgVariableSymbol;

namespace KLT {

namespace Core {

class Data {
  public:
    typedef std::pair<SgExpression *, SgExpression *> section_t;

  protected:
    SgVariableSymbol * p_variable_symbol;

    std::vector<section_t> p_sections;

  public:
    Data(SgVariableSymbol * variable);
    virtual ~Data();

    void addSection(section_t section);

    SgVariableSymbol * getVariableSymbol() const;

    const std::vector<section_t> & getSections() const;

    void toText(std::ostream & out) const;
};

}

}

#endif /* __DATA_HPP__ */

