#include <bROwSE/WStrings.h>

#include <boost/regex.hpp>
#include <bROwSE/WAddressSpace.h>
#include <bROwSE/WStringDetail.h>
#include <sawyer/Stopwatch.h>
#include <stringify.h>                                  // ROSE
#include <Wt/WAbstractTableModel>
#include <Wt/WHBoxLayout>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WStackedWidget>
#include <Wt/WTableView>
#include <Wt/WText>
#include <Wt/WVBoxLayout>

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace rose::Diagnostics;

namespace bROwSE {

enum ColumnNumber {
    AddressColumn,
    CrossRefsColumn,
    LengthEncodingSchemeColumn,
    CharacterEncodingFormColumn,
    CharacterEncodingSchemeColumn,
    NCharsColumn,
    ViewColumn,
    ValueColumn,
    NColumns
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      StringsModel
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class StringsModel: public Wt::WAbstractTableModel {
public:
    struct Row {
        Strings::EncodedString string;
        size_t nrefs;
        bool isMatching;
        Row(const Strings::EncodedString &string, size_t nrefs)
            : string(string), nrefs(nrefs), isMatching(false) {}
    };

    MemoryMap memoryMap_;
    bool isOutOfDate_;                                  // true if memoryMap has changed since rows_ and xrefs_ were set
    std::vector<Row> rows_;
    P2::CrossReferences xrefs_;                         // from string address to instruction

    StringsModel(): isOutOfDate_(false) {}

    bool isOutOfDate() const {
        return isOutOfDate_;
    }

    virtual int rowCount(const Wt::WModelIndex &parent=Wt::WModelIndex()) const ROSE_OVERRIDE {
        ASSERT_forbid(isOutOfDate_);
        return parent.isValid() ? 0 : rows_.size();
    }

    virtual int columnCount(const Wt::WModelIndex &parent=Wt::WModelIndex()) const ROSE_OVERRIDE {
        ASSERT_forbid(isOutOfDate_);
        return parent.isValid() ? 0 : NColumns;
    }

    virtual boost::any headerData(int column, Wt::Orientation orientation=Wt::Horizontal,
                                  int role=Wt::DisplayRole) const ROSE_OVERRIDE {
        ASSERT_forbid(isOutOfDate_);
        if (Wt::Horizontal == orientation) {
            if (Wt::DisplayRole == role) {
                switch (column) {
                    case AddressColumn:
                        return Wt::WString("Address");
                    case CrossRefsColumn:
                        return Wt::WString("NRefs");
                    case LengthEncodingSchemeColumn:
                        return Wt::WString("Length Scheme");
                    case CharacterEncodingFormColumn:
                        return Wt::WString("Char Form");
                    case CharacterEncodingSchemeColumn:
                        return Wt::WString("Char Scheme");
                    case NCharsColumn:
                        return Wt::WString("nChars");
                    case ViewColumn:
                        return Wt::WString("View");
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
        ASSERT_forbid(isOutOfDate_);
        ASSERT_require(index.isValid());
        ASSERT_require(index.row()>=0 && (size_t)index.row()<rows_.size());
        const Row &row = rows_[index.row()];
        Strings::LengthEncodedString::Ptr lengthStr = row.string.encoder().dynamicCast<Strings::LengthEncodedString>();
        Strings::TerminatedString::Ptr terminatedStr = row.string.encoder().dynamicCast<Strings::TerminatedString>();
        if (Wt::DisplayRole == role) {
            switch (index.column()) {
                case AddressColumn:
                    return Wt::WString(StringUtility::addrToString(row.string.address()));
                case CrossRefsColumn:
                    return Wt::WString(StringUtility::numberToString(row.nrefs));
                case LengthEncodingSchemeColumn:
                    if (lengthStr) {
                        return Wt::WString(lengthStr->lengthEncodingScheme()->name());
                    } else if (terminatedStr) {
                        if (1==terminatedStr->terminators().size() && terminatedStr->terminators()[0]==0) {
                            return Wt::WString("NUL-terminated");
                        } else {
                            return Wt::WString("terminated");
                        }
                    } else {
                        return Wt::WString("");
                    }
                    break;
                case CharacterEncodingFormColumn:
                    return Wt::WString(row.string.encoder()->characterEncodingForm()->name());
                case CharacterEncodingSchemeColumn:
                    return Wt::WString(row.string.encoder()->characterEncodingScheme()->name());
                case NCharsColumn:
                    return Wt::WString(StringUtility::numberToString(row.string.length()));
                case ValueColumn: {
                    // The WTableView widget adds elipses when the value overflows the width of the column, but we should add
                    // our own just to be sure that they appear even when our limit is less than the width of the column.
                    static const size_t nCharsToDisplay = 300;
                    std::string s = row.string.narrow();
                    return Wt::WString(StringUtility::cEscape(s.substr(0, nCharsToDisplay)) +
                                       (s.size()>nCharsToDisplay?"...":""));
                }
            }
        } else if (Wt::DecorationRole == role) {
            if (index.column() == ViewColumn)
                return Wt::WString("/images/view-16x16.png");
        } else if (Wt::StyleClassRole == role) {
            if (row.isMatching)
                return Wt::WString("strings_matched");
            if (index.row() % 2)
                return Wt::WString("strings_oddrow");
        }
        return boost::any();
    }

    static bool addressOrder(const Row &a, const Row &b) {
        return a.string.address() < b.string.address();
    }
    static bool nRefsOrder(const Row &a, const Row &b) {
        return a.nrefs < b.nrefs;
    }
    static bool lengthEncodingOrder(const Row &a, const Row &b) {
        Strings::LengthEncodedString::Ptr al = a.string.encoder().dynamicCast<Strings::LengthEncodedString>();
        Strings::LengthEncodedString::Ptr bl = b.string.encoder().dynamicCast<Strings::LengthEncodedString>();
        Strings::TerminatedString::Ptr at = a.string.encoder().dynamicCast<Strings::TerminatedString>();
        Strings::TerminatedString::Ptr bt = b.string.encoder().dynamicCast<Strings::TerminatedString>();
        std::string as, bs;
        if (al) {
            as = al->lengthEncodingScheme()->name();
        } else if (at && at->terminators().size()==1 && at->terminators()[0]==0) {
            as = "NUL-terminated";
        } else if (at) {
            as = "terminated";
        }
        if (bl) {
            bs = bl->lengthEncodingScheme()->name();
        } else if (bt && bt->terminators().size()==1 && bt->terminators()[0]==0) {
            bs = "NUL-terminated";
        } else if (bt) {
            bs = "terminated";
        }
        return as < bs;
    }
    static bool charEncodingFormOrder(const Row &a, const Row &b) {
        return a.string.encoder()->characterEncodingForm()->name() < b.string.encoder()->characterEncodingForm()->name();
    }
    static bool charEncodingSchemeOrder(const Row &a, const Row &b) {
        return a.string.encoder()->characterEncodingScheme()->name() < b.string.encoder()->characterEncodingScheme()->name();
    }
    static bool lengthOrder(const Row &a, const Row &b) {
        return a.string.length() < b.string.length();
    }
    static bool valueOrder(const Row &a, const Row &b) {
        return a.string.wide() < b.string.wide();
    }

    void sort(int column, Wt::SortOrder order) ROSE_OVERRIDE {
        updateMemoryMap();
        ASSERT_require(column>=0 && (ColumnNumber)column < NColumns);
        if (ViewColumn == column)
            return;                                     // not a sortable column

        layoutAboutToBeChanged().emit();
        switch (column) {
            case AddressColumn:
                std::sort(rows_.begin(), rows_.end(), addressOrder);
                break;
            case CrossRefsColumn:
                std::sort(rows_.begin(), rows_.end(), nRefsOrder);
                break;
            case LengthEncodingSchemeColumn:
                std::sort(rows_.begin(), rows_.end(), lengthEncodingOrder);
                break;
            case CharacterEncodingFormColumn:
                std::sort(rows_.begin(), rows_.end(), charEncodingFormOrder);
                break;
            case CharacterEncodingSchemeColumn:
                std::sort(rows_.begin(), rows_.end(), charEncodingSchemeOrder);
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
        memoryMap_ = map;
        isOutOfDate_ = true;
    }

    void updateMemoryMap() {
        if (isOutOfDate_) {
            Sawyer::Stopwatch timer;
            Sawyer::Message::Stream info(mlog[INFO] <<"scanning for string literals");
            layoutAboutToBeChanged().emit();
            rows_.clear();
            xrefs_.clear();

            ByteOrder::Endianness sex = memoryMap_.byteOrder();
            if (ByteOrder::ORDER_UNSPECIFIED == sex)
                sex = ByteOrder::ORDER_LSB;

            Strings::StringFinder analyzer;
            analyzer.settings().minLength = 5;
            analyzer.settings().maxLength = 65536;
            analyzer.settings().keepingOnlyLongest = true;
            analyzer.insertCommonEncoders(sex);
            analyzer.insertUncommonEncoders(sex);
            analyzer.find(memoryMap_.require(MemoryMap::READABLE));
            BOOST_FOREACH (const Strings::EncodedString &string, analyzer.strings()) {
                size_t nrefs = xrefs_.getOrDefault(P2::Reference(string.address())).size();
                rows_.push_back(Row(string, nrefs));
            }
            isOutOfDate_ = false;
            layoutChanged().emit();
            info <<"; took " <<timer <<" seconds\n";
        }
    }

    void updateCrossReferences(const P2::Partitioner &partitioner) {
        updateMemoryMap();
        layoutAboutToBeChanged().emit();

        // Find all references to those strings
        AddressIntervalSet stringAddresses;
        BOOST_FOREACH (const Row &row, rows_)
            stringAddresses.insert(row.string.address());
        xrefs_ = partitioner.instructionCrossReferences(stringAddresses);

        // Update reference counts and prune away non-referenced strings that overlap with code. Pruning away the
        // non-referenced strings that overlap with code is a good way to get rid of lots of false positives.
        std::vector<Row> keep;
        BOOST_FOREACH (Row &row, rows_) {
            row.nrefs = xrefs_.getOrDefault(P2::Reference(row.string.address())).size();
            if (0!=row.nrefs || partitioner.instructionsOverlapping(row.string.address()).empty())
                keep.push_back(row);
        }
        rows_ = keep;

        layoutChanged().emit();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Strings widget
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    
void
WStrings::init() {
    Wt::WVBoxLayout *topVBox = new Wt::WVBoxLayout;
    setLayout(topVBox);

    wStack_ = new Wt::WStackedWidget;
    topVBox->addWidget(wStack_);

    //---------------
    // Overview Pane
    //---------------

    ASSERT_require(OVERVIEW==0);
    wOverview_ = new Wt::WContainerWidget;
    wStack_->addWidget(wOverview_);
    
    Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
    wOverview_->setLayout(vbox);

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
        wPrev_ = new Wt::WPushButton("Previous", wSearch);
        wPrev_->setDisabled(true);
        wPrev_->clicked().connect(boost::bind(&WStrings::searchNavigate, this, -1));
        wNext_ = new Wt::WPushButton("Next", wSearch);
        wNext_->setDisabled(true);
        wNext_->clicked().connect(boost::bind(&WStrings::searchNavigate, this, 1));
        wSearchResults_ = new Wt::WText("", wSearch);
    }
    

    // Table. The HBoxLayout is so the table scrolls horizontally.
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
    wTableView_->setSortingEnabled(ViewColumn, false);
    wTableView_->setAlternatingRowColors(false);        // we do our own colors
    wTableView_->setColumnResizeEnabled(true);
    wTableView_->setSelectionBehavior(Wt::SelectRows);
    wTableView_->setSelectionMode(Wt::SingleSelection);
    wTableView_->setEditTriggers(Wt::WAbstractItemView::NoEditTrigger);
    wTableView_->setColumnWidth(AddressColumn, Wt::WLength(6, Wt::WLength::FontEm));
    wTableView_->setColumnWidth(CrossRefsColumn, Wt::WLength(4, Wt::WLength::FontEm));
    wTableView_->setColumnWidth(LengthEncodingSchemeColumn, Wt::WLength(7, Wt::WLength::FontEm));
    wTableView_->setColumnWidth(CharacterEncodingFormColumn, Wt::WLength(5, Wt::WLength::FontEm));
    wTableView_->setColumnWidth(CharacterEncodingSchemeColumn, Wt::WLength(5, Wt::WLength::FontEm));
    wTableView_->setColumnWidth(NCharsColumn, Wt::WLength(4, Wt::WLength::FontEm));
    wTableView_->setColumnWidth(ViewColumn, Wt::WLength(3, Wt::WLength::FontEm));
    wTableView_->setColumnWidth(ValueColumn, Wt::WLength(100, Wt::WLength::FontEm));
    wTableView_->clicked().connect(boost::bind(&WStrings::selectStringByRow, this, _1));
    hbox->addWidget(wTableView_);

    //--------------
    // Details Pane
    //--------------

    ASSERT_require(DETAILS==1);
    wDetails_ = new WStringDetail;
    wDetails_->button()->clicked().connect(boost::bind(&Wt::WStackedWidget::setCurrentIndex, wStack_, OVERVIEW));
    wStack_->addWidget(wDetails_);


    wStack_->setCurrentIndex(OVERVIEW);
}

void
WStrings::search(Wt::WLineEdit *wSearch) {
    model_->layoutAboutToBeChanged().emit();

    size_t nFound = 0, firstMatch = model_->rows_.size();
    BOOST_FOREACH (StringsModel::Row &row, model_->rows_)
        row.isMatching = false;

    if (wSearch->text().narrow().empty()) {
        wSearchResults_->setText("");
    } else {
        try {
            boost::regex re(wSearch->text().narrow());
            for (size_t i=0; i<model_->rows_.size(); ++i) {
                StringsModel::Row &row = model_->rows_[i];
                std::string lower = boost::to_lower_copy(row.string.narrow()); // simulate case-insensitive matching
                if ((row.isMatching = boost::regex_search(lower, re, boost::regex_constants::match_perl))) {
                    ++nFound;
                    firstMatch = std::min(firstMatch, i);
                }
            }
            wSearchResults_->setText(StringUtility::plural(nFound, "matches", "match"));
        } catch (const boost::regex_error &e) {
            wSearchResults_->setText(e.what());
        }
    }

    model_->layoutChanged().emit();

    wPrev_->setDisabled(nFound<=1);
    wNext_->setDisabled(nFound<=1);

    searchNavigate(1);
}

void
WStrings::searchNavigate(int direction) {
    if (model_->rows_.empty())
        return;

    Wt::WModelIndexSet iset = wTableView_->selectedIndexes();
    ASSERT_require(iset.empty() || iset.begin()->isValid());
    size_t curIdx = iset.empty() ? (direction>=0 ? model_->rows_.size()-1 : 0) : iset.begin()->row();
    for (size_t i=0; i<model_->rows_.size(); ++i) {
        if (direction>=0) {
            if (++curIdx >= model_->rows_.size())
                curIdx = 0;
        } else {
            if (0==curIdx)
                curIdx = model_->rows_.size();
            --curIdx;
        }
        if (model_->rows_[curIdx].isMatching) {
            Wt::WModelIndex newIdx = model_->index(curIdx, 0);
            wTableView_->scrollTo(newIdx);
            wTableView_->select(newIdx);
            return;
        }
    }

    wTableView_->setSelectedIndexes(Wt::WModelIndexSet());
}

const MemoryMap&
WStrings::memoryMap() const {
    return model_->memoryMap_;
}

void
WStrings::memoryMap(const MemoryMap &map) {
    model_->memoryMap(map);
}

void
WStrings::redrawAddressSpace(const MemoryMap &map) {
    wAddressSpace_->insert(map, SegmentsBar);
    WAddressSpace::HeatMap &hmap = wAddressSpace_->map(StringsBar);
    WAddressSpace::HeatMap &gutter = wAddressSpace_->bottomGutterMap();
    hmap.clear();
    gutter.clear();
    BOOST_FOREACH (const StringsModel::Row &row, model_->rows_) {
        hmap.insert(AddressInterval::baseSize(row.string.address(), row.string.size()), 1.0);
        gutter.insert(row.string.address(), 1.0);
    }
    wAddressSpace_->redraw();
}

void
WStrings::updateModelIfNecessary() {
    if (model_->isOutOfDate()) {
        model_->updateMemoryMap();
        redrawAddressSpace(memoryMap());
    }
}

void
WStrings::partitioner(const P2::Partitioner &p) {
    memoryMap(p.memoryMap());
    model_->updateCrossReferences(p);
    redrawAddressSpace(p.memoryMap());
}

void
WStrings::selectStringByRow(const Wt::WModelIndex &idx) {
    if (idx.isValid()) {
        if (idx.column() == ViewColumn) {
            ASSERT_require(idx.row()>=0 && (size_t)idx.row()<model_->rows_.size());
            const StringsModel::Row &row = model_->rows_[idx.row()];
            wDetails_->changeString(row.string);
            wStack_->setCurrentIndex(DETAILS);
        }
        stringClicked_.emit(idx.row());
    }
}

const P2::ReferenceSet&
WStrings::crossReferences(size_t stringIdx) {
    ASSERT_require(stringIdx < model_->rows_.size());
    rose_addr_t stringVa = model_->rows_[stringIdx].string.address();
    return model_->xrefs_.getOrDefault(P2::Reference(stringVa));
}

const rose::BinaryAnalysis::Strings::EncodedString&
WStrings::string(size_t stringIdx) {
    ASSERT_require(stringIdx < model_->rows_.size());
    return model_->rows_[stringIdx].string;
}

} // namespace
