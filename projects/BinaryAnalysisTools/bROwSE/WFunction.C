#include <bROwSE/WFunction.h>

#include <bROwSE/FunctionListModel.h>
#include <bROwSE/WAssemblyListing.h>
#include <bROwSE/WFunctionCfg.h>
#include <bROwSE/WFunctionList.h>
#include <bROwSE/WFunctionSummary.h>
#include <Wt/WScrollArea>
#include <Wt/WTabWidget>
#include <Wt/WVBoxLayout>

#if 1 // DEBUGGING [Robb P. Matzke 2014-12-14]
#include <Wt/WTable>
#endif

namespace bROwSE {

void
WFunction::init() {
    Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
    setLayout(vbox);

    wTabs_ = new Wt::WTabWidget;
    vbox->addWidget(wTabs_, 1 /*stretch*/);
    wTabs_->currentChanged().connect(this, &WFunction::setCurrentTab);

    // List of all functions
    ASSERT_require(FLIST_TAB==0);
    wFunctionList_ = new WFunctionList(new FunctionListModel(ctx_));
    wFunctionList_->hide();
    wFunctionList_->clicked().connect(this, &WFunction::showFunctionSummary);
    wTabs_->addTab(wFunctionList_, "Functions");
    
    // Function summary information
    ASSERT_require(SUMMARY_TAB==1);
    wFunctionSummary_ = new WFunctionSummary(ctx_);
    wFunctionSummary_->hide();
    wTabs_->addTab(wFunctionSummary_, "Summary");

    // Function control flow graph.
    //
    // When using the BootStrap theme, the img.maxwidth=100% needs to be commented out otherwise no horizontal scroll bars will
    // be added.  Edit resources/themes/bootstrap/2/bootstrap.css.
    //
    // FIXME[Robb P. Matzke 2014-12-14]: The scroll area should be part of WFunctionCfg instead.
    ASSERT_require(CFG_TAB==2);
    Wt::WTable *wTable = new Wt::WTable();
    wTable->hide();
    wTable->setHeaderCount(0);
    wFunctionCfg_ = new WFunctionCfg(ctx_);
    wFunctionCfg_->functionClicked().connect(this, &WFunction::showFunctionCfg);
    wTable->elementAt(0, 0)->addWidget(wFunctionCfg_);
    wTabs_->addTab(wTable, "CFG");

    // Assembly listing
    ASSERT_require(ASSEMBLY_TAB==3);
    wAssemblyListing_ = new WAssemblyListing(ctx_);
    wAssemblyListing_->hide();
    wTabs_->addTab(wAssemblyListing_, "Assembly");

    setCurrentTab(FLIST_TAB);
}

void
WFunction::setCurrentTab(int idx) {
    switch (idx) {
        case FLIST_TAB:
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

} // namespace
