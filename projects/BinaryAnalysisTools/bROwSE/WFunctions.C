#include <bROwSE/WFunctions.h>

#include <bROwSE/FunctionListModel.h>
#include <Wt/WScrollArea>
#include <Wt/WTabWidget>
#include <Wt/WVBoxLayout>

namespace bROwSE {

void
WFunctions::init() {
    Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
    setLayout(vbox);

    vbox->addWidget(wTabs_ = new Wt::WTabWidget());
    wTabs_->currentChanged().connect(this, &WFunctions::setCurrentTab);

    // Function list tab.
    ASSERT_require(LIST_TAB==0);
    wListTab_ = new Wt::WContainerWidget(this);
    wListTab_->hide();                              // working around bug with all tabs' widges visible on creation
    Wt::WVBoxLayout *wListTabLayout = new Wt::WVBoxLayout;
    wListTab_->setLayout(wListTabLayout);
    wListTabLayout->addWidget(wFunctionList_ = new WFunctionList(new FunctionListModel(ctx_)), 1);
    wFunctionList_->clicked().connect(this, &WFunctions::showFunctionSummary);
    wFunctionList_->doubleClicked().connect(this, &WFunctions::showFunctionCfg);
    wListTabLayout->addWidget(wSummary1_ = new WFunctionSummary(ctx_));
    wTabs_->addTab(wListTab_, "Functions");

    // CFG tab
    ASSERT_require(CFG_TAB==1);
    wCfgTab_ = new Wt::WContainerWidget(this);
    wCfgTab_->hide();
    Wt::WVBoxLayout *wCfgTabLayout = new Wt::WVBoxLayout;
    wCfgTab_->setLayout(wCfgTabLayout);
    wCfgTabLayout->addWidget(new Wt::WBreak);
    wCfgTabLayout->addWidget(wSummary2_ = new WFunctionSummary(ctx_));
    Wt::WScrollArea *sa = new Wt::WScrollArea;
    wCfgTabLayout->addWidget(sa, 1);
    sa->setWidget(wFunctionCfg_ = new WFunctionCfg(ctx_));
    wFunctionCfg_->functionClicked().connect(this, &WFunctions::showFunctionCfg);
    wFunctionCfg_->basicBlockClicked().connect(this, &WFunctions::showBasicBlock);
    wTabs_->addTab(wCfgTab_, "CFG");

    // Basic block tab
    ASSERT_require(BB_TAB==2);
    wBbTab_ = new Wt::WContainerWidget(this);
    wBbTab_->hide();
    Wt::WVBoxLayout *wBbTabLayout = new Wt::WVBoxLayout;
    wBbTab_->setLayout(wBbTabLayout);
    wBbTabLayout->addWidget(wBasicBlock_ = new WBasicBlock(ctx_));
    wBbTabLayout->addWidget(new Wt::WText, 1);
    wTabs_->addTab(wBbTab_, "Block");

    setCurrentTab(LIST_TAB);
}

void
WFunctions::setCurrentTab(int idx) {
    switch (idx) {
        case LIST_TAB:
            wSummary1_->changeFunction(function_);
            wSummary1_->setHidden(function_==NULL);
            break;
        case CFG_TAB:
            wSummary2_->changeFunction(function_);
            wFunctionCfg_->changeFunction(function_);
            wFunctionCfg_->setHidden(function_==NULL);
            break;
        case BB_TAB:
            wBasicBlock_->changeBasicBlock(bblock_);
            break;
    }

    // When changing away from the CFG_TAB and our current function is something other than what that tab is displaying,
    // hide the CFG image so it doesn't flash an old one when we change back to the tab. The flashing would happen because
    // we don't generate and update the image, which is expensive, until the last possible moment.
    if (idx!=CFG_TAB)
        wFunctionCfg_->setHidden(function_ != wFunctionCfg_->function());
    wTabs_->setCurrentIndex(idx);
    wTabs_->widget(idx)->show();
}

void
WFunctions::showFunctionSummary(const P2::Function::Ptr &function) {
    function_ = function;
    bblock_ = P2::BasicBlock::Ptr();
    setCurrentTab(LIST_TAB);
}

void
WFunctions::showFunctionCfg(const P2::Function::Ptr &function) {
    function_ = function;
    bblock_ = P2::BasicBlock::Ptr();
    setCurrentTab(CFG_TAB);
}

void
WFunctions::showBasicBlock(const P2::BasicBlock::Ptr &bblock) {
    bblock_ = bblock;
    setCurrentTab(BB_TAB);
}

} // namespace
