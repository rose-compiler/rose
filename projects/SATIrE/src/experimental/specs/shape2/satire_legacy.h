/* -*- C++ -*- */
// vim: syntax=cpp

#ifndef H_PAG_ANALYSIS_MAIN
#define H_PAG_ANALYSIS_MAIN

// GB (2008-08-21): I think this inclusion of <config.h> is no longer
// needed.
// #include <config.h>
#include "CFGTraversal.h"
#include "ProcTraversal.h"
#include "TermPrinter.h"

// GB (2008-08-21): ROSE defines macros TRUE and FALSE, then goes redefining
// them in a header called general.h. PAG (here in iterate.h) also includes
// a file called general.h, which is redirected to ROSE's general.h. That in
// itself is not a problem, but the unguarded redefinitions of TRUE and
// FALSE are. So undefine them before including iterate.h.
#undef TRUE
#undef FALSE
#include "iterate.h"

#undef NDEBUG
#include "testcfg.h"
#include "CommandLineParser.h"

#include "StatementAttributeTraversal.h"

#define doit(analysis) xdoit(analysis)
#define xdoit(analysis) analysis##_doit

#define stringize(analysis) xstr(analysis)
#define xstr(analysis) #analysis

#define perform_pag_analysis(analysis) xperform(analysis)
#define xperform(analysis) perform_pag_analysis_##analysis

#define carrier_type_o(type) xcarrier_type(type)
#define xcarrier_type(type) o_##type

#define carrier_printfunc(type) xcarrier_printfunc(type)
#define xcarrier_printfunc(type) o_##type##_print

#if PREFIX_SET
    #define prefixed_DFI_STORE(P) p2_DFI_STORE(P)
    #define prefixed_dfi_get_pre_info(P) p2_dfi_get_pre_info(P)
    #define prefixed_dfi_get_post_info_all(P) p2_dfi_get_post_info_all(P)

    #define p2_DFI_STORE(P) o_##P##DFI_STORE
    #define p2_dfi_get_pre_info(P) o_##P##dfi_get_pre_info
    #define p2_dfi_get_post_info_all(P) o_##P##dfi_get_post_info_all

    #define DFI_STORE prefixed_DFI_STORE(PREFIX)
    #define dfi_get_pre_info prefixed_dfi_get_pre_info(PREFIX)
    #define dfi_get_post_info_all prefixed_dfi_get_post_info_all(PREFIX)
#endif


DFI_STORE perform_pag_analysis(ANALYSIS)(SgProject* root,AnalyzerOptions* opt);
std::string get_statement_pre_info_string(DFI_STORE store, SgStatement* stmt);
std::string get_statement_post_info_string(DFI_STORE store, SgStatement* stmt);
carrier_type_o(CARRIER_TYPE) get_statement_pre_info(DFI_STORE store, SgStatement *stmt);
carrier_type_o(CARRIER_TYPE) get_statement_post_info(DFI_STORE store, SgStatement *stmt);
carrier_type_o(CARRIER_TYPE) select_info(DFI_STORE store, SgStatement *stmt, std::string attrName);

#if 0
template <typename DFI_STORE_TYPE>
class PagDfiTextPrinter : public DfiTextPrinter<DFI_STORE_TYPE> {
public:
  PagDfiTextPrinter(DFI_STORE_TYPE store) : DfiTextPrinter<DFI_STORE>(store) {}
  std::string statementPreInfoString(DFI_STORE_TYPE store, SgStatement *stmt) { 
     return get_statement_pre_info_string(store, stmt);
  }
  std::string statementPostInfoString(DFI_STORE_TYPE store, SgStatement *stmt) { 
     return get_statement_post_info_string(store, stmt);
  }
};
#endif

template <typename DFI_STORE_TYPE>
class PagDfiCommentAnnotator : public DfiCommentAnnotator<DFI_STORE_TYPE> {
public:
  PagDfiCommentAnnotator(DFI_STORE_TYPE store) : DfiCommentAnnotator<DFI_STORE>(store) {}
  std::string statementPreInfoString(DFI_STORE_TYPE store, SgStatement *stmt) { 
     return get_statement_pre_info_string(store, stmt);
  }
  std::string statementPostInfoString(DFI_STORE_TYPE store, SgStatement *stmt) { 
     return get_statement_post_info_string(store, stmt);
  }
};

void satireResetParentPointers(SgNode *ast_root, SgNode *parent);
void runOtherAnalyses(void *cfg);

#endif
