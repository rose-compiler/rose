#include <bROwSE/WFunction.h>

#include <bROwSE/FunctionListModel.h>
#include <bROwSE/WAssemblyListing.h>
#include <bROwSE/WFunctionCfg.h>
#include <bROwSE/WFunctionList.h>
#include <bROwSE/WFunctionSummary.h>
#include <bROwSE/WHexDump.h>
#include <bROwSE/WMemoryMap.h>
#include <Wt/WTabWidget>
#include <Wt/WVBoxLayout>

#if 1 // DEBUGGING [Robb P. Matzke 2014-12-14]
#include <Wt/WBreak>
#endif

namespace bROwSE {

void
WFunction::init() {
    Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
    setLayout(vbox);

    wTabs_ = new Wt::WTabWidget;
    vbox->addWidget(wTabs_, 1 /*stretch*/);
    wTabs_->currentChanged().connect(this, &WFunction::setCurrentTab);

    // Memory map
    ASSERT_require(MEMORY_TAB==0);
    wMemoryMap_ = new WMemoryMap(ctx_.partitioner.memoryMap());
    wMemoryMap_->hide();
    wTabs_->addTab(wMemoryMap_, "Memory");

    // List of all functions
    ASSERT_require(FLIST_TAB==1);
    wFunctionList_ = new WFunctionList(ctx_);
    wFunctionList_->hide();
    wFunctionList_->functionChanged().connect(this, &WFunction::setCurrentFunction);
    wFunctionList_->functionRowDoubleClicked().connect(this, &WFunction::showFunctionSummary);
    wTabs_->addTab(wFunctionList_, "Functions");
    
    // Function summary information
    ASSERT_require(SUMMARY_TAB==2);
    wFunctionSummary_ = new WFunctionSummary(ctx_);
    wFunctionSummary_->hide();
    wTabs_->addTab(wFunctionSummary_, "Summary");

    // Function control flow graph.
    //
    // When using the BootStrap theme, the img.maxwidth=100% needs to be commented out otherwise no horizontal scroll bars will
    // be added.  Edit resources/themes/bootstrap/2/bootstrap.css.
    ASSERT_require(CFG_TAB==3);
    wFunctionCfg_ = new WFunctionCfg(ctx_);
    wFunctionCfg_->hide();
    wFunctionCfg_->functionChanged().connect(this, &WFunction::setCurrentFunction);
    wFunctionCfg_->functionClicked().connect(this, &WFunction::setCurrentFunction);
    wFunctionCfg_->addressClicked().connect(this, &WFunction::showHexDumpAtAddress);
    wTabs_->addTab(wFunctionCfg_, "CFG");

    // Assembly listing
    ASSERT_require(ASSEMBLY_TAB==4);
    wAssemblyListing_ = new WAssemblyListing(ctx_);
    wAssemblyListing_->hide();
    wTabs_->addTab(wAssemblyListing_, "Assembly");

    // Data listing
    ASSERT_require(HEXDUMP_TAB==5);
    wHexDump_ = new WHexDump(ctx_);
    wHexDump_->hide();
    wTabs_->addTab(wHexDump_, "Hexdump");

    setCurrentTab(MEMORY_TAB);
}

// Make function the current function.  This doesn't update any tabs until we actually switch to them with
// setCurrentTab.  It's usually called in response to selecting a new function in the FLIST_TAB.
void
WFunction::setCurrentFunction(const P2::Function::Ptr &function) {
    ASSERT_not_null(function);
    if (function_ != function) {
        function_ = function;
        setCurrentTab(wTabs_->currentIndex());          // don't change tabs, but do the other stuff
    }
}

void
WFunction::setCurrentTab(int idx) {
    switch (idx) {
        case MEMORY_TAB:
            wMemoryMap_->setHidden(false);
            break;
        case FLIST_TAB:
            wFunctionList_->changeFunction(function_);
            wFunctionList_->setHidden(false);
            break;
        case SUMMARY_TAB:
            wFunctionSummary_->changeFunction(function_);
            wFunctionSummary_->setHidden(function_==NULL);
            break;
        case CFG_TAB:
            wFunctionCfg_->changeFunction(function_);
            wFunctionCfg_->setHidden(function_==NULL);
            break;
        case ASSEMBLY_TAB:
            wAssemblyListing_->changeFunction(function_);
            wAssemblyListing_->setHidden(function_==NULL);
            break;
        case HEXDUMP_TAB:
            wHexDump_->setHidden(false);
            break;
        default:
            ASSERT_not_reachable("tab is not handled");
    }

    // When changing away from the CFG_TAB and our current function is something other than what that tab is displaying, hide
    // the CFG image so it doesn't flash an old one when we change back to the tab.  The flashing would happen because we don't
    // generate and update the image, which is expensive, until the last possible moment.
    if (idx != CFG_TAB)
        wFunctionCfg_->setHidden(function_ != wFunctionCfg_->function());
    wTabs_->setCurrentIndex(idx);
    wTabs_->widget(idx)->show();
}

void
WFunction::showFunctionList() {
    setCurrentTab(FLIST_TAB);
}

void
WFunction::showFunctionSummary(const P2::Function::Ptr &function) {
    function_ = function;
    setCurrentTab(SUMMARY_TAB);
}

void
WFunction::showFunctionCfg(const P2::Function::Ptr &function) {
    function_ = function;
    setCurrentTab(CFG_TAB);
}

void
WFunction::showAssemblyListing(const P2::Function::Ptr &function) {
    function_ = function;
    setCurrentTab(ASSEMBLY_TAB);
}

void
WFunction::showHexDump() {
    setCurrentTab(HEXDUMP_TAB);
}

void
WFunction::showHexDumpAtAddress(rose_addr_t va) {
    wHexDump_->makeVisible(va);
    setCurrentTab(HEXDUMP_TAB);
}

} // namespace
