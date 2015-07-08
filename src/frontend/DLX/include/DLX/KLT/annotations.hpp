
#ifndef __DLX_KLT_ANNOTATIONS_HPP__
#define __DLX_KLT_ANNOTATIONS_HPP__

#include <vector>

namespace DLX {

namespace Directives {

template <class language_tpl> struct generic_clause_t;

}

/*!
 * \addtogroup grp_dlx_klt_annotations
 * @{
*/

template <class Lang>
class KLT_Annotation {
  private:
    void createClause() {
      assert(Lang::s_clause_labels.size() > 0);

      typename Lang::clause_labels_map_t::const_iterator it_clause;
      for (it_clause = Lang::s_clause_labels.begin(); it_clause != Lang::s_clause_labels.end(); it_clause++) {
        const typename Lang::label_set_t & labels = it_clause->second;
        typename Lang::label_set_t::const_iterator it_label;
        for (it_label = labels.begin(); it_label != labels.end(); it_label++) {
          if (AstFromString::afs_match_substr(it_label->c_str()))
            clause = Directives::buildClause<Lang>(it_clause->first);
        }
      }

      assert(clause != NULL);
    }

    static void parseClause(std::vector<KLT_Annotation<Lang> > & container) {
      container.push_back(KLT_Annotation<Lang>());
      container.back().createClause();
    }

  public:
    KLT_Annotation(Directives::generic_clause_t<Lang> * clause_ = NULL):
      clause(clause_)
    {}

    Directives::generic_clause_t<Lang> * clause;

    static bool matchLabel() {
      AstFromString::afs_skip_whitespace();
      return AstFromString::afs_match_substr(Lang::language_label.c_str());
    }

    static void parseData   (std::vector<KLT_Annotation<Lang> > & container);

    static void parseRegion (std::vector<KLT_Annotation<Lang> > & container);

    static void parseLoop   (std::vector<KLT_Annotation<Lang> > & container);
};

/** @} */

}

#endif /* __DLX_KLT_ANNOTATIONS_HPP__ */

