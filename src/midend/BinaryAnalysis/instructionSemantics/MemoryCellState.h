#ifndef ROSE_BinaryAnalysis_InstructionSemantics2_MemoryCellState_H
#define ROSE_BinaryAnalysis_InstructionSemantics2_MemoryCellState_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <BaseSemantics2.h>
#include <MemoryCell.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

/** Shared-ownership pointer to a cell-based memory state. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<class MemoryCellState> MemoryCellStatePtr;

/** Cell-based memory state.
 *
 *  This is the base class for cell-based memory states. */
class MemoryCellState: public MemoryState {
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
    MemoryCellState() {}                                // for serialization

    explicit MemoryCellState(const MemoryCellPtr &protocell)
        : MemoryState(protocell->get_address(), protocell->get_value()), protocell(protocell) {}

    MemoryCellState(const SValuePtr &addrProtoval, const SValuePtr &valProtoval)
        : MemoryState(addrProtoval, valProtoval), protocell(MemoryCell::instance(addrProtoval, valProtoval)) {}

    MemoryCellState(const MemoryCellState &other)
        : MemoryState(other), protocell(other.protocell) {} // latestWrittenCell_ is cleared

public:
    /** Promote a base memory state pointer to a BaseSemantics::MemoryCellState pointer.  The memory state @p m must have a
     *  BaseSemantics::MemoryCellState dynamic type. */
    static MemoryCellStatePtr promote(const BaseSemantics::MemoryStatePtr &m) {
        MemoryCellStatePtr retval = boost::dynamic_pointer_cast<MemoryCellState>(m);
        ASSERT_not_null(retval);
        return retval;
    }

public:
    virtual void clear() ROSE_OVERRIDE;

public:
    /** Property: Cell most recently written.
     *
     * @{ */
    virtual MemoryCellPtr latestWrittenCell() const {
        return latestWrittenCell_;
    }
    virtual void latestWrittenCell(const MemoryCellPtr &cell) {
        latestWrittenCell_ = cell;
    }
    /** @} */

    /** Writers for an address.
     *
     *  Returns the set of all writers that wrote to the specified address or any address that might alias the specified
     *  address. Memory states that don't normally compute aliases (e.g., @ref MemoryCellMap) return only the writers for the
     *  specified address, not any aliases, and in this case @ref getWritersUnion and @ref getWritersIntersection return the
     *  same set. */
    virtual MemoryCell::AddressSet getWritersUnion(const SValuePtr &addr, size_t nBits, RiscOperators *addrOps,
                                                   RiscOperators *valOps) = 0;

    /** Writers for an address.
     *
     *  Returns the set of all writers that wrote to the specified address and any address that might alias the specified
     *  address. Memory states that don't normally compute aliases (e.g., @ref MemoryCellMap) return only the writers for the
     *  specified address, not any aliases, and in this case @ref getWritersUnion and @ref getWritersIntersection return the
     *  same set. */
    virtual MemoryCell::AddressSet getWritersIntersection(const SValuePtr &addr, size_t nBits, RiscOperators *addrOps,
                                                          RiscOperators *valOps) = 0;

    /** Find all matching cells.
     *
     *  Returns a vector of cells for which the @p predicate returns true. */
    virtual std::vector<MemoryCellPtr> matchingCells(const MemoryCell::Predicate&) const = 0;

    /** Find leading matching cells.
     *
     *  Returns the vector of cells obtained by invoking the predicate on each cell and returning those leading cells for which
     *  the predicate is true.  The first cell for which the predicate is false terminates the traversal and does not appear
     *  in the return value. */
    virtual std::vector<MemoryCellPtr> leadingCells(const MemoryCell::Predicate&) const = 0;
    
    /** Remove all matching cells.
     *
     *  Traverses the memory cells and removes those for which the predicate returns true. */
    virtual void eraseMatchingCells(const MemoryCell::Predicate&) = 0;

    /** Remove leading matching cells.
     *
     *  Traverses the memory cells and removes those for which the predicate returns true. The traversal is terminated the
     *  first time the predicate returns false. */
    virtual void eraseLeadingCells(const MemoryCell::Predicate&) = 0;

    /** Traverse and modify cells.
     *
     *  Traverse cells and optionally modify them. */
    virtual void traverse(MemoryCell::Visitor&) = 0;

    /** Erase cells that have no writers. */
    void eraseNonWritten() {
        eraseMatchingCells(MemoryCell::NonWrittenCells());
    }

    /** All cells. */
    std::vector<MemoryCellPtr> allCells() const {
        return matchingCells(MemoryCell::AllCells());
    }
};

} // namespace
} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::MemoryCellState);
#endif

#endif
#endif
