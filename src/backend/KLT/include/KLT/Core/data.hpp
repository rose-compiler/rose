
#ifndef __DATA_HPP__
#define __DATA_HPP__

#include <set>
#include <vector>
#include <utility>

#include <iostream>

class SgExpression;
class SgVariableSymbol;
class SgType;

namespace KLT {

namespace Core {

class Data {
  public:
    typedef std::pair<SgExpression *, SgExpression *> section_t;

  protected:
    SgVariableSymbol * p_variable_symbol;

    SgType * p_base_type;

    std::vector<section_t> p_sections;

  protected:
    bool subset(Data * d) const;
    bool match(Data * d) const;
    Data * remove(Data * d);
    Data * add(Data * d);

  protected:
    static bool less(Data * d1, Data * d2);
    static bool equal(Data * d1, Data * d2);

  public:
    Data(SgVariableSymbol * variable);
    virtual ~Data();

    void addSection(section_t section);

    SgVariableSymbol * getVariableSymbol() const;

    SgType * getBaseType();
//  SgType * getBaseType() const;

    const std::vector<section_t> & getSections() const;

    void toText(std::ostream & out) const;

  static void set_union         (std::set<Data *> & result_, const std::set<Data *> & datas_1_, const std::set<Data *> & datas_2_);
  static void set_intersection  (std::set<Data *> & result_, const std::set<Data *> & datas_1_, const std::set<Data *> & datas_2_);
  static void set_remove        (std::set<Data *> & result_, const std::set<Data *> & datas_);
};

void collectBoundExpressions(const std::set<Data *> & datas, std::set<SgExpression *> & exprs);
void collectReferencedSymbols(const std::set<Data *> & datas, std::set<SgVariableSymbol *> & symbols);

}

}

#endif /* __DATA_HPP__ */

