#ifndef bROwSE_WFunction_H
#define bROwSE_WFunction_H

#include <bROwSE/bROwSE.h>

#include <Wt/WContainerWidget>

namespace bROwSE {

class WFunctionCfg;
class WFunctionList;
class WFunctionSummary;
class WAssemblyListing;

class WFunction: public Wt::WContainerWidget {
public:
    enum Tab { FLIST_TAB, SUMMARY_TAB, CFG_TAB, ASSEMBLY_TAB };

private:
    Context ctx_;
    Wt::WTabWidget *wTabs_;
    Wt::WContainerWidget *wCfgTab_, wSummaryTab_;
    WFunctionList *wFunctionList_;
    WFunctionCfg *wFunctionCfg_;
    WFunctionSummary *wFunctionSummary_;
    WAssemblyListing *wAssemblyListing_;
    P2::Function::Ptr function_;                        // current function

public:
    WFunction(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), ctx_(ctx), wTabs_(NULL), wCfgTab_(NULL), wSummaryTab_(NULL), wFunctionCfg_(NULL),
          wFunctionSummary_(NULL), wAssemblyListing_(NULL) {
        init();
    }

    void setCurrentTab(int idx);

private:
    void init();

    // Show list of all functions
    void showFunctionList();

    // Show summary information for one function
    void showFunctionSummary(const P2::Function::Ptr &function);

    // Show CFG for one function
    void showFunctionCfg(const P2::Function::Ptr &function);

    // Show assembly listing for one function
    void showAssemblyListing(const P2::Function::Ptr &function);
};


} // namespace
#endif
