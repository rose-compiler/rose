/* -*- c -*-
 * vim: syntax=cpp
 * Specify the carrier type of the analysis. This should be the same
 * as the one given with "carrier:" in the optla file.
 * For example: #define CARRIER_TYPE State */

#ifndef CARRIER_TYPE
    #define CARRIER_TYPE PLEASE_SPECIFY_CARRIER_TYPE
#endif

#include "main-support.h"
#include "cfg_support.h"
#include "analysis_info.h"

extern "C" DFI_STORE doit(ANALYSIS)(void *);
extern "C" void gdl_create(char *, FLO_BOOL);

#if 0
std::string get_statement_pre_info_string(DFI_STORE store, SgStatement* stmt) {
  carrier_type_o(CARRIER_TYPE) info
      = get_statement_pre_info(store, stmt);
  if (info != NULL)
      return (carrier_printfunc(CARRIER_TYPE)(info));
  else
      return "<undefined dfi>";
}

std::string get_statement_post_info_string(DFI_STORE store, SgStatement* stmt) {
  carrier_type_o(CARRIER_TYPE) info
      = get_statement_post_info(store, stmt);
  if (info != NULL)
      return (carrier_printfunc(CARRIER_TYPE)(info));
  else
      return "<undefined dfi>";
}

carrier_type_o(CARRIER_TYPE) 
select_info(DFI_STORE store, SgStatement *stmt, std::string attrName) {
  if (stmt->attributeExists(attrName)) {
    StatementAttribute* stmtAttr
        = dynamic_cast<StatementAttribute *>(stmt->getAttribute(attrName));
    BasicBlock* block=stmtAttr->get_bb();
    int pos=stmtAttr->get_pos();
    switch (pos) {
    case POS_PRE:
      return dfi_get_pre_info(store, block->id);
    case POS_POST:
      return dfi_get_post_info_all(store, block->id);
    }
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
#endif

DFI_STORE perform_pag_analysis(ANALYSIS)(SATIrE::Program *program,AnalyzerOptions* opt)
{
    TimingPerformance timer("PAG analysis and visualization part:");
    TimingPerformance *nestedTimer;

    bool verbose=opt->verbose();
  
    if (verbose) std::cout << "collecting functions ... " << std::flush;
    nestedTimer = new TimingPerformance("SATIrE ICFG construction:");
    ProcTraversal s(program);
    s.setPrintCollectedFunctionNames(opt->printCollectedFunctionNames());
    SgProject *root = program->astRoot;
    s.traverse(root, preorder);
    if (verbose) std::cout << "done" << std::endl;

    if (verbose) std::cout << "generating cfg ... " << std::flush;
    CFGTraversal t(s, program, opt);
    if (!opt->numberExpressions()) {
      t.numberExpressions(false);
    }
    t.traverse(root, preorder);
    delete nestedTimer;
    if (verbose) std::cout << "done" << std::endl;

    if (opt->checkIcfg()) {
      if (verbose) std::cout << "testing cfg ... " << std::flush;
      nestedTimer = new TimingPerformance("CFG consistency check:");
      int test_result = kfg_testit(t.getCFG(), 1 /* quiet */);
      delete nestedTimer;
      if (verbose) {
        if (test_result == 0) {
          std::cout << "failed." << std::endl;
          std::cerr << std::endl
            << "Warning: PAG reports: there are problems in the ICFG."
            << std::endl;
        } else {
          std::cout << "done" << std::endl;
        }
      }
    }
    
    if(opt->outputIcfg()) {
      TimingPerformance timer("ICFG visualization:");
      if(verbose) std::cout << "generating icfg visualization " << opt->getOutputIcfgFileName() << " ... " << std::flush;
      outputIcfg(t.getCFG(),const_cast<char*>(opt->getOutputIcfgFileName().c_str()));
      if (verbose) std::cout << "done" << std::endl;
    }

 // Run the analyses this one depends on.
 // runOtherAnalyses(t.getCFG());

    if (verbose) std::cout << "performing analysis " stringize(ANALYSIS) " ... " << std::flush;
    std::cout.flush();
    nestedTimer = new TimingPerformance("Actual data-flow analysis " stringize(ANALYSIS) ":");
    DFI_STORE results = doit(ANALYSIS)(t.getCFG());
    delete nestedTimer;
    if (verbose) std::cout << "done" << std::endl;
    
    /* we must run gdl_create for both single file and animation */
    std::string outputgdlfile;
    if(opt->outputGdl())
      outputgdlfile=opt->getOutputGdlFileName();
    else
      if(opt->outputGdlAnim())
        outputgdlfile=opt->getOutputGdlAnimDirName() + "/result.gdl"; // we cannot create an animation, but not create a gdl file

    if(opt->outputGdl() || opt->outputGdlAnim()) {
      TimingPerformance timer("Generate GDL visualization of analysis results:");
      if(verbose) std::cout << "generating ";
      if(verbose && opt->outputGdl()) std::cout << "visualization " << outputgdlfile << " ";
      if(verbose && opt->outputGdl() && opt->outputGdlAnim()) std::cout << "and ";
      if(verbose && opt->outputGdlAnim()) std::cout << "animation in directory " << opt->getOutputGdlAnimDirName() << " ";
      if(verbose) std::cout << "... " << std::flush;
      gdl_create(const_cast<char*>(outputgdlfile.c_str()), 0);
      if (verbose) std::cout << "done" << std::endl;
    }

    return results;
}

// GB (2008-03-05): Simple top-down traversal to set parent pointers correctly.
class ParentPointerResetter: public AstTopDownProcessing<SgNode *>
{
protected:
    SgNode *evaluateInheritedAttribute(SgNode *node, SgNode *parent)
    {
        node->set_parent(parent);
        return node;
    }
};

void satireResetParentPointers(SgNode *root, SgNode *parent)
{
    TimingPerformance timer("SATIrE fixup of parent pointers in original AST:");
    ParentPointerResetter ppr;
    ppr.traverse(root, parent);
}

// The definition of void runOtherAnalyses(void *cfg) is generated by the
// newanalysis script.
