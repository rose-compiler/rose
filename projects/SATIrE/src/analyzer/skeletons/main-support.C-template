/* Specify the carrier type of the analysis. This should be the same
 * as the one given with "carrier:" in the optla file.
 * For example: #define CARRIER_TYPE State */

#ifndef CARRIER_TYPE
    #define CARRIER_TYPE PLEASE_SPECIFY_CARRIER_TYPE
#endif

#include "main.h"
#include "cfg_support.h"
#include "analysis_info.h"

extern "C" DFI_STORE doit(ANALYSIS)(void *);
extern "C" void gdl_create(char *, int);

std::string get_statement_pre_info_string(DFI_STORE store, SgStatement* stmt) {
  return (carrier_printfunc(CARRIER_TYPE)(
					  (carrier_type_o(CARRIER_TYPE))
					  get_statement_pre_info(store, stmt)
					  ));
}

std::string get_statement_post_info_string(DFI_STORE store, SgStatement* stmt) {
  return (carrier_printfunc(CARRIER_TYPE)(
					  (carrier_type_o(CARRIER_TYPE))
					  get_statement_post_info(store, stmt)
					  ));
}

carrier_type_o(CARRIER_TYPE) 
select_info(DFI_STORE store, SgStatement *stmt, std::string attrName) {
  StatementAttribute* stmtAttr = dynamic_cast<StatementAttribute *>(stmt->getAttribute(attrName));
  BasicBlock* block=stmtAttr->get_bb();
  int pos=stmtAttr->get_pos();
  switch (pos) {
  case POS_PRE:
    return dfi_get_pre_info(store, block->id);
  case POS_POST:
    return dfi_get_post_info_all(store, block->id);
  }
  return NULL;
}

carrier_type_o(CARRIER_TYPE) 
get_statement_pre_info(DFI_STORE store, SgStatement *stmt)
{
  return select_info(store, stmt, "PAG statement start");
}

carrier_type_o(CARRIER_TYPE) 
get_statement_post_info(DFI_STORE store, SgStatement *stmt)
{
  return select_info(store, stmt, "PAG statement end");
}

DFI_STORE perform_pag_analysis(ANALYSIS)(SgProject *root,char* output, bool noresult)
{
    std::cout << "collecting functions: ";
    ProcTraversal s;
    s.traverseInputFiles(root, preorder);
    std::cout << "... done" << std::endl;

    std::cout << "generating cfg ... ";
    CFGTraversal t(s.get_procedures());
    t.traverseInputFiles(root, preorder);
    std::cout << "done" << std::endl;

    std::cout << "testing cfg ... " << std::endl;
    int test_result = kfg_testit(t.getCFG(), 0, "cfg_dump.gdl");
    if (test_result == 0)
    {
        std::cout << std::endl
            << "Warning: There are problems in the CFG."
            << std::endl
            << "Do not rely on the analysis results." << std::endl;
    }
    else
        std::cout << "no problems found" << std::endl;
    
    std::cout << "performing analysis " stringize(ANALYSIS) " ... ";
    std::cout.flush();
    DFI_STORE results = doit(ANALYSIS)(t.getCFG());
    std::cout << "done" << std::endl;
    if(!noresult) {
      std::cout << "generating visualization " << output << " ... ";
      gdl_create(output, 0);
      std::cout << "done" << std::endl;
    } else {
      std::cout << "no visualization generated." << std::endl;
    }
    return results;
}
