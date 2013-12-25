
#ifndef __KLT_DATA_HPP__
#define __KLT_DATA_HPP__

#include <set>
#include <list>
#include <vector>
#include <utility>

#include <iostream>

class SgExpression;
class SgVariableSymbol;
class SgType;
class SgFunctionParameterList;
class SgTypeModifier;

namespace KLT {

/*!
 * \addtogroup grp_klt_data
 * @{
*/

template <class Annotation>
class Data {
  public:
    typedef std::pair<SgExpression *, SgExpression *> section_t;

  protected:
    SgVariableSymbol * p_variable_symbol;

    SgType * p_base_type;

    std::vector<section_t> p_sections;

  public:
    std::vector<Annotation> annotations;

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

    const std::vector<section_t> & getSections() const;

    bool isFlowIn() const;
    bool isFlowOut() const;

    void toText(std::ostream & out) const;

  static void set_union(
      std::set<Data<Annotation> *> & result_,
      const std::set<Data<Annotation> *> & datas_1_,
      const std::set<Data<Annotation> *> & datas_2_
  );
  static void set_intersection(
      std::set<Data<Annotation> *> & result_,
      const std::set<Data<Annotation> *> & datas_1_,
      const std::set<Data<Annotation> *> & datas_2_
  );
  static void set_remove(
      std::set<Data<Annotation> *> & result_,
      const std::set<Data<Annotation> *> & datas_
  );
};

//////////////////////////////////////////////////////////////////////////////////////

template <class Annotation>
void collectBoundExpressions(const std::set<Data<Annotation> *> & datas, std::set<SgExpression *> & exprs);

template <class Annotation>
void collectReferencedSymbols(const std::set<Data<Annotation> *> & datas, std::set<SgVariableSymbol *> & symbols);

//////////////////////////////////////////////////////////////////////////////////////

template <class Annotation>
Data<Annotation>::Data(SgVariableSymbol * variable) :
  p_variable_symbol(variable),
  p_base_type(NULL),
  p_sections(),
  annotations()
{}

template <class Annotation>
SgType * Data<Annotation>::getBaseType() {
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

template <class Annotation>
Data<Annotation>::~Data() {}

template <class Annotation>
void Data<Annotation>::addSection(section_t section) { p_sections.push_back(section); }

template <class Annotation>
SgVariableSymbol * Data<Annotation>::getVariableSymbol() const { return p_variable_symbol; }

template <class Annotation>
const std::vector<typename Data<Annotation>::section_t> & Data<Annotation>::getSections() const { return p_sections; }

template <class Annotation>
void Data<Annotation>::toText(std::ostream & out) const {
  out << p_variable_symbol->get_name().getString();
  typename std::vector<typename Data<Annotation>::section_t>::const_iterator it_section;
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

template <class Annotation>
bool Data<Annotation>::subset(Data<Annotation> * d) const {
  assert(getVariableSymbol() == d->getVariableSymbol());
  // FIXME currently: same symbol => same data
  return false;
}

template <class Annotation>
bool Data<Annotation>::match(Data<Annotation> * d) const {
  assert(getVariableSymbol() == d->getVariableSymbol());
  // FIXME currently: same symbol => same data
  return true;
}

template <class Annotation>
Data<Annotation> * Data<Annotation>::remove(Data<Annotation> * d) {
  if (getVariableSymbol() != d->getVariableSymbol()) return this;
  return NULL; // FIXME currently: same symbol => same data
}

template <class Annotation>
Data<Annotation> * Data<Annotation>::add(Data<Annotation> * d) {
  if (getVariableSymbol() != d->getVariableSymbol()) this;
  return this; // FIXME currently: same symbol => same data
}

template <class Annotation>
bool Data<Annotation>::less(Data<Annotation> * d1, Data<Annotation> * d2) {
  return (d1->getVariableSymbol() < d2->getVariableSymbol()) || ((d1->getVariableSymbol() == d2->getVariableSymbol()) && d1->subset(d2));
}

template <class Annotation>
bool Data<Annotation>::equal(Data<Annotation> * d1, Data<Annotation> * d2) {
  return (d1->getVariableSymbol() == d2->getVariableSymbol()) && d1->match(d2);
}

template <class Annotation>
void Data<Annotation>::set_union(
  std::set<Data<Annotation> *> & result_,
  const std::set<Data<Annotation> *> & datas_1_,
  const std::set<Data<Annotation> *> & datas_2_
) {
  assert(result_.empty());
  // FIXME currently: same symbol => same data

  std::vector<Data<Annotation> *> datas_1(datas_1_.begin(), datas_1_.end());
  std::vector<Data<Annotation> *> datas_2(datas_2_.begin(), datas_2_.end());
  std::vector<Data<Annotation> *> result(datas_1.size() + datas_2.size());

  std::sort(datas_1.begin(), datas_1.end(), less);
  std::sort(datas_2.begin(), datas_2.end(), less);

  typename std::vector<Data<Annotation> *>::iterator it_result_begin = result.begin();
  typename std::vector<Data<Annotation> *>::iterator it_result_end = std::set_union(datas_1.begin(), datas_1.end(), datas_2.begin(), datas_2.end(), it_result_begin, less);

  result_.insert(it_result_begin, it_result_end);
}

template <class Annotation>
void Data<Annotation>::set_intersection(
  std::set<Data<Annotation> *> & result_,
  const std::set<Data<Annotation> *> & datas_1_,
  const std::set<Data<Annotation> *> & datas_2_
) {
  assert(result_.empty());
  // FIXME currently: same symbol => same data

  std::vector<Data<Annotation> *> datas_1(datas_1_.begin(), datas_1_.end());
  std::vector<Data<Annotation> *> datas_2(datas_2_.begin(), datas_2_.end());
  std::vector<Data<Annotation> *> result(std::min(datas_1.size(), datas_2.size()));

  std::sort(datas_1.begin(), datas_1.end(), less);
  std::sort(datas_2.begin(), datas_2.end(), less);

  typename std::vector<Data<Annotation> *>::iterator it_result_begin = result.begin();
  typename std::vector<Data<Annotation> *>::iterator it_result_end = std::set_intersection(datas_1.begin(), datas_1.end(), datas_2.begin(), datas_2.end(), it_result_begin, less);

  result_.insert(it_result_begin, it_result_end);
}

template <class Annotation>
void Data<Annotation>::set_remove(std::set<Data<Annotation> *> & result_, const std::set<Data<Annotation> *> & datas) {
  // FIXME currently: same symbol => same data
  std::list<Data<Annotation> *> result(result_.begin(), result_.end());
  result_.clear();

  typename std::list<Data<Annotation> *>::iterator it_result = result.begin();
  while (it_result != result.end()) {
    Data<Annotation> * data = *it_result;
    
    typename std::set<Data<Annotation> *>::iterator it_data;
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

//////////////////////////////////////////////////////////////////////////////////////

template <class Annotation>
void collectBoundExpressions(const std::set<Data<Annotation> *> & datas, std::set<SgExpression *> & exprs) {
  std::vector<SgVarRefExp *> var_refs;
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;

  typename std::set<Data<Annotation> *>::const_iterator it_data;
  for (it_data = datas.begin(); it_data != datas.end(); it_data++) {
    const std::vector<typename Data<Annotation>::section_t> & sections = (*it_data)->getSections();
    typename std::vector<typename Data<Annotation>::section_t>::const_iterator it_section;
    for (it_section = sections.begin(); it_section != sections.end(); it_section++) {
      exprs.insert(it_section->first);
      exprs.insert(it_section->second);
    }
  }
}

template <class Annotation>
void collectReferencedSymbols(const std::set<Data<Annotation> *> & datas, std::set<SgVariableSymbol *> & symbols) {
  std::vector<SgVarRefExp *> var_refs;
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;
  
  typename std::set<Data<Annotation> *>::const_iterator it_data;
  for (it_data = datas.begin(); it_data != datas.end(); it_data++) {
    const std::vector<typename Data<Annotation>::section_t> & sections = (*it_data)->getSections();
    typename std::vector<typename Data<Annotation>::section_t>::const_iterator it_section;
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

/** @} */

}

#endif /* __KLT_DATA_HPP__ */

