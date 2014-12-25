#ifndef bROwSE_WFunction_H
#define bROwSE_WFunction_H

#include <bROwSE/bROwSE.h>

#include <Wt/WContainerWidget>

namespace bROwSE {

class WAssemblyListing;
class WFunctionCfg;
class WFunctionList;
class WFunctionSummary;
class WHexDump;
class WMemoryMap;

class WFunction: public Wt::WContainerWidget {
public:
    enum Tab { MEMORY_TAB, FLIST_TAB, SUMMARY_TAB, CFG_TAB, ASSEMBLY_TAB, HEXDUMP_TAB };

private:
    Context ctx_;
    Wt::WTabWidget *wTabs_;
    Wt::WContainerWidget *wCfgTab_, wSummaryTab_;
    WFunctionList *wFunctionList_;
    WFunctionCfg *wFunctionCfg_;
    WFunctionSummary *wFunctionSummary_;
    WAssemblyListing *wAssemblyListing_;
    WHexDump *wHexDump_;
    WMemoryMap *wMemoryMap_;
    P2::Function::Ptr function_;                        // current function

public:
    WFunction(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), ctx_(ctx), wTabs_(NULL), wCfgTab_(NULL), wSummaryTab_(NULL), wFunctionCfg_(NULL),
          wFunctionSummary_(NULL), wAssemblyListing_(NULL), wHexDump_(NULL), wMemoryMap_(NULL) {
        init();
    }

    void setCurrentFunction(const P2::Function::Ptr&);
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

    // Show the hexdump pane
    void showHexDump();
    void showHexDumpAtAddress(rose_addr_t);
};


} // namespace
#endif
