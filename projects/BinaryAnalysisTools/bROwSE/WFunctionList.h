#ifndef bROwSE_WFunctionList_H
#define bROwSE_WFunctionList_H

#include <bROwSE/bROwSE.h>
#include <Wt/WTableView>

namespace bROwSE {

class FunctionListModel;

// Presents a list of functions
class WFunctionList: public Wt::WContainerWidget {
    FunctionListModel *model_;
    Wt::WTableView *tableView_;
    Wt::Signal<P2::Function::Ptr> clicked_;
    Wt::Signal<P2::Function::Ptr> doubleClicked_;
public:
    WFunctionList(FunctionListModel *model, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), model_(model) {
        ASSERT_not_null(model);
        init();
    }

    // Emitted when a row of the table is clicked
    Wt::Signal<P2::Function::Ptr>& clicked();

    // Emitted when a row of the table is double clicked
    Wt::Signal<P2::Function::Ptr>& doubleClicked();

private:
    void init();

    void clickRow(const Wt::WModelIndex &idx);

    void doubleClickRow(const Wt::WModelIndex &idx);
};

} // namespace
#endif
