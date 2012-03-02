/* -*- C++ -*- */
// vim: syntax=cpp

#ifndef H_PAG_DFI_COMMENT_ANNOTATOR
#define H_PAG_DFI_COMMENT_ANNOTATOR

template <typename DFI_STORE_TYPE>
class PagDfiCommentAnnotator : public DfiCommentAnnotator<DFI_STORE_TYPE>
{
public:
    PagDfiCommentAnnotator(DFI_STORE_TYPE store)
      : DfiCommentAnnotator<DFI_STORE_TYPE>(store)
    {
    }

    std::string
    statementPreInfoString(DFI_STORE_TYPE store, SgStatement *stmt)
    {
        return get_statement_pre_info_string(store, stmt);
    }

    std::string
    statementPostInfoString(DFI_STORE_TYPE store, SgStatement *stmt)
    {
        return get_statement_post_info_string(store, stmt);
    }
};

#endif
