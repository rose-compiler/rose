
#include "KLT/Core/data.hpp"

#include "sage3basic.h"

namespace KLT {

namespace Core {

// typedef std::pair<SgExpression *, SgExpression *> section_t;

//  SgVariableSymbol * p_variable_symbol;

//  std::vector<section_t> p_sections;

Data::Data(SgVariableSymbol * variable) :
  p_variable_symbol(variable),
  p_base_type(NULL),
  p_sections()
{}

// SgType * Data::getBaseType() const { return p_base_type; }

SgType * Data::getBaseType() {
  if (p_base_type != NULL) return p_base_type;

  p_base_type = p_variable_symbol->get_type();

  unsigned int nbr_dims = p_sections.size();

  assert(nbr_dims > 0); // FIXME artificial limitation for debugging

  SgPointerType * ptr_type = isSgPointerType(p_base_type);
  unsigned int cnt = 0;

  while (ptr_type != NULL && cnt < nbr_dims) {
    p_base_type = ptr_type->get_base_type();
    ptr_type = isSgPointerType(p_base_type);
    cnt++;
  }
  assert(cnt == nbr_dims);
  assert(isSgPointerType(p_base_type) == NULL); // FIXME artificial limitation for debugging

  return p_base_type;
}

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

bool Data::subset(Data * d) const {
  assert(getVariableSymbol() == d->getVariableSymbol());
  // FIXME currently: same symbol => same data
  return false;
}

bool Data::match(Data * d) const {
  assert(getVariableSymbol() == d->getVariableSymbol());
  // FIXME currently: same symbol => same data
  return true;
}

Data * Data::remove(Data * d) {
  if (getVariableSymbol() != d->getVariableSymbol()) return this;
  return NULL; // FIXME currently: same symbol => same data
}

Data * Data::add(Data * d) {
  if (getVariableSymbol() != d->getVariableSymbol()) this;
  return this; // FIXME currently: same symbol => same data
}

bool Data::less(Data * d1, Data * d2) {
  return (d1->getVariableSymbol() < d2->getVariableSymbol()) || ((d1->getVariableSymbol() == d2->getVariableSymbol()) && d1->subset(d2));
}

bool Data::equal(Data * d1, Data * d2) {
  return (d1->getVariableSymbol() == d2->getVariableSymbol()) && d1->match(d2);
}

void Data::set_union(std::set<Data *> & result_, const std::set<Data *> & datas_1_, const std::set<Data *> & datas_2_) {
  assert(result_.empty());
  // FIXME currently: same symbol => same data

  std::vector<Data *> datas_1(datas_1_.begin(), datas_1_.end());
  std::vector<Data *> datas_2(datas_2_.begin(), datas_2_.end());
  std::vector<Data *> result(datas_1.size() + datas_2.size());

  std::sort(datas_1.begin(), datas_1.end(), less);
  std::sort(datas_2.begin(), datas_2.end(), less);

  std::vector<Data *>::iterator it_result_begin = result.begin();
  std::vector<Data *>::iterator it_result_end = std::set_union(datas_1.begin(), datas_1.end(), datas_2.begin(), datas_2.end(), it_result_begin, less);

  result_.insert(it_result_begin, it_result_end);
}

void Data::set_intersection(std::set<Data *> & result_, const std::set<Data *> & datas_1_, const std::set<Data *> & datas_2_) {
  assert(result_.empty());
  // FIXME currently: same symbol => same data

  std::vector<Data *> datas_1(datas_1_.begin(), datas_1_.end());
  std::vector<Data *> datas_2(datas_2_.begin(), datas_2_.end());
  std::vector<Data *> result(std::min(datas_1.size(), datas_2.size()));

  std::sort(datas_1.begin(), datas_1.end(), less);
  std::sort(datas_2.begin(), datas_2.end(), less);

  std::vector<Data *>::iterator it_result_begin = result.begin();
  std::vector<Data *>::iterator it_result_end = std::set_intersection(datas_1.begin(), datas_1.end(), datas_2.begin(), datas_2.end(), it_result_begin, less);

  result_.insert(it_result_begin, it_result_end);
}

void Data::set_remove(std::set<Data *> & result_, const std::set<Data *> & datas) {
  // FIXME currently: same symbol => same data
  std::list<Data *> result(result_.begin(), result_.end());
  result_.clear();

  std::list<Data *>::iterator it_result = result.begin();
  while (it_result != result.end()) {
    Data * data = *it_result;
    
    std::set<Data *>::iterator it_data;
    for (it_data = datas.begin(); it_data != datas.end(); it_data++) {
      data = data->remove(*it_data);
      if (data == NULL) break;
    }

    if (data != NULL)
      result_.insert(data);

    it_result++;
  }

  // TODO minimize result_
}

void collectBoundExpressions(const std::set<Data *> & datas, std::set<SgExpression *> & exprs) {
  std::vector<SgVarRefExp *> var_refs;
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;

  std::set<Data *>::const_iterator it_data;
  for (it_data = datas.begin(); it_data != datas.end(); it_data++) {
    const std::vector<Data::section_t> & sections = (*it_data)->getSections();
    std::vector<Data::section_t>::const_iterator it_section;
    for (it_section = sections.begin(); it_section != sections.end(); it_section++) {
      exprs.insert(it_section->first);
      exprs.insert(it_section->second);
    }
  }
}

void collectReferencedSymbols(const std::set<Data *> & datas, std::set<SgVariableSymbol *> & symbols) {
  std::vector<SgVarRefExp *> var_refs;
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;
  
  std::set<Data *>::const_iterator it_data;
  for (it_data = datas.begin(); it_data != datas.end(); it_data++) {
    const std::vector<Data::section_t> & sections = (*it_data)->getSections();
    std::vector<Data::section_t>::const_iterator it_section;
    for (it_section = sections.begin(); it_section != sections.end(); it_section++) {
      var_refs = SageInterface::querySubTree<SgVarRefExp>(it_section->first);
      for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++)
        symbols.insert((*it_var_ref)->get_symbol());

      var_refs = SageInterface::querySubTree<SgVarRefExp>(it_section->second);
      for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++)
        symbols.insert((*it_var_ref)->get_symbol());
    }
  }
}

}

}
