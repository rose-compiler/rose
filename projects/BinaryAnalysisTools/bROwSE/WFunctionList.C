#include <bROwSE/WFunctionList.h>

#include <bROwSE/FunctionListModel.h>
#include <Wt/WHBoxLayout>

namespace bROwSE {

void
WFunctionList::init() {
    tableView_ = new Wt::WTableView;
    tableView_->setModel(model_);
    tableView_->setRowHeaderCount(1); // this must be first property set
    tableView_->setHeaderHeight(28);
    tableView_->setSortingEnabled(true);
    tableView_->setAlternatingRowColors(true);
    tableView_->setColumnWidth(FunctionListModel::C_ENTRY,          Wt::WLength( 6, Wt::WLength::FontEm));
    tableView_->setColumnWidth(FunctionListModel::C_NAME,           Wt::WLength(30, Wt::WLength::FontEm));
    tableView_->setColumnWidth(FunctionListModel::C_SIZE,           Wt::WLength( 5, Wt::WLength::FontEm));
    tableView_->setColumnWidth(FunctionListModel::C_NCALLERS,       Wt::WLength( 5, Wt::WLength::FontEm));
    tableView_->setColumnWidth(FunctionListModel::C_NRETURNS,       Wt::WLength( 5, Wt::WLength::FontEm));
    tableView_->setColumnWidth(FunctionListModel::C_IMPORT,         Wt::WLength( 5, Wt::WLength::FontEm));
    tableView_->setColumnWidth(FunctionListModel::C_EXPORT,         Wt::WLength( 5, Wt::WLength::FontEm));
    tableView_->setColumnResizeEnabled(true);
    tableView_->setSelectionMode(Wt::SingleSelection);
    tableView_->setEditTriggers(Wt::WAbstractItemView::NoEditTrigger);
    tableView_->clicked().connect(this, &WFunctionList::clickRow);
    tableView_->doubleClicked().connect(this, &WFunctionList::doubleClickRow);

    Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout;
    setLayout(hbox);
    hbox->addWidget(tableView_);
}

Wt::Signal<P2::Function::Ptr>&
WFunctionList::clicked() {
    return clicked_;
}

Wt::Signal<P2::Function::Ptr>&
WFunctionList::doubleClicked() {
    return doubleClicked_;
}
    
void
WFunctionList::clickRow(const Wt::WModelIndex &idx) {
    if (idx.isValid()) {
        P2::Function::Ptr function = model_->functionAt(idx.row());
        clicked_.emit(function);
    }
}

void
WFunctionList::doubleClickRow(const Wt::WModelIndex &idx) {
    if (idx.isValid()) {
        P2::Function::Ptr function = model_->functionAt(idx.row());
        doubleClicked_.emit(function);
    }
}

} // namespace
