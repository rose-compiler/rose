
#ifndef __DLX_KLT_LOOPTREES_HPP__
#define __DLX_KLT_LOOPTREES_HPP__

#include "DLX/KLT/annotations.hpp"
#include "KLT/Core/loop-trees.hpp"

namespace DLX {

template <class Language>
KLT::Data<KLT_Annotation<Language> > * convertData(typename Language::data_clause_t * data_clause, const DLX::Frontend::data_sections_t & data_section) {
  typedef KLT_Annotation<Language> Annotation;

  SgVariableSymbol * data_sym = data_section.first;

  SgType * base_type = data_sym->get_type();
  std::vector<DLX::Frontend::section_t>::const_iterator it_section;
  for (it_section = data_section.second.begin(); it_section != data_section.second.end(); it_section++) {
         if (isSgPointerType(base_type)) base_type = ((SgPointerType *)base_type)->get_base_type();
    else if (isSgArrayType  (base_type)) base_type = ((SgArrayType   *)base_type)->get_base_type();
    else assert(false);
    assert(base_type != NULL);
  }
  KLT::Data<Annotation> * data = new KLT::Data<Annotation>(data_sym, base_type);
  for (it_section = data_section.second.begin(); it_section != data_section.second.end(); it_section++) {
    typename KLT::Data<Annotation>::section_t section;
      section.lower_bound = it_section->lower_bound;
      section.size = it_section->size;
      section.stride = it_section->stride;
    data->addSection(section);
  }
  data->annotations.push_back(Annotation(data_clause));
  return data;
}

template <class Language>
void extractData(const std::vector<DLX::Directives::generic_clause_t<Language> *> & clauses, KLT::LoopTrees< KLT_Annotation<Language> > * loop_tree) {
  typedef KLT_Annotation<Language> Annotation;

  typename std::vector<typename Language::clause_t *>::const_iterator it_clause;
  for (it_clause = clauses.begin(); it_clause != clauses.end(); it_clause++) {
    typename Language::data_clause_t * data_clause = Language::isDataClause(*it_clause);
    if (data_clause == NULL) continue;

    const std::vector<DLX::Frontend::data_sections_t> & data_sections = Language::getDataSections(data_clause);
    std::vector<DLX::Frontend::data_sections_t>::const_iterator it_data_sections;
    for (it_data_sections = data_sections.begin(); it_data_sections != data_sections.end(); it_data_sections++)
      loop_tree->addData(convertData<Language>(data_clause, *it_data_sections));
  }
}

template <class Language>
void extractLoopTrees(
  const std::vector<typename Language::directive_t *> & directives,
  std::map<typename Language::directive_t *, KLT::LoopTrees< KLT_Annotation<Language> > *> & loop_trees,
  std::map<SgForStatement *, typename KLT::LoopTrees< KLT_Annotation<Language> >::loop_t *> & loop_map
) {
  typedef KLT_Annotation<Language> Annotation;
  typedef ::KLT::LoopTrees<Annotation> LoopTrees;

  typename std::vector<typename Language::directive_t *>::const_iterator it_directive;
  for (it_directive = directives.begin(); it_directive != directives.end(); it_directive++) {
    typename Language::directive_t * directive = *it_directive;

    typename Language::kernel_construct_t * kernel_construct = Language::isKernelConstruct(directive->construct);
    if (kernel_construct == NULL) continue;

    // Create empty loop-tree (generic)
    LoopTrees * loop_tree = new LoopTrees();
    loop_trees.insert(std::pair<typename Language::directive_t *, LoopTrees *>(directive, loop_tree));

    // Extract Data
    extractData(directive->clause_list, loop_tree);

    std::vector<SgVariableSymbol *> iterators, locals, others;

    // Parse the loop nest (generic)
    SgStatement * region_base = Language::getKernelRegion(kernel_construct);
    SgBasicBlock * region_bb = isSgBasicBlock(region_base);
    size_t loop_cnt = 0;
    if (region_bb != NULL) {
      std::vector<SgStatement *>::const_iterator it_stmt;
      for (it_stmt = region_bb->get_statements().begin(); it_stmt != region_bb->get_statements().end(); it_stmt++)
        if (!isSgPragmaDeclaration(*it_stmt)) {
          loop_tree->addTree(LoopTrees::build(*it_stmt, loop_tree, iterators, locals, others, loop_map, loop_cnt));
        }
    }
    else {
      loop_tree->addTree(LoopTrees::build(region_base, loop_tree, iterators, locals, others, loop_map, loop_cnt));
    }

    // Detect scalar used in loop-tree
    const std::vector<SgVariableSymbol *> & params = loop_tree->getParameters();
    const std::vector<KLT::Data<Annotation> *> & datas_ = loop_tree->getDatas();
    std::vector<SgVariableSymbol *> datas;
    typename std::vector<KLT::Data<Annotation> *>::const_iterator it_data;
    for (it_data = datas_.begin(); it_data != datas_.end(); it_data++)
      datas.push_back((*it_data)->getVariableSymbol());
    std::vector<SgVariableSymbol *>::const_iterator it_other;
    for (it_other = others.begin(); it_other != others.end(); it_other++)
      if (std::find(params.begin(), params.end(), *it_other) == params.end() && std::find(datas.begin(), datas.end(), *it_other) == datas.end())
        loop_tree->addScalar(*it_other); // Neither iterators or parameters or data
  }

  // Apply loop construct to loops in loop-tree
  for (it_directive = directives.begin(); it_directive != directives.end(); it_directive++) {
    typename Language::directive_t * directive = *it_directive;

    typename Language::loop_construct_t * loop_construct = Language::isLoopConstruct(directive->construct);
    if (loop_construct == NULL) continue;
    SgForStatement * for_loop = Language::getLoopStatement(loop_construct);
    assert(for_loop != NULL);

    typename std::map<SgForStatement *, typename LoopTrees::loop_t *>::const_iterator it_loop = loop_map.find(for_loop);
    assert(it_loop != loop_map.end());

    typename LoopTrees::loop_t * loop = it_loop->second;

    const std::vector<DLX::Directives::generic_clause_t<Language> *> & clauses = directive->clause_list;
    typename std::vector<DLX::Directives::generic_clause_t<Language> *>::const_iterator it_clause;
    for (it_clause = clauses.begin(); it_clause != clauses.end(); it_clause++) {
      loop->annotations.push_back(DLX::KLT_Annotation<Language>(*it_clause));
    }
  }
}

}

#endif /* __DLX_KLT_LOOPTREES_HPP__ */

