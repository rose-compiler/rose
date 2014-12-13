#ifndef bROwSE_WInstructionList_H
#define bROwSE_WInstructionList_H

#include <bROwSE/bROwSE.h>
#include <Wt/WContainerWidget>
#include <Wt/WTableView>

namespace bROwSE {

class InstructionListModel;

class WInstructionList: public Wt::WContainerWidget {
    InstructionListModel *model_;
    Wt::WTableView *tableView_;
public:
    WInstructionList(InstructionListModel *model, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), model_(model) {
        init();
    }

private:
    void init();
};

} // namespace
#endif
