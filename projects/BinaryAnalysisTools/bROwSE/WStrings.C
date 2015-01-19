#include <bROwSE/WStrings.h>

#include <BinaryString.h>                               // ROSE
#include <boost/regex.hpp>
#include <bROwSE/WAddressSpace.h>
#include <stringify.h>                                  // ROSE
#include <Wt/WAbstractTableModel>
#include <Wt/WHBoxLayout>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WTableView>
#include <Wt/WText>
#include <Wt/WVBoxLayout>

using namespace rose;
using namespace rose::BinaryAnalysis;

namespace bROwSE {

class StringsModel: public Wt::WAbstractTableModel {
public:
    enum ColumnNumber {
        AddressColumn,
        CrossRefsColumn,
        LengthEncodingColumn,
        CharacterEncodingColumn,
        NCharsColumn,
        ValueColumn,
        NColumns
    };

    struct Row {
        StringFinder::String meta;
        size_t nrefs;
        std::string value;
        bool isMatching;
        Row(const StringFinder::String &meta, size_t nrefs, const std::string &value)
            : meta(meta), nrefs(nrefs), value(value), isMatching(false) {}
    };

    MemoryMap memoryMap_;
    std::vector<Row> rows_;
    P2::CrossReferences xrefs_;                         // from string address to instruction

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
                    case CrossRefsColumn:
                        return Wt::WString("NRefs");
                    case LengthEncodingColumn:
                        return Wt::WString("Encoding");
                    case CharacterEncodingColumn:
                        return Wt::WString("CharType");
                    case NCharsColumn:
                        return Wt::WString("nChars");
                    case ValueColumn:
                        return Wt::WString("Value");
                    default:
                        ASSERT_not_reachable("invalid column number");
                }
            }
        }
        return boost::any();
    }

    virtual boost::any data(const Wt::WModelIndex &index, int role=Wt::DisplayRole) const ROSE_OVERRIDE {
        ASSERT_require(index.isValid());
        ASSERT_require(index.row()>=0 && (size_t)index.row()<rows_.size());
        if (Wt::DisplayRole == role) {
            const Row &row = rows_[index.row()];
            switch (index.column()) {
                case AddressColumn:
                    return Wt::WString(StringUtility::addrToString(row.meta.address()));
                case CrossRefsColumn:
                    return Wt::WString(StringUtility::numberToString(row.nrefs));
                case LengthEncodingColumn:
                    switch (row.meta.lengthEncoding()) {
                        case StringFinder::MAP_TERMINATED:
                            return Wt::WString("mmap boundary");
                        case StringFinder::NUL_TERMINATED:
                            return Wt::WString("NUL-terminated");
                        case StringFinder::SEQUENCE_TERMINATED:
                            return Wt::WString("sequence");
                        case StringFinder::BYTE_LENGTH:
                            return Wt::WString("byte");
                        case StringFinder::LE16_LENGTH:
                            return Wt::WString("le-16");
                        case StringFinder::BE16_LENGTH:
                            return Wt::WString("be-16");
                        case StringFinder::LE32_LENGTH:
                            return Wt::WString("le-32");
                        case StringFinder::BE32_LENGTH:
                            return Wt::WString("be-32");
                    }
                    break;
                case CharacterEncodingColumn:
                    switch (row.meta.characterEncoding()) {
                        case StringFinder::ASCII:
                            return Wt::WString("ASCII");
                    }
                    break;
                case NCharsColumn:
                    return Wt::WString(StringUtility::numberToString(row.meta.nCharacters()));
                case ValueColumn: {
                    // The WTableView widget adds elipses when the value overflows the width of the column, but we should add
                    // our own just to be sure that they appear even when our limit is less than the width of the column.
                    static const size_t nCharsToDisplay = 300;
                    return Wt::WString(StringUtility::cEscape(row.value.substr(0, nCharsToDisplay)) +
                                       (row.value.size()>nCharsToDisplay?"...":""));
                }
            }
        }
        return boost::any();
    }

    static bool addressOrder(const Row &a, const Row &b) { return a.meta.address() < b.meta.address(); }
    static bool nRefsOrder(const Row &a, const Row &b) { return a.nrefs < b.nrefs; }
    static bool lengthEncodingOrder(const Row &a, const Row &b) { return a.meta.lengthEncoding() < b.meta.lengthEncoding(); }
    static bool charEncodingOrder(const Row &a, const Row &b) { return a.meta.characterEncoding() < b.meta.characterEncoding(); }
    static bool lengthOrder(const Row &a, const Row &b) { return a.meta.nCharacters() < b.meta.nCharacters(); }
    static bool valueOrder(const Row &a, const Row &b) { return a.value < b.value; }

    void sort(int column, Wt::SortOrder order) ROSE_OVERRIDE {
        ASSERT_require(column>=0 && (ColumnNumber)column < NColumns);
        layoutAboutToBeChanged().emit();
        switch (column) {
            case AddressColumn:
                std::sort(rows_.begin(), rows_.end(), addressOrder);
                break;
            case CrossRefsColumn:
                std::sort(rows_.begin(), rows_.end(), nRefsOrder);
                break;
            case LengthEncodingColumn:
                std::sort(rows_.begin(), rows_.end(), lengthEncodingOrder);
                break;
            case CharacterEncodingColumn:
                std::sort(rows_.begin(), rows_.end(), charEncodingOrder);
                break;
            case NCharsColumn:
                std::sort(rows_.begin(), rows_.end(), lengthOrder);
                break;
            case ValueColumn:
                std::sort(rows_.begin(), rows_.end(), valueOrder);
        }
        if (order == Wt::DescendingOrder)
            std::reverse(rows_.begin(), rows_.end());

        layoutChanged().emit();
    }
    
    void memoryMap(const MemoryMap &map) {
        layoutAboutToBeChanged().emit();
        memoryMap_ = map;
        rows_.clear();
        xrefs_.clear();
    
        StringFinder analyzer;
        StringFinder::Strings strings = analyzer.findAllStrings(memoryMap_.require(MemoryMap::READABLE));
        BOOST_FOREACH (const StringFinder::String &string, strings.values()) {
            size_t nrefs = xrefs_.getOrDefault(P2::Reference(string.address())).size();
            std::string value = analyzer.decode(memoryMap_, string);
            rows_.push_back(Row(string, nrefs, value));
        }
        layoutChanged().emit();
    }

    void updateCrossReferences(const P2::Partitioner &partitioner) {
        layoutAboutToBeChanged().emit();

        AddressIntervalSet stringAddresses;
        BOOST_FOREACH (const Row &row, rows_)
            stringAddresses.insert(row.meta.address());
        xrefs_ = partitioner.instructionCrossReferences(stringAddresses);

        BOOST_FOREACH (Row &row, rows_)
            row.nrefs = xrefs_.getOrDefault(P2::Reference(row.meta.address())).size();

        layoutChanged().emit();
    }
};
                    
void
WStrings::init() {
    Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
    setLayout(vbox);

    // Address Space
    wAddressSpace_ = new WAddressSpace;
    wAddressSpace_->properties(SegmentsBar).pen = Wt::WPen(Wt::NoPen);
    wAddressSpace_->properties(StringsBar).pen = Wt::WPen(Wt::NoPen);
    vbox->addWidget(wAddressSpace_);

    // Search
    {
        Wt::WContainerWidget *wSearch = new Wt::WContainerWidget;
        vbox->addWidget(wSearch);
        new Wt::WText("Search: ", wSearch);
        Wt::WLineEdit *wEntry = new Wt::WLineEdit(wSearch);
        wEntry->changed().connect(boost::bind(&WStrings::search, this, wEntry));
        Wt::WPushButton *wSubmit = new Wt::WPushButton("Search", wSearch);
        wSubmit->clicked().connect(boost::bind(&WStrings::search, this, wEntry));
        wSearchResults_ = new Wt::WText("", wSearch);
    }
    
    // Table
    Wt::WContainerWidget *wTableContainer = new Wt::WContainerWidget;
    vbox->addWidget(wTableContainer, 1 /*stretch*/);
    Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout;
    wTableContainer->setLayout(hbox);

    model_ = new StringsModel;
    wTableView_ = new Wt::WTableView;
    wTableView_->setModel(model_);
    wTableView_->setRowHeaderCount(1);                  // this must be first
    wTableView_->setHeaderHeight(28);
    wTableView_->setSortingEnabled(true);
    wTableView_->setAlternatingRowColors(true);
    wTableView_->setColumnResizeEnabled(true);
    wTableView_->setSelectionBehavior(Wt::SelectRows);
    wTableView_->setSelectionMode(Wt::ExtendedSelection);
    wTableView_->setEditTriggers(Wt::WAbstractItemView::NoEditTrigger);
    wTableView_->setColumnWidth(StringsModel::AddressColumn, Wt::WLength(6, Wt::WLength::FontEm));
    wTableView_->setColumnWidth(StringsModel::LengthEncodingColumn, Wt::WLength(7, Wt::WLength::FontEm));
    wTableView_->setColumnWidth(StringsModel::CharacterEncodingColumn, Wt::WLength(5, Wt::WLength::FontEm));
    wTableView_->setColumnWidth(StringsModel::NCharsColumn, Wt::WLength(4, Wt::WLength::FontEm));
    wTableView_->setColumnWidth(StringsModel::ValueColumn, Wt::WLength(100, Wt::WLength::FontEm));
    wTableView_->clicked().connect(boost::bind(&WStrings::selectStringByRow, this, _1));
    hbox->addWidget(wTableView_);
}

void
WStrings::search(Wt::WLineEdit *wSearch) {
    try {
        boost::regex re(wSearch->text().narrow());
        Wt::WModelIndexSet selected;
        for (size_t i=0; i<model_->rows_.size(); ++i) {
            StringsModel::Row &row = model_->rows_[i];
            std::string lower = boost::to_lower_copy(row.value); // simulate case-insensitive matching
            if ((row.isMatching = boost::regex_search(lower, re, boost::regex_constants::match_perl)))
                selected.insert(model_->index(i, 0));
        }
        wTableView_->setSelectedIndexes(selected);
        wSearchResults_->setText(StringUtility::plural(selected.size(), "matches"));
    } catch (const boost::regex_error &e) {
        wSearchResults_->setText(e.what());
        wTableView_->setSelectedIndexes(Wt::WModelIndexSet());
    }
}

const MemoryMap&
WStrings::memoryMap() const {
    return model_->memoryMap_;
}

void
WStrings::memoryMap(const MemoryMap &map) {
    model_->memoryMap(map);

    // Update the address space bars
    wAddressSpace_->insert(map, SegmentsBar);
    WAddressSpace::HeatMap &hmap = wAddressSpace_->map(StringsBar);
    WAddressSpace::HeatMap &gutter = wAddressSpace_->bottomGutterMap();
    hmap.clear();
    gutter.clear();
    BOOST_FOREACH (const StringsModel::Row &string, model_->rows_) {
        hmap.insert(AddressInterval::baseSize(string.meta.address(), string.meta.nBytes()), 1.0);
        gutter.insert(string.meta.address(), 1.0);
    }
    wAddressSpace_->redraw();
}

void
WStrings::partitioner(const P2::Partitioner &p) {
    memoryMap(p.memoryMap());
    model_->updateCrossReferences(p);
}

void
WStrings::selectStringByRow(const Wt::WModelIndex &idx) {
    if (idx.isValid())
        stringClicked_.emit(idx.row());
}

const P2::ReferenceSet&
WStrings::crossReferences(size_t stringIdx) {
    ASSERT_require(stringIdx < model_->rows_.size());
    rose_addr_t stringVa = model_->rows_[stringIdx].meta.address();
    return model_->xrefs_.getOrDefault(P2::Reference(stringVa));
}

} // namespace
