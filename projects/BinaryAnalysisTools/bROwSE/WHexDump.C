#include <bROwSE/WHexDump.h>
#include <Wt/WHBoxLayout>
#include <Wt/WTableView>

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
    // We need to be able to quickly look up the virtual address that corresponds to the first byte of the table row.  Segments
    // occupy contiguous memory addresses but there are gaps between them. We want to show these gaps in the table by a blank
    // line (even if they are zero size gaps). Furthermore, the data should be aligned so that address represented by each
    // table column is consistent modulo N for all rows, where N is the number of bytes displayed per row.  I.e., the address
    // of the first byte of each row should be 0 modulo N.
    size_t row = 0;
    BOOST_FOREACH (const AddressInterval &interval, ctx_.partitioner.memoryMap().intervals()) {
#if 0 // [Robb P. Matzke 2014-12-23]: not needed yet
        addrRow_.insert(interval.least(), row);
#endif
        rowAddr_.insert(row, interval.least());
        rose_addr_t beginVa = alignDown(interval.least(), bytesPerRow);
        rose_addr_t endVa = alignUp(interval.greatest()+1, bytesPerRow);
        size_t nRows = (endVa - beginVa) / bytesPerRow; // number of table rows needed to display this segment
        row += nRows + 1;                               // +1 is the blank row between segments
    }
    nRows_ = row;

#if 1 // DEBUGGING [Robb P. Matzke 2014-12-23]
#if 0 // [Robb P. Matzke 2014-12-23]: not needed yet
    std::cerr <<"ROBB: addr->row mapping:\n";
    BOOST_FOREACH (const AddrRowMap::Node &node, addrRow_.nodes())
        std::cerr <<"        " <<StringUtility::addrToString(node.key()) <<" -> " <<node.value() <<"\n";
#endif
    std::cerr <<"ROBB: row->addr mapping:\n";
    BOOST_FOREACH (const RowAddrMap::Node &node, rowAddr_.nodes())
        std::cerr <<"        " <<node.key() <<" -> " <<StringUtility::addrToString(node.value()) <<"\n";
#endif
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
    if (ctx_.partitioner.memoryMap().at(segmentVa).findNode() != ctx_.partitioner.memoryMap().at(rowVa).findNode())
        return Sawyer::Nothing();
    return rowVa;
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
    if (ctx_.partitioner.memoryMap().at(segmentVa).findNode() != ctx_.partitioner.memoryMap().at(cellVa).findNode())
        return Sawyer::Nothing();

    return cellVa;
}

Sawyer::Optional<uint8_t>
HexDumpModel::readByte(const Sawyer::Optional<rose_addr_t> &cellVa) const {
    uint8_t byte;
    if (cellVa && ctx_.partitioner.memoryMap().at(*cellVa).limit(1).read(&byte))
        return byte;
    return Sawyer::Nothing();
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
            if (readByte(cellAddress(row, column-asciiColumn)).assignTo(byte)) {
                switch (byte) {
                    //case '\0': s = "\\0"; break; //too many of these clutter up the important stuff
                    case '\a': s = "\\a"; break;
                    case '\b': s = "\\b"; break;
                    case '\t': s = "\\t"; break;
                    case '\n': s = "\\n"; break;
                    case '\v': s = "\\v"; break;
                    case '\f': s = "\\f"; break;
                    case '\r': s = "\\r"; break;
                    default:
                        if (isgraph(byte))
                            s = (char)byte;
                        break;
                }
            }
            return Wt::WString(s);
        } else if (column == sep1Column || column == sep2Column) {
            return Wt::WString("");
        } else {
            ASSERT_not_reachable("this column needs data");
        }
    } else if (role == Wt::StyleClassRole) {
        if (column==sep1Column || column==sep2Column || !rowAddress(row)) {
            return Wt::WString("hexdump_unmapped");
        } else if (column >= bytesColumn && column < bytesColumn + bytesPerRow && !cellAddress(row, column-bytesColumn)) {
            return Wt::WString("hexdump_unmapped");
        } else if (column >= asciiColumn && column < asciiColumn + bytesPerRow && !cellAddress(row, column-asciiColumn)) {
            return Wt::WString("hexdump_unmapped");
        } else if (row % 2) {
            return Wt::WString("hexdump_oddrow");
        } else {
            return Wt::WString("hexdump_evenrow");
        }
    }
    return boost::any();
}

// size_t
// HexDumpModel::addressToRow(rose_addr_t va) const {
//     ASSERT_forbid(addrRow_.isEmpty());
//     AddrRowMap::ConstNodeIterator found = addrRow_.lowerBound(va);
//     if (found == addrRow_.nodes().end())
//         --found;                                        // va > last segment's starting address
//     if (va < found->key()) {
//         ASSERT_forbid2(found == addrRow_.nodes().begin(), "VA must be greater than or equal to lowest mapped address");
//         --found;
//     }
//     size_t row = found->value();                        // starting row for containing segment
//     rose_addr_t baseVa = alignDown(found->key(), bytesPerRow);// VA for the start of row
//     return row + (va - baseVa) / bytesPerRow;
// }



void
WHexDump::init() {
    Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout;        // so the table scrolls horizontally
    setLayout(hbox);

    model_ = new HexDumpModel(ctx_);

    tableView_ = new Wt::WTableView;
    tableView_->setModel(model_);
    tableView_->setRowHeaderCount(1);                   // this must be first property set
    tableView_->setHeaderHeight(28);
    tableView_->setSortingEnabled(false);
    tableView_->setAlternatingRowColors(false);         // true interferes with our blacking out unmapped addresses
    tableView_->setColumnResizeEnabled(false);
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

    hbox->addWidget(tableView_);
}

} // namespace
