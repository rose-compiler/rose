#include <rose.h>
#include <bROwSE/WInstructionList.h>

#include <bROwSE/InstructionListModel.h>
#include <Wt/WTableView>

namespace bROwSE {

void
WInstructionList::init() {
    tableView_ = new Wt::WTableView(this);
    tableView_->setModel(model_);
    tableView_->setRowHeaderCount(1);               // this must be first property set
    tableView_->setHeaderHeight(28);
    tableView_->setSortingEnabled(false);
    tableView_->setAlternatingRowColors(true);
    tableView_->setColumnWidth(InstructionListModel::C_ADDR,       Wt::WLength(6, Wt::WLength::FontEm));
    tableView_->setColumnWidth(InstructionListModel::C_BYTES,      Wt::WLength(12, Wt::WLength::FontEm));
    tableView_->setColumnWidth(InstructionListModel::C_CHARS,      Wt::WLength(4, Wt::WLength::FontEm));
    tableView_->setColumnWidth(InstructionListModel::C_STACKDELTA, Wt::WLength(4, Wt::WLength::FontEm));
    tableView_->setColumnWidth(InstructionListModel::C_NAME,       Wt::WLength(5, Wt::WLength::FontEm));
    tableView_->setColumnWidth(InstructionListModel::C_ARGS,       Wt::WLength(30, Wt::WLength::FontEm));
    tableView_->setColumnWidth(InstructionListModel::C_COMMENT,    Wt::WLength(50, Wt::WLength::FontEm));
    tableView_->setColumnResizeEnabled(true);
    tableView_->setSelectionMode(Wt::SingleSelection);
    tableView_->setEditTriggers(Wt::WAbstractItemView::NoEditTrigger);
}

} // namespace
