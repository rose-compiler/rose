#include <bROwSE/WMagic.h>

#include <BinaryMagic.h>                                // ROSE
#include <bROwSE/FunctionUtil.h>
#include <Wt/WAbstractTableModel>
#include <Wt/WHBoxLayout>
#include <Wt/WTableView>
#include <Wt/WText>
#include <Wt/WVBoxLayout>

namespace bROwSE {

enum ColumnNumber {
    AddressColumn,
    FirstByteColumn,
    LastByteColumn = FirstByteColumn + 7,
    MagicStringColumn,
    NColumns
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Magic number model
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MagicModel: public Wt::WAbstractTableModel {
    rose::BinaryAnalysis::MagicNumber analyzer_;
    MemoryMap memoryMap_;
public:
    const MemoryMap& memoryMap() const {
        return memoryMap_;
    }

    void memoryMap(const MemoryMap &map) {
        layoutAboutToBeChanged().emit();
        memoryMap_ = map;
        layoutChanged().emit();
    }

    rose_addr_t addressForRow(size_t row) const {
        BOOST_FOREACH (const AddressInterval &interval, memoryMap_.intervals()) {
            if (row < interval.size())
                return interval.least() + row;
            row -= interval.size();
        }
        ASSERT_not_reachable("view asked for an invalid model row");
    }

    virtual int rowCount(const Wt::WModelIndex &parent) const ROSE_OVERRIDE {
        return parent.isValid() ? 0 : memoryMap_.size();
    }

    virtual int columnCount(const Wt::WModelIndex &parent) const ROSE_OVERRIDE {
        return parent.isValid() ? 0 : NColumns;
    }

    virtual boost::any headerData(int column, Wt::Orientation orientation, int role) const ROSE_OVERRIDE {
        if (Wt::Horizontal == orientation && Wt::DisplayRole == role) {
            if (AddressColumn == column) {
                return Wt::WString("Address");
            } else if (column>=FirstByteColumn && column<=LastByteColumn) {
                char buf[16];
                sprintf(buf, "+%x", (unsigned)(column - FirstByteColumn));
                return Wt::WString(buf);
            } else if (MagicStringColumn == column) {
                return Wt::WString("Identified");
            }
        }
        return boost::any();
    }

    virtual boost::any data(const Wt::WModelIndex &index, int role) const ROSE_OVERRIDE {
        if (Wt::DisplayRole == role) {
            ASSERT_require(index.isValid());
            rose_addr_t va = addressForRow(index.row());
            if (AddressColumn == index.column()) {
                return Wt::WString(StringUtility::addrToString(va));
            } else if (index.column()>=FirstByteColumn && index.column()<=LastByteColumn) {
                uint8_t ch;
                if (memoryMap_.at(va + index.column()-FirstByteColumn).limit(1).read(&ch)) {
                    std::string s = charToString(ch);
                    if (s.empty()) {
                        char buf[8];
                        sprintf(buf, "%02x", (unsigned)ch);
                        s = buf;
                    }
                    return Wt::WString(s);
                }
            } else if (MagicStringColumn == index.column()) {
                std::string magic;
                try {
                    magic = analyzer_.identify(memoryMap_, va);
                } catch (const std::runtime_error &e) {
                    magic = std::string("error: ") + e.what();
                }
                return Wt::WString(magic);
            }
        }
        return boost::any();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Magic numbers widget
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
WMagic::init() {
    Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
    setLayout(vbox);

    vbox->addWidget(new Wt::WText("This table is a list of a magic number at every address in the specimen memory map. "
                                  "Identifying magic numbers using magic(5) databases is a slow operation, thus this "
                                  "table's columns are not searchable or sortable. Making them searchable or sortable would "
                                  "require that all rows be calculated in advance.  The ROSE magicScanner tool is more "
                                  "efficient for this purpose."));
    
    // Table. The HBoxLayout is so the table scrolls horizontally.
    Wt::WContainerWidget *wTableContainer = new Wt::WContainerWidget;
    vbox->addWidget(wTableContainer, 1 /*stretch*/);
    Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout;
    wTableContainer->setLayout(hbox);

    model_ = new MagicModel;
    wTableView_ = new Wt::WTableView;
    wTableView_->setModel(model_);
    wTableView_->setRowHeaderCount(1);                  // this must be first
    wTableView_->setHeaderHeight(28);
    wTableView_->setSortingEnabled(false);              // it would be way to slow to scan all bytes
    wTableView_->setAlternatingRowColors(true);
    wTableView_->setColumnResizeEnabled(true);
    wTableView_->setSelectionBehavior(Wt::SelectRows);
    wTableView_->setSelectionMode(Wt::SingleSelection);
    wTableView_->setEditTriggers(Wt::WAbstractItemView::NoEditTrigger);
    wTableView_->setColumnWidth(AddressColumn, Wt::WLength(6, Wt::WLength::FontEm));
    for (size_t i=FirstByteColumn; i<=LastByteColumn; ++i)
        wTableView_->setColumnWidth(i, Wt::WLength(2, Wt::WLength::FontEm));
    wTableView_->setColumnWidth(MagicStringColumn, Wt::WLength(100, Wt::WLength::FontEm));
    hbox->addWidget(wTableView_);
}

const MemoryMap&
WMagic::memoryMap() const {
    return model_->memoryMap();
}

void
WMagic::memoryMap(const MemoryMap &map) {
    model_->memoryMap(map);
}

} // namespace
