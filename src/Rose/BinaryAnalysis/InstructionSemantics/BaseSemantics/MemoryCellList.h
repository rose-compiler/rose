#ifndef ROSE_BinaryAnalysis_InstructionSemantics_BaseSemantics_MemoryCellList_H
#define ROSE_BinaryAnalysis_InstructionSemantics_BaseSemantics_MemoryCellList_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCellState.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.h>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#endif

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

/** Shared-ownership pointer to a list-based memory state. */
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
 *  method can be used by a higher-level readMemory() operation in preference to the usual MemoryState::readMemory().
 *
 *  There is no requirement that a State use a MemoryCellList as its memory state; it can use any subclass of MemoryState.
 *  Since MemoryCellList is derived from MemoryState it must provide virtual allocating constructors, which makes it possible
 *  for users to define their own subclasses and use them in the semantic framework.
 *
 *  This implementation stores memory cells in reverse chronological order: the most recently created cells appear at the
 *  beginning of the list.  Subclasses, of course, are free to reorder the list however they want. */
class MemoryCellList: public MemoryCellState {
public:
    /** Base type. */
    using Super = MemoryCellState;

    /** Shared-ownership pointer. */
    using Ptr = MemoryCellListPtr;

protected:
    CellList cells;                                     // list of cells in reverse chronological order
    bool occlusionsErased_;                             // prune away old cells that are occluded by newer ones.

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serialization
#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(MemoryCellState);
        s & BOOST_SERIALIZATION_NVP(cells);
        s & BOOST_SERIALIZATION_NVP(occlusionsErased_);
    }
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
public:
    ~MemoryCellList();

protected:
    MemoryCellList();                                   // only used for deserialization

    explicit MemoryCellList(const MemoryCellPtr &protocell);

    MemoryCellList(const SValuePtr &addrProtoval, const SValuePtr &valProtoval);

    // deep-copy cell list so that modifying this new state does not modify the existing state
    MemoryCellList(const MemoryCellList &other);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new prototypical memory state. This constructor uses the default type for the cell type (based on the
     *  semantic domain). The prototypical values are usually the same (addresses and stored values are normally the same
     *  type). */
    static MemoryCellListPtr instance(const SValuePtr &addrProtoval, const SValuePtr &valProtoval);

    /** Instantiate a new memory state with prototypical memory cell. */
    static MemoryCellListPtr instance(const MemoryCellPtr &protocell);

    /** Instantiate a new copy of an existing memory state. */
    static MemoryCellListPtr instance(const MemoryCellListPtr &other);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual MemoryStatePtr create(const SValuePtr &addrProtoval, const SValuePtr &valProtoval) const override;

    /** Virtual allocating constructor. */
    virtual MemoryStatePtr create(const MemoryCellPtr &protocell) const;

    virtual AddressSpacePtr clone() const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Promote a base address space pointer to a BaseSemantics::MemoryCellList pointer. The memory state @p m must have a
     *  BaseSemantics::MemoryCellList dynamic type. */
    static MemoryCellListPtr promote(const BaseSemantics::AddressSpacePtr &m);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods we inherited
public:
    virtual void clear() override;
    virtual bool merge(const AddressSpacePtr &other, RiscOperators *addrOps, RiscOperators *valOps) override;
    virtual std::vector<MemoryCellPtr> matchingCells(MemoryCell::Predicate&) const override;
    virtual std::vector<MemoryCellPtr> leadingCells(MemoryCell::Predicate&) const override;
    virtual void eraseMatchingCells(MemoryCell::Predicate&) override;
    virtual void eraseLeadingCells(MemoryCell::Predicate&) override;
    virtual void traverse(MemoryCell::Visitor&) override;
    virtual void hash(Combinatorics::Hasher&, RiscOperators *addrOps, RiscOperators *valOps) const override;

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
                                 RiscOperators *addrOps, RiscOperators *valOps) override;

    virtual SValuePtr peekMemory(const SValuePtr &address, const SValuePtr &dflt,
                                 RiscOperators *addrOps, RiscOperators *valOps) override;

    /** Write a value to memory.
     *
     *  See BaseSemantics::MemoryState() for requirements.  This implementation creates a new memory cell and pushes it onto
     *  the front of the cell list.
     *
     *  The base implementation assumes that all cells contain 8-bit values. */
    virtual void writeMemory(const SValuePtr &addr, const SValuePtr &value,
                             RiscOperators *addrOps, RiscOperators *valOps) override;

    virtual void print(std::ostream&, Formatter&) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods first declared at this level of the class hierarchy
public:
    /** Merge two states without aliasing.
     *
     *  The @p other state is merged into this state without considering any aliasing. Returns true if this state changed,
     *  false otherwise. */
    bool mergeNoAliasing(const AddressSpacePtr &other, RiscOperators *addrOps, RiscOperators *valOps);

    /** Merge two states with aliasing.
     *
     *  The @p other state is merged into this state while considering any aliasing. Returns true if this state changed,
     *  false otherwise. */
    bool mergeWithAliasing(const AddressSpacePtr &other, RiscOperators *addrOps, RiscOperators *valOps);

    /** Predicate to determine whether all bytes are present.
     *
     *  Returns true if bytes at the specified address and the following consecutive addresses are all present in this
     *  memory state. */
    virtual bool isAllPresent(const SValuePtr &address, size_t nBytes, RiscOperators *addrOps, RiscOperators *valOps) const;

    /** Property: erase occluded cells.
     *
     *  If this property is true, then writing a new cell to memory will also erase all older cells that must alias the new
     *  cell.  Erasing occlusions can adversely affect performance for some semantic domains.
     *
     * @{ */
    bool occlusionsErased() const { return occlusionsErased_; }
    void occlusionsErased(bool b) { occlusionsErased_ = b; }
    /** @} */

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
            if (tempCell->mayAlias(*cursor, addrOps)) {
                retval.push_back(*cursor);
                if (tempCell->mustAlias(*cursor, addrOps))
                    break;
            }
        }
        return retval;
    }

    /** Returns the list of all memory cells.
     * @{ */
    virtual const CellList& get_cells() const { return cells; }
    virtual       CellList& get_cells()       { return cells; }
    /** @} */

    virtual AddressSet getWritersUnion(const SValuePtr &addr, size_t nBits, RiscOperators *addrOps,
                                       RiscOperators *valOps) override;

    virtual AddressSet getWritersIntersection(const SValuePtr &addr, size_t nBits, RiscOperators *addrOps,
                                              RiscOperators *valOps) override;

protected:
    // Compute a new value by merging the specified cells.  If the cell list is empty return the specified default.
    virtual SValuePtr mergeCellValues(const CellList &cells, const SValuePtr &dflt, RiscOperators *addrOps,
                                      RiscOperators *valOps);

    // Returns the union of all writers from the specified cells.
    virtual AddressSet mergeCellWriters(const CellList &cells);

    // Returns the union of all properties from the specified cells.
    virtual InputOutputPropertySet mergeCellProperties(const CellList &cells);

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

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::MemoryCellList);
#endif

#endif
#endif
