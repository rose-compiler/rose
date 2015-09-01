#include <rose.h>
#include <bROwSE/WCrossReferences.h>
#include <Wt/WAbstractTableModel>
#include <Wt/WPanel>
#include <Wt/WTableView>
#include <Wt/WText>

namespace bROwSE {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                      Model
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CrossReferencesModel: public Wt::WAbstractTableModel {
public:
    enum ColumnNumber {
        AddressColumn,
        TypeColumn,
        NameColumn,
        NColumns
    };

    std::vector<P2::Reference> rows_;

    virtual int rowCount(const Wt::WModelIndex &parent=Wt::WModelIndex()) const ROSE_OVERRIDE {
        return parent.isValid() ? 0 : rows_.size();
    }

    virtual int columnCount(const Wt::WModelIndex &parent=Wt::WModelIndex()) const ROSE_OVERRIDE {
        return parent.isValid() ? 0 : NColumns;
    }

    virtual boost::any headerData(int column, Wt::Orientation orientation=Wt::Horizontal,
                                  int role=Wt::DisplayRole) const ROSE_OVERRIDE {
        if (Wt::Horizontal == orientation) {
            if (Wt::DisplayRole == role) {
                switch (column) {
                    case AddressColumn:
                        return Wt::WString("Address");
                    case TypeColumn:
                        return Wt::WString("Type");
                    case NameColumn:
                        return Wt::WString("Name");
                }
            }
        }
        return boost::any();
    }

    virtual boost::any data(const Wt::WModelIndex &index, int role=Wt::DisplayRole) const ROSE_OVERRIDE {
        ASSERT_require(index.isValid());
        ASSERT_require(index.row()>=0 && (size_t)index.row()<rows_.size());
        if (Wt::DisplayRole == role) {
            const P2::Reference &ref = rows_[index.row()];
            switch (index.column()) {
                case AddressColumn:
                    return Wt::WString(StringUtility::addrToString(ref.address()));
                case TypeColumn:
                    switch (ref.granularity()) {
                        case P2::Reference::EMPTY:
                            return Wt::WString("empty");
                        case P2::Reference::ADDRESS:
                            return Wt::WString("addr");
                        case P2::Reference::INSTRUCTION:
                            return Wt::WString("insn");
                        case P2::Reference::BASIC_BLOCK:
                            return Wt::WString("blk");
                        case P2::Reference::FUNCTION:
                            return Wt::WString("func");
                    }
                    break;
                case NameColumn:
                    if (P2::Function::Ptr function = ref.function()) {
                        if (!function->name().empty())
                            return Wt::WString(StringUtility::cEscape(function->name()));
                        return Wt::WString("f_" + StringUtility::addrToString(function->address()));
                    }
                    break;
            }
        }
        return boost::any();
    }

    void setReferences(const P2::ReferenceSet &refs) {
        layoutAboutToBeChanged().emit();
        rows_.clear();
        rows_.reserve(refs.size());
        BOOST_FOREACH (const P2::Reference &ref, refs)
            rows_.push_back(ref);
        layoutChanged().emit();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                              View
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
WCrossReferences::init() {
    model_ = new CrossReferencesModel;

    Wt::WContainerWidget *wPanelCenter = new Wt::WContainerWidget;

    // Info about what's being display
    wTableName_ = new Wt::WText(wPanelCenter);

    // Table showing the cross references
    view_ = new Wt::WTableView(wPanelCenter);
    view_->setModel(model_);
    view_->setRowHeaderCount(1);                        // must be first
    view_->setHeaderHeight(28);
    view_->setSortingEnabled(false);
    view_->setAlternatingRowColors(true);
    view_->setColumnResizeEnabled(true);
    view_->setEditTriggers(Wt::WAbstractItemView::NoEditTrigger);
    view_->setColumnWidth(CrossReferencesModel::AddressColumn, Wt::WLength(5, Wt::WLength::FontEm));
    view_->setColumnWidth(CrossReferencesModel::TypeColumn, Wt::WLength(3, Wt::WLength::FontEm));
    view_->setColumnWidth(CrossReferencesModel::NameColumn, Wt::WLength(25, Wt::WLength::FontEm));
    view_->clicked().connect(boost::bind(&WCrossReferences::handleRowClicked, this, _1));

    // A frame to hold all the cross ref info
    Wt::WPanel *wPanel = new Wt::WPanel(this);
    wPanel->setTitle("Cross References");
    wPanel->resize(300, Wt::WLength::Auto);
    wPanel->setCentralWidget(wPanelCenter);
}

void
WCrossReferences::name(const std::string &s) {
    wTableName_->setText(s);
}

void
WCrossReferences::refs(const P2::ReferenceSet &rs) {
    model_->setReferences(rs);
}

void
WCrossReferences::handleRowClicked(const Wt::WModelIndex &idx) {
    if (idx.isValid()) {
        ASSERT_require(idx.row()>=0 && (size_t)idx.row() < model_->rows_.size());
        referenceClicked_.emit(model_->rows_[idx.row()]);
    }
}

} // namespace
