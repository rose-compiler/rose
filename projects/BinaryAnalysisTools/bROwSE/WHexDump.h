#ifndef bROwSE_HexDump_H
#define bROwSE_HexDump_H

#include <bROwSE/bROwSE.h>
#include <sawyer/Map.h>
#include <Wt/WAbstractTableModel>
#include <Wt/WContainerWidget>
#include <Wt/WModelIndex>

namespace bROwSE {

/** Data model for the WHexDump widget. */
class HexDumpModel: public Wt::WAbstractTableModel {
    MemoryMap memoryMap_;
    size_t nRows_;                                      // total number of table rows

    typedef Sawyer::Container::Map<rose_addr_t, size_t> AddrRowMap;
    AddrRowMap addrRow_;                                // partial mapping from virtual address to table row

    // Partial mapping of row to address. We store only those rose that contain the first address of each segment, and the
    // address member is the first address in the segment, which must be aligned downward to get the first address on the table
    // row.
    typedef Sawyer::Container::Map<size_t, rose_addr_t> RowAddrMap;
    RowAddrMap rowAddr_;


public:
    /** Default constructor. Displays an empty memory map. */
    HexDumpModel() { init(); }

    /** Memory map supplying data.
     *
     * @{ */
    const MemoryMap& memoryMap() const { return memoryMap_; }
    void memoryMap(const MemoryMap&);
    /** @} */

    /** Return address for start of table row.
     *
     *  Returns the address that should be displayed in the table's address column, possibly nothing.  A row has no address if
     *  it's a separator between two segments.  This is slightly different than calling @ref cellAddress for the row and
     *  specifying column zero because a row might have data, just not any at column zero. */
    Sawyer::Optional<rose_addr_t> rowAddress(size_t row) const;

    /** Return segment contained in row.
     *
     *  Returns information about the memory segment that the specified row contains. */
    MemoryMap::ConstNodeIterator rowSegment(size_t row) const;

    /** Return the address for a table cell.
     *
     *  The @p pseudoColumn is the address offset from the table's row address and should be zero (inclusive) to the number of
     *  bytes displayed per row (exclusive).  Not all cells have addresses; this method returns nothing for those rows. */
    Sawyer::Optional<rose_addr_t> cellAddress(size_t row, size_t pseudoColumn) const;

    /** Return the byte at the specified cell address.
     *
     *  If no address is specified or the address is not mapped then nothing is returned. */
    Sawyer::Optional<uint8_t> readByte(const Sawyer::Optional<rose_addr_t>&) const;

    /** Returns closest row containing address.
     *
     *  If address is contained in a row then return that row, otherwise return a row that's close. */
    size_t closestRowForAddress(rose_addr_t va) const;

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Widget that shows a hexdump. */
class WHexDump: public Wt::WContainerWidget {
    Wt::WLineEdit *wAddressEdit_;                       // for entering a goto address
    HexDumpModel *model_;
    Wt::WTableView *tableView_;

public:
    /** Default constructor. Displays an empty memory map. */
    explicit WHexDump(Wt::WContainerWidget *parent = NULL)
        : WContainerWidget(parent), wAddressEdit_(NULL), model_(NULL), tableView_(NULL) {
        init();
    }

    /** Show the address closest to the specified address. */
    void makeVisible(rose_addr_t);

    /** Memory map supplying data.
     *
     * @{ */
    const MemoryMap& memoryMap() const { return model_->memoryMap(); }
    void memoryMap(const MemoryMap &m) { model_->memoryMap(m); }
    /** @} */

private:
    void init();

    void handleGoto();
};



} // namespace
#endif
