
#include "KLT/Core/data.hpp"

namespace KLT {

namespace Core {

// typedef std::pair<SgExpression *, SgExpression *> section_t;

//  SgVariableSymbol * p_variable_symbol;

//  std::vector<section_t> p_sections;

Data::Data(SgVariableSymbol * variable) :
  p_variable_symbol(variable),
  p_sections()
{}

Data::~Data() {}

void Data::addSection(section_t section) { p_sections.push_back(section); }

SgVariableSymbol * Data::getVariableSymbol() const { return p_variable_symbol; }

const std::vector<Data::section_t> & Data::getSections() const { return p_sections; }

void Data::toText(std::ostream & out) const {
  // TODO
}

}

}
