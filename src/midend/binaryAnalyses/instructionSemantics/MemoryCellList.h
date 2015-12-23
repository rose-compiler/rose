#ifndef ROSE_BinaryAnalysis_InstructionSemantics2_MemoryCellList_H
#define ROSE_BinaryAnalysis_InstructionSemantics2_MemoryCellList_H

#include <BaseSemantics2.h>
#include <MemoryCell.h>

namespace rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

/** Smart pointer to a MemoryCell object. MemoryCell objects are reference counted and should not be explicitly deleted. */
typedef boost::shared_ptr<class MemoryCellList> MemoryCellListPtr;

/** Simple list-based memory state.
 *
 *  MemoryCellList uses a list of MemoryCell objects to represent the memory state. Each memory cell contains at least an
 *  address and a value, both of which have a run-time width.  The default MemoryCellList configuration restricts memory cell
 *  values to be one byte wide and requires the caller to perform any necessary byte extraction or concatenation when higher
 *  software layers are reading/writing multi-byte values.  Using one-byte values simplifies the aliasing calculations.  The
 *  super class defines a @p byteRestricted property that can be set to false to allow the memory to store variable-width cell
 *  values.
 *
 *  MemoryCellList also provides a scan() method that returns a list of memory cells that alias a specified address. This
 *  method can be used by a higher-level readMemory() operation in preference to the usuall MemoryState::readMemory().
 *
 *  There is no requirement that a State use a MemoryCellList as its memory state; it can use any subclass of MemoryState.
 *  Since MemoryCellList is derived from MemoryState it must provide virtual allocating constructors, which makes it possible
 *  for users to define their own subclasses and use them in the semantic framework.
 *
 *  This implementation stores memory cells in reverse chronological order: the most recently created cells appear at the
 *  beginning of the list.  Subclasses, of course, are free to reorder the list however they want. */
class MemoryCellList: public MemoryState {
public:
    typedef std::list<MemoryCellPtr> CellList;          /**< List of memory cells. */
    typedef Sawyer::Container::Set<rose_addr_t> AddressSet; /**< Set of concrete virtual addresses. */
protected:
    MemoryCellPtr protocell;                            // prototypical memory cell used for its virtual constructors
    CellList cells;                                     // list of cells in reverse chronological order
    MemoryCellPtr latest_written_cell;                  // the cell whose value was most recently written to, if any
    bool occlusionsErased_;                             // prune away old cells that are occluded by newer ones.

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit MemoryCellList(const MemoryCellPtr &protocell)
        : MemoryState(protocell->get_address(), protocell->get_value()),
          protocell(protocell), occlusionsErased_(false) {
        ASSERT_not_null(protocell);
        ASSERT_not_null(protocell->get_address());
        ASSERT_not_null(protocell->get_value());
    }

    MemoryCellList(const SValuePtr &addrProtoval, const SValuePtr &valProtoval)
        : MemoryState(addrProtoval, valProtoval),
          protocell(MemoryCell::instance(addrProtoval, valProtoval)),
          occlusionsErased_(false) {}

    // deep-copy cell list so that modifying this new state does not modify the existing state
    MemoryCellList(const MemoryCellList &other)
        : MemoryState(other), protocell(other.protocell),
          occlusionsErased_(other.occlusionsErased_) {
        for (CellList::const_iterator ci=other.cells.begin(); ci!=other.cells.end(); ++ci)
            cells.push_back((*ci)->clone());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new prototypical memory state. This constructor uses the default type for the cell type (based on the
     *  semantic domain). The prototypical values are usually the same (addresses and stored values are normally the same
     *  type). */
    static MemoryCellListPtr instance(const SValuePtr &addrProtoval, const SValuePtr &valProtoval) {
        return MemoryCellListPtr(new MemoryCellList(addrProtoval, valProtoval));
    }
    
    /** Instantiate a new memory state with prototypical memory cell. */
    static MemoryCellListPtr instance(const MemoryCellPtr &protocell) {
        return MemoryCellListPtr(new MemoryCellList(protocell));
    }

    /** Instantiate a new copy of an existing memory state. */
    static MemoryCellListPtr instance(const MemoryCellListPtr &other) {
        return MemoryCellListPtr(new MemoryCellList(*other));
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual MemoryStatePtr create(const SValuePtr &addrProtoval, const SValuePtr &valProtoval) const ROSE_OVERRIDE {
        return instance(addrProtoval, valProtoval);
    }
    
    /** Virtual allocating constructor. */
    virtual MemoryStatePtr create(const MemoryCellPtr &protocell) const {
        return instance(protocell);
    }

    virtual MemoryStatePtr clone() const ROSE_OVERRIDE {
        return MemoryStatePtr(new MemoryCellList(*this));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Promote a base memory state pointer to a BaseSemantics::MemoryCellList pointer. The memory state @p m must have
     *  a BaseSemantics::MemoryCellList dynamic type. */
    static MemoryCellListPtr promote(const BaseSemantics::MemoryStatePtr &m) {
        MemoryCellListPtr retval = boost::dynamic_pointer_cast<MemoryCellList>(m);
        ASSERT_not_null(retval);
        return retval;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods we inherited
public:
    virtual void clear() ROSE_OVERRIDE {
        cells.clear();
        latest_written_cell.reset();
    }

    virtual bool merge(const MemoryStatePtr &other, RiscOperators *addrOps, RiscOperators *valOps) ROSE_OVERRIDE;

    /** Read a value from memory.
     *
     *  See BaseSemantics::MemoryState() for requirements.  This implementation scans the reverse chronological cell list until
     *  it finds a cell that must alias the specified addresses and value size. Along the way, it accumulates a list of cells
     *  that may alias the specified address.  If the accumulated list does not contain exactly one cell, or the scan fell off
     *  the end of the list, then @p dflt becomes the return value, otherwise the return value is the single value on the
     *  accumulated list. If the @p dflt value is returned, then it is also pushed onto the front of the cell list.
     *
     *  The width of the @p dflt value determines how much data is read. The base implementation assumes that all cells contain
     *  8-bit values. */
    virtual SValuePtr readMemory(const SValuePtr &address, const SValuePtr &dflt,
                                 RiscOperators *addrOps, RiscOperators *valOps) ROSE_OVERRIDE;

    /** Write a value to memory.
     *
     *  See BaseSemantics::MemoryState() for requirements.  This implementation creates a new memory cell and pushes it onto
     *  the front of the cell list.
     *
     *  The base implementation assumes that all cells contain 8-bit values. */
    virtual void writeMemory(const SValuePtr &addr, const SValuePtr &value,
                             RiscOperators *addrOps, RiscOperators *valOps) ROSE_OVERRIDE;

    virtual void print(std::ostream&, Formatter&) const ROSE_OVERRIDE;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first declared at this level of the class hierarchy
public:
    /** Property: erase occluded cells.
     *
     *  If this property is true, then writing a new cell to memory will also erase all older cells that must alias the new
     *  cell.  Erasing occlusions can adversely affect performance for some semantic domains.
     *
     * @{ */
    bool occlusionsErased() const { return occlusionsErased_; }
    void occlusionsErased(bool b) { occlusionsErased_ = b; }
    /** @} */

    /** Remove memory cells that were read but never written.
     *
     *  The determination of whether a cell was read but never written is based on whether the cell has a latest writer. */
    virtual void clearNonWritten();

    /** Scan cell list to find matching cells.
     *
     *  Scans the cell list from front to back (reverse chronological order) and build a list of cells that may alias the given
     *  address and size.  The returned list is also in reverse chronological order.  The scanning begins at the specified
     *  cursor and ends either when an exact alias is found or the end of the list is reached. In either case, the cell that
     *  caused the scan to terminate is returned--either the exact alias or the end iterator.
     *
     *  Typical usage is like this:
     *
     * @code
     *  MemoryCellList::CellList::iterator cursor = memory->get_cells().begin();
     *  MemoryCellList::CellList cells = memory->scan(cursor, address, 8, addrOps, valOps);
     *  if (cursor != memory->get_cells().end())
     *      std::cerr <<"found exact alias: " <<*cursor <<"\n";
     * @endcode */
    template<class Iterator>
    CellList scan(Iterator &cursor /*in,out*/, const SValuePtr &addr, size_t nBits,
                  RiscOperators *addrOps, RiscOperators *valOps) const {
        ASSERT_not_null(addr);
        CellList retval;
        MemoryCellPtr tempCell = protocell->create(addr, valOps->undefined_(nBits));
        for (/*void*/; cursor!=cells.end(); ++cursor) {
            if (tempCell->may_alias(*cursor, addrOps)) {
                retval.push_back(*cursor);
                if (tempCell->must_alias(*cursor, addrOps))
                    break;
            }
        }
        return retval;
    }

    // [Robb P. Matzke 2015-08-18]: deprecated
    virtual CellList scan(const SValuePtr &address, size_t nbits, RiscOperators *addrOps, RiscOperators *valOps,
                          bool &short_circuited/*out*/) const ROSE_DEPRECATED("use the cursor-based scan instead");

    /** Visitor for traversing a cell list. */
    class Visitor {
    public:
        virtual ~Visitor() {}
        virtual void operator()(MemoryCellPtr&) = 0;
    };

    /** Visit each memory cell. */
    void traverse(Visitor &visitor);

    /** Returns the list of all memory cells.
     * @{ */
    virtual const CellList& get_cells() const { return cells; }
    virtual       CellList& get_cells()       { return cells; }
    /** @} */

    /** Returns the cell most recently written. */
    virtual MemoryCellPtr get_latest_written_cell() const { return latest_written_cell; }

    /** Writers for an address.
     *
     *  Returns the set of all writers that wrote to the specified address or any address that might alias the specified
     *  address. */
    virtual AddressSet getWritersUnion(const SValuePtr &addr, size_t nBits, RiscOperators *addrOps, RiscOperators *valOps);

    /** Writers for an address.
     *
     *  Returns the set of all writers that wrote to the specified address and any address that might alias the specified
     *  address. */
    virtual AddressSet getWritersIntersection(const SValuePtr &addr, size_t nBits, RiscOperators *addrOps,
                                              RiscOperators *valOps);

    // [Robb P. Matzke 2015-08-17]: deprecated
    virtual std::set<rose_addr_t> get_latest_writers(const SValuePtr &addr, size_t nbits,
                                                     RiscOperators *addrOps, RiscOperators *valOps)
        ROSE_DEPRECATED("use getWritersUnion instead");

protected:
    // Compute a new value by merging the specified cells.  If the cell list is empty return the specified default.
    virtual SValuePtr mergeCellValues(const CellList &cells, const SValuePtr &dflt, RiscOperators *addrOps,
                                      RiscOperators *valOps);

    // Returns the union of all writers from the specified cells.
    virtual AddressSet mergeCellWriters(const CellList &cells);

    // Returns the union of all properties from the specified cells.
    virtual InputOutputPropertySet mergeCellProperties(const CellList &cells);

    // Adjust I/O properties in the specified cells to make it look like they were just read.  This adds the READ property and
    // may also add READ_AFTER_WRITE, READ_BEFORE_WRITE, and/or READ_UNINITIALIZED.
    virtual void updateReadProperties(CellList &cells);

    // Insert a new cell at the head of the list. It's writers set is empty and its I/O properties will be READ,
    // READ_BEFORE_WRITE, and READ_UNINITIALIZED.
    virtual MemoryCellPtr insertReadCell(const SValuePtr &addr, const SValuePtr &value);

    // Insert a new cell at the head of the list.  The specified writers and I/O properties are used.
    virtual MemoryCellPtr insertReadCell(const SValuePtr &addr, const SValuePtr &value,
                                         const AddressSet &writers, const InputOutputPropertySet &props);
};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
