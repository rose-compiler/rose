#ifndef bROwSE_HexDump_H
#define bROwSE_HexDump_H

#include <bROwSE/bROwSE.h>
#include <sawyer/Map.h>
#include <Wt/WAbstractTableModel>
#include <Wt/WContainerWidget>
#include <Wt/WModelIndex>

namespace bROwSE {

class HexDumpModel: public Wt::WAbstractTableModel {
    Context &ctx_;
    size_t nRows_;                                      // total number of table rows

#if 0 // [Robb P. Matzke 2014-12-23]: not needed yet
    typedef Sawyer::Container::Map<rose_addr_t, size_t> AddrRowMap;
    AddrRowMap addrRow_;                                // partial mapping from virtual address to table row
#endif

    // Partial mapping of row to address. We store only those rose that contain the first address of each segment, and the
    // address member is the first address in the segment, which must be aligned downward to get the first address on the table
    // row.
    typedef Sawyer::Container::Map<size_t, rose_addr_t> RowAddrMap;
    RowAddrMap rowAddr_;


public:
    HexDumpModel(Context &ctx): ctx_(ctx) {
        init();
    }

    /** Return address for start of table row.
     *
     *  Returns the address that should be displayed in the table's address column, possibly nothing.  A row has no address if
     *  it's a separator between two segments.  This is slightly different than calling @ref cellAddress for the row and
     *  specifying column zero because a row might have data, just not any at column zero. */
    Sawyer::Optional<rose_addr_t> rowAddress(size_t row) const;

    /** Return the address for a table cell.
     *
     *  The @p pseudoColumn is the address offset from the table's row address and should be zero (inclusive) to the number of
     *  bytes displayed per row (exclusive).  Not all cells have addresses; this method returns nothing for those rows. */
    Sawyer::Optional<rose_addr_t> cellAddress(size_t row, size_t pseudoColumn) const;

    /** Return the byte at the specified cell address.
     *
     *  If no address is specified or the address is not mapped then nothing is returned. */
    Sawyer::Optional<uint8_t> readByte(const Sawyer::Optional<rose_addr_t>&) const;

#if 0 // [Robb P. Matzke 2014-12-23]: not needed yet
    /** Convert VA to table row index.
     *
     *  Given a virtual address, return the table row that contains that address.  Since a particular row might contain
     *  addresses that aren't actually mapped, the @p va may be an unmapped address. But specifying an unmapped address that
     *  doesn't correspond to any possible row will result in failure. */
    size_t addressToRow(rose_addr_t va) const;
#endif
    
    virtual int rowCount(const Wt::WModelIndex &parent=Wt::WModelIndex()) const ROSE_OVERRIDE;
    virtual int columnCount(const Wt::WModelIndex &parent=Wt::WModelIndex()) const ROSE_OVERRIDE;
    virtual boost::any headerData(int column, Wt::Orientation orientation=Wt::Horizontal,
                                  int role=Wt::DisplayRole) const ROSE_OVERRIDE;
    virtual boost::any data(const Wt::WModelIndex &index, int role=Wt::DisplayRole) const ROSE_OVERRIDE;

private:
    void init();
};

class WHexDump: public Wt::WContainerWidget {
    Context &ctx_;
    HexDumpModel *model_;
    Wt::WTableView *tableView_;

public:
    explicit WHexDump(Context &ctx, Wt::WContainerWidget *parent = NULL)
        : WContainerWidget(parent), ctx_(ctx), model_(NULL), tableView_(NULL) {
        init();
    }

private:
    void init();
};



} // namespace
#endif
