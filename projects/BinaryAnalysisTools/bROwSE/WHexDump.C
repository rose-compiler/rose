#include <rose.h>
#include <bROwSE/WHexDump.h>

#include <bROwSE/FunctionUtil.h>

#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <Rose/Diagnostics.h>
#include <rose_strtoull.h>
#include <Sawyer/Stopwatch.h>
#include <Wt/WHBoxLayout>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WTableView>
#include <Wt/WText>
#include <Wt/WVBoxLayout>

using namespace Rose;
using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;

namespace bROwSE {

static const size_t bytesPerRow = 16;                           // number of bytes represented by each row of the table
static const size_t addressColumn = 0;                          // index of column containing the address
static const size_t sep1Column = addressColumn + 1;             // separates address from what follows
static const size_t bytesColumn = sep1Column + 1;               // index of column containing first hexadecimal byte
static const size_t sep2Column = bytesColumn + bytesPerRow;     // separates hexadecimal bytes from ASCII
static const size_t asciiColumn = sep2Column + 1;               // index of column containing first ASCII byte
static const size_t endColumn = asciiColumn + bytesPerRow;      // index one past table right edge

void
HexDumpModel::init() {
}

void
HexDumpModel::memoryMap(const MemoryMap::Ptr &map) {
    layoutAboutToBeChanged().emit();
    memoryMap_ = map;

    // We need to be able to quickly look up the virtual address that corresponds to the first byte of the table row.  Segments
    // occupy contiguous memory addresses but there are gaps between them. We want to show these gaps in the table by a blank
    // line (even if they are zero size gaps). Furthermore, the data should be aligned so that address represented by each
    // table column is consistent modulo N for all rows, where N is the number of bytes displayed per row.  I.e., the address
    // of the first byte of each row should be 0 modulo N.
    size_t row = 0;
    addrRow_.clear();
    rowAddr_.clear();
    BOOST_FOREACH (const AddressInterval &interval, memoryMap_->intervals()) {
        addrRow_.insert(interval.least(), row);
        rowAddr_.insert(row, interval.least());
        rose_addr_t beginVa = alignDown(interval.least(), bytesPerRow);
        rose_addr_t endVa = alignUp(interval.greatest()+1, bytesPerRow);
        size_t nRows = (endVa - beginVa) / bytesPerRow; // number of table rows needed to display this segment
        row += nRows + 1;                               // +1 is the blank row between segments
    }
    nRows_ = row;

    layoutChanged().emit();
}

MemoryMap::ConstNodeIterator
HexDumpModel::rowSegment(size_t row) const {
    ASSERT_require(row < nRows_);

    // Find the rowAddr map entry for this row.
    RowAddrMap::ConstNodeIterator found = rowAddr_.lowerBound(row);
    if (found == rowAddr_.nodes().end())
        --found;                                        // row > last segment's starting row
    if (row < found->key()) {
        ASSERT_forbid2(found == rowAddr_.nodes().begin(), "row 0 is apparently not mapped");
        --found;
    }

    size_t segmentRowIdx = found->key();
    rose_addr_t segmentVa = found->value();
    rose_addr_t segmentRowVa = alignDown(segmentVa, bytesPerRow);
    if (row == segmentRowIdx)
        return memoryMap_->at(segmentVa).findNode();
    rose_addr_t rowVa = segmentRowVa + (row - segmentRowIdx) * bytesPerRow;
    MemoryMap::ConstNodeIterator mmIter = memoryMap_->at(segmentVa).findNode();
    if (!mmIter->key().isContaining(rowVa))
        return memoryMap_->nodes().end();// segment separator row
    return mmIter;
}

Sawyer::Optional<rose_addr_t>
HexDumpModel::rowAddress(size_t row) const {
    ASSERT_require(row < nRows_);

    // Find the rowAddr map entry for this row.
    RowAddrMap::ConstNodeIterator found = rowAddr_.lowerBound(row);
    if (found == rowAddr_.nodes().end())
        --found;                                        // row > last segment's starting row
    if (row < found->key()) {
        ASSERT_forbid2(found == rowAddr_.nodes().begin(), "row 0 is apparently not mapped");
        --found;
    }

    size_t segmentRowIdx = found->key();                // row where this segments starts
    rose_addr_t segmentRowVa = alignDown(found->value(), bytesPerRow);
    if (row == segmentRowIdx)
        return segmentRowVa;
    rose_addr_t rowVa = segmentRowVa + (row-segmentRowIdx)*bytesPerRow;

    // If the first cell of the row has a address that's unmapped or belongs to some other segment then this must be an empty
    // segement-separator row that has no address.
    rose_addr_t segmentVa = found->value();
    if (memoryMap_->at(segmentVa).findNode() != memoryMap_->at(rowVa).findNode())
        return Sawyer::Nothing();
    return rowVa;
}

Sawyer::Optional<rose_addr_t>
HexDumpModel::cellAddress(const Wt::WModelIndex &idx) const {
    if (!idx.isValid())
        return Sawyer::Nothing();
    size_t column = idx.column();
    size_t row = idx.row();

    if (column >= bytesColumn && column < bytesColumn + bytesPerRow)
        return cellAddress(row, column - bytesColumn);
    if (column >= asciiColumn && column < asciiColumn + bytesPerRow)
        return cellAddress(row, column - asciiColumn);
    return Sawyer::Nothing();
}

Sawyer::Optional<rose_addr_t>
HexDumpModel::cellAddress(size_t row, size_t pseudoColumn) const {
    ASSERT_require(row < nRows_);
    ASSERT_require(pseudoColumn < bytesPerRow);

    // Find the rowAddr map entry for this row.
    RowAddrMap::ConstNodeIterator found = rowAddr_.lowerBound(row);
    if (found == rowAddr_.nodes().end())
        --found;                                        // row > last segment's starting row
    if (row < found->key()) {
        ASSERT_forbid2(found == rowAddr_.nodes().begin(), "row 0 is apparently not mapped");
        --found;
    }

    // Get the address for this particular table cell
    size_t segmentRowIdx = found->key();
    rose_addr_t segmentRowVa = alignDown(found->value(), bytesPerRow);
    rose_addr_t cellVa = segmentRowVa + (row-segmentRowIdx)*bytesPerRow + pseudoColumn;

    // If the cellVa comes from a different memory segment then it has no address
    rose_addr_t segmentVa = found->value();
    if (memoryMap_->at(segmentVa).findNode() != memoryMap_->at(cellVa).findNode())
        return Sawyer::Nothing();

    return cellVa;
}

Sawyer::Optional<uint8_t>
HexDumpModel::readByte(const Sawyer::Optional<rose_addr_t> &cellVa) const {
    uint8_t byte;
    if (cellVa && memoryMap_->at(*cellVa).limit(1).read(&byte))
        return byte;
    return Sawyer::Nothing();
}

size_t
HexDumpModel::closestRowForAddress(rose_addr_t va) const {
    ASSERT_forbid(addrRow_.isEmpty());
    AddrRowMap::ConstNodeIterator found = addrRow_.lowerBound(va);
    if (found == addrRow_.nodes().end())
        --found;                                        // va > last segment's starting address
    if (va < found->key()) {
        if (found == addrRow_.nodes().begin())
            return 0;                                   // va < first row of table
        --found;
    }

    rose_addr_t segmentVa = found->key();               // starting address for segment possibly containing va
    size_t segmentRow = found->value();                 // row where that segment begins
    rose_addr_t segmentRowVa = alignDown(segmentVa, bytesPerRow); // address for beginning of that row
    size_t row = segmentRow + (va - segmentRowVa)/bytesPerRow; // potential return value
    row = std::min(row, nRows_-1);
    return row;
}

int
HexDumpModel::rowCount(const Wt::WModelIndex &parent) const {
    return nRows_;
}

int
HexDumpModel::columnCount(const Wt::WModelIndex &parent) const {
    return parent.isValid() ? 0 : endColumn;
}

boost::any
HexDumpModel::headerData(int column_, Wt::Orientation orientation, int role) const {
    ASSERT_require(column_ >= 0);
    size_t column = column_;
    if (Wt::Horizontal == orientation && Wt::DisplayRole == role) {
        ASSERT_require(column>=0 && (size_t)column < endColumn);
        if (column == addressColumn) {
            return Wt::WString("Address");
        } else if (column >= bytesColumn && column < bytesColumn + bytesPerRow) {
            char buf[8];
            sprintf(buf, "%x", (unsigned)(column - bytesColumn));
            return Wt::WString(buf);
        } else if (column >= asciiColumn && column < asciiColumn + bytesPerRow) {
            char buf[8];
            sprintf(buf, "%x", (unsigned)(column - asciiColumn));
            return Wt::WString(buf);
        } else if (column == sep1Column || column == sep2Column) {
            return Wt::WString("");
        } else {
            ASSERT_not_reachable("this column needs a header");
        }
    }
    return boost::any();
}

boost::any
HexDumpModel::data(const Wt::WModelIndex &index, int role) const {
    ASSERT_require(index.isValid());
    ASSERT_require(index.row() >= 0);                   // Why signed when a valid row is non-negative?
    size_t row = index.row();
    ASSERT_require(row < nRows_);
    ASSERT_require(index.column() >= 0);                // Why signed when a valid column is non-negative?
    size_t column = index.column();
    ASSERT_require(column < endColumn);

    if (role == Wt::DisplayRole) {
        if (column == addressColumn) {
            rose_addr_t va = 0;
            if (rowAddress(row).assignTo(va))
                return Wt::WString(StringUtility::addrToString(va));
            return Wt::WString("");
        } else if (column >= bytesColumn && column < bytesColumn + bytesPerRow) {
            uint8_t byte = 0;
            if (readByte(cellAddress(row, column-bytesColumn)).assignTo(byte) && byte!=0) {
                char buf[8];
                sprintf(buf, "%02x", (unsigned)byte);
                return Wt::WString(buf);
            }
            return Wt::WString("");
        } else if (column >= asciiColumn && column < asciiColumn + bytesPerRow) {
            uint8_t byte = 0;
            std::string s;
            if (readByte(cellAddress(row, column-asciiColumn)).assignTo(byte))
                s = charToString(byte);
            return Wt::WString(s);
        } else if (column == sep1Column || column == sep2Column) {
            return Wt::WString("");
        } else {
            ASSERT_not_reachable("this column needs data");
        }
    } else if (role == Wt::ToolTipRole) {
        MemoryMap::ConstNodeIterator mmNode = memoryMap_->nodes().end();
        rose_addr_t va = 0;
        if (column == addressColumn) {
            mmNode = rowSegment(row);
            va = cellAddress(row, 0).orElse(0);
        } else if (column >= bytesColumn && column < bytesColumn + bytesPerRow &&
                   cellAddress(row, column-bytesColumn).assignTo(va)) {
            mmNode = memoryMap_->at(va).findNode();
        } else if (column >= asciiColumn && column < asciiColumn + bytesPerRow &&
                   cellAddress(row, column-asciiColumn).assignTo(va)) {
            mmNode = memoryMap_->at(va).findNode();
        }
        if (mmNode != memoryMap_->nodes().end()) {
            const AddressInterval &interval = mmNode->key();
            const MemoryMap::Segment &segment = mmNode->value();
            std::string tip = StringUtility::htmlEscape(segment.name());
            if (interval.isContaining(va))
                tip += "+" + StringUtility::addrToString(va-interval.least());
            return Wt::WString(tip);
        }
    } else if (role == Wt::StyleClassRole) {
        if (column==sep1Column || column==sep2Column || !rowAddress(row)) {
            return Wt::WString("hexdump_unmapped");
        } else if (column >= bytesColumn && column < bytesColumn + bytesPerRow && !cellAddress(row, column-bytesColumn)) {
            return Wt::WString("hexdump_unmapped");
        } else if (column >= asciiColumn && column < asciiColumn + bytesPerRow) {
            if (!cellAddress(row, column-asciiColumn)) {
                return Wt::WString("hexdump_unmapped");
            } else if (charToString(*readByte(cellAddress(row, column-asciiColumn))).empty()) {
                return Wt::WString("hexdump_nochar");
            }
        } else if (column == addressColumn) {
            MemoryMap::ConstNodeIterator mmIter = rowSegment(row);
            ASSERT_require(mmIter != memoryMap_->nodes().end()); // would have been caught above
            unsigned a = mmIter->value().accessibility();
            std::string style;
            if (0!=(a & MemoryMap::READABLE))
                style += "r";
            if (0!=(a & MemoryMap::WRITABLE))
                style += "w";
            if (0!=(a & MemoryMap::EXECUTABLE))
                style += "x";
            if (style.empty())
                style = "none";
            return Wt::WString("hexdump_addr_" + style);
        } else if (row % 2) {
            return Wt::WString("hexdump_oddrow");
        } else {
            return Wt::WString("hexdump_evenrow");
        }
    }
    return boost::any();
}

void
WHexDump::init() {
    Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
    setLayout(vbox);

    Wt::WContainerWidget *actionsBox = new Wt::WContainerWidget;
    vbox->addWidget(actionsBox);
    {
        new Wt::WText("Goto: ", actionsBox);
        wAddressEdit_ = new Wt::WLineEdit(actionsBox);
        wAddressEdit_->enterPressed().connect(this, &WHexDump::handleGoto);

        new Wt::WBreak(actionsBox);
        new Wt::WText("Search: ", actionsBox);
        wSearchEdit_ = new Wt::WLineEdit(actionsBox);
        wSearchEdit_->keyPressed().connect(this, &WHexDump::resetSearch);
        wSearchNext_ = new Wt::WPushButton("Find", actionsBox);
        wSearchNext_->clicked().connect(this, &WHexDump::handleSearch);
        wSearchResults_ = new Wt::WText("Enter a big-endian hexadecimal value", actionsBox);
        
    }

    Wt::WContainerWidget *tableContainer = new Wt::WContainerWidget;
    vbox->addWidget(tableContainer, 1 /*stretch*/);
    Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout;        // so the table scrolls horizontally
    tableContainer->setLayout(hbox);

    model_ = new HexDumpModel;

    tableView_ = new Wt::WTableView;
    tableView_->setModel(model_);
    tableView_->setRowHeaderCount(1);                   // this must be first property set
    tableView_->setHeaderHeight(28);
    tableView_->setSortingEnabled(false);
    tableView_->setAlternatingRowColors(false);         // true interferes with our blacking out unmapped addresses
    tableView_->setColumnResizeEnabled(true);
    tableView_->setSelectionMode(Wt::SingleSelection);
    tableView_->setEditTriggers(Wt::WAbstractItemView::NoEditTrigger);
    tableView_->setColumnWidth(addressColumn, Wt::WLength(6, Wt::WLength::FontEm));
    tableView_->setColumnWidth(sep1Column, Wt::WLength(1, Wt::WLength::FontEm));
    tableView_->setColumnWidth(sep2Column, Wt::WLength(1, Wt::WLength::FontEm));
    for (size_t i=0; i<bytesPerRow; ++i) {
        int extra = 7==i%8 && i+1<bytesPerRow ? 1 : 0;
        tableView_->setColumnWidth(bytesColumn + i, Wt::WLength(2+extra, Wt::WLength::FontEm));
        tableView_->setColumnWidth(asciiColumn + i, Wt::WLength(2+extra, Wt::WLength::FontEm));
    }
    tableView_->clicked().connect(boost::bind(&WHexDump::handleClick, this, _1));
    hbox->addWidget(tableView_);
}

void
WHexDump::makeVisible(rose_addr_t va) {
    size_t rowIdx = model_->closestRowForAddress(va);
    static const size_t leadingContextRows = 5;         // number of rows to show above that

    tableView_->scrollTo(model_->index(rowIdx - std::min(rowIdx, leadingContextRows), 0));
    tableView_->scrollTo(model_->index(rowIdx, 0));
    tableView_->select(model_->index(rowIdx, 0));
}

void
WHexDump::handleClick(const Wt::WModelIndex &idx) {
    rose_addr_t va = 0;
    if (model_->cellAddress(idx).assignTo(va))
        byteClicked_.emit(va);
}
    
void
WHexDump::handleGoto() {
    std::string str = wAddressEdit_->valueText().narrow();
    rose_addr_t va = rose_strtoull(str.c_str(), NULL, 0);
    makeVisible(va);
}

void
WHexDump::resetSearch() {
    wSearchNext_->setText("Find");
    wSearchResults_->setText("Enter a big-endian hexadecimal value");
    searchRegion_ = AddressInterval();
}

void
WHexDump::handleSearch() {
    // Convert the search string to a vector of bytes, ignoring white space
    std::string str = wSearchEdit_->valueText().narrow();
    boost::erase_all(str, " ");
    if (boost::starts_with(str, "0x"))
        str = str.substr(2);
    if (str.empty()) {
        resetSearch();
        return;
    }
    if (str.empty() || str.size() % 2 != 0) {
        wSearchResults_->setText("Not byte aligned");
        return;
    }
    std::vector<uint8_t> bytes;
    for (const char *ch = str.c_str(); *ch; ch+=2) {
        uint8_t byte = 0;
        for (int i=0; i<2; ++i) {
            if (isdigit(ch[i])) {
                byte = (byte << 4) | (ch[i]-'0');
            } else if (ch[i]>='a' && ch[i]<='f') {
                byte = (byte << 4) | (ch[i]-'a'+10);
            } else if (ch[i]>='A' && ch[i]<='F') {
                byte = (byte << 4) | (ch[i]-'A'+10);
            } else {
                wSearchResults_->setText("Invalid hexadecimal character");
                return;
            }
        }
        bytes.push_back(byte);
    }

    // If the search region is empty then start from the beginning again.
    if (!searchRegion_)
        searchRegion_ = model_->memoryMap()->hull();

    // Find the vector
    Sawyer::Message::Stream info(mlog[INFO] <<"searching for " <<str);
    Sawyer::Stopwatch timer;
    bool found = false;
    rose_addr_t startVa = 0;
#if 0 // [Robb P. Matzke 2015-05-04]
    std::vector<uint8_t> leadBytes(1, bytes[0]);
    while (!found && searchRegion_) {
        if (model_->memoryMap()->findAny(searchRegion_, leadBytes, 0, 0).assignTo(startVa) &&
            startVa <= searchRegion_.greatest()) {
            
            searchRegion_ = AddressInterval::hull(startVa, searchRegion_.greatest());
            std::vector<uint8_t> readBytes(bytes.size());
            if (model_->memoryMap()->at(startVa).read(readBytes).size() == bytes.size() &&
                std::equal(bytes.begin(), bytes.end(), readBytes.begin())) {
                found = true;
            }
        }
        if (searchRegion_.isSingleton()) {
            searchRegion_ = AddressInterval();
        } else {
            searchRegion_ = AddressInterval::hull(searchRegion_.least()+1, searchRegion_.greatest());
        }
    }
#else
    if (model_->memoryMap()->findSequence(searchRegion_, bytes).assignTo(startVa)) {
        found = true;
        searchRegion_ = startVa==searchRegion_.greatest() ?
                        AddressInterval() :
                        AddressInterval::hull(startVa+1, searchRegion_.greatest());
    }
#endif

    // Report results
    if (found) {
        info <<"; found at " <<StringUtility::addrToString(startVa) <<"; took " <<timer <<"\n";
        wSearchNext_->setText("Next");
        wSearchResults_->setText("Found at " + StringUtility::addrToString(startVa));
        makeVisible(startVa);
    } else {
        info <<"; not found; took " <<timer <<"\n";
        wSearchNext_->setText("Find");
        wSearchResults_->setText("Not found");
    }
}

void
WHexDump::partitioner(const P2::Partitioner &p) {
    memoryMap(p.memoryMap());

    AddressIntervalSet allAddresses = p.memoryMap()->intervals();
    xrefs_ = p.instructionCrossReferences(allAddresses);
}

const P2::ReferenceSet&
WHexDump::crossReferences(rose_addr_t va) const {
    return xrefs_.getOrDefault(P2::Reference(va));
}

} // namespace
