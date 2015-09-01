#ifndef ROSE_BinaryAnalysis_InstructionSemantics2_MemoryCellList_H
#define ROSE_BinaryAnalysis_InstructionSemantics2_MemoryCellList_H

#include <BaseSemantics2.h>

namespace rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      MemoryCell
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Smart pointer to a MemoryCell object.  MemoryCell objects are reference counted and should not be explicitly deleted. */
typedef boost::shared_ptr<class MemoryCell> MemoryCellPtr;

/** Represents one location in memory.
 *
 *  MemoryCell objects are used by the MemoryCellList to represent a memory state. Each memory cell has an address and a
 *  value. A cell also has an optional list of instruction addresses that wrote to that cell, and this list is manipulated by
 *  the @ref RiscOperators separately from updating cell addresses and values and according to settings in the
 *  RiscOperators. Cells written to by RiscOperators typically contain one writer address since each write operation creates a
 *  new cell; however, the result of a dataflow merge operation might produce cells that have multiple writers. */
class MemoryCell: public boost::enable_shared_from_this<MemoryCell> {
public:
    typedef Sawyer::Container::Set<rose_addr_t> AddressSet; /**< A set of concrete virtual addresses. */

private:
    SValuePtr address_;                                 // Address of memory cell.
    SValuePtr value_;                                   // Value stored at that address.
    AddressSet writers_;                                // Instructions that wrote to this cell
    InputOutputPropertySet ioProperties_;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    MemoryCell(const SValuePtr &address, const SValuePtr &value)
        : address_(address), value_(value) {
        ASSERT_not_null(address);
        ASSERT_not_null(value);
    }

    // deep-copy cell list so modifying this new one doesn't alter the existing one
    MemoryCell(const MemoryCell &other) {
        address_ = other.address_->copy();
        value_ = other.value_->copy();
        writers_ = other.writers_;
        ioProperties_ = other.ioProperties_;
    }

public:
    virtual ~MemoryCell() {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiates a new memory cell object with the specified address and value. */
    static MemoryCellPtr instance(const SValuePtr &address, const SValuePtr &value) {
        return MemoryCellPtr(new MemoryCell(address, value));
    }

    /** Instantiates a new copy of an existing cell. */
    static MemoryCellPtr instance(const MemoryCellPtr &other) {
        return MemoryCellPtr(new MemoryCell(*other));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    /** Creates a new memory cell object with the specified address and value. */
    virtual MemoryCellPtr create(const SValuePtr &address, const SValuePtr &value) {
        return instance(address, value);
    }

    /** Creates a new deep-copy of this memory cell. */
    virtual MemoryCellPtr clone() const {
        return MemoryCellPtr(new MemoryCell(*this));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts. No-op since this is the base class.
public:
    static MemoryCellPtr promote(const MemoryCellPtr &x) {
        ASSERT_not_null(x);
        return x;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first declared at this level of the class hierarchy
public:
    /** Accessor for the memory cell address.
     * @{ */
    virtual SValuePtr get_address() const { return address_; }
    virtual void set_address(const SValuePtr &addr) {
        ASSERT_not_null(addr);
        address_ = addr;
    }
    /** @}*/

    /** Accessor for the value stored at a memory location.
     * @{ */
    virtual SValuePtr get_value() const { return value_; }
    virtual void set_value(const SValuePtr &v) {
        ASSERT_not_null(v);
        value_ = v;
    }
    /** @}*/

    /** Get writer information.
     *
     *  Returns all instruction addresses that have written to the specified memory address. */
    virtual AddressSet getWriters() const {
        return writers_;
    }

    /** Insert writer information.
     *
     *  Insert the specified instruction addresses as writers for this memory cell. Returns true if any address was inserted,
     *  false if they were all already members.
     *
     * @{ */
    bool insertWriter(rose_addr_t writerVa) /*final*/ { return writers_.insert(writerVa); }
    virtual bool insertWriters(const AddressSet &writerVas) { return writers_.insert(writerVas); }
    /** @} */

    /** Erase specified writers.
     *
     *  Removes the specified addresses from the set of writers for this memory cell. Returns true if none of the writer
     *  addresses existed, false if any were removed.
     *
     * @{ */
    bool eraseWriter(rose_addr_t writerVa) /*FINAL*/ { return writers_.erase(writerVa); }
    virtual bool eraseWriters(const AddressSet &writerVas) { return writers_.erase(writerVas); }
    /** @} */

    /** Sets writer information.
     *
     *  Changes writer information to be excactly the specified address or set of addresses.
     *
     * @{ */
    void setWriter(rose_addr_t writerVa) ROSE_FINAL;
    virtual void setWriters(const AddressSet &writerVas) { writers_.insert(writerVas); }
    /** @} */

    /** Erase all writers.
     *
     *  Erases all writer information for this memory cell. */
    virtual void eraseWriters() { writers_.clear(); }

    /** Properties: Boolean property set.
     *
     *  This set holds Boolean input/output properties.  If a property is present in the set then it is considered to be true,
     *  otherwise false.
     *
     * @{ */
    const InputOutputPropertySet& ioProperties() const { return ioProperties_; }
    InputOutputPropertySet& ioProperties() { return ioProperties_; }
    /** @} */


    //----------------------------------------------------------------------
    // The following writers API is deprecated. [Robb P. Matzke 2015-08-10]
    //----------------------------------------------------------------------

    virtual boost::optional<rose_addr_t> get_latest_writer() const ROSE_DEPRECATED("use getWriters instead") {
        AddressSet vas = getWriters();
        if (vas.isEmpty())
            return boost::optional<rose_addr_t>();
        return *vas.values().begin();                   // return an arbitrary writer
    }
    virtual void set_latest_writer(rose_addr_t writer_va) ROSE_DEPRECATED("use setWriter instead") {
        setWriter(writer_va);
    }
    virtual void clear_latest_writer() ROSE_DEPRECATED("use clearWriters instead") {
        eraseWriters();
    }
    virtual Sawyer::Optional<rose_addr_t> latestWriter() const ROSE_DEPRECATED("use getWriters instead") {
        AddressSet vas = getWriters();
        if (vas.isEmpty())
            return Sawyer::Nothing();
        return *vas.values().begin();                   // return an arbitrary writer
    }
    virtual void latestWriter(rose_addr_t writerVa) ROSE_DEPRECATED("use setWriter instead") {
        setWriter(writerVa);
    }
    virtual void latestWriter(const Sawyer::Optional<rose_addr_t> w) ROSE_DEPRECATED("use setWriter instead") {
        if (w) {
            setWriter(*w);
        } else {
            eraseWriters();
        }
    }
    virtual void clearLatestWriter() ROSE_DEPRECATED("use eraseWrites instead") {
        eraseWriters();
    }

    /** Determines whether two memory cells can alias one another.  Two cells may alias one another if it is possible that
     *  their addresses cause them to overlap.  For cells containing one-byte values, aliasing may occur if their two addresses
     *  may be equal; multi-byte cells will need to check ranges of addresses. */
    virtual bool may_alias(const MemoryCellPtr &other, RiscOperators *addrOps) const;

    /** Determines whether two memory cells must alias one another.  Two cells must alias one another when it can be proven
     * that their addresses cause them to overlap.  For cells containing one-byte values, aliasing must occur unless their
     * addresses can be different; multi-byte cells will need to check ranges of addresses. */
    virtual bool must_alias(const MemoryCellPtr &other, RiscOperators *addrOps) const;
    
    /** Print the memory cell on a single line.
     * @{ */
    void print(std::ostream &stream) const {
        Formatter fmt;
        print(stream, fmt);
    }
    virtual void print(std::ostream&, Formatter&) const;
    /** @} */

    /** State with formatter. See with_formatter(). */
    class WithFormatter {
        MemoryCellPtr obj;
        Formatter &fmt;
    public:
        WithFormatter(const MemoryCellPtr &obj, Formatter &fmt): obj(obj), fmt(fmt) {}
        void print(std::ostream &stream) const { obj->print(stream, fmt); }
    };

    /** Used for printing states with formatting. The usual way to use this is:
     * @code
     *  MemoryCellPtr obj = ...;
     *  Formatter fmt = ...;
     *  std::cout <<"The value is: " <<(*obj+fmt) <<"\n";
     * @endcode
     * @{ */
    WithFormatter with_format(Formatter &fmt) { return WithFormatter(shared_from_this(), fmt); }
    WithFormatter operator+(Formatter &fmt) { return with_format(fmt); }
    /** @} */
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      MemoryCellList
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Smart pointer to a MemoryCell object. MemoryCell objects are reference counted and should not be explicitly deleted. */
typedef boost::shared_ptr<class MemoryCellList> MemoryCellListPtr;

/** Simple list-based memory state.
 *
 *  MemoryCellList uses a list of MemoryCell objects to represent the memory state. Each memory cell contains at least an
 *  address and a value, both of which have a run-time width.  The default MemoryCellList configuration restricts memory cell
 *  values to be one byte wide and requires the caller to perform any necessary byte extraction or concatenation when higher
 *  software layers are reading/writing multi-byte values.  Using one-byte values simplifies the aliasing calculations.
 *  However, this class defines a @p byte_restricted property that can be set to false to allow the memory to store
 *  variable-width cell values.
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
    bool byte_restricted;                               // are cell values all exactly one byte wide?
    MemoryCellPtr latest_written_cell;                  // the cell whose value was most recently written to, if any
    bool occlusionsErased_;                             // prune away old cells that are occluded by newer ones.

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    explicit MemoryCellList(const MemoryCellPtr &protocell)
        : MemoryState(protocell->get_address(), protocell->get_value()),
          protocell(protocell), byte_restricted(true), occlusionsErased_(false) {
        ASSERT_not_null(protocell);
        ASSERT_not_null(protocell->get_address());
        ASSERT_not_null(protocell->get_value());
    }

    MemoryCellList(const SValuePtr &addrProtoval, const SValuePtr &valProtoval)
        : MemoryState(addrProtoval, valProtoval),
          protocell(MemoryCell::instance(addrProtoval, valProtoval)),
          byte_restricted(true), occlusionsErased_(false) {}

    // deep-copy cell list so that modifying this new state does not modify the existing state
    MemoryCellList(const MemoryCellList &other)
        : MemoryState(other), protocell(other.protocell), byte_restricted(other.byte_restricted),
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
    /** Indicates whether memory cell values are required to be eight bits wide.  The default is true since this simplifies the
     * calculations for whether two memory cells are alias and how to combine the value from two or more aliasing cells. A
     * memory that contains only eight-bit values requires that the caller concatenate/extract individual bytes when
     * reading/writing multi-byte values.
     * @{ */
    virtual bool get_byte_restricted() const { return byte_restricted; }
    virtual void set_byte_restricted(bool b) { byte_restricted = b; }
    /** @} */

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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Free functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream&, const MemoryCell&);
std::ostream& operator<<(std::ostream&, const MemoryCell::WithFormatter&);

} // namespace
} // namespace
} // namespace
} // namespace

#endif
