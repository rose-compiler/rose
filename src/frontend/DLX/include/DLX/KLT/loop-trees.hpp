
#ifndef __DLX_KLT_LOOPTREES_HPP__
#define __DLX_KLT_LOOPTREES_HPP__

#include "KLT/Core/loop-trees.hpp"

#include "DLX/KLT/annotations.hpp"
#include "DLX/KLT/data.hpp"

namespace DLX {

namespace KLT {

template <class Language>
void extractLoopTrees(
  const std::vector<typename Language::directive_t *> & directives,
  std::map<typename Language::directive_t *, ::KLT::LoopTrees<Annotation<Language> > *> & loop_trees,
  std::map<SgForStatement *, typename ::KLT::LoopTrees<Annotation<Language> >::loop_t *> & loop_map
) {
  typedef ::KLT::LoopTrees<Annotation<Language> > LoopTrees;
  typedef ::KLT::Data<Annotation<Language> > Data;

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
    const std::vector<Data *> & datas_ = loop_tree->getDatas();
    std::vector<SgVariableSymbol *> datas;
    typename std::vector<Data *>::const_iterator it_data;
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
      loop->annotations.push_back(Annotation<Language>(*it_clause));
    }
  }
}

}

}

#endif /* __DLX_KLT_LOOPTREES_HPP__ */

