
#ifndef __DLX_KLT_DATA_HPP__
#define __DLX_KLT_DATA_HPP__

#include "DLX/KLT/annotations.hpp"
#include "KLT/Core/data.hpp"

namespace DLX {

namespace KLT {

template <class Language>
::KLT::Data<Annotation<Language> > * convertData(typename Language::data_clause_t * data_clause, const DLX::Frontend::data_sections_t & data_section) {
  typedef Annotation<Language> Annotation;
  typedef ::KLT::Data<Annotation> Data;

  SgVariableSymbol * data_sym = data_section.first;

  SgType * base_type = data_sym->get_type();
  std::vector<DLX::Frontend::section_t>::const_iterator it_section;
  for (it_section = data_section.second.begin(); it_section != data_section.second.end(); it_section++) {
         if (isSgPointerType(base_type)) base_type = ((SgPointerType *)base_type)->get_base_type();
    else if (isSgArrayType  (base_type)) base_type = ((SgArrayType   *)base_type)->get_base_type();
    else assert(false);
    assert(base_type != NULL);
  }
  Data * data = new Data(data_sym, base_type);
  for (it_section = data_section.second.begin(); it_section != data_section.second.end(); it_section++) {
    typename Data::section_t section;
      section.offset = it_section->lower_bound;
      section.length = it_section->size;
//    section.stride = it_section->stride;
      assert(it_section->stride == NULL);
    data->addSection(section);
  }
  data->annotations.push_back(Annotation(data_clause));
  return data;
}

template <class Language>
void extractData(const std::vector<DLX::Directives::generic_clause_t<Language> *> & clauses, ::KLT::LoopTrees< Annotation<Language> > * loop_tree) {
  typedef Annotation<Language> Annotation;

  typename std::vector<typename Language::clause_t *>::const_iterator it_clause;
  for (it_clause = clauses.begin(); it_clause != clauses.end(); it_clause++) {
    typename Language::data_clause_t * data_clause = Language::isDataClause(*it_clause);
    if (data_clause != NULL) {
      const std::vector<DLX::Frontend::data_sections_t> & data_sections = Language::getDataSections(data_clause);
      std::vector<DLX::Frontend::data_sections_t>::const_iterator it_data_sections;
      for (it_data_sections = data_sections.begin(); it_data_sections != data_sections.end(); it_data_sections++)
        loop_tree->addData(convertData<Language>(data_clause, *it_data_sections));
    }
    else loop_tree->annotations.push_back(Annotation(*it_clause));
  }
}

}

}

#endif /* __DLX_KLT_DATA_HPP__ */

