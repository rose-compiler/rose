#ifndef H_PAG_ANALYSIS_MAIN
#define H_PAG_ANALYSIS_MAIN

#include <config.h>
#include "CFGTraversal.h"
#include "ProcTraversal.h"

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

DFI_STORE perform_pag_analysis(ANALYSIS)(SgProject* root,char* output,bool noresult);
std::string get_statement_pre_info_string(DFI_STORE store, SgStatement* stmt);
std::string get_statement_post_info_string(DFI_STORE store, SgStatement* stmt);
carrier_type_o(CARRIER_TYPE) get_statement_pre_info(DFI_STORE store, SgStatement *stmt);
carrier_type_o(CARRIER_TYPE) get_statement_post_info(DFI_STORE store, SgStatement *stmt);
carrier_type_o(CARRIER_TYPE) select_info(DFI_STORE store, SgStatement *stmt, std::string attrName);

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

#endif
