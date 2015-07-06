
#ifndef __DLX_KLT_LOOPTREES_HPP__
#define __DLX_KLT_LOOPTREES_HPP__

#include "DLX/KLT/annotations.hpp"
#include "KLT/Core/loop-trees.hpp"

namespace DLX {

template <class Language> 
void extractLoopTrees(
  const std::vector<DLX::Directives::directive_t<Language> *> & directives,
  std::map<DLX::Directives::directive_t<Language> *, KLT::LoopTrees< ::DLX::KLT_Annotation<Language> > *> & loop_trees,
  std::map<SgForStatement *, typename KLT::LoopTrees< ::DLX::KLT_Annotation<Language> >::loop_t *> & loop_map
) {
  typedef ::DLX::KLT_Annotation<Language> Annotation;
  typedef ::KLT::LoopTrees<Annotation> LoopTrees;

  typename std::vector<DLX::Directives::directive_t<Language> *>::const_iterator it_directive;
  for (it_directive = directives.begin(); it_directive != directives.end(); it_directive++) {

    DLX::Directives::directive_t<Language> * directive = *it_directive;

    if (directive->construct->kind != Language::e_construct_kernel) continue;                                    // (language dependant)
    DLX::Directives::construct_t<Language, Language::e_construct_kernel> * construct =                           // (language dependant)
                 (DLX::Directives::construct_t<Language, Language::e_construct_kernel> *)(directive->construct); // (language dependant)

    // Create empty loop-tree (generic)
    LoopTrees * loop_tree = new LoopTrees();
    loop_trees.insert(std::pair<DLX::Directives::directive_t<Language> *, LoopTrees *>(directive, loop_tree));

    // Add data to loop-tree (language dependant)
    typename std::vector<DLX::Directives::generic_clause_t<Language> *>::const_iterator it_clause;
    for (it_clause = directive->clause_list.begin(); it_clause != directive->clause_list.end(); it_clause++) {
      DLX::Directives::generic_clause_t<Language> * clause = *it_clause;

      if (clause->kind != Language::e_clause_data) continue;                               // (language dependant)
      DLX::Directives::clause_t<Language, Language::e_clause_data> * data_clause =         // (language dependant)
                   (DLX::Directives::clause_t<Language, Language::e_clause_data> *)clause; // (language dependant)

      std::vector<DLX::Frontend::data_sections_t>::const_iterator it_data_sections;
      for (it_data_sections = data_clause->parameters.data_sections.begin(); it_data_sections != data_clause->parameters.data_sections.end(); it_data_sections++) {
        SgVariableSymbol * data_sym = it_data_sections->first;

        SgType * base_type = data_sym->get_type();
        std::vector<DLX::Frontend::section_t>::const_iterator it_section;
        for (it_section = it_data_sections->second.begin(); it_section != it_data_sections->second.end(); it_section++) {
          SgPointerType * ptr_type = isSgPointerType(base_type);
          SgArrayType * arr_type = isSgArrayType(base_type);
          if (ptr_type != NULL)
            base_type = ptr_type->get_base_type();
          else if (arr_type != NULL)
            base_type = arr_type->get_base_type();
          else assert(false);
          assert(base_type != NULL);
        }
        KLT::Data<Annotation> * data = new KLT::Data<Annotation>(data_sym, base_type);
        for (it_section = it_data_sections->second.begin(); it_section != it_data_sections->second.end(); it_section++) {
          typename KLT::Data<Annotation>::section_t section;
            section.lower_bound = it_section->lower_bound;
            section.size = it_section->size;
            section.stride = it_section->stride;
          data->addSection(section);
        }
        loop_tree->addData(data);
      }
    }

    std::vector<SgVariableSymbol *> iterators, locals, others;

    // Parse the loop nest (generic)
    SgStatement * region_base = construct->assoc_nodes.kernel_region;
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
    DLX::Directives::directive_t<Language> * directive = *it_directive;

    if (directive->construct->kind != Language::e_construct_loop) continue;                                                                     // (language dependant)
    DLX::Directives::construct_t<Language, Language::e_construct_loop> * construct =                                                            // (language dependant)
                 (DLX::Directives::construct_t<Language, Language::e_construct_loop> *)(directive->construct);                                  // (language dependant)
    assert(construct->assoc_nodes.for_loop != NULL);                                                                                            // (language dependant)
    typename std::map<SgForStatement *, typename LoopTrees::loop_t *>::const_iterator it_loop = loop_map.find(construct->assoc_nodes.for_loop); // (language dependant)
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

