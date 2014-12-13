#ifndef bROwSE_WFunctions_H
#define bROwSE_WFunctions_H

#include <bROwSE/bROwSE.h>
#include <bROwSE/WBasicBlock.h>
#include <bROwSE/WFunctionCfg.h>
#include <bROwSE/WFunctionList.h>
#include <bROwSE/WFunctionSummary.h>
#include <Wt/WContainerWidget>

namespace bROwSE {

// All information about functions, CFGs, basic blocks, instructions...
class WFunctions: public Wt::WContainerWidget {
public:
    enum Tab { LIST_TAB, CFG_TAB, BB_TAB };
private:
    Context ctx_;
    Wt::WTabWidget *wTabs_;                             // function list; function details
    Wt::WContainerWidget *wListTab_, *wCfgTab_, *wBbTab_; // tabs in the tab widget
    WFunctionList *wFunctionList_;                      // list of all functions
    WFunctionSummary *wSummary1_, *wSummary2_;          // function summary info (in two different tabs)
    WFunctionCfg *wFunctionCfg_;                        // control flow graph
    WBasicBlock *wBasicBlock_;                          // basic block display
    P2::Function::Ptr function_;                        // current function
    P2::BasicBlock::Ptr bblock_;                        // current basic block
public:
    WFunctions(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), ctx_(ctx), wTabs_(NULL), wListTab_(NULL), wCfgTab_(NULL), wBbTab_(NULL),
          wFunctionList_(NULL), wSummary1_(NULL), wSummary2_(NULL), wFunctionCfg_(NULL), wBasicBlock_(NULL) {
        init();
    }

    void setCurrentTab(int idx);
    
private:
    void init();

    // Show the function list and summary of selected function
    void showFunctionSummary(const P2::Function::Ptr &function);

    // display function details
    void showFunctionCfg(const P2::Function::Ptr &function);

    // display basic block details
    void showBasicBlock(const P2::BasicBlock::Ptr &bblock);
};

} // namespace
#endif
