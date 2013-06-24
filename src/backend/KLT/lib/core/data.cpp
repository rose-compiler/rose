
#include "KLT/Core/data.hpp"

#include "sage3basic.h"

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
  out << p_variable_symbol->get_name().getString();
  std::vector<Data::section_t>::const_iterator it_section;
  for (it_section = p_sections.begin(); it_section != p_sections.end(); it_section++) {
    out << "[";
    out << it_section->first->unparseToString();
    if (it_section->first != it_section->second) {
      out << ":";
      out << it_section->second->unparseToString();
    }
    out << "]";
  }
}

}

}
