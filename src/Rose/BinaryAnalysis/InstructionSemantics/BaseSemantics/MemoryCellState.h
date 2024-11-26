#ifndef ROSE_BinaryAnalysis_InstructionSemantics_BaseSemantics_MemoryCellState_H
#define ROSE_BinaryAnalysis_InstructionSemantics_BaseSemantics_MemoryCellState_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCell.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryState.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

/** Cell-based memory state.
 *
 *  This is the base class for cell-based memory states. */
class MemoryCellState: public MemoryState {
public:
    /** Base type. */
    using Super = MemoryState;

    /** Shared-ownership pointer. */
    using Ptr = MemoryCellStatePtr;

protected:
    MemoryCellPtr protocell;                            // prototypical memory cell used for its virtual constructors
    MemoryCellPtr latestWrittenCell_;                   // the cell whose value was most recently written to, if any

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(MemoryState);
        s & BOOST_SERIALIZATION_NVP(protocell);
        s & BOOST_SERIALIZATION_NVP(latestWrittenCell_);
    }
#endif

protected:
    MemoryCellState();                                  // for serialization

    explicit MemoryCellState(const MemoryCellPtr &protocell);

    MemoryCellState(const SValuePtr &addrProtoval, const SValuePtr &valProtoval);

    MemoryCellState(const MemoryCellState &other);

public:
    ~MemoryCellState();

public:
    /** Promote a base address space pointer to a BaseSemantics::MemoryCellState pointer.  The memory state @p m must have a
     *  BaseSemantics::MemoryCellState dynamic type. */
    static MemoryCellStatePtr promote(const BaseSemantics::AddressSpacePtr&);

public:
    virtual void clear() override;

public:
    /** Property: Cell most recently written.
     *
     * @{ */
    virtual MemoryCellPtr latestWrittenCell() const;
    virtual void latestWrittenCell(const MemoryCellPtr&);
    /** @} */

    /** Writers for an address.
     *
     *  Returns the set of all writers that wrote to the specified address or any address that might alias the specified
     *  address. Memory states that don't normally compute aliases (e.g., @ref MemoryCellMap) return only the writers for the
     *  specified address, not any aliases, and in this case @ref getWritersUnion and @ref getWritersIntersection return the
     *  same set. */
    virtual AddressSet getWritersUnion(const SValuePtr &addr, size_t nBits, RiscOperators *addrOps,
                                       RiscOperators *valOps) = 0;

    /** Writers for an address.
     *
     *  Returns the set of all writers that wrote to the specified address and any address that might alias the specified
     *  address. Memory states that don't normally compute aliases (e.g., @ref MemoryCellMap) return only the writers for the
     *  specified address, not any aliases, and in this case @ref getWritersUnion and @ref getWritersIntersection return the
     *  same set. */
    virtual AddressSet getWritersIntersection(const SValuePtr &addr, size_t nBits, RiscOperators *addrOps,
                                              RiscOperators *valOps) = 0;

    /** Find all matching cells.
     *
     *  Returns a vector of cells for which the @p predicate returns true. */
    virtual std::vector<MemoryCellPtr> matchingCells(MemoryCell::Predicate&) const = 0;

    /** Find leading matching cells.
     *
     *  Returns the vector of cells obtained by invoking the predicate on each cell and returning those leading cells for which
     *  the predicate is true.  The first cell for which the predicate is false terminates the traversal and does not appear
     *  in the return value. */
    virtual std::vector<MemoryCellPtr> leadingCells(MemoryCell::Predicate&) const = 0;
    
    /** Remove all matching cells.
     *
     *  Traverses the memory cells and removes those for which the predicate returns true. */
    virtual void eraseMatchingCells(MemoryCell::Predicate&) = 0;

    /** Remove leading matching cells.
     *
     *  Traverses the memory cells and removes those for which the predicate returns true. The traversal is terminated the
     *  first time the predicate returns false. */
    virtual void eraseLeadingCells(MemoryCell::Predicate&) = 0;

    /** Traverse and modify cells.
     *
     *  Traverse cells and optionally modify them. */
    virtual void traverse(MemoryCell::Visitor&) = 0;

    /** Adjust I/O properties after reading memory.
     *
     *  Adjust I/O properties in the specified cells to make it look like they were just read.  This adds the READ property and
     *  may also add READ_AFTER_WRITE, READ_BEFORE_WRITE, and/or READ_UNINITIALIZED. This should be called automatically by
     *  each @ref BaseSemantics::MemoryState::readMemory implementation. */
    virtual void updateReadProperties(const CellList&);

    /** Adjust I/O properties after writing memory.
     *
     *  Adjust I/O properties in the specified cells to make it look like they were just read. This adds the specified
     *  properties to the list of I/O properties for the cell. */
    virtual void updateWriteProperties(const CellList&, InputOutputPropertySet);

    /** Erase cells that have no writers. */
    void eraseNonWritten();

    /** All cells. */
    std::vector<MemoryCellPtr> allCells() const;
};

} // namespace
} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::MemoryCellState);
#endif

#endif
#endif
