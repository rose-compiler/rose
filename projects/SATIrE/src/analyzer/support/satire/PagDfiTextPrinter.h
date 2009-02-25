/* -*- C++ -*- */
// vim: syntax=cpp

#ifndef H_PAG_DFI_TEXT_PRINTER
#define H_PAG_DFI_TEXT_PRINTER

#include "StatementAttributeTraversal.h"

template <typename DFI_STORE_TYPE>
class PagDfiTextPrinter : public DfiTextPrinter<DFI_STORE_TYPE>
{
public:
    PagDfiTextPrinter(DFI_STORE_TYPE store)
      : DfiTextPrinter<DFI_STORE_TYPE>(store)
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
